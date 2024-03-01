#include "BrickBall.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "GameInstance.h"
#include "Collider.h"
#include "UI_Manager.h"
#include "BrickBar.h"
#include "CommonSurfaceTrail.h"
CBrickBall::CBrickBall(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrickBall::CBrickBall(const CBrickBall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBrickBall::Init_Prototype()
{
	return S_OK;
}

HRESULT CBrickBall::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_fSpeed = 9.f;
	m_pTransformCom->Set_Scale(_vec3(2.f, 2.f, 2.f));
	m_pTransformCom->Set_Speed(m_fSpeed);

	
	_vec3 vPos = ((BALL_DESC*)pArg)->vPos;

	m_pTransformCom->Set_Position(vPos);
	_vec3 vNormal = _vec3(0.f, 0.f, -1.f);
	m_pTransformCom->LookAt_Dir(vNormal);

	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	/*
	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball_Init");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
	*/
	//m_vDir = _vec4(0.f, 0.f, -1.f, 0.f);

	m_shouldRenderBlur = true;
	m_eCurBrickColor = BLUE;


	if (FAILED(Init_Effect()))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CBrickBall::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scale(_vec3(1.3f, 1.3f, 1.3f));


	m_fSpeed = 12.f;
	Set_BallColor();

	CUI_Manager::Get_Instance()->Set_BrickBallColor(m_eCurBrickColor);

	m_fX += fTimeDelta;

	_vec2 vCenterPos = _vec2(-2000.70496f, -1999.06152f);
	_vec2 vSize = _vec2(32.f, 30.f);
	RECT rcRect = {
		  (LONG)(vCenterPos.x - vSize.x * 0.5f),
		  (LONG)(vCenterPos.y - vSize.y * 0.5f),
		  (LONG)(vCenterPos.x + vSize.x * 0.5f),
		  (LONG)(vCenterPos.y + vSize.y * 0.5f)
	};
	POINT vCurPos = { (_long)m_pTransformCom->Get_State(State::Pos).x, (_long)m_pTransformCom->Get_State(State::Pos).z };
	if (!PtInRect(&rcRect, vCurPos))
	{
		m_isDead = true;
	}



	if (m_pGameInstance->Key_Down(DIK_UP, InputChannel::GamePlay))
	{
		m_iBallColor++;
		if (m_iBallColor > (_uint)COLOR_END)
		{
			m_iBallColor = 0;
		}
	}
	//RayCast();
	Check_Collision(fTimeDelta);
	m_pTransformCom->Set_Speed(m_fSpeed);
	m_pTransformCom->Go_Straight(fTimeDelta);

	//_vec4 vPos = m_pTransformCom->Get_State(State::Pos) + m_vDir * fTimeDelta * m_fSpeed;
	//m_pTransformCom->Set_State(State::Pos, vPos);

	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));


	/*
	if (m_pEffect_Ball)
	{
		m_pEffect_Ball->Tick(fTimeDelta);
	}
	*/
	_mat UpMatrix{};
	_mat	BottomMatrix{};
	if (m_pTrail != nullptr)
	{
		m_pTrail->Set_Color(m_vColor);
		m_pTrail->On();
		m_pDistortionTrail->On();
		if (m_pTransformCom->Get_State(State::Look).z < 0.f)
		{
			BottomMatrix = _mat::CreateTranslation(0.15f, 0.f, -0.6f) * m_pTransformCom->Get_World_Matrix();
			UpMatrix = _mat::CreateTranslation(-0.15f, 0.f, -0.6f) * m_pTransformCom->Get_World_Matrix();
		}
		else
		{
			BottomMatrix = _mat::CreateTranslation(0.15f, 0.f, -0.8f) * m_pTransformCom->Get_World_Matrix();
			UpMatrix = _mat::CreateTranslation(-0.15f, 0.f, -0.8f) * m_pTransformCom->Get_World_Matrix();
		}
		
		m_pTrail->Tick(UpMatrix.Position_vec3(), BottomMatrix.Position_vec3());

		BottomMatrix = _mat::CreateTranslation(0.2f, -0.3f, 0.f) * m_pTransformCom->Get_World_Matrix();;
		UpMatrix = _mat::CreateTranslation(-0.2f, -0.3f, 0.f) * m_pTransformCom->Get_World_Matrix();
		m_pDistortionTrail->Tick(UpMatrix.Position_vec3(), BottomMatrix.Position_vec3());
	}
}

void CBrickBall::Late_Tick(_float fTimeDelta)
{
	m_pTrail->Late_Tick(fTimeDelta);
	//m_pDistortionTrail->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	//if (m_pEffect_Ball)
	//{
	//	m_pEffect_Ball->Late_Tick(fTimeDelta);
	//}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBrickBall::Render()
{
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

		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof _vec4)))
		{
			return E_FAIL;
		}

		_bool isBlur = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlur", &isBlur, sizeof _bool)))
		{
			return E_FAIL;
		}

		_vec2 vUV = _vec2(m_fX, 0.f);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &vUV, sizeof _vec2)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect)))
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

HRESULT CBrickBall::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f);
	CollDesc.fRadius = 0.4f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CBrickBall::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}


void CBrickBall::RayCast()
{
	_float fDist = 5.f;
	PxRaycastBuffer Buffer{};

	if (m_pGameInstance->Raycast(m_pTransformCom->Get_State(State::Pos), 
		m_pTransformCom->Get_State(State::Look).Get_Normalized(),
		fDist, Buffer))
	{
		_vec3 vNormal = _vec3(PxVec3ToVector(Buffer.block.normal));
		_vec3 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();
		vNormal.Normalize();
		m_vDir = _vec3::Reflect(vLook, vNormal);
		m_vDir.y = 0.f;
		m_pTransformCom->LookAt_Dir(m_vDir);


		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball_Smoke");
		Info.pMatrix = &m_EffectMatrix;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

	}

}

HRESULT CBrickBall::Init_Effect()
{
	SURFACETRAIL_DESC Desc{};
	Desc.vColor = _color(1.f, 0.5f, 0.1f, 1.f);

	Desc.iNumVertices = 12;
	m_pTrail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	Desc.iPassIndex = 2;
	Desc.strMaskTextureTag = L"FX_J_Noise_Normal004_Tex";
	m_pDistortionTrail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	
	m_Mat = &m_pTransformCom->Get_World_Matrix();
	_mat UpMatrix{};
	_mat	BottomMatrix{};
	if (m_pTrail != nullptr)
	{
		m_pTrail->Set_Color(m_vColor);
		m_pTrail->On();
		BottomMatrix = _mat::CreateTranslation(0.1f, 0.f, 0.f) * m_pTransformCom->Get_World_Matrix();
		UpMatrix = _mat::CreateTranslation(-0.1f, 0.f, 0.f) * m_pTransformCom->Get_World_Matrix();
		m_pTrail->Tick(UpMatrix.Position_vec3(), BottomMatrix.Position_vec3());

		BottomMatrix = _mat::CreateTranslation(0.1f, 0.f, 0.f) * m_pTransformCom->Get_World_Matrix();;
		UpMatrix = _mat::CreateTranslation(0.1f, 0.f, 0.f) * m_pTransformCom->Get_World_Matrix();
		m_pDistortionTrail->Tick(UpMatrix.Position_vec3(), BottomMatrix.Position_vec3());


	}
	return S_OK;
}

void CBrickBall::Check_Collision(_float fTimeDelta)
{
	m_isCombo = false;
	Update_Collider();

	CCollider* pCollider{ nullptr };
	_bool isColl{};
	_uint iWall{};
	for (iWall = 0; iWall < 3; iWall++)
	{
		pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_Wall"), TEXT("Com_Collider_BrickWall"), iWall);
		if (pCollider == nullptr)
		{
			break;
		}
		if (m_pColliderCom->Intersect(pCollider))
		{
			isColl = true;
			break;
		}
	}

	if (isColl && m_pCurCollider != pCollider)
	{
		m_isCombo = true;
		m_pCurCollider = nullptr;
		m_pCurCollider = pCollider;
		_vec3 vNormal{};
		if (iWall == 0)
		{
			vNormal = _vec4(-1.f, 0.f, 0.f, 0.f);
		}
		else if (iWall == 1)
		{
			vNormal = _vec4(0.f, 0.f, -1.f, 0.f);
		}
		else if (iWall == 2)
		{
			vNormal = _vec4(1.f, 0.f, 0.f, 0.f);
		}
		else if (iWall == 3)
		{
			vNormal = _vec4(1.f, 0.f, 0.f, 0.f);
		}
		_vec3 vLook = m_pTransformCom->Get_State(State::Look);
		vNormal.Normalize();
		m_vDir = _vec3::Reflect(vLook, vNormal);
		m_vDir.y = 0.f;
		m_pTransformCom->LookAt_Dir(m_vDir);
	}


	CCollider* pBarCollider{ nullptr };

	pBarCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_BrickBar"), TEXT("Com_Collider_Bar"));
	if (pBarCollider != nullptr)
	{
		if (m_pColliderCom->Intersect(pBarCollider) && m_pCurCollider != pBarCollider)
		{
			m_isCombo = true;
			m_pCurCollider = nullptr;
			m_pCurCollider = pBarCollider;
			_vec3 vLook = m_pTransformCom->Get_State(State::Look);
			_vec3 vNormal = pBarCollider->Get_Normal(m_pGameInstance->Get_CollideFace(pBarCollider, m_pColliderCom));


			if (CUI_Manager::Get_Instance()->Get_BarDir() == BAR_LEFT)
			{
				vNormal = _vec4(-0.2f, 0.f, 1.f, 0.f);
			}
			else if (CUI_Manager::Get_Instance()->Get_BarDir() == BAR_RIGHT)
			{
				vNormal = _vec4(0.2f, 0.f, 1.f, 0.f);
			}

			vNormal.Normalize();
			m_vDir = _vec3::Reflect(vLook, vNormal);
			m_vDir.y = 0.f;
			m_pTransformCom->LookAt_Dir(m_vDir);
			m_eCurBrickColor = (BrickColor)m_iBallColor;
		}

	}


	CCollider* pBalloonCollider{ nullptr };
	_bool isBalloonColl{};
	_uint iNum = m_pGameInstance->Get_LayerSize(LEVEL_TOWER, TEXT("Layer_Balloons"));
	for (_uint i = 0; i < iNum; i++)
	{
		pBalloonCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_Balloons"), TEXT("Com_Collider_Sphere"), i);
		if (pBalloonCollider == nullptr)
		{
			break;
		}
		if (m_pColliderCom->Intersect(pBalloonCollider))
		{
			isBalloonColl = true;
			break;
		}
	}

	if (isBalloonColl && m_pCurCollider != pBalloonCollider)
	{
		m_isCombo = true;
		m_pCurCollider = nullptr;
		m_pCurCollider = pBalloonCollider;
		_vec3 vLook = m_pTransformCom->Get_State(State::Look);
		_vec3 vNormal = pBalloonCollider->Get_Normal(m_pGameInstance->Get_CollideFace(pBalloonCollider, m_pColliderCom));
		vNormal.Normalize();
		m_vDir = _vec3::Reflect(vLook, vNormal);
		m_vDir.y = 0.f;
		m_pTransformCom->LookAt_Dir(m_vDir);
	}


	CCollider* pMonCollider{ nullptr };

	pMonCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_BlackCat"), TEXT("Com_Collider_Bar"));
	if (pMonCollider != nullptr)
	{
		if (m_pColliderCom->Intersect(pMonCollider) && m_pCurCollider != pMonCollider)
		{
			m_isCombo = true;
			m_pCurCollider = nullptr;
			m_pCurCollider = pMonCollider;
			_vec3 vLook = m_pTransformCom->Get_State(State::Look);
			_vec3 vNormal = pMonCollider->Get_Normal(m_pGameInstance->Get_CollideFace(pMonCollider, m_pColliderCom));
			vNormal.Normalize();
			m_vDir = _vec3::Reflect(vLook, vNormal);
			m_vDir.y = 0.f;
			m_pTransformCom->LookAt_Dir(m_vDir);
			m_eCurBrickColor = (BrickColor)m_iBallColor;
		}
	}
}

void CBrickBall::Set_BallColor()
{

	switch (m_eCurBrickColor)
	{
	case PINK:
		m_vColor = _vec4(1.f, 0.56f, 0.93f, 1.f);
		break;
	case YELLOW:
		m_vColor = _vec4(0.94f, 0.77f, 0.2f, 1.f);
		break;
	case PURPLE:
		m_vColor = _vec4(0.63f, 0.4f, 0.9f, 1.f);
		break;
	case BLUE:
		m_vColor = _vec4(0.f, 0.6f, 1.f, 1.f);
		break;
	case COLOR_END:
		break;
	default:
		break;
	}
}

HRESULT CBrickBall::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Effect"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Effect_FX_A_SphereBuff002_SM.mo"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FX_A_Fractal006_Tex"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	m_pEffect_Ball = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	
	return S_OK;
}

HRESULT CBrickBall::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBrickBall* CBrickBall::Create(_dev pDevice, _context pContext)
{
	CBrickBall* pInstance = new CBrickBall(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBrickBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrickBall::Clone(void* pArg)
{
	CBrickBall* pInstance = new CBrickBall(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBrickBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrickBall::Free()
{
	__super::Free();

	Safe_Release(m_pTrail);
	Safe_Release(m_pDistortionTrail);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pEffect_Ball);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
