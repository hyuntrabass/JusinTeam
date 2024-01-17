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

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_Scale(_vec3(m_Effect.vSize.x, m_Effect.vSize.y, 1.f));
	m_vScaleAcc = m_Effect.vSize;

	return S_OK;
}

void CEffect_Dummy::Tick(_float fTimeDelta)
{
	if (m_Effect.fLifeTime >= 0.f and m_fTimer > m_Effect.fLifeTime)
	{
		if (m_Effect.iDissolveTextureID >= 0)
		{
			m_fDissolveRatio += fTimeDelta / m_Effect.fDissolveDuration;
			if (m_fDissolveRatio > 1.f)
			{
				m_isDead = true;
			}
		}
		else
		{
			m_isDead = true;
		}
	}

	if (m_Effect.isSprite)
	{
		m_iSpriteIndex = static_cast<_int>(m_fSpriteTimer);
		m_fSpriteTimer += (fTimeDelta * m_Effect.vNumSprites.x * m_Effect.vNumSprites.y) / m_Effect.fSpriteDuration;
		if (m_iSpriteIndex >= m_Effect.vNumSprites.x * m_Effect.vNumSprites.y)
		{
			m_iSpriteIndex = 0;
			m_fSpriteTimer = {};
		}
	}

	m_fTimer += fTimeDelta;

	if (m_Effect.pPos)
	{
		m_pTransformCom->Set_Position(*m_Effect.pPos);
	}
	else
	{
		m_pTransformCom->Set_Position(m_Effect.vPos);
	}

	if (m_Effect.iType == Effect_Type::ET_PARTICLE)
	{
		m_pParticle->Update(fTimeDelta, m_pTransformCom->Get_World_Matrix(), m_Effect.iNumInstances, m_Effect.bApplyGravity, m_Effect.vGravityDir);
		m_WorldMatrix = m_pTransformCom->Get_World_Matrix();
	}
	else
	{
		m_pTransformCom->LookAway(m_pGameInstance->Get_CameraPos());

		m_pTransformCom->Set_Scale(_vec3(m_vScaleAcc.x, m_vScaleAcc.y, 1.f));
		m_vScaleAcc.x += m_Effect.vSizeDelta.x * fTimeDelta;
		m_vScaleAcc.y += m_Effect.vSizeDelta.y * fTimeDelta;

		m_WorldMatrix = m_pTransformCom->Get_World_Matrix();

		_vec4 vPos = m_WorldMatrix.Position();;

		_vec4 vRight = m_WorldMatrix.Right().Get_Normalized();
		_vec4 vUp = m_WorldMatrix.Up().Get_Normalized();
		_vec4 vLook = m_WorldMatrix.Look().Get_Normalized();

		vPos += vRight * m_Effect.vPosOffset.x;
		vPos += vUp * m_Effect.vPosOffset.y;
		vPos += vLook * m_Effect.vPosOffset.z;

		m_WorldMatrix.Position(vPos);
	}
}

void CEffect_Dummy::Late_Tick(_float fTimeDelta)
{
	__super::Compute_CamDistance();
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	m_pRendererCom->Add_RenderGroup(RG_Blur, this);
}

HRESULT CEffect_Dummy::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(m_Effect.iPassIndex)))
	{
		return E_FAIL;
	}

	HRESULT hr{};
	switch (m_Effect.iType)
	{
	case Effect_Type::ET_PARTICLE:
		hr = m_pParticle->Render();
		break;
	case Effect_Type::ET_RECT:
		hr = m_pRect->Render();
		break;
	}

	return hr;
}

HRESULT CEffect_Dummy::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	switch (m_Effect.iType)
	{
	case Effect_Type::ET_PARTICLE:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Point"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pParticle), &m_Effect.PartiDesc)))
		{
			return E_FAIL;
		}
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Instancing"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
		break;
	case Effect_Type::ET_RECT:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pRect))))
		{
			return E_FAIL;
		}
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
		break;
	}

	if (m_Effect.iMaskTextureID >= 0)
	{
		wstring PrototypeTag = L"Prototype_Component_Texture_Effect_" + to_wstring(m_Effect.iMaskTextureID);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, PrototypeTag, TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom)))
		{
			return E_FAIL;
		}
	}

	if (m_Effect.iDissolveTextureID >= 0)
	{
		wstring PrototypeTag = L"Prototype_Component_Texture_Effect_" + to_wstring(m_Effect.iDissolveTextureID);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, PrototypeTag, TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTextureCom)))
		{
			return E_FAIL;
		}
	}

	if (m_Effect.iDiffTextureID >= 0)
	{
		wstring PrototypeTag = L"Prototype_Component_Texture_Effect_" + to_wstring(m_Effect.iDiffTextureID);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, PrototypeTag, TEXT("Com_DiffTexture"), (CComponent**)&m_pDiffTextureCom)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEffect_Dummy::Bind_ShaderResources()
{
	if (m_Effect.iMaskTextureID >= 0)
	{
		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		{
			return E_FAIL;
		}
	}

	if (m_Effect.iDissolveTextureID >= 0)
	{
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof m_fDissolveRatio)))
		{
			return E_FAIL;
		}
	}

	if (m_Effect.iDiffTextureID >= 0)
	{
		if (FAILED(m_pDiffTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_Effect.vColor, sizeof _vec4)))
		{
			return E_FAIL;
		}
	}

	if (m_Effect.iType == ET_PARTICLE)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _vec4)))
		{
			return E_FAIL;
		}
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}

	if (m_Effect.isSprite)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &m_iSpriteIndex, sizeof m_iSpriteIndex)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &m_Effect.vNumSprites, sizeof m_Effect.vNumSprites)))
		{
			return E_FAIL;
		}
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
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pDiffTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pParticle);
	Safe_Release(m_pRect);
}
