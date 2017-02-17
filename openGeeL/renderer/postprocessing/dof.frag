#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D blurredImage;

uniform float focalDistance;
uniform float aperture;
uniform float farDistance;

void main() {
	vec3 focused = texture(image, TexCoords).rgb; 
	vec3 blurred = texture(blurredImage, TexCoords).rgb; 
	float depth  = texture(gPositionDepth, TexCoords).w;

	float diff = abs(focalDistance - depth);
	diff = (diff / farDistance) * aperture;
	diff = min(diff, 1.f);

	vec3 result = (1.f - diff) * focused + diff * blurred;
	color = vec4(result, 1.0f);
}

