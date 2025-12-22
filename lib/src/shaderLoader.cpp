#include <GL/glew.h>
#include <shaderLoader.hpp>
#include <iostream>
#include <fstream>

uint32_t compileShader(uint32_t type, const char* shaderSource) {
    uint32_t id = glCreateShader(type);
    glShaderSource(id, 1, &shaderSource, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile " << 
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

uint32_t createShader(const std::filesystem::path vertexFilename, const std::filesystem::path fragmentFilename) {
    const std::filesystem::path vertexPath = "shaders" / vertexFilename;
    const std::filesystem::path fragmentPath = "shaders" / fragmentFilename;
    std::ifstream vertexFile(vertexPath);
    if (!vertexFile) {
        std::cerr << "Error opening " << vertexPath << std::endl;
        return 0;
    }
    std::uintmax_t size = std::filesystem::file_size(vertexPath);
    char* buffer = (char*)malloc(size+1);
    vertexFile.read(buffer, size);
    vertexFile.close();
    buffer[size] = '\0';

    const uint32_t vertexShader = compileShader(GL_VERTEX_SHADER, buffer);
    free(buffer);

    std::ifstream fragmentFile(fragmentPath);
    if (!fragmentFile) {
        std::cerr << "Error opening " << fragmentPath << std::endl;
        return 0;
    }
    size = std::filesystem::file_size(fragmentPath);
    buffer = (char*)malloc(size+1);
    fragmentFile.read(buffer, size);
    fragmentFile.close();
    buffer[size] = '\0';

    const uint32_t fragmentShader = compileShader(GL_FRAGMENT_SHADER, buffer);
    free(buffer);

    const uint32_t shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);
    glValidateProgram(shader);

    int success;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        int length;
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length);
        glGetProgramInfoLog(shader, length, &length, message);
        std::cerr << "Shader program linking failed: " << message << std::endl;
        glDeleteProgram(shader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::cout << "Successfully built shader with id " << shader << std::endl;

    return shader;
}
