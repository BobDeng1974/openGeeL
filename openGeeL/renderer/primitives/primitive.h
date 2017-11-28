#ifndef PRIMITIVE_H
#define PRIMITIVE_H

namespace geeL {

	class Primitive {

	public:
		virtual void init() = 0;
		virtual void draw() const = 0;
		void drawComplete();
		bool initialized();

	protected:
		unsigned int vbo;
		unsigned int vao;

	};


	inline void Primitive::drawComplete() {
		if (vao == 0)
			init();

		draw();
	}

	inline bool Primitive::initialized() {
		return !(vao == 0 || vbo == 0);
	}
}

#endif

