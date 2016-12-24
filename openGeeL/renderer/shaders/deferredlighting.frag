#version 430

struct PointLight {
	samplerCube shadowMap;
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float bias;
	float farPlane;
};

struct SpotLight {
	sampler2D shadowMap;
	mat4 lightTransform;

    vec3 position;
    vec3 direction;

    float angle;
    float outerAngle;
	float bias;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
	
	float constant;
    float linear;
    float quadratic;   
};


struct DirectionalLight {
	sampler2D shadowMap;
	mat4 lightTransform;

	vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float bias;
};

in vec2 textureCoordinates;
in mat3 TBN;

out vec4 color;
  
uniform int plCount;
uniform int dlCount;
uniform int slCount;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gDiffuseSpec;

uniform sampler2D ssao;
uniform int useSSAO;

uniform mat4 inverseView;
uniform vec3 origin;

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float specularMat, float occlusion, bool blinn);

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float specularMat, float occlusion, bool blinn);

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float specularMat, float occlusion, bool blinn);

float random(vec3 seed, int i);
float calculatePointLightShadows(int i, vec3 norm, vec3 fragPosition);
float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition);
float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition);


void main() {
	vec3 fragPosition = texture(gPositionDepth, textureCoordinates).rgb;
    vec3 normal		  = texture(gNormal, textureCoordinates).rgb;
    vec3 albedo		  = texture(gDiffuseSpec, textureCoordinates).rgb;
    float specular	  = texture(gDiffuseSpec, textureCoordinates).a;
	float occlusion   = (useSSAO == 1) ? texture(ssao, textureCoordinates).r : 1.f;

	vec3 viewDirection = normalize(fragPosition);
	
	bool blinn = true;	
	vec3 result = vec3(0.f, 0.f, 0.f);
	for(int i = 0; i < plCount; i++)
        result += calculatePointLight(i, pointLights[i], normal, fragPosition, viewDirection, albedo, specular, occlusion, blinn);

	for(int i = 0; i < dlCount; i++)
        result += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo, specular, occlusion, blinn);

	for(int i = 0; i < slCount; i++)
		result += calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo, specular, occlusion, blinn);

	color = vec4(result * occlusion, 1.f);
}

//Lighting.....................................................................................................................................


vec3 calculateDiffuseLighting(vec3 normal, vec3 lightDirection, vec3 diffuseMat, vec3 diffuseLight) {
	float diff = max(dot(normal, lightDirection), 0.0f);
	return diff * diffuseLight * diffuseMat;
}

vec3 calculateSpecularLighting(vec3 normal, vec3 lightDirection, vec3 viewDirection, float specularMat, vec3 specularLight, bool blinn) {
	float spec = 0.0f;

	//Blinn-Phong shading
	if(blinn) {
		vec3 halfwayDir = normalize(lightDirection - viewDirection);  
        spec = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);	
	}
	//Phong shading
	else {
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		spec = pow(max(dot(-viewDirection, reflectionDirection), 0.0f), 32.0f);
	}

	return spec * specularMat * specularLight;
}

vec3 calculatePointLight(int index, PointLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float specularMat, float occlusion, bool blinn) {

	vec3 dir = light.position - fragPosition;
	float distance = length(dir);
	vec3 lightDirection = normalize(dir);

	//Inverse square law
	float attenuation = 1.0f / (distance * distance);  

	vec3 diffuse = calculateDiffuseLighting(normal, lightDirection, albedo, light.diffuse);

	vec3 specular = calculateSpecularLighting(normal, lightDirection, 
		viewDirection, specularMat, light.specular, blinn);
	
	vec3 ambient = light.ambient * albedo * occlusion;
	float shadow = 1.0f - calculatePointLightShadows(index, normal, fragPosition);
	
    return (ambient + shadow * (diffuse + specular)) * attenuation;

}

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float specularMat, float occlusion, bool blinn) {
	
	vec3 dir = light.position - fragPosition;
	float distance = length(dir);
	vec3 lightDirection = normalize(dir);

	//Inverse square law
	float attenuation = 1.0f / (distance * distance);

	//Set intensity depending on if object is inside spotlights halo (or outer halo)
	float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = (light.angle - light.outerAngle);
    float intensity = clamp((theta - light.outerAngle) / epsilon, 0.0, 1.0);

	vec3 diffuse = calculateDiffuseLighting(normal, lightDirection, albedo, light.diffuse) * intensity;

	vec3 specular = calculateSpecularLighting(normal, lightDirection, 
		viewDirection, specularMat, light.specular, blinn) * intensity;

	vec3 ambient = light.ambient * albedo * occlusion;
	float shadow = 1.0f - calculateSpotLightShadows(index, normal, fragPosition);

    return (ambient + shadow * (diffuse + specular)) * attenuation;
}

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, 
	vec3 fragPosition, vec3 viewDirection, vec3 albedo, float specularMat, float occlusion, bool blinn) {
	
	vec3 lightDirection = normalize(light.direction - fragPosition);

	vec3 diffuse = calculateDiffuseLighting(normal, lightDirection, albedo, light.diffuse);

	vec3 specular = calculateSpecularLighting(normal, lightDirection, 
		viewDirection, specularMat, light.specular, blinn);	

	vec3 ambient = light.ambient * albedo * occlusion;
	float shadow = 1.0f - calculateDirectionalLightShadows(index, normal, fragPosition);
	
    return (ambient + shadow * (diffuse + specular));
}


//Shadows...................................................................................................................


vec3 sampleDirections3D[20] = vec3[] (
   vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), 
   vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
   vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
   vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
); 

//Poisson disc
vec2 sampleDirections2D[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float calculatePointLightShadows(int i, vec3 norm, vec3 fragPosition) {

	vec4 posLightSpace = inverseView * vec4(origin + fragPosition - pointLights[i].position, 1);
	vec3 direction = posLightSpace.xyz;
	float curDepth = length(direction) / pointLights[i].farPlane;

	//Dont' draw shadow when too far away from the viewer or from light
	float camDistance = length(fragPosition) / pointLights[i].farPlane;
	if(camDistance > 1.0f || curDepth > 0.5f)
		return 0.0f;

	//float minBias = pointLights[i].bias;
	float minBias = 0.006f;
	vec3 lightDir =  spotLights[i].position - fragPosition;
	float bias = max((minBias * 5.0f) * (1.0f - dot(norm, lightDir)), minBias);

	float shadow = 0.0f;
	int samples = 5;
	float diskRadius = 0.03f;

	for(int j = 0; j < samples; j++) {
		int index = int(20.0f * random(floor(fragPosition.xyz * 1000.0f), j)) % 20;

		float depth = texture(pointLights[i].shadowMap, direction + sampleDirections3D[index] * diskRadius).r;
		shadow += curDepth - bias > depth ? 1.0f : 0.0f;        
	}    

	return shadow / float(samples);
}

float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition) {
	vec4 posLightSpace = spotLights[i].lightTransform * inverseView * vec4(fragPosition, 1.0f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.0f)
        return 0.0f;
	
	//float minBias = spotLights[i].bias;
	float minBias = 0.001f;
	vec3 lightDir =  spotLights[i].position - fragPosition;
	float bias = max((minBias * 10.0f) * (1.0f - dot(norm, lightDir)), minBias);
	float curDepth = coords.z - bias;

	float shadow = 0.0;
	vec2 texelSize = 1.0f / textureSize(spotLights[i].shadowMap, 0);
	//Interpolate shadow map in kernel around point
	int kernel = 1;
	for(int x = -kernel; x <= kernel; x++) {
		for(int y = -kernel; y <= kernel; y++) {
			float depth = texture(spotLights[i].shadowMap, coords.xy + vec2(x, y) * texelSize).r;
			shadow += curDepth - bias > depth ? 1.0f : 0.0f;     
		}    
	}
	
	int kernelSize = 2 * kernel + 1; 
	return shadow / (kernelSize * kernelSize);
}

float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition) {
	vec4 posLightSpace = spotLights[i].lightTransform * inverseView * vec4(fragPosition, 1.0f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.0f)
        return 0.0f;

	float mapDepth = texture(directionalLights[i].shadowMap, coords.xy).r;
	float bias = directionalLights[i].bias;
	//float bias = max(0.05f * (1.0f - dot(norm, lightDir)), 0.005f);
	float curDepth = coords.z - bias;

	float shadow = 0.0;
	vec2 texelSize = 1.0f / textureSize(directionalLights[i].shadowMap, 0);
	//Interpolate shadow map in kernel around point
	int kernel = 1;
	for(int x = -kernel; x <= kernel; x++) {
		for(int y = -kernel; y <= kernel; y++) {
			float depth = texture(directionalLights[i].shadowMap, coords.xy + vec2(x, y) * texelSize).r; 
			shadow += curDepth - bias > depth ? 1.0f : 0.0f;        
		}    
	}
	
	int kernelSize = 2 * kernel + 1; 
	return shadow / (kernelSize * kernelSize);
}


//Helper functions......................................................................................................................

float random(vec3 seed, int i) {
	vec4 seed4 = vec4(seed, i);
	float dt = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	float sn = mod(dt,3.14);

	return fract(sin(sn) * 43758.5453);
}
