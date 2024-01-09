#pragma once


namespace Engine
{
	/* ���α׷����� ���Ǵ� ���� Ÿ�Ե��� �������Ѵ�.  */
	using _char = char;
	using _tchar = wchar_t;

	using _ubyte = unsigned char;
	using _byte = signed char;

	using _ushort = unsigned short;
	using _short = signed short;

	using _uint = unsigned int;
	using _int = signed int;

	using _ulong = unsigned long;
	using _long = signed long;

	using _float = float;
	using _double = double;

	using _bool = bool;

	using _lint = LARGE_INTEGER;

	using _int2 = XMINT2;
	using _int3 = XMINT3;
	using _int4 = XMINT4; 
	using _uint2 = XMUINT2;
	using _uint3 = XMUINT3;
	using _uint4 = XMUINT4;
	using _float2 = XMFLOAT2;
	using _float3 = XMFLOAT3;
	using _float4 = XMFLOAT4;
	using _vector = XMVECTOR;
	using _fvector = FXMVECTOR;
	using _gvector = GXMVECTOR;

	using _float44 = XMFLOAT4X4;
	using _matrix = XMMATRIX;
	using _fmatrix = FXMMATRIX;
	using _cmatrix = CXMMATRIX;

	using _dev = ID3D11Device*;
	using _context = ID3D11DeviceContext*;

	using _randNum = std::mt19937_64;
	using _randFloat = std::uniform_real_distribution<float>;

	using CUTSCENE = std::vector<std::pair<XMFLOAT4, XMFLOAT4>>;
}
