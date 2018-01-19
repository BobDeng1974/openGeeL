

#ifdef POSITION_MAP

	vec4 readPositionRoughness(vec2 texCoords) {
		return texture(POSITION_MAP, texCoords);
	}
	
	vec3 readPosition(vec2 texCoords) {
		return texture(POSITION_MAP, texCoords).xyz;
	}

	float readDepth(vec2 texCoords) {
		return -texture(POSITION_MAP, texCoords).z;
	}


#endif

#ifdef NORMAL_MAP

	vec4 readNormalMetallic(vec2 texCoords) {
		return texture(NORMAL_MAP, texCoords);
	}

	vec3 readNormal(vec2 texCoords) {
		return texture(NORMAL_MAP, texCoords).xyz;
	}

	float readMetallic(vec2 texCoords) {
		return texture(NORMAL_MAP, texCoords).w;
	}


#endif

#ifdef DIFFUSE_MAP

	vec4 readDiffuse(vec2 texCoords) {
		return texture(DIFFUSE_MAP, texCoords);
	}

#endif

#ifdef EMISSIVITY_MAP

	vec3 readEmissitivity(vec2 texCoords) {
		return texture(EMISSIVITY_MAP, texCoords).rgb;
	}

#endif

#ifdef OCCLUSION_MAP 

	float readOcclusion(vec2 texCoords) {
		return texture(OCCLUSION_MAP, texCoords).r;
	}

#endif