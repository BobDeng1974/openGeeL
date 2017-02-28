#ifndef PRIMITIVE_H
#define PRIMITIVE_H

namespace geeL {

	class Primitive {

	public:
		virtual void init() = 0;
		virtual void draw() const = 0;

		void drawComplete() {
			if (vao == 0)
				init();

			draw();
		}

	protected:
		unsigned int vbo;
		unsigned int vao;

	};

}

#endif

