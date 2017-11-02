#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "glguards.h"

namespace geeL {
	
	BlendGuard::BlendGuard() : i(-1){
		glEnable(GL_BLEND);
	}
	
	BlendGuard::BlendGuard(unsigned int i) : i(i) {
		glEnablei(GL_BLEND, i);
	}

	BlendGuard::~BlendGuard() {
		if (i < 0) {
			glDisable(GL_BLEND);
		}
		else {
			glDisablei(GL_BLEND, i);
		}
	}


	void BlendGuard::blendAlpha() const {
		if (i < 0) {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else {
			glBlendFunci(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, i);
		}
	}



	CullingGuard::CullingGuard(CullingMode mode) : mode(mode){
		switch (mode) {
			case CullingMode::cullNone:
				glDisable(GL_CULL_FACE);
				break;
			case CullingMode::cullBack:
				glCullFace(GL_BACK);
				break;
		}
	}

	CullingGuard::~CullingGuard() {
		switch (mode) {
			case CullingMode::cullNone:
				glEnable(GL_CULL_FACE);
				break;
			case CullingMode::cullBack:
				glCullFace(GL_FRONT);
				break;
		}
	}

}