#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform float scale;

const vec3  luminance = vec3(0.299f, 0.587f, 0.114f);

const float kernelX[3][3] = {{-1, 0, 1}, 
							{-2, 0, 2}, 
						    {-1, 0, 1}};

const float kernelY[3][3] = {{-1, -2, -1}, 
							{0,  0,  0}, 
							{1,  2,  1}};


//Get luminance at pixel (defined by offset from current pixel)
float get(float x, float y, vec2 texOffset);

void main() {
	vec2 t = 1.f / textureSize(image, 0); 

	float x = (kernelX[0][0] * get(-1.f, -1.f, t)) + (kernelX[0][1] * get(0.f, -1.f, t)) + (kernelX[0][2] * get(1.f, -1.f, t)) +
              (kernelX[1][0] * get(-1.f, 0.f, t))  + (kernelX[1][1] * get(0.f, 0.f, t))  + (kernelX[1][2] * get(1.f, 0.f, t)) +
              (kernelX[2][0] * get(-1.f, 1.f, t))  + (kernelX[2][1] * get(0.f, 1.f, t))  + (kernelX[2][2] * get(1.f, 1.f, t));

	float y = (kernelY[0][0] * get(-1.f, -1.f, t)) + (kernelY[0][1] * get(0.f, -1.f, t)) + (kernelY[0][2] * get(1.f, -1.f, t)) +
              (kernelY[1][0] * get(-1.f, 0.f, t))  + (kernelY[1][1] * get(0.f, 0.f, t))  + (kernelY[1][2] * get(1.f, 0.f, t)) +
              (kernelY[2][0] * get(-1.f, 1.f, t))  + (kernelY[2][1] * get(0.f, 1.f, t))  + (kernelY[2][2] * get(1.f, 1.f, t));

	float sobel = sqrt((x * x) + (y * y)) * scale;
	color = vec4(vec3(sobel), 1.f);
}

float get(float x, float y, vec2 texOffset) {
	
	vec2 offset = texOffset * vec2(x, y);
	vec3 col = texture(image, TexCoords + offset).rgb;

	return dot(luminance, col);
}
