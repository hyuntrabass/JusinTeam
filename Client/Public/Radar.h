#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Wearable_Slot.h"


BEGIN(Client)
class CWearable_Slot;
class CRadar final : public COrthographicObject
{

private:
	CRadar(_dev pDevice, _context pContext);
	CRadar(const CRadar& rhs);
	virtual ~CRadar() = default;

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
	_bool						m_isPrototype{ false };
	_bool						m_isActive{ true };

	_float						m_fTime{};
	_float						m_fAngleCamera{};
	_float						m_fAnglePlayer{};
	class CTextButton*			m_pRadar{ nullptr };
	CTextButton*				m_pButton{ nullptr };
	CTextButton*				m_pPlayerIcon{ nullptr };

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CRadar* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END