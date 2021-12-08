//dibpal.cpp
//implementation file for CDIBPal class


#include "dibpal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CDIBPalette::CDIBPalette()
{
}

CDIBPalette::~CDIBPalette()
{
}

BOOL CDIBPalette::CreatePal(CDIBitmap* pDIB)
{
//Create a palette from the color table in a CDIBitmap object..

    DWORD dwColors = pDIB->GetNumClrEntries();
    //Does the DIB have a color table?
    if (!dwColors)
    	{
		AfxMessageBox(L"No color table in this DIB");
        return FALSE;
    	}

    //Get a pointer to the RGB quads in the color table.
    RGBQUAD* pColorTable = pDIB->GetBMColorTabPtr();

    //Allocate a logical palette and fill it with the color table info.
    LOGPALETTE* pPal = (LOGPALETTE*) malloc(sizeof(LOGPALETTE) 
                     + dwColors * sizeof(PALETTEENTRY));
    if (!pPal)
    	{
		AfxMessageBox(L"Insufficient memory for the logical palette");
        return FALSE;
    	}
	//Specify a Windows 3.0 palette.
    pPal->palVersion = 0x300;       
    //Specify the table size.       
    pPal->palNumEntries = (WORD) dwColors;
    for (UINT i = 0; i < dwColors; i++)
    	{
        pPal->palPalEntry[i].peRed = pColorTable[i].rgbRed;
        pPal->palPalEntry[i].peGreen = pColorTable[i].rgbGreen;
        pPal->palPalEntry[i].peBlue = pColorTable[i].rgbBlue;
        pPal->palPalEntry[i].peFlags = 0;
    	}
    BOOL bResult = CreatePalette(pPal);
    free(pPal);
    return bResult;
}

BOOL CDIBPalette::CreateIdentityPalette()
{
    int i, NumSysColors, NumPalEntries;
    HPALETTE hpalOld;

    //Get screen DC.
    HWND hActiveWnd = ::GetActiveWindow();
    HDC hScreenDC = ::GetDC(hActiveWnd);
	
    //Get the number of system colors and the number of palette
    //entries. Note that the number of colors is actually the number
    //of guaranteed colors, that is, the number of reserved system colors.
    NumSysColors = GetDeviceCaps(hScreenDC, NUMCOLORS);
    NumPalEntries = GetDeviceCaps(hScreenDC, SIZEPALETTE);

    //Are there more than 256 colors? If so, quit.
    if (NumSysColors > 256)
		{
		::ReleaseDC(hActiveWnd, hScreenDC);
		return FALSE;
		}

	//By changing the system palette to have only 2 static colors, then
	//resetting it to 20 static colors, we force it to reset its tables
	//so that the next palette that is realized will have its colors placed
	//in the physical palette in the same order they are in the logical palette.
    SetSystemPaletteUse(hScreenDC, SYSPAL_NOSTATIC);
    SetSystemPaletteUse(hScreenDC, SYSPAL_STATIC);

    //Select and realize our palette so that its colors 
    //will be entered into the physical palette. Note that the
	//identifier m_hObject is a member of the GDIObject class, and
	//contains the handle of the object currently attached to the
	//GDI object.
    hpalOld = ::SelectPalette(hScreenDC, (HPALETTE)m_hObject, FALSE);
    ::RealizePalette(hScreenDC);		 
    //Select (but don't realize) the old palette.
    ::SelectPalette(hScreenDC, hpalOld, FALSE);

    //The physical palette now has the system colors at either end and 
    //the colors from our logical palette in the middle. We can retrieve the 
	//palette entries and put them in an array
    PALETTEENTRY pal_data[256];
    GetSystemPaletteEntries(hScreenDC, 0, NumPalEntries, pal_data);

    //Set the PC_NOCOLLAPSE flag for each of the logical palette colors
    //but *NOT* for the system colors.
    //First 10 system colors.
    for (i = 0; i < NumSysColors/2; i++) 
        pal_data[i].peFlags = 0;
    
	//Our colors.
    for (; i < NumPalEntries-NumSysColors/2; i++) 
        pal_data[i].peFlags = PC_NOCOLLAPSE;
    
    //Last 10 system colors.
    for (; i < NumPalEntries; i++) 
        pal_data[i].peFlags = 0;
    
    //Resize the palette  to be sure it has the full 256 entries.
    ResizePalette(NumPalEntries);

    //Update the logical palette entries with the entries that 
    //we read from the physical palette.
    SetPaletteEntries(0, NumPalEntries, pal_data);
    
    ::ReleaseDC(hActiveWnd, hScreenDC);
    return TRUE;
}

