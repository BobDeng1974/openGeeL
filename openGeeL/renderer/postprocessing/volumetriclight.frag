#version 430 core

const float PI = 3.14159265359;

struct SpotLight {
	sampler2D shadowMap;
	mat4 lightTransform;

    vec3 position;
    vec3 direction;
	vec3 diffuse;

    float angle;
    float outerAngle;
	float bias;
};

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D shadowMap;

uniform SpotLight light;
uniform float density;
uniform int samples;
uniform mat4 inverseView;

uniform int effectOnly;

float random(vec2 co);
vec3 convertToLightSpace(vec3 fragPosition);
float calculateSpotLightShadows(vec3 coords);

void main() {
	vec3 result = /*step(effectOnly, 0.f) * */texture(image, TexCoords).rgb;
	vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
	float depth = length(fragPos);

	vec3 fragLightSpace = convertToLightSpace(fragPos);
	vec3 origLightSpace = convertToLightSpace(vec3(0.f));
	vec3 direLightSpace = fragLightSpace - origLightSpace;
	
	vec3 volume = vec3(0.f);
	float f = 1.f / float(samples);
	float stepi = random(TexCoords);
	for(int i = 0; i < samples; i++) {
		vec3 frag = origLightSpace + (direLightSpace * f * stepi);
		vec3 fragView = fragPos * f * stepi;

		float shadow = 1.f - calculateSpotLightShadows(frag);

		vec3 lightDirection = light.position - fragView;
		float lightDistance = length(lightDirection);
		lightDirection = normalize(lightDirection);

		float attenuation = 1.f / (lightDistance * lightDistance);

		attenuation = (attenuation < 0.001f) ? 0.f : attenuation;

		//Set intensity depending on if object is inside spotlights halo (or outer halo)
		float theta = dot(lightDirection, normalize(-light.direction)); 
		float epsilon = (light.angle - light.outerAngle);
		float intensity = clamp((theta - light.outerAngle) / epsilon, 0.f, 1.f);

		vec3 vol = light.diffuse * shadow * attenuation * density * intensity;

		volume += vol;
		stepi += 1.f;
	}

	volume *= (1.f / stepi);
	color = vec4(result + volume, 1.0f);
}


float calculateSpotLightShadows(vec3 coords) {
    if(coords.z > 1.f)
        return 0.f;
	else {
		float bias = light.bias;
		float curDepth = coords.z;
		float depth = texture(shadowMap, coords.xy).r;
	
		return step(depth, curDepth - bias);
	}
}

vec3 convertToLightSpace(vec3 fragPosition) {
	vec4 posLightSpace = light.lightTransform * inverseView * vec4(fragPosition, 1.0f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	return coords;
}

float random(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
