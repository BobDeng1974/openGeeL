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
		blend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void BlendGuard::blendReverseAlpha() const {
		blend(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

	}

	void BlendGuard::blendAdd() const {
		blend(GL_ONE, GL_ONE);
	}

	void BlendGuard::blend(unsigned int source, unsigned int destination) const {
		if (i < 0) {
			glBlendFunc(source, destination);
		}
		else {
			glBlendFunci(source, destination, i);
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