#include "Imgui_Manager.h"
#include "Camera.h"
#include "Effect_Dummy.h"
#include <commdlg.h>

IMPLEMENT_SINGLETON(CImgui_Manager)

using namespace ImGui;

CImgui_Manager::CImgui_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImgui_Manager::Init(_dev pDevice, _context pContext, vector<string>* pTextureList, vector<string>* pModelList)
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

	m_iNumModels = static_cast<_int>(pModelList->size());
	m_pItemList_Model = new const _char * [m_iNumModels];
	m_pModels = new CModel* [m_iNumModels];

	for (_int i = 0; i < m_iNumModels; i++)
	{
		m_pItemList_Model[i] = (*pModelList)[i].c_str();
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

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	static _bool bDemo{ true };
	ShowDemoWindow(&bDemo);
	EffectInfo Info{};
	Begin("Effect_Tool");

	const _char* szType[ET_END]{ "Particle", "Rect", "Mesh" };

	if (Combo("Type", &m_iCurrent_Type, szType, IM_ARRAYSIZE(szType)))
	{

	}

	static _bool hasLifeTime{};
	Checkbox("Limited Life Time", &hasLifeTime);

	if (hasLifeTime)
	{
		InputFloat("Effect Life", &m_fEffectLifeTime);
		Info.fLifeTime = m_fEffectLifeTime;
	}
	else
	{
		m_fEffectLifeTime = -1.f;
		Info.fLifeTime = m_fEffectLifeTime;
	}

	NewLine();
	SeparatorText("Position Offset");
	static _vec3 vPosOffset{};
	InputFloat3("Offset", reinterpret_cast<_float*>(&vPosOffset), "%.2f");
	Info.vPosOffset = vPosOffset;
	NewLine();

	static _int iPassIndex{};

	const _char* szInstancingPasses[InstPass_End]
	{
		"InstPass_Particle_TextureMask",
		"InstPass_Particle_Sprite",
		"InstPass_Particle_MaskColor",
		"InstPass_Particle_TextureMask_Dissolve",
		"InstPass_Particle_Sprite_Dissolve",
		"InstPass_Particle_MaskColor_Dissolve",

	};

	const _char* szVTPasses[VTPass_End]
	{
		"VTPass_UI",
		"VTPass_UI_Alpha",
		"VTPass_UI_Color_Alpha",
		"VTPass_Button",
		"VTPass_Background",
		"VTPass_Background_Mask",
		"VTPass_Mask_Texture",
		"VTPass_Inv_Mask_Texture",
		"VTPass_Mask_Color",
		"VTPass_Mask_ColorAlpha",
		"VTPass_HP",
		"VTPass_Hit",
		"VTPass_Sprite",
		"VTPass_SpriteMaskTexture",
		"VTPass_SpriteMaskColor",
		"VTPass_Hell",
		"VTPass_Mask_Texture_Dissolve",
		"VTPass_Inv_Mask_Texture_Dissolve",
		"VTPass_Mask_Color_Dissolve",
		"VTPass_Sprite_Dissolve",
		"VTPass_SpriteMaskTexture_Dissolve",
		"VTPass_SpriteMaskColor_Dissolve",
	};

	const _char* szStatPasses[StaticPass_End]
	{
		"StaticPass_Default",
		"StaticPass_NonLight",
		"StaticPass_OutLine",
		"StaticPass_AlphaTestMeshes",
		"StaticPass_Sky",
		"StaticPass_COLMesh",
		"StaticPass_SingleColorFx",
		"StaticPass_SingleColorDissolve",
		"StaticPass_Fireball",
		"StaticPass_MaskEffect",
		"StaticPass_MaskDissolve",
		"StaticPass_MaskEffectClamp",
		"StaticPass_SingleColoredEffectFrontCull",
		"StaticPass_Shadow",
	};

	SeparatorText("Particle Information");

	if (m_iCurrent_Type == ET_PARTICLE)
	{
		ListBox("Pass##1", &iPassIndex, szInstancingPasses, InstPass_End);
		if (iPassIndex >= InstPass_End)
		{
			iPassIndex = 0;
		}
	}
	else if (m_iCurrent_Type == ET_RECT)
	{
		ListBox("Pass##2", &iPassIndex, szVTPasses, VTPass_End);
		if (iPassIndex >= VTPass_End)
		{
			iPassIndex = 0;
		}
	}
	else if (m_iCurrent_Type == ET_MESH)
	{
		ListBox("Pass##3", &iPassIndex, szStatPasses, StaticPass_End);
		if (iPassIndex >= StaticPass_End)
		{
			iPassIndex = 0;
		}
	}
	Info.iType = m_iCurrent_Type;
	Info.iPassIndex = iPassIndex;

	NewLine();

	static _int iIsColor{};
	static _bool hasDissolve{};

	SeparatorText("Diffuse");
	RadioButton("Texture", &iIsColor, 0); SameLine();
	RadioButton("Color", &iIsColor, 1);

	static _bool isSprite{};
	static _int2 vNumSprites{ 1, 1 };
	static _float fSpriteDuration{ 1.f };
	if (iIsColor == 0)
	{
		Checkbox("Sprite", &isSprite);
		if (isSprite)
		{
			InputInt2("Number of Sprites", reinterpret_cast<_int*>(&vNumSprites));

			InputFloat("Duration", &fSpriteDuration, 0.f, 0.f, "%.1f");

			Info.isSprite = true;
			Info.vNumSprites = vNumSprites;
			Info.fSpriteDuration = fSpriteDuration;
		}
		Separator();
		NewLine();

		ListBox("Texture", &m_iSelected_Texture, m_pItemList_Texture, m_iNumTextures);
		Image(reinterpret_cast<void*>(m_pTextures[m_iSelected_Texture]->Get_SRV()), ImVec2(128.f, 128.f));
		m_hasDiffTexture = true;
	}
	else
	{
		ColorPicker4("Color", reinterpret_cast<_float*>(&m_vColor), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB);
		m_hasDiffTexture = false;
	}

	if (m_hasDiffTexture)
	{
		Info.iDiffTextureID = m_iSelected_Texture;
	}
	else
	{
		Info.iDiffTextureID = -1;
		Info.vColor = m_vColor;
	}

	Checkbox("Mask", &m_hasMask);

	if (m_hasMask)
	{
		SameLine();
		SeparatorText("Texture");
		ListBox("Mask Texture", &m_iSelected_MaskTexture, m_pItemList_Texture, m_iNumTextures);
		Image(reinterpret_cast<void*>(m_pTextures[m_iSelected_MaskTexture]->Get_SRV()), ImVec2(128.f, 128.f));
		Info.iMaskTextureID = m_iSelected_MaskTexture;
	}
	else
	{
		Info.iMaskTextureID = -1;
	}
	Separator();
	NewLine();

	Checkbox("Dissolve", &hasDissolve);

	static _int iSelectd_Dissolve{};
	static _float fDissolveDuration{};
	if (hasDissolve)
	{
		SameLine();
		SeparatorText("Texture");
		InputFloat("Dissove Duration", &fDissolveDuration);
		Info.fDissolveDuration = fDissolveDuration;
		if (iSelectd_Dissolve < 0)
		{
			iSelectd_Dissolve = 0;
		}
		ListBox("Dissolve Texture", &iSelectd_Dissolve, m_pItemList_Texture, m_iNumTextures);
		Image(reinterpret_cast<void*>(m_pTextures[iSelectd_Dissolve]->Get_SRV()), ImVec2(128.f, 128.f));
		Info.iDissolveTextureID = iSelectd_Dissolve;
		m_EffectInfo.iDissolveTextureID = iSelectd_Dissolve;
	}
	else
	{
		iSelectd_Dissolve = -1;
		Info.iDissolveTextureID = -1;
		m_EffectInfo.iDissolveTextureID = iSelectd_Dissolve;
	}
	Separator();
	NewLine();

	static _vec2 vSize{ 1.f, 1.f };
	static _float fSizeforSprite{ 1.f };
	static _vec2 vSizeDelta{};
	static _bool bApplyGravity{};
	static _vec3 vGravityDir{};

	if (m_iCurrent_Type == ET_PARTICLE)
	{
		SeparatorText("Particle Information");
		Checkbox("Loop", &m_ParticleInfo.isLoop);

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

		Checkbox("Gravity", &bApplyGravity);

		if (bApplyGravity)
		{
			InputFloat3("Gravity Dir", reinterpret_cast<_float*>(&vGravityDir), "%.2f");
		}

		Info.iNumInstances = m_iNumInstance;
		Info.PartiDesc = m_ParticleInfo;
		Info.vSize = _vec2(1.f);
		Info.bApplyGravity = bApplyGravity;
		Info.vGravityDir = vGravityDir;
	}
	else if (m_iCurrent_Type == ET_RECT)
	{
		SeparatorText("Rect Information");

		if (Info.isSprite)
		{
			InputFloat("Size##1", &fSizeforSprite);

			Info.vSize.y = fSizeforSprite;
			_float fSizeRatio = 1.f / (Info.vNumSprites.x / Info.vNumSprites.y);
			Info.vSize.x = fSizeforSprite * fSizeRatio;
		}
		else
		{
			InputFloat2("Size##2", reinterpret_cast<_float*>(&vSize), "%.2f");
			Info.vSize = vSize;
		}

		InputFloat2("Size Delta", reinterpret_cast<_float*>(&vSizeDelta), "%.2f");
		Info.vSizeDelta = vSizeDelta;
	}
	else if (m_iCurrent_Type == ET_MESH)
	{
		SeparatorText("Mesh Information");

		ListBox("Model", &m_iSelected_Model, m_pItemList_Model, m_iNumModels);
		Info.iModelIndex = m_iSelected_Model;
	}

	if (Button("Export"))
	{
		Export_Data();
	} SameLine();

	if (Button("Load"))
	{
		EffectInfo Info = Load_Data();

		Safe_Release(m_pEffect);
		m_pEffect = dynamic_cast<CEffect_Dummy*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_Dummy", &Info));
		m_EffectInfo = Info;

		m_iCurrent_Type = Info.iType;

		if (Info.fLifeTime < 0)
		{
			hasLifeTime = false;
			m_fEffectLifeTime = -1.f;
		}
		else
		{
			hasLifeTime = true;
			m_fEffectLifeTime = Info.fLifeTime;
		}

		vPosOffset = Info.vPosOffset;

		iPassIndex = Info.iPassIndex;

		if (Info.iDiffTextureID < 0)
		{
			iIsColor = 1;
			m_hasDiffTexture = false;
			m_vColor = Info.vColor;
		}
		else
		{
			iIsColor = 0;
			m_hasDiffTexture = true;
			m_iSelected_Texture = Info.iDiffTextureID;
			isSprite = Info.isSprite;
			vNumSprites = Info.vNumSprites;
			fSpriteDuration = Info.fSpriteDuration;
		}

		if (Info.iMaskTextureID < 0)
		{
			m_hasMask = false;
		}
		else
		{
			m_hasMask = true;
			m_iSelected_MaskTexture = Info.iMaskTextureID;
		}

		if (Info.iDissolveTextureID < 0)
		{
			hasDissolve = false;
		}
		else
		{
			hasDissolve = true;
			iSelectd_Dissolve = Info.iDissolveTextureID;
			fDissolveDuration = Info.fDissolveDuration;
		}

		if (Info.iType == ET_PARTICLE)
		{
			m_ParticleInfo = Info.PartiDesc;
			m_iNumInstance = Info.iNumInstances;
			bApplyGravity = Info.bApplyGravity;
			vGravityDir = Info.vGravityDir;
		}
		else
		{
			if (Info.isSprite)
			{
				fSizeforSprite = Info.vSize.y;
			}
			vSize = Info.vSize;
			vSizeDelta = Info.vSizeDelta;
		}
	}

	if (Button("Add"))
	{
		CEffect_Dummy* pEffect{};
		pEffect = dynamic_cast<CEffect_Dummy*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_Dummy", &Load_Data()));
		m_AddEffect.push_back(pEffect);
	} SameLine();

	if (Button("Clear Add"))
	{
		for (auto& pEffect : m_AddEffect)
		{
			Safe_Release(pEffect);
		}
		m_AddEffect.clear();
	}

	End();

	if (m_pEffect)
	{
		m_pEffect->Tick(fTimeDelta);
	}

	for (auto& pEffect : m_AddEffect)
	{
		pEffect->Tick(fTimeDelta);
	}

	if (not m_pEffect or m_pEffect->isDead() or m_pGameInstance->Key_Down(DIK_RETURN) or m_pGameInstance->Key_Down(DIK_E))
	{
		Safe_Release(m_pEffect);
		m_pEffect = dynamic_cast<CEffect_Dummy*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_Dummy", &Info));
		m_EffectInfo = Info;
		m_pEffect->Tick(0.f);
	}
}

HRESULT CImgui_Manager::Render()
{
	if (m_pEffect)
	{
		if (FAILED(m_pEffect->Render()))
		{
			return E_FAIL;
		}
	}

	for (auto& pEffect : m_AddEffect)
	{
		if (FAILED(pEffect->Render()))
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

	_uint2 vTerrainSize{ 3, 3 };
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), &vTerrainSize)))
	{
		return E_FAIL;
	}

	//CVIBuffer_Instancing::ParticleDesc Desc{};
	//m_pVtxTexShader = dynamic_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxTex"));
	//if (not m_pVtxTexShader)
	//{
	//	return E_FAIL;
	//}

	//m_pInstanceShader = dynamic_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxTex_Instancing"));
	//if (not m_pInstanceShader)
	//{
	//	return E_FAIL;
	//}

	//m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect", &m_pVIBuffer_Rect);
	//m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Instancing_Point", &m_pVIBuffer_Instancing);

	for (_int i = 0; i < m_iNumTextures; i++)
	{
		wstring strPrototypeTag = L"Prototype_Component_Texture_Effect_" + to_wstring(i);
		m_pTextures[i] = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, strPrototypeTag));
	}

	for (_int i = 0; i < m_iNumModels; i++)
	{
		wstring strPrototypeTag = L"Prototype_Model_" + to_wstring(i);
		m_pModels[i] = dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, strPrototypeTag));
	}

	return S_OK;
}

EffectInfo CImgui_Manager::Load_Data()
{
	EffectInfo Info{};

	OPENFILENAME ofn;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"Effect.effect";
	static TCHAR filter[] = L"¿Ã∆Â∆Æ ∆ƒ¿œ(*.effect)\0*.effect\0";

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = 256;
	ofn.lpstrInitialDir = L"..\\..\\Client\\Bin\\EffectData";

	if (GetOpenFileName(&ofn))
	{
		filesystem::path strFilePath = ofn.lpstrFile;
		ifstream InFile(strFilePath.c_str(), ios::binary);

		if (InFile.is_open())
		{
			InFile.read(reinterpret_cast<_char*>(&Info), sizeof EffectInfo);

			InFile.close();
		}
	}

	return Info;
}

HRESULT CImgui_Manager::Export_Data()
{
	OPENFILENAME ofn;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"Effect.effect";
	static TCHAR filter[] = L"¿Ã∆Â∆Æ ∆ƒ¿œ(*.effect)\0*.effect\0";

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = 256;
	ofn.lpstrInitialDir = L"..\\..\\Client\\Bin\\EffectData";
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
	{
		filesystem::path strFilePath = ofn.lpstrFile;
		ofstream OutFile(strFilePath.c_str(), ios::binary);

		if (OutFile.is_open())
		{
			OutFile.write(reinterpret_cast<const _char*>(&m_EffectInfo), sizeof EffectInfo);

			OutFile.close();
		}
	}

	return S_OK;
}

void CImgui_Manager::Free()
{
	for (auto& pEffect : m_AddEffect)
	{
		Safe_Release(pEffect);
	}
	m_AddEffect.clear();

	Safe_Release(m_pEffect);

	for (_int i = 0; i < m_iNumTextures; i++)
	{
		Safe_Release(m_pTextures[i]);
	}
	Safe_Delete_Array(m_pTextures);

	for (_int i = 0; i < m_iNumModels; i++)
	{
		Safe_Release(m_pModels[i]);
	}
	Safe_Delete_Array(m_pModels);

	//Safe_Release(m_pInstanceShader);
	//Safe_Release(m_pVtxTexShader);
	Safe_Delete_Array(m_pItemList_Texture);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();
}
