#version 430 core

out vec4 color;

in vec3 localPos;

uniform sampler2D environmentMap;

const vec2 invAtan = vec2(0.1591f, 0.3183f);
vec2 sampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5f;

    return uv;
}

void main() {
		
    vec2 TexCoords = sampleSphericalMap(normalize(localPos));
    vec3 result = texture(environmentMap, TexCoords).rgb;
    
    color = vec4(result, 1.f);
}