#version 430

struct Material {
	sampler2D diffuse;
	sampler2D specular;

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

layout (std140) uniform shader_data { 
	vec3 position;

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

out vec4 color;
  
uniform vec3 viewerPosition;

uniform Material material;

#define MAX_POINT_LIGHTS 5
uniform PointLight pointLights[MAX_POINT_LIGHTS];


void main() {

	vec3 texColor = vec3(texture(material.diffuse, textureCoordinates));
	vec3 speColor = vec3(texture(material.specular, textureCoordinates));

    vec3 ambient = pointLights[0].ambient * texColor;

	vec3 norm = normalize(normal);

	vec3 dir = pointLights[0].position - fragPosition;

	float distance = length(dir);
	vec3 direction = normalize(dir);

	float attenuation = 1.0f / (pointLights[0].constant + pointLights[0].linear * distance + 
    		    pointLights[0].quadratic * (distance * distance));  

	//Diffuse
	float diff = max(dot(norm, direction), 0.0f);
	vec3 diffuse = diff * pointLights[0].diffuse * texColor;

	//Specular
	vec3 viewDirection = normalize(viewerPosition - fragPosition);
	vec3 reflectionDirection = reflect(-direction, normal);
	float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
	vec3 specular = spec * speColor * pointLights[0].specular;
	
    color = vec4((ambient + diffuse + specular) * attenuation, 1.0f);
}