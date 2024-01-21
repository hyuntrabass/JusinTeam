#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CItem final : public COrthographicObject
{
public:
	typedef struct tagItemSlotDesc
	{
		_bool					bCanInteract;
		//CTransform* pParentTransform;
		_float					fDepth;
		_float2					vSize;
		_float2					vPosition;
		ITEM					eItemDesc;
		_uint					iNumItem = { 1 };
	}ITEM_DESC;

private:
	CItem(_dev pDevice, _context pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

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
	CTexture* m_pMaskTextureCom{ nullptr };

private:
	ITEM					m_eItemDesc;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CItem* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END