#version 430

const float PI = 3.14159265359;
const float epsilon = 0.000001f;
const float FLOAT_MAX = 100000.f;
const vec3  luminance = vec3(0.299f, 0.587f, 0.114f);

in vec2 TexCoords;

layout (location = 0) out vec4 color;

uniform mat4 inverseView;
uniform vec3 origin;

uniform int dimensions;
uniform float farClip;
uniform int minStep;

uniform sampler3D voxelTexture;
uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;
uniform sampler2D gDiffuseSpec;


vec4 getFragmentColor(vec3 position, float lvl);


void main() {
	vec4 normMet  = texture(gNormalMet, TexCoords);
	vec4 diffSpec = texture(gDiffuseSpec, TexCoords);

	vec3 baseColor = texture(image, TexCoords).rgb;
	vec3 posView  = texture(gPositionDepth, TexCoords).rgb;
	vec3 normView = normalize(normMet.rgb);
	vec3 viewView = normalize(-posView);

	vec3 position = (inverseView * vec4(posView, 1.f)).xyz;
	vec3 normal = normalize((inverseView * vec4(origin + normView, 1.f)).xyz);
	vec3 view = normalize((inverseView * vec4(origin + viewView, 1.f)).xyz);
	vec3 refl = normalize(reflect(-view, normal));
	vec3 camPosition = (inverseView * vec4(vec3(0.f), 1.f)).xyz;

	vec3 albedo = diffSpec.rgb;
	float roughness = diffSpec.w;
	float metallic = normMet.w;
	float luma = dot(luminance, baseColor);

	vec4 fragColor = getFragmentColor(position, 1);

	color = vec4(fragColor.rgb, 1.f);
}


vec4 sampleTexture(vec3 position, vec3 direction, int maxLevel) {
	

	return vec4(1.f);
}


vec4 getFragmentColor(vec3 position, float lvl) {
	vec4 col = textureLod(voxelTexture, position * 0.5f / dimensions, lvl); 

	//if(length(col) < 0.0001f)
	//	return vec4(1.f);

	col.rgb = pow(col.rgb, vec3(2.2f)); //Gamma correction

	return col;
}

