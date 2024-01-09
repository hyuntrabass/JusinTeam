#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(_dev pDevice, _context pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Init_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT Init(void* pArg);

public:
	HRESULT Begin(_uint iPassIndex);

	HRESULT Bind_ShaderResourceView(const _char* pVariableName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_ShaderResourceViewArray(const _char* pVariableName, ID3D11ShaderResourceView** pSRVs, _uint iNumTextures);

	HRESULT Bind_Matrix(const _char* pVariableName, const _float44& Matrix);
	HRESULT Bind_Matrices(const _char* pVariableName, const _float44* pMatrices, _uint iNumMatrices);

	HRESULT Bind_RawValue(const _char* pVariableName, const void* pData, _uint iDataSize);

private:
	ID3DX11Effect* m_pEffect{ nullptr };
	_uint m_iNumPasses{};
	vector<ID3D11InputLayout*> m_InputLayouts{};

public:
	static CShader* Create(_dev pDevice, _context pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END