#include "BlackSmith.h"
#include "3DUITex.h"
#include "UI_Manager.h"
#include "DialogText.h"
#include "TextButton.h"
#include "Event_Manager.h"
#include "TextButtonColor.h"
#include "Camera_Manager.h"

CBlackSmith::CBlackSmith(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CBlackSmith::CBlackSmith(const CBlackSmith& rhs)
	: CNPC(rhs)
{
}

HRESULT CBlackSmith::Init_Prototype()
{
    return S_OK;
}

HRESULT CBlackSmith::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_BlackSmith");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.4f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.6f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);


	if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

	if (FAILED(Init_Dialog()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}


	CUI_Manager::Get_Instance()->Set_RadarPos(CUI_Manager::NPC, m_pTransformCom);

	m_fButtonTime = m_pArrow->Get_Position().y;

	return S_OK;
}

void CBlackSmith::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	if (m_pArrow->Get_TransPosition().y < m_pArrow->Get_Position().y - 5.f)
	{
		m_fDir = 0.6f;
		m_pArrow->Set_Position(_float2(m_pArrow->Get_Position().x, m_pArrow->Get_Position().y - 5.f));
	}
	if (m_pArrow->Get_TransPosition().y > m_pArrow->Get_Position().y)
	{
		m_fDir = -1.f;
		m_pArrow->Set_Position(_float2(m_pArrow->Get_Position().x, m_pArrow->Get_Position().y));
	}
	m_fButtonTime += fTimeDelta * m_fDir * 10.f;
	m_pArrow->Set_Position(_float2(m_pArrow->Get_Position().x, m_fButtonTime));


	if (m_bTalking == true)
	{
		CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
		if (m_pGameInstance->Mouse_Down(DIM_RBUTTON, InputChannel::UI))
		{
			if (m_eState != TALK)
			{
				CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);
				CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
				m_bTalking = false;
				return;
			}
			Set_Text(m_eState);
		}
	}
	else
	{
		m_pSpeechBubble->Tick(fTimeDelta);
		m_Animation.iAnimIndex = IDLE;
	}

	m_pModelCom->Set_Animation(m_Animation);


	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	if (pCollider == nullptr)
	{
		return;
	}
	_bool isColl = m_pColliderCom->Intersect(pCollider);
	m_isColl = isColl;
	if (!m_bTalking && isColl && m_pGameInstance->Key_Down(DIK_E) /* && collider */) // 나중에 조건 추가
	{
		CCamera_Manager::Get_Instance()->Set_CameraState(CS_ZOOM);
		_vec4 vLook = m_pTransformCom->Get_State(State::Look);
		vLook.Normalize();
		_vec4 vTargetPos = m_pTransformCom->Get_State(State::Pos);
		CCamera_Manager::Get_Instance()->Set_CameraTargetPos(vTargetPos);
		CCamera_Manager::Get_Instance()->Set_CameraTargetLook(vLook);
		if (m_eState == QUEST_ING)
		{
			if (!CEvent_Manager::Get_Instance()->Find_Quest(m_strQuestOngoing))
			{
				m_eState = TALK;
				m_strQuestOngoing = TEXT("");
			}
		}
		m_bTalking = true;
		Set_Text(m_eState);
		m_Animation.iAnimIndex = TALK01;
	}


	if (m_pDialogText != nullptr)
	{
		m_pDialogText->Tick(fTimeDelta);
	}
	m_pSkipButton->Tick(fTimeDelta);
	m_pArrow->Tick(fTimeDelta);
	m_pLine->Tick(fTimeDelta);
	m_pBackGround->Tick(fTimeDelta);
	__super::Update_Collider();

	m_pTransformCom->Gravity(fTimeDelta);

	//사운드 채널 갱신 / 사운드 나오는 도중에 사운드 넘어가기 위해
	if (m_iSoundChannel != -1)
	{
		if (not m_pGameInstance->Get_IsPlayingSound(m_iSoundChannel))
		{
			m_iSoundChannel = -1;
		}
		else if(m_IsSoundSkip)
		{
			if (m_pGameInstance->Get_ChannelCurPosRatio(m_iSoundChannel) >= m_fSoundSkipRatio)
			{
				m_pGameInstance->StopSound(m_iSoundChannel);
				m_iSoundChannel = -1;

				m_IsSoundSkip = false;
			}
		}
	}
}

void CBlackSmith::Late_Tick(_float fTimeDelta)
{
	if (!m_bTalking)
	{
		if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 2.f))
		{
			if (m_isColl)
			{
				m_pSpeechBubble->Late_Tick(fTimeDelta);
			}
		}

	}
	else
	{
		m_pSkipButton->Late_Tick(fTimeDelta);
		m_pArrow->Late_Tick(fTimeDelta);
		m_pLine->Late_Tick(fTimeDelta);
		m_pBackGround->Late_Tick(fTimeDelta);
		if (m_pDialogText != nullptr)
		{
			m_pDialogText->Late_Tick(fTimeDelta);
		}
	}

	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 2.f))
	{
		__super::Late_Tick(fTimeDelta);
	}

}

HRESULT CBlackSmith::Render()
{
	__super::Render();

    return S_OK;
}

void CBlackSmith::Set_Text(BLACKSMITH_STATE eState)
{
	if (m_pDialogText != nullptr)
	{
		Safe_Release(m_pDialogText);
	}

	switch (eState)
	{
	case TALK:
	{
		if (m_DialogList.empty())
		{
			return;
		}
		wstring strText = m_DialogList.front();
		if (strText == TEXT("END"))
		{
			CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);
			CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
			m_bTalking = false;
			m_eState = ROSKVA_END;
			return;
		}

		if (strText[0] == L'!')
		{
			CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);
			CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
			m_bTalking = false;
			wstring strQuest = strText.substr(1, strText.length());
			CEvent_Manager::Get_Instance()->Set_Quest(strQuest);
			m_eState = QUEST_ING;
			m_strQuestOngoing = strQuest;
			m_DialogList.pop_front();
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
		Play_TalkSound(m_DialogList.front());
		m_DialogList.pop_front();
	}
	break;
	case QUEST_ING:
	{
		CDialogText::DIALOGTEXT_DESC TextDesc = {};
		TextDesc.eLevelID = LEVEL_STATIC;
		TextDesc.fDepth = (_float)D_TALK / (_float)D_END - 0.02f;
		TextDesc.strText = m_ChattList.front();
		TextDesc.vTextPos = _vec2((_float)g_ptCenter.x, 620.f);
		m_pDialogText = (CDialogText*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_DialogText"), &TextDesc);
		if (m_pDialogText == nullptr)
		{
			return;
		}
	}
	break;
	case ROSKVA_END:
	{
		CDialogText::DIALOGTEXT_DESC TextDesc = {};
		TextDesc.eLevelID = LEVEL_STATIC;
		TextDesc.fDepth = (_float)D_TALK / (_float)D_END - 0.02f;
		TextDesc.strText = m_ChattList.back();
		TextDesc.vTextPos = _vec2((_float)g_ptCenter.x, 620.f);
		m_pDialogText = (CDialogText*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_DialogText"), &TextDesc);
		if (m_pDialogText == nullptr)
		{
			return;
		}
	}
	break;
	}
}

void CBlackSmith::Play_TalkSound(const wstring& strTalkText)
{
	if (strTalkText == m_strLines[0] ||
		strTalkText == m_strLines[1] ||
		strTalkText == m_strLines[2] ||
		strTalkText == m_strLines[6] ||
		strTalkText == m_strLines[8] ||
		strTalkText == m_strLines[9] ||
		strTalkText == m_strLines[13] ||
		strTalkText == m_strLines[14] ||
		strTalkText == m_strLines[15] ||
		strTalkText == m_strLines[16])
	{
		if (m_iSoundChannel != -1)
		{
			m_pGameInstance->StopSound(m_iSoundChannel);
		}
		if (strTalkText == m_strLines[1] ||
			strTalkText == m_strLines[6])
		{
			m_IsSoundSkip = true;
			m_fSoundSkipRatio = 0.4f;
		}
		else if (strTalkText == m_strLines[8])
		{
			m_IsSoundSkip = true;
			m_fSoundSkipRatio = 0.5f;
		}
		m_iSoundChannel = m_pGameInstance->Play_Sound(m_TalkSounds.front());
		m_TalkSounds.pop_front();
	}
}

HRESULT CBlackSmith::Init_Dialog()
{
	for (size_t i = 0; i < size(m_strLines); i++)
	{
		m_DialogList.push_back(m_strLines[i]);
	}

	m_ChattList.push_back(TEXT("뭐야 재료를 가져와야 만들어주지!!"));
	m_ChattList.push_back(TEXT("뚝딱뚝딱"));

	m_TalkSounds.push_back(TEXT("10035_3_EndTalk"));
	m_TalkSounds.push_back(TEXT("10063_1_StartTalk_1"));
	m_TalkSounds.push_back(TEXT("10063_1_StartTalk_2"));
	m_TalkSounds.push_back(TEXT("10063_1_StartTalk_3"));
	m_TalkSounds.push_back(TEXT("10064_1_StartTalk_1"));
	m_TalkSounds.push_back(TEXT("10064_1_StartTalk_2"));
	m_TalkSounds.push_back(TEXT("10071_3_EndTalk"));
	m_TalkSounds.push_back(TEXT("10040_1_StartTalk_2"));
	m_TalkSounds.push_back(TEXT("10040_1_StartTalk_3"));
	m_TalkSounds.push_back(TEXT("10041_2_InformTalk"));
	
	return S_OK;
}

HRESULT CBlackSmith::Add_Parts()
{
	_float fDepth = (_float)D_TALK / (_float)D_END;
	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.eLevelID = LEVEL_STATIC;
	ButtonDesc.fDepth = fDepth - 0.01f;
	ButtonDesc.fFontSize = 0.45f;
	ButtonDesc.strText = TEXT("");
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.vTextPosition = _vec2(20.f, 0.f);
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_ClickArrow");
	ButtonDesc.vPosition = _vec2(1100.f, 600.f);
	ButtonDesc.vSize = _vec2(30.f, 30.f);

	m_pArrow = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pArrow)
	{
		return E_FAIL;
	}
	ButtonDesc.vTextPosition = _vec2(40.f, 0.f);
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.vTextBorderColor = _vec4(0.3f, 0.75f, 0.87f, 1.f);
	ButtonDesc.strText = TEXT("SKIP");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BlueArrow");
	ButtonDesc.vPosition = _vec2(1180.f, 40.f);
	ButtonDesc.vSize = _vec2(30.f, 30.f);

	m_pSkipButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pSkipButton)
	{
		return E_FAIL;
	}
	ButtonDesc.vTextBorderColor = _vec4(0.f, 0.f, 0.f, 1.f);
	ButtonDesc.vTextColor = _vec4(1.f, 0.95f, 0.87f, 1.f);
	ButtonDesc.strText = TEXT("블랙스미스");
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


	C3DUITex::UITEX_DESC TexDesc = {};
	TexDesc.eLevelID = LEVEL_STATIC;
	TexDesc.pParentTransform = m_pTransformCom;
	TexDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SpeechBubble");
	TexDesc.vPosition = _vec3(0.f, 2.2f, 0.f);
	TexDesc.vSize = _vec2(40.f, 40.f);

	m_pSpeechBubble = (C3DUITex*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &TexDesc);
	if (not m_pSpeechBubble)
	{
		return E_FAIL;
	}

	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(10.f, 1.f, 10.f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.9f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_BlackSmith"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

CBlackSmith* CBlackSmith::Create(_dev pDevice, _context pContext)
{
	CBlackSmith* pInstance = new CBlackSmith(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBlackSmith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlackSmith::Clone(void* pArg)
{
	CBlackSmith* pInstance = new CBlackSmith(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBlackSmith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackSmith::Free()
{
	__super::Free();

	Safe_Release(m_pLine);
	Safe_Release(m_pArrow);
	Safe_Release(m_pDialogText);
	Safe_Release(m_pSkipButton);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pSpeechBubble);
}
