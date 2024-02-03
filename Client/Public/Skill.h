#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)
class CTextButtonColor;
class CTextButton;
class CSkill final : public COrthographicObject
{
public:
	typedef struct tagItemSlotDesc
	{
		_float					fDepth;
		_float2					vSize;
		_float2					vPosition;
		SKILLINFO				tSkillInfo;
	}SKILL_DESC;

private:
	CSkill(_dev pDevice, _context pContext);
	CSkill(const CSkill& rhs);
	virtual ~CSkill() = default;

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
	_bool					m_haveBG{ false };
	SKILLINFO				m_tSkillInfo{};
	RECT					m_rcRect{};
	
	//CTextButton*			m_pBorder{ nullptr };
	//CTextButtonColor*		m_pBackGround{ nullptr };

public:
	const SKILLINFO& Get_SkillInfo() const { return m_tSkillInfo; }
	void Set_Position(_vec2 vPos);
	const RECT& Get_Rect() const { return m_rcRect; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSkill* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END