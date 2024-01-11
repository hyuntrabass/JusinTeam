#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "ImGui_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "GameObject.h"
#include "Layer.h"

#include "Objects.h"
#include "Map.h"
#include "Monster.h"

CImGui_Manager::CImGui_Manager(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
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
	GetCursorPos(&m_ptMouse);
	ScreenToClient(m_hWnd, &m_ptMouse);
	m_vMousePos.x = (_float)m_ptMouse.x;
	m_vMousePos.y = (_float)m_ptMouse.y;

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
	{
		if (m_iSelectIdx == -1)
		{
			if ((m_vMousePos.x >= 0.f && m_vMousePos.x < m_iWinSizeX) && (m_vMousePos.y >= 0.f && m_vMousePos.y < m_iWinSizeY))
			{
				m_pTerrainPos = m_pGameInstance->PickingDepth(m_vMousePos.x, m_vMousePos.y);
			}
		}
	}

}


HRESULT CImGui_Manager::Render()
{
	bool bDemo = true;
	ImGui::ShowDemoWindow(&bDemo);

	ImGuiMenu();

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
	ImGuizmo::SetRect(0, 0, (_float)(m_iWinSizeX), (_float)(m_iWinSizeY));
	ImGuizmoMenu();

	ImGui::Render();
	
	return S_OK;
}

HRESULT CImGui_Manager::ImGuiMenu()
{
#pragma endregion
	ImGui::Begin("MENU");

	ImGui::SeparatorText("MOUSE POS : ");
	ImGui::InputFloat2("Mouse Pos", &m_vMousePos.x, 0);

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
				Create_Object(m_iSelectIdx);
				m_iSelectIdx = -1;
			}

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

	return S_OK;
}

void CImGui_Manager::Create_Object(_uint iIndex)
{

	return;
}

void CImGui_Manager::Select_Object()
{
	
	return;
}


const char* CImGui_Manager::Search_Files()
{
	string strMapFilePath = "../Bin/Resources/AnimMesh/Player/Mesh/";

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



CImGui_Manager* CImGui_Manager::Create(_dev pDevice, _context pContext, const GRAPHIC_DESC& GraphicDesc)
{
	CImGui_Manager* pInstance = new CImGui_Manager(pDevice, pContext);

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

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
