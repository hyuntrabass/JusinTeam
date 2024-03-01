#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)
class CTowerLazer final : public CGameObject
{
public:

private:
	CTowerLazer(_dev pDevice, _context pContext);
	CTowerLazer(const CTowerLazer& rhs);
	virtual ~CTowerLazer() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_vec4 Compute_PlayerPos();
	_vec4 Compute_PlayerLook();
	_float Compute_PlayerDistance();
	_vec4 Compute_Player_To_Dir();
private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;

	CCollider* m_pColliderCom = nullptr;
	CTexture* m_pDissolveTextureCom = nullptr;

	class CEffect_Dummy* m_pEffect{ nullptr };

private:
	EffectInfo Info{};
	_uint m_iPassIndex{};
	_float m_fDeadTime{ 0.f };
	_float m_fDissolveRatio{ 0.f };
	_float m_fIdleTime{ 0.f };
	_float m_fDetectTime{ 0.f };

	_bool m_bChangePass{ false };
	_bool m_bDamaged{ false };
	_bool m_isDetected{ false };
	_uint m_iIndex{ 0 };
	_mat EffectMatrix{};
	_vec4 m_vLook{};


public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	HRESULT Add_Collider();
	void Update_Collider();

public:
	static CTowerLazer* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END