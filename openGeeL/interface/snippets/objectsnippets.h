#ifndef OBJECTSNIPPETS_H
#define OBJECTSNIPPETS_H

#include "guisnippets.h"

namespace geeL {
	
	class SceneCamera;
	class Light;
	class MeshRenderer;
	class PerspectiveCamera;
	class SceneObject;


	class SceneObjectSnippet : public GUISnippet {

	public:
		SceneObjectSnippet(SceneObject& obj);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		SceneObject& obj;

	};


	class MeshRendererSnippet : public SceneObjectSnippet {

	public:
		MeshRendererSnippet(MeshRenderer& mesh);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		MeshRenderer& mesh;

	};


	class LightSnippet : public SceneObjectSnippet {

	public:
		LightSnippet(Light& light);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		Light& light;

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

