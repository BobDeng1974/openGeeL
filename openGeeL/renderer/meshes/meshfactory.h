#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <map>
#include <string>
#include <list>

enum   aiTextureType;
struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiScene;

namespace geeL {

	class MaterialFactory;
	class Model;
	class MeshRenderer;
	class StaticMesh;
	class SkinnedMesh;
	enum CullingMode;

	class MeshFactory {

	public:

		MeshFactory(MaterialFactory& factory);

		//Creates, initializes and returns a new model from given file path or 
		//returns an existing model if it already uses this file
		Model& CreateModel(std::string filePath);

		MeshRenderer& CreateMeshRenderer(Model& model, Transform& transform, 
			CullingMode faceCulling, std::string name = "MeshRenderer");

		//Create mesh renderer on heap. Caller has to manage its memory manually
		MeshRenderer* CreateMeshRendererManual(Model& model, Transform& transform, 
			CullingMode faceCulling, bool deferred = true, std::string name = "MeshRenderer");

		std::map<std::string, Model>::iterator modelsBegin();
		std::map<std::string, Model>::iterator modelsEnd();

		std::list<MeshRenderer>::iterator rendererBegin();
		std::list<MeshRenderer>::iterator rendererEnd();

	private:
		MaterialFactory& factory;
		std::map<std::string, Model> models;
		std::list<MeshRenderer> meshRenderer;

		void fillModel(Model& model, std::string path);
		void processNode(Model& model, std::string directory, aiNode* node, const aiScene* scene);

		template<class V>
		void processVertices(std::vector<V>& vertices, aiMesh* mesh);
		void processIndices(std::vector<unsigned int>& indices, aiMesh* mesh);
		void processBones(std::vector<SkinnedVertex>& vertices, std::map<std::string, Bone>& bones, aiMesh* mesh);
		void processTextures(std::vector<SimpleTexture*>& textures, std::string directory, aiMesh* mesh, const aiScene* scene);

		StaticMesh* processStaticMesh(std::string directory, aiMesh* mesh, const aiScene* scene);
		SkinnedMesh* processSkinnedMesh(std::string directory, aiMesh* mesh, const aiScene* scene);

		void loadMaterialTextures(std::vector<SimpleTexture*>& textures, aiMaterial* mat,
			aiTextureType aiType, TextureType type, std::string directory, bool linear);

	};
}

#endif 
