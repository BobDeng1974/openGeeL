#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <vec3.hpp>
#include <list>
#include <vector>

namespace geeL {

class Camera;
class LightManager;
class Skybox;
class Shader;
class MeshRenderer;
class Model;
class Material;
class MeshFactory;
class Transform;
enum CullingMode;

class RenderScene {

public:
	Camera& camera;
	LightManager& lightManager;

	RenderScene(LightManager& lightManager, Camera& camera, MeshFactory& meshFactory);
	~RenderScene();

	void setSkybox(Skybox& skybox);
	void bindSkybox(const Shader& shader) const;

	//Create and add new mesh renderer to scene
	MeshRenderer& AddMeshRenderer(std::string modelPath, Transform& transform, CullingMode faceCulling);

	//Create and add new mesh renderer with custom materials to scene
	MeshRenderer& AddMeshRenderer(std::string modelPath, Transform& transform, 
		std::vector<Material*> materials, CullingMode faceCulling);
		
	//Draw only those objects that are suited for deferred rendering
	void drawDeferred() const;

	//Draw all those objects that are suited not for deferred rendering (with forward rendering)
	void drawForward() const;

	//Draw all objects with forward rendering
	void draw() const;

	//Draw skybox indepentently
	void drawSkybox() const;

	//Draw only the objects in the scene and all with given shader
	void drawObjects(const Shader& shader) const;

	glm::vec3 TranslateToScreenSpace(glm::vec3 vector) const;


private:
	Skybox* skybox;
	MeshFactory& meshFactory;
	std::list<MeshRenderer*> deferredRenderObjects;
	std::list<MeshRenderer*> forwardRenderObjects;
	
	void drawObjects(const std::list<MeshRenderer*>& objects, bool deferred = true) const;

};


}

#endif