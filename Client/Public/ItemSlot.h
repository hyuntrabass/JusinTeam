#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CItemSlot final : public COrthographicObject
{
public:
	typedef struct tagItemSlotDesc
	{
		_float2 vSize;
		_float2 vPosition;
	}ITEMSLOT_DESC;

private:
	CItemSlot(_dev pDevice, _context pContext);
	CItemSlot(const CItemSlot& rhs);
	virtual ~CItemSlot() = default;

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
	_bool			m_isFull{ false };
	_float			m_fTime{};
	_float			m_fCoolTime{};
	CGameObject*	m_pItem{ nullptr };

public:
	const _bool& Is_Full() const { return m_isFull; }

private: 
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CItemSlot* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END