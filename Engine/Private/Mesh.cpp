#include "Mesh.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Shader.h"

CMesh::CMesh(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
	, m_pVerticesPos(rhs.m_pVerticesPos)
	, m_pVerticesNor(rhs.m_pVerticesNor)
	, m_pIndices(rhs.m_pIndices)
	, m_iMatIndex(rhs.m_iMatIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_BoneIndices(rhs.m_BoneIndices)
	, m_OffsetMatrices(rhs.m_OffsetMatrices)
	, m_BoneMatrices(rhs.m_BoneMatrices)
	, m_eType(rhs.m_eType)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CMesh::Init_Prototype(ModelType eType, ifstream& ModelFile, _fmatrix OffsetMatrix)
{
	m_eType = eType;
	ModelFile.read(reinterpret_cast<_char*>(&m_iMatIndex), sizeof _uint);

	_uint iNameSize{};
	ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
	if (iNameSize >= sizeof(m_szName))
	{
		MSG_BOX("Name Is Too Long!");
	}
	ModelFile.read(m_szName, iNameSize);
	ModelFile.read(reinterpret_cast<_char*>(&m_iNumVertices), sizeof _uint);
	_uint iNumFaces{};
	ModelFile.read(reinterpret_cast<_char*>(&iNumFaces), sizeof _uint);

	m_pVerticesPos = new _float3[m_iNumVertices]{};
	m_pVerticesNor = new _float3[m_iNumVertices]{};

	m_iNumVertexBuffers = 1;

	m_iIndexStride = 4;
	m_iNumIndices = iNumFaces * 3;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex
	switch (m_eType)
	{
	case Engine::ModelType::Static:
	case Engine::ModelType::Collision:
		Ready_StaticMesh(ModelFile, OffsetMatrix);
		break;
	case Engine::ModelType::Anim:
		Ready_AnimMesh(ModelFile);
		break;
	}
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

	_ulong* pIndices = new _ulong[m_iNumIndices]{};
	m_pIndices = new _ulong[m_iNumIndices]{};

	_ulong dwIndex{};
	for (size_t i = 0; i < iNumFaces; i++)
	{
		ModelFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
		m_pIndices[dwIndex++] = pIndices[dwIndex];
		ModelFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
		m_pIndices[dwIndex++] = pIndices[dwIndex];
		ModelFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
		m_pIndices[dwIndex++] = pIndices[dwIndex];
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		return E_FAIL;
	}
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CMesh::Init(void* pArg)
{
	if (m_eType == ModelType::Collision)
	{
		m_pActor = m_pGameInstance->Cook_StaticMesh(m_iNumVertices, m_pVerticesPos, m_iNumIndices, m_pIndices);
		if (not m_pActor)
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const vector<CBone*>& Bones, const _char* pVariableName, _fmatrix PivotMatrix)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedMatrix()) * PivotMatrix);
	}

	if (FAILED(pShader->Bind_Matrices(pVariableName, m_BoneMatrices, m_iNumBones)))
	{
		MSG_BOX("Failed to Bind Bone Matrices!");
		return E_FAIL;
	}

	return S_OK;
}

_bool CMesh::Intersect_RayMesh(_fmatrix WorldMatrix, _float4* pPickPos)
{
	_uint Index[3]{};
	_float3 vPickPos{};

	m_pGameInstance->TransformRay_ToLocal(WorldMatrix);

	for (_uint i = 0; i < m_iNumIndices / 3; i++)
	{
		Index[0] = i * 3;
		Index[1] = (i * 3) + 1;
		Index[2] = (i * 3) + 2;

		_vector Vertices[3]
		{
			XMLoadFloat3(&m_pVerticesPos[m_pIndices[Index[0]]]),
			XMLoadFloat3(&m_pVerticesPos[m_pIndices[Index[1]]]),
			XMLoadFloat3(&m_pVerticesPos[m_pIndices[Index[2]]])
		};

		//if (!BoundingFrustum(m_pGameInstance->Get_Transform(D3DTS::Proj)).Intersects(Vertices[0], Vertices[1], Vertices[2]))
		//{
		//	return false;
		//}
		_vector Normal = XMVector3Normalize(XMLoadFloat3(&m_pVerticesNor[m_pIndices[Index[0]]]) +
											XMLoadFloat3(&m_pVerticesNor[m_pIndices[Index[1]]]) +
											XMLoadFloat3(&m_pVerticesNor[m_pIndices[Index[2]]]));

		if (m_pGameInstance->Picking_InLocal(Vertices[0],
											 Vertices[1],
											 Vertices[2],
											 Normal, pPickPos))
		{
			return true;
		}
	}

	return false;
}

void CMesh::Apply_TransformToActor(_fmatrix WorldMatrix)
{
	if (not m_pActor)
	{
		return;
	}
	_vector vQuat = XMQuaternionRotationMatrix(WorldMatrix);
	PxTransform Transform(VectorToPxVec3(WorldMatrix.r[3]), PxQuat(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]));
	m_pActor->setGlobalPose(Transform);
}

HRESULT CMesh::Ready_StaticMesh(ifstream& ModelFile, _fmatrix OffsetMatrix)
{
	m_iVertexStride = sizeof VTXSTATICMESH;
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	VTXSTATICMESH* pVertices = new VTXSTATICMESH[m_iNumVertices]{};

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vPosition), sizeof _float3);
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), OffsetMatrix));
		m_pVerticesPos[i] = pVertices[i].vPosition;

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vNormal), sizeof _float3);
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), OffsetMatrix));
		m_pVerticesNor[i] = pVertices[i].vNormal;

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vTexcoord), sizeof _float2);

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vTangent), sizeof _float3);
	}


	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);


	return S_OK;
}

HRESULT CMesh::Ready_AnimMesh(ifstream& ModelFile)
{
	ModelFile.read(reinterpret_cast<_char*>(&m_iNumBones), sizeof _uint);
	if (m_iNumBones > 256)
	{
		MSG_BOX("Too Many Bones!");
	}
	m_BoneMatrices = new _float44[m_iNumBones];

	m_iVertexStride = sizeof VTXANIMMESH;
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices]{};

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vPosition), sizeof _float3);
		m_pVerticesPos[i] = pVertices[i].vPosition;

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vNormal), sizeof _float3);
		m_pVerticesNor[i] = pVertices[i].vNormal;

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vTexcoord), sizeof _float2);

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vTangent), sizeof _float3);

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vBlendIndices), sizeof XMUINT4);

		ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vBlendWeights), sizeof _float4);
	}


	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		_float44 OffsetMatrix{};
		_uint iBoneIndex{};
		ModelFile.read(reinterpret_cast<_char*>(&OffsetMatrix), sizeof _float44);
		ModelFile.read(reinterpret_cast<_char*>(&iBoneIndex), sizeof _uint);

		m_OffsetMatrices.push_back(OffsetMatrix);
		m_BoneIndices.push_back(iBoneIndex);
	}

	return S_OK;
}

CMesh* CMesh::Create(_dev pDevice, _context pContext, ModelType eType, ifstream& ModelFile, _fmatrix OffsetMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(eType, ModelFile, OffsetMatrix)))
	{
		MSG_BOX("Failed to Create : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	if (m_hasCloned)
	{
		if (m_eType == ModelType::Collision)
		{
			m_pActor->release();
		}
	}
	else
	{
		Safe_Delete_Array(m_BoneMatrices);
		Safe_Delete_Array(m_pVerticesPos);
		Safe_Delete_Array(m_pVerticesNor);
		Safe_Delete_Array(m_pIndices);
	}
}
