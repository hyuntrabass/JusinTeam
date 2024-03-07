#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Player.h"
#include "InfinityTower.h"
BEGIN(Client)

class CInfinityStart final : public COrthographicObject
{
public:
	typedef struct tagSummonWindowDesc
	{
		TOWER eTower{};

	}STARTGAME_DESC;
private:
	CInfinityStart(_dev pDevice, _context pContext);
	CInfinityStart(const CInfinityStart& rhs);
	virtual ~CInfinityStart() = default;

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
	_bool						m_bStartEffect{ false };

	TOWER						m_eTower{};
	wstring						m_strGameName{};

	_float						m_fDeadTime{};
	_float						m_fSmokeSize{500.f};
	_float						m_fStop{};
	_float						m_fTime{};
	_float						m_fBright{6.f};

	CGameObject*				m_pFade{ nullptr };
	class CTextButtonColor*		m_pPattern{ nullptr };
	CTextButtonColor*			m_pSmoke{ nullptr };

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CInfinityStart* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END