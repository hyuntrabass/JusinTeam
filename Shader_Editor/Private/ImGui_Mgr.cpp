#include "ImGui_Mgr.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CImGui_Mgr)

CImGui_Mgr::CImGui_Mgr()
{
}

HRESULT CImGui_Mgr::Init(_dev pDevice, _context pContext, CGameInstance* pGameInstance, CRenderer* pRenderer)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\gulim.ttc", 18.f, NULL, io.Fonts->GetGlyphRangesKorean());

	if (false == ImGui_ImplWin32_Init(g_hWnd))
		return E_FAIL;

	if (false == ImGui_ImplDX11_Init(pDevice, pContext))
		return E_FAIL;

	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

void CImGui_Mgr::Frame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiTabBarFlags TabBarFlag = ImGuiTabBarFlags_None;
}

void CImGui_Mgr::Editin(_float fTimeDelta)
{
}

void CImGui_Mgr::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

void CImGui_Mgr::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
