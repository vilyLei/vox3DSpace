#include "/SDShaderParams.h"

layout(location = 0) in vec3 v_worldPosition;
layout(location = 1) in vec3 v_position;
layout(location = 2) in flat uint v_flag;
layout(location = 3) in vec3 v_normal;
layout(location = 4) in vec2 v_texCoord;
layout(location = 5) in vec4 v_color;
//layout(location = 7) in int gl_PrimitiveID;
layout(location = 7) in vec4 v_glPosition;

#ifdef SHADING_WITH_EARLY_Z
//layout(early_fragment_tests) in;
#endif

#ifdef SHADING_WITH_SHADOW
layout(binding = BINDING_TEXTURE_DEPTH) uniform sampler2DShadow texDepthMap;
SHADER_LIGHT_MATRIX_DECLARE
#endif

#include "/Material.h"
#include "/PBR.h"
#include "/Math.h"
#include "/Laser.h"

out vec4 FragColor;

SHADER_CAMERA_DECLARE

//#define MATERIAL_WITH_TEX_NORMAL
//#define SHADING_WITH_AO
//#define SHADING_WITH_FACE_NORMAL
#define SHADING_WITH_DOUBLE_SIDE
#define SHADING_WITH_ENVIRONMENT
#define SHADING_WITH_PBR

layout(binding = BINDING_TEXTURE_ENV_DIFFUSE) uniform samplerCube texEnvDiffuse;
layout(binding = BINDING_TEXTURE_ENV_SPECULAR) uniform samplerCube texEnvSpecular;

layout(binding = BINDING_TEXTURE_AO) uniform sampler2D texAO;
layout(binding = BINDING_TEXTURE_MODEL_NORMAL) uniform sampler2D texBakingNormal;
layout(binding = BINDING_TEXTURE_MODEL_LIGHTMAP) uniform sampler2D texBakingLightmap;

layout(std140, binding = BINDING_CB_SCENE) uniform SceneConstant
{
	SDParamScene g_Scene;
};

#ifdef SHADING_WITH_PBR
layout(std140, binding = BINDING_CB_LIGHTS) uniform LightConstant
{
	SDParamLightArray g_LightArray;
};
#endif //SHADING_WITH_PBR

float F_Schlick(float VoH, float f0, float f90) 
{
	return f0 + (f90 - f0) * pow(1.0 - VoH, 5.0);
}

float Fd_Burley(float linearRoughness, float NoV, float NoL, float LoH)
{
	float f90 = 0.5 + 2.0 * linearRoughness * LoH * LoH;
	float lightScatter = F_Schlick(NoL, 1.0, f90);
	float viewScatter = F_Schlick(NoV, 1.0, f90);
	return lightScatter * viewScatter;
}

float GetSideFactorIntensity(float NoV, float frontScale, float sideScale)
{
	float invNoV = 1.0 - NoV;
	float x2 = invNoV * invNoV;
	float x3 = x2 * invNoV;
	float x4 = x3 * invNoV;
	float x5 = x4 * invNoV;
	float y = clamp(-10.15 * x5 + 20.189 * x4 - 12.811 * x3 + 4.0585 * x2 - 0.2931 * invNoV, 0.0, 1.0);
	float sideFactorIntensity = mix(frontScale, sideScale, y);
	return sideFactorIntensity;
}

float SideCorrection(float b)
{
    return 0.015+0.0006/(b+0.015);
}

vec3 GetDiffussColorSide(vec3 mixedDiffuse, float bCorrection, float sideInt, float tanNL, float tanNV)
{
	sideInt *= 140.0;
	float side = InvGauss( 0.4*sideInt*bCorrection, tanNL, 0.3) * InvGauss( 0.4*sideInt*bCorrection, tanNV, 0.4) - 1.0;
	return mixedDiffuse * side;
}

vec3 GetSideEnvDiffuseColor(vec3 mixedDiffuse, float sideInt, float tanNV){
	//sideInt *= 1.0;
	float side = InvGauss( 1.4*sideInt, tanNV, 0.4) - 1.0;
	return mixedDiffuse * side;
}

#define  OneOnLN2_x6 8.656171f// == 1/ln(2) * 6 (6 is SpecularPower of 5 + 1)
vec3 FresnelSchlick(vec3 specularColor, float dot, float glossiness)
{
	return specularColor + (max(vec3(glossiness), specularColor) - specularColor) * exp2(-OneOnLN2_x6 * dot); 
}



void GetTB(vec3 position, vec3 normal, vec2 texCoord, out vec3 outTangent, out vec3 outBinormal)
{
	// compute derivations of the world position
	vec3 p_dx = dFdx(position);
	vec3 p_dy = dFdy(position);
	// compute derivations of the texture coordinate
	vec2 tc_dx = dFdx(texCoord);
	vec2 tc_dy = dFdy(texCoord);
	// compute initial tangent and bi-tangent
	vec3 t = normalize(-tc_dy.y * p_dx + tc_dx.y * p_dy);
	vec3 b = normalize(tc_dy.x * p_dx - tc_dx.x * p_dy);
	// sign inversion
	// get new tangent from a given mesh normal
	t = cross(cross(normal, t), normal);
	b = cross(normal, cross(b, normal));
	outTangent = normalize(t);
	outBinormal = normalize(b);
}

vec3 PerturbNormal(vec3 N, vec3 V)
{
bool hasNormalMap = false;
VEC3 texNormalValue;
#if (defined MATERIAL_WITH_TEX_NORMAL) || (defined MATERIAL_WITH_TEX_NORMAL_BACK)
	vec2 uvTex = (g_material.uvMatrixNormal * VEC4(v_texCoord.xy, 0, 1)).xy;
	mat3 TBNTex = CotangentFrame(N, V, uvTex);
	float strength = g_material.normalMapStrength;//#TODO ����ͼ����
	if (g_isFront) {
#ifdef MATERIAL_WITH_TEX_NORMAL
		texNormalValue = texture2D(texNormal, uvTex).xyz;
		hasNormalMap = true;
#endif //MATERIAL_WITH_TEX_NORMAL
	}else {
#ifdef MATERIAL_WITH_TEX_NORMAL_BACK
		texNormalValue = texture2D(texNormalBack, uvTex).xyz;
		hasNormalMap = true;
#endif //MATERIAL_WITH_TEX_NORMAL_BACK
	}
	if(hasNormalMap)
	{
		texNormalValue = texNormalValue * 2.0 - 1.0;
		texNormalValue.z *= abs(2.6 / (abs(strength * 2.0) - 0.01 ) - 0.306);
		//texNormalValue.z = 3.0 / ((abs(strength * 10.5) - 1.0)*0.311 + 1.0);
		texNormalValue.xy *= sign(strength);
		texNormalValue = normalize(TBNTex * texNormalValue);
	}

#endif

bool hasBakingMap = false;
vec3 texBakingValue;
#ifdef SHADING_WITH_BAKING_NORMAL
	vec2 uvBaking = (g_material.uvBakingMatrix * VEC4(v_texCoord.x, v_texCoord.y, 0, 1)).xy;
	mat3 TBNBaking = CotangentFrame(N, V, uvBaking);
	texBakingValue = texture(texBakingNormal, uvBaking).xyz;
	texBakingValue = texBakingValue * 2.0 - 1.0;
	texBakingValue = normalize(TBNBaking * texBakingValue);
	hasBakingMap = true;
#endif

	if(hasNormalMap &&hasBakingMap)
	{
		float z0 = dot(N, texNormalValue);
		float z1 = dot(N, texBakingValue);
		return normalize(texNormalValue - z0 * N + texBakingValue - z1 * N + z0 * z1 * N);
	}
	if(hasNormalMap && !hasBakingMap)
		return texNormalValue;
	if(!hasNormalMap && hasBakingMap)
		return texBakingValue;	
	return N;
}

VEC3 Rotate(VEC3 dir, float radian)
{
	VEC3 result;
	result.x = cos(radian) * dir.x - sin(radian) * dir.z;
	result.y = dir.y;
	result.z = sin(radian) * dir.x + cos(radian) * dir.z;
	return result;
}
VEC3 GetEnvDir(float rotateAngle, VEC3 normal)
{
	VEC3 worldNormal = mat3(g_Camera.V_Inv) * normal;
	vec3 worldInvE = -normalize(g_Camera.eye.xyz - v_worldPosition);
	vec3 worldR = reflect(worldInvE, normalize(worldNormal));
	worldR.z = -worldR.z;
	worldR.y = -worldR.y;
	worldR = Rotate(worldR, rotateAngle);
	float preX = worldR.x;
	float preZ = worldR.z;
	return worldR;
}

vec3 GetFaceNormal(vec3 pos) {
	vec3 fdx = dFdx(pos);
	vec3 fdy = dFdy(pos);
	return normalize(cross(fdx, fdy));
}

void main()
{

	//��ʼ��������Ϣ
	InitMaterial();

	//���߻�ȡ
	vec3 normal = normalize(v_normal);
#ifdef SHADING_WITH_FACE_NORMAL
	normal = GetFaceNormal(v_position);
#endif
	//˫����Ⱦ��������
#ifdef SHADING_WITH_DOUBLE_SIDE
	if (!gl_FrontFacing) 
		normal = -normal;
#endif
	//������ͼ��������
	vec3 N = PerturbNormal(normal, v_position);

	//��������
	vec3 V = -normalize(v_position); //Eye - v_Position
	float dotNV = clamp(dot(N, V), 0.0f, 1.0f);
	float dotNormalV = clamp(dot(normalize(normal), V), 0.0f, 1.0f);
	float crossNormalV = sqrt(1 - dotNormalV*dotNormalV);

	//����ģʽ
#ifdef SHADING_WITH_NORMAL_DEBUG
	FragColor = VEC4(0.5*N + 0.5, 1.0);
	return;
#endif //SHADING_WITH_NORMAL_DEBUG

	//��ɫģʽ
#ifdef VBO_WITH_COLOR
	if (v_color.x >= -0.1)
	{
		FragColor = v_color;
		return;
	}
#endif //VBO_WITH_COLOR

	//��ȡ��Ԥ��Ĳ��ʲ���
#if defined DEBUG_SILK
	int matIsSpecularPass = 1;//
	float matReflectionIntensity = 0.75f; //����ǿ��
	float matFrontColorScale = 1.0f; //������ɫ����
	float matSideColorScale = 1.0f;//������ɫ����
	float matMetalness = 0.0f; //������
	g_material.roughness = 0.35f; //�⻬��
	float sideLightIntensity = 0.0f;//�����ǿ��
	g_material.reflection = matReflectionIntensity;
#elif defined DEBUG_MATTE
	int matIsSpecularPass = 0;//
	float matReflectionIntensity = 0.15f; //����ǿ��
	float matFrontColorScale = 1.0f; //������ɫ����
	float matSideColorScale = 1.0f;//������ɫ����
	float matMetalness = 0.0f; //������
	g_material.roughness = 0.5f; //�⻬��
	float sideLightIntensity = 0.0f;//�����ǿ��
	g_material.reflection = matReflectionIntensity;
#elif defined DEBUG_SHINY
	int matIsSpecularPass = 0;//
	float matReflectionIntensity = 0.4f; //����ǿ��
	float matFrontColorScale = 1.0f; //������ɫ����
	float matSideColorScale = 1.0f;//������ɫ����
	float matMetalness = 0.0f; //������
	g_material.roughness = 0.3f; //�⻬��
	float sideLightIntensity = 0.0f;//�����ǿ��
	g_material.reflection = matReflectionIntensity;
#else
	int matIsSpecularPass = g_material.isSpecularPass;//
	float matReflectionIntensity = g_material.reflection;//����ǿ��
	float matFrontColorScale = g_material.frontScale;//������ɫ����
	float matSideColorScale = g_material.sideScale;//������ɫ����
	float matMetalness = GetMetalness();//������//g_material.metalness;
	float sideLightIntensity = g_material.sideLight;
#endif
	//�ֲڶȺ����ϵ��
	float matRoughness = 1- GetGlossiness(); 
	float glossinessLinear = (1-matRoughness) * (1-matRoughness);
	float specularPower = exp2(10.0 * glossinessLinear + 1.0);
	//������ɫ
	VEC4 colorInput = GetDiffuse();//VEC4(0.8, 0.125, 0.125, 1);
	VEC3 matBaseColor = colorInput.rgb;	
	//͸���ȡ������ɫ��˥��ϵ��
	float matAlpha = GetAlpha(colorInput.w);
	vec4 blendColor = g_material.blendColor;	
	float frontAttenuation = g_frontAttenuation;

	//����diffuseColor��specularColor
	VEC3 diffuseColor, specularColor;
	VEC3 baseColorInv = GammaCorrectionInv(matBaseColor.rgb);
	if (matIsSpecularPass == 1)
	{
		diffuseColor = GammaCorrectionInv(0.8f * matBaseColor);
		specularColor = GammaCorrectionInv(matReflectionIntensity * (matBaseColor + VEC3(0.1) ) );		
	}
	else
	{
		specularColor = vec3(mix(0.025 * matReflectionIntensity, 0.078 * matReflectionIntensity, 1-matRoughness));
		if (matMetalness != 0.0)
		{
			vec3 specularColorMetal = mix(vec3(0.04), baseColorInv, matMetalness);
			float ratio = clamp(8 * matMetalness, 0, 1);
			specularColor = mix(specularColor , specularColorMetal , ratio);

		}
		diffuseColor = mix(baseColorInv, vec3(0.0), matMetalness);
	}

	//����������ǿ��
	float sideFactorIntensity = GetSideFactorIntensity(dotNV, matFrontColorScale, matSideColorScale);
	diffuseColor *= sideFactorIntensity;
	specularColor *= sideFactorIntensity * matFrontColorScale;

	vec3 diffuseTotal = vec3(0.0);
	vec3 specularTotal = vec3(0.0);

	//�����
	vec3 sideMixedColor = mix(diffuseColor , vec3(1.0) , 0.05);
	float sideCorrection = SideCorrection( ColorIntensity(diffuseColor));


	Laser laser;
#ifdef MATERIAL_WITH_LASER
	laser = LaserCreate(dotNV, GetLaserParam());
#endif

#ifdef SHADING_WITH_ENVIRONMENT
	//����ͼ����
	float envIntensity = g_Scene.envLightIntensity; //����ͼ����
	vec3 envDir = GetEnvDir(g_Scene.envLightRotateAngle, N);
	//Diffuse
	vec3 diffuseEnvColor = frontAttenuation * envIntensity * (g_Scene.envLightColor.xyz) * texture(texEnvDiffuse, envDir).xyz;
#ifdef SHADING_WITH_BAKING_LIGHTMAP
	if(gl_FrontFacing)
	{
		float lightmapIntensity = g_Scene.lightmapIntensity;
	    vec2 uvBaking = (g_material.uvBakingMatrix * VEC4(v_texCoord.x, v_texCoord.y, 0, 1)).xy;
		vec3 lightMapColor = 0.8 * texture(texBakingLightmap, uvBaking).xyz;
		diffuseEnvColor = mix(diffuseEnvColor,lightMapColor , lightmapIntensity);
	}
#endif
	//�����
	vec3 sideEnvDiffuseColor = GetSideEnvDiffuseColor(sideMixedColor,sideLightIntensity,dotNormalV/crossNormalV);
	vec3 diffuseTotalEnv = (diffuseColor + sideEnvDiffuseColor) * diffuseEnvColor;
	diffuseTotalEnv = LaserGetDiffuseTotalEnv(laser, diffuseTotalEnv);
	diffuseTotal.rgb +=  diffuseTotalEnv;
	//Specular
	float mipLevel = 7.0 - glossinessLinear * 7.0;
	vec3 specularEnvColor = envIntensity * textureLod(texEnvSpecular, envDir, mipLevel).xyz;
	if(gl_FrontFacing)
	{
		vec3 specularTotalEnv = FresnelSchlick(specularColor, dotNV, 0.25 * matReflectionIntensity) * specularEnvColor;
		specularTotalEnv = LaserGetSpecularTotalEnv(laser, specularTotalEnv, envIntensity, texEnvSpecular, envDir, dotNV);
		specularTotal.rgb += specularTotalEnv;
	}
#endif

	//��Ӱ����
#ifdef SHADING_WITH_SHADOW
	vec4 lightPosition = g_lightMatrix.VP * VEC4(v_worldPosition, 1);
	lightPosition /= lightPosition.w;
	lightPosition = 0.5*lightPosition + 0.5;
	//��Ӱ���ƫ�Ƽ��㡣���ߺ͹��߽Ƕȴ�,ƫ��Խ�󣻷�Χ��[0.0002, 0.001]
	VEC4 lightDir = g_LightArray.lights[3].direction;
	if(g_LightArray.lights[3].isFollowCamera == 0)
	{
		lightDir = g_Camera.V * lightDir;
	}
	float dotNLShadow = dot(N, normalize(lightDir.xyz));
	if (dotNLShadow > 0.001) //��Ӱ�� ��Ӱ���ߺͷ��߷����෴��ʱ��������
	{
		float bias = max(0.001 * (1.0 - abs(dotNLShadow)), 0.0002);
		lightPosition.z -= bias;
		float shadow = texture(texDepthMap, lightPosition.xyz);
		shadow = 1 - (1 - shadow)*g_Scene.shadowIntensity;
		frontAttenuation = min(frontAttenuation, shadow);
	}

#endif //SHADING_WITH_SHADOW



#pragma region PBRLightComputing

	for (int iLight = 0; iLight < g_LightArray.lightNum; iLight++)
	{
		SDParamLight light = g_LightArray.lights[iLight];
		vec3 lightColor = pow(light.color.rgb, vec3(GAMMA)) * light.intensity;
		VEC4 lightDir = light.direction; //����������ϵ�ķ���ת���ӵ�����ϵ
		if(light.isFollowCamera == 0)
		{
			lightDir = g_Camera.V * lightDir;
		}
		VEC3 L = normalize(lightDir.xyz);
		vec3 H = normalize(L + V);
		float dotNL = clamp(dot(N, L), 0.0f, 1.0f);
		float crossNL = sqrt(1 - dotNL*dotNL);
		float dotNormalL = clamp(dot(normalize(normal), L), 0.0f, 1.0f);
		float crossNormalL = sqrt(1 - dotNormalL*dotNormalL);
		float dotLH = clamp(dot(L, H), 0.0f, 1.0f);
		float dotNH = clamp(dot(N, H), 0.0f, 1.0f);
		float fdBurley = Fd_Burley(matRoughness, dotNV, dotNL, dotLH);
		vec3 diffussColorSide = GetDiffussColorSide(sideMixedColor, sideCorrection, sideLightIntensity, dotNormalL/crossNormalL, dotNormalV/crossNormalV);

		vec3 diffuseMaterialColorLight = (diffuseColor + diffussColorSide) * fdBurley;
		diffuseMaterialColorLight = LaserGetDiffuseMaterialColorLight(laser, diffuseMaterialColorLight);

		vec3 diffuseTotalLight = frontAttenuation * lightColor * dotNL * diffuseMaterialColorLight;
		diffuseTotal.rgb += diffuseTotalLight;

		vec3 specularMaterialColorLight = FresnelSchlick(specularColor, dotLH, 1.0) * ((specularPower + 2.0) / 8.0) * pow(dotNH, specularPower);
		specularMaterialColorLight = LaserGetSpecularMaterialColorLight(laser, specularMaterialColorLight, dotLH, dotNH);

		vec3 specularTotalLight = frontAttenuation * lightColor * dotNL * specularMaterialColorLight;
		specularTotal.rgb += specularTotalLight;

	}

	

#pragma endregion PBRLightComputing

	
	specularTotal *= GammaCorrectionInv(GetSpecularColor()); //�߹���ɫ����

	FragColor.rgb = diffuseTotal + specularTotal;


#ifdef SHADING_WITH_AO
	vec2 size = textureSize(texAO, 0);
	VEC2 screenUV = gl_FragCoord.xy / size;
	float ao = texture2D(texAO, screenUV).r;
	ao = 1 - (1 - ao)*g_Scene.SSAOIntensity;
	FragColor.rgb = FragColor.rgb*ao;
#endif //SHADING_WITH_AO



	//�����ɫ����
	FragColor = FragColor*(1 - blendColor.a) + blendColor*blendColor.a;

	FragColor.rgb = GammaCorrection(FragColor.rgb);
	FragColor.a = matAlpha;


}

