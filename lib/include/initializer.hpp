#pragma once
#include <GLFW/glfw3.h>
#include <filesystem>

void setWindowSize(int width, int height);
void initGLFW(int majorVersion, int minorVersion);
void initGLAD();
GLFWwindow* createWindow(bool fullscreen);
uint32_t loadTexture(std::filesystem::path texturePath);
