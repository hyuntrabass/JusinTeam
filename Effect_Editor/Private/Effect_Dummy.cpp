#include "Effect_Dummy.h"

CEffect_Dummy::CEffect_Dummy(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CEffect_Dummy::CEffect_Dummy(const CEffect_Dummy& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CEffect_Dummy::Init_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Dummy::Init(void* pArg)
{
	if (not pArg)
	{
		MSG_BOX("No Argument!");
		return E_FAIL;
	}

	m_Effect = *reinterpret_cast<EffectInfo*>(pArg);
	m_ppShaderCom = m_Effect.ppShader;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CEffect_Dummy::Tick(_float fTimeDelta)
{
	if (m_fTimer > m_Effect.fLifeTime)
	{
		m_isDead = true;
	}

	m_fTimer += fTimeDelta;

	if (m_Effect.eType == EffectType::Particle)
	{
		m_pParticle->Update(fTimeDelta, m_pTransformCom->Get_World_Matrix(), m_Effect.iNumInstances);
	}
	else
	{
		m_pTransformCom->LookAt(m_pGameInstance->Get_CameraPos());
	}
}

void CEffect_Dummy::Late_Tick(_float fTimeDelta)
{
}

HRESULT CEffect_Dummy::Render()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(*m_ppShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	(*m_ppShaderCom)->Begin(InstancingPass::InstPass_Particle_MaskColor);

	HRESULT hr{};
	switch (m_Effect.eType)
	{
	case EffectType::Particle:
		hr = m_pParticle->Render();
		break;
	case EffectType::Rect:
		hr = m_pRect->Render();
		break;
	}

	return hr;
}

HRESULT CEffect_Dummy::Add_Components()
{
	switch (m_Effect.eType)
	{
	case EffectType::Particle:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Point"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pParticle), &m_Effect.PartiDesc)))
		{
			return E_FAIL;
		}
		break;
	case EffectType::Rect:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pRect))))
		{
			return E_FAIL;
		}
		break;
	}

	return S_OK;
}

CEffect_Dummy* CEffect_Dummy::Create(_dev pDevice, _context pContext)
{
	CEffect_Dummy* pInstance = new CEffect_Dummy(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Dummy::Clone(void* pArg)
{
	CEffect_Dummy* pInstance = new CEffect_Dummy(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Dummy::Free()
{
	Safe_Release(m_pParticle);
	Safe_Release(m_pRect);
}
