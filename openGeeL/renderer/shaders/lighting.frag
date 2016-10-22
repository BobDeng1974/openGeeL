#version 430

struct Material {
	sampler2D diffuse;
	sampler2D specular;

	int type; //0 = Opaque, 1 = Cutout, 2 = Transparent
	float shininess;
};

struct PointLight {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	sampler2D shadowMap;

    vec3 position;
    vec3 direction;

    float angle;
    float outerAngle;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
	
	float constant;
    float linear;
    float quadratic;   
};


struct DirectionalLight {
	sampler2D shadowMap;
	vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal;
in vec3 fragPosition;
in vec2 textureCoordinates;
in vec3 cameraPosition;

in vec4 spotLightTransforms[5];
in vec4 direLightTransforms[5];

out vec4 color;
  
uniform Material material;

uniform int plCount;
uniform int dlCount;
uniform int slCount;

uniform PointLight pointLights[5];
uniform DirectionalLight directionalLights[5];
uniform SpotLight spotLights[5];

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, bool blinn);
vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, bool blinn);
vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, bool blinn);
float calculateSpotLightShadows(int i);
float calculateDirectionalLightShadows(int i);

void main() {

	//Discard fragment when material type is cutout and alpha value is very low
	if(material.type == 1 && texture(material.diffuse, textureCoordinates).a < 0.1f)
		discard;

	vec3 texColor = vec3(texture(material.diffuse, textureCoordinates).rgb);
	vec3 speColor = vec3(texture(material.specular, textureCoordinates));

	vec3 norm = normalize(normal);
	vec3 viewDirection = normalize(cameraPosition - fragPosition);

	bool blinn = true;
	
	vec3 result = vec3(0.f, 0.f, 0.f);
	for(int i = 0; i < plCount; i++)
        result += calculatePointLight(pointLights[i], norm, fragPosition, viewDirection, texColor, speColor, blinn);

	for(int i = 0; i < dlCount; i++)
        result += calculateDirectionaLight(i, directionalLights[i], norm, fragPosition, viewDirection, texColor, speColor, blinn);

	for(int i = 0; i < slCount; i++)
		result += calculateSpotLight(i, spotLights[i], norm, fragPosition, viewDirection, texColor, speColor, blinn);

	color = vec4(result, 1.f);
}


//Point Lights

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, bool blinn) {

	vec3 dir = light.position - fragPosition;

	float distance = length(dir);
	vec3 direction = normalize(dir);

	vec3 ambient = light.ambient * texColor;

	float attenuation = 1.0f / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));  

	//Diffuse
	float diff = max(dot(normal, direction), 0.0f);
	vec3 diffuse = diff * light.diffuse * texColor;

	//Specular
	float spec = 0.0f;
	if(blinn) {
		vec3 halfwayDir = normalize(direction + viewDirection);  
        spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess);	
	}
	else {
		vec3 reflectionDirection = reflect(-direction, normal);
		spec = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
	}

	vec3 specular = spec * speColor * light.specular;
	
    return (ambient + diffuse + specular) * attenuation;

}

//Spotlights

float calculateSpotLightShadows(int i) {
	vec3 coords = spotLightTransforms[i].xyz / spotLightTransforms[i].w;
	coords = coords * 0.5f + 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.0f)
        return 0.0f;

	float mapDepth = texture(spotLights[i].shadowMap, coords.xy).r;
	float bias = 0.0005f;
	//float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);
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

vec3 calculateSpotLight(int index, SpotLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, bool blinn) {

	vec3 dir = light.position - fragPosition;

	float distance = length(dir);
	vec3 direction = normalize(dir);

	vec3 ambient = light.ambient * texColor;


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
        spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess);	
	}
	else {
		vec3 reflectionDirection = reflect(-direction, normal);
		spec = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
	}

	vec3 specular = spec * speColor * light.specular * intensity;

	float shadow = calculateSpotLightShadows(index);
    return (ambient + (1.0f - shadow) * (diffuse + specular)) * attenuation;
}

//Directional Lights

float calculateDirectionalLightShadows(int i) {
	vec3 coords = direLightTransforms[i].xyz / direLightTransforms[i].w;
	coords = coords * 0.5f + 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.0f)
        return 0.0f;

	float mapDepth = texture(directionalLights[i].shadowMap, coords.xy).r;
	float bias = 0.0005f;
	//float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);
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

vec3 calculateDirectionaLight(int index, DirectionalLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, bool blinn) {

	vec3 direction = normalize(light.direction - fragPosition);

	vec3 ambient = light.ambient * texColor;

	//Diffuse
	float diff = max(dot(normal, direction), 0.0f);
	vec3 diffuse = diff * light.diffuse * texColor;

	//Specular
	float spec = 0.0f;
	if(blinn) {
		vec3 halfwayDir = normalize(direction + viewDirection);  
        spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess);	
	}
	else {
		vec3 reflectionDirection = reflect(-direction, normal);
		spec = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
	}

	vec3 specular = spec * speColor * light.specular;

	float shadow = calculateDirectionalLightShadows(index);
	
    return (ambient + (1.0f - shadow) * (diffuse + specular));
}