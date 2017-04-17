#version 430 core

const float PI = 3.14159265359f;

out vec4 color;

in vec3 localPos;

uniform samplerCube environmentMap;

void main() {		
	vec3 irradiance = vec3(0.f);

    vec3 normal = normalize(localPos);
	vec3 right = cross(vec3(0.f, 1.f, 0.f), normal);
	vec3 up = cross(normal, right);

	float sampleStep = 0.025f;
	int sampleCount = 0;

	//Sample hemisphere to approximate ediffuse lighting integral
	for(float phi = 0.f; phi < 2.f * PI; phi += sampleStep) {
		for(float theta = 0.f; theta < 0.5f * PI; theta += sampleStep) {

			//Spherical to cartesian
			float x = sin(theta) * cos(phi);
			float y = sin(theta) * sin(phi);
			float z = cos(theta);

			//Translate to word space
			vec3 samp = x * right + y * up + z * normal;
			irradiance += texture(environmentMap, samp).rgb * cos(theta) * sin(theta);
			sampleCount++;
		}
	}

	float div = 1.f / float(sampleCount);
	irradiance = PI * irradiance * div;

	color = vec4(irradiance, 1.f);
}