#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <map>
#include <string>
#include <list>
#include <vector>
#include "animation/bone.h"

enum aiTextureType;

struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiScene;

namespace geeL {

	enum class ColorType;
	enum class CullingMode;
	enum class MapType;
	enum class ShadingMethod;

	struct SkinnedVertex;
	struct MeshBoneData;

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
	

	class MeshFactory {

	public:
		MeshFactory(MaterialFactory& factory);
		~MeshFactory();

		//Create new mesh renderer with default shading
		StaticMeshRenderer& CreateMeshRenderer(StaticModel& model, Transform& transform,
			CullingMode faceCulling, const std::string& name = "MeshRenderer");

		StaticMeshRenderer& CreateMeshRenderer(StaticModel& model, SceneShader& shader, Transform& transform,
			CullingMode faceCulling, const std::string& name = "MeshRenderer");

		//Create separate mesh renderers for every mesh in given model
		std::list<StaticMeshRenderer*> CreateMeshRenderers(StaticModel& model, SceneShader& shader, Transform& transform,
			CullingMode faceCulling);

		//Create new skinned mesh renderer with default shading
		SkinnedMeshRenderer& CreateSkinnedMeshRenderer(SkinnedModel& model, Transform& transform,
			CullingMode faceCulling, const std::string& name = "SkinnedMeshRenderer");

		SkinnedMeshRenderer& CreateSkinnedMeshRenderer(SkinnedModel& model, SceneShader& shader, Transform& transform,
			CullingMode faceCulling, const std::string& name = "SkinnedMeshRenderer");

		//Creates, initializes and returns a new static model from given file path or 
		//returns an existing model if it already uses this file
		StaticModel& CreateStaticModel(std::string filePath);

		//Creates, initializes and returns a new static model from given file path or 
		//returns an existing model if it already uses this file
		SkinnedModel& CreateSkinnedModel(std::string filePath);

	private:
		MaterialFactory& factory;
		std::map<std::string, StaticModel> staticModels;
		std::map<std::string, SkinnedModel> skinnedModels;
		std::list<MeshRenderer*> meshRenderer;

		void fillStaticModel(StaticModel& model, std::string path);
		void processStaticNode(StaticModel& model, std::string directory, aiNode* node, const aiScene* scene);

		void fillSkinnedModel(SkinnedModel& model, std::string path);
		void processSkinnedNode(SkinnedModel& model, Bone& bone, std::string directory, aiNode* node, const aiScene* scene);

		template<class V>
		void processVertices(std::vector<V>& vertices, aiMesh* mesh);
		void processIndices(std::vector<unsigned int>& indices, aiMesh* mesh);
		void processBones(std::vector<SkinnedVertex>& vertices, std::map<std::string, MeshBone>& bones, aiMesh* mesh);

		void processMaterial(DefaultMaterialContainer& material, aiMesh* mesh, const aiScene* scene);
		void processTextures(std::vector<TextureMap*>& textures, std::string directory, aiMesh* mesh, const aiScene* scene);
		void processAnimations(SkinnedModel& model, const aiScene* scene);
		void processSkeleton(Bone& bone, aiNode* node);

		void loadMaterialTextures(std::vector<TextureMap*>& textures, aiMaterial* mat,
			aiTextureType aiType, MapType type, std::string directory, ColorType colorType);

	};
}

#endif 
