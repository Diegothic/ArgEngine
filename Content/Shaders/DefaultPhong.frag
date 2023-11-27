#version 460 core

#define DIR_LIGHTS_MAX 1
#define POINT_LIGHTS_MAX 64
#define SPOT_LIGHTS_MAX 32

struct Material
{
	vec3 diffuse;
	sampler2D diffuseMap;
	float specular;
	sampler2D specularMap;
	float shininess;
	float reflection;
	sampler2D reflectionMap;
};

struct LightProperties
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct LightAttenuation
{
	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight
{
	vec3 direction;
	LightProperties properties;
	bool castShadows;
	sampler2D shadowMap;
	sampler2D shadowMapFar;
};

struct PointLight
{
	vec3 position;
	LightAttenuation attenuation;
	LightProperties properties;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float innerCone;
	float outerCone;
	LightAttenuation attenuation;
	LightProperties properties;
};

uniform Material u_Material;

uniform DirectionalLight u_DirLights[DIR_LIGHTS_MAX];
uniform int u_DirLightsCount;

uniform PointLight u_PointLights[POINT_LIGHTS_MAX];
uniform int u_PointLightsCount;

uniform SpotLight u_SpotLights[SPOT_LIGHTS_MAX];
uniform int u_SpotLightsCount;

uniform samplerCube u_SkyBox;
uniform mat4 u_View;

uniform bool u_ReceiveShadows;

in VS_OUT 
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexUV;

	bool ReceiveShadows;
	vec4 FragPosDirLightSpace;
	vec4 FragPosDirLightSpaceFar;
} fs_in;

out vec4 FragColor;

vec3 CalculateReflectionColor(
	vec3 fragNormal, 
	vec3 viewDirection
);
vec3 CalculateDirectionalLightImpact(
	DirectionalLight light, 
	vec3 fragNormal, 
	vec3 viewDirection,
	vec3 baseAmbient,
	vec3 baseDiffuse,
	vec3 baseSpecular
);
vec3 CalculatePointLightImpact(
	PointLight light, 
	vec3 fragNormal, 
	vec3 viewDirection,
	vec3 baseAmbient,
	vec3 baseDiffuse,
	vec3 baseSpecular
);
vec3 CalculateSpotLightImpact(
	SpotLight light, 
	vec3 fragNormal, 
	vec3 viewDirection,
	vec3 baseAmbient,
	vec3 baseDiffuse,
	vec3 baseSpecular
);
float ShadowCalculation(
	vec3 fragPosLightSpace, 
	sampler2D shadowMap, 
	int sampleSize, 
	vec3 fragNormal, 
	vec3 lightDirection
);

void main()
{
	vec3 result = vec3(0.0);

	vec3 fragNormal = normalize(fs_in.Normal);
	vec3 viewDirection = normalize(-fs_in.FragPos);

	// Environment ambient
	vec3 fragNormalWorld = inverse(mat3(u_View)) * fragNormal;
	vec3 skyBoxColor = texture(u_SkyBox, fragNormalWorld).rgb;
	result += skyBoxColor * 0.05;

	vec3 baseAmbient = vec3(texture(u_Material.diffuseMap, fs_in.TexUV)) 
		* u_Material.diffuse;

	vec3 baseDiffuse = vec3(texture(u_Material.diffuseMap, fs_in.TexUV)) 
		* u_Material.diffuse;

	vec3 baseSpecular = vec3(texture(u_Material.specularMap, fs_in.TexUV))
		* u_Material.specular;

	vec3 baseReflection = vec3(texture(u_Material.reflectionMap, fs_in.TexUV)) 
		* u_Material.reflection;

	// Calculate directional lights
	int dirLightsCount = min(u_DirLightsCount, DIR_LIGHTS_MAX);
	for (int i = 0; i < dirLightsCount; i++)
	{
		vec3 lightImpact = CalculateDirectionalLightImpact(
			u_DirLights[i], 
			fragNormal, 
			viewDirection,
			baseAmbient,
			baseDiffuse,
			baseSpecular
		);

		result += lightImpact;
	}

	// Calculate point lights
	int pointLightsCount = min(u_PointLightsCount, POINT_LIGHTS_MAX);
	for (int i = 0; i < pointLightsCount; i++)
	{
		vec3 lightImpact = CalculatePointLightImpact(
			u_PointLights[i], 
			fragNormal, 
			viewDirection,
			baseAmbient,
			baseDiffuse,
			baseSpecular
		);

		result += lightImpact;
	}

	// Calculate spot lights
	int spotLightsCount = min(u_SpotLightsCount, SPOT_LIGHTS_MAX);
	for (int i = 0; i < spotLightsCount; i++)
	{
		vec3 lightImpact = CalculateSpotLightImpact(
			u_SpotLights[i], 
			fragNormal, 
			viewDirection,
			baseAmbient,
			baseDiffuse,
			baseSpecular
		);

		result += lightImpact;
	}

	// Calculate environment reflection
	vec3 reflectionColor = CalculateReflectionColor(
		fragNormal, 
		viewDirection
	);
	//result = mix(result, reflectionColor, baseReflection);
	result += reflectionColor * baseReflection;

    vec4 fragColor = vec4(result, 1.0);

	// Gamma correction
	float gamma = 2.2;
	FragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));

	FragColor = fragColor;
}

vec3 CalculateReflectionColor(
	vec3 fragNormal, 
	vec3 viewDirection
)
{
	vec3 reflected = reflect(-viewDirection, fragNormal);
	vec3 reflectedWorld = inverse(mat3(u_View)) * reflected;
	return texture(u_SkyBox, reflectedWorld).rgb;
}

float ShadowCalculation(
	vec4 fragPosLightSpace, 
	sampler2D shadowMap, 
	int sampleSize, 
	vec3 fragNormal, 
	vec3 lightDirection
)
{
	float shadow = 0.0;

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	if (projCoords.z >= 1.0)
	{
		return shadow;
	}
	// Transform NDC to range [0,1];
	projCoords = projCoords * 0.5 + 0.5;
	// Depth from light point of view
	float closestDepth = vec3(texture(shadowMap, projCoords.xy)).r;
	// Depth from viewers perspective
	float currentDepth = projCoords.z;

	// Bias to compensate resolution errors
	float bias = max(0.01 * (1.0 - max(dot(fragNormal, lightDirection), 0.0)), 0.001);
	// Calculate samples for smooth shadows
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -sampleSize; x <= sampleSize; x++)
	{
		for (int y = -sampleSize; y <= sampleSize; y++)
		{
			// Calculate shadow based on depth comparison
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= (sampleSize * 2 + 1) * (sampleSize * 2 + 1);

	return shadow;
}

vec3 CalculateDirectionalLightImpact(
	DirectionalLight light, 
	vec3 fragNormal, 
	vec3 viewDirection,
	vec3 baseAmbient,
	vec3 baseDiffuse,
	vec3 baseSpecular
)
{
	vec3 lightDirection = normalize(-light.direction);
	vec3 reflectDirection = reflect(-lightDirection, fragNormal);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);

	vec3 ambient = light.properties.ambient 
		* baseAmbient;

	float diffuseIntensity =  max(dot(fragNormal, lightDirection), 0.0);
	vec3 diffuse = light.properties.diffuse 
		* diffuseIntensity 
		* baseDiffuse;

	float specularIntensity = pow(max(
		dot(fragNormal, halfwayDirection), 0.0), 
		u_Material.shininess * 128);
	vec3 specular = light.properties.specular 
		* specularIntensity 
		* baseSpecular;

	// Calculate shadow
	float shadow = 0.0;
	if (u_ReceiveShadows && light.castShadows)
	{
		float distanceFromCenter = min(distance(vec3(0.0), fs_in.FragPosDirLightSpace.xyz), 1.0);
		
		if (distanceFromCenter <= 0.9)
		 {
			shadow = ShadowCalculation(
				fs_in.FragPosDirLightSpace, 
				light.shadowMap, 
				2,
				fragNormal, 
				lightDirection
			);
		 }
		 else if (distanceFromCenter < 1.0)
		 {
			float percent = 1.0 - (distanceFromCenter - 0.9) * 10.0;
			float shadowClose = ShadowCalculation(
				fs_in.FragPosDirLightSpace, 
				light.shadowMap, 
				1,
				fragNormal, 
				lightDirection
			);
			float shadowFar = ShadowCalculation(
				fs_in.FragPosDirLightSpaceFar, 
				light.shadowMapFar, 
				1,
				fragNormal, 
				lightDirection
			);
			shadow = percent * shadowClose + (1.0 - percent) * shadowFar;
		 }
		 else
		 {
			float distanceFromCenterFar = min(distance(vec3(0.0), fs_in.FragPosDirLightSpaceFar.xyz), 1.0);
			if (distanceFromCenterFar <= 0.9)
			{
				shadow = ShadowCalculation(
					fs_in.FragPosDirLightSpaceFar, 
					light.shadowMapFar, 
					1,
					fragNormal, 
					lightDirection
				);
			}
			else
			{
				float percent = 1.0 - (distanceFromCenterFar - 0.9) * 10.0;
				shadow = percent * ShadowCalculation(
					fs_in.FragPosDirLightSpaceFar, 
					light.shadowMapFar, 
					0,
					fragNormal, 
					lightDirection
				);
			}
		 }
	}

	return	(ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalculatePointLightImpact(
	PointLight light, 
	vec3 fragNormal, 
	vec3 viewDirection,
	vec3 baseAmbient,
	vec3 baseDiffuse,
	vec3 baseSpecular
)
{
	float distance = length(light.position - fs_in.FragPos);
	float attenuation = 1.0 
		/ (light.attenuation.constant 
			+ light.attenuation.linear 
			* distance 
			+ light.attenuation.quadratic 
			* distance 
			* distance
		);

	vec3 lightDirection = normalize(light.position - fs_in.FragPos);
	vec3 reflectDirection = reflect(-lightDirection, fragNormal);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);

	vec3 ambient = light.properties.ambient 
		* baseAmbient;

	float diffuseIntensity = max(dot(fragNormal, lightDirection), 0.0);
	vec3 diffuse = light.properties.diffuse 
		* diffuseIntensity 
		* baseDiffuse;

	float specularIntensity = pow(max(
		dot(fragNormal, halfwayDirection), 0.0), 
		u_Material.shininess * 128);
	vec3 specular = light.properties.specular 
		* specularIntensity 
		* baseSpecular;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return	(ambient + diffuse + specular);
}

vec3 CalculateSpotLightImpact(
	SpotLight light, 
	vec3 fragNormal, 
	vec3 viewDirection,
	vec3 baseAmbient,
	vec3 baseDiffuse,
	vec3 baseSpecular
)
{
	float distance = length(light.position - fs_in.FragPos);
	float attenuation = 1.0 
		/ (light.attenuation.constant 
			+ light.attenuation.linear * distance 
			+ light.attenuation.quadratic * distance * distance
		);

	vec3 lightDirection = normalize(light.position - fs_in.FragPos);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 ambient = light.properties.ambient 
		* baseAmbient;

	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.innerCone - light.outerCone;
	float intensity = clamp((theta - light.outerCone) / epsilon, 0.0, 1.0);
	if (intensity > 0.0)
	{
		float diff = max(dot(fragNormal, lightDirection), 0.0);
		diffuse = light.properties.diffuse 
			* diff 
			* baseDiffuse;

		vec3 reflectDirection = reflect(-lightDirection, fragNormal);
		float specularIntensity = pow(
			max(dot(fragNormal, halfwayDirection), 0.0), 
			u_Material.shininess * 128
		);
		specular = light.properties.specular 
			* specularIntensity 
			* baseSpecular;
	}

	ambient *= attenuation;
	diffuse *= intensity * attenuation;
	specular *= intensity * attenuation;

	return	(ambient + diffuse + specular);
}
