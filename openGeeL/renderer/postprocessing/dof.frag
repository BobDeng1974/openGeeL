#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D blurredImage;

uniform float focalDistance;
uniform float aperture;

void main() {
	vec3 focused = texture(image, TexCoords).rgb; 
	vec3 blurred = texture(blurredImage, TexCoords).rgb; 
	float depth  = texture(gPositionDepth, TexCoords).w;

	//Kinda ugly: Filter out skybox depths. Can create
	//artifacts for other objects with same depth 
	if(depth == 1.f)
		depth = 100.f;

	float focal = 1.f / focalDistance;
	float diff = (focal > depth) ? focal - depth : depth - focal;
	diff = diff * 0.1f * aperture;
	diff = min(diff, 1.f);

	vec3 result = (1.f - diff) * focused + diff * blurred;
	color = vec4(result, 1.0f);
}

