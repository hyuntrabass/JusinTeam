#include "Imgui_Manager.h"
#include "Camera.h"
#include "Effect_Dummy.h"

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

	StyleColorsClassic();
	//StyleColorsDark();

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

	m_ParticleInfo.vMinPos = _float3(0.f, 0.f, 0.f);
	m_ParticleInfo.vMaxPos = _float3(0.f, 0.f, 0.f);
	m_ParticleInfo.vScaleRange = _float2(1.f, 1.f);
	m_ParticleInfo.vSpeedRange = _float2(1.f, 1.f);
	m_ParticleInfo.vLifeTime = _float2(1.f, 1.f);
	m_ParticleInfo.vMinDir = _float3(0.f, 1.f, 0.f);
	m_ParticleInfo.vMaxDir = _float3(0.f, 1.f, 0.f);
	m_ParticleInfo.isLoop = true;

	Load_Data();

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	static _bool bDemo{ true };
	ShowDemoWindow(&bDemo);
	EffectInfo Info{};
	Begin("Effect_Tool");

	const _char* szType[2]{ "Particle", "Rect" };

	if (Combo("Type", &m_iCurrent_Type, szType, IM_ARRAYSIZE(szType)))
	{

	}

	InputFloat("Effect Life", &m_fEffectLifeTime);

	static _int iIsColor{};

	SeparatorText("Color");
	RadioButton("Texture", &iIsColor, 0); SameLine();
	RadioButton("Color", &iIsColor, 1);
	Checkbox("Mask", &m_hasMask);

	if (iIsColor == 0)
	{
		ListBox("Texture", &m_iSelected_Texture, m_pItemList_Texture, m_iNumTextures);
		Image(reinterpret_cast<void*>(m_pTextures[m_iSelected_Texture]->Get_SRV()), ImVec2(128.f, 128.f));
		m_hasDiffTexture = true;
	}
	else
	{
		ColorPicker4("Color", reinterpret_cast<_float*>(&m_vColor), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB);
		m_hasDiffTexture = false;
	}

	if (m_hasMask)
	{
		SeparatorText("Mask Texture");
		ListBox("Mask Texture", &m_iSelected_MaskTexture, m_pItemList_Texture, m_iNumTextures);
		Image(reinterpret_cast<void*>(m_pTextures[m_iSelected_MaskTexture]->Get_SRV()), ImVec2(128.f, 128.f));
	}

	if (m_iCurrent_Type == ET_PARTICLE)
	{
		SeparatorText("Particle Information");
		Info.eType = EffectType::Particle;
		static _bool isLoop{ true };
		Checkbox("Loop", &isLoop);

		InputInt("Instance Number", &m_iNumInstance);
		m_iNumInstance = clamp(m_iNumInstance, 0, 100);

		InputFloat3("Min Pos", reinterpret_cast<_float*>(&m_ParticleInfo.vMinPos));
		InputFloat3("Max Pos", reinterpret_cast<_float*>(&m_ParticleInfo.vMaxPos)); SameLine(); if (Button("Same with Min")) { m_ParticleInfo.vMaxPos = m_ParticleInfo.vMinPos; }
		if (m_ParticleInfo.vMinPos.x > m_ParticleInfo.vMaxPos.x)
		{
			m_ParticleInfo.vMinPos.x = m_ParticleInfo.vMaxPos.x;
		}
		if (m_ParticleInfo.vMinPos.y > m_ParticleInfo.vMaxPos.y)
		{
			m_ParticleInfo.vMinPos.y = m_ParticleInfo.vMaxPos.y;
		}
		if (m_ParticleInfo.vMinPos.z > m_ParticleInfo.vMaxPos.z)
		{
			m_ParticleInfo.vMinPos.z = m_ParticleInfo.vMaxPos.z;
		}

		InputFloat3("Min Dir", reinterpret_cast<_float*>(&m_ParticleInfo.vMinDir));
		InputFloat3("Max Dir", reinterpret_cast<_float*>(&m_ParticleInfo.vMaxDir)); SameLine(); if (Button("Same with Min##1")) { m_ParticleInfo.vMaxDir = m_ParticleInfo.vMinDir; }
		if (m_ParticleInfo.vMinDir.x > m_ParticleInfo.vMaxDir.x)
		{
			m_ParticleInfo.vMinDir.x = m_ParticleInfo.vMaxDir.x;
		}
		if (m_ParticleInfo.vMinDir.y > m_ParticleInfo.vMaxDir.y)
		{
			m_ParticleInfo.vMinDir.y = m_ParticleInfo.vMaxDir.y;
		}
		if (m_ParticleInfo.vMinDir.z > m_ParticleInfo.vMaxDir.z)
		{
			m_ParticleInfo.vMinDir.z = m_ParticleInfo.vMaxDir.z;
		}

		InputFloat2("LifeTime", reinterpret_cast<_float*>(&m_ParticleInfo.vLifeTime));
		if (m_ParticleInfo.vLifeTime.x > m_ParticleInfo.vLifeTime.y)
		{
			m_ParticleInfo.vLifeTime.x = m_ParticleInfo.vLifeTime.y;
		}

		InputFloat2("Scale", reinterpret_cast<_float*>(&m_ParticleInfo.vScaleRange));
		if (m_ParticleInfo.vScaleRange.x > m_ParticleInfo.vScaleRange.y)
		{
			m_ParticleInfo.vScaleRange.x = m_ParticleInfo.vScaleRange.y;
		}

		InputFloat2("Speed", reinterpret_cast<_float*>(&m_ParticleInfo.vSpeedRange));
		if (m_ParticleInfo.vSpeedRange.x > m_ParticleInfo.vSpeedRange.y)
		{
			m_ParticleInfo.vSpeedRange.x = m_ParticleInfo.vSpeedRange.y;
		}

		Info.iNumInstances = m_iNumInstance;
		Info.fLifeTime = m_fEffectLifeTime;
		Info.PartiDesc.isLoop = isLoop;
		Info.PartiDesc.vMinPos = m_ParticleInfo.vMinPos;
		Info.PartiDesc.vMaxPos = m_ParticleInfo.vMaxPos;
		Info.PartiDesc.vMinDir = m_ParticleInfo.vMinDir;
		Info.PartiDesc.vMaxDir = m_ParticleInfo.vMaxDir;
		Info.PartiDesc.vLifeTime = m_ParticleInfo.vLifeTime;
		Info.PartiDesc.vScaleRange = m_ParticleInfo.vScaleRange;
		Info.PartiDesc.vSpeedRange = m_ParticleInfo.vSpeedRange;
		Info.ppShader = &m_pInstanceShader;
	}
	else
	{
		SeparatorText("Rect Information");

		Info.ppShader = &m_pVtxTexShader;
	}

	End();

	if (m_pEffect)
	{
		m_pEffect->Tick(fTimeDelta);
	}
	if (not m_pEffect or m_pEffect->isDead())
	{
		Safe_Release(m_pEffect);
		m_pEffect = dynamic_cast<CEffect_Dummy*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_Dummy", &Info));
	}
}

HRESULT CImgui_Manager::Render()
{
	if (m_pEffect)
	{
		if (FAILED(Render_Effect()))
		{
			return E_FAIL;
		}
	}

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(GetDrawData());

	return S_OK;
}

HRESULT CImgui_Manager::Ready_Layers()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(-1.f, -2.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LEVEL_STATIC, TEXT("Light_Main"), LightDesc)))
	{
		return E_FAIL;
	}

	CCamera::Camera_Desc CamDesc;
	CamDesc.vCameraPos = _float4(0.f, 5.f, -5.f, 1.f);
	CamDesc.vFocusPos = _float4(0.f, 0.f, 0.f, 1.f);
	CamDesc.fFovY = XMConvertToRadians(60.f);
	CamDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CamDesc.fNear = 0.1f;
	CamDesc.fFar = 100.f;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, L"Layer_Camera", TEXT("Prototype_GameObject_Camera"), &CamDesc)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"))))
	{
		return E_FAIL;
	}

	CVIBuffer_Instancing::ParticleDesc Desc{};
	m_pVtxTexShader = dynamic_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxTex"));
	if (not m_pVtxTexShader)
	{
		return E_FAIL;
	}

	m_pInstanceShader = dynamic_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxTex_Instancing"));
	if (not m_pInstanceShader)
	{
		return E_FAIL;
	}

	//m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect", &m_pVIBuffer_Rect);
	//m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Instancing_Point", &m_pVIBuffer_Instancing);

	for (_int i = 0; i < m_iNumTextures; i++)
	{
		wstring strPrototypeTag = L"Prototype_Component_Texture_Effect_" + to_wstring(i);
		m_pTextures[i] = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, strPrototypeTag));
	}

	return S_OK;
}

HRESULT CImgui_Manager::Render_Effect()
{
	CShader* pShader{};
	if (m_iShaderType == ET_PARTICLE)
	{
		pShader = m_pInstanceShader;
	}
	else
	{
		pShader = m_pVtxTexShader;
	}

	if (m_hasMask)
	{
		if (FAILED(m_pTextures[m_iSelected_MaskTexture]->Bind_ShaderResource(pShader, "g_MaskTexture")))
		{
			return E_FAIL;
		}
	}

	if (m_hasDiffTexture)
	{
		if (FAILED(m_pTextures[m_iSelected_Texture]->Bind_ShaderResource(pShader, "g_Texture")))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof m_vColor)))
		{
			return E_FAIL;
		}
	}

	if (FAILED(pShader->Bind_RawValue("g_CamPos", &m_pGameInstance->Get_CameraPos(), sizeof _vec4)))
	{
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	//pShader->Begin(InstancingPass::InstPass_Particle_MaskColor);

	m_pEffect->Render();

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
	for (_int i = 0; i < m_iNumTextures; i++)
	{
		Safe_Release(m_pTextures[i]);
	}
	Safe_Delete_Array(m_pTextures);

	Safe_Release(m_pInstanceShader);
	Safe_Release(m_pVtxTexShader);
	Safe_Delete_Array(m_pItemList_Texture);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();
}
