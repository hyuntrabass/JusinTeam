#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Wearable_Slot.h"


BEGIN(Client)
class CWearable_Slot;
class CSkillBook final : public COrthographicObject
{
public:
	enum SKILL_TYPE { SNIPER, ASSASSIN, TYPE_END };
private:
	CSkillBook(_dev pDevice, _context pContext);
	CSkillBook(const CSkillBook& rhs);
	virtual ~CSkillBook() = default;

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
	SKILL_TYPE									m_ePrevType{ SNIPER };
	SKILL_TYPE									m_eCurType{ SNIPER };
	_bool										m_isPrototype{ false };
	_bool										m_bNewSkillIn{ false };
	_bool										m_isActive{ false };

	CGameObject*								m_pMoney{ nullptr };
	CGameObject*								m_pDiamond{ nullptr };
	CGameObject*								m_pNotify{ nullptr };
	CGameObject*								m_pTitleButton{ nullptr };
	CGameObject*								m_pExitButton{ nullptr };
	CGameObject*								m_pBackGround{ nullptr };

	CGameObject*								m_pUnderBar{ nullptr };
	CGameObject*								m_pSelectButton{ nullptr };
	CGameObject*								m_pSkillType[TYPE_END];

	vector<class CSkillDesc*>					m_vecSkillDesc[TYPE_END];


private:
	void Init_SkillBookState(); 
	HRESULT Init_SkillDesc(); 


private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSkillBook* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END