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
	RG_Water_Refraction,
	RG_Water_Reflection,
	RG_Water,
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

	//원명 다운 스케일링
	THPARAM m_THParam;

	ID3D11Buffer* m_pDSBuffer = nullptr;

	ID3DBlob* m_pDSBlob = nullptr;

	ID3D11ComputeShader* m_pDSCShader = nullptr;

	ID3D11Texture2D* m_pDSTexture = nullptr;

	ID3D11UnorderedAccessView* m_pDSUAV = nullptr;

	ID3D11ShaderResourceView* m_pDSSRV = nullptr;

	// 원명 블러	
	BLURPARAM m_BLParam;

	ID3D11Buffer* m_pBlurBuffer = nullptr;

	ID3DBlob* m_pBlurBlob = nullptr;

	ID3D11ComputeShader* m_pBlurCShader = nullptr;

	ID3D11Texture2D* m_pBlurTexture = nullptr;

	ID3D11UnorderedAccessView* m_pBlurUAV = nullptr;

	ID3D11ShaderResourceView* m_pBlurSRV = nullptr;


	_float fWidth = 0.f;
	_float fHeight = 0.f;


private:
	_vec3 m_vRandom[16]{};
	_bool m_TurnOnSSAO = false;
	_bool m_TurnOnToneMap = false;
	_bool m_TurnOnBlur = false;
	_bool m_Thunder = false;
	SSAO_DESC m_SSAO;

public:
	SSAO_DESC Get_SSAO() const {
		return m_SSAO;
	}

	void Set_SSAO(SSAO_DESC Desc) {
		m_SSAO = Desc;
	}


private:
	HRESULT Ready_ShadowDSV();

	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_Refraction();
	HRESULT Render_Reflection();
	HRESULT Render_Water();
	HRESULT Render_Blur();
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_BlendBlur();
	HRESULT Render_HDR();
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