// dibpal.h
//header file for CDIBPal class.


#ifndef __DIBPAL__
#define __DIBPAL__

#include "../pch.h"

#include "dib.h"

class CDIBPalette : public CPalette
{
public:
    CDIBPalette();
    ~CDIBPalette();
    BOOL CreatePal(CDIBitmap* pDIBitmap);
	BOOL CreateIdentityPalette();
};

#endif // __DIBPAL__

