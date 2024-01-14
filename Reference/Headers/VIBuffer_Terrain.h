#pragma once
#include "VIBuffer.h"

struct Info {
	_uint m_iNumVerticesX{};
	_uint m_iNumVerticesZ{};
};

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
	CVIBuffer_Terrain(_dev pDevice, _context pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	VTXNORTEX* Get_Vertices() { return pVertices; }

public:
	//_uint Get_NumVerticesX() { return m_iNumVerticesX; }
	//_uint Get_NumVerticesZ() { return m_iNumVerticesZ; }

private:
	VTXNORTEX* pVertices = { nullptr };
	Info m_pTerrain{};

private:
	_bool	m_isClone;
	
public:
	HRESULT ModifyTerrainVertexBuffer( _uint iNumVerticesX,  _uint iNumVerticesZ);


public:
	static CVIBuffer_Terrain* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END