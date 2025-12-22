#version 330 core

in vec2 texCoords;
uniform sampler2D inputTexture;
out vec4 fragColor;

int main() {
    fragColor = texture(inputTexture, texCoords);
}