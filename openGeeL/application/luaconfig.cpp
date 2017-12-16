#include <iostream>
#include <list>
#include <memory>
#include "../application/scripting/luacomp.h"
#include "../application/configuration.h"
#include "luaconfig.h"

using namespace glm;
using namespace std;

namespace geeL {

	LUAConfigurator::LUAConfigurator(const std::string& filePath) {
		try {
			state.script_file(filePath);
		}
		catch (const sol::error& e) {
			throw e;
		}
	}

	void LUAConfigurator::run() {

		//Window initalization

		unsigned int width  = state.get_or<unsigned int>("width", 1920);
		unsigned int height = state.get_or<unsigned int>("height", 1080);
		const string& name = state.get_or<string>("name", "geeL");

		const string& modeName = state.get_or<string>("mode", "Windowed");
		WindowMode mode = WindowMode::Windowed;
		if (modeName == "Fullscreen")
			mode = WindowMode::Fullscreen;
		else if (modeName == "ResizableWindow")
			mode = WindowMode::ResizableWindow;
		else if (modeName == "BorderlessWindow")
			mode = WindowMode::BorderlessWindow;

		RenderWindow& window = RenderWindow(name.c_str(), Resolution(width, height), mode);


		auto init = [&window, this](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {


			//Camera initalization
			PerspectiveCamera* camera = nullptr;

			{
				auto& cam = state["camera"];
				if (cam.valid()) {

					float x = cam["position"]["x"].get_or(0.f);
					float y = cam["position"]["y"].get_or(0.f);
					float z = cam["position"]["z"].get_or(0.f);
					vec3 position(x, y, z);

					x = cam["rotation"]["x"].get_or(0.f);
					y = cam["rotation"]["y"].get_or(0.f);
					z = cam["rotation"]["z"].get_or(0.f);
					vec3 rotation(x, y, z);

					float fov = cam["fov"].get_or(60.f);
					float far = cam["far"].get_or(0.01f);
					float near = cam["near"].get_or(100.f);

					Transform& cameraTransform = transformFactory.CreateTransform(position, rotation, vec3(1.f));
					camera = new PerspectiveCamera(cameraTransform, fov, window.getWidth(), window.getHeight(), near, far);
					scene.setCamera(*camera);


					//Add scripts as components

					bool movable = cam["movable"].get_or(false);
					if (movable) {
						float speed = cam["speed"].get_or(1.f);
						float sensi = cam["sensitivity"].get_or(1.f);

						camera->addComponent<MovableCamera>(MovableCamera(speed, sensi));
					}

					auto& scriptsInit = cam["scripts"];
					if (scriptsInit.valid()) {

						unsigned int j = 1;
						auto* script = &scriptsInit[j];
						while (script->valid()) {
							auto& s = *script;

							auto& path = s["path"];
							assert(path.valid() && "No path specified for given script");
							string filePath = path;

							try {
								LUAComponent& component = camera->addComponent<LUAComponent>(filePath);

								auto& parameters = s["parameters"];
								if (parameters.valid()) {
									component.injectParameters([&parameters](sol::state& state) {

										unsigned int h = 1;
										auto* parameter = &parameters[h];
										while (parameter->valid()) {
											auto& p = *parameter;

											auto& name = p["name"];
											auto& val  = p["value"];
											assert(name.valid() && "Given parameter has no name");
											assert(val.valid()  && "Given parameter has no value");

											state[name] = val;

											h++;
											parameter = &parameters[h];
										}
									});
								}
							}
							catch (const exception& e) {
								std::cout << "Script won't be added because it terminated with error:\n'"
									<< e.what() << "'\n";
							}

							j++;
							script = &scriptsInit[j];
						}
					}
				}
			}


			//Skybox initalization
			Skybox* skybox = nullptr;
			
			{
				auto& sky = state["skybox"];
				if (sky.valid()) {

					auto& path = sky["path"];
					assert(path.valid() && "No path specified for given skybox");

					unsigned int resolution = sky["resolution"].get_or(512);
					bool useIBL = sky["useIBL"].get_or(false);
					bool drawIrradiance = sky["drawIrradiance"].get_or(false);

					const string& filePath = path;
					EnvironmentCubeMap& envCubeMap = cubeMapFactory.createEnvironmentCubeMap(filePath, resolution);

					if (useIBL) {
						IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

						if (drawIrradiance)
							skybox = new Skybox(iblMap.getIrradianceMap());
						else
							skybox = new Skybox(envCubeMap);

						lightManager.addReflectionProbe(iblMap);
					}
					else
						skybox = new Skybox(envCubeMap);

					scene.setSkybox(*skybox);
				}
			}


			//Reflection probes initalization

			{
				auto& probes = state["reflectionprobes"];
				if (probes.valid()) {

					unsigned int i = 1;
					auto* pro = &probes[i];

					while (pro->valid()) {
						auto& p = *pro;

						float x = p["position"]["x"].get_or(0.f);
						float y = p["position"]["y"].get_or(0.f);
						float z = p["position"]["z"].get_or(0.f);
						vec3 position(x, y, z);

						x = p["rotation"]["x"].get_or(0.f);
						y = p["rotation"]["y"].get_or(0.f);
						z = p["rotation"]["z"].get_or(0.f);
						vec3 rotation(x, y, z);

						unsigned int width = p["dimensions"]["width"].get_or(10);
						unsigned int height = p["dimensions"]["height"].get_or(10);
						unsigned int depth = p["dimensions"]["depth"].get_or(10);
						unsigned int resolution = p["resolution"].get_or(512);

						Transform& probeTransform = transformFactory.CreateTransform(position, rotation, vec3(1.f));
						DynamicIBLMap& probe = cubeMapFactory.createReflectionProbeIBL(probeTransform,
							resolution, width, height, depth);

						lightManager.addReflectionProbe(probe);

						i++;
						pro = &probes[i];
					}
				}
			}


			//Mesh renderer initalization

			{
				auto& meshes = state["objects"];
				if (meshes.valid()) {

					unsigned int i = 1;
					auto* mesh = &meshes[i];

					while (mesh->valid()) {
						auto& m = *mesh;

						float x = m["position"]["x"].get_or(0.f);
						float y = m["position"]["y"].get_or(0.f);
						float z = m["position"]["z"].get_or(0.f);
						vec3 position(x, y, z);

						x = m["rotation"]["x"].get_or(0.f);
						y = m["rotation"]["y"].get_or(0.f);
						z = m["rotation"]["z"].get_or(0.f);
						vec3 rotation(x, y, z);

						x = m["scale"]["x"].get_or(1.f);
						y = m["scale"]["y"].get_or(1.f);
						z = m["scale"]["z"].get_or(1.f);
						vec3 scale(x, y, z);

						Transform& meshTransform = transformFactory.CreateTransform(position, rotation, scale);

						auto& path = m["path"];
						assert(path.valid() && "No path specified for given mesh renderer");
						const string& filePath = path;
						const string& name = m["name"].get_or<string>("Mesh");

						bool separate = m["separate"].get_or(false);

						//Build meshe renderers
						
						std::list<MeshRenderer*> meshRenderers;
						if (separate) {
							std::list<StaticMeshRenderer*> renderers = std::move(meshFactory.createMeshRenderers(
								meshFactory.createStaticModel(filePath),
								materialFactory.getDeferredShader(),
								meshTransform));

							for (auto it(renderers.begin()); it != renderers.end(); it++) {
								MeshRenderer* rendererPtr = *it;

								MeshRenderer& renderer = scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(rendererPtr));
								meshRenderers.push_back(&renderer);
							}
						}
						else {
							std::unique_ptr<MeshRenderer> rendererPtr = meshFactory.createMeshRenderer(
								meshFactory.createStaticModel(filePath),
								meshTransform, name);

							MeshRenderer& meshRenderer = scene.addMeshRenderer(std::move(rendererPtr));
							meshRenderers.push_back(&meshRenderer);
						}
						

						//Iterate through all materials

						//Set material options for all meshes (if specified)

						auto& materialsInit = m["allmaterials"];
						if (materialsInit.valid()) {
							for (auto it(meshRenderers.begin()); it != meshRenderers.end(); it++) {
								MeshRenderer& renderer = **it;

								renderer.iterateMaterials([&](MaterialContainer& container) {
									auto& roughness = materialsInit["roughness"];
									if (roughness.valid())
										container.setFloatValue("Roughness", roughness);

									auto& metallic = materialsInit["metallic"];
									if (metallic.valid())
										container.setFloatValue("Metallic", metallic);

									auto& transparency = materialsInit["transparency"];
									if (transparency.valid())
										container.setFloatValue("Transparency", transparency);

									auto& emissivity = materialsInit["emissivity"];
									if (emissivity.valid())
										container.setFloatValue("Emissivity", transparency);

								});
							}
						}


						//Set material options for specific meshes
						
						auto& materialsInit2 = m["materials"];
						if (materialsInit2.valid()) {
							for (auto it(meshRenderers.begin()); it != meshRenderers.end(); it++) {
								MeshRenderer& renderer = **it;

								renderer.iterateMaterials([&](MaterialContainer& container) {

									unsigned int j = 1;
									auto* material = &materialsInit2[j];
									while (material->valid()) {
										auto& mat = *material;

										auto& name = mat["name"];
										assert(name.valid() && "Given material has no name");

										if (name == container.name) {
											auto& texturesInit = mat["textures"];
											if (texturesInit.valid()) {

												unsigned int k = 1;
												auto* texture = &texturesInit[k];
												while (texture->valid()) {
													auto& tex = *texture;

													auto& path = tex["path"];
													auto& type = tex["type"];
													assert(path.valid() && "No path specified for given texture");
													assert(type.valid() && "No type specified for given texture");

													container.addTexture(type, 
														materialFactory.createTexture(path, ColorType::GammaSpace));

													k++;
													texture = &texturesInit[k];
												}
											}


											auto& x = mat["color"]["r"];
											auto& y = mat["color"]["g"];
											auto& z = mat["color"]["b"];

											if (x.valid() && y.valid() && z.valid()) {
												vec3 color(x, y, z);
												container.setVectorValue("Color", color);
											}

											auto& roughness = mat["roughness"];
											if (roughness.valid())
												container.setFloatValue("Roughness", roughness);

											auto& metallic = mat["metallic"];
											if (metallic.valid())
												container.setFloatValue("Metallic", metallic);

											auto& transparency = mat["transparency"];
											if (transparency.valid())
												container.setFloatValue("Transparency", transparency);

											auto& emissivity = mat["emissivity"];
											if (emissivity.valid())
												container.setFloatValue("Emissivity", transparency);

										}

										j++;
										material = &materialsInit2[j];
									}
								});
							}
						}

						//Iterate individual meshes

						auto& meshesInit = m["meshes"];
						if (meshesInit.valid()) {
							for (auto it(meshRenderers.begin()); it != meshRenderers.end(); it++) {
								MeshRenderer& renderer = **it;

								//Look for mesh renderer mask

								auto& maskInit = m["mask"];
								if (maskInit.valid()) {
									string maskName = maskInit;
									RenderMask mask = Masking::getShadingMask(maskName);

									renderer.setRenderMask(mask);
								}


								//Set material options for specific meshes

								unsigned int j = 1;
								auto* mesh2 = &meshesInit[j];
								while (mesh2->valid()) {
									auto& mm = *mesh2;

									auto& name = mm["name"];
									assert(name.valid() && "Given mesh has no name");

									const MeshInstance* currentMesh = renderer.getMesh(name);
									if (currentMesh != nullptr) {

										auto& maskInit = mm["mask"];
										if (maskInit.valid()) {
											string maskName = maskInit;
											RenderMask mask = Masking::getShadingMask(maskName);

											renderer.setRenderMask(mask, *currentMesh);
										}

										auto& methodInit = mm["method"];
										if (methodInit.valid()) {
											string methodName = methodInit;
											ShadingMethod method = getShadingMethod(methodName);

											SceneShader& ss = materialFactory.getDefaultShader(method);
											renderer.changeMaterial(ss, *currentMesh);
										}
									}

									j++;
									mesh2 = &meshesInit[j];
								}
							}
						}

						//Add scripts as components

						auto& scriptsInit = m["scripts"];
						if (scriptsInit.valid()) {
							for (auto it(meshRenderers.begin()); it != meshRenderers.end(); it++) {
								MeshRenderer& renderer = **it;

								unsigned int j = 1;
								auto* script = &scriptsInit[j];
								while (script->valid()) {
									auto& s = *script;

									auto& path = s["path"];
									assert(path.valid() && "No path specified for given script");
									string filePath = path;

									try {
										LUAComponent& component = renderer.addComponent<LUAComponent>(filePath);

										auto& parameters = s["parameters"];
										if (parameters.valid()) {
											component.injectParameters([&parameters](sol::state& state) {

												unsigned int h = 1;
												auto* parameter = &parameters[h];
												while (parameter->valid()) {
													auto& p = *parameter;

													auto& name = p["name"];
													auto& val  = p["value"];
													assert(name.valid() && "Given parameter has no name");
													assert(val.valid()  && "Given parameter has no value");

													state[name] = val;

													h++;
													parameter = &parameters[h];
												}
											});
										}
									}
									catch (const exception& e) {
										std::cout << "Script won't be added because it terminated with error:\n'" 
											<< e.what() << "'\n";
									}

									j++;
									script = &scriptsInit[j];
								}
							}
						}

						i++;
						mesh = &meshes[i];
					}
				}
			}
			

			//Light initalization

			{
				auto& lights = state["lights"];
				if (lights.valid()) {

					unsigned int i = 1;
					auto* light = &lights[i];

					while (light->valid()) {
						auto& l = *light;

						float x = l["position"]["x"].get_or(0.f);
						float y = l["position"]["y"].get_or(0.f);
						float z = l["position"]["z"].get_or(0.f);
						vec3 position(x, y, z);

						x = l["rotation"]["x"].get_or(0.f);
						y = l["rotation"]["y"].get_or(0.f);
						z = l["rotation"]["z"].get_or(0.f);
						vec3 rotation(x, y, z);

						x = l["color"]["r"].get_or(0.f);
						y = l["color"]["g"].get_or(0.f);
						z = l["color"]["b"].get_or(0.f);
						vec3 color(x, y, z);

						float intensity = l["intensity"].get_or(1.f);

						ShadowMapConfiguration config = noShadowMapConfig;
						bool useShadowmap = l["useShadowmap"].get_or(false);
						if (useShadowmap) {
							float shadowBias = l["shadowBias"].get_or(0.f);
							float shadowRes = l["softShadowResolution"].get_or(1.f);

							const string& typeString = l["shadowmapType"].get_or<string>("Hard");
							ShadowMapType type = (typeString == "Hard") ? ShadowMapType::Hard : ShadowMapType::Soft;

							const string& resString = l["shadowResolution"].get_or<string>("Medium");
							ShadowmapResolution resolution = ShadowmapResolution::Medium;

							if (resString == "Tiny")
								resolution = ShadowmapResolution::Tiny;
							else if (resString == "Small")
								resolution = ShadowmapResolution::Small;
							else if (resString == "High")
								resolution = ShadowmapResolution::High;
							else if (resString == "VeryHigh")
								resolution = ShadowmapResolution::VeryHigh;
							else if (resString == "Huge")
								resolution = ShadowmapResolution::Huge;

							config = ShadowMapConfiguration(shadowBias, type, resolution, 2.f, shadowRes);
						}

						Transform& lightTransform = transformFactory.CreateTransform(position, rotation, vec3(1.f));

						auto& type = l["type"];
						assert(type.valid() && "No type specified for given light");
						const string& typeString = type;

						Light* lightSource = nullptr;
						if (typeString == "Spot") {
							float angle = l["angle"].get_or(glm::cos(glm::radians(25.5f)));
							float outerAngle = l["outerAngle"].get_or(glm::cos(glm::radians(27.5f)));

							lightSource = &lightManager.addSpotlight(lightTransform, color * intensity, angle, outerAngle, config);
						}
						else if (typeString == "Directional")
							lightSource = &lightManager.addDirectionalLight(*camera, lightTransform, color * intensity, config);
						else if (typeString == "Point")
							lightSource = &lightManager.addPointLight(lightTransform, color * intensity, config);
						else
							std::cout << "Valid light types : { Point, Spot, Directional }\n";


						//Add scripts as components

						auto& scriptsInit = l["scripts"];
						if (scriptsInit.valid() && light != nullptr) {

								unsigned int j = 1;
								auto* script = &scriptsInit[j];
								while (script->valid()) {
									auto& s = *script;

									auto& path = s["path"];
									assert(path.valid() && "No path specified for given script");
									string filePath = path;

									try {
										LUAComponent& component = lightSource->addComponent<LUAComponent>(filePath);

										auto& parameters = s["parameters"];
										if (parameters.valid()) {
											component.injectParameters([&parameters](sol::state& state) {

												unsigned int h = 1;
												auto* parameter = &parameters[h];
												while (parameter->valid()) {
													auto& p = *parameter;

													auto& name = p["name"];
													auto& val  = p["value"];
													assert(name.valid() && "Given parameter has no name");
													assert(val.valid()  && "Given parameter has no value");

													state[name] = val;

													h++;
													parameter = &parameters[h];
												}
											});
										}
									}
									catch (const exception& e) {
										std::cout << "Script won't be added because it terminated with error:\n'"
											<< e.what() << "'\n";
									}

									j++;
									script = &scriptsInit[j];
								}
						}

						i++;
						light = &lights[i];
					}
				}
			}
			
			
			

			//GUI initialization
			ObjectLister* objectLister = nullptr;
			PostProcessingEffectLister* postLister = nullptr;
			
			{
				auto& guiInit = state["gui"];
				if (guiInit.valid()) {

					bool showObjects = guiInit["showObjects"].get_or(false);
					bool showEffects = guiInit["showEffects"].get_or(false);
					bool showSystem = guiInit["showSystem"].get_or(false);

					if (showObjects || showEffects) {
						unique_ptr<GUILister> guiLister(new GUILister(window, 0.01f, 0.15f, 0.17f, 0.5f));

						if (showObjects) {
							objectLister = new ObjectLister(scene);
							objectLister->add(*camera);
							guiLister->add(*objectLister);
						}

						if (showEffects) {
							postLister = new PostProcessingEffectLister();
							guiLister->add(*postLister);
						}

						gui.addElement<GUILister>(std::move(guiLister));
					}

					if (showSystem)
						gui.addSystemInformation(0.01f, 0.655f, 0.17f, 0.14f);

				}
			}
			


			//Post processing initialization
			list<PostProcessingEffect*> effects;

			{
				float exposure = state["exposure"].get_or(1.f);
				def.setExposure(exposure);
				if (postLister) postLister->add(def);

				auto& ssaoInit = state["ssao"];
				if (ssaoInit.valid()) {

					float radius = ssaoInit["radius"].get_or(1.f);
					float blurSigma = ssaoInit["blurSigma"].get_or(5.f);
					float blurFactor = ssaoInit["blurFactor"].get_or(5.f);
					float resolution = ssaoInit["resolution"].get_or(1.f);

					BilateralFilter* blur = new BilateralFilter(blurSigma, 7, blurFactor);
					SSAO* ssao = new SSAO(*blur, radius, getRenderResolution(ResolutionScale(resolution)));
					renderer.addEffect(*ssao);
					scene.addRequester(*ssao);

					if (postLister) {
						unique_ptr<PostEffectSnippet> ssaoSnippet(new SSAOSnippet(*ssao));
						unique_ptr<PostEffectSnippet> blurSnippet(new BilateralFilterSnippet(*blur));
						unique_ptr<PostEffectSnippet> groupSnippet(new PostGroupSnippet(*ssaoSnippet.get(), *blurSnippet.get()));

						postLister->add(std::move(groupSnippet), std::move(ssaoSnippet), std::move(blurSnippet));
					}

					effects.push_back(blur);
					effects.push_back(ssao);
				}


				auto& iblInit = state["ibl"];
				if (iblInit.valid()) {
					ImageBasedLighting* ibl = new ImageBasedLighting(scene);
					renderer.addEffect(*ibl);
					effects.push_back(ibl);

					if (postLister) {
						unique_ptr<PostEffectSnippet> snippet(new GenericPostSnippet(*ibl));
						postLister->add(snippet);
					}
				}

				auto& bloomInit = state["bloom"];
				if (bloomInit.valid()) {
					float scatter = bloomInit["scatter"].get_or(1.f);
					float blurSigma = bloomInit["blurSigma"].get_or(5.f);
					float resolution = bloomInit["resolution"].get_or(1.f);
					ResolutionPreset resPreset = getRenderResolution(ResolutionScale(resolution));

					BrightnessFilterCutoff* filter = new BrightnessFilterCutoff(scatter);
					GaussianBlur* blur = new GaussianBlur(blurSigma, 17);
					Bloom* bloom = new Bloom(*filter, *blur, resPreset, resPreset);
					renderer.addEffect(*bloom, DrawTime::Late);

					if (postLister) postLister->add(*bloom);

					effects.push_back(filter);
					effects.push_back(blur);
					effects.push_back(bloom);
				}


				auto& ssrrInit = state["ssrr"];
				if (ssrrInit.valid()) {

					unsigned int stepCount = ssrrInit["stepCount"].get_or(60);
					float stepSize = ssrrInit["stepSize"].get_or(0.2f);
					float stepGain = ssrrInit["stepGain"].get_or(1.02f);

					float blurSigma = ssrrInit["blurSigma"].get_or(5.f);
					float resolution = ssrrInit["resolution"].get_or(1.f);
					ResolutionPreset resPreset = getRenderResolution(ResolutionScale(resolution));

					GaussianBlur* blur = new GaussianBlur(blurSigma, 5);
					SSRR* ssrr = new SSRR(stepCount, stepSize, stepGain);
					BlurredPostEffect* ssrrSmooth = new BlurredPostEffect(*ssrr, *blur, resPreset, resPreset);

					renderer.addEffect(*ssrrSmooth);
					scene.addRequester(*ssrr);

					if (postLister) {
						unique_ptr<PostEffectSnippet> ssrrSnippet(new SSRRSnippet(*ssrr));
						unique_ptr<PostEffectSnippet> blurSnippet(new GaussianBlurSnippet(*blur));

						postLister->add(*ssrrSmooth, std::move(ssrrSnippet), std::move(blurSnippet));
					}

					effects.push_back(blur);
					effects.push_back(ssrr);
					effects.push_back(ssrrSmooth);
				}


				auto& dofInit = state["dof"];
				if (dofInit.valid()) {

					float aperture = dofInit["aperture"].get_or(10.f);
					float focalLength = dofInit["focalLength"].get_or(10.f);
					float blurSigma = dofInit["blurSigma"].get_or(2.f);
					float resolution = dofInit["resolution"].get_or(0.5f);
					ResolutionPreset resPreset = getRenderResolution(ResolutionScale(resolution));

					DepthOfFieldBlur* blur = new DepthOfFieldBlur(0.1f, blurSigma);
					DepthOfFieldBlurred* dof = new DepthOfFieldBlurred(*blur, focalLength, aperture, 100.f, resPreset);
					renderer.addEffect(*dof);

					if (postLister) postLister->add(*dof);

					effects.push_back(blur);
					effects.push_back(dof);
				}


				auto& godInit = state["godray"];
				if (godInit.valid()) {

					float x = godInit["position"]["x"].get_or(0.f);
					float y = godInit["position"]["y"].get_or(0.f);
					float z = godInit["position"]["z"].get_or(0.f);
					vec3 position(x, y, z);

					unsigned int sampleCount = godInit["sampleCount"].get_or(20);
					float blurSigma = godInit["blurSigma"].get_or(5.f);
					float blurFactor = godInit["blurFactor"].get_or(5.f);
					float resolution = godInit["resolution"].get_or(1.f);
					ResolutionPreset resPreset = getRenderResolution(ResolutionScale(resolution));

					BilateralFilter* blur = new BilateralFilter(blurSigma, 7, blurFactor);
					GodRay* ray = new GodRay(position, sampleCount);
					BlurredPostEffect* raySmooth = new BlurredPostEffect(*ray, *blur, resPreset, resPreset);
					renderer.addEffect(*raySmooth, DrawTime::Late);
					scene.addRequester(*ray);

					if (postLister) {
						unique_ptr<PostEffectSnippet> raySnippet(new GodRaySnippet(*ray));
						unique_ptr<PostEffectSnippet> blurSnippet(new BilateralFilterSnippet(*blur));

						postLister->add(*raySmooth, std::move(raySnippet), std::move(blurSnippet));
					}

					effects.push_back(blur);
					effects.push_back(ray);
					effects.push_back(raySmooth);
				}


				auto& colorInit = state["colorcorrect"];
				if (colorInit.valid()) {

					float r = colorInit["r"].get_or(1.f);
					float g = colorInit["g"].get_or(1.f);
					float b = colorInit["b"].get_or(1.f);
					float h = colorInit["hue"].get_or(1.f);
					float s = colorInit["saturation"].get_or(1.f);
					float v = colorInit["brightness"].get_or(1.f);

					float cr = colorInit["cr"].get_or(0.f);
					float cg = colorInit["cg"].get_or(0.f);
					float cb = colorInit["cb"].get_or(0.f);

					ColorCorrection* colorCorrect = new ColorCorrection(r, g, b, h, s, v);
					colorCorrect->setDistortionDirection(glm::vec2(1.f, 0.f));
					colorCorrect->setChromaticDistortion(glm::vec3(cr, cg, cb));
					renderer.addEffect(*colorCorrect, DrawTime::Late);

					if(postLister) postLister->add(*colorCorrect);

					effects.push_back(colorCorrect);
				}


				auto& fxaaInit = state["fxaa"];
				if (fxaaInit.valid()) {
					float minColorDiff = fxaaInit["minColorDiff"].get_or(0.f);
					float fxaaMul = fxaaInit["fxaaMul"].get_or(0.f);

					FXAA* fxaa = new FXAA(minColorDiff, fxaaMul);
					renderer.addEffect(*fxaa, DrawTime::Late);

					if (postLister) postLister->add(*fxaa);
					effects.push_back(fxaa);
				}

			}

			
			//Run application

			app.run();


			//Cleanup after exiting application

			if (camera != nullptr) delete camera;
			if (skybox != nullptr) delete skybox;
			if (postLister != nullptr) delete postLister;
			if (objectLister != nullptr) delete objectLister;

			for (auto effect(effects.begin()); effect != effects.end(); effect++)
				delete *effect;
		};


		bool usePhysics  = state.get_or<int>("usePhysics", false);
		bool useEmissive = state.get_or<int>("useEmissive", false);

		GBufferContent content = useEmissive ? GBufferContent::DefaultEmissive : GBufferContent::Default;
		PhysicsType physicsType = usePhysics ? PhysicsType::World : PhysicsType::None;

		Configuration config(window, init, content, physicsType);
		config.run();
	}


}