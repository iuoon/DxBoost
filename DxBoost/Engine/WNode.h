#pragma once
#include<D3D11.h>
#include<D3DX11.h>
#include<D3Dcompiler.h>
#include <iostream>  
#include <stdlib.h>  
#include<math.h>  
#include <iomanip>   
#include <fstream>

using namespace std;

class WNode
{
public:
	WNode();
	~WNode();
	bool CompileD3DShader(LPCTSTR filePath, char* entry, char* shaderModel, ID3DBlob** buffer);

	template<class T>
	void Safe_Release(T t)
	{
		if (t != NULL)
		{
			t->Release();
			t = 0;
		}
	}
private:

};

