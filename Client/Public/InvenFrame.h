#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Item.h"

BEGIN(Client)
class CItem;
class CInven;
class CInvenFrame final : public COrthographicObject
{
public:
	typedef struct tagInvenFrameDesc
	{
		CGameObject*			pParent;
		_float					fDepth;
		_float2					vSize;
		_float2					vPosition;
	}INVENFRAME_DESC;

private:
	CInvenFrame(_dev pDevice, _context pContext);
	CInvenFrame(const CInvenFrame& rhs);
	virtual ~CInvenFrame() = default;

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
	INVEN_TYPE									m_ePrevInvenType{ INVEN_ALL };
	INVEN_TYPE									m_eCurInvenType{ INVEN_ALL };
	_bool										m_isPrototype{ false };
	_bool										m_bNewItemIn{ false };
	_bool										m_isActive{ false };

	vector<_uint>								m_vecItemSlot[INVEN_TYPE::INVEN_END];
	vector<CItem*>								m_vecItems;

	CGameObject*								m_pParent{ nullptr };
	CGameObject*								m_pUnderBar{ nullptr };
	CGameObject*								m_pSelectButton{ nullptr };

	vector <CGameObject*>						m_pVerticalBar;
	CGameObject*								m_pInvenType[INVEN_TYPE::INVEN_END];

	CGameObject*								m_pBackGround{ nullptr };
	CGameObject*								m_pSelectSlot[5];

public:
	void Init_State();

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CInvenFrame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END