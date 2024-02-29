#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSurvivalGame final : public CGameObject
{
public:
	enum STAGE
	{
		STAGE_INIT,
		STAGE01, // ������ġ�� ��ó �����Ǽ� 8�� �̻��� �߻�
		STAGE02, // �÷��̾� ��ġ�� ���� ��ġ�� �����Ǵ� ���߹�
		STAGE03, // �÷��̾� ���� ���ƿ��� ���� �̻���
		STAGE04, // ĳ���� ������
		STAGE05, // ���ķ� + ���ĸ�
		STAGE_END
	};

private:
	CSurvivalGame(_dev pDevice, _context pContext);
	CSurvivalGame(const CSurvivalGame& rhs);
	virtual ~CSurvivalGame() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Init_Pattern(_float fTimeDelta);
	void Tick_Pattern(_float fTimeDelta);

private:
	CRenderer* m_pRendererCom = { nullptr };

private:
	STAGE m_ePreStage = STAGE_END;
	STAGE m_eCurStage = STAGE_END;

private:
	_float m_fTime[2] = {};

private:
	_bool m_bGameStart = { false };

private:
	_uint m_iCount = {};

private:
	wstring m_strStage = {};

public:
	HRESULT Add_Components();

public:
	static CSurvivalGame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END