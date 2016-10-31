#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffuseSpec;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;

	int mapFlags;
	int type; //0 = Opaque, 1 = Cutout, 2 = Transparent
	float shininess;
};

in vec3 normal;
in vec3 fragPosition;
in vec2 textureCoordinates;
in mat3 TBN;

uniform Material material;

void main() {    

	//Check if materials is actually textured
	float diffFlag = mod(material.mapFlags, 10);
	float specFlag = mod(material.mapFlags / 10, 10);
	float normFlag = mod(material.mapFlags / 100, 10);

	//Discard fragment when material type is cutout and alpha value is very low
	if(material.type == 1 && diffFlag == 1 && texture(material.diffuse, textureCoordinates).a < 0.1f)
		discard;

    gPosition = fragPosition;
    
	vec3 norm = normalize(normal);
	if(normFlag == 1) {
		norm = texture(material.normal, textureCoordinates).rgb;
		norm = normalize(norm * 2.0f - 1.0f);
		norm = normalize(TBN * norm);
	}
	gNormal = norm;

	vec3 texColor = (diffFlag == 1) ? texture(material.diffuse, textureCoordinates).rgb : vec3(0.01f, 0.01f, 0.01f);
	vec3 speColor = (specFlag == 1) ? texture(material.specular, textureCoordinates).rgb : vec3(0.1f, 0.1f, 0.1f); 
    gDiffuseSpec.rgb = texColor;
    gDiffuseSpec.a = speColor.r;
} 
