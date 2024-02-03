#include "Camera_CutScene.h"

CCamera_CutScene::CCamera_CutScene(_dev pDevice, _context pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_CutScene::CCamera_CutScene(const CCamera_CutScene& rhs)
	: CCamera(rhs)
	//, m_pImGui_Manager(CImGui_Manager::Get_Instance())
{
	//Safe_AddRef(m_pImGui_Manager);
}

HRESULT CCamera_CutScene::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera_CutScene::Init(void* pArg)
{
	CamInfo = *(CameraInfo*)pArg;
	if (FAILED(__super::Init(pArg)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	//if (CamInfo.ppCamera)
	//{
	//	*CamInfo.ppCamera = this;
	//	CamInfo.ppCamera = nullptr;
	//}
	wstring strLayerTag = TEXT("Layer_Camera");


	m_iNextSectionIndex = 0;
	m_iCurrentSectionIndex = 0;
	m_fCutSceneSpeed = 10.f;
	m_fTimeDeltaAcc = 0.f;
	m_iLastFrame = 0;
	
	m_isPreCameraLerp = false;
	m_vPreEye = _float3(0.f, 0.f, 0.f);
	m_vPreAt = _float3(0.f, 0.f, 0.f);
	m_fPreCameraLerpTimeAcc = 0.f;
	m_fPreCameraLerpTime = 1.5f;

	m_vResultEye = _vec4(CamInfo.eCamera_Desc.vCameraPos);
	m_vResultAt = _vec4(CamInfo.eCamera_Desc.vFocusPos);

	Add_Eye_Curve(CamInfo.vStartCutScene, CamInfo.vEndCutScene);
	Add_At_Curve(CamInfo.vStartCutScene, CamInfo.vEndCutScene);

	return S_OK;
}

void CCamera_CutScene::Tick(_float fTimeDelta)
{


	//__super::Tick(fTimeDelta);
}

void CCamera_CutScene::Late_Tick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

}

HRESULT CCamera_CutScene::Render()
{
	
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			continue;
		}

	
		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}
	return S_OK;
}


HRESULT CCamera_CutScene::Add_Eye_Curve(_vec4 vFirstPoint, _vec4 vSecondPoint)
{
	_uint	iCurrentLevel = m_pGameInstance->Get_CurrentLevelIndex();
	_uint	iCutSceneType = CCutScene_Curve::SECTION_TYPE_EYE;

	SectionInfo SectionInfo;
	SectionInfo.vStartCutScene = vFirstPoint;
	SectionInfo.vEndCutScene = vSecondPoint;

	wstring strSectionEyeName = CamInfo.strName + L"_Curve_Eye_" + to_wstring(m_CameraEyeList.size());
	m_pEyeCurve = static_cast<CCutScene_Curve*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Camera_Curve"), &SectionInfo));



	_mat	matPoints;
	ZeroMemory(&matPoints, sizeof _mat);

	/* First Section */
	if (m_CameraEyeList.empty() || m_CameraEyeList.size() == 1)
	{
		matPoints = {
			vFirstPoint.x, vFirstPoint.y, vFirstPoint.z, 1.f,
			vFirstPoint.x, vFirstPoint.y, vFirstPoint.z, 1.f,
			vSecondPoint.x, vSecondPoint.y, vSecondPoint.z, 1.f,
			vSecondPoint.x, vSecondPoint.y, vSecondPoint.z, 1.f
		};
	}
	/* Not First Section */
	else if (!m_CameraEyeList.empty() && m_CameraEyeList.size() != 1)
	{
		_int iIndex = (_int)m_CameraEyeList.size() - 2;
		if (iIndex < 0)
		{
			return E_FAIL;
		}

		_mat matPrePoints;
		static_cast<CCutScene_Curve*>(m_CameraEyeList[iIndex])->Get_ControlPoints(&matPrePoints);

		matPoints = {
			matPrePoints._11, matPrePoints._12, matPrePoints._13, matPrePoints._14,
			vFirstPoint.x, vFirstPoint.y, vFirstPoint.z, 1.f,
			vSecondPoint.x, vSecondPoint.y, vSecondPoint.z, 1.f,
			vSecondPoint.x, vSecondPoint.y, vSecondPoint.z, 1.f
		};
	}

	m_pEyeCurve->Set_ControlPoints(matPoints);
	m_CameraEyeList.push_back(static_cast<CCutScene_Curve*>(m_pEyeCurve));

	return S_OK;
}

HRESULT CCamera_CutScene::Add_At_Curve(_vec4 vFirstPoint, _vec4 vSecondPoint)
{
	_uint	iCurrentLevel = m_pGameInstance->Get_CurrentLevelIndex();
	_uint	iCutSceneType = CCutScene_Curve::SECTION_TYPE_EYE;

	SectionInfo SectionInfo;
	SectionInfo.vStartCutScene = vFirstPoint;
	SectionInfo.vEndCutScene = vSecondPoint;

	wstring strSectionEyeName = CamInfo.strName + L"_Curve_At_" + to_wstring(m_CameraAtList.size());
	m_pAtCurve = static_cast<CCutScene_Curve*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Camera_Curve"), &SectionInfo));



	_mat	matPoints;
	ZeroMemory(&matPoints, sizeof _mat);

	/* First Section */
	if (m_CameraAtList.empty() || m_CameraAtList.size() == 1)
	{
		matPoints = {
			vFirstPoint.x, vFirstPoint.y, vFirstPoint.z, 1.f,
			vFirstPoint.x, vFirstPoint.y, vFirstPoint.z, 1.f,
			vSecondPoint.x, vSecondPoint.y, vSecondPoint.z, 1.f,
			vSecondPoint.x, vSecondPoint.y, vSecondPoint.z, 1.f
		};
	}
	/* Not First Section */
	else if (!m_CameraAtList.empty() && m_CameraAtList.size() != 1)
	{
		_int iIndex = (_int)m_CameraAtList.size() - 2;
		if (iIndex < 0)
		{
			return E_FAIL;
		}

		_mat matPrePoints;
		static_cast<CCutScene_Curve*>(m_CameraAtList[iIndex])->Get_ControlPoints(&matPrePoints);

		matPoints = {
			matPrePoints._11, matPrePoints._12, matPrePoints._13, matPrePoints._14,
			vFirstPoint.x, vFirstPoint.y, vFirstPoint.z, 1.f,
			vSecondPoint.x, vSecondPoint.y, vSecondPoint.z, 1.f,
			vSecondPoint.x, vSecondPoint.y, vSecondPoint.z, 1.f
		};
	}

	m_pAtCurve->Set_ControlPoints(matPoints);
	m_CameraAtList.push_back(static_cast<CCutScene_Curve*>(m_pAtCurve));

	return S_OK;
}
HRESULT	CCamera_CutScene::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Camera"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}
HRESULT	CCamera_CutScene::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
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

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	return S_OK;
}



CCamera_CutScene* CCamera_CutScene::Create(_dev pDevice, _context pContext)
{
	CCamera_CutScene* pInstance = new CCamera_CutScene(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_CutScene::Clone(void* pArg)
{
	CCamera_CutScene* pInstance = new CCamera_CutScene(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_CutScene::Free()
{
	__super::Free();
	//Safe_Release(m_pImGui_Manager);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pEyeCurve);
	Safe_Release(m_pAtCurve);
}