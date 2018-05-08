#ifndef DEPTHREADER_H
#define DEPTHREADER_H

namespace geeL {

	class ComputeShader;
	class TextureProvider;


	class DepthReader {

	public:
		DepthReader(TextureProvider& provider);
		~DepthReader();

		void readDepth();

		//Returns depth that was last read
		float getDepth() const;

	private:
		float depth;
		unsigned int ssbo;
		ComputeShader* shader;
		TextureProvider& provider;

	};

}

#endif
