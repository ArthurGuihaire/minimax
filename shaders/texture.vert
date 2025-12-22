#version 330 core

layout(location=0) in vec2 position;
layout(location=1) in vec2 texPosition;
layout(location=2) in vec2 offset;

out vec2 texCoords;

int main() {
    gl_Position = position + offset;
    texCoords = texPosition;
}