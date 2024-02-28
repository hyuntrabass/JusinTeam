#pragma once
#include "Interaction_NonAnim_Object.h"
#include "Camera_Manager.h"
#include "NameTag.h"
#include "UI_Manager.h"
#include "TextButtonColor.h"
#include "TextButton.h"
#include "3DUITEX.h"
#include "Item.h"
#include "Effect_Manager.h"

// Prototype_Model_Hurbs
// Prototype_Model_OakTree

CInteraction_NonAnim::CInteraction_NonAnim(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CInteraction_NonAnim::CInteraction_NonAnim(const CInteraction_NonAnim& rhs)
	: CGameObject(rhs)
	, m_pTrigger_Manager(CTrigger_Manager::Get_Instance())

{
	Safe_AddRef(m_pTrigger_Manager);
	//m_isInstancing = true;
}


HRESULT CInteraction_NonAnim::Init_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_NonAnim::Init(void* pArg)
{
	m_Info = *(ObjectInfo*)pArg;
	m_ePlaceType = (PlaceType)m_Info.m_iIndex;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	_vec4 vRight = _vec4(m_Info.m_WorldMatrix._11, m_Info.m_WorldMatrix._12, m_Info.m_WorldMatrix._13, m_Info.m_WorldMatrix._14);
	_vec4 vUp = _vec4(m_Info.m_WorldMatrix._21, m_Info.m_WorldMatrix._22, m_Info.m_WorldMatrix._23, m_Info.m_WorldMatrix._24);
	_vec4 vLook = _vec4(m_Info.m_WorldMatrix._31, m_Info.m_WorldMatrix._32, m_Info.m_WorldMatrix._33, m_Info.m_WorldMatrix._34);
	_vec4 vPos = _vec4(m_Info.m_WorldMatrix._41, m_Info.m_WorldMatrix._42, m_Info.m_WorldMatrix._43, m_Info.m_WorldMatrix._44);

	m_pTransformCom->Set_State(State::Right, vRight);
	m_pTransformCom->Set_State(State::Up, vUp);
	m_pTransformCom->Set_State(State::Look, vLook);
	m_pTransformCom->Set_State(State::Pos, vPos);

	
	//m_pModelCom->Apply_TransformToActor(m_Info.m_WorldMatrix);
	ITEM eItem{};

	CNameTag::NAMETAG_DESC NameTagDesc = {};
	NameTagDesc.eLevelID = LEVEL_STATIC;
	NameTagDesc.fFontSize = 0.36f;
	NameTagDesc.pParentTransform = m_pTransformCom;
	if (m_Info.strPrototypeTag == TEXT("Prototype_Model_OakTree"))
	{
		NameTagDesc.strNameTag = TEXT("오크나무");
	}
	else
	{
		NameTagDesc.strNameTag = TEXT("아마풀");
	}

	NameTagDesc.vColor = _vec4(0.31f, 0.96f, 1.f, 1.f);
	NameTagDesc.vTextPosition = _vec2(0.f, 3.2f);

	m_pNameTag = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NameTag"), &NameTagDesc);
	if (not m_pNameTag)
	{
		return E_FAIL;
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
	return S_OK;
}

void CInteraction_NonAnim::Tick(_float fTimeDelta)
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
		m_pItem->Set_Position(_vec2((_float)g_ptCenter.x, 160.f + m_fTime));

		if (m_fCollectTime >= 4.f)
		{
			wstring strItem = m_pItem->Get_ItemDesc().strName;
			CUI_Manager::Get_Instance()->Set_Item(strItem);
			m_isDead = true;
			m_isCollect = false;
		}

		m_pBar->Tick(fTimeDelta);
		m_pItem->Tick(fTimeDelta);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	m_pWideColliderCom->Update(m_pTransformCom->Get_World_Matrix());

	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	m_isCollision = m_pColliderCom->Intersect(pCollider);
	m_isWideCollision = m_pWideColliderCom->Intersect(pCollider);
	if (m_isCollision)
	{
		if (!m_isCollect && m_pGameInstance->Key_Down(DIK_E))
		{
			CUI_Manager::Get_Instance()->Set_Collect();
			m_isCollect = true;
		}
	}

	if (m_isWideCollision)
	{
		dynamic_cast<CNameTag*>(m_pNameTag)->Tick(fTimeDelta);
		m_pSpeechBubble->Tick(fTimeDelta);
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

void CInteraction_NonAnim::Late_Tick(_float fTimeDelta)
{
	CAMERA_STATE CamState = CCamera_Manager::Get_Instance()->Get_CameraState();
	if (CamState == CS_SKILLBOOK or CamState == CS_INVEN) //or CamState == CS_WORLDMAP)
	{
		return;
	}
	if (m_isCollect)
	{
		m_pItem->Late_Tick(fTimeDelta);
		m_pBG->Late_Tick(fTimeDelta);
		m_pBar->Late_Tick(fTimeDelta);
	}

	if (m_isWideCollision)
	{
		m_pNameTag->Late_Tick(fTimeDelta);
		m_pSpeechBubble->Late_Tick(fTimeDelta);
	}
	m_pRendererCom->Add_RenderGroup(RG_NonBlend_Instance, this);
}


HRESULT CInteraction_NonAnim::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStaticMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh_Instance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}
	m_pShaderCom->Set_PassIndex(3);
	m_iShaderPass = StaticPass_Default;
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_Info.strPrototypeTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}


		/* For.Com_Collider_SPHERE */
	// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.fRadius = 2.f;
	CollDesc.vCenter = _vec3(0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Interaction_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	Collider_Desc ColDesc2{};
	ColDesc2.eType = ColliderType::Sphere;
	ColDesc2.fRadius = 20.f;
	ColDesc2.vCenter = _vec3(0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Interaction_Sphere1"), (CComponent**)&m_pWideColliderCom, &ColDesc2)))
		return E_FAIL;

	C3DUITex::UITEX_DESC TexDesc = {};
	TexDesc.eLevelID = LEVEL_STATIC;
	TexDesc.pParentTransform = m_pTransformCom;
	TexDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SpeechBubble");
	TexDesc.vPosition = _vec3(0.f,3.8f, 0.f);
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
	ColButtonDesc.strText = TEXT("채집중...");
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

HRESULT CInteraction_NonAnim::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			return E_FAIL;
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

		_bool HasMaskTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
		{
			HasMaskTex = false;
		}
		else
		{
			HasMaskTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
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


HRESULT CInteraction_NonAnim::Render_Shadow()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	CASCADE_DESC Desc = m_pGameInstance->Get_CascadeDesc();

	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeView", Desc.LightView, 3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProj", Desc.LightProj, 3)))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			continue;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_Shadow)))
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

HRESULT CInteraction_NonAnim::Render_Instance()
{
	if (FAILED(Bind_ShaderResources()))
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


HRESULT CInteraction_NonAnim::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
		return E_FAIL;

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



CInteraction_NonAnim* CInteraction_NonAnim::Create(_dev pDevice, _context pContext)
{
	CInteraction_NonAnim* pInstance = new CInteraction_NonAnim(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CInteraction_NonAnim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInteraction_NonAnim::Clone(void* pArg)
{
	CInteraction_NonAnim* pInstance = new CInteraction_NonAnim(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CInteraction_NonAnim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction_NonAnim::Free()
{
	__super::Free();
	Safe_Release(m_pNameTag);
	Safe_Release(m_pBar);
	Safe_Release(m_pBG);
	Safe_Release(m_pItem);
	Safe_Release(m_pSpeechBubble);
	Safe_Release(m_pTrigger_Manager);
	Safe_Release(m_pSpeechBubble);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pWideColliderCom);
	Safe_Release(m_pColliderCom);


}
