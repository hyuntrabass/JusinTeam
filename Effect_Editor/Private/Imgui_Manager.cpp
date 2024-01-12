#include "Imgui_Manager.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

using namespace ImGui;

CImgui_Manager::CImgui_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImgui_Manager::Init(_dev pDevice, _context pContext, vector<string>* pTextureList)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);

	IMGUI_CHECKVERSION();
	CreateContext();

	StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	m_iNumTextures = static_cast<_int>(pTextureList->size());
	m_pItemList_Texture = new const _char * [m_iNumTextures];
	m_pTextures = new CTexture * [m_iNumTextures];

	for (_int i = 0; i < m_iNumTextures; i++)
	{
		m_pItemList_Texture[i] = (*pTextureList)[i].c_str();
	}
	
	if (FAILED(Ready_Layers()))
	{
		return E_FAIL;
	}

	Load_Data();

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	static _bool bDemo{ true };
	ShowDemoWindow(&bDemo);

	Begin("Effect_Tool");

	const _char* szType[2]{ "Particle", "Rect" };

	if (Combo("Type", &m_iCurrect_Type, szType, IM_ARRAYSIZE(szType)))
	{

	}

	ListBox("Texture", &m_iSelected_Texture, m_pItemList_Texture, m_iNumTextures); SameLine();
	Image(reinterpret_cast<void*>(m_pTextures[m_iSelected_Texture]->Get_SRV()), ImVec2(128.f, 128.f));

	End();
}

HRESULT CImgui_Manager::Render()
{
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(GetDrawData());

	return S_OK;
}

HRESULT CImgui_Manager::Ready_Layers()
{
	//m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect", &m_pVIBuffer_Rect);
	//m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Instancing_Point", &m_pVIBuffer_Instancing);

	for (_int i = 0; i < m_iNumTextures; i++)
	{
		wstring strPrototypeTag = L"Prototype_Component_Texture_Effect_" + to_wstring(i);
		m_pTextures[i] = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, strPrototypeTag));
	}

	return S_OK;
}

HRESULT CImgui_Manager::Load_Data()
{
	return S_OK;
}

HRESULT CImgui_Manager::Export_Data()
{
	return S_OK;
}

void CImgui_Manager::Free()
{
	Safe_Delete_Array(m_pItemList_Texture);
	Safe_Delete_Array(m_pTextures);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();
}
