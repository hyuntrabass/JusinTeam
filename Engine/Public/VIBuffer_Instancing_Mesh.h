//#pragma once
//#include "VIBuffer_Model_Instancing.h"
//
//BEGIN(Engine)
//
//class ENGINE_DLL CVIBuffer_Instancing_Mesh final : public CVIBuffer_Model_Instancing
//{
//private:
//	CVIBuffer_Instancing_Mesh(_dev pDevice, _context pContext);
//	CVIBuffer_Instancing_Mesh(const CVIBuffer_Instancing_Mesh& rhs);
//	virtual ~CVIBuffer_Instancing_Mesh() = default;
//
//public:
//	//virtual HRESULT Init_Prototype(vector<VTXSTATICMESH> m_Vertices, vector<_ulong> m_Indices, _uint iNumInstance);
//	virtual HRESULT Init_Prototype(const string& strFilePath, _uint iNumInstance);
//	virtual HRESULT Init(void* pArg) override;
//
//public:
//	//static CVIBuffer_Instancing_Mesh* Create(_dev pDevice, _context pContext, vector<VTXSTATICMESH> m_Vertices,	vector<_ulong> m_Indices, _uint iNumInstance);
//	static CVIBuffer_Instancing_Mesh* Create(_dev pDevice, _context pContext, const string& strFilePath, _uint iNumInstance);
//	virtual CComponent* Clone(void* pArg) override;
//	virtual void Free() override;
//};
//
//END