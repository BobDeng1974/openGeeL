#version 430 core

in vec2 TexCoords;

out vec4 color;

const float PI = 3.14159265359;
const float epsilon = 0.01f;
const int maxSteps = 50;

const vec2 resolution = vec2(1920, 1080);

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

vec3 directIllumination(vec3 position, vec3 direction, vec3 normal, vec3 albedo);
vec3 indirectIllumination(vec3 position, vec3 direction, vec3 normal, vec3 albedo);
vec3 lightIllumination(vec3 position, vec3 normal, vec3 albedo);
vec3 lightIllumination2(vec3 position, vec3 normal, vec3 albedo);
vec3 pointLightIllumination(vec3 position, vec3 normal, PointLight light);

float shadow(vec3 position, vec3 direction, float dist);
float shadow(vec3 position, vec3 direction, float dist, float size);

vec3 getNormal(vec3 position);
vec3 reflection(vec3 position, vec3 direction, vec3 normal);

vec3 fog(vec3 pixelColor, vec3 eye, vec3 direction, float dist);
vec3 ambient(vec3 position, vec3 normal);
float ambientOcclusion(vec3 position, vec3 normal);

//Returns scene distance(w) and albedo(xyz) of current hit point 
vec4 distanceScene(vec3 position);

float distanceSphere(vec3 position, float radius);
float distanceBox(vec3 position, vec3 dimensions);
float distancePlane(vec3 position, vec3 normal);

vec3 repetitionFactor(vec3 position, vec3 offset);
vec3 transformationFactor(vec3 position, mat4 transform);

vec3 getRandomDirection(vec3 normal, int i, int count);
vec2 hammersleySeq(int i, int count);
float doto(vec3 a, vec3 b);


void main() { 
	float focal = 1.f;

	vec3 eye = -camera.position;
	vec3 right = normalize(-camera.right);
	vec3 up = normalize(-camera.up);
	vec3 forward = normalize(cross(right, up));

	vec2 uv = 2.f * TexCoords - vec2(1.f);
	uv.y = uv.y / (float(resolution.x) / float(resolution.y));

	forward = normalize(right * uv.x + up * uv.y + forward * focal);
	vec3 result = raymarch(eye, forward);

	/*
	vec2 offset = vec2(1.f) / resolution;
	vec3 f1 = normalize(right * (uv.x - offset.x) + up * uv.y + forward * focal);
	vec3 f2 = normalize(right * (uv.x + offset.x) + up * uv.y + forward * focal);
	vec3 f3 = normalize(right * uv.x + up * (uv.y - offset.y) + forward * focal);
	vec3 f4 = normalize(right * uv.x + up * (uv.y + offset.y) + forward * focal);

	//4x super sampling
	vec3 result = (raymarch(eye, f1) + raymarch(eye, f2) + 
		raymarch(eye, f3) + raymarch(eye, f4)) / 4.f;
	*/

	color = vec4(result, 1.f);
}


vec3 raymarch(vec3 position, vec3 direction) {

	float s = 0.f;
	for(int i = 0; i < maxSteps; i++) {
		vec3 pos = position + s * direction;
		vec4 a = distanceScene(pos);	
		float d = a.w;
		
		if(d < epsilon) {
			vec3 albedo = a.xyz;	
			vec3 normal = getNormal(pos);

			return directIllumination(pos, direction, normal, albedo);
		}

		s += d;
	}

	return vec3(0.f);
}

vec3 directIllumination(vec3 position, vec3 direction, vec3 normal, vec3 albedo) {
	float fresnel = 0.2f;

	return ambient(position, normal) + 
		(1.f - fresnel) * lightIllumination(position, normal, albedo) + 
		fresnel * reflection(position, direction, normal);
}

vec3 indirectIllumination(vec3 position, vec3 direction, vec3 normal, vec3 albedo) {
	int count = 10;
	vec3 color = vec3(0.f);
	
	for(int i = 0; i < count; i++) {
		vec3 dir = getRandomDirection(normal, i, count);
		vec3 startPos = position + dir * (2.f * epsilon);

		float s = 0.f;
		for(int i = 0; i < 5; i++) {
			vec3 pos = startPos + s * dir;
			vec4 a = distanceScene(pos);	
			float d = a.w;
		
			if(d < epsilon) {
				vec3 albedo = a.xyz;	
				vec3 normal = getNormal(pos);

				color += lightIllumination(pos, normal, albedo);
			}

			s += d;
		}
	}

	return color / float(count);
}

vec3 lightIllumination(vec3 position, vec3 normal, vec3 albedo) {
	PointLight light;
	light.position = vec3(0.5f, -0.5f, 1.f);
	//light.position = vec3(3.5f, -0.5f, 1.f);
	light.color = vec3(0.1f, 0.1f, 0.2f) * 10.f;

	vec3 dir = light.position - position;
	float dist = length(dir);
	dir = normalize(dir);

	vec3 lightColor = pointLightIllumination(position, normal, light) * 
		shadow(position + (20.f * epsilon) * dir, dir, dist, 1);

	return lightColor * albedo;
}

vec3 lightIllumination2(vec3 position, vec3 normal, vec3 albedo) {
	PointLight light;
	light.position = vec3(0.5f, -0.5f, 1.f);
	//light.position = vec3(3.5f, -0.5f, 1.f);
	light.color = vec3(0.1f, 0.1f, 0.2f) * 10.f;

	vec3 dir = light.position - position;
	float dist = length(dir);
	dir = normalize(dir);

	vec3 lightColor = pointLightIllumination(position, normal, light) * 
		shadow(position + (10.f * epsilon) * dir, dir, dist);

	return lightColor * albedo;
}

vec3 pointLightIllumination(vec3 position, vec3 normal, PointLight light) {
    vec3 lightDir = light.position - position;
	float dist = length(lightDir);
	float attenuation = 1.f / (dist * dist);

	lightDir = normalize(lightDir);
    vec3 intensity = light.color * doto(normal, lightDir);

    return intensity * attenuation;
}

float shadow(vec3 position, vec3 direction, float dist) {
	for(float i = 0.f; i < dist;) {
		float d = distanceScene(position + i * direction).w;		

		if(d < epsilon)
			return 0.f;

		i += d;
	}

	return 1.f;
}

float shadow(vec3 position, vec3 direction, float dist, float size) {
	float penumbra = 1.f;

	for(float i = 0.f; i < dist;) {
		float d = distanceScene(position + i * direction).w;		

		if(d < epsilon)
			return 0.f;

		penumbra = min(penumbra, size * d / i); 
		i += d;
	}

	return penumbra;
}

vec3 reflection(vec3 position, vec3 direction, vec3 normal) {
	vec3 reflection = normalize(reflect(direction, normal));
	vec3 startPos = position + reflection * (5.f * epsilon);

	float s = 0.f;
	for(int i = 0; i < maxSteps / 4; i++) {
		vec3 pos = startPos + s * reflection;
		vec4 a = distanceScene(pos);	
		float d = a.w;
		
		if(d < epsilon) {
			vec3 albedo = a.xyz;	
			vec3 normal = getNormal(pos);

			return lightIllumination(pos, normal, albedo);
		}

		s += d;
	}

	return vec3(0.f);
}

vec3 fog(vec3 pixelColor, vec3 eye, vec3 direction, float dist) {
	float c = 0.01f;
	float b = 0.01f;

	float fogAmount = c * exp(-eye.y * b) * (1.f - exp( -dist * direction.y * b )) / direction.y;
    vec3  fogColor  = vec3(0.5f, 0.6f, 0.7f);

    return mix(pixelColor, fogColor, fogAmount);
}

vec3 ambient(vec3 position, vec3 normal) {
	vec3 ambience = vec3(0.1f, 0.5f, 0.2f) * 0.0001f;
	
	return ambience * ambientOcclusion(position, normal);
}

float ambientOcclusion(vec3 position, vec3 normal) {
	float s = 0.01f;
	float t = s;
	float ao = 0.f;
	for(int i = 0; i < 10; i++) {
		float d = distanceScene(position + normal * t).w;

		ao += t - d;
		t += s;
	}

	return 1.f - clamp(ao, 0.f, 1.f);
}


vec3 getNormal(vec3 position) {
	float o = 0.0001f; //Offset

	//Approximate gradient
	return normalize(vec3(
		distanceScene(position + vec3(o, 0.f, 0.f)).w - distanceScene(position - vec3(o, 0.f, 0.f)).w,
		distanceScene(position + vec3(0.f, o, 0.f)).w - distanceScene(position - vec3(0.f, o, 0.f)).w,
		distanceScene(position + vec3(0.f, 0.f, o)).w - distanceScene(position - vec3(0.f, 0.f, o)).w    
		));
}

vec4 distanceScene(vec3 position) {
	vec3 rep = repetitionFactor(position, vec3(1.f));

	float sphere1 = distanceSphere(rep, 0.2f);
	float box1 = distanceBox(rep, vec3(0.1f, 0.1f, 0.1f));
	float plane1  = distancePlane(position, vec3(0.f, -1.f, 0.f));

	if(sphere1 < plane1)
		return vec4(0.1f, 0.1f, 0.f, sphere1);
	else
		return vec4(0.f, 0.2f, 0.2f, plane1);
}

float distanceSphere(vec3 position, float radius) {
	return length(position) - radius;
}

float distanceBox(vec3 position, vec3 dimensions) {
	return length(max(abs(position) - dimensions, 0.f));
}

float distancePlane(vec3 position, vec3 normal) {
	return dot(position, normal);
}

vec3 repetitionFactor(vec3 position, vec3 offset) {
	return mod(position, offset) - 0.5f * offset;
}

vec3 transformationFactor(vec3 position, mat4 trans) {
	vec4 pos = inverse(trans) * vec4(position, 0.f);

	return pos.xyz;
}


//Helper functions

float doto(vec3 a, vec3 b) {
	return max(dot(a, b), 0.0f);
}

//Hammersley sequence according to
//http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
vec2 hammersleySeq(int i, int count) {

	uint bits = i;
	bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    float j = float(bits) * 2.3283064365386963e-10;

	return vec2(float(i) / float(count), j);
}

vec3 getRandomDirection(vec3 n, int i, int count) {

	//Generate arbitrary span vector
	vec3 span1 = vec3(0.f);
	if (n.x != 0.f)
		span1 = vec3((-(n.y + n.z)) / n.x, 1.f, 1.f);
	else if(n.y != 0.f)
		span1 = vec3(1, (-(n.x + n.z)) / n.y, 1.f);
	else if (n.z != 0.f)
		span1 = vec3(1.f, 1.f, (-(n.x + n.y) / n.z));
	else
		return span1;

	span1 = normalize(span1);
	vec3 span2 = normalize(cross(n, span1));

	vec2 rand = hammersleySeq(i, count);

	float sinTheta = rand.x;
	float cosTheta = sqrt(1.f - (sinTheta * sinTheta));
	float psi = rand.y * 2.f * PI;

	vec3 x = sinTheta * cos(psi) * span1;
	vec3 y = sinTheta * sin(psi) * span2;
	vec3 z = cosTheta * n;

	return normalize(x + y + z);
}
