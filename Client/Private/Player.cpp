#include "Player.h"
#include "BodyPart.h"
CPlayer::CPlayer(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Init_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = true;

	Add_Parts();

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		m_Animation.iAnimIndex += 1;
		if (m_Animation.iAnimIndex >= m_pModelCom->Get_NumAnim())
		{
			m_Animation.iAnimIndex -= 1;
		}
	}
	if (m_pGameInstance->Key_Down(DIK_DOWN))
	{
		if (m_Animation.iAnimIndex != 0)
		{
			m_Animation.iAnimIndex -= 1;
		}
	}
	if (m_pGameInstance->Key_Down(DIK_K))
	{
		((CBodyPart*)m_vecParts[PT_HAIR])->Set_ModelIndex();
		((CBodyPart*)m_vecParts[PT_HAIR])->Reset_Model();
		((CBodyPart*)m_vecParts[PT_FACE])->Reset_Model();
		((CBodyPart*)m_vecParts[2])->Reset_Model();
	}


	if (m_pGameInstance->Key_Down(DIK_L))
	{
		((CBodyPart*)m_vecParts[PT_FACE])->Set_ModelIndex();
		((CBodyPart*)m_vecParts[PT_HAIR])->Reset_Model();
		((CBodyPart*)m_vecParts[PT_FACE])->Reset_Model();
		((CBodyPart*)m_vecParts[2])->Reset_Model();
	}
	m_pModelCom->Set_Animation(m_Animation);

	for (int i = 0; i < m_vecParts.size(); i++)
	{
		m_vecParts[i]->Tick(fTimeDelta);
	}
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	/*m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);*/
	for (int i = 0; i < m_vecParts.size();i++)
	{
		m_vecParts[i]->Late_Tick(fTimeDelta);
	}
}

HRESULT CPlayer::Render()
{
	/*if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}*/

	/*for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
		}

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}*/

	wstring strTotalAnim = L"ÃÑ ¾Ö´Ô °³¼ö : " + to_wstring(m_pModelCom->Get_NumAnim());
	wstring strCurrAnim = L"ÇöÀç ¾Ö´Ô : " + to_wstring(m_Animation.iAnimIndex);

	m_pGameInstance->Render_Text(L"Font_Dialogue", strTotalAnim, _vec2(150.f, 600.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue", strCurrAnim, _vec2(150.f, 650.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CPlayer::Add_Parts()
{
	CGameObject* pParts = { nullptr };

	BODYPART_DESC BodyParts_Desc{};
	BodyParts_Desc.pParentTransform = m_pTransformCom;
	BodyParts_Desc.Animation = &m_Animation;
	BodyParts_Desc.eType = PT_HAIR;
	BodyParts_Desc.iNumVariations = 9;
	wstring strName{};
	strName = TEXT("Prototype_GameObject_Body_Parts");
	pParts = m_pGameInstance->Clone_Object(strName, &BodyParts_Desc);

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	BodyParts_Desc.eType = PT_FACE;
	BodyParts_Desc.iNumVariations = 7;


	pParts = m_pGameInstance->Clone_Object(strName, &BodyParts_Desc);

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	BodyParts_Desc.eType = PT_BODY;
	BodyParts_Desc.iNumVariations = 1;

	pParts = m_pGameInstance->Clone_Object(strName, &BodyParts_Desc);

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Player"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CPlayer* CPlayer::Create(_dev pDevice, _context pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();


	for (auto& iter : m_vecParts)
	{
		Safe_Release(iter);
	}
	m_vecParts.clear();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
