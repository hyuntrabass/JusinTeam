#include "BrickItem.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "Effect_Manager.h"
#include "Camera_Manager.h"
#include "Effect_Dummy.h"

CBrickItem::CBrickItem(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrickItem::CBrickItem(const CBrickItem& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBrickItem::Init_Prototype()
{
	return S_OK;
}

HRESULT CBrickItem::Init(void* pArg)
{
	m_eType = ((BRICKITEM_DESC*)pArg)->eType;
	fStartPos = ((BRICKITEM_DESC*)pArg)->vPos;
	m_pTransformCom->Set_State(State::Pos, fStartPos);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}
	m_pTransformCom->Set_Scale(_vec3(0.035f, 0.035f, 0.035f));
	m_pTransformCom->Rotation(_vec3(1.f, 0.f, 0.f), 180.f);
	m_pBodyColliderCom->Set_Normal();

	m_EffectMat = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"BrickItemEffect");
	Info.pMatrix = &m_EffectMat;
	Info.isFollow = true;
	m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	ItemCheck();
	return S_OK;
}

void CBrickItem::Tick(_float fTimeDelta)
{
	if (m_isExist)
	{
		Kill();
		return;
	}

	CCollider* pBalloonCollider{ nullptr };
	_bool isBalloonColl{};
	_uint iNum = m_pGameInstance->Get_LayerSize(LEVEL_TOWER, TEXT("Layer_Balloons"));
	for (_uint i = 0; i < iNum; i++)
	{
		pBalloonCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_Balloons"), TEXT("Com_Collider_Sphere"), i);
		if (pBalloonCollider == nullptr)
		{
			break;
		}
		if (m_pBodyColliderCom->Intersect(pBalloonCollider))
		{
			Kill();
			return;
		}
	}


	if (CCamera_Manager::Get_Instance()->Get_CameraState() != CS_BRICKGAME)
	{
		m_isDead = true;
		return;
	}

	if (m_pTransformCom->Get_State(State::Pos).y > fStartPos.y + 0.1f)
	{
		m_fDir = -1.f;
	}
	if (m_pTransformCom->Get_State(State::Pos).y < fStartPos.y - 0.1f)
	{
		m_fDir = 1.f;
	}
	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	vPos.y += fTimeDelta * 0.3f * m_fDir;
	m_pTransformCom->Set_State(State::Pos, vPos);

	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_BrickGame"), TEXT("BrickBall"));
	if (pCollider != nullptr && m_pBodyColliderCom->Intersect(pCollider))
	{
		m_isDead = true;
	}

	Update_BodyCollider();

	m_EffectMat = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));
	m_pEffect->Tick(fTimeDelta);
}

void CBrickItem::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Blend, this);
	m_pEffect->Late_Tick(fTimeDelta);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif
}

HRESULT CBrickItem::Render()
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
		_bool isBlur = false;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlur", &isBlur, sizeof _bool)))
		{
			return E_FAIL;
		}
		_vec2 uv = _vec2(0.f, 0.f);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &uv, sizeof _vec2)))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Begin(24)))
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

void CBrickItem::ItemCheck()
{
	wstring strLayer{};
	switch (m_eType)
	{
	case CBrickItem::POWER:
		strLayer = TEXT("Layer_BrickPower");
		break;
	case CBrickItem::DOUBLE:
		strLayer = TEXT("Layer_BrickDouble");
		break;
	case CBrickItem::STOP:
		strLayer = TEXT("Layer_BrickStop");
		break;
	default:
		break;
	}

	CCollider* pItemCol{ nullptr };
	_bool isColl{};
	_uint iNum = m_pGameInstance->Get_LayerSize(LEVEL_TOWER, strLayer);
	for (_uint i = 0; i < iNum; i++)
	{
		pItemCol = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, strLayer, TEXT("Com_Collider"), i);
		if (pItemCol == nullptr)
		{
			break;
		}
		if (m_pBodyColliderCom->Intersect(pItemCol))
		{
			m_isExist = true;
			break;
		}
	}
}

HRESULT CBrickItem::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(5.f, 5.f, 1.f);
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), (CComponent**)&m_pBodyColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}


void CBrickItem::Update_BodyCollider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

HRESULT CBrickItem::Add_Components()
{
	wstring strModelCom{ };
	switch (m_eType)
	{
	case Client::CBrickItem::POWER:
		strModelCom = TEXT("Prototype_Model_PowerCube");
		break;
	case Client::CBrickItem::DOUBLE:
		strModelCom = TEXT("Prototype_Model_DoubleCube");
		break;
	case Client::CBrickItem::STOP:
		strModelCom = TEXT("Prototype_Model_StopCube");
		break;
	case Client::CBrickItem::TYPE_END:
		break;
	default:
		break;
	}
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Effect"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelCom, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBrickItem::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBrickItem* CBrickItem::Create(_dev pDevice, _context pContext)
{
	CBrickItem* pInstance = new CBrickItem(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBrickItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrickItem::Clone(void* pArg)
{
	CBrickItem* pInstance = new CBrickItem(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBrickItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrickItem::Free()
{
	__super::Free();


	Safe_Release(m_pEffect);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBodyColliderCom);

}
