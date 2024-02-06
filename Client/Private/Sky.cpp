#include "Sky.h"

CSky::CSky(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSky::CSky(const CSky& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSky::Init_Prototype()
{
	return S_OK;
}

HRESULT CSky::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	m_iTextureIndex = 10;
	return S_OK;
}

void CSky::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_NUMPAD2))
	{
		m_iTextureIndex++;

	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD3))
	{
		m_iTextureIndex--;

	}

	switch (m_pGameInstance->Get_CurrentLevelIndex())
	{
	case LEVEL_GAMEPLAY:
		m_fLightning_Time += fTimeDelta;
		if (m_fLightning_Time > 8.f && m_fLightning_Time<8.1f)
		{
			if(m_iTextureIndex==10)
			{
				if (m_pGameInstance->Get_CameraState() == CS_SKILLBOOK or m_pGameInstance->Get_CameraState() == CS_INVEN)
				{
					return;
				}
				int random = rand() % 3;
				switch (random)
				{
				case 0:m_pGameInstance->Play_Sound(TEXT("lightning0"), 0.8f, false);
					break;
				case 1:m_pGameInstance->Play_Sound(TEXT("lightning1"), 0.8f, false);
					break;
				case 2:m_pGameInstance->Play_Sound(TEXT("lightnin2"), 0.8f, false);
					break;
				default:
					break;
				}
				
			}
			m_iTextureIndex = 9;
		}
		else if(m_fLightning_Time> 8.1f && m_fLightning_Time<8.2f)
		{
			m_iTextureIndex = 10;
		}
		else if (m_fLightning_Time>=8.2f && m_fLightning_Time<8.3f)
		{
			m_iTextureIndex = 9;
		}
		else if (m_fLightning_Time >= 8.4f)
		{
			m_fLightning_Time = 0.f;
			m_iTextureIndex = 10;
		}
		break;
	case LEVEL_VILLAGE:
		m_iTextureIndex = 12;
		break;
	default:
		break;
	}
}

void CSky::Late_Tick(_float fTimeDelta)
{
	_vec4 vPos = m_pGameInstance->Get_CameraPos() - _vec4(0.f,15.f, 0.f, 0.f);

	m_pTransformCom->Set_State(State::Pos, vPos);

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Priority, this);
}

HRESULT CSky::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		//{
		//	return E_FAIL;
		//}

		if (FAILED(m_pShaderCom->Begin(StaticPass_Sky)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSky::Render_Reflection(_float4 vClipPlane)
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}


	_mat ReflectCamWorldMat = m_pGameInstance->Get_Transform_Inversed(TransformType::View);

	_vec3 vRefelctCamPos = ReflectCamWorldMat.Position_vec3() - _vec4(0.f, 15.f, 0.f, 0.f);

	m_pTransformCom->Set_Position(vRefelctCamPos);

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vClipPlane", &vClipPlane, sizeof(_float4))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		//{
		//	return E_FAIL;
		//}

		if (FAILED(m_pShaderCom->Begin(StaticPass_Sky)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSky::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sky"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	for (size_t i = 0; i < 19; i++)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky") + to_wstring(i), TEXT("Com_Texture") + to_wstring(i), reinterpret_cast<CComponent**>(&m_Textures[i]))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSky::Bind_ShaderResources()
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

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vFogNF", &m_pGameInstance->Get_FogNF(), sizeof _float2)))
	//{
	//	return E_FAIL;
	//}

	//const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Light_Main"));

	_vec4 diffuse = _vec4(1, 1, 1, 1);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &diffuse, sizeof _vec4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_Textures[m_iTextureIndex]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSky* CSky::Create(_dev pDevice, _context pContext)
{
	CSky* pInstance = new CSky(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSky::Clone(void* pArg)
{
	CSky* pInstance = new CSky(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	for (int i = 0; i < 19; i++)
	{
	Safe_Release(m_Textures[i]);
	}

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
