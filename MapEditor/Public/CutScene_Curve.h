#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"
#include "CutScene_Point.h"
#include "ImGui_Manager.h"


BEGIN(MapEditor)

class CCutScene_Curve final : public CGameObject
{
public:
	enum SECTIONTYPE
	{
		SECTION_TYPE_EYE
		, SECTION_TYPE_AT
		, SECTION_TYPE_END
	};


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
	void Set_Dead() { m_isDead = true; }

	void Get_ControlPoints(_mat* pOutPoints);
	HRESULT Set_ControlPoints(_mat& Points);
	string Get_SectionName();

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
	_uint m_iSectionType = { SECTION_TYPE_END };

private:
	void Set_Points(SectionInfo Info);
	CCutScene_Point* m_pStartPoint{};
	CCutScene_Point* m_pEndPoint{};
	SectionInfo Info{};
	_mat m_matPoint{};
	wstring m_strSectionName{};

public:
	static CCutScene_Curve* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END