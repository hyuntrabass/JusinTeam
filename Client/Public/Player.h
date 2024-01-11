#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

enum PART_TYPE
{
	PT_HEAD,
	PT_FACE,
	PT_UPPER_BODY,
	PT_LOWER_BODY,
	PT_END
};

struct BODYPART_DESC
{
	PART_TYPE eType{};
	_uint iNumVariations{};


	ANIM_DESC* Animation{};
	CTransform* pParentTransform{ nullptr };
};

class CPlayer final : public CGameObject
{
private:
	CPlayer(_dev pDevice, _context pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };

private:
	ANIM_DESC m_Animation{};
	_float4 m_vPos{};
	_float m_fGravity{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END