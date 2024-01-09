#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(_dev pDevice, _context pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RenderTargetView();

public:
	HRESULT Init(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor);
	void Clear();
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const _char* pVariableName);

#ifdef _DEBUGTEST
	HRESULT Ready_Debug(_float2 vPos, _float2 vSize);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG


private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

	ID3D11Texture2D* m_pTexture{ nullptr };
	ID3D11RenderTargetView* m_pRenderTargetView{ nullptr };
	ID3D11ShaderResourceView* m_pShaderResourceView{ nullptr };

	_float4 m_vClearColor{};

#ifdef _DEBUGTEST
	_float44 m_WorldMatrix{};
#endif // _DEBUG


public:
	static CRenderTarget* Create(_dev pDevice, _context pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor);
	virtual void Free() override;
};

END