#include "EyeBomb.h"
#include "Effect_Manager.h"
#include "Effect_Dummy.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
CEyeBomb::CEyeBomb(_dev pDevice, _context pContext)
	:CVTFMonster(pDevice, pContext)
{
}

CEyeBomb::CEyeBomb(const CEyeBomb& rhs)
	:CVTFMonster(rhs)
{
}

HRESULT CEyeBomb::Init_Prototype()
{
	return S_OK;
}

HRESULT CEyeBomb::Init(void* pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_Animation = {};
	m_Animation.iAnimIndex = Anim_Attack03;
	m_Animation.bSkipInterpolation = true;
	m_Animation.fAnimSpeedRatio = 1.5f;

	_mat EffectMatrix = _mat::CreateScale(10.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() *_mat::CreateTranslation(_vec3(0.f,0.1f,0.f));

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
	Info.pMatrix = &EffectMatrix;
	m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Dim");
	Info.pMatrix = &EffectMatrix;
	m_pDimEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
	m_fBaseEffectScale = 0.1f;
	EffectMatrix =  _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.2f, 0.f));
	m_BaseOriEffectMat = EffectMatrix;
	m_BaseEffectMat = EffectMatrix;
	Info.pMatrix = &m_BaseEffectMat;
	Info.isFollow = true;
	m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	return S_OK;
}

void CEyeBomb::Tick(_float fTimeDelta)
{
	if (m_bIsCollision)
	{
		if (m_fBaseEffectScale >= 0.f)
		{
			m_fBaseEffectScale -= fTimeDelta;
			m_BloodTextrue->Set_Alpha(m_fBaseEffectScale / 4.f);
			m_BloodTextrue->Tick(fTimeDelta);
			
		}
		else
		{
			Kill();
		}
		return;
	}
	m_fBaseEffectScale += fTimeDelta *6.f;

	if (m_fBaseEffectScale > 10.f)
	{
		
		if (m_pGameInstance->Attack_Player(m_pBodyColliderCom, rand() % 20 + 100, MonAtt_Hit))
		{
			if (CUI_Manager::Get_Instance()->Get_Hp().x <= 0)
			{
				Kill();
				return;
			}
			CTextButtonColor::TEXTBUTTON_DESC ButtonDesc = {};
			ButtonDesc.eLevelID = LEVEL_TOWER;
			ButtonDesc.fDepth = (_float)D_ALERT / (_float)D_END;
			ButtonDesc.strText = TEXT("");
			int Randnum = rand() % 3;
			ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_EyeBomb_Blood") + std::to_wstring(Randnum);
			_randInt RandomPosX(-400, 400);
			_randInt RandomPosY(-200, 200);
			_uint iRandomX = RandomPosX(m_RandomNumber);
			_uint iRandomY = RandomPosY(m_RandomNumber);

			ButtonDesc.vPosition = _vec2((_float)(g_ptCenter.x + iRandomX), (_float)(g_ptCenter.y + iRandomY));
			ButtonDesc.vSize = _vec2(1800.f, 1200.f);
			ButtonDesc.fAlpha = 1.f;
			
			m_BloodTextrue = dynamic_cast<CTextButtonColor*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ButtonDesc));
			if (not m_BloodTextrue)
			{
				return;
			}
			m_BloodTextrue->Set_Pass(VTPass_UI_Alpha);
			m_bIsCollision = true;
			m_fBaseEffectScale = 4.f;

			m_pGameInstance->Play_Sound(TEXT("Sfx_Mon_Niflheim_egg_Explosion_Die_01-01"), 0.5f, false, 0.2f);
		}
		else
		{
			Kill();

			m_pGameInstance->Play_Sound(TEXT("Sfx_Mon_Niflheim_egg_Explosion_Die_01-01"), 0.5f, false, 0.2f);
		}
		
	}

	m_BaseEffectMat = _mat::CreateScale(m_fBaseEffectScale) * m_BaseOriEffectMat;

	if (m_pBaseEffect)
	{
		m_pBaseEffect->Tick(fTimeDelta);
	}
	if (m_pDimEffect)
	{
		m_pDimEffect->Tick(fTimeDelta);
	}
	if (m_pFrameEffect)
	{
		m_pFrameEffect->Tick(fTimeDelta);
	}
	__super::Tick(fTimeDelta);

	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());

	if (m_isDead)
	{
		//m_pGameInstance->Play_Sound(TEXT("SD_4062_FireBall_SFX_01"));

		CEffect_Manager* pEffect_Manager = CEffect_Manager::Get_Instance();
		Safe_AddRef(pEffect_Manager);
		EffectInfo Info{};

		_mat EffectMat{ _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos))) };

		Info = pEffect_Manager->Get_EffectInformation(L"EyeBomb_Smoke");
		Info.pMatrix = &EffectMat;
		pEffect_Manager->Add_Layer_Effect(Info);

		Info = pEffect_Manager->Get_EffectInformation(L"EyeBomb_Particle");
		Info.pMatrix = &EffectMat;
		pEffect_Manager->Add_Layer_Effect(Info);

		Safe_Release(pEffect_Manager);
	}
}

void CEyeBomb::Late_Tick(_float fTimeDelta)
{
	if (m_bIsCollision)
	{
		m_BloodTextrue->Late_Tick(fTimeDelta);
		return;
	}
	if (m_pBaseEffect)
	{
		m_pBaseEffect->Late_Tick(fTimeDelta);
	}
	if (m_pDimEffect)
	{
		m_pDimEffect->Late_Tick(fTimeDelta);
	}
	if (m_pFrameEffect)
	{
		m_pFrameEffect->Late_Tick(fTimeDelta);
	}
	__super::Late_Tick(fTimeDelta);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif // DEBUG
}

HRESULT CEyeBomb::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CEyeBomb::Render_Instance()
{
	__super::Render_Instance();

	return S_OK;
}


HRESULT CEyeBomb::Add_Components()
{
	Collider_Desc ColliderDesc{};
	ColliderDesc.eType = ColliderType::Sphere;
	ColliderDesc.vCenter = _vec3(0.f, 0.5f, 0.f);
	ColliderDesc.fRadius = 3.6f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pBodyColliderCom), &ColliderDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CEyeBomb* CEyeBomb::Create(_dev pDevice, _context pContext)
{
	CEyeBomb* pInstance = new CEyeBomb(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEyeBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEyeBomb::Clone(void* pArg)
{
	CEyeBomb* pInstance = new CEyeBomb(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEyeBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEyeBomb::Free()
{
	__super::Free();
	Safe_Release(m_BloodTextrue);
	Safe_Release(m_pDimEffect);
	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pBaseEffect);
}
