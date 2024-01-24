#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCompute_Shader final :
    public CComponent
{
private:
    CCompute_Shader(_dev pDevice, _context pContext);
    CCompute_Shader(const CCompute_Shader& rhs);
    virtual ~CCompute_Shader() = default;

public:
    virtual HRESULT Init_Prototype(const wstring& strShaderFilePath, const string& strEntryPoint, _uint iDataSize, _uint2 iSize);
    virtual HRESULT Init(void* pArg);

public:
    HRESULT Set_Shader();

    // 시작하기
    HRESULT Begin(_uint3 ThreadGroupCount);

    HRESULT Change_Value(void* pArg, _uint iDataSize);

    // 입력할 텍스처, 출력된 텍스처
    HRESULT Bind_ShaderResourceView(ID3D11ShaderResourceView* pSRV, ID3D11UnorderedAccessView* pUAV);

    ID3D11ShaderResourceView* Get_SRV() const {
        return m_pSRV;
    }

private:
    _uint2 m_iSize{};

    ID3D11Buffer* m_pBuffer = nullptr;

    ID3DBlob* m_pBlob = nullptr;

    ID3D11ComputeShader* m_pShader = nullptr;

    ID3D11Texture2D* m_pTexture = nullptr;

    ID3D11ShaderResourceView* m_pSRV = nullptr;

    ID3D11UnorderedAccessView* m_pUAV = nullptr;

public:
    static CCompute_Shader* Create(_dev pDevice, _context pContext, const wstring& strShaderFilePath, const string& strEntryPoint, _uint iDataSize, _uint2 iSize);
    // CComponent을(를) 통해 상속됨
    virtual CComponent* Clone(void* pArg = nullptr) override;
    virtual void Free() override;
};

END