#include "BlackCat.h"
#include "Effect_Manager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "TextButtonColor.h"
#include "TextButton.h"
#include "Dialog.h"
#include "3DUITex.h"
#include "DialogText.h"

CBlackCat::CBlackCat(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBlackCat::CBlackCat(const CBlackCat& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlackCat::Init_Prototype()
{
	return S_OK;
}

HRESULT CBlackCat::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(75.f, 0.f, 110.f, 1.f));
	m_Animation.fAnimSpeedRatio = 2.f;
	m_Animation.bSkipInterpolation = false;
	m_Animation.isLoop = true;

	m_pTransformCom->Set_Scale(_vec3(2.f, 2.f, 2.f));

	m_eCurState = STATE_START;
	m_iPassIndex = AnimPass_Default;

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	/*
	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Cat_Light");
	Info.pMatrix = &m_EffectMatrixLight;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info, true);
	*/
	
	m_pColliderCom->Set_Normal();
	m_pTransformCom->Set_State(State::Pos, _vec4(-2000.f, -1.f, -2007.f, 1.f));
	return S_OK;
}

void CBlackCat::Tick(_float fTimeDelta)
{
	if (m_bChangePass == true)
	{
		m_fHitTime += fTimeDelta;

		if (m_iPassIndex == AnimPass_Default)
		{
			m_iPassIndex = AnimPass_Rim;
		}
		else
		{
			m_iPassIndex = AnimPass_Default;
		}

		if (m_fHitTime >= 1.f)
		{
			m_fHitTime = 0.f;
			m_bChangePass = false;
			m_iPassIndex = AnimPass_Default;
		}
	}

	

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);
	Update_Collider();
	m_pModelCom->Set_Animation(m_Animation);

	m_EffectMatrixLight = _mat::CreateTranslation(2.f, 3.5f, 0.9f) * m_pTransformCom->Get_World_Matrix();
	m_EffectMatrix = _mat::CreateTranslation(0.f, 2.f, 0.9f) * m_pTransformCom->Get_World_Matrix();
	if (m_bHit)
	{
		m_pDialog->Tick(fTimeDelta);
	}
}

void CBlackCat::Late_Tick(_float fTimeDelta)
{
	if (m_eCurState == STATE_START)
	{
		if (m_pDialogText)
		{
			m_pDialogText->Late_Tick(fTimeDelta);
		}
		if (m_pBackGround)
		{
			m_pBackGround->Late_Tick(fTimeDelta);
		}
		if (m_pLine)
		{
			m_pLine->Late_Tick(fTimeDelta);
		}
	}

	if (m_eCurState != STATE_CHANGE && m_eCurState != STATE_START)
	{
		m_pHpBG->Late_Tick(fTimeDelta);
		m_pHpBar->Late_Tick(fTimeDelta);
		m_pHpBorder->Late_Tick(fTimeDelta);
	}

	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

	if (m_bHit && m_eCurState != STATE_CHANGE)
	{
		//m_pDialog->Late_Tick(fTimeDelta);
	}
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBlackCat::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
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

	if (!m_bChangePass)
	{
		m_iPassIndex = AnimPass_Default;
	}
	return S_OK;
}

void CBlackCat::Init_State(_float fTimeDelta)
{

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case STATE_START:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			CCamera_Manager::Get_Instance()->Set_ZoomFactor(2.f);
			break;
		case Client::CBlackCat::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;
		case Client::CBlackCat::STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;
		case Client::CBlackCat::STATE_ANGRY:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;
		case Client::CBlackCat::STATE_HIT:
			m_Animation.iAnimIndex = EMOTION;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 3.f;
			break;
		case Client::CBlackCat::STATE_CHANGE:
			CCamera_Manager::Get_Instance()->Set_ZoomFactor(2.f);
			m_Animation.iAnimIndex = TELEPORT_START;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 1.4f;
			break;
		case Client::CBlackCat::STATE_DIE:
			m_Animation.iAnimIndex = TELEPORT_END;
			m_Animation.isLoop = false;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CBlackCat::Tick_State(_float fTimeDelta)
{
	m_fBarFloating += fTimeDelta * 2.f;
	m_pHpBar->Set_Time(m_fBarFloating);

	if (m_fTargetHp < m_Hp.x)
	{
		m_pHpBar->Set_Bright(true);
		m_Hp.x -= fTimeDelta * 5.f;
	

	}
	else
	{
		m_Hp.x = (_float)m_fTargetHp;
		m_pHpBar->Set_Bright(false);
	}
	
	
	if (m_iHitCount >= 2)
	{
		m_iHitCount = 0;
		m_fTargetHp -= 2.f;
		if (m_fTargetHp <= 0.f)
		{
			m_eCurState = STATE_DIE;
		}
	}

	if (m_Hp.x <= m_Hp.y * 2.f / 3.f && !m_bChangePhase)
	{
		m_bChangePhase = true;
		m_eCurState = STATE_CHANGE;
	}

	m_pHpBar->Set_Factor(m_Hp.x / (_float)m_Hp.y);


	m_pHpBG->Tick(fTimeDelta);
	m_pHpBar->Tick(fTimeDelta);
	m_pHpBorder->Tick(fTimeDelta);





	m_pTransformCom->Set_State(State::Pos, _vec4(-2000.f, -1.f, -2007.f, 1.f));
	_vec3 vNormal = _vec3(0.f, 0.f, -1.f);
	switch (m_eCurState)
	{
	case STATE_START:
	{		
		m_pTransformCom->LookAt_Dir(vNormal);
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
		{
			Set_Text();
		}

		if (m_pDialogText)
		{
			m_pDialogText->Tick(fTimeDelta);
		}
		if (m_pBackGround)
		{
			m_pBackGround->Tick(fTimeDelta);
		}
		if (m_pLine)
		{
			m_pLine->Tick(fTimeDelta);
		}
	}
		break;
	case CBlackCat::STATE_IDLE:
	{
		m_pTransformCom->LookAt_Dir(vNormal);
		CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_BrickGame"), TEXT("BrickBall"));
		if (pCollider != nullptr && m_pColliderCom->Intersect(pCollider))
		{
			m_bChangePass = true;
			_uint iRandomText = m_vecText.size() - 1;
			iRandomText = rand() % iRandomText;
			dynamic_cast<CDialog*>(m_pDialog)->Set_Text(m_vecText[iRandomText]);
			m_bHit = true;
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 1.6f);
			m_eCurState = STATE_HIT;
			m_iHitCount++;
		}
	}
		break;

	case CBlackCat::STATE_ANGRY:
	{

	}
		break;
	case CBlackCat::STATE_HIT:
	{

		if (m_pModelCom->IsAnimationFinished(EMOTION))
		{
			m_bHit = false;
			m_eCurState = STATE_IDLE;
		}

		CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_BrickGame"), TEXT("BrickBall"));
		if (pCollider != nullptr && !m_bChangePass && m_pColliderCom->Intersect(pCollider))
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 1.6f);
			m_bChangePass = true;
			m_iHitCount++;
		}

	}
		break;
	case CBlackCat::STATE_CHANGE:
	{

		if (m_pModelCom->IsAnimationFinished(TELEPORT_END))
		{
			CCamera_Manager::Get_Instance()->Set_ZoomFactor(-1.f);
			m_eCurState = STATE_ANGRY;
			m_bPhaseStart = true;

		}
		else if (m_pModelCom->IsAnimationFinished(TELEPORT_START))
		{
			_mat Matrix = _mat::CreateTranslation(0.f, 1.f, 0.f) * m_pTransformCom->Get_World_Matrix();
			CCamera_Manager::Get_Instance()->Set_ZoomFactor(0.f);
			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Cat_Dust");
			Info.pMatrix = &Matrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);


			m_Animation.iAnimIndex = TELEPORT_END;
		}
	}
	break;
	case CBlackCat::STATE_DIE:
	{
		///Safe_Release(m_pHpBG);
		///Safe_Release(m_pHpBar);
		///Safe_Release(m_pHpBorder);
		if (m_pModelCom->IsAnimationFinished(TELEPORT_END))
		{

			m_isDead = true;
		}
	}
	break;
	}


}

HRESULT CBlackCat::Add_Parts()
{
	m_DialogList.push_back(TEXT("안녕"));
	m_DialogList.push_back(TEXT("게임을 시작하지"));
	m_DialogList.push_back(TEXT("END"));

	CDialog::DIALOG_DESC DialogDesc = {};
	DialogDesc.eLevelID = LEVEL_STATIC;
	DialogDesc.pParentTransform = m_pTransformCom;
	DialogDesc.vPosition = _vec3(0.f, 12.f, 0.f);
	DialogDesc.strText = TEXT("처음");

	m_pDialog = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Dialog"), &DialogDesc);
	if (m_pDialog == nullptr)
	{
		return E_FAIL;
	}

	m_vecText.push_back(TEXT("아얏"));
	m_vecText.push_back(TEXT("아파!!"));
	m_vecText.push_back(TEXT("어이쿠"));

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_CatSmoke1");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info, true);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_CatSmoke2");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info, true);

	_float fDepth = (_float)D_TALK / (_float)D_END;
	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.vTextBorderColor = _vec4(0.f, 0.f, 0.f, 1.f);
	ButtonDesc.vTextColor = _vec4(1.f, 0.95f, 0.87f, 1.f);
	ButtonDesc.strText = TEXT("검은고양이");
	ButtonDesc.fFontSize = 0.5f;
	ButtonDesc.vTextPosition = _vec2(0.f, -30.f);
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SiegeQuest");
	ButtonDesc.vPosition = _vec2((_float)g_ptCenter.x, 590.f);
	ButtonDesc.vSize = _vec2(400.f, 10.f);

	m_pLine = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pLine)
	{
		return E_FAIL;
	}

	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = fDepth;
	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.fFontSize = 0.f;
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_FadeBox");
	ColButtonDesc.vSize = _vec2(g_iWinSizeX, 250.f);
	ColButtonDesc.vPosition = _vec2((_float)g_ptCenter.x, (_float)g_iWinSizeY);


	m_pBackGround = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}
	m_pBackGround->Set_Pass(VTPass_FadeVertical);

	return S_OK;
}

HRESULT CBlackCat::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_BlackCat"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	C3DUITex::UITEX_DESC TexDesc = {};
	TexDesc.eLevelID = LEVEL_TOWER;
	TexDesc.pParentTransform = m_pTransformCom;
	TexDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_CatHpBg");
	TexDesc.vPosition = _vec3(0.f, 9.8f, 0.1f);
	TexDesc.vSize = _vec2(200.f, 200.f);

	m_pHpBG = (C3DUITex*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &TexDesc);
	if (not m_pHpBG)
	{
		return E_FAIL;
	}
	
	TexDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_CatHpHp");
	TexDesc.strTexture2 = TEXT("Prototype_Component_Texture_UI_Gameplay_Mask_FlagMove");
	TexDesc.vPosition = _vec3(0.f, 9.8f, 0.f);

	m_pHpBar = (C3DUITex*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &TexDesc);
	if (not m_pHpBar)
	{
		return E_FAIL;
	}	
	m_pHpBar->Set_Pass(VTPass_HPBoss);
	m_pHpBar->Set_Factor(1.f);

	TexDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_CatHpUp");
	TexDesc.strTexture2 = TEXT("");
	TexDesc.vPosition = _vec3(0.f, 9.8f, -0.1f);

	m_pHpBorder = (C3DUITex*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &TexDesc);
	if (not m_pHpBorder)
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBlackCat::Bind_ShaderResources()
{
	if (m_iPassIndex == AnimPass_Rim && m_bChangePass)
	{
		_vec4 vColor = Colors::Red;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &vColor, sizeof vColor)))
		{
			return E_FAIL;
		}
	}

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

	return S_OK;
}

HRESULT CBlackCat::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(1.5f, 1.5f, 1.5f);
	CollDesc.vCenter = _vec3(0.f, 1.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Bar"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

_bool CBlackCat::Create_Bricks()
{
	if (m_bPhaseStart)
	{
		m_bPhaseStart = false;
		return true;
	}
	return false;
}

void CBlackCat::Set_Text()
{
	if (m_pDialogText != nullptr)
	{
		Safe_Release(m_pDialogText);
	}

	if (m_DialogList.empty())
	{
		return;
	}

	wstring strText = m_DialogList.front();
	if (strText == TEXT("END"))
	{
		CCamera_Manager::Get_Instance()->Set_ZoomFactor(0.f);
		m_eCurState = STATE_IDLE;
		m_bGameStart = true;
		return;
	}


	CDialogText::DIALOGTEXT_DESC TextDesc = {};
	TextDesc.eLevelID = LEVEL_STATIC;
	TextDesc.fDepth = (_float)D_TALK / (_float)D_END - 0.01f;
	TextDesc.strText = m_DialogList.front();
	TextDesc.vTextPos = _vec2((_float)g_ptCenter.x, 620.f);
	m_pDialogText = (CDialogText*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_DialogText"), &TextDesc);
	if (m_pDialogText == nullptr)
	{
		return;
	}
	m_DialogList.pop_front();
}

void CBlackCat::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}


CBlackCat* CBlackCat::Create(_dev pDevice, _context pContext)
{
	CBlackCat* pInstance = new CBlackCat(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBlackCat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlackCat::Clone(void* pArg)
{
	CBlackCat* pInstance = new CBlackCat(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBlackCat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackCat::Free()
{
	__super::Free();

	Safe_Release(m_pLine);
	Safe_Release(m_pDialogText);
	Safe_Release(m_pBackGround);

	Safe_Release(m_pHpBG);
	Safe_Release(m_pHpBar);
	Safe_Release(m_pHpBorder);

	Safe_Release(m_pDialog);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
}
