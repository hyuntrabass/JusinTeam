#include "Lever.h"
#include "Trigger_Manager.h"
#include "TextButtonColor.h"
#include "TextButton.h"
#include "3DUITex.h"
#include "NameTag.h"
#include "UI_Manager.h"

CLever::CLever(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CLever::CLever(const CLever& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLever::Init_Prototype()
{
	return S_OK;
}

HRESULT CLever::Init(void* pArg)
{
	m_Info = *(LeverInfo*)pArg;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Collider()))
		return E_FAIL;

	m_pTransformCom->Set_Matrix(m_Info.mMatrix);
	
	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.5f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.4f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
	m_pModelCom->Play_Animation(0);

	m_ShaderPassIndex = AnimPass_OutLine;
	return S_OK;
}

void CLever::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_OldMatrix();

	if (true == m_isAllDone)
		return;

	if (m_isInteracting)
	{
		m_pBar->Set_Factor(m_fCollectTime / 2.5f);
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

		if (m_fCollectTime >= 2.5f)
		{
			//여기가 바 로딩 끝나는 부분
			m_isInteracting = false;
			m_isOn = true;
			m_pGameInstance->Play_Sound(L"Siege_Obj_Ladder_Up_SFX_04");

			m_ShaderPassIndex = AnimPass_Default;
		}

		m_pBar->Tick(fTimeDelta);
	}

	if (m_pModelCom->IsAnimationFinished(0) and 0 == m_Info.iIndex) {
		CTrigger_Manager::Get_Instance()->Set_Lever1();
		m_isAllDone = true;
		return;
	}

	if (m_pModelCom->IsAnimationFinished(0) and 1 == m_Info.iIndex) {
		CTrigger_Manager::Get_Instance()->Set_Lever2();
		m_isAllDone = true;
		return;
	}

	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	_bool isColl = m_pBodyColliderCom->Intersect(pCollider);
	m_isWideCollision = m_pWideColliderCom->Intersect(pCollider);

	if (isColl) {
		if (m_pGameInstance->Key_Down(DIK_E))
		{
			m_isInteracting = true;
		}
	}

	Update_Collider();

	if (m_isWideCollision)
	{
		if (m_pNameTag)
		{
			m_pNameTag->Tick(fTimeDelta);
		}
		if (m_pSpeechBubble)
		{
			m_pSpeechBubble->Tick(fTimeDelta);
		}
	}

}

void CLever::Late_Tick(_float fTimeDelta)
{
	if (m_isOn)
	{
		m_pModelCom->Play_Animation(fTimeDelta);
	}

	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_CenterPos(), 20.f))
	{
		m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
	}	

	if (m_isInteracting)
	{
		m_pBG->Late_Tick(fTimeDelta);
		m_pBar->Late_Tick(fTimeDelta);
	}

	if (m_isWideCollision)
	{
		if (m_pNameTag)
		{
			m_pNameTag->Late_Tick(fTimeDelta);
		}
		if (m_pSpeechBubble)
		{
			m_pSpeechBubble->Late_Tick(fTimeDelta);
		}
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif // _DEBUG

}

HRESULT CLever::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i) {
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			return E_FAIL;

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

		_bool HasGlowTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_GlowTexture", i, TextureType::Specular)))
		{
			HasGlowTex = false;
		}
		else
		{
			HasGlowTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasGlowTex", &HasGlowTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_ShaderPassIndex)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLever::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_TOWER, TEXT("Prototype_Model_Lever"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}


	C3DUITex::UITEX_DESC TexDesc = {};
	TexDesc.eLevelID = LEVEL_STATIC;
	TexDesc.pParentTransform = m_pTransformCom;
	TexDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SpeechBubble");
	TexDesc.vSize = _vec2(40.f, 40.f);
	TexDesc.vPosition = _vec3(0.f, 3.f, 0.f);

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
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_CollectBar");
	ColButtonDesc.strTexture2 = TEXT("Prototype_Component_Texture_UI_Gameplay_Mask_FlagMove");
	ColButtonDesc.vSize = _vec2(180.f, 12.f);
	ColButtonDesc.vPosition = _vec2((_float)g_ptCenter.x, 200.f);
	ColButtonDesc.vColor = _vec4(0.31f, 0.96f, 1.f, 1.f);
	ColButtonDesc.strText = TEXT("");

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
	CNameTag::NAMETAG_DESC NameTagDesc = {};
	NameTagDesc.eLevelID = LEVEL_STATIC;
	NameTagDesc.fFontSize = 0.36f;
	NameTagDesc.pParentTransform = m_pTransformCom;
	NameTagDesc.vColor = _vec4(0.31f, 0.96f, 1.f, 1.f);
	NameTagDesc.vTextPosition = _vec2(0.f, 3.2f);
	NameTagDesc.strNameTag = TEXT("레버");
	m_pNameTag = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NameTag"), &NameTagDesc);
	if (not m_pNameTag)
	{
		return E_FAIL;
	}


	


	return S_OK;
}

HRESULT CLever::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_OldWorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
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

	_uint iOutlineColor = OutlineColor_Yellow;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_OutlineColor", &iOutlineColor, sizeof(_uint))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLever::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::Sphere;
	BodyCollDesc.fRadius = 1.f;
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider", L"Com_Collider_Body_Sphere", (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	Collider_Desc ColDesc2{};
	ColDesc2.eType = ColliderType::Sphere;
	ColDesc2.fRadius = 27.f;
	ColDesc2.vCenter = _vec3(0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Interaction_Sphere1"), (CComponent**)&m_pWideColliderCom, &ColDesc2)))
		return E_FAIL;

	return S_OK;
}

void CLever::Update_Collider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

CLever* CLever::Create(_dev pDevice, _context pContext)
{
	CLever* pInstance = new CLever(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLever::Clone(void* pArg)
{
	CLever* pInstance = new CLever(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLever::Free()
{
	__super::Free();

	Safe_Release(m_pBar);
	Safe_Release(m_pSpeechBubble);
	Safe_Release(m_pBG);
	Safe_Release(m_pNameTag);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pWideColliderCom);
}