#include "VIBuffer_Instancing_Point.h"

CVIBuffer_Instancing_Point::CVIBuffer_Instancing_Point(_dev pDevice, _context pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Instancing_Point::CVIBuffer_Instancing_Point(const CVIBuffer_Instancing_Point& rhs)
	: CVIBuffer_Instancing(rhs)
{
}

HRESULT CVIBuffer_Instancing_Point::Init_Prototype(_uint iNumInstances)
{
	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof VTXPOINT;
	m_iNumVertices = 1;

	m_iNumInstances = iNumInstances;
	m_iIndexCountPerInstance = 1;
	m_iInstanceStride = sizeof VTXINSTANCING;

	m_iIndexStride = 2;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstances;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region Vertex
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	VTXPOINT* pVertex = new VTXPOINT;
	ZeroMemory(pVertex, sizeof(VTXPOINT) * m_iNumVertices);

	pVertex->vPosition = _float3(0.f, 0.f, 0.f);
	pVertex->vPSize = _float2(1.f, 1.f);

	m_InitialData.pSysMem = pVertex;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	Safe_Delete(pVertex);
#pragma endregion

#pragma region Index
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	_ushort* pIndices = new _ushort[m_iNumIndices]{};

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);
#pragma endregion

#pragma region Instance
	m_InstancingBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstancingBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstancingBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstancingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstancingBufferDesc.MiscFlags = 0;
	m_InstancingBufferDesc.StructureByteStride = m_iInstanceStride;

	m_InstancingInitialData = {};

	VTXINSTANCING* pVertexInstance = new VTXINSTANCING[m_iNumInstances]{};

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		pVertexInstance[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pVertexInstance[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pVertexInstance[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pVertexInstance[i].vPos = _float4(0.f + i, 0.f, 0.f + i, 1.f);
	}

	m_InstancingInitialData.pSysMem = pVertexInstance;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Instancing_Point::Init(void* pArg)
{
	random_device rand;
	_randNum RandomNumber(rand());

	VTXINSTANCING* pVertexInstance = reinterpret_cast<VTXINSTANCING*>(const_cast<void*>(m_InstancingInitialData.pSysMem));

	if (pArg)
	{
		ParticleDesc Desc = *reinterpret_cast<ParticleDesc*>(pArg);
		m_isLoop = Desc.isLoop;

		_randFloat RandomX = _randFloat(Desc.vMinPos.x, Desc.vMaxPos.x);
		_randFloat RandomY = _randFloat(Desc.vMinPos.y, Desc.vMaxPos.y);
		_randFloat RandomZ = _randFloat(Desc.vMinPos.z, Desc.vMaxPos.z);

		_randFloat RandomDirX = _randFloat(Desc.vMinDir.x, Desc.vMaxDir.x);
		_randFloat RandomDirY = _randFloat(Desc.vMinDir.y, Desc.vMaxDir.y);
		_randFloat RandomDirZ = _randFloat(Desc.vMinDir.z, Desc.vMaxDir.z);

		_randFloat RandomScale = _randFloat(Desc.vScaleRange.x, Desc.vScaleRange.y);
		_randFloat RandomSpeed = _randFloat(Desc.vSpeedRange.x, Desc.vSpeedRange.y);
		_randFloat RandomLifeTime = _randFloat(Desc.vLifeTime.x, Desc.vLifeTime.y);

		_randFloat RandomRadian = _randFloat(0.f, XMVectorGetX(g_XMPi) * 2.f);

		for (size_t i = 0; i < m_iNumInstances; i++)
		{
			_float fScale = RandomScale(RandomNumber);
			_float fCeta = RandomRadian(RandomNumber);

			pVertexInstance[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
			pVertexInstance[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
			pVertexInstance[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
			pVertexInstance[i].vPos = _float4(RandomX(RandomNumber), RandomY(RandomNumber), RandomZ(RandomNumber), 1.f);
			pVertexInstance[i].vOriginPos = pVertexInstance[i].vPos;
			pVertexInstance[i].fSpeed = RandomSpeed(RandomNumber);
			pVertexInstance[i].vLifeTime.y = RandomLifeTime(RandomNumber);
			XMStoreFloat4(&pVertexInstance[i].vDirection, XMVector3Normalize(XMVectorSet(RandomDirX(RandomNumber), RandomDirY(RandomNumber), RandomDirZ(RandomNumber), 0.f)));
		}
	}

	if (FAILED(m_pDevice->CreateBuffer(&m_InstancingBufferDesc, &m_InstancingInitialData, &m_pVBInstance)))
	{
		Safe_Delete_Array(m_InstancingInitialData.pSysMem);
		return E_FAIL;
	}

	return S_OK;
}

CVIBuffer_Instancing_Point* CVIBuffer_Instancing_Point::Create(_dev pDevice, _context pContext, _uint iNumInstances)
{
	CVIBuffer_Instancing_Point* pInstance = new CVIBuffer_Instancing_Point(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(iNumInstances)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Instancing_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Instancing_Point::Clone(void* pArg)
{
	CVIBuffer_Instancing_Point* pInstance = new CVIBuffer_Instancing_Point(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Instancing_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instancing_Point::Free()
{
	if (not m_hasCloned)
	{
		Safe_Delete_Array(m_InstancingInitialData.pSysMem);
	}
	__super::Free();
}
