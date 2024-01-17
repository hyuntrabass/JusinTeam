#pragma once
#include "AnimTool_Define.h"
#include "GameObject.h"

BEGIN(AnimTool)

class CPlayer final : public CGameObject
{
public:
	enum TYPE { TYPE_MONSTER, TYPE_PLAYER, TYPE_END };

private:
	CPlayer(_dev pDevice, _context pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	void Set_ModelIndex(_uint iModelIndex) {
		m_iCurrentIndex = iModelIndex;
	}

	_uint Get_ModelIndex() {
		return m_iCurrentIndex;
	}

	void Set_ModelType(TYPE eType) {
		m_eType = eType;
	}

	TYPE Get_ModelType() {
		return m_eType;
	}

	void Set_TimeDelta(_float fTimeDelta) {
		m_fTimeDelta = fTimeDelta;
	}

public:
	virtual HRESULT Init_Prototype(_uint iNumMonsterModels, _uint iNumPlayerModels);
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel** m_pMonsterModelCom{ nullptr };
	CModel** m_pPlayerModelCom{ nullptr };
	CModel* m_pModelCom{ nullptr };

private:
	_float4 m_vPos{};
	_float m_fGravity{};
	_uint m_iNumMonsterModels = { 0 };
	_uint m_iNumPlayerModels = { 0 };
	wstring* m_pMonsterModelTag = {};
	wstring* m_pPlayerModelTag = {};
	_uint m_iCurrentIndex = { 0 };
	TYPE m_eType = { TYPE_END };
	_float m_fTimeDelta = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(_dev pDevice, _context pContext, _uint iNumMonsterModels, _uint iNumPlayerModels);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END