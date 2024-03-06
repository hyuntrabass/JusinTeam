#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CBrickItem final : public CGameObject
{
public:
	enum TYPE { POWER, DOUBLE, STOP, TYPE_END};
	typedef struct tagBalloonDesc
	{
		TYPE eType{};
		_vec4 vPos{};
	}BRICKITEM_DESC;

private:
	CBrickItem(_dev pDevice, _context pContext);
	CBrickItem(const CBrickItem& rhs);
	virtual ~CBrickItem() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pBodyColliderCom = { nullptr };

private:
	TYPE m_eType{};
	_float m_fDir{ 1.f };
	_vec4 fStartPos{};
	_mat m_EffectMat{};
	class CEffect_Dummy* m_pEffect{ nullptr };

private:
	HRESULT Add_Collider();
	void Update_BodyCollider();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBrickItem* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END