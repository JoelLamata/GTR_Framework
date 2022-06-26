#pragma once
#include "prefab.h"
#include "sphericalharmonics.h"
#include "mesh.h"

//forward declarations
class Camera;
class Shader;

using namespace std;

struct sProbe {
	Vector3 pos; //where is located
	Vector3 local; //its ijk pos in the matrix
	int index; //its index in the linear array
	SphericalHarmonics sh; //coeffs
};

struct sReflectionProbe {
	Vector3 pos;
	Texture* cubemap = NULL;
};

struct sIrrHeader {
	Vector3 start;
	Vector3 end;
	Vector3 delta;
	Vector3 dims;
	int num_probes;
};

namespace GTR {
	class Prefab;
	class Material;
	
	class RenderCall {
	public:
		Material* material;
		Mesh* mesh;
		BoundingBox world_bounding;
		Matrix44 model;

		float distance_to_camera = 0;

		bool operator > (const RenderCall& str) const
		{
			if (material->alpha_mode == eAlphaMode::BLEND)
				if (str.material->alpha_mode == eAlphaMode::BLEND)
					return (distance_to_camera > str.distance_to_camera);
				else
					return false;
			else
				if (str.material->alpha_mode == eAlphaMode::BLEND)
					return true;
				else
					return (distance_to_camera > str.distance_to_camera);
		}
	};

	// This class is in charge of rendering anything in our system.
	// Separating the render from anything else makes the code cleaner
	class Renderer
	{

	public:
		
		enum ePipeline{
			FORWARD,
			DEFERRED
		};
		enum eRenderShape {
			QUAD,
			GEOMETRY
		};
		enum ePipelineSpace {
			LINEAR = 0,
			GAMMA = 1
		};
		enum eDynamicRange {
			SDR = 0,
			HDR = 1
		};

		std::vector<GTR::LightEntity*> lights;
		std::vector<RenderCall> render_calls;
		std::vector<GTR::DecalEntity*> decals;
		
		ePipeline pipeline;
		eRenderShape renderShape;
		ePipelineSpace pipelineSpace;
		eDynamicRange dynamicRange;
		FBO* gbuffers_fbo;
		FBO* illumination_fbo;
		FBO* ssao_fbo;
		FBO* irr_fbo;
		FBO* reflection_fbo;
		FBO* reflection_probe_fbo;
		FBO* decal_fbo;
		Texture* ssao_blur;
		Texture* probes_texture;
		Texture* postFX_textureA;
		Texture* postFX_textureB;
		bool multipass;
		bool show_gbuffers;
		bool show_ssao;
		bool show_probes;
		bool show_probes_texture;
		bool show_irradiance;
		bool show_reflections;
		bool show_decal;
		bool is_rendering_reflections;
		bool show_chrab_lensdist;
		bool show_motblur;
		bool show_antial;
		bool show_DoF;
		vector<Vector3> random_points;
		Vector3 start_irr;
		Vector3 end_irr;
		Vector3 dim_irr;
		Vector3 delta;
		vector<sProbe> probes;
		vector<sReflectionProbe*> reflection_probes;
		Texture* skybox;
		Texture* cloned_depth_texture;
		Texture* blur;
		Mesh cube;
		Matrix44 viewproj_old;
		float deb_fac;

		Renderer();
		//add here your functions
		void renderSkybox(Camera* camera);
		void renderForward(Camera* camera, GTR::Scene* scene);
		void renderDeferred(Camera* camera, GTR::Scene* scene);

		//renders several elements of the scene
		void renderScene(GTR::Scene* scene, Camera* camera);
	
		//to render a whole prefab (with all its nodes)
		void renderPrefab(const Matrix44& model, GTR::Prefab* prefab, Camera* camera);

		//to render one node from the prefab and its children
		void renderNode(const Matrix44& model, GTR::Node* node, Camera* camera);

		void placeAndGenerateProbes(GTR::Scene* scene);
		void renderProbe(Vector3 pos, float size, float* coeffs);
		void captureProbe(sProbe& probe, GTR::Scene* scene);
		bool loadProbes();
		void uploadProbesToGPU();

		void renderReflectionProbes(GTR::Scene* scene, Camera* camera);
		void updateReflectionProbes(GTR::Scene* scene);
		void captureReflectionProbe(GTR::Scene* scene, Texture* tex, Vector3 pos);

		//to render one mesh given its material and transformation matrix
		void renderMeshWithMaterialToGBuffers(const Matrix44 model, Mesh* mesh, GTR::Material* material, Camera* camera);
		void renderMeshWithMaterialAndLighting(const Matrix44 model, Mesh* mesh, GTR::Material* material, Camera* camera);

		void uploadLightToShaderMultipass(LightEntity* light, Shader* shader);
		void uploadLightToShaderSinglepass(Shader* shader);
		void uploadLightToShaderDeferred(Shader* shader, Matrix44 inv_vp, int width, int height, Camera* camera);
	
		void renderFlatMesh(const Matrix44 model, Mesh* mesh, GTR::Material* material, Camera* camera);
		void generateShadowmap(LightEntity* light);
		void showShadowmap(LightEntity* light);

		void uploadUniformsAndTextures(Shader* shader, GTR::Material* material, Camera* camera, const Matrix44 model);

		void applyfx(Texture* color, Texture* depth, Camera* camera);
	};

	vector<Vector3> generateSpherePoints(int num, float radius, bool hemi);

	Texture* CubemapFromHDRE(const char* filename);
};