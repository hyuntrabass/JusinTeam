#include "Balloon.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "Effect_Manager.h"
#include "Camera_Manager.h"
#include "Trigger_Manager.h"
#include "BrickBall.h"
CBalloon::CBalloon(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBalloon::CBalloon(const CBalloon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBalloon::Init_Prototype()
{
	return S_OK;
}

HRESULT CBalloon::Init(void* pArg)
{
	m_isEmpty = ((BALLOON_DESC*)pArg)->isEmpty;
	m_vColor = ((BALLOON_DESC*)pArg)->vColor;
	_vec3 vPos = ((BALLOON_DESC*)pArg)->vPosition;
	m_pTransformCom->Set_Position(vPos);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}
	Set_RandomColor();

	m_eCurState = STATE_IDLE;

	m_pTransformCom->Set_Scale(_vec3(0.015f, 0.015f, 0.015f));

	m_pBodyColliderCom->Set_Normal();
	return S_OK;
}

void CBalloon::Tick(_float fTimeDelta)
{

	if (CTrigger_Manager::Get_Instance()->Get_CurrentSpot() != TS_BrickMap)
	{
		Kill();
		return;
	}

	if (m_eCurColor == COLOR_END)
	{
		Kill();
		return;
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);
	
	Update_BodyCollider();

//	m_pTransformCom->Gravity(fTimeDelta);


}

void CBalloon::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Blend, this);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif
}

HRESULT CBalloon::Render()
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof _vec4)))
		{
			return E_FAIL;
		}
		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		{
			return E_FAIL;
		}
		_bool isBlur = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlur", &isBlur, sizeof _bool)))
		{
			return E_FAIL;
		}

		_bool isFalse = { false };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &isFalse, sizeof _bool)))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &isFalse, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bSelected", &isFalse, sizeof _bool)))
		{
			return E_FAIL;
		}

		_float fAlpha = 0.1f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof _float)))
		{
			return E_FAIL;
		}

		_vec2 vUV = _vec2(m_fX, 0.f);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &vUV, sizeof _vec2)))
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

void CBalloon::Init_State(_float fTimeDelta)
{

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CBalloon::STATE_IDLE:
			break;

		case Client::CBalloon::STATE_HIT:
			break;

		case Client::CBalloon::STATE_DIE:
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CBalloon::Tick_State(_float fTimeDelta)
{
	m_isColl = false;
	switch (m_eCurState)
	{
	case Client::CBalloon::STATE_IDLE:
	{
		if (!m_bStart)
		{
			if (m_fDelayTime >= 0.3f)
			{
				m_bStart = true;
			}
			m_fDelayTime += fTimeDelta;
		}

		CCollider* pBalloonCol{ nullptr };
		_bool isBrickColl{};
		_uint iNum = m_pGameInstance->Get_LayerSize(LEVEL_TOWER, TEXT("Layer_Balloons"));
		for (_uint i = 0; i < iNum; i++)
		{
			pBalloonCol = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_Balloons"), TEXT("Com_Collider_Sphere"), i);
			if (pBalloonCol == nullptr)
			{
				break;
			}
			if (m_pBodyColliderCom->Intersect(pBalloonCol))
			{
				if (pBalloonCol->Get_Extents().y == POWEREXT)
				{
					if (pBalloonCol == m_pBodyColliderCom)
					{
						m_isPowerBrick = true;
					}
					m_eCurState = STATE_DIE;
					return;
				}
			}
		}

		if (m_isFall && !m_isReadyToFall )
		{
			_vec4 Pos = m_pTransformCom->Get_State(State::Pos);
			Pos.y -= fTimeDelta * 6.f;
			
			if (m_pTransformCom->Get_State(State::Pos).y <= 1.5f)
			{
				m_isFall = false;
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
				if (pBalloonCollider == m_pBodyColliderCom)
				{
					break;
				}
				if (m_pBodyColliderCom->Intersect(pBalloonCollider))
				{
					isBalloonColl = true;
					break;
				}
			}
			if (isBalloonColl)
			{
				if (m_pTransformCom->Get_State(State::Pos).y <= 3.f)
				{
					Pos.y = 3.f;
				}
				else if (m_pTransformCom->Get_State(State::Pos).y <= 4.5f)
				{
					Pos.y = 4.5f;
				}
				m_isFall = false;
				m_isReadyToFall = true;
			}
			m_pTransformCom->Set_State(State::Pos, Pos);
		}

		if (m_isReadyToFall)
		{
			if (m_isFall)
			{
				_vec4 Pos = m_pTransformCom->Get_State(State::Pos);
				Pos.y -= fTimeDelta * 6.f;
				if (Pos.y <= 1.5f)
				{
					Pos.y = 1.5f;
					m_isFall = false;
					m_isReadyToFall = false;
				}
				else if (Pos.y <= 3.f)
				{
					Pos.y = 3.f;
					m_isFall = false;
					m_isReadyToFall = false;

				}
				m_pTransformCom->Set_State(State::Pos, Pos);

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
				if (pBalloonCollider == m_pBodyColliderCom)
				{
					break;
				}
				if (m_pBodyColliderCom->Intersect(pBalloonCollider))
				{
					isBalloonColl = true;
					break;
				}
			}
			if (!isBalloonColl)
			{
				m_isFall = true;
			}
		}

		CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_BrickGame"), TEXT("BrickBall"));
		if (pCollider == nullptr)
		{
			return;
		}

		m_isColl = m_pBodyColliderCom->Intersect(pCollider);
		if (m_isColl)
		{
			_float fR = pCollider->Get_Radius();
			if (pCollider->Get_Radius() == POWERCOL)
			{
				Set_ItemExtents(CBrickItem::POWER);
			}
			else if (m_bStart)
			{
				if (CUI_Manager::Get_Instance()->Get_BrickBallColor() != m_eCurColor)
				{
					return;
				}
				_mat Mat = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

				CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 2.0f);

				m_eCurState = STATE_DIE;
			}
	
		}
		
	}
	break;

	case Client::CBalloon::STATE_HIT:
		m_eCurState = STATE_IDLE;
		m_iCount--;
		break;

	case Client::CBalloon::STATE_DIE:
	{
		_mat Mat = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

		if (m_eCurColor == BLUE)
		{
			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_IceExplosion");
			Info.pMatrix = &Mat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_IceCubeExplosion");
			Info.pMatrix = &Mat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
		}
		else
		{
			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_FireballExplosion");
			Info.pMatrix = &Mat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_FireExplosion");
			Info.pMatrix = &Mat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

		}
		if (m_isPowerBrick)
		{
			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"BrickPowerBall");
			Info.pMatrix = &Mat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
		}
		if (m_pTransformCom->Get_State(State::Pos).y < 2.f)
		{
			Create_Item();
		}

		m_isDead = true;
	}

		break;
	}

	Set_Color();
}

void CBalloon::Create_Item()
{
	_uint iRandom = rand() % 100;
	if (iRandom > 40)
	{
		return;
	}

	CBrickItem::TYPE eItemType[CBrickItem::TYPE_END] = { CBrickItem::POWER,CBrickItem::DOUBLE, CBrickItem::STOP };
	CBrickItem::TYPE eType{};
	_uint iRandomType = rand() % 100;

	if (iRandomType < 40)
	{
		eType = CBrickItem::DOUBLE;
	}
	else if (iRandomType < 80)
	{
		eType = CBrickItem::POWER;
	}
	else
	{
		eType = CBrickItem::STOP;
	}

	CBrickItem::BRICKITEM_DESC Desc{};
	Desc.eType = eItemType[eType];
	Desc.vPos = m_pTransformCom->Get_State(State::Pos);

	wstring strLayer{};
	switch (eType)
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
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, strLayer, TEXT("Prototype_GameObject_BrickItem"), &Desc)))
	{
		return;
	}
}

void CBalloon::Set_Color()
{
	switch (m_eCurColor)
	{
	case RED:
		m_vColor = _vec4(1.f, 0.32f, 0.23f, 1.f);
		break;
	case BLUE:
		m_vColor = _vec4(0.143f, 0.267f, 0.321f, 1.f);
		break;
	case COLOR_END:
		m_isDead = true;
		break;
	default:
		break;
	}

}

void CBalloon::Set_RandomColor()
{
	if (m_isEmpty)
	{
		_uint iRandom = rand() % 100;

		if (iRandom < 30)
		{
			m_eCurColor = RED;
		}
		else if (iRandom < 60)
		{
			m_eCurColor = BLUE;
		}
		else
		{
			m_eCurColor = COLOR_END;
		}
	}
	else
	{
		_uint iRandom = rand() % 100;

		if (iRandom < 50)
		{
			m_eCurColor = BLUE;
		}
		else
		{
			m_eCurColor = RED;
		}
	}

	
	Set_Color();
}

void CBalloon::Set_ItemExtents(CBrickItem::TYPE eType)
{
	switch (eType)
	{
	case CBrickItem::POWER:
		m_pBodyColliderCom->Change_Extents(_vec3(POWEREXT, POWEREXT, POWEREXT));
		break;
	default:
		m_pBodyColliderCom->Change_Extents(_vec3(74.f, DEFEXT, 74.f));
		break;
	}

}

HRESULT CBalloon::Add_Collider()
{

	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(74.f, 50.1f, 74.f);
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pBodyColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}


void CBalloon::Update_BodyCollider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

HRESULT CBalloon::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Effect"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_BrickCube"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_cubeone"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CBalloon::Bind_ShaderResources()
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

CBalloon* CBalloon::Create(_dev pDevice, _context pContext)
{
	CBalloon* pInstance = new CBalloon(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBalloon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBalloon::Clone(void* pArg)
{
	CBalloon* pInstance = new CBalloon(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBalloon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBalloon::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pBodyColliderCom);

	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pDissolveTextureCom);
}
