#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Wearable_Slot.h"

BEGIN(Client)
class CWearable_Slot;
class CGraphicSetting final : public COrthographicObject
{
public:

	enum GRAPHICLIST { SSAO, TONE, BLOOM, SHADOW, MOTIONBLUR, FXAA, DOF, GODRAY, LIST_END };
	struct GRAPHICSETTING
	{
		GRAPHICLIST eList{};
		_vec2 vPos{};
	};
public:
	enum MENU { ENV, MENU_END };
	enum ENV_SLOT {GRAPHIC, SOUND,ENV_END };

private:
	CGraphicSetting(_dev pDevice, _context pContext);
	CGraphicSetting(const CGraphicSetting& rhs);
	virtual ~CGraphicSetting() = default;

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
	GRAPHICLIST					m_eGraphic{};
	_bool						m_isPrototype{ false };

	wstring						m_strText{};
	class CNineSlice*			m_pBackGround{ nullptr };
	CNineSlice*					m_pOn{ nullptr };
	CNineSlice*					m_pOff{ nullptr };

private:
	void Update_State(_bool isOn);

private:
	HRESULT Init_State();
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGraphicSetting* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END