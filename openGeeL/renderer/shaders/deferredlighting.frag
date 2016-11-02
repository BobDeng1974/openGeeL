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

uniform mat4 inverseView;
uniform vec3 origin;

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

vec3 calculatePointLight(int index, PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, float occlusion, bool blinn);
vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, float occlusion, bool blinn);
vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, float occlusion, bool blinn);

float calculatePointLightShadows(int i, vec3 norm, vec3 fragPosition);
float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition);
float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition);


void main() {
	vec3 fragPosition = texture(gPositionDepth, textureCoordinates).rgb;
    vec3 normal = texture(gNormal, textureCoordinates).rgb;
    vec3 texColor = texture(gDiffuseSpec, textureCoordinates).rgb;
    float spec = texture(gDiffuseSpec, textureCoordinates).a;
	float occlusion = texture(ssao, textureCoordinates).r;

	vec3 speColor = vec3(spec, spec, spec);
	vec3 viewDirection = normalize(fragPosition);
	
	bool blinn = true;	
	vec3 result = vec3(0.f, 0.f, 0.f);
	for(int i = 0; i < plCount; i++)
        result += calculatePointLight(i, pointLights[i], normal, fragPosition, viewDirection, texColor, speColor, occlusion, blinn);

	for(int i = 0; i < dlCount; i++)
        result += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, texColor, speColor, occlusion, blinn);

	for(int i = 0; i < slCount; i++)
		result += calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, texColor, speColor, occlusion, blinn);

	//result = pow(result.rgb, vec3(0.4545f));
	color = vec4(result, 1.f);
	
}

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed, i);
	float dt = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	float sn = mod(dt,3.14);
	return fract(sin(sn) * 43758.5453);
}

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

//Point Lights

float calculatePointLightShadows(int i, vec3 norm, vec3 fragPosition) {

	vec4 posLightSpace = inverseView * vec4(origin + fragPosition - pointLights[i].position, 1);
	vec3 direction = posLightSpace.xyz;
	float curDepth = length(direction) / pointLights[i].farPlane;

	//Dont' draw shadow when too far away from the viewer or from light
	float camDistance = length(fragPosition) / pointLights[i].farPlane;
	if(camDistance > 1.0f || curDepth > 0.5f)
		return 0.0f;

	//float minBias = pointLights[i].bias;
	float minBias = 0.0055f;
	vec3 lightDir =  spotLights[i].position - fragPosition;
	//float bias = max((minBias * 5.0f) * (1.0f - dot(norm, lightDir)), minBias);
	float bias = minBias;

	float shadow = 0.0f;
	int samples = 5;
	float diskRadius = 0.03f;

	for(int j = 0; j < samples; j++) {
		//int index = j;
		int index = int(20.0f * random(floor(fragPosition.xyz * 1000.0f), j)) % 20;

		float depth = texture(pointLights[i].shadowMap, direction + sampleDirections3D[index] * diskRadius).r;
		shadow += curDepth - bias > depth ? 1.0f : 0.0f;        
	}    

	return shadow / float(samples);
}

vec3 calculatePointLight(int index, PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, float occlusion, bool blinn) {

	vec3 dir = light.position - fragPosition;
	float distance = length(dir);
	vec3 direction = normalize(dir);

	float attenuation = 1.0f / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));  

	//Diffuse
	float diff = max(dot(normal, direction), 0.0f);
	vec3 diffuse = diff * light.diffuse * texColor;

	//Specular
	float spec = 0.0f;
	if(blinn) {
		vec3 halfwayDir = normalize(direction + viewDirection);  
        spec = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);	
	}
	else {
		vec3 reflectionDirection = reflect(-direction, normal);
		spec = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32.0f);
	}

	vec3 specular = spec * speColor * light.specular;
	vec3 ambient = light.ambient * texColor * occlusion;
	float shadow = calculatePointLightShadows(index, normal, fragPosition);
	
    return (ambient + (1.0f - shadow) * (diffuse + specular)) * attenuation;

}

//Spotlights

float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition) {
	vec4 posLightSpace = spotLights[i].lightTransform * inverseView * vec4(fragPosition, 1.0f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.0f)
        return 0.0f;
	
	//float minBias = spotLights[i].bias;
	float minBias = 0.0005f;
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

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, float occlusion, bool blinn) {
	vec3 dir = light.position - fragPosition;
	float distance = length(dir);
	vec3 direction = normalize(dir);

	//float attenuation = 1.0f / (light.constant + light.linear * distance + 
    //		    light.quadratic * (distance * distance));  

	float attenuation = 1.0f / (distance * distance);  

	float theta = dot(direction, normalize(-light.direction)); 
    float epsilon = (light.angle - light.outerAngle);
    float intensity = clamp((theta - light.outerAngle) / epsilon, 0.0, 1.0);

	//Diffuse
	float diff = max(dot(normal, direction), 0.0f);
	vec3 diffuse = diff * light.diffuse * texColor * intensity;

	//Specular
	float spec = 0.0f;
	if(blinn) {
		vec3 halfwayDir = normalize(direction + viewDirection);  
        spec = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);	
	}
	else {
		vec3 reflectionDirection = reflect(-direction, normal);
		spec = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32.0f);
	}

	vec3 specular = spec * speColor * light.specular * intensity;
	vec3 ambient = light.ambient * texColor * occlusion;
	float shadow = calculateSpotLightShadows(index, normal, fragPosition);

    return (ambient + (1.0f - shadow) * (diffuse + specular)) * attenuation;
}

//Directional Lights

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

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, float occlusion, bool blinn) {

	vec3 direction = normalize(light.direction - fragPosition);

	//Diffuse
	float diff = max(dot(normal, direction), 0.0f);
	vec3 diffuse = diff * light.diffuse * texColor;

	//Specular
	float spec = 0.0f;
	if(blinn) {
		vec3 halfwayDir = normalize(direction + viewDirection);  
        spec = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);	
	}
	else {
		vec3 reflectionDirection = reflect(-direction, normal);
		spec = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32.0f);
	}

	vec3 specular = spec * speColor * light.specular;
	vec3 ambient = light.ambient * texColor * occlusion;
	float shadow = calculateDirectionalLightShadows(index, normal, fragPosition);
	
    return (ambient + (1.0f - shadow) * (diffuse + specular));
}