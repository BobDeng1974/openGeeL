#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <list>
#include "utility/resolution.h"

namespace geeL {

	class IFrameBuffer;


	//Interface for all objects that can be used as drawing target of a framebuffer
	class RenderTarget {
		friend class LayeredTarget;

	public:
		//Assing this target to given framebuffer
		void assignTo(const IFrameBuffer& buffer, unsigned int position, bool bindFB = false);

		//Assign this target to given framebuffer besides an already assigned main buffer. 
		//Note: This function will only work if target has been assigned to a main buffer beforehand
		bool assignToo(const IFrameBuffer& buffer, unsigned int position, bool bindFB = false) const;

		virtual void applyRenderResolution() const = 0;
		virtual Resolution getRenderPreset() const = 0;
		virtual unsigned int getSize() const = 0;

		bool isAssigned() const;

	protected:
		const IFrameBuffer* parent;

		RenderTarget();

		virtual void assign(unsigned int position) const = 0;

	};

	//Layered render target that managages multiple render targets. 
	//It is assumed that all targets have same resolution
	class LayeredTarget : public RenderTarget {

	public:
		LayeredTarget(RenderTarget& target);

		template<typename... RenderTargets>
		LayeredTarget(RenderTarget& target, RenderTargets& ...targets);

		virtual void applyRenderResolution() const;
		virtual Resolution getRenderPreset() const;
		virtual unsigned int getSize() const;

	protected:
		virtual void assign(unsigned int position) const;

	private:
		std::list<RenderTarget*> targets;

		void add(RenderTarget& target);

		template<typename... RenderTargets>
		void add(RenderTarget& target, RenderTargets& ...targets);

	};



	inline LayeredTarget::LayeredTarget(RenderTarget& target) {
		add(target);
	}

	template<typename... RenderTargets>
	inline LayeredTarget::LayeredTarget(RenderTarget& target, RenderTargets& ...targets) {
		add(target, targets...);
	}

	inline void LayeredTarget::add(RenderTarget& target) {
		targets.push_back(&target);
	}

	template<typename ...RenderTargets>
	inline void LayeredTarget::add(RenderTarget& target, RenderTargets& ...targets) {
		add(target);
		add(targets...);
	}

}

#endif