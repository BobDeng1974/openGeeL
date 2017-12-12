#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <map>
#include <functional>
#include <string>
#include <list>
#include <vector>
#include "animation/bone.h"
#include "memory/memoryobject.h"
#include "memory/poolallocator.h"
#include "utility/listener.h"
#include "appglobals.h"
#include "workerthread.h"

enum aiTextureType;

struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiScene;

using namespace geeL::memory;

namespace geeL {

	enum class ColorType;
	enum class CullingMode;
	enum class MapType;
	enum class ShadingMethod;

	struct SkinnedVertex;
	struct MeshBone;

	class AnimationMemory;
	class DefaultMaterialContainer;
	class MaterialFactory;
	class StaticMeshRenderer;
	class Model;
	class SceneShader;
	class SkinnedMesh;
	class SkinnedMeshRenderer;
	class SkinnedModel;
	class StaticModel;
	class StaticMesh;
	class TextureMap;
	class Transform;
	

	class MeshFactory : public DataEventActuator<Model> {

	public:
		MeshFactory(MaterialFactory& factory);

		//Create new mesh renderer with default shading
		std::unique_ptr<StaticMeshRenderer> createMeshRenderer(MemoryObject<StaticModel> model,
			Transform& transform, const std::string& name);

		std::unique_ptr<StaticMeshRenderer> createMeshRenderer(MemoryObject<StaticModel> model,
			SceneShader& shader, Transform& transform, const std::string& name);

		//Create separate mesh renderers for every mesh in given model
		//Note: Memory of returned mesh renderers is unmanaged
		std::list<StaticMeshRenderer*> createMeshRenderers(MemoryObject<StaticModel> model,
			SceneShader& shader, Transform& transform);

		//Create new skinned mesh renderer with default shading
		std::unique_ptr<SkinnedMeshRenderer> createMeshRenderer(MemoryObject<SkinnedModel> model,
			Transform& transform, const std::string& name);

		std::unique_ptr<SkinnedMeshRenderer> createMeshRenderer(MemoryObject<SkinnedModel> model,
			SceneShader& shader, Transform& transform, const std::string& name);

		//Creates, initializes and returns a new static model from given file path or 
		//returns an existing model if it already uses this file
		MemoryObject<StaticModel> createStaticModel(const std::string& filePath);
		

		//Creates, initializes and returns a new static model from given file path or 
		//returns an existing model if it already uses this file
		MemoryObject<SkinnedModel> createSkinnedModel(const std::string& filePath);

#if MULTI_THREADING_SUPPORT
		void createModelParallel(const std::string& filePath, std::function<void(MemoryObject<StaticModel>)> callback);
		void createModelParallel(const std::string& filePath, std::function<void(MemoryObject<SkinnedModel>)> callback);
#endif
		

	private:
#if MULTI_THREADING_SUPPORT
		WorkerThread workerThread;
#endif

		MaterialFactory& factory;
		std::unique_ptr<Memory> memory;
		std::map<std::string, std::weak_ptr<StaticModel>> staticModels;
		std::map<std::string, std::weak_ptr<SkinnedModel>> skinnedModels;
		
		void fillStaticModel(StaticModel& model, std::string path);
		void processStaticNode(StaticModel& model, std::string directory, aiNode* node, const aiScene* scene);

		void fillSkinnedModel(SkinnedModel& model, std::string path);
		void processSkinnedNode(SkinnedModel& model, Bone& bone, std::string directory, aiNode* node, const aiScene* scene);

		template<class V>
		void processVertices(std::vector<V, MemoryAllocator<V>>& vertices, aiMesh* mesh);
		void processIndices(std::vector<unsigned int, MemoryAllocator<unsigned int>>& indices, aiMesh* mesh);
		void processBones(std::vector<SkinnedVertex, MemoryAllocator<SkinnedVertex>>& vertices,
			std::map<std::string, MeshBone>& bones, aiMesh* mesh);

		void processMaterial(DefaultMaterialContainer& material, aiMesh* mesh, const aiScene* scene);
		void processTextures(std::vector<MemoryObject<TextureMap>>& textures, std::string directory, aiMesh* mesh, const aiScene* scene);
		void processAnimations(SkinnedModel& model, const aiScene* scene);
		void processSkeleton(Bone& bone, aiNode* node);

		void loadMaterialTextures(std::vector<MemoryObject<TextureMap>>& textures, aiMaterial* mat,
			aiTextureType aiType, MapType type, std::string directory, ColorType colorType);

	};
}

#endif 
