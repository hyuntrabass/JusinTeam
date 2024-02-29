#pragma once

#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Client)

class CHook final : public CGameObject
{
public:
	typedef struct tagLogDesc
	{
		_mat WorldMatrix{};
		_vec4 vLookat{};
	}HOOK_DESC;

private:
	CHook(_dev pDevice, _context pContext);
	CHook(const CHook& rhs);
	virtual ~CHook() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Get_Dragging() { return m_bDragging; }
	_bool Get_HadCollision() { return m_bHadCollision; }
	void Set_Dragging(_bool bDrag) {m_bDragging = bDrag;}
	_vec4 Get_Position();
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pBodyColliderCom = { nullptr };
	CTexture* m_pDissolveTextureCom = { nullptr };

private:
	_float m_fLifeTime{};
	_bool m_bDragging{};
	_bool m_bHadCollision{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHook* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END