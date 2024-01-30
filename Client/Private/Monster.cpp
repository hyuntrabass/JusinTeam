#include "Monster.h"


CMonster::CMonster(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CGameObject(rhs)
	, m_pInfo(rhs.m_pInfo)
{
}

HRESULT CMonster::Init_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}


	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{
	if (m_fDeadTime >= 2.f)
	{
		m_iPassIndex = AnimPass_Dissolve;
	}

	if (m_fDissolveRatio >= 1.f)
	{
		Kill();
		m_pGameInstance->Delete_CollisionObject(this);
	}
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CMonster::Render()
{
	if (m_iPassIndex == AnimPass_Dissolve)
	{
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
		{
			return E_FAIL;
		}

		m_fDissolveRatio += 0.02f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof _float)))
		{
			return E_FAIL;
		}

		_bool bHasNorTex = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &bHasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			_bool bFailed = true;
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

		if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
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

_vec4 CMonster::Compute_PlayerPos()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	return vPlayerPos;
}

_float CMonster::Compute_PlayerDistance()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);

	_float fDistance = (vPlayerPos - vPos).Length();

	return fDistance;
}

_vec4 CMonster::Compute_ModelTestPos()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_ModelTest", LEVEL_GAMEPLAY);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	return vPlayerPos;
}

_float CMonster::Compute_ModelTestDistance()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_ModelTest", LEVEL_GAMEPLAY);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);

	_float fDistance = (vPlayerPos - vPos).Length();

	return fDistance;
}

void CMonster::Update_MonsterCollider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

void CMonster::Change_Extents(_vec3 vStartSize)
{
	if (m_pGameInstance->Key_Down(DIK_NUMPAD7))
	{
		m_fSizeX += m_fScale;
		cout << "Collider Extents" << endl << m_pBodyColliderCom->Get_Extents().x << endl
			<< m_pBodyColliderCom->Get_Extents().y << endl
			<< m_pBodyColliderCom->Get_Extents().z << endl;
		cout << endl << "Scale Value" << endl << m_fScale << endl;
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD8))
	{
		m_fSizeY += m_fScale;
		cout << "Collider Extents" << endl << m_pBodyColliderCom->Get_Extents().x << endl
			<< m_pBodyColliderCom->Get_Extents().y << endl
			<< m_pBodyColliderCom->Get_Extents().z << endl;
		cout << endl << "Scale Value" << endl << m_fScale << endl;
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD9))
	{
		m_fSizeZ += m_fScale;
		cout << "Collider Extents" << endl << m_pBodyColliderCom->Get_Extents().x << endl
			<< m_pBodyColliderCom->Get_Extents().y << endl
			<< m_pBodyColliderCom->Get_Extents().z << endl;
		cout << endl << "Scale Value" << endl << m_fScale << endl;
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
	{
		m_fSizeX -= m_fScale;
		cout << "Collider Extents" << endl << m_pBodyColliderCom->Get_Extents().x << endl
			<< m_pBodyColliderCom->Get_Extents().y << endl
			<< m_pBodyColliderCom->Get_Extents().z << endl;
		cout << endl << "Scale Value" << endl << m_fScale << endl;
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
	{
		m_fSizeY -= m_fScale;
		cout << "Collider Extents" << endl << m_pBodyColliderCom->Get_Extents().x << endl
			<< m_pBodyColliderCom->Get_Extents().y << endl
			<< m_pBodyColliderCom->Get_Extents().z << endl;
		cout << endl << "Scale Value" << endl << m_fScale << endl;
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD6))
	{
		m_fSizeZ -= m_fScale;
		cout << "Collider Extents" << endl << m_pBodyColliderCom->Get_Extents().x << endl
			<< m_pBodyColliderCom->Get_Extents().y << endl
			<< m_pBodyColliderCom->Get_Extents().z << endl;
		cout << endl << "Scale Value" << endl << m_fScale << endl;
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		m_fScale -= 0.1f;
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD3))
	{
		m_fScale += 0.1f;
	}

	m_pBodyColliderCom->Change_Extents(_vec3(vStartSize.x + m_fSizeX, vStartSize.y + m_fSizeY, vStartSize.z + m_fSizeZ));
}

HRESULT CMonster::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_T_EFF_Noise_04_BC"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pAttackColliderCom);

	Safe_Release(m_pDissolveTextureCom);
}
