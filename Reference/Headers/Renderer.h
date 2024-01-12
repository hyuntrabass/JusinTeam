#pragma once
#include "Component.h"

BEGIN(Engine)

enum RenderGroup
{
	RG_Priority,
	RG_Shadow,
	RG_NonBlend,
	RG_Blur,
	RG_NonLight,
	RG_Blend,
	RG_BlendBlur,
	RG_UI,
	RG_Cursor,
	RG_End
};

class ENGINE_DLL CRenderer final : public CComponent
{
private:
	CRenderer(_dev pDevice, _context pContext);
	CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RenderGroup eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Draw_RenderGroup();

#ifdef _DEBUGTEST
	HRESULT Add_DebugComponent(class CComponent* pDebugComponent);
#endif // _DEBUG


private:
	list<class CGameObject*> m_RenderObjects[RG_End]{};

#ifdef _DEBUGTEST
	list<class CComponent*> m_DebugComponents{};
#endif // _DEBUG


private:
	class CVIBuffer_Rect* m_pVIBuffer{ nullptr };
	class CShader* m_pShader{ nullptr };

	_float44 m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

	ID3D11DepthStencilView* m_pShadowDSV{ nullptr };
	//ID3D11DepthStencilView* m_pBlurDSV{ nullptr };

private:
	HRESULT Ready_ShadowDSV();

	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_Blur();
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_BlendBlur();
	HRESULT Render_UI();
#ifdef _DEBUGTEST
private:
	HRESULT Render_Debug();
#endif // _DEBUG

public:
	static CRenderer* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END