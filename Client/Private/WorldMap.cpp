#include "WorldMap.h"
#include "UI_Manager.h"

CWorldMap::CWorldMap(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CWorldMap::CWorldMap(const CWorldMap& rhs)
	: CGameObject(rhs)
{
}


HRESULT CWorldMap::Init_Prototype()
{
	return S_OK;
}

HRESULT CWorldMap::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(1000.f,1000.f,1000.f, 1.f));

	return S_OK;
}

void CWorldMap::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CameraState() != CS_WORLDMAP)
	{
		return;
	}
	m_fWater_Nomal.x += fTimeDelta * 0.1f;
	m_fWater_Nomal.y -= fTimeDelta * 0.1f;

}             

void CWorldMap::Late_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CameraState() != CS_WORLDMAP)
		return;

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

}

HRESULT CWorldMap::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	for (_uint j = 0; j < 3; j++)
	{
		_uint iNumMeshes = m_pModelCom[j]->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom[j]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
				return E_FAIL;
			}

			_bool HasNorTex{};
			if (FAILED(m_pModelCom[j]->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
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
			if (j == 2)
			{
				if(i==1)
				{
					_float fy = m_fWater_Nomal.y * 0.7f;
					if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &fy, sizeof _float)))
					{
						return E_FAIL;
					}
				}
				else if (i == 2)
				{
					_float fy = m_fWater_Nomal.y * 1.5f;
					if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &fy, sizeof _float)))
					{
						return E_FAIL;
					}
				}
				else
				{
					_float fy = m_fWater_Nomal.y * 0.15f;
					if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &fy, sizeof _float)))
					{
						return E_FAIL;
					}
				}
				if (FAILED(m_pShaderCom->Begin(15)))
				{
					return E_FAIL;
				}
			}
			else if (j == 1)
			{
			
				if (FAILED(m_pMaskTexture->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
				{
					return E_FAIL;
				}
				_float fy = m_fWater_Nomal.y * 0.08f;
				_float fx = m_fWater_Nomal.x * 0.08f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &fy, sizeof _float)))
				{
					return E_FAIL;
				}
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &fx, sizeof _float)))
				{
					return E_FAIL;
				}
				if (FAILED(m_pShaderCom->Begin(16)))
				{
					return E_FAIL;
				}
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(0)))
				{
					return E_FAIL;
				}
			}

			if (FAILED(m_pModelCom[j]->Render(i)))
			{
				return E_FAIL;
			}


		}
	}


	return S_OK;
}

HRESULT CWorldMap::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}
	m_iShaderPass = StaticPass_Default;
	

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_WorldMap_Object"), TEXT("Com_Model0"), reinterpret_cast<CComponent**>(&m_pModelCom[0]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_WorldMap_Ground"), TEXT("Com_Model1"), reinterpret_cast<CComponent**>(&m_pModelCom[1]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_WorldMap_Water"), TEXT("Com_Model2"), reinterpret_cast<CComponent**>(&m_pModelCom[2]))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_WorldMap_Cloud"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pMaskTexture))))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CWorldMap::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}



	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
	{
		return E_FAIL;
	}

	return S_OK;
}


CWorldMap* CWorldMap::Create(_dev pDevice, _context pContext)
{
	CWorldMap* pInstance = new CWorldMap(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CWorldMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWorldMap::Clone(void* pArg)
{
	CWorldMap* pInstance = new CWorldMap(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CWorldMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWorldMap::Free()
{
	__super::Free();

	for (int i = 0; i < 3; i++)
	{

	Safe_Release(m_pModelCom[i]);
	}

	Safe_Release(m_pMaskTexture);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
