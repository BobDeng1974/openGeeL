#ifndef GLGUARDS_H
#define GLGUARDS_H

namespace geeL {

	class BlendGuard {

	public:
		BlendGuard();
		BlendGuard(unsigned int i);
		~BlendGuard();

		//Blend with GL_SRC_ALPHA and GL_ONE_MINUS_SRC_ALPHA
		void blendAlpha() const;

		//Blend with GL_ONE_MINUS_SRC_ALPHA and GL_SRC_ALPHA
		void blendInverseAlpha() const;

		//Blend with GL_ZERO and GL_ONE_MINUS_SRC_COLOR
		void blendUnder() const;

		//Blend with GL_ONE and GL_ONE
		void blendAdd() const;

	private:
		int i;

		void blend(unsigned int source, unsigned int destination) const;

	};



	enum class CullingMode {
		cullNone,
		cullFront,
		cullBack
	};

	class CullingGuard {
		
	public:
		CullingGuard(CullingMode mode);
		~CullingGuard();

	private:
		CullingMode mode;

	};


	class StencilGuard {

	public:
		StencilGuard(bool guardStencil = true);
		~StencilGuard();

	private:
		bool guard;

	};


}

#endif
