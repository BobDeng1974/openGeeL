#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;

uniform float FXAA_CLAMP;
uniform float FXAA_REDUCE_MUL;
uniform float FXAA_MIN;
uniform float DIFF_THRESHOLD;

const vec3 luminance = vec3(0.299f, 0.587f, 0.114f);

void main() {
	vec2 texOffset = 1.f / textureSize(image, 0); 

	float lumaTL = dot(luminance, texture2D(image, TexCoords + (vec2(-1.f, -1.f) * texOffset)).rgb);
	float lumaTR = dot(luminance, texture2D(image, TexCoords + (vec2(1.f, -1.f) * texOffset)).rgb);
	float lumaBL = dot(luminance, texture2D(image, TexCoords + (vec2(-1.f, 1.f) * texOffset)).rgb);
	float lumaBR = dot(luminance, texture2D(image, TexCoords + (vec2(1.f, 1.f) * texOffset)).rgb);
	float lumaMD = dot(luminance, texture2D(image, TexCoords).rgb);

	float lumaMin = min(lumaTL, min(lumaTR, min(lumaBL, min(lumaBR, lumaMD))));
	float lumaMax = max(lumaTL, max(lumaTR, max(lumaBL, max(lumaBR, lumaMD))));

	vec2 blurDirection = vec2(
		-((lumaTL + lumaTR) - (lumaBL + lumaBR)), 
		(lumaTL + lumaBL) - (lumaTR + lumaBR));

	//Only blur if threshold for blur direction is reached
	if(length(blurDirection) < DIFF_THRESHOLD) {
		color = vec4(texture2D(image, TexCoords).rgb, 1.0f);
	}
	else {
		//Scale min value with mean luminance
		float minValue = max((lumaTL + lumaTR + lumaBL + lumaBR) * (0.25f * FXAA_REDUCE_MUL), FXAA_MIN); 
		float normalize = 1.f / min(abs(blurDirection.x + minValue), abs(blurDirection.y) + minValue);

		blurDirection = blurDirection * normalize;
		blurDirection = clamp(blurDirection, vec2(-FXAA_CLAMP), vec2(FXAA_CLAMP));
		blurDirection = blurDirection * texOffset;


		vec3 result1 = 0.5f * 
			(texture2D(image, TexCoords + (vec2(-0.15f) * blurDirection)).rgb + 
			texture2D(image, TexCoords + (vec2(0.15f) * blurDirection)).rgb);

		vec3 result2 = result1 * 0.5f + 0.25f * 
			(texture2D(image, TexCoords + (vec2(-0.6f) * blurDirection)).rgb + 
			texture2D(image, TexCoords + (vec2(0.6f) * blurDirection)).rgb);

		float lumaResult = dot(luminance, result2);

		if(lumaResult > lumaMin && lumaResult < lumaMax)
			color = vec4(result2, 1.0f);
		else
			color = vec4(result1, 1.0f);	
	}
}
