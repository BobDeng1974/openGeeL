#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "../renderer.h"
#include "stackbuffer.h"

using namespace std;

namespace geeL {

	StackBufferPingPong::StackBufferPingPong() {
		reset();
	}

	StackBufferPingPong::~StackBufferPingPong() {
		//remove();

	}


	void StackBufferPingPong::init(unsigned int width, unsigned int height, ColorType colorType, 
		FilterMode filterMode, WrapMode wrapMode) {
		
		first.init(width, height, colorType, filterMode, wrapMode);
		second.init(width, height, colorType, filterMode, wrapMode);
	}

	void StackBufferPingPong::bind() const {
		current->bind();
	}

	void StackBufferPingPong::reset() {
		current = &first;
	}

	void StackBufferPingPong::initDepth() {
		first.initDepth();
	}

	
	void StackBufferPingPong::resize(ResolutionScale resolution) {
		current->resize(resolution);
	}

	void StackBufferPingPong::copyDepth(const FrameBuffer& buffer) const {
		current->copyDepth(buffer);
	}
	

	void StackBufferPingPong::fill(std::function<void()> drawCall) {
		current->fill(drawCall);
		swap();
	}

	void StackBufferPingPong::fill(Drawer & drawer) {
		current->fill(drawer);
		swap();
	}

	
	const RenderTexture& StackBufferPingPong::getTexture(unsigned int position) const {
		if (position == 0)
			return first.getTexture(0);
		else
			return second.getTexture(0);
	}

	void StackBufferPingPong::resetSize() const {
		current->resetSize();
	}

	unsigned int StackBufferPingPong::getWidth() const {
		return current->getWidth();
	}

	unsigned int StackBufferPingPong::getHeight() const {
		return current->getHeight();
	}

	std::string StackBufferPingPong::toString() const {
		return "Stack Buffer";
	}

	void StackBufferPingPong::swap() {
		if (current == &first)
			current = &second;
		else
			current = &first;

	}



	void StackBuffer::reset() {
		current = buffers[0];
	}

	void StackBuffer::init(unsigned int width, unsigned int height, ColorType colorType, FilterMode filterMode, WrapMode wrapMode) {
		info.setDimension(width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glGenFramebuffers(1, &info.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);

		// Create color attachment textures
		RenderTexture* texture1 = new RenderTexture(width, height, colorType, wrapMode, filterMode);
		RenderTexture* texture2 = new RenderTexture(width, height, colorType, wrapMode, filterMode);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture1->getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture2->getID(), 0);

		buffers.push_back(texture1);
		buffers.push_back(texture2);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void StackBuffer::fill(std::function<void()> drawCall) {

		unsigned int id = (current == buffers[0]) ? 0 : 1;
		

		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, current->getID(), 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + id);
		glViewport(0, 0, info.currWidth, info.currHeight);
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawCall();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		swap();
	}

	void StackBuffer::fill(Drawer & drawer) {
		unsigned int id = (current == buffers[0]) ? 0 : 1;

		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, current->getID(), 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + id);
		glViewport(0, 0, info.currWidth, info.currHeight);
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawer.draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		swap();
	}

	void StackBuffer::swap() {
		if (current == buffers[0])
			current = buffers[1];
		else
			current = buffers[0];
	}

}