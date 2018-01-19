

#ifdef POSITION_MAP

	vec3 readPosition(vec2 texCoords) {
		return texture(POSITION_MAP, texCoords).xyz;
	}

	float readDepth(vec2 texCoords) {
		return -texture(POSITION_MAP, texCoords).z;
	}


#endif

#ifdef NORMAL_MAP

	vec3 readNormal(vec2 texCoords) {
		return texture(NORMAL_MAP, texCoords).xyz;
	}

#endif

#ifdef DIFFUSE_MAP

	vec4 readDiffuse(vec2 texCoords) {
		return texture(DIFFUSE_MAP, texCoords);
	}

#endif

#ifdef PROPERTY_MAP

	vec4 readProperties(vec2 texCoords) {
		return texture(PROPERTY_MAP, texCoords);
	}

	float readOcclusion(vec2 texCoords) {
		return texture(PROPERTY_MAP, texCoords).a;
	}

	vec3 readEmissitivity(vec2 texCoords) {
		return vec3(texture(PROPERTY_MAP, texCoords).b);
	}

	float readRoughness(vec2 texCoords) {
		return texture(PROPERTY_MAP, texCoords).r;
	}

	float readMetallic(vec2 texCoords) {
		return texture(PROPERTY_MAP, texCoords).g;
	}


#endif
