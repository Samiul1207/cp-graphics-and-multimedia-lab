#include "glad.h"
#include "glfw3.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Window settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 700;

const int RAIN_COUNT = 500;

// Vertex Shader
const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec2 aPos;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
    gl_PointSize = 4.0;
}
)";

// Fragment Shader
const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
)";

// Rain Drop Structure
struct RainDrop
{
    float x;
    float y;
    float speed;
};

std::vector<RainDrop> rain(RAIN_COUNT);

int main()
{
    srand((unsigned int)time(0));

    // GLFW INIT
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "Matrix Rain Effect",
        NULL,
        NULL
    );

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // BUILD SHADERS
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    glCompileShader(fragmentShader);

    // SHADER PROGRAM
    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);

    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);

    glDeleteShader(fragmentShader);

    // INITIALIZE RAIN
    for (int i = 0; i < RAIN_COUNT; i++)
    {
        rain[i].x = ((rand() % 2000) / 1000.0f) - 1.0f;

        rain[i].y = ((rand() % 2000) / 1000.0f) - 1.0f;

        rain[i].speed =
            0.002f +
            ((rand() % 100) / 50000.0f);
    }

    // VAO + VBO
    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        RAIN_COUNT * 2 * sizeof(float),
        NULL,
        GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // RENDER LOOP
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // UPDATE RAIN
        for (int i = 0; i < RAIN_COUNT; i++)
        {
            rain[i].y -= rain[i].speed;

            if (rain[i].y < -1.2f)
            {
                rain[i].y = 1.2f;

                rain[i].x =
                    ((rand() % 2000) / 1000.0f) - 1.0f;
            }
        }

        // CREATE POSITION ARRAY
        std::vector<float> vertices;

        for (int i = 0; i < RAIN_COUNT; i++)
        {
            vertices.push_back(rain[i].x);
            vertices.push_back(rain[i].y);
        }

        // SEND TO GPU
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            vertices.size() * sizeof(float),
            vertices.data()
        );

        // BLACK BACKGROUND
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        // DRAW
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);

        glDrawArrays(GL_POINTS, 0, RAIN_COUNT);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // CLEANUP
    glDeleteVertexArrays(1, &VAO);

    glDeleteBuffers(1, &VBO);

    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}

// INPUT
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

// RESIZE
void framebuffer_size_callback(
    GLFWwindow* window,
    int width,
    int height
)
{
    glViewport(0, 0, width, height);
}