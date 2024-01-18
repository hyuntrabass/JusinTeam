#pragma once
#include "Effect_Define.h"
#include "Base.h"
#include "GameInstance.h"

BEGIN(Effect)

enum Effect_Type
{
	ET_PARTICLE,
	ET_RECT,
	ET_MESH,
	ET_END
};

struct OldEffectInfo
{
	unsigned int iType{};
	bool isSprite{};
	XMINT2 vNumSprites{};
	float fSpriteDuration{};
	ParticleDesc PartiDesc{};
	unsigned int iNumInstances{};
	float fLifeTime{};
	int iDiffTextureID{};
	int iMaskTextureID{};
	SimpleMath::Vector4 vColor{};
	unsigned int iPassIndex{};
	SimpleMath::Vector2 vSize{};
	SimpleMath::Vector3* pPos{};
	SimpleMath::Vector3 vPosOffset{};
	SimpleMath::Vector2 vSizeDelta{};
	bool bApplyGravity{};
	SimpleMath::Vector3 vGravityDir{};
	SimpleMath::Vector3 vPos{};
	int iDissolveTextureID{};
	float fDissolveDuration{};
	unsigned int iModelIndex{};
	bool bSkipBloom{};
	int iUnDissolveTextureID{};
	float fUnDissolveDuration{};
	SimpleMath::Vector2 vUVDelta{};
	bool isRandomSprite{};
};

class CImgui_Manager : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)
private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT Init(_dev pDevice, _context pContext, vector<string>* pTextureList, vector<string>* pModelList);
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	CGameInstance* m_pGameInstance{ nullptr };
	//CVIBuffer_Rect* m_pVIBuffer_Rect{ nullptr };
	//CVIBuffer_Instancing_Point* m_pVIBuffer_Instancing{ nullptr };

private:
	class CEffect_Dummy* m_pEffect{ nullptr };
	list<class CEffect_Dummy*> m_AddEffect{};
	//class CShader* m_pInstanceShader{ nullptr };
	//class CShader* m_pVtxTexShader{ nullptr };
	_int m_iCurrent_Type{};
	_int m_iSelected_Texture{};
	_int m_iSelected_MaskTexture{};

	const _char** m_pItemList_Texture{ nullptr };
	_int m_iNumTextures{};
	CTexture** m_pTextures{ nullptr };

	_int m_iSelected_Model{};
	const _char** m_pItemList_Model{ nullptr };
	_int m_iNumModels{};
	CModel** m_pModels{ nullptr };

	_vec4 m_vColor{ 1.f };
	_uint m_iShaderType{};
	_float m_fEffectLifeTime{ 1.f };
	_int m_iNumInstance{ 30 };
	_bool m_hasDiffTexture{};
	_bool m_hasMask{};
	ParticleDesc m_ParticleInfo{};

private:
	HRESULT Ready_Layers();

	const _int& Compute_TextureIndex(const wstring& strTexture);
	const _int& Compute_ModelIndex(const string& strModel);

	EffectInfo Load_Data();
	OldEffectInfo Load_OldData();
	HRESULT Export_Data(EffectInfo& Info);

public:
	virtual void Free() override;
};

END