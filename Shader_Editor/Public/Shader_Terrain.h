#pragma once
#include "GameObject.h"
#include "Shader_Define.h"

BEGIN(ShaderEdit)

class CShader_Terrain final :
    public CGameObject
{
public:
	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };

private:
	CShader_Terrain(_dev pDevice, _context pContext);
	CShader_Terrain(const CShader_Terrain& rhs);
	virtual ~CShader_Terrain() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	// ��ü�� ������Ʈ�� ����� �� �����ϰԲ� ��������� �����Ѵ�.
	CRenderer* m_pRendererCom = { nullptr };
	CTexture* m_pTextureCom[TYPE_END] = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };

private:
	_float				m_fIndex = { 0.f };
	_float				m_fSizeX, m_fSizeY;
	_float				m_fX, m_fY;
	_float44			m_ViewMatrix, m_ProjMatrix;
	_uint				m_iTextureNum = { 0 };
	//_uint2				m_vTerrainSize{};
	TERRAIN_INFO			m_TerrainInfo;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	// ���� ��ü�� ������ ��
	static CShader_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	// �ǻ�밴ü�� ������ �� : ������ü�� ȣ���Ͽ� �纻(�ǻ�밴ü)�� �����Ѵ� 
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END