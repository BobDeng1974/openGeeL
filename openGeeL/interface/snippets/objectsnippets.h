#ifndef OBJECTSNIPPETS_H
#define OBJECTSNIPPETS_H

#include "guisnippets.h"

namespace geeL {
	
	class SceneCamera;
	class Light;
	class PerspectiveCamera;
	class SceneObject;
	class SimpleShadowmap;
	class MeshRenderer;


	class SceneObjectSnippet : public GUISnippet {

	public:
		SceneObjectSnippet(SceneObject& obj);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		SceneObject& obj;

	};


	class SingleMeshRendererSnippet : public SceneObjectSnippet {

	public:
		SingleMeshRendererSnippet(MeshRenderer& mesh);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		MeshRenderer& mesh;

	};

	class ShadowMapSnippet : public GUISnippet {

	public:
		ShadowMapSnippet(SimpleShadowmap& map);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		SimpleShadowmap& map;

		void drawResolutionMeter(GUIContext* context);

	};


	class LightSnippet : public SceneObjectSnippet {

	public:
		LightSnippet(Light& light);
		LightSnippet(Light& light, ShadowMapSnippet& snippet);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		Light& light;
		ShadowMapSnippet* snippet;

	};

	
	class CameraSnippet : public SceneObjectSnippet {

	public:
		CameraSnippet(SceneCamera& cam);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		SceneCamera& cam;

	};

	class PerspectiveCameraSnippet : public CameraSnippet {

	public:
		PerspectiveCameraSnippet(PerspectiveCamera& pcam);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		PerspectiveCamera& pcam;

	};
}

#endif

