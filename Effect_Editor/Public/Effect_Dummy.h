#pragma once
#include "Effect_Define.h"
#include "BlendObject.h"

enum class EffectType
{
	Particle,
	Rect,
	End
};

struct EffectInfo
{
	EffectType eType{};
	CVIBuffer_Instancing::ParticleDesc PartiDesc{};
	_uint iNumInstances{};
	CShader** ppShader{ nullptr };
	_float fLifeTime{};
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
	EffectInfo m_Effect{};
	_float m_fTimer{};
	CShader** m_ppShaderCom{ nullptr };

private:
	HRESULT Add_Components();

public:
	static CEffect_Dummy* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END