#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "GameObject.h"
#include "Animation.h"
#include "Layer.h"

#include "Player.h"

#include <commdlg.h>

CImgui_Manager::CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImgui_Manager::Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc)
{
	m_hWnd = GraphicDesc.hWnd;
	m_iWinSizeX = GraphicDesc.iWinSizeX;
	m_iWinSizeY = GraphicDesc.iWinSizeY;

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	GetCursorPos(&m_ptMouse);
	ScreenToClient(m_hWnd, &m_ptMouse);

	if (m_pPlayer)
	{
		if (m_pGameInstance->Key_Down(DIK_X))
		{
			m_pPlayer->Set_TimeDelta(0.f);
		}
		else if(m_pGameInstance->Key_Down(DIK_C))
		{
			m_pPlayer->Set_TimeDelta(fTimeDelta);
		}
		m_pPlayer->Tick(fTimeDelta);
	}
}

void CImgui_Manager::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer)
	{
		m_pPlayer->Late_Tick(fTimeDelta);
	}
}

HRESULT CImgui_Manager::Render()
{
	if (m_pPlayer)
	{
		m_pPlayer->Render();
	}

	bool bDemo = true;
	ImGui::ShowDemoWindow(&bDemo);
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGuiMenu();

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
	ImGuizmo::SetRect(0, 0, (_float)(m_iWinSizeX), (_float)(m_iWinSizeY));
	ImGuizmoMenu();

	ImGui::Render();

	return S_OK;
}

HRESULT CImgui_Manager::ImGuiMenu()
{
#pragma endregion
	ImGui::Begin("MENU");

	ImGui::SeparatorText("SELECT"); 
	
	ImGui::RadioButton("MONSTER", &m_eType, TYPE_MONSTER); ImGui::SameLine();
	ImGui::RadioButton("PLAYER", &m_eType, TYPE_PLAYER);

	if (m_eType == TYPE_MONSTER)
	{
		const char* szModelTag[19] = { "Barlog","Furgoat","GiantBoss","Goat","Hirokin","Nastron02","Nastron03","Nott","NPCvsMon","Orc02","Penguin",
		"Rabbit","Skjaldmaer","Skjaldmaer_A","Thief04","Trilobite","TrilobiteA","Void13","VoidDragon" };
		static const char* szCurrentModel = "Barlog";
		if (m_ePreType != m_eType)
		{
			m_ePreType = m_eType;
			m_iCurrentModelIndex = 0;
			szCurrentModel = "Barlog";
		}

		if (ImGui::BeginCombo("LIST", szCurrentModel))
		{
			for (size_t i = 0; i < IM_ARRAYSIZE(szModelTag); i++)
			{
				_bool bSelectedModel = (szCurrentModel == szModelTag[i]);
				if (ImGui::Selectable(szModelTag[i], bSelectedModel))
				{
					szCurrentModel = szModelTag[i];
					m_iCurrentModelIndex = i;
				}
			}
			ImGui::EndCombo();
		}
	}
	else if (m_eType == TYPE_PLAYER)
	{
		const char* szModelTag[4] = { "Select_Priest","Select_Rogue","Select_Sorceress","Select_Warrior" };
		static const char* szCurrentModel = "Select_Priest";
		if (m_ePreType != m_eType)
		{
			m_ePreType = m_eType;
			m_iCurrentModelIndex = 0;
			szCurrentModel = "Select_Priest";
		}

		if (ImGui::BeginCombo("LIST", szCurrentModel))
		{
			for (size_t i = 0; i < IM_ARRAYSIZE(szModelTag); i++)
			{
				_bool bSelectedModel = (szCurrentModel == szModelTag[i]);
				if (ImGui::Selectable(szModelTag[i], bSelectedModel))
				{
					szCurrentModel = szModelTag[i];
					m_iCurrentModelIndex = i;
				}
			}
			ImGui::EndCombo();
		}
	}

	ImGui::SeparatorText("FILE");

	if (ImGui::Button("SAVE"))
	{
		SaveFile();
	}
	ImGui::SameLine();
	if (ImGui::Button("LOAD"))
	{
		LoadFile();
	}

#pragma region CreateObject

	//if (ImGui::Button("CREATE"))
	if (m_IsCreateModel)
	{
		if (not m_pPlayer)
		{
			m_pPlayer = (CPlayer*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Player"));
			CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));

			m_vPreScale = pTargetTransform->Get_Scale();
			m_vPreRight = pTargetTransform->Get_State(State::Right);
			m_vPreUp = pTargetTransform->Get_State(State::Up);
			m_vPreLook = pTargetTransform->Get_State(State::Look);
			m_vPrePosition = pTargetTransform->Get_State(State::Pos);
		}
	}
#pragma endregion

	ImGui::SameLine();

	ImGui::End();

	//ImGuizmo
	if (m_pPlayer)
	{
		ImGui::Begin("OBJECT MENU");

		ImGui::RadioButton("STATE", &m_iManipulateType, TYPE_STATE); ImGui::SameLine();
		ImGui::RadioButton("RESET", &m_iManipulateType, TYPE_RESET);
		m_eManipulateType = (MANIPULATETYPE)(m_iManipulateType);
		if (ImGui::Button("SCALE"))
		{
			if (m_eManipulateType == TYPE_STATE)
			{
				m_eStateType = ImGuizmo::OPERATION::SCALE;
			}
			else if (m_eManipulateType == TYPE_RESET)
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_Scale(m_vPreScale);
				m_vCurrentScale = m_vPreScale;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("ROTATION"))
		{
			if (m_eManipulateType == TYPE_STATE)
			{
				m_eStateType = ImGuizmo::OPERATION::ROTATE;
			}
			else if (m_eManipulateType == TYPE_RESET)
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_State(State::Right, XMVector3Normalize(XMLoadFloat4(&m_vPreRight)) * m_vCurrentScale.x);
				pTargetTransform->Set_State(State::Up, XMVector3Normalize(XMLoadFloat4(&m_vPreUp)) * m_vCurrentScale.y);
				pTargetTransform->Set_State(State::Look, XMVector3Normalize(XMLoadFloat4(&m_vPreLook)) * m_vCurrentScale.z);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("POSITION"))
		{
			if (m_eManipulateType == TYPE_STATE)
			{
				m_eStateType = ImGuizmo::OPERATION::TRANSLATE;
			}
			else if (m_eManipulateType == TYPE_RESET)
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_State(State::Pos, XMLoadFloat4(&m_vPrePosition));
			}
		}
		if (m_eManipulateType == TYPE_RESET)
		{
			ImGui::SameLine();
			if (ImGui::Button("ALL"))
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_State(State::Right, XMLoadFloat4(&m_vPreRight));
				pTargetTransform->Set_State(State::Up, XMLoadFloat4(&m_vPreUp));
				pTargetTransform->Set_State(State::Look, XMLoadFloat4(&m_vPreLook));
				pTargetTransform->Set_State(State::Pos, XMLoadFloat4(&m_vPrePosition));
				m_vCurrentScale = m_vPreScale;
			}
		}

		_tchar szComName[MAX_PATH] = TEXT("");
		const wstring& strMonsterComName = TEXT("Com_Model_Monster%d");
		const wstring& strPlayerComName = TEXT("Com_Model_Player%d");
		if (m_eType == TYPE_MONSTER)
		{
			wsprintf(szComName, strMonsterComName.c_str(), m_iCurrentModelIndex);
		}
		else if (m_eType == TYPE_PLAYER)
		{ 
			wsprintf(szComName, strPlayerComName.c_str(), m_iCurrentModelIndex);
		}
		wstring strFinalComName = szComName;
		CModel* pCurrentModel = (CModel*)m_pPlayer->Find_Component(strFinalComName);
		if (pCurrentModel != nullptr)
		{
			_uint iNumAnimations = pCurrentModel->Get_NumAnim();
			vector<CAnimation*> pAnimations = pCurrentModel->Get_Animations();

			m_AnimationNames.clear();

			auto iter = pAnimations.begin();
			for (_uint i = 0; i < iNumAnimations; i++)
			{
				m_AnimationNames.push_back((*iter)->Get_Name());
				++iter;
			}
			static int iCurrentAnimation = 0;
			if (m_AnimationNames.size() != 0)
			{
				iCurrentAnimation = pCurrentModel->Get_CurrentAnimationIndex();
				if (ImGui::ListBox("ANIMATION", &iCurrentAnimation, m_AnimationNames.data(), m_AnimationNames.size()))
				{
					m_AnimDesc.iAnimIndex = iCurrentAnimation;
					m_AnimDesc.bSkipInterpolation = true;
					pCurrentModel->Set_Animation(m_AnimDesc);
				}
			}

			_int iCurrentAnimPos = (_int)pCurrentModel->Get_CurrentAnimPos();
			iter = pAnimations.begin();
			for (_uint i = 0; i < iCurrentAnimation; i++)
			{
				++iter;
			}
			
			if (ImGui::SliderInt("ANIMPOS", &iCurrentAnimPos, 0.f, (_int)(*iter)->Get_Duration()))
			{
				(*iter)->Set_CurrentAnimPos((_float)iCurrentAnimPos);
			}

			if (ImGui::InputInt("ANIMP0S", &iCurrentAnimPos, 1))
			{
				if (iCurrentAnimPos > (_int)(*iter)->Get_Duration())
				{
					iCurrentAnimPos = (_int)(*iter)->Get_Duration();
				}
				else if (iCurrentAnimPos < 0)
				{
					iCurrentAnimPos = 0;
				}
				(*iter)->Set_CurrentAnimPos((_float)iCurrentAnimPos);
			}

			if (ImGui::Button("ADD TRIGGER"))
			{
				(*iter)->Add_Trigger((*iter)->Get_CurrentAnimPos());
			}
			
			vector<const _char*> TriggerTimes;
			vector<_float> Triggers = (*iter)->Get_Triggers();
			_tchar szTriggerTime[MAX_PATH] = TEXT("");
			const wstring& strTriggerTime = TEXT("%d");
			auto Trigger = Triggers.begin();
			_char* szTrigger = { nullptr };
			for (size_t i = 0; i < (*iter)->Get_NumTrigger(); i++)
			{
				szTrigger = new _char;
				wsprintf(szTriggerTime, strTriggerTime.c_str(), (_int)(*Trigger));
				WideCharToMultiByte(CP_ACP, 0, szTriggerTime, (_int)lstrlen(szTriggerTime), szTrigger, MAX_PATH, nullptr, nullptr);
				TriggerTimes.push_back(szTrigger);
				++Trigger;
			}
			ImGui::SameLine();
			static int iTrigger = 0;
			if (ImGui::ListBox("TRIGGER", &iTrigger, TriggerTimes.data(), TriggerTimes.size()))
			{
			}
		}

		ImGui::End();
	}

	if (m_pPlayer)
	{
		m_pPlayer->Set_ModelType((CPlayer::TYPE)m_eType);
		m_pPlayer->Set_ModelIndex(m_iCurrentModelIndex);
	}

	return S_OK;
}

HRESULT CImgui_Manager::ImGuizmoMenu()
{
	_mat ViewMatrix = m_pGameInstance->Get_Transform(TransformType::View);
	_mat ProjMatrix = m_pGameInstance->Get_Transform(TransformType::Proj);

	if (m_pPlayer)
	{
		CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
		if (pTargetTransform != nullptr)
		{
			_mat TargetMatrix = pTargetTransform->Get_World_Matrix();
			_mat PreMatrix = pTargetTransform->Get_World_Matrix();
			ImGuizmo::Manipulate(&ViewMatrix.m[0][0], &ProjMatrix.m[0][0], m_eStateType, ImGuizmo::MODE::WORLD, &TargetMatrix.m[0][0]);
			if (ImGuizmo::IsUsing())
			{
				if (m_eStateType == ImGuizmo::OPERATION::SCALE)
				{
					m_vCurrentScale = _vec3(TargetMatrix.Right().Length(),
						TargetMatrix.Up().Length(),
						TargetMatrix.Backward().Length());

					_vec4 Right = PreMatrix.Right();
					Right.Normalize();
					_vec4 Up = PreMatrix.Up();
					Up.Normalize();
					_vec4 Look = PreMatrix.Look();
					Look.Normalize();

					pTargetTransform->Set_State(State::Right, Right * m_vCurrentScale.x);
					pTargetTransform->Set_State(State::Up, Up * m_vCurrentScale.y);
					pTargetTransform->Set_State(State::Look, Look * m_vCurrentScale.z);
				}
				else
				{
					pTargetTransform->Set_State(State::Right, TargetMatrix.Right());
					pTargetTransform->Set_State(State::Up, TargetMatrix.Up());
					pTargetTransform->Set_State(State::Look, TargetMatrix.Look());
					pTargetTransform->Set_State(State::Pos, TargetMatrix.Position());
				}
			}
		}
	}

	return S_OK;
}


HRESULT CImgui_Manager::SaveFile()
{
	_tchar szComName[MAX_PATH] = TEXT("");
	const wstring& strMonsterComName = TEXT("Com_Model_Monster%d");
	const wstring& strPlayerComName = TEXT("Com_Model_Player%d");
	if (m_eType == TYPE_MONSTER)
	{
		wsprintf(szComName, strMonsterComName.c_str(), m_iCurrentModelIndex);
	}
	else if (m_eType == TYPE_PLAYER)
	{
		wsprintf(szComName, strPlayerComName.c_str(), m_iCurrentModelIndex);
	}
	wstring strFinalComName = szComName;
	CModel* pCurrentModel = (CModel*)m_pPlayer->Find_Component(strFinalComName);

	_char szFilePath[MAX_PATH] = "";
	_char szDirectory[MAX_PATH] = "";
	_char szFileName[MAX_PATH] = "";
	_splitpath_s(pCurrentModel->Get_FilePath(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	_char szExt[MAX_PATH] = ".animtrigger";
	strcpy_s(szFilePath, MAX_PATH, szDirectory);
	strcat_s(szFilePath, MAX_PATH, szFileName);
	strcat_s(szFilePath, MAX_PATH, szExt);

	ofstream Fileout(szFilePath, ios::binary);

	if (Fileout.is_open())
	{
		vector<CAnimation*> pAnimations = pCurrentModel->Get_Animations();
		auto iter = pAnimations.begin();
		for (_uint i = 0; i < pCurrentModel->Get_NumAnim(); i++)
		{
			if ((*iter)->Get_NumTrigger() != 0)
			{
				_uint iAnimIndex = i;
				Fileout.write(reinterpret_cast<char*>(&iAnimIndex), sizeof _uint);
				_uint iNumTrigger = (*iter)->Get_NumTrigger();
				Fileout.write(reinterpret_cast<char*>(&iNumTrigger), sizeof _uint);
				vector<_float> Triggers = (*iter)->Get_Triggers();
				auto Triggeriter = Triggers.begin();
				for (_uint i = 0; i < (*iter)->Get_NumTrigger(); i++)
				{
					_float fTrigger = (*Triggeriter);
					Fileout.write(reinterpret_cast<char*>(&fTrigger), sizeof _float);
					++Triggeriter;
				}
			}
			++iter;
		}

		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
	}

	return S_OK;
}

HRESULT CImgui_Manager::LoadFile()
{
	_tchar szComName[MAX_PATH] = TEXT("");
	const wstring& strMonsterComName = TEXT("Com_Model_Monster%d");
	const wstring& strPlayerComName = TEXT("Com_Model_Player%d");
	if (m_eType == TYPE_MONSTER)
	{
		wsprintf(szComName, strMonsterComName.c_str(), m_iCurrentModelIndex);
	}
	else if (m_eType == TYPE_PLAYER)
	{
		wsprintf(szComName, strPlayerComName.c_str(), m_iCurrentModelIndex);
	}
	wstring strFinalComName = szComName;
	CModel* pCurrentModel = (CModel*)m_pPlayer->Find_Component(strFinalComName);

	_char szFilePath[MAX_PATH] = "";
	_char szDirectory[MAX_PATH] = "";
	_char szFileName[MAX_PATH] = "";
	_splitpath_s(pCurrentModel->Get_FilePath(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	_char szExt[MAX_PATH] = ".animtrigger";
	strcpy_s(szFilePath, MAX_PATH, szDirectory);
	strcat_s(szFilePath, MAX_PATH, szFileName);
	strcat_s(szFilePath, MAX_PATH, szExt);

	ifstream Filein(szFilePath, ios::binary);

	if (Filein.is_open())
	{
		vector<CAnimation*> pAnimations = pCurrentModel->Get_Animations();
		auto iter = pAnimations.begin();

		for (_uint i = 0; i < pCurrentModel->Get_NumAnim(); i++)
		{
			(*iter)->Reset_Trigger();
			++iter;
		}

		iter = pAnimations.begin();
		_uint iAnimIndex = { 0 };
		Filein.read(reinterpret_cast<char*>(&iAnimIndex), sizeof _uint);
		for (_uint i = 0; i < iAnimIndex; i++)
		{
			++iter;
		}
		_uint iNumTrigger = { 0 };
		Filein.read(reinterpret_cast<char*>(&iNumTrigger), sizeof _uint);
		for (_uint i = 0; i < iNumTrigger; i++)
		{
			_float fTrigger = { 0.f };
			Filein.read(reinterpret_cast<char*>(&fTrigger), sizeof _float);
			(*iter)->Add_Trigger(fTrigger);
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
	}

	return S_OK;
}

CImgui_Manager* CImgui_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const GRAPHIC_DESC& GraphicDesc)
{
	CImgui_Manager* pInstance = new CImgui_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(GraphicDesc)))
	{
		MSG_BOX("Failed to Created : CImgui_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CImgui_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pPlayer);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
