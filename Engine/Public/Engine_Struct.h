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
		int iAnimIndex = 0;
		unsigned int iCurrFrame = 0;
		unsigned int iNextFrame = 0;
		float fRatio = 0.f;
		float fTime = 0.f;
		float fSpeed = 1.f;
		XMFLOAT2 vPadding;
	};

	using PLAYANIM_DESC = struct tagPlayAnimDesc {
		void PLAYANIM_DESC() {
			eCurrent.iAnimIndex = 0;
			eNext.iAnimIndex = -1;
		}

		void ResetNextAnim() {
			eNext.iAnimIndex = -1;
			eNext.iCurrFrame = 0;
			eNext.iNextFrame = 0;
			eNext.fTime = 0;
			SwitchTime = 0.f;
			SwitchRatio = 0.f;
		}

		float SwitchDuration = 0.2f;
		float SwitchRatio = 0.f;
		float SwitchTime = 0.f;
		float fPadding = 0.f;
		ANIMTIME_DESC eCurrent;
		ANIMTIME_DESC eNext;
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