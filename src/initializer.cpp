#include <GL/glew.h>
#include <initializer.hpp>
#include <constants.hpp>
#include <iostream>

uint32_t windowWidth, windowHeight;

void setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

void initGLFW(int majorVersion, int minorVersion) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* createWindow(bool fullscreen) {
    GLFWwindow* window;
    if (fullscreen) {
        GLFWmonitor* mon = glfwGetPrimaryMonitor();
        const GLFWvidmode* vidMode = glfwGetVideoMode(mon);
        window = glfwCreateWindow(vidMode->width, vidMode->height, "OpenGL", mon, NULL);
        windowWidth = vidMode->width;
        windowHeight = vidMode->height;
    }
    else {
        window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", NULL, NULL);
    }
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    //Create an OpenGL contex for the window
    glfwSwapInterval(1); //Limit framerate
    glfwMakeContextCurrent(window);
    return window;
}

void initOpenGL(GLFWwindow* window) {
    glewExperimental = GL_TRUE;
    int result = glewInit();
    if (result != GLEW_OK) {
        std::cerr << "Error initializing GLEW" << std::endl;
        exit(1);
    }
}
