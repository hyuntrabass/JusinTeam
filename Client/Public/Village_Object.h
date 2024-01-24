#pragma once
#include "Client_Define.h"
#include "Objects.h"

BEGIN(Client)

class CVillage_Object final : public CObjects
{
private:
	CVillage_Object(_dev pDevice, _context pContext);
	CVillage_Object(const CVillage_Object& rhs);
	virtual ~CVillage_Object() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	ObjectInfo Get_Info() const { return m_Info; }


private:

	ObjectInfo m_Info{};
	_bool m_isSelected{};
	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};


public:
	static CVillage_Object* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END