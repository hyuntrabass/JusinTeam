#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Wearable_Slot.h"


BEGIN(Client)
class CWearable_Slot;
class CShop final : public COrthographicObject
{

private:
	CShop(_dev pDevice, _context pContext);
	CShop(const CShop& rhs);
	virtual ~CShop() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

private:
	_bool										m_isPrototype{ false };
	_bool										m_isActive{ false };

	CGameObject*								m_pMoney{ nullptr };
	CGameObject*								m_pDiamond{ nullptr };
	CGameObject*								m_pTitleButton{ nullptr };
	CGameObject*								m_pExitButton{ nullptr };
	CGameObject*								m_pBackGround{ nullptr };
	CGameObject*								m_pSeigeLine{ nullptr };
	CGameObject*								m_pInvenFrame{ nullptr };

public:
	_bool IsActive() { return m_isActive; }
	HRESULT Set_Item(ITEM eItem, _uint iNum = 1);
	void Open_Shop();

private:
	void Init_ShopState();

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CShop* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END