#ifndef GLGUARDS_H
#define GLGUARDS_H

namespace geeL {

	class BlendGuard {

	public:
		BlendGuard();
		BlendGuard(unsigned int i);
		~BlendGuard();

		void blendAlpha() const;

	private:
		int i;

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

}

#endif
