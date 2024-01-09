#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
	CVIBuffer_Terrain(_dev pDevice, _context pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Init_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ);
	virtual HRESULT Init(void* pArg) override;

public:
	static CVIBuffer_Terrain* Create(_dev pDevice, _context pContext, _uint iNumVerticesX, _uint iNumVerticesZ);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END