#pragma once
#include "Client_Define.h"
#include "Objects.h"
/*
	Wall_Torch
	Stand_Torch1
	Stand_Torch2
*/
BEGIN(Client)

class CTorch_Object final : public CObjects
{
private:
	CTorch_Object(_dev pDevice, _context pContext);
	CTorch_Object(const CTorch_Object& rhs);
	virtual ~CTorch_Object() = default;

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

	class CEffect_Dummy* m_pFire{ nullptr };
	_mat m_FireMat{};

public:
	static CTorch_Object* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END