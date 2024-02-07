#include "Camera_CutScene.h"
//#include "Trigger_Manager.h"
CCamera_CutScene::CCamera_CutScene(_dev pDevice, _context pContext)
	: CCamera(pDevice, pContext)
{

}

CCamera_CutScene::CCamera_CutScene(const CCamera_CutScene& rhs)
	: CCamera(rhs)
	, m_pTrigger_Manager(CTrigger_Manager::Get_Instance())
{
	Safe_AddRef(m_pTrigger_Manager);
}

HRESULT CCamera_CutScene::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera_CutScene::Init(void* pArg)
{

	if (FAILED(__super::Init(pArg)))
	{
		return E_FAIL;
	}

	m_iNextSectionIndex = 0;
	m_iCurrentSectionIndex = 0;
	m_fCutSceneSpeed = 10.f;
	m_fTimeDeltaAcc = 0.f;
	m_iLastFrame = 0;

	m_pTrigger_Manager->Set_CutSceneCamera(this);

	return S_OK;
}

void CCamera_CutScene::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CameraModeIndex() != CM_CUTSCENE )
	{
		return;
	}
	if (m_pGameInstance->Key_Down(DIK_L))
	{
		m_pGameInstance->Set_CameraModeIndex(CM_MAIN);
	}
	m_pGameInstance->Set_CameraNF(_float2(m_fNear, m_fFar));

	if (m_isPlayCutScene == false)
	{
		m_iFrame = 0;
		m_fTimeDeltaAcc = 0.f;
		m_iTotalFrame = 0;
		m_fTotalTimeDeltaAcc = 0.f;
		m_iCurrentSectionIndex = 0;
		m_iNextSectionIndex = 0;
		m_isPlayCutScene = true;
	}
	else
		Play_Camera(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CCamera_CutScene::Late_Tick(_float fTimeDelta)
{

}


HRESULT CCamera_CutScene::Add_Eye_Curve(_mat matPoints,  _float fCurveSpeed)
{
	_uint	iCutSceneType = CCutScene_Curve::SECTION_TYPE_EYE;

	SectionInfo SectionInfo;
	//SectionInfo.vStartCutScene = matPoints.Up();
	//SectionInfo.vEndCutScene = matPoints.Look();
	SectionInfo.mCutSceneMatrix = matPoints;
	SectionInfo.iSectionType = iCutSceneType;
	SectionInfo.ppCurve = &m_pEyeCurve;
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Camera_Curve"), TEXT("Prototype_GameObject_Camera_Curve"), &SectionInfo)))
	{
		return E_FAIL;
	}

	m_pEyeCurve->Set_SectionSpeed(fCurveSpeed);
	m_pEyeCurve->Set_ControlPoints(matPoints);
	m_CameraEyeList.push_back(static_cast<CCutScene_Curve*>(m_pEyeCurve));
	m_pEyeCurve = nullptr;
	return S_OK;
}

HRESULT CCamera_CutScene::Add_At_Curve(_mat matPoints)
{
	_uint	iCurrentLevel = m_pGameInstance->Get_CurrentLevelIndex();
	_uint	iCutSceneType = CCutScene_Curve::SECTION_TYPE_AT;

	SectionInfo SectionInfo;
	//SectionInfo.vStartCutScene = matPoints.Up();
	//SectionInfo.vEndCutScene = matPoints.Look();
	SectionInfo.mCutSceneMatrix = matPoints;
	SectionInfo.iSectionType = iCutSceneType;
	SectionInfo.ppCurve = &m_pAtCurve;

	if (FAILED(m_pGameInstance->Add_Layer(iCurrentLevel, TEXT("Layer_Camera_Curve"), TEXT("Prototype_GameObject_Camera_Curve"), &SectionInfo)))
	{
		return E_FAIL;
	}

	m_pAtCurve->Set_SectionSpeed(5.f);
	m_pAtCurve->Set_ControlPoints(matPoints);
	m_CameraAtList.push_back(static_cast<CCutScene_Curve*>(m_pAtCurve));
	m_pAtCurve = nullptr;

	return S_OK;
}

void CCamera_CutScene::Play_Camera(_float fTimeDelta)
{

	if (m_CameraEyeList.size() > 0)
	{
		m_iSectionCount = (_uint)m_CameraEyeList.size();
		m_iLastFrame = m_CameraEyeList.front()->Get_CurveSize() * m_iSectionCount;
	}

	if (m_iFrame > m_CameraEyeList.front()->Get_CurveSize() - 2 && !m_CameraEyeList.empty())
	{
		m_iFrame = 0;
		m_fTimeDeltaAcc = 0.f;
		m_iNextSectionIndex = m_iCurrentSectionIndex + 1;

		if (m_iNextSectionIndex < m_CameraEyeList.size())
		{
			m_iCurrentSectionIndex = m_iNextSectionIndex;
		}
		else
		{
			m_isPlayCutScene = false;
			m_pGameInstance->Set_CameraModeIndex(CM_MAIN);
		}
	}
	if (m_iCurrentSectionIndex < m_CameraEyeList.size())
	{
		_vec4 vFrameEyePos = m_CameraEyeList[m_iCurrentSectionIndex]->Get_CurvePos(m_iFrame);
		_vec4 vFrameAtPos = m_CameraAtList[m_iCurrentSectionIndex]->Get_CurvePos(m_iFrame);

		_float fSectionSpeed = m_CameraEyeList[m_iCurrentSectionIndex]->Get_SectionSpeed();

		if (fTimeDelta < 1.0)
		{
			m_fTimeDeltaAcc += (_float)fTimeDelta * m_fCutSceneSpeed * fSectionSpeed;
		}

		m_fTotalTimeDeltaAcc += (_float)fTimeDelta * m_fCutSceneSpeed * fSectionSpeed;
		m_iFrame = (_uint)m_fTimeDeltaAcc;
		m_iTotalFrame = (_uint)m_fTotalTimeDeltaAcc;

		_vec4 vAt = _vec4(vFrameAtPos.x, vFrameAtPos.y, vFrameAtPos.z, 1.f);
		m_pTransformCom->Set_State(State::Pos, _vec4(vFrameEyePos.x, vFrameEyePos.y, vFrameEyePos.z, 1.f));
		m_pTransformCom->LookAt(vAt);
	}

}


CCamera_CutScene* CCamera_CutScene::Create(_dev pDevice, _context pContext)
{
	CCamera_CutScene* pInstance = new CCamera_CutScene(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_CutScene::Clone(void* pArg)
{
	CCamera_CutScene* pInstance = new CCamera_CutScene(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_CutScene::Free()
{
	__super::Free();
	for (int i = 0; i < m_CameraAtList.size(); i++) {
		Safe_Release(m_CameraAtList[i]);
	}
	m_CameraAtList.clear();

	for (int i = 0; i < m_CameraEyeList.size(); i++) {
		Safe_Release(m_CameraEyeList[i]);
	}
	m_CameraEyeList.clear();

	//Safe_Release(m_pEyeCurve);
	//Safe_Release(m_pAtCurve);
	if(m_pTrigger_Manager)
		Safe_Release(m_pTrigger_Manager);
}