#include "Player.h"

CPlayer::CPlayer(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
	, m_iNumMonsterModels(rhs.m_iNumMonsterModels)
	, m_iNumPlayerModels(rhs.m_iNumPlayerModels)
{
}

HRESULT CPlayer::Init_Prototype(_uint iNumMonsterModels, _uint iNumPlayerModels)
{
	m_iNumMonsterModels = iNumMonsterModels;
	m_iNumPlayerModels = iNumPlayerModels;

	return S_OK;
}

HRESULT CPlayer::Init(void* pArg)
{
	m_pMonsterModelCom = new CModel * [m_iNumMonsterModels];
	m_pMonsterModelTag = new wstring[m_iNumMonsterModels];

	for (_uint i = 0; i < m_iNumMonsterModels; i++)
	{
		_tchar szPrototypeTag[MAX_PATH] = TEXT("");
		const wstring& strPrototypeTag = TEXT("Prototype_Model_Monster_%d");
		wsprintf(szPrototypeTag, strPrototypeTag.c_str(), i);
		m_pMonsterModelTag[i] = szPrototypeTag;
	}

	m_pPlayerModelCom = new CModel * [m_iNumPlayerModels];
	m_pPlayerModelTag = new wstring[m_iNumPlayerModels];

	for (_uint i = 0; i < m_iNumPlayerModels; i++)
	{
		_tchar szPrototypeTag[MAX_PATH] = TEXT("");
		const wstring& strPrototypeTag = TEXT("Prototype_Model_Player_%d");
		wsprintf(szPrototypeTag, strPrototypeTag.c_str(), i);
		m_pPlayerModelTag[i] = szPrototypeTag;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		_vector vPos = XMLoadFloat4(&m_vPos);
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		vPos += vUp * m_fGravity * fTimeDelta * 0.6f;

		m_fGravity -= 19.8f * fTimeDelta;

		m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta);
		vPos = XMVectorSetW(vPos, 1.f);
		m_pTransformCom->Set_State(State::Pos, vPos);

		XMStoreFloat4(&m_vPos, vPos);
	}
	if (m_eType == TYPE_MONSTER)
	{
		m_pModelCom = m_pMonsterModelCom[m_iCurrentIndex];
	}
	else if (m_eType == TYPE_PLAYER)
	{
		m_pModelCom = m_pPlayerModelCom[m_iCurrentIndex];
	}

	if (m_pModelCom)
	{
		m_pModelCom->Play_Animation(0.f);
	}
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (m_pModelCom)
	{
		for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
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

			_uint iPassIndex{ AnimPass_Default };

			if (FAILED(m_pShaderCom->Begin(iPassIndex)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}
	}

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

	for (_uint i = 0; i < m_iNumMonsterModels; i++)
	{
		_tchar szComName[MAX_PATH] = TEXT("");
		const wstring& strComName = TEXT("Com_Model_Monster%d");
		wsprintf(szComName, strComName.c_str(), i);
		wstring strFinalComName = szComName;

		if (FAILED(__super::Add_Component(LEVEL_TOOL, m_pMonsterModelTag[i], strFinalComName, reinterpret_cast<CComponent**>(&m_pMonsterModelCom[i]))))
		{
			return E_FAIL;
		}
	}

	for (_uint i = 0; i < m_iNumPlayerModels; i++)
	{
		_tchar szComName[MAX_PATH] = TEXT("");
		const wstring& strComName = TEXT("Com_Model_Player%d");
		wsprintf(szComName, strComName.c_str(), i);
		wstring strFinalComName = szComName;

		if (FAILED(__super::Add_Component(LEVEL_TOOL, m_pPlayerModelTag[i], strFinalComName, reinterpret_cast<CComponent**>(&m_pPlayerModelCom[i]))))
		{
			return E_FAIL;
		}
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

CPlayer* CPlayer::Create(_dev pDevice, _context pContext, _uint iNumMonsterModels, _uint iNumPlayerModels)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(iNumMonsterModels, iNumPlayerModels)))
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

	if (m_pMonsterModelCom)
	{
		for (_uint i = 0; i < m_iNumMonsterModels; i++)
		{
			Safe_Release(m_pMonsterModelCom[i]);
		}
	}

	if (m_pPlayerModelCom)
	{
		for (_uint i = 0; i < m_iNumPlayerModels; i++)
		{
			Safe_Release(m_pPlayerModelCom[i]);
		}
	}

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
