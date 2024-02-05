#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CPop_Skill final : public COrthographicObject
{
public:
	typedef struct tagQuestInDesc
	{
		_uint	iSkillLevel;

	}SKILLIN_DESC;
private:
	CPop_Skill(_dev pDevice, _context pContext);
	CPop_Skill(const CPop_Skill& rhs);
	virtual ~CPop_Skill() = default;

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
	_bool			m_isMain{ false };

	_float			m_fDir{ -1.f };

	wstring			m_strQuestTitle;
	wstring			m_strText;


	_float			m_fIndex{};
	_float			m_fTime{};
	_float			m_fDeadTime{};
	_float			m_fButtonTime{};
	_float2			m_fStartButtonPos{};


	CGameObject*	m_pBackground{ nullptr };
	CGameObject*	m_pBorder{ nullptr };
	CGameObject*	m_pButton{ nullptr };
	/*
	
	class CTextButton* m_pLeft{ nullptr };
	CTextButton* m_pRight{ nullptr };
	CTextButton* m_pBoxBow{ nullptr };
	CTextButton* m_pBoxSword{ nullptr };
	class CTextButtonColor* m_pEffect{ nullptr };
	*/
private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPop_Skill* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END