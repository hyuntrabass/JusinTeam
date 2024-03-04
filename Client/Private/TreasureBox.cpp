#pragma once
#include "TreasureBox.h"
#include "Camera_Manager.h"
#include "NameTag.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "3DUITEX.h"
#include "Item.h"
#include "TextButtonColor.h"
#include "TextButton.h"
#include "Pop_Reward.h"
#include "Effect_Manager.h"

CTreasureBox::CTreasureBox(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CTreasureBox::CTreasureBox(const CTreasureBox& rhs)
	: CGameObject(rhs)
{

}


HRESULT CTreasureBox::Init_Prototype()
{
	return S_OK;
}

HRESULT CTreasureBox::Init(void* pArg)
{
	_vec4 vPos = ((TREASURE_DESC*)pArg)->vPos;
	m_vecItem = ((TREASURE_DESC*)pArg)->vecItem;
	m_eDir = ((TREASURE_DESC*)pArg)->eDir;

	if (m_vecItem.empty())
	{
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_Scale(_vec3(0.7f, 0.7f, 0.7f));


	m_Animation.iAnimIndex = 0;
	m_Animation.fAnimSpeedRatio = 1.8f;
	m_Animation.isLoop = false;

	CNameTag::NAMETAG_DESC NameTagDesc = {};
	NameTagDesc.eLevelID = LEVEL_STATIC;
	NameTagDesc.fFontSize = 0.36f;
	NameTagDesc.pParentTransform = m_pTransformCom;
	NameTagDesc.vColor = _vec4(1.f, 0.2f, 0.2f, 1.f);
	NameTagDesc.vTextPosition = _vec2(0.f, 1.3f);
	NameTagDesc.strNameTag = TEXT("???");
	m_pNameTag = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NameTag"), &NameTagDesc);
	if (not m_pNameTag)
	{
		return E_FAIL;
	}

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.2f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.2f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
	m_pTransformCom->Set_Position(_vec3(vPos));
	m_pTransformCom->Set_Speed(1.f);

	m_iHP = 1;

	return S_OK;
}

void CTreasureBox::Tick(_float fTimeDelta)
{
	m_fJumpTime += fTimeDelta;
	if (!m_isJump)
	{
		m_pTransformCom->Jump(5.f);
		m_isJump = true;
	}
	if (m_isCollect)
	{
		m_pBar->Set_Factor(m_fCollectTime / 4.f);
		m_pBar->Set_UV(m_fCollectTime, 0.f);

		m_fCollectTime += fTimeDelta;

		if (m_fTime >= 2.f)
		{
			m_fDir = -1.f;
		}
		if (m_fTime <= -2.f)
		{
			m_fDir = 1.f;
		}
		m_fTime += fTimeDelta * 5.f * m_fDir;


		if (m_fCollectTime >= 4.f)
		{
			m_isCollect = false;
			m_isAnimStart = true;
		}

		m_pBar->Tick(fTimeDelta);

	}
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	m_pWideColliderCom->Update(m_pTransformCom->Get_World_Matrix());

	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	_bool isColl = m_pColliderCom->Intersect(pCollider);
	m_isWideCollision = m_pWideColliderCom->Intersect(pCollider);
	if (isColl)
	{
		if (!m_isCollect && m_pGameInstance->Key_Down(DIK_E))
		{
			CUI_Manager::Get_Instance()->Set_Collect();
			m_isCollect = true;
		}
		m_pShaderCom->Set_PassIndex(VTF_InstPass_OutLine);
	}
	else
	{
		m_pShaderCom->Set_PassIndex(VTF_InstPass_Default);
	}

	if (m_isWideCollision)
	{
		dynamic_cast<CNameTag*>(m_pNameTag)->Tick(fTimeDelta);
	}	
	if (m_fJumpTime <= 0.9f)
	{
		switch (m_eDir)
		{
		case CTreasureBox::LEFT:
			m_pTransformCom->Go_Left(fTimeDelta * 5.f);
			break;
		case CTreasureBox::RIGHT:
			m_pTransformCom->Go_Right(fTimeDelta * 5.f);
			break;
		case CTreasureBox::FRONT:
			m_pTransformCom->Go_Straight(fTimeDelta * 5.f);
			break;
		case CTreasureBox::BACK:
			m_pTransformCom->Go_Backward(fTimeDelta * 5.f);
			break;
		case CTreasureBox::DIR_END:
			break;
		default:
			break;
		}

	}

	m_pTransformCom->Gravity(fTimeDelta);

	if (m_pModelCom->Get_CurrentAnimPos() > 10.f and m_iHP > 0)
	{
		_mat EffectMat = m_pTransformCom->Get_World_Matrix();
		EffectMat.Position_vec3(EffectMat.Position_vec3() + _vec3(0.f, 0.5f, 0.f));
		EffectInfo EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Chest_Parti");
		EffectDesc.pMatrix = &EffectMat;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);

		m_iHP = 0;
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pWideColliderCom);
#endif
}

void CTreasureBox::Late_Tick(_float fTimeDelta)
{
	CAMERA_STATE CamState = CCamera_Manager::Get_Instance()->Get_CameraState();
	if (CamState == CS_SKILLBOOK or CamState == CS_INVEN)// or CamState == CS_WORLDMAP)
	{
		return;
	}
	if (m_isCollect)
	{
		if (m_pItem != nullptr)
		{
			m_pItem->Late_Tick(fTimeDelta);
		}

		m_pBG->Late_Tick(fTimeDelta);
		m_pBar->Late_Tick(fTimeDelta);
	}

	if (m_isAnimStart)
	{
		if (m_pModelCom->IsAnimationFinished(0))
		{
			CPop_Reward::REWARD_DESC Desc{};
			//ecReward.push_back(make_pair(TEXT("엘드룬의 수호 갑옷"), 1));
			//vecReward.push_back(make_pair(TEXT("엘드룬의 수호 투구"), 1));
			Desc.vecRewards = m_vecItem;
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Pop_Reward"), &Desc)))
			{
				return;
			}

			m_isDead = true;
		}
		m_pModelCom->Play_Animation(fTimeDelta);
	}

	if (m_isWideCollision)
	{
		m_pNameTag->Late_Tick(fTimeDelta);
	}
	m_pRendererCom->Add_RenderGroup(RG_AnimNonBlend_Instance, this);

}

HRESULT CTreasureBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Bind_Animation(m_pShaderCom)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Bind_PlayAnimation(m_pShaderCom)))
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

		if (FAILED(m_pShaderCom->Begin(0)))
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


HRESULT CTreasureBox::Render_Instance()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTreasureBox::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VTF_Instance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	m_pShaderCom->Set_PassIndex(0);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_TreasureBox"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	/* For.Com_Collider_SPHERE */
// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.fRadius = 5.f;
	CollDesc.vCenter = _vec3(0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Interaction_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;
	
	Collider_Desc ColDesc2{};
	ColDesc2.eType = ColliderType::Sphere;
	ColDesc2.fRadius = 27.f;
	ColDesc2.vCenter = _vec3(0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Interaction_Sphere1"), (CComponent**)&m_pWideColliderCom, &ColDesc2)))
		return E_FAIL;


	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = (_float)D_SCREEN / (_float)D_END;
	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.fFontSize = 0.36f;
	ColButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ColButtonDesc.vTextPosition = _vec2(0.f, 20.f);
	ColButtonDesc.strText = TEXT("여는중...");

	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_CollectBar");
	ColButtonDesc.strTexture2 = TEXT("Prototype_Component_Texture_UI_Gameplay_Mask_FlagMove");
	ColButtonDesc.vSize = _vec2(180.f, 12.f);
	ColButtonDesc.vPosition = _vec2((_float)g_ptCenter.x, 200.f);
	ColButtonDesc.vColor = _vec4(0.31f, 0.96f, 1.f, 1.f);

	m_pBar = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pBar)
	{
		return E_FAIL;
	}
	m_pBar->Set_Pass(VTPass_HPBoss);
	m_pBar->Set_Factor(0.f);

	CTextButton::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = (_float)D_SCREEN / (_float)D_END;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_FadeBox");
	Button.vPosition = _vec2((_float)g_ptCenter.x, 200.f);
	Button.vSize = _vec2(183.f, 15.f);
	Button.fFontSize = 0.5f;
	Button.vTextColor = _vec4(0.5f, 0.07f, 0.04f, 1.f);
	m_pBG = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pBG)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTreasureBox::Bind_ShaderResources()
{
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

	if (true == m_pGameInstance->Get_TurnOnShadow()) {

		CASCADE_DESC Desc = m_pGameInstance->Get_CascadeDesc();

		if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeView", Desc.LightView, 3)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProj", Desc.LightProj, 3)))
			return E_FAIL;

	}

	return S_OK;
}



CTreasureBox* CTreasureBox::Create(_dev pDevice, _context pContext)
{
	CTreasureBox* pInstance = new CTreasureBox(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTreasureBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTreasureBox::Clone(void* pArg)
{
	CTreasureBox* pInstance = new CTreasureBox(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTreasureBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTreasureBox::Free()
{
	__super::Free();
	Safe_Release(m_pNameTag);

	Safe_Release(m_pBar);
	Safe_Release(m_pBG);
	Safe_Release(m_pItem);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pWideColliderCom);
}
