#define GLEW_STATIC
#include <glew.h>
#include "dummytexture.h"

using namespace std;

namespace geeL {

	unique_ptr<Texture2D> DummyTexture::createTexture2D() {
		vector<GLubyte> data(4, 0);
		Texture2D* t = new Texture2D(Resolution(1, 1), ColorType::RGBA, FilterMode::None, 
			WrapMode::Repeat, AnisotropicFilter::None, &data);

		return unique_ptr<Texture2D>(t);
	}

	std::unique_ptr<TextureCube> DummyTexture::createTextureCube() {
		vector<GLubyte> d(4, 0);
		void* dataArray[6] = { &d, &d, &d, &d, &d, &d };

		TextureCube* t = new TextureCube(1, ColorType::RGBA, FilterMode::None,
			WrapMode::Repeat, dataArray);

		return std::unique_ptr<TextureCube>(t);
	}

}