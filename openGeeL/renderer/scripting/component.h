#ifndef  COMPONENT_H
#define  COMPONENT_H

namespace geeL {

	class Component {

	public:
		virtual void init() {}
		virtual void update() {}
		virtual void lateUpdate() {}
	};
}

#endif
