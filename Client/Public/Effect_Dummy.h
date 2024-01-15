#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

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
	_vec3 vPos{};
	_vec3 vPosOffset{};
	_vec2 vSizeDelta{};
};

enum Effect_Type
{
	ET_PARTICLE,
	ET_RECT,
	ET_END
};

class CEffect_Dummy final : public CBlendObject
{
private:
	CEffect_Dummy(_dev pDevice, _context pContext);
	CEffect_Dummy(const CEffect_Dummy& rhs);
	virtual ~CEffect_Dummy() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CVIBuffer_Instancing_Point* m_pParticle{ nullptr };
	CVIBuffer_Rect* m_pRect{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CTexture* m_pMaskTextureCom{};
	CTexture* m_pDiffTextureCom{};

private:
	EffectInfo m_Effect{};
	_mat m_WorldMatrix{};
	_float m_fTimer{};
	_float m_fSpriteTimer{};
	_int m_iSpriteIndex{};
	_vec2 m_vScaleAcc{ 1.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Dummy* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END