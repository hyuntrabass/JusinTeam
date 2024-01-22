#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

struct TRAIL_DESC
{
	_float4 vColor{};
	_float2 vPSize{};
	_uint iNumVertices{};
};

class CCommonTrail final : public CBlendObject
{
private:
	CCommonTrail(_dev pDevice, _context pContext);
	CCommonTrail(const CCommonTrail& rhs);
	virtual ~CCommonTrail() = default;

public:
	void On();
	void Off();

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_float3 vPos);
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

	CVIBuffer_Trail* m_pTrailBufferCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

private:
	_bool m_bNoRender{};

	list<_float3> m_TrailPosList{};
	TRAIL_DESC m_Info{};
	_float3* m_PosArray{};
	_float4* m_ColorArray{};
	_float2* m_PSizeArray{};


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CCommonTrail* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END