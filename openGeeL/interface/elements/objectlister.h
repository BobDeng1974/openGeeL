#ifndef OBJECTLISTER_H
#define OBJECTLISTER_H

#include "../guielement.h"

namespace geeL {

	class RenderScene;

	class ObjectLister : public GUIElement {

	public:
		ObjectLister(RenderScene& scene);

		virtual void draw(GUIContext* context);

	private:
		RenderScene& scene;

	};
}

#endif
