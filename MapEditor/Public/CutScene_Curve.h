#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"
#include "CutScene_Point.h"
#include "ImGui_Manager.h"


BEGIN(MapEditor)

class CCutScene_Curve final : public CGameObject
{
private:
	CCutScene_Curve(_dev pDevice, _context pContext);
	CCutScene_Curve(const CCutScene_Curve& rhs);
	virtual ~CCutScene_Curve() = default;
public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	void Select(const _bool& isSelected);
	void Set_Dead() { m_isDead = true; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


private:
	CRenderer* m_pRendererCom{};
	CShader* m_pShaderCom{};
	CModel* m_pModelCom {};
	CVIBuffer_Curve* m_pVIBuffer{};

	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};
	_bool m_isSelected{};

private:
	void Set_Points();
	CCutScene_Point* m_pStartPoint{};
	CCutScene_Point* m_pEndPoint{};
	CameraInfo Info{};
	_mat m_WorldMatrix{};

public:
	static CCutScene_Curve* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END