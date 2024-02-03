#include "CutScene_Curve.h"
static _int iID = 1;

CCutScene_Curve::CCutScene_Curve(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CCutScene_Curve::CCutScene_Curve(const CCutScene_Curve& rhs)
	: CGameObject(rhs)
	, Info(rhs.Info)
{
	m_iID = iID++;
}

HRESULT CCutScene_Curve::Init_Prototype()
{
	return S_OK;
}

HRESULT CCutScene_Curve::Init(void* pArg)
{
	Info = *(CameraInfo*)pArg;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (Info.ppCamera)
	{
		*Info.ppCamera = this;
		Info.ppCamera = nullptr;
	}

	m_WorldMatrix = m_pTransformCom->Get_World_Matrix();

	Set_Points();

	return S_OK;
}

void CCutScene_Curve::Tick(_float TimeDelta)
{
	m_pStartPoint->Tick(TimeDelta);
	m_pEndPoint->Tick(TimeDelta);

}

void CCutScene_Curve::Late_Tick(_float TimeDelta)
{
	m_pStartPoint->Late_Tick(TimeDelta);
	m_pEndPoint->Late_Tick(TimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
}

HRESULT CCutScene_Curve::Render()
{
	m_pVIBuffer->Modify_Line();

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	m_pShaderCom->Begin(0);
	m_pVIBuffer->Render();
	return S_OK;
}

HRESULT CCutScene_Curve::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Curve"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Curve"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCutScene_Curve::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	_float4 vColor = _float4(0.f, 1.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CCutScene_Curve::Select(const _bool& isSelected)
{
	m_isSelected = isSelected;
}


void CCutScene_Curve::Set_Points()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Camera_Point"), TEXT("Prototype_GameObject_Camera_Point"), & Info.vStartCutScene)))
	{
		MSG_BOX("Failed to Add Layer : Dummy");
	}

	m_pStartPoint = static_cast<CCutScene_Point*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Camera_Point"), &Info.vStartCutScene));
	m_pEndPoint = static_cast<CCutScene_Point*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Camera_Point"), &Info.vEndCutScene));
}

CCutScene_Curve* CCutScene_Curve::Create(_dev pDevice, _context pContext)
{
	CCutScene_Curve* pInstance = new CCutScene_Curve(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created CCutScene_Curve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCutScene_Curve::Clone(void* pArg)
{
	CCutScene_Curve* pInstance = new CCutScene_Curve(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned CCutScene_Curve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCutScene_Curve::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pStartPoint);
	Safe_Release(m_pEndPoint);
}