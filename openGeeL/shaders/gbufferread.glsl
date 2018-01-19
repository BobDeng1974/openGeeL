
#ifndef TEXTURE_COORDINATES
#define TEXTURE_COORDINATES texCoords
#endif

#ifdef POSITION_MAP

	vec4 readPositionRoughness() {
		return texture(POSITION_MAP, TEXTURE_COORDINATES);
	}
	
	vec3 readPosition() {
		return texture(POSITION_MAP, TEXTURE_COORDINATES).xyz;
	}

	float readDepth() {
		return -texture(POSITION_MAP, TEXTURE_COORDINATES).z;
	}


#endif

#ifdef NORMAL_MAP

	vec4 readNormalMetallic() {
		return texture(NORMAL_MAP, TEXTURE_COORDINATES);
	}

	vec3 readNormal() {
		return texture(NORMAL_MAP, TEXTURE_COORDINATES).xyz;
	}

	float readMetallic() {
		return texture(NORMAL_MAP, TEXTURE_COORDINATES).w;
	}


#endif

#ifdef DIFFUSE_MAP

	vec4 readDiffuse() {
		return texture(DIFFUSE_MAP, TEXTURE_COORDINATES);
	}

#endif

#ifdef EMISSIVITY_MAP

	vec3 readEmissitivity() {
		return texture(EMISSIVITY_MAP, TEXTURE_COORDINATES).rgb;
	}

#endif

#ifdef OCCLUSION_MAP 

	float readOcclusion() {
		return texture(OCCLUSION_MAP, TEXTURE_COORDINATES).r;
	}

#endif