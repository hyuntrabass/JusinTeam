#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
	CLight();
	virtual ~CLight() = default;

public:
	LIGHT_DESC* Get_LightDesc();


public:
	HRESULT Init(const LIGHT_DESC& LightDeesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Bind_ViewProjMatrix(class CShader* pShader, const _char* pViewVariableName, const _char* pProjVariableName);

private:
	LIGHT_DESC m_LightDesc{};

public:
	static CLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

END