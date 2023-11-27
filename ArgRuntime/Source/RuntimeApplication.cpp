#include "RuntimeApplication.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Debug/Assert.hpp"
#include "Debug/Log.hpp"
#include "Renderer/Camera/Camera.hpp"
#include "Renderer/Camera/PerspectiveCamera.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/ShaderProgram.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Content/Import/TextureImporter.hpp"
#include "Content/Import/StaticMeshImporter.hpp"
#include <glad/glad.h>
#include "Renderer/Light/DirectionalLight.hpp"
#include "Renderer/Light/PointLight.hpp"
#include "Renderer/Light/SpotLight.hpp"
#include "Content/Import/SkeletonImporter.hpp"
#include "Renderer/SkeletalMesh.hpp"
#include "Content/Import/SkeletalMeshImporter.hpp"
#include "Content/Import/SkeletalAnimationImporter.hpp"
#include "Renderer/Animation/SkeletalAnimation.hpp"
#include "Content/ResourceCache.hpp"
#include "Content/Resource/Resource.hpp"
#include "Content/Content.hpp"
#include "Debug/ScopeTimer.hpp"

auto Arg::Client_CreateApplication() -> std::unique_ptr<Arg::Application>
{
	ARG_CONSOLE_LOG("Creating Runtime");
	std::unique_ptr<Application> app(new RuntimeApplication());
	return app;
}

void Arg::RuntimeApplication::VOnStartUp()
{
	const Arg::WindowSpec windowSpec{
		.Title = "Game",
		.Width = 1920,
		.Height = 1080,
		.Mode = WindowMode::Windowed,
		.bIsVSync = true,
	};
	m_pWindow = std::make_unique<Window>(windowSpec);
	m_pWindow->Create();
}

void Arg::RuntimeApplication::VOnShutDown()
{
	m_pWindow->Destroy();
}

void Arg::RuntimeApplication::VOnRun()
{
	std::shared_ptr<Renderer::Renderer> renderer(new Renderer::Renderer());

	const std::shared_ptr<Renderer::StaticMesh> cubeMesh(new Renderer::StaticMesh());
	cubeMesh->Bind();
	{
		Import::StaticMeshImporter meshImporter;
		const bool bIsSuccess = meshImporter.ImportFile("..\\Content\\Meshes\\cube.fbx");
		if (bIsSuccess)
		{
			cubeMesh->SetData(meshImporter.GetData());
		}
	}
	cubeMesh->Unbind();

	const std::shared_ptr<Renderer::StaticMesh> planeMesh(new Renderer::StaticMesh());
	planeMesh->Bind();
	{
		Import::StaticMeshImporter meshImporter;
		const bool bIsSuccess = meshImporter.ImportFile("..\\Content\\Meshes\\plane.fbx");
		if (bIsSuccess)
		{
			planeMesh->SetData(meshImporter.GetData());
		}
	}
	planeMesh->Unbind();

	const std::shared_ptr<Renderer::StaticMesh> armorMesh(new Renderer::StaticMesh());
	armorMesh->Bind();
	{
		Import::StaticMeshImporter meshImporter;
		const bool bIsSuccess = meshImporter.ImportFile("..\\Content\\Meshes\\tmpl_armor.fbx");
		if (bIsSuccess)
		{
			armorMesh->SetData(meshImporter.GetData());
		}
	}
	armorMesh->Unbind();

	const std::shared_ptr<Renderer::StaticMesh> skyBoxMesh(new Renderer::StaticMesh());
	skyBoxMesh->Bind();
	{
		Import::StaticMeshImporter meshImporter;
		const bool bIsSuccess = meshImporter.ImportFile("..\\Content\\Meshes\\skyBox.fbx");
		if (bIsSuccess)
		{
			skyBoxMesh->SetData(meshImporter.GetData());
		}
	}
	skyBoxMesh->Unbind();

	const std::shared_ptr<Renderer::SkeletalMesh> characterMesh(new Renderer::SkeletalMesh());
	characterMesh->Bind();
	{
		Import::SkeletalMeshImporter meshImporter;
		const bool bIsSuccess = meshImporter.ImportFile("..\\Content\\Meshes\\character.fbx");
		if (bIsSuccess)
		{
			characterMesh->SetData(meshImporter.GetData());
		}
	}
	characterMesh->Unbind();

	const std::shared_ptr<Renderer::StaticMesh> screenQuadMesh(new Renderer::StaticMesh());
	screenQuadMesh->Bind();
	{
		const Renderer::StaticMeshData meshData{
			.Vertices = {
				Renderer::StaticVertex{Vec3(-1.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 0.0f)},
				Renderer::StaticVertex{Vec3(-1.0f, -0.5f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 1.0f)},
				Renderer::StaticVertex{Vec3(-0.5f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec2(1.0f, 0.0f)},
				Renderer::StaticVertex{Vec3(-0.5f, -0.5f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec2(1.0f, 1.0f)},
			},
			.Indices = {
				0,3,1,
				0,2,3
			}
		};
		screenQuadMesh->SetData(meshData);
	}
	screenQuadMesh->Unbind();

	const std::shared_ptr<Renderer::Material> cubeMaterial(new Renderer::Material());

	const std::shared_ptr<Renderer::ShaderProgram> shader(new Renderer::ShaderProgram());
	{
		std::stringstream buffer;

		std::ifstream vertFile("..\\Content\\Shaders\\DefaultStatic.vert");
		buffer.str(std::string());
		buffer << vertFile.rdbuf();
		const std::string vertSource = buffer.str();
		vertFile.close();

		std::ifstream fragFile("..\\Content\\Shaders\\DefaultPhong.frag");
		buffer.str(std::string());
		buffer << fragFile.rdbuf();
		const std::string fragSource = buffer.str();
		fragFile.close();

		const Renderer::ShaderSource shaderSource{
				.VertexSource = vertSource,
				.FragmentSource = fragSource,
		};

		shader->Create(shaderSource);
	}

	const std::shared_ptr<Renderer::ShaderProgram> skeletalShader(new Renderer::ShaderProgram());
	{
		std::stringstream buffer;

		std::ifstream vertFile("..\\Content\\Shaders\\DefaultSkeletal.vert");
		buffer.str(std::string());
		buffer << vertFile.rdbuf();
		const std::string vertSource = buffer.str();
		vertFile.close();

		std::ifstream fragFile("..\\Content\\Shaders\\DefaultPhong.frag");
		buffer.str(std::string());
		buffer << fragFile.rdbuf();
		const std::string fragSource = buffer.str();
		fragFile.close();

		const Renderer::ShaderSource shaderSource{
				.VertexSource = vertSource,
				.FragmentSource = fragSource,
		};

		skeletalShader->Create(shaderSource);
	}

	const std::shared_ptr<Renderer::ShaderProgram> screenShader(new Renderer::ShaderProgram());
	{
		const std::string vertSource = R"(
			#version 460 core
			layout (location = 0) in vec3 v_Position;
			layout (location = 2) in vec2 v_TexUV;

			out vec2 TexUV;

			void main()
			{
				gl_Position = vec4(v_Position.x, v_Position.y, 0.0, 1.0);
				TexUV = v_TexUV;
			}  
		)";
		const std::string fragSource = R"(
			#version 460 core

			uniform sampler2D u_ScreenTexture;

			in vec2 TexUV;

			out vec4 FragColor;

			void main()
			{
				float depthValue = texture(u_ScreenTexture, TexUV).r;
				FragColor = vec4(vec3(depthValue), 1.0);
			}
		)";

		const Renderer::ShaderSource shaderSource{
				.VertexSource = vertSource,
				.FragmentSource = fragSource,
		};

		screenShader->Create(shaderSource);
	}

	std::shared_ptr<Renderer::Camera> camera(new Renderer::PerspectiveCamera());
	camera->SetPosition(Vec3(-20.0f, 0.0f, 1.0f));

	Renderer::Texture whiteTexture;
	whiteTexture.Bind(0);
	{
		uint32_t whiteTexturePixel[] = { 0xFFFFFFFF };
		const Renderer::TextureData whiteTextureData{
			.Data = whiteTexturePixel,
			.Width = 1,
			.Height = 1,
			.Format = Renderer::TextureFormat::FormatRGBA
		};
		whiteTexture.SetData(whiteTextureData);
	}
	whiteTexture.Unbind();

	const Renderer::Texture diffuseTexture;
	diffuseTexture.Bind();
	{
		Import::TextureImporter textureImporter;
		const bool bIsSuccess = textureImporter.ImportFile("..\\Content\\Textures\\container2.png");
		if (bIsSuccess)
		{
			diffuseTexture.SetData(textureImporter.GetData());
		}
	}
	diffuseTexture.Unbind();

	const Renderer::Texture specularTexture;
	specularTexture.Bind();
	{
		Import::TextureImporter textureImporter;
		const bool bIsSuccess = textureImporter.ImportFile("..\\Content\\Textures\\container2_specular.png");
		if (bIsSuccess)
		{
			specularTexture.SetData(textureImporter.GetData());
		}
	}
	specularTexture.Unbind();

	const Renderer::Texture woodTexture;
	woodTexture.Bind();
	{
		const Debug::ScopeTimer timer("TextureImport");
		Import::TextureImporter textureImporter;
		const bool bIsSuccess = textureImporter.ImportFile("..\\Content\\Textures\\container.jpg");
		if (bIsSuccess)
		{
			woodTexture.SetData(textureImporter.GetData());
		}
	}
	woodTexture.Unbind();

	const Renderer::Texture armorTexture;
	armorTexture.Bind();
	{
		Import::TextureImporter textureImporter;
		const bool bIsSuccess = textureImporter.ImportFile("..\\Content\\Textures\\tmpl_armor.tga");
		if (bIsSuccess)
		{
			armorTexture.SetData(textureImporter.GetData());
		}
	}
	armorTexture.Unbind();

	uint32_t cubeMapTexture;
	glGenTextures(1, &cubeMapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	for (auto i = 0; i < 6; i++)
	{
		Import::TextureImporter textureImporter;
		const bool bIsSuccess = textureImporter.ImportFile(
			std::format("..\\Content\\Textures\\skyBox_{}.png", i)
		);
		if (bIsSuccess)
		{
			const Renderer::TextureData& data = textureImporter.GetData();
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, data.Width, data.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.Data
			);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	bool bLookWithMouse = false;

	const std::shared_ptr<Renderer::ShaderProgram> shadowMapShader(new Renderer::ShaderProgram());
	{
		std::stringstream buffer;

		std::ifstream vertFile("..\\Content\\Shaders\\ShadowMap.vert");
		buffer.str(std::string());
		buffer << vertFile.rdbuf();
		const std::string vertSource = buffer.str();
		vertFile.close();

		std::ifstream fragFile("..\\Content\\Shaders\\ShadowMap.frag");
		buffer.str(std::string());
		buffer << fragFile.rdbuf();
		const std::string fragSource = buffer.str();
		fragFile.close();

		const Renderer::ShaderSource shaderSource{
				.VertexSource = vertSource,
				.FragmentSource = fragSource,
		};

		shadowMapShader->Create(shaderSource);
	}

	const std::shared_ptr<Renderer::ShaderProgram> skyBoxShader(new Renderer::ShaderProgram());
	{
		std::stringstream buffer;

		std::ifstream vertFile("..\\Content\\Shaders\\SkyBox.vert");
		buffer.str(std::string());
		buffer << vertFile.rdbuf();
		const std::string vertSource = buffer.str();
		vertFile.close();

		std::ifstream fragFile("..\\Content\\Shaders\\SkyBox.frag");
		buffer.str(std::string());
		buffer << fragFile.rdbuf();
		const std::string fragSource = buffer.str();
		fragFile.close();

		const Renderer::ShaderSource shaderSource{
				.VertexSource = vertSource,
				.FragmentSource = fragSource,
		};

		skyBoxShader->Create(shaderSource);
	}

	const Renderer::DirectionalLightSpec dirLightSpec
	{
		.Direction = Vec3(-0.3f, -0.4f, -1.0f),
		.Color = Vec3(1.0f),
		.Intensity = 1.0f,
		.bCastShadows = true,
		.ShadowMapShader = shadowMapShader
	};
	Renderer::DirectionalLight dirLight(dirLightSpec);

	const Renderer::PointLightSpec pointLightSpec
	{
		.Position = Vec3(0.0f, 2.5f, 0.5f),
		.Color = Vec3(1.0f, 0.0f, 0.0f),
		.Range = 50.0f,
		.Intensity = 2.0f
	};
	Renderer::PointLight pointLight(pointLightSpec);

	const Renderer::PointLightSpec pointLightSpec2
	{
		.Position = Vec3(0.0f, -500.0f, 5.0f),
		.Color = Vec3(0.1f, 0.2f, 0.8f),
		.Range = 500.0f,
		.Intensity = 10.0f
	};
	Renderer::PointLight pointLight2(pointLightSpec2);

	const Renderer::SpotLightSpec spotLightSpec
	{
		.Position = Vec3(10.0f),
		.Direction = Vec3(-5.0f),
		.InnerConeAngle = 44.0f,
		.OuterConeAngle = 45.0f,
		.Color = Vec3(0.52f, 0.3f, 0.94f),
		.Range = 100.0f,
		.Intensity = 1.0f
	};
	Renderer::SpotLight spotLight(spotLightSpec);

	Renderer::SkeletonSpec skeleton;
	{
		Import::SkeletonImporter skeletonImporter;
		const bool bIsSuccess = skeletonImporter.ImportFile("..\\Content\\Skeletons\\humanoid.fbx");
		if (bIsSuccess)
		{
			skeleton = skeletonImporter.GetData();
		}
	}

	Renderer::SkeletalAnimationData runningAnimation;
	{
		Import::SkeletalAnimationImporter importer;
		const bool bIsSuccess = importer.ImportFile("..\\Content\\Animations\\running.fbx");
		if (bIsSuccess)
		{
			runningAnimation = importer.GetData();
		}
	}

	//const uint64_t gameMapID = std::hash<std::string>("Maps\\Map_Default");

	//// From resource manager:
	//Content::ResourceCache resourceCache;
	//resourceCache.AddResource(gameMapID);

	//// From game engine:
	//const GameMapResource& gameMapResource = resourceCache.GetGameMap(gameMapID);
	//if(!gameMapResource.IsLoaded())
	//{
	//	gameMapResource.Load();
	//}
	//std::shared_ptr<GameMapData> gameMapData = gameMapResource.GetData();
	//engine->LoadMap(gameMapData);
	//gameMapResource.Free();

	//resourceCache.RemoveResource(gameMapID);

	//std::shared_ptr<Texture> texture = resourceCache.GetTexture();

	//texture->Bind();

	std::shared_ptr<Content::ResourceCache> resourceCache = std::make_shared<Content::ResourceCache>();

	const Content::ContentSpec contentSpec{
		.RootDirectory = "Content",
		.ResourceCache = resourceCache,
	};
	Content::Content content(contentSpec);
	content.Initialize();

	//const GUID textureID = std::hash<std::string>{}("Textures\\awesomeface");
	//{
	//	Content::TextureHandle texture = resourceCache->GetTexture(textureID);
	//	{
	//		Content::TextureHandle texture2 = resourceCache->GetTexture(textureID);
	//	}
	//}

	const GUID containerTextureID = std::hash<std::string>{}("Textures\\container");
	Content::TextureHandle containerTexture = resourceCache->GetTexture(containerTextureID);

	while (IsRunning() && !m_pWindow->ShouldClose())
	{
		m_pWindow->Update();
		const auto& keyboardState = m_pWindow->GetInput().GetKeyboardState();
		const auto& mouseState = m_pWindow->GetInput().GetMouseState();
		const auto& gamepadState = m_pWindow->GetInput().GetGamepadState(0);
		if (keyboardState.IsKeyPressed(Input::KeyCode::F4, Input::KeyMods::Alt))
		{
			Close();
		}

		// Camera controls
		{
			if (keyboardState.IsKeyDown(Input::KeyCode::W))
			{
				Vec3 cameraPos = camera->GetPosition();
				cameraPos += camera->GetForwardVector() * 0.1f;
				camera->SetPosition(cameraPos);
			}

			if (keyboardState.IsKeyDown(Input::KeyCode::S))
			{
				Vec3 cameraPos = camera->GetPosition();
				cameraPos += camera->GetForwardVector() * -0.1f;
				camera->SetPosition(cameraPos);
			}

			if (keyboardState.IsKeyDown(Input::KeyCode::D))
			{
				Vec3 cameraPos = camera->GetPosition();
				cameraPos += camera->GetRightVector() * 0.1f;
				camera->SetPosition(cameraPos);
			}

			if (keyboardState.IsKeyDown(Input::KeyCode::A))
			{
				Vec3 cameraPos = camera->GetPosition();
				cameraPos += camera->GetRightVector() * -0.1f;
				camera->SetPosition(cameraPos);
			}

			if (keyboardState.IsKeyDown(Input::KeyCode::Space))
			{
				Vec3 cameraPos = camera->GetPosition();
				cameraPos += Vec3(0.0f, 0.0f, 1.0f) * 0.1f;
				camera->SetPosition(cameraPos);
			}

			if (keyboardState.IsKeyDown(Input::KeyCode::LeftShift))
			{
				Vec3 cameraPos = camera->GetPosition();
				cameraPos += Vec3(0.0f, 0.0f, 1.0f) * -0.1f;
				camera->SetPosition(cameraPos);
			}

			if (mouseState.IsButtonDown(Input::MouseButton::Right))
			{
				m_pWindow->SetCursorMode(Input::CursorMode::Locked);
				bLookWithMouse = true;
			}

			if (mouseState.IsButtonUp(Input::MouseButton::Right))
			{
				m_pWindow->SetCursorMode(Input::CursorMode::Normal);
				bLookWithMouse = false;
			}

			if (bLookWithMouse)
			{
				Vec2 mouseDelta = mouseState.GetPositionDelta();
				float cameraPitch = camera->GetPitch();
				cameraPitch += mouseDelta.y * -0.1f;
				camera->SetPitch(cameraPitch);
				float cameraYaw = camera->GetYaw();
				cameraYaw += mouseDelta.x * -0.1f;
				camera->SetYaw(cameraYaw);
			}
		}

		whiteTexture.Bind(0);

		const Vec2i viewportSize = Vec2i(m_pWindow->GetWidth(), m_pWindow->GetHeight());
		const float aspectRatio = (float)viewportSize.x / viewportSize.y;
		const Mat4 proj = camera->VGetProjection(aspectRatio);
		const Mat4 view = camera->GetView();

		if (dirLight.IsCastingShadows())
		{
			dirLight.BeginShadowMap(renderer, camera);
			{
				{
					Mat4 model(1.0f);
					model = Math::translate(model, Vec3(0.0f, 0.0f, 1.0f));
					model = Math::rotate(model, m_pWindow->GetElapsedTime() * 0.5f, Vec3(0.0f, 0.0f, 1.0f));

					dirLight.DrawToShadowMap(cubeMesh, model);
				}

				{
					Mat4 model(1.0f);
					model = Math::scale(model, Vec3(10.0f, 10.0f, 1.0f));

					dirLight.DrawToShadowMap(planeMesh, model);
				}

				{
					Mat4 model(1.0f);
					model = Math::translate(model, Vec3(2.0f, 2.0f, 3.0f));

					dirLight.DrawToShadowMap(cubeMesh, model);
				}

				{
					Mat4 model(1.0f);
					model = Math::translate(model, Vec3(4.0f, -2.0f, 2.0f));

					dirLight.DrawToShadowMap(cubeMesh, model);
				}

				{
					Mat4 model(1.0f);
					model = Math::translate(model, Vec3(-4.0f, -4.0f, 0.0f));
					model = Math::scale(model, Vec3(3.0f));

					dirLight.DrawToShadowMap(armorMesh, model);
				}

				{

					shadowMapShader->SetUniform("u_IsSkeletal", true);
					{
						Mat4 model(1.0f);
						model = Math::translate(model, Vec3(0.0f, 4.0f, 0.0f));
						model = Math::scale(model, Vec3(3.0f));

						std::vector<Mat4> animationBoneTransforms(skeleton.Bones.size());
						const float timeElapsed = m_pWindow->GetElapsedTime()
							* runningAnimation.TicksPerSecond;
						const float animationTime = timeElapsed
							- ((int)(timeElapsed / runningAnimation.Duration) * runningAnimation.Duration);
						for (auto i = 0; i < skeleton.Bones.size(); i++)
						{
							animationBoneTransforms[i] = Mat4(1.0f);
							const auto boneChannels = runningAnimation.Channels[i];
							{
								int32_t keyIndex = -1;
								for (auto j = 0; j < boneChannels.PositionKeys.size(); j++)
								{
									if (boneChannels.PositionKeys[j].Time > animationTime)
									{
										keyIndex = j;
										break;
									}
								}

								if (keyIndex >= 0)
								{
									const float keyTime = boneChannels
										.PositionKeys[keyIndex].Time;
									const float prevKeyTime = boneChannels
										.PositionKeys[keyIndex - 1].Time;
									const float keyDiff = keyTime - prevKeyTime;
									const float percent = (animationTime - prevKeyTime)
										/ keyDiff;
									const Vec3 position = (percent
										* boneChannels.PositionKeys[keyIndex].Value)
										+ ((1.0f - percent)
											* boneChannels.PositionKeys[keyIndex - 1].Value);

									animationBoneTransforms[i] = Math::translate(animationBoneTransforms[i],
										position);
								}
							}
							{
								int32_t keyIndex = -1;
								for (auto j = 0; j < boneChannels.RotationKeys.size(); j++)
								{
									if (boneChannels.RotationKeys[j].Time > animationTime)
									{
										keyIndex = j;
										break;
									}
								}

								if (keyIndex >= 0)
								{
									const float keyTime = boneChannels
										.RotationKeys[keyIndex].Time;
									const float prevKeyTime = boneChannels
										.RotationKeys[keyIndex - 1].Time;
									const float keyDiff = keyTime - prevKeyTime;
									const float percent = (animationTime - prevKeyTime)
										/ keyDiff;
									const Quat rotation = Math::slerp(boneChannels
										.RotationKeys[keyIndex - 1].Value,
										boneChannels
										.RotationKeys[keyIndex].Value,
										percent);

									const Mat4 rotationMat = Math::mat4_cast(rotation);
									animationBoneTransforms[i] = animationBoneTransforms[i]
										* rotationMat;
								}
							}
							{
								int32_t keyIndex = -1;
								for (auto j = 0; j < boneChannels.ScaleKeys.size(); j++)
								{
									if (boneChannels.ScaleKeys[j].Time > animationTime)
									{
										keyIndex = j;
										break;
									}
								}

								if (keyIndex >= 0)
								{
									const float keyTime = boneChannels
										.ScaleKeys[keyIndex].Time;
									const float prevKeyTime = boneChannels
										.ScaleKeys[keyIndex - 1].Time;
									const float keyDiff = keyTime - prevKeyTime;
									const float percent = (animationTime - prevKeyTime)
										/ keyDiff;
									const Vec3 scale = (percent
										* boneChannels.ScaleKeys[keyIndex].Value)
										+ ((1.0f - percent)
											* boneChannels.ScaleKeys[keyIndex - 1].Value);

									animationBoneTransforms[i] = Math::scale(animationBoneTransforms[i],
										scale);
								}
							}
						}

						std::vector<Mat4> boneTransforms(skeleton.Bones.size());

						for (auto i = 0; i < skeleton.Bones.size(); i++)
						{
							{
								// For rest pose:
								//Mat4 boneTransform = skeleton.Bones[i].LocalTransform;

								// For animation pose:
								Mat4 boneTransform = animationBoneTransforms[i];

								if (skeleton.Bones[i].ParentIndex > 0)
								{
									boneTransform = boneTransforms[skeleton.Bones[i].ParentIndex]
										* boneTransform;
								}

								boneTransforms[i] = boneTransform;
							}
						}

						for (auto i = 0; i < skeleton.Bones.size(); i++)
						{
							shadowMapShader->SetUniform(
								std::format("u_BoneTransforms[{}]", i),
								boneTransforms[i] * skeleton.Bones[i].OffsetTransform
							);
						}

						shadowMapShader->SetUniform("u_Model", model);

						characterMesh->Draw();
					}
				}
			}
			dirLight.EndShadowMap();

			dirLight.BeginShadowMapFar(renderer, camera);
			{

				{
					Mat4 model(1.0f);
					model = Math::translate(model, Vec3(0.0f, 0.0f, 1.0f));
					model = Math::rotate(model, m_pWindow->GetElapsedTime() * 0.5f, Vec3(0.0f, 0.0f, 1.0f));

					dirLight.DrawToShadowMapFar(cubeMesh, model);
				}

				{
					Mat4 model(1.0f);
					model = Math::scale(model, Vec3(10.0f, 10.0f, 1.0f));

					dirLight.DrawToShadowMapFar(planeMesh, model);
				}

				{
					Mat4 model(1.0f);
					model = Math::translate(model, Vec3(2.0f, 2.0f, 3.0f));

					dirLight.DrawToShadowMapFar(cubeMesh, model);
				}

				{
					Mat4 model(1.0f);
					model = Math::translate(model, Vec3(4.0f, -2.0f, 2.0f));

					dirLight.DrawToShadowMapFar(cubeMesh, model);
				}

				{
					Mat4 model(1.0f);
					model = Math::translate(model, Vec3(-4.0f, -4.0f, 0.0f));
					model = Math::scale(model, Vec3(3.0f));

					dirLight.DrawToShadowMapFar(armorMesh, model);
				}
			}
			dirLight.EndShadowMapFar();
		}

		// Regular scene rendering
		{
			const Renderer::FrameParams frameParams{
				.ViewportSize = viewportSize,
				.ClearColor = Vec4(0.5f, 0.5f, 0.5f, 1.0f)
			};
			renderer->BeginFrame(frameParams);

			shader->Use();

			shader->SetUniform("u_Proj", proj);
			shader->SetUniform("u_View", view);

			glActiveTexture(GL_TEXTURE0 + 9);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
			shader->SetUniform("u_SkyBox", 9);

			dirLight.Apply(shader, camera);

			shader->SetUniform("u_PointLightsCount", 2);

			pointLight.Apply(shader, camera, 0);
			pointLight2.Apply(shader, camera, 1);

			shader->SetUniform("u_SpotLightsCount", 1);

			spotLight.Apply(shader, camera, 0);

			// Draw opaque
			renderer->BeginOpaque();
			{
				Mat4 model(1.0f);
				model = Math::translate(model, Vec3(0.0f, 0.0f, 1.0f));
				model = Math::rotate(model, m_pWindow->GetElapsedTime() * 0.5f, Vec3(0.0f, 0.0f, 1.0f));

				const Mat3 normal(Math::transpose(Math::inverse(view * model)));

				shader->SetUniform("u_Model", model);
				shader->SetUniform("u_Normal", normal);

				shader->SetUniform("u_ReceiveShadows", true);

				shader->SetUniform("u_Material.diffuse", Vec3(1.0f));
				diffuseTexture.Bind(1);
				shader->SetUniform("u_Material.diffuseMap", 1);

				shader->SetUniform("u_Material.specular", 1.0f);
				specularTexture.Bind(2);
				shader->SetUniform("u_Material.specularMap", 2);
				shader->SetUniform("u_Material.shininess", 1.0f);

				shader->SetUniform("u_Material.reflection", 0.4f);
				shader->SetUniform("u_Material.reflectionMap", 2);

				cubeMesh->Draw();
			}

			{
				Mat4 model(1.0f);
				model = Math::scale(model, Vec3(10.0f, 10.0f, 1.0f));

				const Mat3 normal(Math::transpose(Math::inverse(view * model)));

				shader->SetUniform("u_Model", model);
				shader->SetUniform("u_Normal", normal);

				shader->SetUniform("u_ReceiveShadows", true);

				shader->SetUniform("u_Material.diffuse", Vec3(1.0f));
				//woodTexture.Bind(1);
				containerTexture.Bind(2);
				shader->SetUniform("u_Material.diffuseMap", 2);

				shader->SetUniform("u_Material.specular", 1.0f);
				shader->SetUniform("u_Material.specularMap", 0);
				shader->SetUniform("u_Material.shininess", 1.0f);

				shader->SetUniform("u_Material.reflection", 0.0f);
				shader->SetUniform("u_Material.reflectionMap", 0);

				planeMesh->Draw();
			}

			{
				Mat4 model(1.0f);
				model = Math::translate(model, Vec3(2.0f, 2.0f, 3.0f));

				const Mat3 normal(Math::transpose(Math::inverse(view * model)));

				shader->SetUniform("u_Model", model);
				shader->SetUniform("u_Normal", normal);

				shader->SetUniform("u_ReceiveShadows", true);

				shader->SetUniform("u_Material.diffuse", Vec3(1.0f));
				diffuseTexture.Bind(1);
				shader->SetUniform("u_Material.diffuseMap", 1);

				shader->SetUniform("u_Material.specular", 1.0f);
				specularTexture.Bind(2);
				shader->SetUniform("u_Material.specularMap", 2);
				shader->SetUniform("u_Material.shininess", 1.0f);

				shader->SetUniform("u_Material.reflection", 0.0f);
				shader->SetUniform("u_Material.reflectionMap", 0);

				cubeMesh->Draw();
			}

			{
				Mat4 model(1.0f);
				model = Math::translate(model, Vec3(4.0f, -2.0f, 2.0f));

				const Mat3 normal(Math::transpose(Math::inverse(view * model)));

				shader->SetUniform("u_Model", model);
				shader->SetUniform("u_Normal", normal);

				shader->SetUniform("u_ReceiveShadows", true);

				shader->SetUniform("u_Material.diffuse", Vec3(1.0f, 0.0f, 1.0f));
				shader->SetUniform("u_Material.diffuseMap", 0);

				shader->SetUniform("u_Material.specular", 0.5f);
				shader->SetUniform("u_Material.specularMap", 0);
				shader->SetUniform("u_Material.shininess", 0.2f);

				shader->SetUniform("u_Material.reflection", 0.0f);
				shader->SetUniform("u_Material.reflectionMap", 0);

				cubeMesh->Draw();
			}

			{
				Mat4 model(1.0f);
				model = Math::translate(model, Vec3(-4.0f, -4.0f, 0.0f));
				model = Math::scale(model, Vec3(3.0f));

				const Mat3 normal(Math::transpose(Math::inverse(view * model)));

				shader->SetUniform("u_Model", model);
				shader->SetUniform("u_Normal", normal);

				shader->SetUniform("u_ReceiveShadows", true);

				shader->SetUniform("u_Material.diffuse", Vec3(1.0f));
				armorTexture.Bind(1);
				shader->SetUniform("u_Material.diffuseMap", 1);

				shader->SetUniform("u_Material.specular", 0.2f);
				shader->SetUniform("u_Material.specularMap", 0);
				shader->SetUniform("u_Material.shininess", 0.6f);

				shader->SetUniform("u_Material.reflection", 0.0f);
				shader->SetUniform("u_Material.reflectionMap", 0);

				armorMesh->Draw();
			}

			std::vector<Mat4> boneTransforms;
			for (auto i = 0; i < skeleton.Bones.size(); i++)
			{
				{
					Mat4 boneTransform = skeleton.Bones[i].LocalTransform;
					if (skeleton.Bones[i].ParentIndex > 0)
					{
						boneTransform = boneTransforms[skeleton.Bones[i].ParentIndex] * boneTransform;
					}
					boneTransforms.push_back(boneTransform);
				}

				{
					Mat4 model(1.0f);
					model = Math::translate(model, Vec3(0.0f, 0.0f, 5.f));
					model = Math::scale(model, Vec3(3.0f));
					model *= boneTransforms[i];
					model = Math::scale(model, Vec3(0.01f));

					const Mat3 normal(Math::transpose(Math::inverse(view * model)));

					shader->SetUniform("u_Model", model);
					shader->SetUniform("u_Normal", normal);

					shader->SetUniform("u_ReceiveShadows", true);

					shader->SetUniform("u_Material.diffuse", Vec3(0.1f));
					shader->SetUniform("u_Material.diffuseMap", 0);

					shader->SetUniform("u_Material.specular", 0.0f);
					shader->SetUniform("u_Material.specularMap", 0);
					shader->SetUniform("u_Material.shininess", 0.1f);

					shader->SetUniform("u_Material.reflection", 0.0f);
					shader->SetUniform("u_Material.reflectionMap", 0);

					cubeMesh->Draw();
				}
			}

			// Skeletal meshes
			{
				skeletalShader->Use();

				skeletalShader->SetUniform("u_Proj", proj);
				skeletalShader->SetUniform("u_View", view);

				glActiveTexture(GL_TEXTURE0 + 9);
				glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
				skeletalShader->SetUniform("u_SkyBox", 9);

				dirLight.Apply(skeletalShader, camera);

				skeletalShader->SetUniform("u_PointLightsCount", 2);

				pointLight.Apply(skeletalShader, camera, 0);
				pointLight2.Apply(skeletalShader, camera, 1);

				skeletalShader->SetUniform("u_SpotLightsCount", 1);

				spotLight.Apply(skeletalShader, camera, 0);

				{
					Mat4 model(1.0f);
					model = Math::translate(model, Vec3(0.0f, 4.0f, 0.0f));
					model = Math::scale(model, Vec3(3.0f));

					const Mat3 normal(Math::transpose(Math::inverse(view * model)));

					std::vector<Mat4> animationBoneTransforms(skeleton.Bones.size());
					const float timeElapsed = m_pWindow->GetElapsedTime()
						* runningAnimation.TicksPerSecond;
					const float animationTime = timeElapsed
						- ((int)(timeElapsed / runningAnimation.Duration) * runningAnimation.Duration);
					for (auto i = 0; i < skeleton.Bones.size(); i++)
					{
						animationBoneTransforms[i] = Mat4(1.0f);
						const auto boneChannels = runningAnimation.Channels[i];
						{
							int32_t keyIndex = -1;
							for (auto j = 0; j < boneChannels.PositionKeys.size(); j++)
							{
								if (boneChannels.PositionKeys[j].Time > animationTime)
								{
									keyIndex = j;
									break;
								}
							}

							if (keyIndex >= 0)
							{
								const float keyTime = boneChannels
									.PositionKeys[keyIndex].Time;
								const float prevKeyTime = boneChannels
									.PositionKeys[keyIndex - 1].Time;
								const float keyDiff = keyTime - prevKeyTime;
								const float percent = (animationTime - prevKeyTime)
									/ keyDiff;
								const Vec3 position = (percent
									* boneChannels.PositionKeys[keyIndex].Value)
									+ ((1.0f - percent)
										* boneChannels.PositionKeys[keyIndex - 1].Value);

								animationBoneTransforms[i] = Math::translate(animationBoneTransforms[i],
									position);
							}
						}
						{
							int32_t keyIndex = -1;
							for (auto j = 0; j < boneChannels.RotationKeys.size(); j++)
							{
								if (boneChannels.RotationKeys[j].Time > animationTime)
								{
									keyIndex = j;
									break;
								}
							}

							if (keyIndex >= 0)
							{
								const float keyTime = boneChannels
									.RotationKeys[keyIndex].Time;
								const float prevKeyTime = boneChannels
									.RotationKeys[keyIndex - 1].Time;
								const float keyDiff = keyTime - prevKeyTime;
								const float percent = (animationTime - prevKeyTime)
									/ keyDiff;
								const Quat rotation = Math::slerp(boneChannels
									.RotationKeys[keyIndex - 1].Value,
									boneChannels
									.RotationKeys[keyIndex].Value,
									percent);

								const Mat4 rotationMat = Math::mat4_cast(rotation);
								animationBoneTransforms[i] = animationBoneTransforms[i]
									* rotationMat;
							}
						}
						{
							int32_t keyIndex = -1;
							for (auto j = 0; j < boneChannels.ScaleKeys.size(); j++)
							{
								if (boneChannels.ScaleKeys[j].Time > animationTime)
								{
									keyIndex = j;
									break;
								}
							}

							if (keyIndex >= 0)
							{
								const float keyTime = boneChannels
									.ScaleKeys[keyIndex].Time;
								const float prevKeyTime = boneChannels
									.ScaleKeys[keyIndex - 1].Time;
								const float keyDiff = keyTime - prevKeyTime;
								const float percent = (animationTime - prevKeyTime)
									/ keyDiff;
								const Vec3 scale = (percent
									* boneChannels.ScaleKeys[keyIndex].Value)
									+ ((1.0f - percent)
										* boneChannels.ScaleKeys[keyIndex - 1].Value);

								animationBoneTransforms[i] = Math::scale(animationBoneTransforms[i],
									scale);
							}
						}
					}

					std::vector<Mat4> boneTransforms(skeleton.Bones.size());

					for (auto i = 0; i < skeleton.Bones.size(); i++)
					{
						{
							// For rest pose:
							//Mat4 boneTransform = skeleton.Bones[i].LocalTransform;

							// For animation pose:
							Mat4 boneTransform = animationBoneTransforms[i];

							if (skeleton.Bones[i].ParentIndex > 0)
							{
								boneTransform = boneTransforms[skeleton.Bones[i].ParentIndex]
									* boneTransform;
							}

							boneTransforms[i] = boneTransform;
						}
					}

					for (auto i = 0; i < skeleton.Bones.size(); i++)
					{
						skeletalShader->SetUniform(
							std::format("u_BoneTransforms[{}]", i),
							boneTransforms[i] * skeleton.Bones[i].OffsetTransform
						);
					}

					skeletalShader->SetUniform("u_Model", model);
					skeletalShader->SetUniform("u_Normal", normal);

					skeletalShader->SetUniform("u_ReceiveShadows", true);

					skeletalShader->SetUniform("u_Material.diffuse", Vec3(0.3f, 0.2f, 0.9f));
					skeletalShader->SetUniform("u_Material.diffuseMap", 0);

					skeletalShader->SetUniform("u_Material.specular", 0.2f);
					skeletalShader->SetUniform("u_Material.specularMap", 0);
					skeletalShader->SetUniform("u_Material.shininess", 0.6f);

					skeletalShader->SetUniform("u_Material.reflection", 0.6f);
					skeletalShader->SetUniform("u_Material.reflectionMap", 0);

					characterMesh->Draw();
				}
			}

			// Sort transparent

			// Draw transparent sorted
			renderer->BeginTransparent();

			renderer->BeginOpaque();

			// Sky box
			//glDepthFunc(GL_LEQUAL);
			//skyBoxShader->Use();
			//skyBoxShader->SetUniform("u_Proj", proj);
			//const Mat4 skyBoxView = Mat4(Mat3(view));
			//skyBoxShader->SetUniform("u_View", skyBoxView);
			//glActiveTexture(GL_TEXTURE0 + 1);
			//glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
			//skyBoxShader->SetUniform("u_SkyBox", 1);
			//skyBoxMesh->Draw();
			//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			//glDepthFunc(GL_LESS);
		}

		// Shadow map preview
		//{
		//	//const Vec2i viewportSize = Vec2i(m_pWindow->GetWidth(), m_pWindow->GetHeight());
		//	//const Renderer::FrameParams frameParams{
		//	//	.ViewportSize = viewportSize,
		//	//	.ClearColor = Vec4(0.5f, 0.5f, 0.5f, 1.0f)
		//	//};
		//	//renderer.BeginFrame(frameParams);
		//	glDisable(GL_DEPTH_TEST);
		//	screenShader->Use();
		//	shadowMapTexture.Bind(1);
		//	screenShader->SetUniform("u_ScreenTexture", 1);
		//	screenQuadMesh->Draw();
		//}

		m_pWindow->SwapBuffers();
	}
}
