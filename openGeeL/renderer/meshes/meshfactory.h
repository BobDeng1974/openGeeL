#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <map>
#include <string>
#include <list>
#include <vector>

enum aiTextureType;

struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiScene;

namespace geeL {

	enum CullingMode;
	enum class DefaultShading;

	struct SkinnedVertex;
	struct MeshBoneData;

	class Animation;
	class MaterialFactory;
	class MeshRenderer;
	class Model;
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

		//Create new mesh renderer with default shading
		MeshRenderer& CreateMeshRenderer(StaticModel& model, Transform& transform, DefaultShading shading,
			CullingMode faceCulling, std::string name = "MeshRenderer");

		//Create mesh renderer on heap. Caller has to manage its memory manually
		MeshRenderer* CreateMeshRendererManual(StaticModel& model, Transform& transform,
			CullingMode faceCulling, bool deferred = true, std::string name = "MeshRenderer");

		MeshRenderer& CreateSkinnedMeshRenderer(SkinnedModel& model, Transform& transform, DefaultShading shading,
			CullingMode faceCulling, std::string name = "SkinnedMeshRenderer");

		//Create skinned mesh renderer on heap. Caller has to manage its memory manually
		SkinnedMeshRenderer* CreateSkinnedMeshRendererManual(SkinnedModel& model, Transform& transform,
			CullingMode faceCulling, bool deferred = true, std::string name = "SkinnedMeshRenderer");

		//Creates, initializes and returns a new static model from given file path or 
		//returns an existing model if it already uses this file
		StaticModel& CreateStaticModel(std::string filePath);

		//Creates, initializes and returns a new static model from given file path or 
		//returns an existing model if it already uses this file
		SkinnedModel& CreateSkinnedModel(std::string filePath);

		std::map<std::string, StaticModel>::iterator modelsBegin();
		std::map<std::string, StaticModel>::iterator modelsEnd();

		std::list<MeshRenderer>::iterator rendererBegin();
		std::list<MeshRenderer>::iterator rendererEnd();

	private:
		MaterialFactory& factory;
		std::map<std::string, StaticModel> staticModels;
		std::map<std::string, SkinnedModel> skinnedModels;
		std::list<MeshRenderer> meshRenderer;

		void fillStaticModel(StaticModel& model, std::string path);
		void processStaticNode(StaticModel& model, std::string directory, aiNode* node, const aiScene* scene);

		void fillSkinnedModel(SkinnedModel& model, std::string path);
		void processSkinnedNode(SkinnedModel& model, Transform& bone, std::string directory, aiNode* node, const aiScene* scene);

		template<class V>
		void processVertices(std::vector<V>& vertices, aiMesh* mesh);
		void processIndices(std::vector<unsigned int>& indices, aiMesh* mesh);
		void processBones(std::vector<SkinnedVertex>& vertices, std::map<std::string, MeshBoneData>& bones, aiMesh* mesh);
		void processTextures(std::vector<TextureMap*>& textures, std::string directory, aiMesh* mesh, const aiScene* scene);
		void processAnimations(SkinnedModel& model, const aiScene* scene);


		void loadMaterialTextures(std::vector<TextureMap*>& textures, aiMaterial* mat,
			aiTextureType aiType, TextureType type, std::string directory, bool linear);

	};
}

#endif 
