/*
 * Copyright 2022 Alexandru-Sergiu Marton
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

GLFWwindow* Window;
GLuint ProgramId, VaoId, VboId, ColorBufferId;
GLfloat XMin = 0, XMax = 100, YMin = 0, YMax = 100;
GLint TransformLoc;
glm::mat4 ResizeMat;

const char* VertexShaderSource = R"SHADER-SOURCE(
#version 330 core

layout (location = 0) in vec2 in_Position;
layout (location = 1) in vec4 in_Color;

uniform mat4 transform;

out vec4 ex_Color;

void main() {
    gl_Position = transform * vec4(in_Position, 0.0, 1.0);
    ex_Color = in_Color;
}
)SHADER-SOURCE";

const char* FragmentShaderSource = R"SHADER-SOURCE(
#version 330 core

in vec4 ex_Color;
out vec4 out_Color;

void main() {
    out_Color = ex_Color;
}
)SHADER-SOURCE";

void Initialize();
void Render();
void Cleanup();

GLuint LoadShaders(const char* vertexSource, const char* fragSource);

int main() {
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window = glfwCreateWindow(800, 800, "Depășire", nullptr, nullptr);
    if (!Window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(Window);
    glewInit();
    glfwSwapInterval(1);

    Initialize();

    while (!glfwWindowShouldClose(Window)) {
        int width, height;
        glfwGetFramebufferSize(Window, &width, &height);

        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Render();

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    Cleanup();

    glfwDestroyWindow(Window);
    glfwTerminate();
    return 0;
}

void Initialize() {
    GLfloat vertices[] = {
        // Car:
        // - body
        //   - bottom-left
        10, 10,
        10, 20,
        35, 10,
        //   - top-right
        10, 20,
        35, 20,
        35, 10,
        // - roof
        //   - back
        11.5, 20,
        13, 25,
        25.5, 25,
        //   - front
        11.5, 20,
        25.5, 25,
        30, 20,
        // Wheel
        12.5, 11,
        14.5, 13,
        16.5, 13,

        12.5, 11,
        16.5, 13,
        18.5, 11,

        12.5, 11,
        18.5, 11,
        18.5, 9,

        12.5, 11,
        18.5, 9,
        16.5, 7,

        12.5, 11,
        16.5, 7,
        14.5, 7,

        12.5, 11,
        14.5, 7,
        12.5, 9,
        // Road:
        // bottom-left
        XMin, 0,
        XMin, 50,
        XMax, 0,
        // top-right
        XMin, 50,
        XMax, 50,
        XMax, 0,
        // Road marking
        XMin, 24,
        XMin, 26,
        XMin + 10, 24,
        XMin, 26,
        XMin + 10, 26,
        XMin + 10, 24,
        // Grass
        XMin, 50,
        XMin, YMax,
        XMax, 50,
        XMin, YMax,
        XMax, YMax,
        XMax, 50,
        // Windows
        //   - back
        12.5, 20,
        13.5, 24,
        19, 24,
        19, 24,
        19, 20,
        12.5, 20,
        //   - front
        20, 20,
        20, 24,
        24.75, 24,
        20, 20,
        24.75, 24,
        28.5, 20,
    };

    GLfloat colors[] = {
        // Car
        1.0, 0.5, 0.0, 1.0,
        1.0, 0.5, 0.0, 1.0,
        1.0, 0.5, 0.0, 1.0,
        1.0, 0.5, 0.0, 1.0,
        1.0, 0.5, 0.0, 1.0,
        1.0, 0.5, 0.0, 1.0,
        // Car roof
        1.0, 0.5, 0.0, 1.0,
        1.0, 0.5, 0.0, 1.0,
        1.0, 0.5, 0.0, 1.0,
        1.0, 0.5, 0.0, 1.0,
        1.0, 0.5, 0.0, 1.0,
        1.0, 0.5, 0.0, 1.0,
        // Wheel
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 1.0,
        // Road
        0.2, 0.2, 0.2, 1.0,
        0.2, 0.2, 0.2, 1.0,
        0.2, 0.2, 0.2, 1.0,
        0.2, 0.2, 0.2, 1.0,
        0.2, 0.2, 0.2, 1.0,
        0.2, 0.2, 0.2, 1.0,
        // Road marking
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        // Grass
        0.37, 0.57, 0.15, 1.0,
        0.37, 0.57, 0.15, 1.0,
        0.37, 0.57, 0.15, 1.0,
        0.37, 0.57, 0.15, 1.0,
        0.37, 0.57, 0.15, 1.0,
        0.37, 0.57, 0.15, 1.0,
        // Windows
        0.14, 0.58, 0.75, 1.0,
        0.14, 0.58, 0.75, 1.0,
        0.14, 0.58, 0.75, 1.0,
        0.14, 0.58, 0.75, 1.0,
        0.14, 0.58, 0.75, 1.0,
        0.14, 0.58, 0.75, 1.0,
        0.14, 0.58, 0.75, 1.0,
        0.14, 0.58, 0.75, 1.0,
        0.14, 0.58, 0.75, 1.0,
        0.14, 0.58, 0.75, 1.0,
        0.14, 0.58, 0.75, 1.0,
        0.14, 0.58, 0.75, 1.0,
    };

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &ColorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    ProgramId = LoadShaders(VertexShaderSource, FragmentShaderSource);

    TransformLoc = glGetUniformLocation(ProgramId, "transform");

    ResizeMat = glm::ortho(XMin, XMax, YMin, YMax);
}

const float Period = 4;
float VertMovWave(float t) {
    double r = t - glm::floor(t / Period) * Period;
    return 1 <= r && r < 2
        ? glm::sin(glm::pi<float>() * (r - 1))
        : 0;
}

float HorizMovWave(float t) {
    double r = t - glm::floor(t / Period) * Period;
    if (0 <= r && r < 1) {
        return 0;
    }
    if (1 <= r && r < 2) {
        return glm::sin(glm::pi<float>() / 2 * (r - 1));
    }
    if (2 <= r && r < 3) {
        return 1;
    }
    return glm::cos(glm::pi<float>() / 2 * (r - 3));
}

float WiggleWave(float t) {
    double r = t - glm::floor(t / Period) * Period;
    if (0 <= r && r < 1) {
        return glm::sin(5 * r * glm::pi<float>() + glm::pi<float>());
    } else if (2 <= r && r < 3) {
        return glm::sin(5 * (r - 2) * glm::pi<float>());
    }

    return 0;
}

float RotateWave(float t) {
    double r = t - glm::floor(t / Period) * Period;
    if (1 <= r && r < 1.5) {
        return glm::sin(glm::pi<float>() * (r - 1) * 2);
    } else if (1.5 <= r && r < 2) {
        return -glm::sin(glm::pi<float>() * (r - 1.5) * 2);
    }
    return 0;
}

auto id = glm::identity<glm::mat4>();
glm::vec3 upMovVec = glm::vec3(0.0, 30.0, 0.0);
glm::vec3 horizMovVec = glm::vec3(XMax - 40, 0.0, 0.0);
glm::mat4 wheelStride = glm::translate(id, glm::vec3(13, 0.0, 0.0));
glm::vec3 wheelToOrigin(15.5, 10.0, 0.0);
glm::vec3 carToOrigin(12.5, 15, 0.0);

void Render() {
    const auto elapsedTime = glfwGetTime();
    glm::mat4 transform;

    glm::mat4 translateUp1 = glm::translate(id, upMovVec * VertMovWave(elapsedTime));
    glm::mat4 translateUp2 = glm::translate(id, upMovVec * VertMovWave(elapsedTime + Period / 2));

    glm::mat4 translateHorizontal1 = glm::translate(id, horizMovVec * HorizMovWave(elapsedTime));
    glm::mat4 translateHorizontal2 = glm::translate(id, horizMovVec * HorizMovWave(elapsedTime + Period / 2));

    glm::mat4 scale1 = glm::scale(id, glm::vec3(1.0) - glm::vec3(0.15, 0.15, 0.0) * VertMovWave(elapsedTime));
    glm::mat4 scale2 = glm::scale(id, glm::vec3(1.0) - glm::vec3(0.15, 0.15, 0.0) * VertMovWave(elapsedTime + Period / 2));

    glm::mat4 wiggle1 = glm::translate(id, glm::vec3(WiggleWave(elapsedTime), 0.0, 0.0));
    glm::mat4 wiggle2 = glm::translate(id, glm::vec3(WiggleWave(elapsedTime + Period / 2), 0.0, 0.0));

    glm::mat4 rotateMat1 = glm::rotate(id, glm::pi<float>() / 9 * RotateWave(elapsedTime), glm::vec3(0, 0, 1));
    glm::mat4 rotateMat2 = glm::rotate(id, glm::pi<float>() / 9 * RotateWave(elapsedTime + Period / 2), glm::vec3(0, 0, 1));

    // Grass
    transform = ResizeMat;
    glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 42, 6);

    // Road
    transform = ResizeMat;
    glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 30, 6);

    // Road marking
    float markingTraverseTime = 3.0f;
    GLsizei markingsCount = 6;
    for (GLsizei i = 0; i < markingsCount; i++) {
        float offset = (elapsedTime + (markingTraverseTime / markingsCount) * i) / markingTraverseTime;
        float xPos = XMax - (offset - glm::floor(offset)) * (XMax + 20);
        glm::mat4 translate = glm::translate(id, glm::vec3(xPos, 0.0, 0.0));
        transform = ResizeMat * translate;
        glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, &transform[0][0]);
        glDrawArrays(GL_TRIANGLES, 36, 6);
    }

    // Car 1
    glm::mat4 carToOriginMat = glm::translate(id, -carToOrigin);
    glm::mat4 carBackMat = glm::translate(id, carToOrigin);
    transform = ResizeMat * scale1 * translateUp1 * translateHorizontal1 * wiggle1 * carBackMat * rotateMat1 * carToOriginMat;
    glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 12);

    // Wheels
    glm::mat4 centerWheel = glm::translate(id, -wheelToOrigin);
    glm::mat4 moveBackWheel = glm::translate(id, wheelToOrigin);
    float rotationPeriod = 1.0f;
    glm::mat4 rotateWheel = glm::rotate(id, (float) -(glm::pi<float>() * 2 * (elapsedTime / rotationPeriod - glm::floor(elapsedTime / rotationPeriod))), glm::vec3(0.0, 0.0, 1.0));

    transform = ResizeMat * scale1 * translateUp1 * translateHorizontal1 * wiggle1 * carBackMat * rotateMat1 * carToOriginMat * moveBackWheel * rotateWheel * centerWheel;
        glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 12, 18);

    transform = ResizeMat * scale1 * translateUp1 * translateHorizontal1 * wiggle1 * carBackMat * rotateMat1 * carToOriginMat * wheelStride * moveBackWheel * rotateWheel * centerWheel;
    glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 12, 18);

    // Windows
    transform = ResizeMat * scale1 * translateUp1 * translateHorizontal1 * wiggle1 * carBackMat * rotateMat1 * carToOriginMat;
    glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 48, 12);

    // Car 2
    transform = ResizeMat * scale2 * translateUp2 * translateHorizontal2 * wiggle2 * carBackMat * rotateMat2 * carToOriginMat;
    glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 12);

    // Wheels
    transform = ResizeMat * scale2 * translateUp2 * translateHorizontal2 * wiggle2 * carBackMat * rotateMat2 * carToOriginMat * moveBackWheel * rotateWheel * centerWheel;
    glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 12, 18);

    transform = ResizeMat * scale2 * translateUp2 * translateHorizontal2 * wiggle2 * carBackMat * rotateMat2 * carToOriginMat * wheelStride * moveBackWheel * rotateWheel * centerWheel;
    glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 12, 18);

    // Windows
    transform = ResizeMat * scale2 * translateUp2 * translateHorizontal2 * wiggle2 * carBackMat * rotateMat2 * carToOriginMat;
    glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 48, 12);

    glFlush();
}

void Cleanup() {
    glDeleteProgram(ProgramId);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &VboId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

GLuint LoadShaders(const char* vertexSource, const char* fragSource) {
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &vertexSource, nullptr);
    glCompileShader(vertexShaderId);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShaderId, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &fragSource, nullptr);
    glCompileShader(fragmentShaderId);

    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShaderId, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);
    glUseProgram(programId);

    return programId;
}
