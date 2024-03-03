#include "Log.h"

CLog::CLog(_dev pDevice, _context pContext)
    :CGameObject(pDevice, pContext)
{
}

CLog::CLog(const CLog& rhs)
    :CGameObject(rhs)
{
}

HRESULT CLog::Init_Prototype()
{
    return S_OK;
}

HRESULT CLog::Init(void* pArg)
{
    PxCapsuleControllerDesc ControllerDesc{};
    ControllerDesc.height = 1.2f; // 높이(위 아래의 반구 크기 제외
    ControllerDesc.radius = 0.5f; // 위아래 반구의 반지름
    ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
    ControllerDesc.slopeLimit = cosf(PxDegToRad(1.f)); // 캐릭터가 오를 수 있는 최대 각도
    ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
    ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이
    m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

    if (pArg)
    {
        LOG_DESC* pLogDesc = reinterpret_cast<LOG_DESC*>(pArg);

        m_pTransformCom->Set_Matrix(pLogDesc->WorldMatrix);
    }

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pGameInstance->Register_CollisionObject(this, m_pColliderCom);

    m_iPassIndex = StaticPass_Dissolve;
    m_fDissolveRatio = 1.f;

    random_device rand;
    m_RandomNumber = _randNum(rand());

    return S_OK;
}

void CLog::Tick(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());

    if (m_IsFall)
    {
        m_fFallTime += fTimeDelta;

        m_pTransformCom->Set_OldMatrix();
        m_pTransformCom->Gravity(fTimeDelta);
        if (not m_pTransformCom->Is_Jumping())
        {
            if (m_fJumpForce <= 1.f)
            {
                m_fDissolveRatio += fTimeDelta;
                m_iPassIndex = StaticPass_Dissolve;

                m_pTransformCom->Delete_Controller();
                m_pGameInstance->Delete_CollisionObject(this);
                if (m_fDissolveRatio >= 1.f)
                {
                    Kill();
                }
            }
            else if (m_fJumpForce >= 0.f)
            {
                m_pTransformCom->Jump(m_fJumpForce);

                _randInt RandomSound(1, 4);
                wstring SoundTag = TEXT("Hit_Huge_Wood_SFX_0") + to_wstring(RandomSound(m_RandomNumber));
                m_pGameInstance->Play_Sound(SoundTag, m_fJumpForce * 0.05f);
                m_pGameInstance->Play_Sound(TEXT("Ask_Matriarch_Attack05_SFX_02"), m_fJumpForce * 0.01f);
            }
        }
        else
        {
            m_fJumpForce -= fTimeDelta * 5.f;
        }

        m_pGameInstance->Attack_Monster(m_pColliderCom, 999, AT_Critical);
    }
    else
    {
        if (m_iPassIndex == StaticPass_Dissolve)
        {
            m_fDissolveRatio -= fTimeDelta;
            if (m_fDissolveRatio <= 0.f)
            {
                m_iPassIndex = StaticPass_OutLine;
                m_fDissolveRatio = 0.f;
            }
        }
    }
}

void CLog::Late_Tick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
#ifdef _DEBUG
    m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif // DEBUG
}

HRESULT CLog::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        return E_FAIL;
    }

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
        {
            _bool bFailed = true;
        }

        _bool HasNorTex{};
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
        {
            HasNorTex = false;
        }
        else
        {
            HasNorTex = true;
        }

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pModelCom->Render(i)))
        {
            return E_FAIL;
        }
    }
    return S_OK;
}

void CLog::Set_Damage(_int iDamage, _uint MonAttType)
{
    if (MonAttType == AT_Bow_Skill2)
    {
        m_IsFall = true;
    }
}

HRESULT CLog::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_TOWER, TEXT("Prototype_Model_Log"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Noise"), TEXT("Com_Dissolve_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
    {
        return E_FAIL;
    }

	Collider_Desc ColliderDesc{};
	ColliderDesc.eType = ColliderType::OBB;
	ColliderDesc.vExtents = _vec3(0.1f, 0.5f, 2.f);
	ColliderDesc.vCenter = _vec3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
	{
		return E_FAIL;
	}

    return S_OK;
}

HRESULT CLog::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _vector)))
    {
        return E_FAIL;
    }

    if (true == m_pGameInstance->Get_TurnOnShadow()) {

        CASCADE_DESC Desc = m_pGameInstance->Get_CascadeDesc();

        if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeView", Desc.LightView, 3)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProj", Desc.LightProj, 3)))
            return E_FAIL;

    }

    if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof _float)))
    {
        return E_FAIL;
    }

    _uint iOutlineColor = OutlineColor_White;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_OutlineColor", &iOutlineColor, sizeof _uint)))
    {
        return E_FAIL;
    }

    return S_OK;
}

CLog* CLog::Create(_dev pDevice, _context pContext)
{
	CLog* pInstance = new CLog(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLog::Clone(void* pArg)
{
	CLog* pInstance = new CLog(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLog::Free()
{
	__super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pDissolveTextureCom);
}
