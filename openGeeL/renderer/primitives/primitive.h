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

		bool initialized() {
			return !(vao == 0 || vbo == 0);
		}

	protected:
		unsigned int vbo;
		unsigned int vao;

	};

}

#endif

