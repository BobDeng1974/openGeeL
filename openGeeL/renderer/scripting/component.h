#ifndef  COMPONENT_H
#define  COMPONENT_H

namespace geeL {

	class Component {

	public:
		virtual void init() = 0;
		virtual void update() = 0;
	};
}

#endif
