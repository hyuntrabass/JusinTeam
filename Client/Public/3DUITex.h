#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class C3DUITex final : public COrthographicObject
{
public:
	typedef struct tagNameTagDesc
	{
		wstring strTexture;
		_vec2   vSize;
		_vec3	vPosition;
		class CTransform* pParentTransform;
		LEVEL_ID eLevelID;
	}UITEX_DESC;
private:
	C3DUITex(_dev pDevice, _context pContext);
	C3DUITex(const C3DUITex& rhs);
	virtual ~C3DUITex() = default;

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
	LEVEL_ID		m_eLevel{};
	wstring			m_strTexture;
	_vec2			m_vSize;
	_vec3			m_vPosition{};
	CTransform*		m_pParentTransform;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static C3DUITex* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END