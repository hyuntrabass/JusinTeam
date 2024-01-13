#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "ImGui_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "GameObject.h"
#include "Layer.h"

#include "GameObject.h"

#include "Objects.h"
#include "Monster.h"
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

	if (m_isCreate == true && m_eItemType != ItemType::Map)
	{
		if (m_pSelectedDummy)
		{
			CTransform* pTransform = (CTransform*)m_pSelectedDummy->Find_Component(TEXT("Com_Transform"));
			pTransform->Set_State(State::Pos, m_pTerrainPos);
			m_pSelectedDummy->Select(true);
			//if ( m_pGameInstance->Mouse_Down(DIM_LBUTTON))
			//{
			//	pTransform->Set_State(State::Pos, m_pTerrainPos);
			//	m_DummyList.emplace(m_pSelectedDummy->Get_ID(), m_pSelectedDummy);
			//	m_pSelectedDummy = nullptr;
			//}
		}
	}

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
	{
		FastPicking();
		if (m_pSelectedDummy)
		{
		//	m_pSelectedDummy->Select(false);
			m_isCreate = false;
		}

	}
	if (m_pGameInstance->Mouse_Down(DIM_RBUTTON))
	{
		if (m_pSelectedDummy)
		{
			m_pSelectedDummy->Select(false);
			m_pSelectedDummy = nullptr;
		}

	}
	
	if (m_iSelectIdx == -1)
	{
		if ((m_vMousePos.x >= 0.f && m_vMousePos.x < m_iWinSizeX) && (m_vMousePos.y >= 0.f && m_vMousePos.y < m_iWinSizeY))
		{
			m_pTerrainPos = m_pGameInstance->PickingDepth(m_vMousePos.x, m_vMousePos.y);
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
		if (ImGui::BeginTabItem("MAP"))
		{
			ImGui::SeparatorText("LIST");

			static int Map_current_idx = 0; // Here we store our selection data as an index.
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

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				if (m_iSelectIdx != -1)
				{
					m_eItemType = ItemType::Map;
					Create_Dummy(Map_current_idx);
					m_iSelectIdx = -1;
				}

				ImGui::EndListBox();
			}
			ImGui::EndTabItem();

		}
		if (ImGui::BeginTabItem("Objects"))
		{
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

			if (m_iSelectIdx != -1)
			{
				m_eItemType = ItemType::Objects;
				Create_Dummy(Object_current_idx);
				m_iSelectIdx = -1;
			}

			ImGui::SeparatorText("MATRIX : ");
			ImGui::InputFloat4("Right", &m_ObjectMatrix.m[0][0], 0);
			ImGui::InputFloat4("Up", &m_ObjectMatrix.m[1][0], 0);
			ImGui::InputFloat4("Look", &m_ObjectMatrix.m[2][0], 0);
			ImGui::InputFloat4("Position", &m_ObjectMatrix.m[3][0], 0);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Monster"))
		{
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
			if (m_iSelectIdx != -1)
			{
				m_eItemType = ItemType::Monster;
				Create_Dummy(Monster_current_idx);
				m_iSelectIdx = -1;
			}

			ImGui::SeparatorText("MATRIX : ");
			ImGui::InputFloat4("Right", &m_ObjectMatrix.m[0][0], 0);
			ImGui::InputFloat4("Up", &m_ObjectMatrix.m[1][0], 0);
			ImGui::InputFloat4("Look", &m_ObjectMatrix.m[2][0], 0);
			ImGui::InputFloat4("Position", &m_ObjectMatrix.m[3][0], 0);


			if (ImGui::Button("Delete"))
			{
				Delete_Dummy();
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
	ImGui::Separator();

	
	if (ImGui::Button("SAVE"))
	{
		//SaveFile();
	}
	ImGui::SameLine();
	if (ImGui::Button("LOAD"))
	{
		//LoadFile();
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

	if (m_eItemType != ItemType::Map)
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
		{
			MSG_BOX("Failed to Add Layer : Dummy");
		}
	
		m_DummyList.emplace(m_pSelectedDummy->Get_ID(), m_pSelectedDummy);
	}
	else
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"), &Info)))
		{
			MSG_BOX("Failed to Add Layer : Dummy");
		}
	}

}

void CImGui_Manager::Delete_Dummy()
{
	if (m_pSelectedDummy)
	{
		if (!m_DummyList.empty())
			m_DummyList.erase(DummyIndex);
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

	string strMonsterFilePath = "../Bin/Resources/AnimMesh/Player/Mesh/";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(strMonsterFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
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
	if (m_pSelectedDummy)
		m_pSelectedDummy->Select(false);
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
		m_DummyList.clear();
	}

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pSelectedDummy);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
