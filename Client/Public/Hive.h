#pragma once

#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Client)

class CHive final : public CGameObject
{
public:
	typedef struct tagHiveDesc
	{
		_vec3 vPosition{};

	}HIVE_DESC;

private:
	CHive(_dev pDevice, _context pContext);
	CHive(const CHive& rhs);
	virtual ~CHive() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CTexture* m_pDissolveTextureCom = { nullptr };

private:
	_bool m_HasCreated{};
	_uint m_iPassIndex{};
	_float m_fDissolveRatio{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHive* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END