#pragma once
#include "AnimTool_Define.h"
#include "GameObject.h"

BEGIN(AnimTool)

class CPlayer final : public CGameObject
{
private:
	CPlayer(_dev pDevice, _context pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	void Set_ModelIndex(_uint iModelIndex) {
		m_iCurrentIndex = iModelIndex;
	}

public:
	virtual HRESULT Init_Prototype(_uint iNumModels);
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel** m_pModelCom{ nullptr };

private:
	_float4 m_vPos{};
	_float m_fGravity{};
	_uint m_iNumModels = { 0 };
	wstring* m_pModelName = {};
	_uint m_iCurrentIndex = { 0 };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(_dev pDevice, _context pContext, _uint iNumModels);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END