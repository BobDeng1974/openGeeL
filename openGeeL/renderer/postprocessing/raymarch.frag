#version 430 core

in vec2 TexCoords;

out vec4 color;

const float PI = 3.14159265359;
const float epsilon = 0.1f;
const int maxSteps = 20;

struct PointLight {
	vec3 position;
	vec3 color;

};

struct Camera {
	vec3 position;
	vec3 forward;
	vec3 up;
	vec3 right;
};


uniform Camera camera;

vec3 raymarch(vec3 position, vec3 direction);

vec3 shade(vec3 position);
vec3 shade(vec3 position, PointLight light);

float shadow(vec3 position, vec3 direction, float dist);
float shadow(vec3 position, vec3 direction, float dist, float size);
float ambientOcclusion(int stepCount);

vec3 getNormal(vec3 position);

float distanceScene(vec3 position);
float distanceSphere(vec3 position, float radius);
float distancePlane(vec3 position, vec3 normal);

void main() { 

	float focal = 1.f;
    float u = gl_FragCoord.x * 2.f / 1000 - 1.0;
    float v = gl_FragCoord.y * 2.f / 1000 - 1.0;

	vec3 forward = normalize(cross(camera.right, camera.up));
    vec3 position = camera.position + camera.right * u + camera.up * v + forward * focal;

	color = vec4(raymarch(position, forward), 1.f);
}


vec3 raymarch(vec3 position, vec3 direction) {

	float s = 0.f;
	for(int i = 0; i < maxSteps; i++) {
		float d = distanceScene(position + s * direction);		

		if(d < epsilon)
			return shade(position); // * ambientOcclusion(i);

		s += d;
	}

	return vec3(0.f);
}

vec3 shade(vec3 position) {
	PointLight light;
	light.position = vec3(1.f, 1.f, 1.f);
	light.color = vec3(0.7f, 0.2f, 0.4f);

	vec3 dir = light.position - position;
	return shade(position, light) * shadow(light.position, normalize(dir), length(dir));
}

vec3 shade(vec3 position, PointLight light) {
	vec3 normal = getNormal(position);
    vec3 lightDir = normalize(light.position - position);
    vec3 intensity = light.color * dot(normal, lightDir);

    return intensity;
}

float shadow(vec3 position, vec3 direction, float dist) {
	for(float i = 0.f; i < dist;) {
		float d = distanceScene(position + i * direction);		

		if(d < epsilon)
			return 0.f;

		i += d;
	}

	return 1.f;
}

float shadow(vec3 position, vec3 direction, float dist, float size) {

	float penumbra = 1.f;

	for(float i = 0.f; i < dist;) {
		float d = distanceScene(position + i * direction);		

		if(d < epsilon)
			return 0.f;

		penumbra = min(penumbra, size * d / i); 
		i += d;
	}

	return penumbra;
}

float ambientOcclusion(int stepCount) {
	return min(1.f, 1.f / (stepCount + 0.00001f));
}


vec3 getNormal(vec3 position) {
	float o = 0.0001f; //Offset

	//Approximate gradient
	return normalize(vec3(
		distanceScene(position + vec3(o, 0.f, 0.f)) - distanceScene(position - vec3(o, 0.f, 0.f)),
		distanceScene(position + vec3(0.f, o, 0.f)) - distanceScene(position - vec3(0.f, o, 0.f)),
		distanceScene(position + vec3(0.f, 0.f, o)) - distanceScene(position - vec3(0.f, 0.f, o))));
}

float distanceScene(vec3 position) {
	float sphere1 = distanceSphere(position, 0.5f);
	float plane1  = distancePlane(position, vec3(0.f, 1.f, 0.f));

	return min(sphere1, plane1);
}

float distanceSphere(vec3 position, float radius) {
	return length(position) - radius;
}

float distancePlane(vec3 position, vec3 normal) {
	return dot(position, normal);
}
