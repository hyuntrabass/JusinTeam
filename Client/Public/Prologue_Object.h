#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

struct ObjectInfo
{
	wstring Prototype{};
	_mat m_Matrix{};
};


BEGIN(Client)

class CPrologue_Object final : public CBlendObject
{
private:
	CPrologue_Object(_dev pDevice, _context pContext);
	CPrologue_Object(const CPrologue_Object& rhs);
	virtual ~CPrologue_Object() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	ObjectInfo Get_Info() const { return m_Info; }

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
private:

	ObjectInfo m_Info{};
	_bool m_isSelected{};
	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CPrologue_Object* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END