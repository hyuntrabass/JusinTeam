#include "Trigger_Manager.h"
#include "Trigger.h"
#include "Camera_CutScene.h"

IMPLEMENT_SINGLETON(CTrigger_Manager)

CTrigger_Manager::CTrigger_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}
HRESULT CTrigger_Manager::Init()
{
	return S_OK;
}

void CTrigger_Manager::Tick(_float fTimeDelta)
{
	for (auto& iter : m_pTrigger)
	{
		//m_pGameInstance->Get_CurrentLevelIndex();
		m_isColl = iter->Get_Collision();
		if (m_isColl == true)
		{
			TriggerType test = iter->Get_TriggerType();
			_bool tests = iter->Get_Limited();
			if (iter->Get_TriggerType() == VILLAGE_TRIGGER && iter->Get_Limited() == true)
			{
				CutScene_Registration(L"../Bin/Data/Village_CutScene.dat");
				m_pGameInstance->Set_CameraModeIndex(CM_CUTSCENE);
				iter->Set_Limited(false);
			}
			else if (iter->Get_TriggerType() == FRONTDOOR_IN_TRIGGER && iter->Get_Limited() == false)
			{

			}
			else if (iter->Get_TriggerType() == FRONTDOOR_OUT_TRIGGER && iter->Get_Limited() == false)
			{

			}
			else if (iter->Get_TriggerType() == BACKDOOR_IN_TRIGGER && iter->Get_Limited() == false)
			{

			}
			else if (iter->Get_TriggerType() == BACKDOOR_OUT_TRIGGER && iter->Get_Limited() == false)
			{

			}
			else if (iter->Get_TriggerType() == BOSS_TRIGGER && iter->Get_Limited() == true)
			{
				CutScene_Registration(L"../Bin/Data/test_CutScene.dat");
				m_pGameInstance->Set_CameraModeIndex(CM_CUTSCENE);
				iter->Set_Limited(false);
			}
		}
	}
}

void CTrigger_Manager::Limited_CutScene(_bool isLimited)
{
	m_isLimited = isLimited;
}

void CTrigger_Manager::Set_Trigger(CTrigger* pTrigger)
{
	m_pTrigger.push_back(pTrigger);
}

void CTrigger_Manager::Set_CutSceneCamera(CCamera_CutScene* pCutScene)
{
	m_pCutScene = pCutScene;
}

HRESULT CTrigger_Manager::CutScene_Registration(const wstring& strDataPath)
{
	//const TCHAR* pGetPath = L"../Bin/Data/test_CutScene.dat";
	const TCHAR* pGetPath = strDataPath.c_str();

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"../Bin/Data/Village_CutScene.dat 파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
		return E_FAIL;
	}

	_uint InputNameSize;
	string InputName;
	inFile.read(reinterpret_cast<char*>(&InputNameSize), sizeof(_uint));
	inFile.read(reinterpret_cast<char*>(&InputName[0]), sizeof(InputNameSize));

	_uint iEyeSize;
	_uint iAtSize;
	inFile.read(reinterpret_cast<char*>(&iEyeSize), sizeof(_uint));
	inFile.read(reinterpret_cast<char*>(&iAtSize), sizeof(_uint));

	_mat m_mCameraEyePoint{};
	_mat m_mCameraAtPoint{};
	// 카메라 Eye
	_vec4 vEyeStartCurved{};
	_vec4 vEyeStartPos{};
	_vec4 vEyeEndPos{};
	_vec4 vEyeEndCurved{};
	// 카메라 At
	_vec4 vAtStartCurved{};
	_vec4 vAtStartPos{};
	_vec4 vAtEndPos{};
	_vec4 vAtEndCurved{};
	_float fEyeSpeed{};
	_float fCameraSpeed{};

	for (_uint i = 0; i < iEyeSize; i++)
	{

		inFile.read(reinterpret_cast<char*>(&vEyeStartCurved), sizeof(_vec4));
		m_mCameraEyePoint.Right(vEyeStartCurved);
		inFile.read(reinterpret_cast<char*>(&vEyeStartPos), sizeof(_vec4));
		m_mCameraEyePoint.Up(vEyeStartPos);
		inFile.read(reinterpret_cast<char*>(&vEyeEndPos), sizeof(_vec4));
		m_mCameraEyePoint.Look(vEyeEndPos);
		inFile.read(reinterpret_cast<char*>(&vEyeEndCurved), sizeof(_vec4));
		m_mCameraEyePoint.Position(vEyeEndCurved);

		inFile.read(reinterpret_cast<char*>(&fCameraSpeed), sizeof(_float));

		if (FAILED(m_pCutScene->Add_Eye_Curve(m_mCameraEyePoint, fCameraSpeed)))
		{
			return E_FAIL;
		}

	}
	for (_uint i = 0; i < iAtSize; ++i)
	{
		inFile.read(reinterpret_cast<char*>(&vAtStartCurved), sizeof(_vec4));
		m_mCameraAtPoint.Right(vAtStartCurved);
		inFile.read(reinterpret_cast<char*>(&vAtStartPos), sizeof(_vec4));
		m_mCameraAtPoint.Up(vAtStartPos);
		inFile.read(reinterpret_cast<char*>(&vAtEndPos), sizeof(_vec4));
		m_mCameraAtPoint.Look(vAtEndPos);
		inFile.read(reinterpret_cast<char*>(&vAtEndCurved), sizeof(_vec4));
		m_mCameraAtPoint.Position(vAtEndCurved);

		if (FAILED(m_pCutScene->Add_At_Curve(m_mCameraAtPoint)))
		{
			return E_FAIL;
		}
	}

	inFile.close();

	m_pGameInstance->Set_CameraModeIndex(CM_CUTSCENE);

	return S_OK;

}

void CTrigger_Manager::Free()
{
	for (auto& iter : m_pTrigger)
	{
		Safe_Release(iter);
	}
	m_pTrigger.clear();

	Safe_Release(m_pCutScene);
	Safe_Release(m_pGameInstance);
}
