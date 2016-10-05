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
	vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal;
in vec3 fragPosition;
in vec2 textureCoordinates;
in vec3 cameraPosition;

out vec4 color;
  
uniform Material material;

uniform int plCount;


#define MAX_POINT_LIGHTS 5
uniform PointLight pointLights[MAX_POINT_LIGHTS];

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, bool blinn);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, bool blinn);
vec3 calculateDirectionaLight(DirectionalLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, bool blinn);

void main() {

	//Discard fragment when material type is cutout and alpha value is very low
	if(material.type == 1 && texture(material.diffuse, textureCoordinates).a < 0.1f)
		discard;

	vec3 texColor = vec3(texture(material.diffuse, textureCoordinates));
	vec3 speColor = vec3(texture(material.specular, textureCoordinates));

	vec3 norm = normalize(normal);
	vec3 viewDirection = normalize(cameraPosition - fragPosition);

	bool blinn = true;
	
	vec3 result = vec3(0.f, 0.f, 0.f);
	for(int i = 0; i < plCount; i++)
        result += calculatePointLight(pointLights[i], norm, fragPosition, viewDirection, texColor, speColor, blinn);    

	color = vec4(result, 1.f);
}

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

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, bool blinn) {

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
	
    return (ambient + diffuse + specular) * attenuation;
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection, vec3 texColor, vec3 speColor, bool blinn) {

	vec3 direction = normalize(-light.direction);

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
	
    return (ambient + diffuse + specular);
}