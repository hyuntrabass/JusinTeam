#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Wearable_Slot.h"

#define MAX_ALPHA 0.75f
#define MAX_VOLUME 100
BEGIN(Client)
class CWearable_Slot;
class CSoundSetting final : public COrthographicObject
{
public:

	enum SOUNDLIST { ALL, BACKGROUND, ENV, EFFECT, LIST_END };
	struct SOUNDSETTING
	{
		SOUNDLIST eList{};
		_vec2 vPos{};
	};

private:
	CSoundSetting(_dev pDevice, _context pContext);
	CSoundSetting(const CSoundSetting& rhs);
	virtual ~CSoundSetting() = default;

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
	SOUNDLIST					m_eSound{};
	_bool						m_isPrototype{ false };
	_bool						m_isPickingButton{ false };
	_uint						m_iCurVolume{100};
	_float						m_fSound{};
	wstring						m_strText{};

	_vec2						m_vStartPos{};

	class CTextButtonColor*		m_pNumBar{ nullptr };
	class CTextButtonColor*		m_pNumBarBg{ nullptr };
	class CTextButton*			m_pNumButton{ nullptr };

private:
	void Update_State();

private:
	HRESULT Init_State();
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSoundSetting* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END