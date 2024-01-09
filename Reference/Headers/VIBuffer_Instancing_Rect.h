#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing_Rect final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Instancing_Rect(_dev pDevice, _context pContext);
	CVIBuffer_Instancing_Rect(const CVIBuffer_Instancing_Rect& rhs);
	virtual ~CVIBuffer_Instancing_Rect() = default;

public:
	virtual HRESULT Init_Prototype(_uint iNumInstances);
	virtual HRESULT Init(void* pArg) override;

public:
	static CVIBuffer_Instancing_Rect* Create(_dev pDevice, _context pContext, _uint iNumInstances);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END