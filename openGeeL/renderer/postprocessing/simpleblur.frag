#version 330 core

in vec2 TexCoords;

out float fragColor;

uniform sampler2D image;
uniform int amount;

void main() {
    vec2 texelSize = 1.f / vec2(textureSize(image, 0));

    float result = 0.f;
    for (int x = -amount; x < amount; x++) {
        for (int y = -amount; y < amount; y++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(image, TexCoords + offset).r;
        }
    }

    fragColor = result / (amount * amount * amount * amount);
} 
