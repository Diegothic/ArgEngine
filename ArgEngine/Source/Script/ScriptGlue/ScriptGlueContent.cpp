#include <arg_pch.hpp>
#include "ScriptGlueContent.hpp"

#include "Script/ScriptComponent.hpp"

void Arg::Script::ScriptExport_Resources(const ScriptEngine& scriptEngine)
{
	auto& scriptState = scriptEngine.GetState();

	scriptState.new_usertype<TextureHandle>(
		"Texture",
		sol::meta_function::equal_to, sol::overload([](
			const TextureHandle& lhs,
			const TextureHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](TextureHandle& self) -> bool
		{
			return self.IsValid();
		}
	);

	scriptState.new_usertype<StaticModelHandle>(
		"StaticModel",
		sol::meta_function::equal_to, sol::overload([](
			const StaticModelHandle& lhs,
			const StaticModelHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](StaticModelHandle& self) -> bool
		{
			return self.IsValid();
		}
	);

	scriptState.new_usertype<SkeletonHandle>(
		"Skeleton",
		sol::meta_function::equal_to, sol::overload([](
			const SkeletonHandle& lhs,
			const SkeletonHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](SkeletonHandle& self) -> bool
		{
			return self.IsValid();
		}
	);

	scriptState.new_usertype<SkeletalModelHandle>(
		"SkeletalModel",
		sol::meta_function::equal_to, sol::overload([](
			const SkeletalModelHandle& lhs,
			const SkeletalModelHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](SkeletalModelHandle& self) -> bool
		{
			return self.IsValid();
		}
	);

	scriptState.new_usertype<SkeletalAnimationHandle>(
		"SkeletalAnimation",
		sol::meta_function::equal_to, sol::overload([](
			const SkeletalAnimationHandle& lhs,
			const SkeletalAnimationHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](SkeletalAnimationHandle& self) -> bool
		{
			return self.IsValid();
		}
	);

	scriptState.new_usertype<MaterialHandle>(
		"Material",
		sol::meta_function::equal_to, sol::overload([](
			const MaterialHandle& lhs,
			const MaterialHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](MaterialHandle& self) -> bool
		{
			return self.IsValid();
		},
		"DiffuseTexture",
		[](MaterialHandle& self) -> TextureHandle
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			TextureHandle result = material->GetDiffuseMap();

			if (self.IsValid())
			{
				self.FreeRef();
			}

			return result;
		},
		"SetDiffuseTexture",
		[](MaterialHandle& self, const TextureHandle& texture)
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			material->SetDiffuseMap(texture);

			if (self.IsValid())
			{
				self.FreeRef();
			}
		},
		"SpecularTexture",
		[](MaterialHandle& self) -> TextureHandle
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			TextureHandle result = material->GetSpecularMap();

			if (self.IsValid())
			{
				self.FreeRef();
			}

			return result;
		},
		"SetSpecularTexture",
		[](MaterialHandle& self, const TextureHandle& texture)
		{
			if (self.IsValid())
			{
				self.AddRef();
			}


			Renderer::Material* material = self.Get()->GetMaterial().get();
			material->SetSpecularMap(texture);

			if (self.IsValid())
			{
				self.FreeRef();
			}
		},
		"ReflectivityTexture",
		[](MaterialHandle& self) -> TextureHandle
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			const TextureHandle result = material->GetReflectivityMap();

			if (self.IsValid())
			{
				self.FreeRef();
			}

			return result;
		},
		"SetReflectivityTexture",
		[](MaterialHandle& self, const TextureHandle& texture)
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			material->SetReflectivityMap(texture);

			if (self.IsValid())
			{
				self.FreeRef();
			}
		},
		"Diffuse",
		[](MaterialHandle& self) -> Vec3
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			const Vec4 diffuseColor = material->GetDiffuseColor();
			const Vec3 result = Vec3(diffuseColor.x, diffuseColor.y, diffuseColor.z);

			if (self.IsValid())
			{
				self.FreeRef();
			}

			return result;
		},
		"SetDiffuse",
		[](MaterialHandle& self, const Vec3& diffuse)
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			const Vec4 diffuseColor = Vec4(diffuse.x, diffuse.y, diffuse.z, 1.0f);
			material->SetDiffuseColor(diffuseColor);

			if (self.IsValid())
			{
				self.FreeRef();
			}
		},
		"Specular",
		[](MaterialHandle& self) -> float
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			const float result = material->GetSpecular();

			if (self.IsValid())
			{
				self.FreeRef();
			}

			return result;
		},
		"SetSpecular",
		[](MaterialHandle& self, const float& specular)
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			material->SetSpecular(specular);

			if (self.IsValid())
			{
				self.FreeRef();
			}
		},
		"Shininess",
		[](MaterialHandle& self) -> float
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			const float result = material->GetShininess();

			if (self.IsValid())
			{
				self.FreeRef();
			}

			return result;
		},
		"SetShininess",
		[](MaterialHandle& self, const float& shininess)
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			material->SetShininess(shininess);

			if (self.IsValid())
			{
				self.FreeRef();
			}
		},
		"Reflectivity",
		[](MaterialHandle& self) -> float
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			const float result = material->GetReflectivity();

			if (self.IsValid())
			{
				self.FreeRef();
			}

			return result;
		},
		"SetReflectivity",
		[](MaterialHandle& self, const float& reflectivity)
		{
			if (self.IsValid())
			{
				self.AddRef();
			}

			Renderer::Material* material = self.Get()->GetMaterial().get();
			material->SetReflectivity(reflectivity);

			if (self.IsValid())
			{
				self.FreeRef();
			}
		}
	);

	scriptState.new_usertype<SoundResourceHandle>(
		"Sound",
		sol::meta_function::equal_to, sol::overload([](
			const SoundResourceHandle& lhs,
			const SoundResourceHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](SoundResourceHandle& self) -> bool
		{
			return self.IsValid();
		}
	);
}
