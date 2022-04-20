#pragma once
#include "prefab.h"

//forward declarations
class Camera;

namespace GTR {

	class Prefab;
	class Material;
	
	// This class is in charge of rendering anything in our system.
	// Separating the render from anything else makes the code cleaner
	class Renderer
	{

	public:

		std::vector<GTR::LightEntity*> lights;
		std::vector<GTR::RenderCall> render_calls;

		//add here your functions
		//...

		//renders several elements of the scene
		void renderScene(GTR::Scene* scene, Camera* camera);
	
		//to render a whole prefab (with all its nodes)
		void renderPrefab(const Matrix44& model, GTR::Prefab* prefab, Camera* camera);

		//to render one node from the prefab and its children
		void renderNode(const Matrix44& model, GTR::Node* node, Camera* camera);

		//to render one mesh given its material and transformation matrix
		void renderMeshWithMaterial(const Matrix44 model, Mesh* mesh, GTR::Material* material, Camera* camera);
	};

	Texture* CubemapFromHDRE(const char* filename);

	class RenderCall {
	public:
		PrefabEntity* prefab;
		Matrix44 model;

		float distance_to_camera = 0;

		bool operator > (const RenderCall& str) const
		{
			return (distance_to_camera > str.distance_to_camera);
		}
	};
};