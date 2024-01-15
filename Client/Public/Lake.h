#pragma once

#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Client)

class CLake final : public CGameObject
{

public:
	typedef struct tagWaterDesc {
		_vec3 vPos{};
		_vec2 vSize{};
		_float fReflectionScale = 0.f;
		_float fRefractionScale = 0.f;
		_float fWaterPos = 0.f;
		_float fWaterSpeed = 0.f;
	}WATER_DESC;
private:
	CLake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLake(const CLake& rhs);
	virtual ~CLake() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	// ��ü�� ������Ʈ�� ����� �� �����ϰԲ� ��������� �����Ѵ�.
	CRenderer* m_pRendererCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };


private:
	_mat m_ReflectionViewMatrix{};

	WATER_DESC m_Desc{};

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	// ���� ��ü�� ������ ��
	static CLake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// �ǻ�밴ü�� ������ �� : ������ü�� ȣ���Ͽ� �纻(�ǻ�밴ü)�� �����Ѵ� 
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
