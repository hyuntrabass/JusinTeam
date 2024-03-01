#include "BrickBar.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "UI_Manager.h"
#include "GameInstance.h"
#include "Collider.h"
#include "BrickCat.h"

CBrickBar::CBrickBar(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrickBar::CBrickBar(const CBrickBar& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBrickBar::Init_Prototype()
{
	return S_OK;
}

HRESULT CBrickBar::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_fSpeed = 5.f;
	m_pTransformCom->Set_Scale(_vec3(1.f, 0.5f, 0.5f));
	m_pTransformCom->Set_Speed(m_fSpeed);

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	vPlayerPos.y += 1.f; 
	_vec4 vCenterPos = _vec4(-2000.70496f, 1.4677677f, -1987.06152f, 1.f);
	_vec4 vPlayerLook = pPlayerTransform->Get_State(State::Look);
	vPlayerLook.y = 0.f;

	m_pTransformCom->Set_State(State::Pos, vCenterPos);

	m_pColliderCom->Set_Normal();
	m_vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	m_shouldRenderBlur = true;

	m_eCurBrickColor = BLUE;

	return S_OK;
}

void CBrickBar::Tick(_float fTimeDelta)
{
	m_eCurDir = BAR_STOP;

	m_pTransformCom->Set_Scale(_vec3(2.f, 0.5f, 0.3f));
	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_eCurDir = BAR_LEFT;
		_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
		vPos.x += fTimeDelta * 10.f; 
		m_pTransformCom->Set_State(State::Pos, vPos);
	}
	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_eCurDir = BAR_RIGHT;
		_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
		vPos.x -= fTimeDelta * 10.f; 
		m_pTransformCom->Set_State(State::Pos, vPos);
	}

	if (!m_isChanged && m_pGameInstance->Key_Down(DIK_UP, InputChannel::GamePlay))
	{
		m_iBallColor++;

		if (m_iBallColor > (_uint)COLOR_END)
		{
			m_iBallColor = 0;
		}
		m_isChanged = true;
	}
	if (m_isChanged)
	{
		if (m_fTime >= 90.f)
		{
			m_eCurBrickColor = (BrickColor)m_iBallColor;
			m_fTime = 0.f;
			m_isChanged = false;
		}
		m_fTime += fTimeDelta * 150.f;
		m_pTransformCom->Rotation(_vec4(1.f, 0.f, 0.f, 0.f), m_fTime);
	}

	Set_BarColor();


	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);

	Update_Collider();

	CUI_Manager::Get_Instance()->Set_BarDir(m_eCurDir);
	return;


}

void CBrickBar::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Blend, this);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBrickBar::Render()
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

		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof _vec4)))
		{
			return E_FAIL;
		}

		_bool isBlur = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlur", &isBlur, sizeof _bool)))
		{
			return E_FAIL;
		}

	
		if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect)))
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

HRESULT CBrickBar::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(1.f, 0.5f, 1.f);
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Bar"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CBrickBar::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}



void CBrickBar::Set_BarColor()
{
	switch (m_eCurBrickColor)
	{
	case PINK:
		m_vColor = _vec4(1.f, 0.56f, 0.93f, 1.f);
		break;
	case YELLOW:
		m_vColor = _vec4(0.94f, 0.77f, 0.2f, 1.f);
		break;
	case PURPLE:
		m_vColor = _vec4(0.63f, 0.4f, 0.9f, 1.f);
		break;
	case BLUE:
		m_vColor = _vec4(0.f, 0.6f, 1.f, 1.f);
		break;
	case COLOR_END:
		break;
	default:
		break;
	}
}

HRESULT CBrickBar::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Effect_CommonCube"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_T_EFF_Noise_04_BC"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBrickBar::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CBrickBar* CBrickBar::Create(_dev pDevice, _context pContext)
{
	CBrickBar* pInstance = new CBrickBar(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBrickBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrickBar::Clone(void* pArg)
{
	CBrickBar* pInstance = new CBrickBar(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBrickBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrickBar::Free()
{
	__super::Free();

	Safe_Release(m_pEffect_Ball);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
