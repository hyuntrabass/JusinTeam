#pragma once
#include "Interaction_Anim_Object.h"
#include "Camera_Manager.h"
#include "NameTag.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "3DUITEX.h"
#include "Item.h"
#include "TextButtonColor.h"
#include "TextButton.h"
#include "Pop_Reward.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"

// 부셔지는 애니메이션 하나 밖에 없음
// Prototype_Model_GoldStone
// Prototype_Model_SaltStone
// Prototype_Model_TreasureBox

CInteraction_Anim::CInteraction_Anim(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CInteraction_Anim::CInteraction_Anim(const CInteraction_Anim& rhs)
	: CGameObject(rhs)
	, m_pTrigger_Manager(CTrigger_Manager::Get_Instance())

{
	Safe_AddRef(m_pTrigger_Manager);
}


HRESULT CInteraction_Anim::Init_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_Anim::Init(void* pArg)
{
	m_Info = *(ObjectInfo*)pArg;
	m_ePlaceType = (PlaceType)m_Info.m_iIndex;
	m_iObjectIndex = m_Info.m_iIndex;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	_vec4 vRight = _vec4(m_Info.m_WorldMatrix._11, m_Info.m_WorldMatrix._12, m_Info.m_WorldMatrix._13, m_Info.m_WorldMatrix._14);
	_vec4 vUp = _vec4(m_Info.m_WorldMatrix._21, m_Info.m_WorldMatrix._22, m_Info.m_WorldMatrix._23, m_Info.m_WorldMatrix._24);
	_vec4 vLook = _vec4(m_Info.m_WorldMatrix._31, m_Info.m_WorldMatrix._32, m_Info.m_WorldMatrix._33, m_Info.m_WorldMatrix._34);
	_vec4 vPos = _vec4(m_Info.m_WorldMatrix._41, m_Info.m_WorldMatrix._42, m_Info.m_WorldMatrix._43, 1.f);

	m_pTransformCom->Set_State(State::Right, vRight);
	m_pTransformCom->Set_State(State::Up, vUp);
	m_pTransformCom->Set_State(State::Look, vLook);
	m_pTransformCom->Set_State(State::Pos, vPos);

	m_Animation.iAnimIndex = 0;
	m_Animation.fAnimSpeedRatio = 1.8f;
	m_Animation.isLoop = false;

	//m_pModelCom->Apply_TransformToActor(m_Info.m_WorldMatrix);


	CNameTag::NAMETAG_DESC NameTagDesc = {};
	NameTagDesc.eLevelID = LEVEL_STATIC;
	NameTagDesc.fFontSize = 0.36f;
	NameTagDesc.pParentTransform = m_pTransformCom;

	NameTagDesc.vColor = _vec4(0.31f, 0.96f, 1.f, 1.f);
	NameTagDesc.vTextPosition = _vec2(0.f, 3.2f);


	if (m_Info.strPrototypeTag == TEXT("Prototype_Model_GoldStone"))
	{
		NameTagDesc.strNameTag = TEXT("금광석");

		_mat EffectMat = _mat::CreateTranslation(_vec3(vPos));
		EffectInfo EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Mineral_Parti");
		EffectDesc.pMatrix = &EffectMat;
		m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(EffectDesc);
	}
	else if (m_Info.strPrototypeTag == TEXT("Prototype_Model_SaltStone"))
	{
		NameTagDesc.strNameTag = TEXT("소금광석");

		_mat EffectMat = _mat::CreateTranslation(_vec3(vPos));
		EffectInfo EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Mineral_Parti_Blue");
		EffectDesc.pMatrix = &EffectMat;
		m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(EffectDesc);
	}
	else if (m_Info.strPrototypeTag == TEXT("Prototype_Model_TreasureBox"))
	{
		NameTagDesc.vTextPosition = _vec2(0.f, 2.f);
		m_strName = TEXT("보물상자");
		NameTagDesc.strNameTag = TEXT("보물상자");
	}

	NameTagDesc.vColor = _vec4(0.31f, 0.96f, 1.f, 1.f);
	NameTagDesc.vTextPosition = _vec2(0.f, 3.2f);


	m_pNameTag = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NameTag"), &NameTagDesc);
	if (not m_pNameTag)
	{
		return E_FAIL;
	}
	if (NameTagDesc.strNameTag == TEXT("보물상자"))
	{
		return S_OK;
	}
	CItem::ITEM_DESC ItemDesc{};
	ItemDesc.bCanInteract = false;
	ItemDesc.eItemDesc = CUI_Manager::Get_Instance()->Find_Item(NameTagDesc.strNameTag);
	ItemDesc.fDepth = (_float)D_SCREEN / (_float)D_END;
	ItemDesc.haveBG = true;
	ItemDesc.isScreen = false;
	ItemDesc.vPosition = _vec2((_float)g_ptCenter.x, 160.f);
	ItemDesc.vSize = _vec2(50.f, 50.f);

	m_pItem = (CItem*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Item"), &ItemDesc);
	if (not m_pItem)
	{
		return E_FAIL;
	}

	m_iHP = 1;

	return S_OK;
}

void CInteraction_Anim::Tick(_float fTimeDelta)
{
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
		if (m_pItem != nullptr)
		{
			m_pItem->Set_Position(_vec2((_float)g_ptCenter.x, 160.f + m_fTime));
		}


		if (m_fCollectTime >= 4.f)
		{
			m_isCollect = false;
			m_isAnimStart = true;
		}

		m_pBar->Tick(fTimeDelta);
		if (m_pItem != nullptr)
		{
			m_pItem->Tick(fTimeDelta);
		}

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
			//CCamera_Manager::Get_Instance()->Set_CameraState(CS_COLLECT);
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
		m_pSpeechBubble->Tick(fTimeDelta);
	}

	if (m_pEffect)
	{
		m_pEffect->Tick(fTimeDelta);

		if (m_isAnimStart)
		{
			_vec3 vPos = m_pTransformCom->Get_State(State::Pos);
			_mat EffectMat = _mat::CreateTranslation(vPos);
			EffectInfo EffectDesc{};

			if (m_Info.strPrototypeTag == TEXT("Prototype_Model_GoldStone"))
			{
				EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Mineral_Parti_Diss");
			}
			else
			{
				EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Mineral_Parti_Blue_Diss");
			}

			EffectDesc.pMatrix = &EffectMat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);
			Safe_Release(m_pEffect);
		}
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pWideColliderCom);
#endif
}

void CInteraction_Anim::Late_Tick(_float fTimeDelta)
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
			//CCamera_Manager::Get_Instance()->Set_CameraState(CS_DEFAULT);
			if (m_pItem != nullptr)
			{
				wstring strItem = m_pItem->Get_ItemDesc().strName;
				CUI_Manager::Get_Instance()->Set_Item(strItem);
			}
			if (m_strName == TEXT("보물상자"))
			{
				CPop_Reward::REWARD_DESC Desc{};
				if (m_Info.m_iIndex == 0)
				{
					Desc.vecRewards.push_back(make_pair(TEXT("엘드룬의 수호 갑옷"), 1));
					Desc.vecRewards.push_back(make_pair(TEXT("엘드룬의 수호 투구"), 1));
				}
				else
				{
					Desc.vecRewards.push_back(make_pair(TEXT("헤임달의 단검"), 1));
					Desc.vecRewards.push_back(make_pair(TEXT("헤임달의 활"), 1));
				}
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Pop_Reward"), &Desc)))
				{
					return;
				}
			}
			m_isDead = true;
		}

		if (m_iHP > 0 and m_Info.strPrototypeTag == TEXT("Prototype_Model_SaltStone"))
		{
			CEvent_Manager::Get_Instance()->Update_Quest(TEXT("채집하기"));
			m_iHP = 0;
		}

		m_pModelCom->Play_Animation(fTimeDelta);
	}

	if (m_isWideCollision)
	{
		m_pNameTag->Late_Tick(fTimeDelta);
		m_pSpeechBubble->Late_Tick(fTimeDelta);
	}

	if (m_pEffect)
	{
		m_pEffect->Late_Tick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(RG_AnimNonBlend_Instance, this);
}

HRESULT CInteraction_Anim::Render()
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


HRESULT CInteraction_Anim::Render_Instance()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInteraction_Anim::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_Info.strPrototypeTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
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

	C3DUITex::UITEX_DESC TexDesc = {};
	TexDesc.eLevelID = LEVEL_STATIC;
	TexDesc.pParentTransform = m_pTransformCom;
	TexDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SpeechBubble");
	if (m_strName == TEXT("보물상자"))
	{
		TexDesc.vPosition = _vec3(0.f, 2.3f, 0.f);
	}
	else
	{
		TexDesc.vPosition = _vec3(0.f, 3.8f, 0.f);
	}

	TexDesc.vSize = _vec2(40.f, 40.f);

	m_pSpeechBubble = (C3DUITex*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &TexDesc);
	if (not m_pSpeechBubble)
	{
		return E_FAIL;
	}


	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = (_float)D_SCREEN / (_float)D_END;
	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.fFontSize = 0.36f;
	ColButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ColButtonDesc.vTextPosition = _vec2(0.f, 20.f);
	if (m_strName == TEXT("보물상자"))
	{
		ColButtonDesc.strText = TEXT("여는중...");
	}
	else
	{
		ColButtonDesc.strText = TEXT("채집중...");
	}
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

HRESULT CInteraction_Anim::Bind_ShaderResources()
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

	_uint iOutLineColorIndex = 2;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_OutlineColor", &iOutLineColorIndex, sizeof iOutLineColorIndex)))
	{
		return E_FAIL;
	}

	if (true == m_pGameInstance->Get_TurnOnShadow())
	{

		CASCADE_DESC Desc = m_pGameInstance->Get_CascadeDesc();

		if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeView", Desc.LightView, 3)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProj", Desc.LightProj, 3)))
			return E_FAIL;

	}

	return S_OK;
}



CInteraction_Anim* CInteraction_Anim::Create(_dev pDevice, _context pContext)
{
	CInteraction_Anim* pInstance = new CInteraction_Anim(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CInteraction_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInteraction_Anim::Clone(void* pArg)
{
	CInteraction_Anim* pInstance = new CInteraction_Anim(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CInteraction_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction_Anim::Free()
{
	__super::Free();
	Safe_Release(m_pNameTag);
	Safe_Release(m_pEffect);
	Safe_Release(m_pBar);
	Safe_Release(m_pBG);
	Safe_Release(m_pItem);
	Safe_Release(m_pSpeechBubble);
	Safe_Release(m_pTrigger_Manager);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pWideColliderCom);
}
