#pragma once
#include "Effect_Define.h"
#include "BlendObject.h"
#include "Imgui_Manager.h"

struct EffectInfo
{
	_uint eType{};
	_bool isSprite{};
	CVIBuffer_Instancing::ParticleDesc PartiDesc{};
	_uint iNumInstances{};
	_float fLifeTime{};
	_int iDiffTextureID{};
	_int iMaskTextureID{};
	_vec4 vColor{};
	_uint iPassIndex{};
};

BEGIN(Effect)

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
	CVIBuffer_Instancing_Point* m_pParticle{ nullptr };
	CVIBuffer_Rect* m_pRect{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CTexture* m_pMaskTextureCom{};
	CTexture* m_pDiffTextureCom{};

private:
	EffectInfo m_Effect{};
	_float m_fTimer{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Dummy* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END