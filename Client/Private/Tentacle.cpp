#include "Tentacle.h"

#include "Effect_Dummy.h"
#include "Effect_Manager.h"

CTentacle::CTentacle(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CTentacle::CTentacle(const CTentacle& rhs)
	: CMonster(rhs)
{
}

HRESULT CTentacle::Init_Prototype()
{
    return S_OK;
}

HRESULT CTentacle::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Groar_Tentacle");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_Animation.iAnimIndex = 0;
	m_Animation.isLoop = true;
	m_Animation.fAnimSpeedRatio = 3.f;
	m_Animation.fStartAimPos = 70.f; // 65 end

	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	m_pTransformCom->Set_Position(_vec3(vPlayerPos));

	_mat EffectMatrix = _mat::CreateScale(3.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateTranslation(_vec3(vPlayerPos) + _vec3(0.f, 0.1f, 0.f));
			
	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
	Info.pMatrix = &EffectMatrix;
	m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(&Info);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
	Info.pMatrix = &EffectMatrix;
	m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(&Info);

    return S_OK;
}

void CTentacle::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	if (m_fTime >= 1.5f)
	{
		if (m_pBaseEffect && m_pFrameEffect)
		{
			Safe_Release(m_pFrameEffect);
			Safe_Release(m_pBaseEffect);
		}


	}

	if (m_pFrameEffect)
	{
		m_pFrameEffect->Tick(fTimeDelta);
	}
	
	if (m_pBaseEffect)
	{
		m_pBaseEffect->Tick(fTimeDelta);
	}

	m_pModelCom->Set_Animation(m_Animation);
}

void CTentacle::Late_Tick(_float fTimeDelta)
{
	if (m_pFrameEffect)
	{
		m_pFrameEffect->Late_Tick(fTimeDelta);
	}

	if (m_pBaseEffect)
	{
		m_pBaseEffect->Late_Tick(fTimeDelta);
	}

	__super::Late_Tick(fTimeDelta);
}

HRESULT CTentacle::Render()
{
	__super::Render();

    return S_OK;
}

HRESULT CTentacle::Add_Collider()
{
    return S_OK;
}

void CTentacle::Update_Collider()
{
}

CTentacle* CTentacle::Create(_dev pDevice, _context pContext)
{
	CTentacle* pInstance = new CTentacle(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTentacle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTentacle::Clone(void* pArg)
{
	CTentacle* pInstance = new CTentacle(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTentacle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTentacle::Free()
{
	__super::Free();
}
