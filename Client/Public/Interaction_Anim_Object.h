#pragma once
#include "Client_Define.h"
#include "Objects.h"
#include "Trigger_Manager.h"
//Interaction_Anim_Object -> 애니메이션 있는 채집 // Prototype_Model_GoldStone // Prototype_Model_SaltStone
BEGIN(Client)

class CInteraction_Anim final : public CGameObject
{
private:
	CInteraction_Anim(_dev pDevice, _context pContext);
	CInteraction_Anim(const CInteraction_Anim& rhs);
	virtual ~CInteraction_Anim() = default;

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
	CTrigger_Manager* m_pTrigger_Manager{ nullptr };
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
	_bool m_isAnimStart{false};
	_bool m_isWideCollision{false};
	_bool m_isCollect{};
	_float m_fCollectTime{};

	_float m_fTime{};
	_float m_fDir{1.f};

	_uint m_iObjectIndex{ 0 };

	class CTextButtonColor* m_pBar{ nullptr };
	class C3DUITex* m_pSpeechBubble{ nullptr };
	class CItem* m_pItem{ nullptr };
	CGameObject* m_pBG{ nullptr };
	CGameObject* m_pNameTag{ nullptr };


public:
	static CInteraction_Anim* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END