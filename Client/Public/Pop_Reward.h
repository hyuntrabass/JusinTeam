#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CPop_Reward final : public COrthographicObject
{
public:
	typedef struct tagQuestEndDesc
	{
		vector<pair<wstring, _uint>> vecRewards;
	}REWARD_DESC;
private:
	CPop_Reward(_dev pDevice, _context pContext);
	CPop_Reward(const CPop_Reward& rhs);
	virtual ~CPop_Reward() = default;

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
	_float			m_fDir{ -1.f };

	wstring			m_strQuestTitle;


	_float			m_fTime{};
	_float			m_fDeadTime{};
	_float			m_fButtonTime{};
	_float2			m_fStartButtonPos{};

	CGameObject* m_pBackground{ nullptr };
	CGameObject* m_pBorder{ nullptr };
	CGameObject* m_pButton{ nullptr };
	CGameObject* m_pExclamationMark{ nullptr };

	vector<class CItem*> m_vecItems;

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPop_Reward* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END