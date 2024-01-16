#pragma once
#include "Client_Define.h"
#include "Base.h"
#include "GameInstance.h"

BEGIN(Client)

struct EffectInfo
{
	_uint eType{};
	_bool isSprite{};
	_int2 vNumSprites{};
	_float fSpriteDuration{};
	CVIBuffer_Instancing::ParticleDesc PartiDesc{};
	_uint iNumInstances{};
	_float fLifeTime{};
	_int iDiffTextureID{};
	_int iMaskTextureID{};
	_vec4 vColor{};
	_uint iPassIndex{};
	_vec2 vSize{};
	_vec3* pPos{};
	_vec3 vPosOffset{};
	_vec2 vSizeDelta{};
	_bool bApplyGravity{};
	_vec3 vGravityDir{};
	_vec3 vPos{};
};

enum Effect_Type
{
	ET_PARTICLE,
	ET_RECT,
	ET_END
};

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)
private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	EffectInfo Get_EffectInformation(const wstring& strEffectTag);
	HRESULT Add_Layer_Effect(EffectInfo* pInfo);
	class CEffect_Dummy* Clone_Effect(EffectInfo* pInfo);

public:
	HRESULT Read_EffectFile();

private:
	CGameInstance* m_pGameInstance{ nullptr };
	map<const wstring, EffectInfo> m_Effects{};

public:
	virtual void Free() override;
};

END