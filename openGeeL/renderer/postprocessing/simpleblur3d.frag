#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform int amount;

void main() {
    vec2 texelSize = 1.f / vec2(textureSize(image, 0));

    vec3 result = vec3(0.f);
    for (int x = -amount; x < amount; x++) {
        for (int y = -amount; y < amount; y++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(image, TexCoords + offset).xyz;
        }
    }

	result = result / (amount * amount * amount * amount);

    color = vec4(result, 1.f); 
} 
