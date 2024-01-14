#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGui_Manager.h"
#include "GameInstance.h"

#include "Dummy.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{

	Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_Manager::Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc)
{
	m_hWnd = GraphicDesc.hWnd;
	m_iWinSizeX = GraphicDesc.iWinSizeX;
	m_iWinSizeY = GraphicDesc.iWinSizeY;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;

	Search_Files();

	return S_OK;
}

void CImGui_Manager::Tick(_float fTimeDelta)
{
	Mouse_Pos();

	//if (m_isCreate == true && m_eItemType != ItemType::Map)
	//{
	//	if (m_pSelectedDummy)
	//	{
	//		CTransform* pTransform = (CTransform*)m_pSelectedDummy->Find_Component(TEXT("Com_Transform"));
	//		pTransform->Set_State(State::Pos, m_pTerrainPos);
	//		m_pSelectedDummy->Select(true);
	//	}
	//}

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{

		if ((m_vMousePos.x >= 0.f && m_vMousePos.x < m_iWinSizeX) && (m_vMousePos.y >= 0.f && m_vMousePos.y < m_iWinSizeY))
		{
			m_pTerrainPos = m_pGameInstance->PickingDepth(m_vMousePos.x, m_vMousePos.y);
		}

		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Select(false);
			m_pSelectedDummy = nullptr;
			DummyIndex = 0;
		}
		FastPicking();
	}
	if (m_pGameInstance->Mouse_Down(DIM_RBUTTON))
	{
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Select(false);
			m_pSelectedDummy = nullptr;
			DummyIndex = 0;
		}

	}

}


HRESULT CImGui_Manager::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	bool bDemo = true;
	ImGui::ShowDemoWindow(&bDemo);

	ImGuiMenu();

	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
	ImGuizmo::SetRect(0, 0, (_float)(m_iWinSizeX), (_float)(m_iWinSizeY));

	ImGuizmoMenu();

	ImGui::Render();

	return S_OK;
}

_bool CImGui_Manager::ComputePickPos()
{
	return m_ComputePickPos;
}

_bool CImGui_Manager::ComputeSelection()
{
	return m_ComputeSelection;
}

void CImGui_Manager::SetPos(const _float4& vPos, CDummy* pDummy)
{
	_vector vCamPos = XMLoadFloat4(&m_pGameInstance->Get_CameraPos());
	_float fNewDist = XMVector4Length(vCamPos - XMLoadFloat4(&vPos)).m128_f32[0];
	if (m_fCamDist < 0 || m_fCamDist > fNewDist)
	{
		m_vPos.x = vPos.x;
		m_vPos.y = vPos.y;
		m_vPos.z = vPos.z;
		m_vPos.w = 1.f;

		m_fCamDist = fNewDist;
	}
}

void CImGui_Manager::Select(const _vec4& vPos, CDummy* pDummy)
{
	_vector vCamPos = XMLoadFloat4(&m_pGameInstance->Get_CameraPos());
	_float fNewDist = XMVector4Length(vCamPos - XMLoadFloat4(&vPos)).m128_f32[0];
	if (m_fCamDist < 0 || m_fCamDist > fNewDist)
	{
		m_fCamDist = fNewDist;
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Select(false);
		}
		m_pSelectedDummy = pDummy;
		m_pSelectedDummy->Select(true);

		m_pSelectedDummy->Get_State(m_vPos, m_vLook);
	}
}

void CImGui_Manager::SetDummy(CDummy* pDummy)
{
	//m_DummyList.push_back(pDummy);
}

HRESULT CImGui_Manager::ImGuiMenu()
{
#pragma endregion
	ImGui::Begin("MENU");

	ImGui::SeparatorText("MOUSE POS : ");
	ImGui::InputFloat2("Mouse Pos", &m_vMousePos.x, 0);

	ImGui::SeparatorText("MOUSE WORLDPOS : ");
	ImGui::InputFloat3("Mouse WorldPos", &m_vMouseWorld.x, 0);

	ImGui::SeparatorText("TERRAIN POS : ");
	ImGui::InputFloat3("Terrain Pos", &m_pTerrainPos.x, 0);

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{

		/* 맵 */
		if (ImGui::BeginTabItem("MAP"))
		{
			m_eItemType = ItemType::Map;

			ImGui::SeparatorText("LIST");

			static int Map_current_idx = 0;
			ImGui::Text("Map");
			if (ImGui::BeginListBox("MAPS FILE", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (int n = 0; n < Maps.size(); n++)
				{
					const bool is_selected = (Map_current_idx == n);
					if (ImGui::Selectable(Maps[n], is_selected))
					{
						Map_current_idx = n;
						m_iSelectIdx = Map_current_idx;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndListBox();
			}

			ImGui::SeparatorText("MATRIX : ");
			ImGui::InputFloat4("Right", &m_ObjectMatrix.m[0][0], 0);
			ImGui::InputFloat4("Up", &m_ObjectMatrix.m[1][0], 0);
			ImGui::InputFloat4("Look", &m_ObjectMatrix.m[2][0], 0);
			ImGui::InputFloat4("Position", &m_ObjectMatrix.m[3][0], 0);

			ImGui::Separator();
			if (ImGui::Button("Delete"))
			{
				Delete_Dummy();
			}

			ImGui::SameLine();

			if (ImGui::Button("Create"))
			{
				if (m_iSelectIdx != -1)
				{
					Create_Dummy(Map_current_idx);
				}
			}

			ImGui::EndTabItem();

		}
		if (ImGui::BeginTabItem("Objects"))
		{
			m_eItemType = ItemType::Objects;

			/* 오브젝트 */
			ImGui::SeparatorText("LIST");

			static int Object_current_idx = 0;
			ImGui::Text("Objects");
			if (ImGui::BeginListBox("OBJECTS FILE", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (int n = 0; n < Objects.size(); n++)
				{
					const bool is_selected = (Object_current_idx == n);
					if (ImGui::Selectable(Objects[n], is_selected))
					{
						Object_current_idx = n;
						m_iSelectIdx = Object_current_idx;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndListBox();
			}


			ImGui::SeparatorText("MATRIX : ");
			ImGui::InputFloat4("Right", &m_ObjectMatrix.m[0][0], 0);
			ImGui::InputFloat4("Up", &m_ObjectMatrix.m[1][0], 0);
			ImGui::InputFloat4("Look", &m_ObjectMatrix.m[2][0], 0);
			ImGui::InputFloat4("Position", &m_ObjectMatrix.m[3][0], 0);

			ImGui::Separator();
			if (ImGui::Button("Delete"))
			{
				Delete_Dummy();
			}

			ImGui::SameLine();

			if (ImGui::Button("Create"))
			{
				if (m_iSelectIdx != -1)
				{
					Create_Dummy(Object_current_idx);
				}
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Monster"))
		{
			/* 몬스터 */
			m_eItemType = ItemType::Monster;

			ImGui::SeparatorText("LIST");

			static int Monster_current_idx = 0;
			ImGui::Text("Monster");
			if (ImGui::BeginListBox("MONSTERS FILE", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (int n = 0; n < Monsters.size(); n++)
				{
					const bool is_selected = (Monster_current_idx == n);
					if (ImGui::Selectable(Monsters[n], is_selected))
					{
						Monster_current_idx = n;
						m_iSelectIdx = Monster_current_idx;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndListBox();
			}

			ImGui::SeparatorText("MATRIX : ");
			ImGui::InputFloat4("Right", &m_ObjectMatrix.m[0][0], 0);
			ImGui::InputFloat4("Up", &m_ObjectMatrix.m[1][0], 0);
			ImGui::InputFloat4("Look", &m_ObjectMatrix.m[2][0], 0);
			ImGui::InputFloat4("Position", &m_ObjectMatrix.m[3][0], 0);

			ImGui::Separator();
			if (ImGui::Button("Delete"))
			{
				Delete_Dummy();
			}

			ImGui::SameLine();

			if (ImGui::Button("Create"))
			{
				if (m_iSelectIdx != -1)
				{
					Create_Dummy(Monster_current_idx);
				}
			}

			ImGui::Separator();

			if (ImGui::Button("SAVE"))
			{
				Save_Monster();
			}
			ImGui::SameLine();
			if (ImGui::Button("LOAD"))
			{
				Load_Monster();
			}


			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Camera"))
		{
			ImGui::Text("카메라 작업중......");
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}



	ImGui::End();

	return S_OK;
}


HRESULT CImGui_Manager::ImGuizmoMenu()
{
	m_ViewMatrix = m_pGameInstance->Get_Transform(TransformType::View);
	m_ProjMatrix = m_pGameInstance->Get_Transform(TransformType::Proj);
	if (m_pSelectedDummy)
	{
		CTransform* pObjectsTransform = (CTransform*)m_pSelectedDummy->Find_Component(TEXT("Com_Transform"));
		m_ObjectMatrix = pObjectsTransform->Get_World_Matrix();
		ImGuizmo::Manipulate(&m_ViewMatrix.m[0][0], &m_ProjMatrix.m[0][0], ImGuizmo::OPERATION::SCALE, ImGuizmo::MODE::WORLD, &m_ObjectMatrix.m[0][0]);
		ImGuizmo::Manipulate(&m_ViewMatrix.m[0][0], &m_ProjMatrix.m[0][0], ImGuizmo::OPERATION::ROTATE, ImGuizmo::MODE::WORLD, &m_ObjectMatrix.m[0][0]);
		ImGuizmo::Manipulate(&m_ViewMatrix.m[0][0], &m_ProjMatrix.m[0][0], ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, &m_ObjectMatrix.m[0][0]);
		if (ImGuizmo::IsUsing() == true)
		{
			_vector ObjRight = { m_ObjectMatrix._11, m_ObjectMatrix._12, m_ObjectMatrix._13, m_ObjectMatrix._14 };
			_vector ObjUp = { m_ObjectMatrix._21, m_ObjectMatrix._22, m_ObjectMatrix._23, m_ObjectMatrix._24 };
			_vector ObjLook = { m_ObjectMatrix._31, m_ObjectMatrix._32, m_ObjectMatrix._33, m_ObjectMatrix._34 };
			_vector ObjPosition = { m_ObjectMatrix._41, m_ObjectMatrix._42, m_ObjectMatrix._43, m_ObjectMatrix._44 };
			pObjectsTransform->Set_State(State::Right, ObjRight);
			pObjectsTransform->Set_State(State::Up, ObjUp);
			pObjectsTransform->Set_State(State::Look, ObjLook);
			pObjectsTransform->Set_State(State::Pos, ObjPosition);
		}
	}
	
	return S_OK;
}

void CImGui_Manager::Create_Dummy(const _int& iListIndex)
{
	if (m_isCreate == false)
	
		m_isCreate = true;
	

	if (m_pSelectedDummy)
	{
		m_pSelectedDummy->Select(false);
		m_pSelectedDummy = nullptr;
	}

	DummyInfo Info{};

	Info.ppDummy = &m_pSelectedDummy;
	
	Info.vPos = m_pTerrainPos;
	XMStoreFloat4(&Info.vLook, XMVector4Normalize(XMLoadFloat4(&m_vLook)));
	Info.Prototype = L"Prototype_Model_";
	Info.eType = m_eItemType;
	_tchar strUnicode[MAX_PATH]{}; 
	switch (m_eItemType)
	{
	case ItemType::Map:
		MultiByteToWideChar(CP_ACP, 0, Maps[iListIndex], static_cast<int>(strlen(Maps[iListIndex])), strUnicode, static_cast<int>(strlen(Maps[iListIndex])));
		break;
	case ItemType::Objects:
		MultiByteToWideChar(CP_ACP, 0, Objects[iListIndex], static_cast<int>(strlen(Objects[iListIndex])), strUnicode, static_cast<int>(strlen(Objects[iListIndex])));
		break;
	case ItemType::Monster:
		MultiByteToWideChar(CP_ACP, 0, Monsters[iListIndex], static_cast<int>(strlen(Monsters[iListIndex])), strUnicode, static_cast<int>(strlen(Monsters[iListIndex])));
		break;
	}
	Info.Prototype += strUnicode;


	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
	{
		MSG_BOX("Failed to Add Layer : Dummy");
	}
	
	
	switch (m_eItemType)
	{
	case MapEditor::ItemType::Objects:
		m_ObjectsList.push_back(m_pSelectedDummy);
		break;
	case MapEditor::ItemType::Monster:
		m_MonsterList.push_back(m_pSelectedDummy);
		break;
	}
	m_DummyList.emplace(m_pSelectedDummy->Get_ID(), m_pSelectedDummy);
	m_pSelectedDummy = nullptr;
}

void CImGui_Manager::Delete_Dummy()
{


	if (m_pSelectedDummy)
	{
		if (!m_DummyList.empty())
			m_DummyList.erase(DummyIndex);

		if (m_eItemType == ItemType::Objects)
		{
	
			for (auto it = m_ObjectsList.begin(); it != m_ObjectsList.end(); it++)
			{
				if ((*it)->Get_Selected() == true)
				{
					m_ObjectsList.erase(it);
					break;
				}
			}

		}
		else if (m_eItemType == ItemType::Monster)
		{
			for (auto it = m_MonsterList.begin(); it != m_MonsterList.end(); it++)
			{
				if ((*it)->Get_Selected() == true)
				{
					m_MonsterList.erase(it);
					break;
				}
			}
		
		}
	
		m_pSelectedDummy->Set_Dead();
		m_pSelectedDummy = nullptr;

	}
	

}


void CImGui_Manager::Select(const _float4& vPos, CDummy* pDummy)
{
	_vector vCamPos = XMLoadFloat4(&m_pGameInstance->Get_CameraPos());
	_float fNewDist = XMVector4Length(vCamPos - XMLoadFloat4(&vPos)).m128_f32[0];
	if (m_fCamDist < 0 || m_fCamDist > fNewDist)
	{
		m_fCamDist = fNewDist;
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Select(false);
		}
		m_pSelectedDummy = pDummy;
		m_pSelectedDummy->Select(true);

		m_pSelectedDummy->Get_State(m_vPos, m_vLook);
	}
}


const char* CImGui_Manager::Search_Files()
{
	string strMapFilePath = "../Bin/Resources/StaticMesh/Common/Mesh/";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(strMapFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			char* cstr = new char[strName.length() + 1];
			strcpy_s(cstr, strName.length() + 1, strName.c_str());
			Maps.push_back(cstr);
		}
		
	}

	string strObjectsFilePath = "../Bin/Resources/StaticMesh/Common/Mesh/";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(strObjectsFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			char* cstr = new char[strName.length() + 1];
			strcpy_s(cstr, strName.length() + 1, strName.c_str());
			Objects.push_back(cstr);
		}

	}

	string strMonsterFilePath = "../Bin/Resources/AnimMesh/Monster/";

	for (const auto& entry : std::filesystem::directory_iterator(strMonsterFilePath))
	{
		if (entry.is_directory())
		{
			string strName = entry.path().filename().string();
			char* cstr = new char[strName.length() + 1];
			strcpy_s(cstr, strName.length() + 1, strName.c_str());
			Monsters.push_back(cstr);
		}
	}

	return nullptr;
}

void CImGui_Manager::Mouse_Pos()
{
	// 마우스 좌표를 월드 좌표로
	GetCursorPos(&m_ptMouse);
	ScreenToClient(m_hWnd, &m_ptMouse);
	m_vMousePos.x = (_float)m_ptMouse.x;
	m_vMousePos.y = (_float)m_ptMouse.y;

	_vec2 normalizedDeviceCoords;
	normalizedDeviceCoords.x = (2.0f * m_ptMouse.x) / m_iWinSizeX - 1.0f;
	normalizedDeviceCoords.y = 1.0f - (2.0f * m_ptMouse.y) / m_iWinSizeY;

	_vec4 clipCoords;
	clipCoords.x = normalizedDeviceCoords.x;
	clipCoords.y = normalizedDeviceCoords.y;
	clipCoords.z = 0.0f;
	clipCoords.w = 1.0f;

	_mat InversViewMat = m_pGameInstance->Get_Transform_Inversed(TransformType::View);
	_mat InversProjMat = m_pGameInstance->Get_Transform_Inversed(TransformType::Proj);

	_vec4 eyeCoords = XMVector3TransformCoord(XMLoadFloat4(&clipCoords), InversProjMat);
	eyeCoords = XMVector4Transform(XMLoadFloat4(&eyeCoords), InversViewMat);

	m_vMouseWorld = eyeCoords;
}

void CImGui_Manager::FastPicking()
{
	//if (m_pSelectedDummy)
	//{
	//	m_pSelectedDummy->Select(false);
	//	m_pSelectedDummy = nullptr;
	//}
	DummyIndex = 0;

	if ((m_vMousePos.x >= 0.f && m_vMousePos.x < m_iWinSizeX) && (m_vMousePos.y >= 0.f && m_vMousePos.y < m_iWinSizeY))
	{
		DummyIndex = m_pGameInstance->FastPicking(m_vMousePos.x , m_vMousePos.y);

		auto iter = m_DummyList.find(DummyIndex);
		if (iter != m_DummyList.end())
		{
			m_pSelectedDummy = (*iter).second;
			m_pSelectedDummy->Select(true);
		}


	}
	
}


HRESULT CImGui_Manager::Save_Monster()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"MonsterData.dat";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0dat 파일\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data";

	if (GetSaveFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (!outFile.is_open())
			return E_FAIL;

		_uint MonsterListSize = m_MonsterList.size();
		outFile.write(reinterpret_cast<const char*>(&MonsterListSize), sizeof(_uint));

		for (auto& monster : m_MonsterList)
		{
			CTransform* pMonsterTransform = dynamic_cast<CTransform*>(monster->Find_Component(TEXT("Com_Transform")));

			wstring MonsterPrototype = monster->Get_Info().Prototype;
			_ulong MonsterPrototypeSize = MonsterPrototype.size();

			outFile.write(reinterpret_cast<const char*>(&MonsterPrototypeSize), sizeof(_ulong));
			outFile.write(reinterpret_cast<const char*>(MonsterPrototype.c_str()), MonsterPrototypeSize * sizeof(wchar_t));

			_mat MonsterWorldMat = pMonsterTransform->Get_World_Matrix();
			outFile.write(reinterpret_cast<const char*>(&MonsterWorldMat), sizeof(_mat));
		}
		
		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
	}
	return S_OK;
}


HRESULT CImGui_Manager::Load_Monster()
{

	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.dat)\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = filePathName;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data\\";

	if (GetOpenFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MessageBox(g_hWnd, L"파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
			return E_FAIL;
		}

		_uint MonsterListSize;
		inFile.read(reinterpret_cast<char*>(&MonsterListSize), sizeof(_uint));


		for (_uint i = 0; i < MonsterListSize; ++i)
		{
			_ulong MonsterPrototypeSize;
			inFile.read(reinterpret_cast<char*>(&MonsterPrototypeSize), sizeof(_ulong));

			wstring MonsterPrototype;
			MonsterPrototype.resize(MonsterPrototypeSize);
			inFile.read(reinterpret_cast<char*>(&MonsterPrototype[0]), MonsterPrototypeSize * sizeof(wchar_t));

			_mat MonsterWorldMat;
			inFile.read(reinterpret_cast<char*>(&MonsterWorldMat), sizeof(_mat));

			DummyInfo MonsterInfo{};
			MonsterInfo.eType = ItemType::Monster;
			MonsterInfo.Prototype = MonsterPrototype;
			MonsterInfo.vLook = _float4(MonsterWorldMat._31, MonsterWorldMat._32, MonsterWorldMat._33, MonsterWorldMat._34);
			MonsterInfo.vPos = _float4(MonsterWorldMat._41, MonsterWorldMat._42, MonsterWorldMat._43, MonsterWorldMat._44);
			MonsterInfo.ppDummy = &m_pSelectedDummy;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &MonsterInfo)))
			{
				MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
				return E_FAIL;
			}

			CTransform* pMonsterTransform = dynamic_cast<CTransform*>(m_pSelectedDummy->Find_Component(TEXT("Com_Transform")));
			
			pMonsterTransform->Set_State(State::Right, MonsterWorldMat.Right());
			pMonsterTransform->Set_State(State::Up, MonsterWorldMat.Up());
			pMonsterTransform->Set_State(State::Look, MonsterWorldMat.Look());
			pMonsterTransform->Set_State(State::Pos, MonsterWorldMat.Position());
			
			
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
	}
	return S_OK;
}

CImGui_Manager* CImGui_Manager::Create(const GRAPHIC_DESC& GraphicDesc)
{
	CImGui_Manager* pInstance = new CImGui_Manager();

	if (FAILED(pInstance->Initialize_Prototype(GraphicDesc)))
	{
		MSG_BOX("Failed to Created : CImGui_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImGui_Manager::Free()
{
	for (auto& cstr : Maps)
	{
		Safe_Delete_Array(cstr);
	}
	Maps.clear();

	for (auto& cstr : Objects)
	{
		Safe_Delete_Array(cstr);
	}
	Objects.clear();

	for (auto& cstr : Monsters)
	{
		Safe_Delete_Array(cstr);
	}
	Monsters.clear();

	//if (!m_ObjectsList.empty())
	//{
	//	for (auto& cstr : m_ObjectsList)
	//	{
	//		Safe_Release(cstr);
	//	}
	//	m_ObjectsList.clear();
	//}

	//if (!m_MonsterList.empty())
	//{
	//	for (auto& cstr : m_MonsterList)
	//	{
	//		Safe_Release(cstr);
	//	}
	//	m_MonsterList.clear();

	//}

	if (!m_DummyList.empty())
	{
		for (auto& Pair : m_DummyList)
		{
			Safe_Release(Pair.second);
		}
	}
	m_DummyList.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pSelectedDummy);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
