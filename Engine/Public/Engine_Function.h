#pragma once

namespace Engine
{
	/**
	\brief �̳� Ŭ������ �ε����� �� �� �ְ� uint�� ĳ���� ���ִ� �Լ�

	\param[in]  ENUM: �̳� Ŭ������ ���Ҹ� �ֽ��ϴ�.
	\return unsigned int�� ĳ���õ� ���� ����.
	*/
	template<class T>
	inline constexpr unsigned int ToIndex(const T& ENUM)
	{
		return static_cast<unsigned int>(ENUM);
	}

	template<typename T>
	void Safe_Delete(T& Temp)
	{
		if (Temp)
		{
			delete Temp;
			Temp = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_AddRef(T& Temp)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != Temp)
		{
			dwRefCnt = Temp->AddRef();
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_Release(T& Temp)
	{
		unsigned long dwRefCnt = 0;

		if (Temp)
		{
			dwRefCnt = Temp->Release();
			if (0 == dwRefCnt)
				Temp = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	void Safe_Delete_Array(T& Temp)
	{
		if (Temp)
		{
			delete[] Temp;
			Temp = nullptr;
		}
	}

	/**
	\brief �ΰ��� ���Ҹ� �������� �մϴ�.
	\brief �翬�� float3������ �ȵ�. ��ǻ� float, double��.
	\brief ���ʹ� �ɰ� �����ѵ� ����? XMVectorLerp�� �̿� �սô�.

	\param[in] A: ���� ��.
	\param[in] B: �� ��.
	\param[in] Alpha: 0 ~ 1 ������ ��. 0�� ����� ���� A�� ����� ��, 1�� �������� B�� ����� ���� ���ɴϴ�. Alpha ���� 0 ���� 1 ���̷� �ڵ� Ŭ���� ��.
	\return ���� ������ ��.
	*/
	template<typename T>
	T Lerp(T A, T B, float Alpha)
	{
		if (Alpha < 0.f)
		{
			Alpha = 0.f;
		}
		if (Alpha > 1.f)
		{
			Alpha = 1.f;
		}
		return A * (1.f - Alpha) + B * Alpha;
	}

	/**
	\brief XMVECTOR�� PxVec3�� ��ȯ�մϴ�. w�� �����.

	\param[in]  vVector: ���͸� ��������.
	\return  PxVec3�� ��ȯ�� ���� ����.
	*/
	const PxVec3 ENGINE_DLL VectorToPxVec3(FXMVECTOR vVector);

	/**
	\brief PxExtendedVec3�� XMVECTOR�� ��ȯ�մϴ�.

	\param[in]  Src: ��ȯ�ϰ� ���� ���� ��������.
	\param[in]  w: w���� �ְ� ������ ��������. �⺻���� 0.
	\return  XMVECTOR�� ��ȯ�� ���� ����.
	*/
	const XMVECTOR ENGINE_DLL PxExVec3ToVector(PxExtendedVec3 Src, float w = 0.f);

	/**
	\brief PxExtendedVec3�� PxVec3�� ��ȯ�մϴ�.

	\param[in]  Src: ��ȯ�ϰ� ���� ���� ��������.
	\return  PxVec3�� ��ȯ�� ���� ����.
	*/
	const PxVec3 ENGINE_DLL PxExVec3ToPxVec3(PxExtendedVec3 Src);

	/**
	\brief PxVec3�� XMVECTOR�� ��ȯ�մϴ�.

	\param[in]  Src: ��ȯ�ϰ� ���� ���� ��������.
	\param[in]  w: w���� �ְ� ������ ��������. �⺻���� 0.
	\return  XMVECTOR�� ��ȯ�� ���� ����.
	*/
	const XMVECTOR ENGINE_DLL PxVec3ToVector(PxVec3 Src, float w = 0.f);

	/**
	\brief PxVec3�� PxExtendedVec3�� ��ȯ�մϴ�.

	\param[in]  Src: ��ȯ�ϰ� ���� ���� ��������.
	\return  PxExtendedVec3�� ��ȯ�� ���� ����.
	*/
	const PxExtendedVec3 ENGINE_DLL PxVec3ToPxExVec3(PxVec3 Src);

	/**
	\brief PxVec3�� float3�� ��ȯ�մϴ�.

	\param[in]  Src: ��ȯ�ϰ� ���� ���� ��������.
	\return  float3�� ��ȯ�� ���� ����.
	*/
	const _float3 ENGINE_DLL PxVec3ToFloat3(PxVec3 Src);
}
