#pragma once
#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_BoneName() const;
	const _float44* Get_CombinedMatrix() const;
	const _float44& Get_Transformation() const;

	void Set_Transformation(_fmatrix TransformationMatrix);

private:
	HRESULT Init(ifstream& ModelFile);

private:
	_char m_szName[MAX_PATH]{};
	_float44 m_TransformationMatrix{};
	_float44 m_CombindTransformationMatrix{};

	_int m_iParentIndex{};

public:
	void Update_CombinedMatrix(const vector<CBone*>& Bones);

public:
	static CBone* Create(ifstream& ModelFile);
	CBone* Clone();
	virtual void Free() override;
};

END