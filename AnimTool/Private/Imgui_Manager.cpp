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

	ImGui::SeparatorText("FILE");

	if (ImGui::Button("SAVE"))
	{
		//SaveFile();
	}
	ImGui::SameLine();
	if (ImGui::Button("LOAD"))
	{
		//LoadFile();
	}

	ImGui::SeparatorText("SELECT");

	const char* szModelName[2] = { "Loser02", "Sandman" };
	static const char* szCurrentModel = "Loser02";

	if (ImGui::BeginCombo("LIST", szCurrentModel))
	{
		for (size_t i = 0; i < IM_ARRAYSIZE(szModelName); i++)
		{
			_bool bSelectedModel = (szCurrentModel == szModelName[i]);
			if (ImGui::Selectable(szModelName[i], bSelectedModel))
			{
				szCurrentModel = szModelName[i];
				m_pPlayer->Set_ModelIndex(i);
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SeparatorText("SELECTED OBJECT");

#pragma region CreateObject

	if (ImGui::Button("CREATE"))
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

		ImGui::SeparatorText("ANIMATION");
		_uint iCurrentModelIndex = m_pPlayer->Get_ModelIndex();
		_tchar szComName[MAX_PATH] = TEXT("");
		const wstring& strComName = TEXT("Com_Model%d");
		wsprintf(szComName, strComName.c_str(), iCurrentModelIndex);
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
					pCurrentModel->Set_Animation(m_AnimDesc);
				}
			}

			_int iCurrentAnimPos = (_int)pCurrentModel->Get_CurrentAnimPos();
			iter = pAnimations.begin();
			for (_uint i = 0; i < iCurrentAnimation; i++)
			{
				++iter;
			}
			ImGui::SliderInt("ANIMPOS", &iCurrentAnimPos, 0.f, (_int)(*iter)->Get_Duration());
			(*iter)->Set_CurrentAnimPos((_float)iCurrentAnimPos);

			ImGui::InputInt("AnimPos", &iCurrentAnimPos, 1);
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

		ImGui::End();
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

//
//HRESULT CImgui_Manager::SaveFile()
//{
//	OPENFILENAME OpenSaveFile;
//	TCHAR FileName[MAX_PATH] = TEXT("Lumia.dat");
//	static TCHAR FileFilter[] = TEXT("");
//	
//	memset(&OpenSaveFile, 0, sizeof(OPENFILENAME));
//	OpenSaveFile.lStructSize = sizeof(OPENFILENAME);
//	OpenSaveFile.hwndOwner = g_hWnd;
//	OpenSaveFile.lpstrFilter = FileFilter;
//	OpenSaveFile.lpstrFile = FileName;
//	OpenSaveFile.nMaxFile = 256;
//	OpenSaveFile.lpstrInitialDir = TEXT("..\\Bin\\Resources\\Models\\Map\\Lumia\\");
//
//	if (GetSaveFileName(&OpenSaveFile) != 0)
//	{
//		const TCHAR* pGetPath = OpenSaveFile.lpstrFile;
//
//		HANDLE hFile = CreateFile(pGetPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
//
//		if (INVALID_HANDLE_VALUE == hFile)
//			return E_FAIL;
//
//		DWORD dwByte = 0;
//		TOOLOBJECT_DESC ObjectDesc = {};
//		_uint iZero = { 0 };
//
//		CLayer* pRoofLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Roof"));
//		if (pRoofLayer != nullptr)
//		{
//			_uint iNumRoofs = pRoofLayer->Get_ListSize();
//			list<CGameObject*>* pRoofs = pRoofLayer->Get_ObjectList(); 
//
//			if (iNumRoofs != 0)
//			{
//				WriteFile(hFile, &iNumRoofs, sizeof _uint, &dwByte, nullptr);
//				for (auto iter = (*pRoofs).begin(); iter != (*pRoofs).end(); ++iter)
//				{
//					CRoof* pRoof = Cast<CRoof*>(*iter);
//					ObjectDesc = pRoof->Get_ObjectDesc();
//					CTransform* pRoofTransform = (CTransform*)pRoof->Find_Component(TEXT("Component_Transform"));
//
//					WriteFile(hFile, reinterpret_cast<char*>(&pRoofTransform->Get_WorldFloat4x4().m[0]), sizeof _float4x4, &dwByte, nullptr);
//
//					_char szModelName[MAX_PATH] = "";
//					WideCharToMultiByte(CP_ACP, 0, ObjectDesc.strModelName.c_str(), (_int)lstrlen(ObjectDesc.strModelName.c_str()), szModelName, MAX_PATH, nullptr, nullptr);
//					WriteFile(hFile, reinterpret_cast<char*>(&szModelName), sizeof _char * MAX_PATH, &dwByte, nullptr);
//				}
//			}
//		}
//		else 
//		{
//			WriteFile(hFile, &iZero, sizeof _uint, &dwByte, nullptr);
//		}
//
//		CLayer* pLIDailinLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_LIDailin"));
//		if (pLIDailinLayer != nullptr)
//		{
//			_uint iNumLIDailins = pLIDailinLayer->Get_ListSize();
//			list<CGameObject*>* pLIDailins = pLIDailinLayer->Get_ObjectList();
//			if (iNumLIDailins != 0)
//			{
//				ObjectDesc = {};
//				WriteFile(hFile, &iNumLIDailins, sizeof _uint, &dwByte, nullptr);
//				for (auto iter = (*pLIDailins).begin(); iter != (*pLIDailins).end(); ++iter)
//				{
//					CLIDailin* pLIDailin = Cast<CLIDailin*>(*iter);
//					ObjectDesc = pLIDailin->Get_ObjectDesc();
//					CTransform* pLIDailinTransform = (CTransform*)pLIDailin->Find_Component(TEXT("Component_Transform"));
//
//					WriteFile(hFile, reinterpret_cast<char*>(&pLIDailinTransform->Get_WorldFloat4x4().m[0]), sizeof _float4x4, &dwByte, nullptr);
//				}
//			}
//		}
//		else
//		{
//			WriteFile(hFile, &iZero, sizeof _uint, &dwByte, nullptr);
//		}
//
//		CLayer* pMonster= m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Monster"));
//		if (pMonster != nullptr)
//		{
//			_uint iNumMonsters = pMonster->Get_ListSize();
//			list<CGameObject*>* pMonsters = pMonster->Get_ObjectList();
//			if (iNumMonsters != 0)
//			{
//				ObjectDesc = {};
//				WriteFile(hFile, &iNumMonsters, sizeof _uint, &dwByte, nullptr);
//				for (auto iter = (*pMonsters).begin(); iter != (*pMonsters).end(); ++iter)
//				{
//					CMonster* pMonster = Cast<CMonster*>(*iter);
//					ObjectDesc = pMonster->Get_ObjectDesc();
//					CTransform* pMonsterTransform = (CTransform*)pMonster->Find_Component(TEXT("Component_Transform"));
//
//					WriteFile(hFile, reinterpret_cast<char*>(&pMonsterTransform->Get_WorldFloat4x4().m[0]), sizeof _float4x4, &dwByte, nullptr);
//					_char szModelName[MAX_PATH] = "";
//					WideCharToMultiByte(CP_ACP, 0, ObjectDesc.strModelName.c_str(), (_int)lstrlen(ObjectDesc.strModelName.c_str()), szModelName, MAX_PATH, nullptr, nullptr);
//					WriteFile(hFile, reinterpret_cast<char*>(&szModelName), sizeof _char * MAX_PATH, &dwByte, nullptr);
//					_char szPrototypeTag[MAX_PATH] = "";
//					WideCharToMultiByte(CP_ACP, 0, ObjectDesc.strPrototypeTag.c_str(), (_int)lstrlen(ObjectDesc.strPrototypeTag.c_str()), szPrototypeTag, MAX_PATH, nullptr, nullptr);
//					WriteFile(hFile, reinterpret_cast<char*>(&szPrototypeTag), sizeof _char * MAX_PATH, &dwByte, nullptr);
//				}
//			}
//		}
//		else
//		{
//			WriteFile(hFile, &iZero, sizeof _uint, &dwByte, nullptr);
//		}
//		
//		CLayer* pWickline = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Wickline"));
//		if (pWickline != nullptr)
//		{
//			_uint iNumWicklines = pWickline->Get_ListSize();
//			list<CGameObject*>* pWicklines = pWickline->Get_ObjectList();
//			if (iNumWicklines != 0)
//			{
//				ObjectDesc = {};
//				WriteFile(hFile, &iNumWicklines, sizeof _uint, &dwByte, nullptr);
//				for (auto iter = (*pWicklines).begin(); iter != (*pWicklines).end(); ++iter)
//				{
//					CWickline* pWickline = Cast<CWickline*>(*iter);
//					ObjectDesc = pWickline->Get_ObjectDesc();
//					CTransform* pWicklineTransform = (CTransform*)pWickline->Find_Component(TEXT("Component_Transform"));
//
//					WriteFile(hFile, reinterpret_cast<char*>(&pWicklineTransform->Get_WorldFloat4x4().m[0]), sizeof _float4x4, &dwByte, nullptr);
//				}
//			}
//		}
//		else
//		{
//			WriteFile(hFile, &iZero, sizeof _uint, &dwByte, nullptr);
//		}
//
//		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
//		CloseHandle(hFile);
//	}
//
//	return S_OK;
//}
//
//HRESULT CImgui_Manager::LoadFile()
//{
//	OPENFILENAME OpenLoadFile;
//	TCHAR FileName[MAX_PATH] = TEXT("Lumia.dat");
//	static TCHAR FileFilter[] = TEXT("");
//
//	memset(&OpenLoadFile, 0, sizeof(OPENFILENAME));
//	OpenLoadFile.lStructSize = sizeof(OPENFILENAME);
//	OpenLoadFile.hwndOwner = g_hWnd;
//	OpenLoadFile.lpstrFilter = FileFilter;
//	OpenLoadFile.lpstrFile = FileName;
//	OpenLoadFile.nMaxFile = 256;
//	OpenLoadFile.lpstrInitialDir = TEXT("..\\Bin\\Resources\\Models\\Map\\Lumia\\");
//
//	if (GetOpenFileName(&OpenLoadFile) != 0) {
//
//		const TCHAR* pGetPath = OpenLoadFile.lpstrFile;
//
//		HANDLE hFile = CreateFile(pGetPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//
//		if (INVALID_HANDLE_VALUE == hFile)
//			return E_FAIL;
//
//		DWORD dwByte = 0;
//		TOOLOBJECT_DESC ObjectDesc = {};
//
//		_uint iNumRoofs = 0;
//		ReadFile(hFile, &iNumRoofs, sizeof _uint, &dwByte, nullptr);
//		for (_uint i = 0; i < iNumRoofs; ++i)
//		{
//			ReadFile(hFile, reinterpret_cast<char*>(&ObjectDesc.WorldMatrix.m[0]), sizeof _float4x4, &dwByte, nullptr);
//
//			_char szModelName[MAX_PATH] = "";
//			ReadFile(hFile, reinterpret_cast<char*>(&szModelName), sizeof _char * MAX_PATH, &dwByte, nullptr);
//			_tchar szWideName[MAX_PATH] = TEXT("");
//			MultiByteToWideChar(CP_ACP, 0, szModelName, (_int)strlen(szModelName), szWideName, MAX_PATH);
//			ObjectDesc.strModelName = szWideName;
//
//			if (FAILED(m_pGameInstance->Add_Layers(LEVEL_TOOL, TEXT("Prototype_Roof"), TEXT("Layer_Roof"), &ObjectDesc)))
//			{
//				MessageBox(g_hWnd, L"파일 로드 실패(지붕)", L"파일 로드", MB_OK);
//				CloseHandle(hFile);
//				return E_FAIL;
//			}
//		}
//
//		_uint iNumLIDailins = 0;
//		ReadFile(hFile, &iNumLIDailins, sizeof _uint, &dwByte, nullptr);
//		for (_uint i = 0; i < iNumLIDailins; ++i)
//		{
//			ObjectDesc = {};
//			ReadFile(hFile, reinterpret_cast<char*>(&ObjectDesc.WorldMatrix.m[0]), sizeof _float4x4, &dwByte, nullptr);
//
//			if (FAILED(m_pGameInstance->Add_Layers(LEVEL_TOOL, TEXT("Prototype_LIDailin"), TEXT("Layer_LIDailin"), &ObjectDesc)))
//			{
//				MessageBox(g_hWnd, L"파일 로드 실패(리다이린)", L"파일 로드", MB_OK);
//				CloseHandle(hFile);
//				return E_FAIL;
//			}
//		}
//
//		_uint iNumMonsters = 0;
//		ReadFile(hFile, &iNumMonsters, sizeof _uint, &dwByte, nullptr);
//		for (_uint i = 0; i < iNumMonsters; ++i)
//		{
//			ObjectDesc = {};
//			ReadFile(hFile, reinterpret_cast<char*>(&ObjectDesc.WorldMatrix.m[0]), sizeof _float4x4, &dwByte, nullptr);
//
//			_char szModelName[MAX_PATH] = "";
//			ReadFile(hFile, reinterpret_cast<char*>(&szModelName), sizeof _char * MAX_PATH, &dwByte, nullptr);
//			_tchar szWideModelName[MAX_PATH] = TEXT("");
//			MultiByteToWideChar(CP_ACP, 0, szModelName, (_int)strlen(szModelName), szWideModelName, MAX_PATH);
//			ObjectDesc.strModelName = szWideModelName;
//
//			_char szPrototypeTag[MAX_PATH] = "";
//			ReadFile(hFile, reinterpret_cast<char*>(&szPrototypeTag), sizeof _char * MAX_PATH, &dwByte, nullptr);
//			_tchar szWidePrototypeTag[MAX_PATH] = TEXT("");
//			MultiByteToWideChar(CP_ACP, 0, szPrototypeTag, (_int)strlen(szPrototypeTag), szWidePrototypeTag, MAX_PATH);
//			ObjectDesc.strPrototypeTag = szWidePrototypeTag;
//
//			if (FAILED(m_pGameInstance->Add_Layers(LEVEL_TOOL, TEXT("Prototype_Monster"), TEXT("Layer_Monster"), &ObjectDesc)))
//			{
//				MessageBox(g_hWnd, L"파일 로드 실패(몬스터)", L"파일 로드", MB_OK);
//				CloseHandle(hFile);
//				return E_FAIL;
//			}
//		}
//
//		_uint iNumWicklines = 0;
//		ReadFile(hFile, &iNumWicklines, sizeof _uint, &dwByte, nullptr);
//		for (_uint i = 0; i < iNumWicklines; ++i)
//		{
//			ObjectDesc = {};
//			ReadFile(hFile, reinterpret_cast<char*>(&ObjectDesc.WorldMatrix.m[0]), sizeof _float4x4, &dwByte, nullptr);
//
//			if (FAILED(m_pGameInstance->Add_Layers(LEVEL_TOOL, TEXT("Prototype_Wickline"), TEXT("Layer_Wickline"), &ObjectDesc)))
//			{
//				MessageBox(g_hWnd, L"파일 로드 실패(위클라인)", L"파일 로드", MB_OK);
//				CloseHandle(hFile);
//				return E_FAIL;
//			}
//		}
//
//		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
//		CloseHandle(hFile);
//	}
//
//	return S_OK;
//}

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
