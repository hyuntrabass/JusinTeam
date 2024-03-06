#pragma once

#include "Client_Define.h"
#include "GameObject.h"


BEGIN(Client)

struct LeverInfo {
	_uint iIndex{};
	_mat mMatrix{};
};

class CLever final :
    public CGameObject
{
private:
	CLever(_dev pDevice, _context pContext);
	CLever(const CLever& rhs);
	virtual ~CLever() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CCollider* m_pBodyColliderCom = nullptr;
	CCollider* m_pWideColliderCom = nullptr;

	_float m_fCollectTime{};
	_float m_fTime{};
	_float m_fDir{ 1.f };

	class CTextButtonColor* m_pBar{ nullptr };


	_bool m_isOn = false;
	_bool m_isAllDone = false;
	_bool m_isInteracting = false;
	_bool m_isWideCollision = false;

	class CTextButtonColor* m_pBar{ nullptr };
	class C3DUITex* m_pSpeechBubble{ nullptr };
	CGameObject* m_pBG{ nullptr };
	CGameObject* m_pNameTag{ nullptr };

private:
	LeverInfo m_Info;
	ANIM_DESC m_Animation{};
	_uint m_ShaderPassIndex = 0;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	HRESULT Add_Collider();
	void Update_Collider();

public:
	static CLever* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END