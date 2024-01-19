#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CPop_QuestEnd final : public COrthographicObject
{
private:
	CPop_QuestEnd(_dev pDevice, _context pContext);
	CPop_QuestEnd(const CPop_QuestEnd& rhs);
	virtual ~CPop_QuestEnd() = default;

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
	_float			m_fTime{};
	_float			m_fStartSize{};
	_float2			m_vRatio{};
	CGameObject*	m_pBackground{ nullptr };
	CGameObject*	m_pBorder{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPop_QuestEnd* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END