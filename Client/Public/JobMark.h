#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CJobMark final : public COrthographicObject
{
public:
	enum JOBSTATE{ BOW, ASSASSIN, JOBSTATE_END };
private:
	CJobMark(_dev pDevice, _context pContext);
	CJobMark(const CJobMark& rhs);
	virtual ~CJobMark() = default;

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
	JOBSTATE m_eCurState{ BOW };
	JOBSTATE m_ePrevState{ BOW };

	CGameObject* m_pJob{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CJobMark* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END