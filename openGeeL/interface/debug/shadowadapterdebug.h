#ifndef SHADOWMAPADAPTERDEBUG_H
#define SHADOWMAPADAPTERDEBUG_H

#include "snippets/guisnippets.h"

namespace geeL {

	class ShadowmapAdapter;


	class ShadowAdapterDebug : public GUISnippet {

	public:
		ShadowAdapterDebug(ShadowmapAdapter& adapter);


		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		ShadowmapAdapter& adapter;

	};

}

#endif
