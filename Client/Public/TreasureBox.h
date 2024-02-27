#pragma once
#include "Client_Define.h"
#include "Objects.h"
BEGIN(Client)

class CTreasureBox final : public CGameObject
{
public:
	enum DIR { LEFT, RIGHT, FRONT, BACK, DIR_END };
	typedef struct tagTreasureDesc
	{
		DIR eDir{};
		_vec4 vPos{};
		vector<pair<wstring, _uint>> vecItem;
	}TREASURE_DESC;

private:
	CTreasureBox(_dev pDevice, _context pContext);
	CTreasureBox(const CTreasureBox& rhs);
	virtual ~CTreasureBox() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Instance() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	PlaceType m_ePlaceType{ PLACE_END };
	ObjectInfo m_Info{};
	ANIM_DESC m_Animation{};

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CVTFModel* m_pModelCom{ nullptr };
	CCollider* m_pColliderCom{ nullptr };
	CCollider* m_pWideColliderCom{ nullptr };

private:
	 DIR  m_eDir{};
	_bool m_isJump{false};
	_bool m_isAnimStart{false};
	_bool m_isWideCollision{false};
	_bool m_isCollect{};
	_float m_fCollectTime{};

	_float m_fJumpTime{};
	_float m_fTime{};
	_float m_fDir{1.f};

	vector<pair<wstring, _uint>> m_vecItem;

	class CTextButtonColor* m_pBar{ nullptr };
	class C3DUITex* m_pSpeechBubble{ nullptr };
	class CItem* m_pItem{ nullptr };
	CGameObject* m_pBG{ nullptr };
	CGameObject* m_pNameTag{ nullptr };

public:
	static CTreasureBox* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END