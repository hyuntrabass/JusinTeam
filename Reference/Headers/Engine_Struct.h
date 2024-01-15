#pragma once


namespace Engine
{
	enum class ModelType
	{
		Static,
		Anim,
		Collision,
		End
	};

	using GRAPHIC_DESC = struct tagGraphic_Desc
	{
		HWND hWnd{};
		HINSTANCE hInst{};
		unsigned int iWinSizeX{}, iWinSizeY{};
		bool isWindowed{};
	};

	using LIGHT_DESC = struct tagLightDesc
	{
		enum TYPE { Directional, Point, End };

		TYPE eType{End};
		SimpleMath::Vector4 vDirection{};
		SimpleMath::Vector4 vPosition{};
		// Range, Constant, Linear, Quadratic
		SimpleMath::Vector4 vAttenuation{};

		SimpleMath::Vector4 vDiffuse{};
		SimpleMath::Vector4 vAmbient{};
		SimpleMath::Vector4 vSpecular{};
	};

	using Model_Material = struct tagMaterial
	{
		class CTexture* pMaterials[18]{};
	};

	using KEYFRAME = struct tagKeyFrame
	{
		float fTime{};
		SimpleMath::Vector4 vScaling{};
		SimpleMath::Vector4 vRotation{};
		SimpleMath::Vector4 vPosition{};
	};



	using ANIMTIME_DESC = struct tagAnimTimeDesc {
		float SwitchDuration = 0.2f;
		float SwitchRatio = 0.f;
		float SwitchTime = 0.f;
		float fPadding = 0.f;

	};

	using VTXPOSCOLOR = struct ENGINE_DLL tagVertex_Position_Color
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vColor{};

		static const unsigned int iNumElements{ 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXPOSTEX = struct ENGINE_DLL tagVertex_Position_Texcoord
	{
		XMFLOAT3 vPosition{};
		XMFLOAT2 vTexcoord{};

		static const unsigned int iNumElements{ 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXNORTEX = struct ENGINE_DLL tagVertex_Position_Normal_Texcoord
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};

		static const unsigned int iNumElements{ 3 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXCUBETEX = struct ENGINE_DLL tagVertex_Position_Normal_Texcoord_Cube
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT3 vTexcoord{};

		static const unsigned int iNumElements{ 3 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXSTATICMESH = struct ENGINE_DLL tagVertex_Static_Mesh
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};
		XMFLOAT3 vTangent{};

		static const unsigned int iNumElements{ 4 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXANIMMESH = struct ENGINE_DLL tagVertex_Anim_Mesh
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};
		XMFLOAT3 vTangent{};

		XMUINT4 vBlendIndices{};
		XMFLOAT4 vBlendWeights{};

		static const unsigned int iNumElements{ 6 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXINSTANCING = struct tagVertex_Instancing
	{
		XMFLOAT4 vRight{};
		XMFLOAT4 vUp{};
		XMFLOAT4 vLook{};
		XMFLOAT4 vPos{};

		float fSpeed{};
		XMFLOAT2 vLifeTime{};
		XMFLOAT4 vOriginPos{};
		XMFLOAT4 vDirection{};
	};

	using VTXPOINT = struct ENGINE_DLL tagVertex_Point
	{
		XMFLOAT3 vPosition{};
		XMFLOAT2 vPSize{};

		static const unsigned int iNumElements{ 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXTRAIL = struct ENGINE_DLL tagVertex_Trail
	{
		XMFLOAT3 vPosition{};
		XMFLOAT2 vPSize{};
		XMFLOAT4 vColor{};

		static const unsigned int iNumElements{ 3 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXPOINT_INSTANCING = struct ENGINE_DLL tagVertex_Point_Instancing
	{
		static const unsigned int iNumElements{ 6 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};
}