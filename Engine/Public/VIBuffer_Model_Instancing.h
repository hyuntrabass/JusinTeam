#pragma once
//#include "VIBuffer.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Model_Instancing final : public CComponent
{
protected:
	CVIBuffer_Model_Instancing(_dev pDevice, _context pContext);
	CVIBuffer_Model_Instancing(const CVIBuffer_Model_Instancing& rhs);
	virtual ~CVIBuffer_Model_Instancing() = default;

public:
	virtual HRESULT Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix, _uint iNumInstance);
	virtual HRESULT Init(void* pArg) override;
	virtual void Update(_float fTimeDelta, _mat WorldMatrix);
	virtual HRESULT Render() override;

	HRESULT Bind_Material(CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);

	void Apply_TransformToActor(_fmatrix WorldMatrix);
	_bool Intersect_RayModel(_fmatrix WorldMatrix, _vec4* pPickPos);
	HRESULT Read_Meshes(ifstream& File, _fmatrix PivotMatrix, _uint iNumInstance));
	HRESULT Read_Materials(ifstream& File, const string& strFilePath);
	class CTransform* m_pOwnerTransform{};

protected:
	D3D11_BUFFER_DESC m_InstancingBufferDesc{};
	D3D11_SUBRESOURCE_DATA m_InstancingInitialData{};

	ID3D11Buffer* m_pVBInstance{ nullptr };
	_char m_szFilePath[MAX_PATH] = "";

	_uint m_iNumInstances{};
	_uint m_iIndexCountPerInstance{};
	_uint m_iInstanceStride{};
	_uint m_iNumMeshes{};
	vector<class CVIBuffer_Instancing_Mesh*> m_InstancingMeshes{};
	_mat m_PivotMatrix{};

	_uint m_iNumMaterials{};
	vector<Model_Material> m_Materials{};
public:
	static CVIBuffer_Model_Instancing* Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END