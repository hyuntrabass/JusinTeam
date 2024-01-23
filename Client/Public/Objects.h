#pragma once
#include "Client_Define.h"
#include "BlendObject.h"



BEGIN(Client)

class CObjects abstract : public CBlendObject
{
protected:
	CObjects(_dev pDevice, _context pContext);
	CObjects(const CObjects& rhs);
	virtual ~CObjects() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	ObjectInfo Get_Info() const { return m_Info; }

protected:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
protected:

	ObjectInfo m_Info{};
	_bool m_isSelected{};
	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};

protected:
	HRESULT Add_Components(wstring strPrototype, ObjectType eType);
	HRESULT Bind_ShaderResources();


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END