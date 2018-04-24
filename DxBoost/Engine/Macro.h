#pragma once
#ifndef _MACRO_H
#define _MACRO_H
//¸¨ÖúµÄºê
#define HRR(X) {if(FAILED(X)) { MessageBox(0,L"Create Failed",0,0); return false;}}
#define ReleaseCOM(x) { if (x) { x->Release(); x = 0; } }

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                        \
	{                                                \
		HRESULT hr = (x);                            \
		if(FAILED(hr))                               \
		{                                            \
			MessageBox(0,L"Create Failed",0,0);      \
		}                                            \
	}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

#define SafeDelete(x) { delete x; x = 0; }

#endif // !_MACRO_H
