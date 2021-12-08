// Image.cpp : implementation file
//

#include "../pch.h"
#include "CNEImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE 
static char THIS_FILE[] = __FILE__;
#endif

#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

/******************************************************
				생성자, 초기화 함수
******************************************************/

CNEImage::CNEImage()
{
	m_Size		 = CSize(1,1);
	m_hImage	 = NULL;
	m_pPal		 = NULL;
}

CNEImage::CNEImage( CNEImage &Src )
{
	m_Size		 = CSize(1,1);
	m_hImage	 = NULL;
	m_pPal		 = NULL;

	*this = Src;
}

CNEImage::~CNEImage()
{ 
	Free(); 
}

BOOL CNEImage::InitDIB(BOOL bCreatePalette)
{
	// 이미지의 가로, 세로 크기 설정
	LPSTR pDIB = (LPSTR) m_hImage;
	m_Size = CSize((int) ::DIBWidth(pDIB), (int) ::DIBHeight(pDIB));

	if(bCreatePalette)
	{
		if(m_pPal != NULL) delete m_pPal;
		// 팔레트 생성
		m_pPal = new CPalette;
		if (CreateDIBPalette() == NULL)
		{
			// 팔레트를 가지지 않는 경우
			delete m_pPal;
			m_pPal = NULL;
			return FALSE;
		}
	}
	return TRUE;
}

void CNEImage::SetHandle(HANDLE hHandle)
{
	m_hImage = (LPVOID)hHandle;
} 


LPBYTE CNEImage::GetImagePtr()
{
	LPBITMAPINFOHEADER lpBI;
	DWORD nSize;
	
	// 메모리 고정
	lpBI = (LPBITMAPINFOHEADER)m_hImage;
	nSize= lpBI->biSize + PaletteSize((LPSTR)lpBI);

	return (LPBYTE)((LPBYTE)m_hImage+nSize);
}


BOOL CNEImage::Create(int width, int height, int depth)
{
    LPBITMAPINFOHEADER lpbi ;
	BYTE		*lpPal;
    DWORD       dwSizeImage, nSize;
    int         i;

//	m_Size.cx=width;
//	m_Size.cy=height;

    dwSizeImage = height*(DWORD)((width*depth/8+3)&~3);

	if(depth == 24)
	{
		nSize=sizeof(BITMAPINFOHEADER)+dwSizeImage;
	}
    else
	{
		nSize=sizeof(BITMAPINFOHEADER)+dwSizeImage + 1024;
	}
	m_hImage= (LPVOID)new BYTE[nSize];

    if (m_hImage == NULL)	
        return FALSE;

	lpbi = (LPBITMAPINFOHEADER)m_hImage;
	lpbi->biSize            = sizeof(BITMAPINFOHEADER) ;
    lpbi->biWidth           = width;
    lpbi->biHeight          = height;
    lpbi->biPlanes          = 1;
    lpbi->biBitCount        = depth;
    lpbi->biCompression     = BI_RGB ;
    lpbi->biSizeImage       = dwSizeImage;
    lpbi->biXPelsPerMeter   = 0 ;
    lpbi->biYPelsPerMeter   = 0 ;
    lpbi->biClrUsed         = 0 ;
    lpbi->biClrImportant    = 0 ;

	lpPal = (BYTE *) lpbi;
	if (depth == 8)
	{
		lpbi->biClrUsed = 256;

		DWORD offDest = sizeof(BITMAPINFOHEADER);
		for(i = 0; i < 256; i++)
		{
			lpPal[offDest++] = (BYTE)i;
			lpPal[offDest++] = (BYTE)i;
			lpPal[offDest++] = (BYTE)i;
			lpPal[offDest++] = 0x00;
		}                  
	}
//bitmap depth4
	else if(depth == 4)
	{
		lpbi->biClrUsed = 32;

		DWORD offDest = sizeof(BITMAPINFOHEADER);
		for(i = 0; i < 32; i++)
		{
			lpPal[offDest++] = (BYTE)i;
			lpPal[offDest++] = (BYTE)i;
			lpPal[offDest++] = (BYTE)i;
			lpPal[offDest++] = 0x00;
		}                  
	}

	InitDIB(FALSE);
	return TRUE;
}

BOOL CNEImage::CreateDIBPalette()
{
	LPLOGPALETTE lpPal;      // pointer to a logical palette
	HANDLE hLogPal;          // handle to a logical palette
	HPALETTE hPal = NULL;    // handle to a palette
	int i;                   // loop index
	WORD wNumColors;         // number of colors in color table
	LPSTR lpbi;              // pointer to packed-DIB
	LPBITMAPINFO lpbmi;      // pointer to BITMAPINFO structure (Win3.0)
	LPBITMAPCOREINFO lpbmc;  // pointer to BITMAPCOREINFO structure (old)
	BOOL bWinStyleDIB;       // flag which signifies whether this is a Win3.0 DIB
	BOOL bResult = FALSE;

	/* if handle to DIB is invalid, return FALSE */

	if (m_hImage == NULL)
	  return FALSE;

   lpbi = (LPSTR)m_hImage;

   /* get pointer to BITMAPINFO (Win 3.0) */
   lpbmi = (LPBITMAPINFO)lpbi;

   /* get pointer to BITMAPCOREINFO (old 1.x) */
   lpbmc = (LPBITMAPCOREINFO)lpbi;

   /* get the number of colors in the DIB */
   wNumColors = ::DIBNumColors(lpbi);

   if (wNumColors != 0)
   {
		/* allocate memory block for logical palette */
		hLogPal = new BYTE[sizeof(LOGPALETTE)
									+ sizeof(PALETTEENTRY)
									* wNumColors];

		/* if not enough memory, clean up and return NULL */
		if (hLogPal == 0)
		{
			return FALSE;
		}

		lpPal = (LPLOGPALETTE) hLogPal;

		/* set version and number of palette entries */
		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD)wNumColors;

		/* is this a Win 3.0 DIB? */
		bWinStyleDIB = IS_WIN30_DIB(lpbi);
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else
			{
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}

		/* create the palette and get handle to it */
		bResult = m_pPal->CreatePalette(lpPal);
		
		delete hLogPal;
	}

	return bResult;
}

/******************************************************
				소멸자, 정리 함수
******************************************************/

void CNEImage::Free()
{
	if(m_hImage)
	{
		delete [] m_hImage ;
		m_hImage = NULL;
	}

	if(m_pPal)
	{
		delete m_pPal;
		m_pPal = NULL;
	}
}

/******************************************************
				이미지 정보를 얻는 함수
******************************************************/

int CNEImage::GetBitCount()
{
	if (m_hImage == NULL) return -1;
	LPBITMAPINFOHEADER lpbi;
	lpbi = (LPBITMAPINFOHEADER)m_hImage;
	return lpbi->biBitCount;
}



/******************************************************
						그리기
******************************************************/

BOOL CNEImage::Draw(HDC hDC, LPRECT lpDIBRect, LPRECT lpDCRect)
{
	LPSTR	lpDIBHdr;	// BITMAPINFOHEADER를 가리킬 포인터
	LPSTR	lpDIBBits;	// DIB 비트를 가리킬 포인터
	BOOL		bSuccess=FALSE;	 // Success/fail 플래그
	HPALETTE 	hPal=NULL;		 // DIB 팔레트
	HPALETTE 	hOldPal=NULL;	 // 이전 팔레트

	
	lpDIBHdr  = (LPSTR)m_hImage;
	// DIB 비트가 저장되어 있는 곳의 주소를 얻음
	lpDIBBits = ::FindDIBBits(lpDIBHdr);

	// 팔레트를 얻어 DC에 선택
	if(m_pPal != NULL)
	{
		hPal = (HPALETTE) m_pPal->m_hObject;
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}

	::SetStretchBltMode(hDC, COLORONCOLOR);

/* --------------------------------------------------------------------------------------
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
	   (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
		// 원래 크기로 그대로 출력하는 경우
		bSuccess = ::SetDIBitsToDevice(hDC, // hDC
			lpDCRect->left,		 			// DestX
			lpDCRect->top,		 			// DestY
			RECTWIDTH(lpDCRect),	 		// nDestWidth
			RECTHEIGHT(lpDCRect),			// nDestHeight
			lpDIBRect->left,		 		// SrcX
			(int)DIBHeight(lpDIBHdr) - lpDIBRect->top -	RECTHEIGHT(lpDIBRect),   		// SrcY
			0,                          	// nStartScan
			(WORD)DIBHeight(lpDIBHdr),  	// nNumScans
			lpDIBBits,                  	// lpBits
			(LPBITMAPINFO)lpDIBHdr,			// lpBitsInfo
			DIB_RGB_COLORS);				// wUsage
	else	// 확대 또는 축소하여 출력하는 경우
		bSuccess = ::StretchDIBits(hDC, 	// hDC
			lpDCRect->left,					// DestX
			lpDCRect->top,					// DestY
			RECTWIDTH(lpDCRect),			// nDestWidth
			RECTHEIGHT(lpDCRect),			// nDestHeight
			lpDIBRect->left,				// SrcX
			lpDIBRect->top,					// SrcY
			RECTWIDTH(lpDIBRect),			// wSrcWidth
			RECTHEIGHT(lpDIBRect),			// wSrcHeight
			lpDIBBits,						// lpBits
			(LPBITMAPINFO)lpDIBHdr,			// lpBitsInfo
			DIB_RGB_COLORS,					// wUsage
			SRCCOPY);						// dwROP
--------------------------------------------------------------------------------------------*/

	bSuccess = ::StretchDIBits(hDC, 	// hDC
		lpDCRect->left,					// DestX
		lpDCRect->top,					// DestY
		RECTWIDTH(lpDCRect),			// nDestWidth
		RECTHEIGHT(lpDCRect),			// nDestHeight
		lpDIBRect->left,				// SrcX
		lpDIBRect->top,					// SrcY
		RECTWIDTH(lpDIBRect),			// wSrcWidth
		RECTHEIGHT(lpDIBRect),			// wSrcHeight
		lpDIBBits,						// lpBits
		(LPBITMAPINFO)lpDIBHdr,			// lpBitsInfo
		DIB_RGB_COLORS,					// wUsage
		SRCCOPY);						// dwROP

	// DC 복원
	if (hOldPal != NULL) ::SelectPalette(hDC, hOldPal, TRUE);
	return bSuccess;
}


/******************************************************
				파일 읽어오기, 저장하기
******************************************************/

BOOL CNEImage::Save(LPCTSTR lpszFileName)
{
	CString filetype;
	filetype = lpszFileName;
	filetype.MakeUpper();

	if(filetype.Find(L".BMP") > -1) return SaveBMP(lpszFileName);
//	else if(filetype.Find(".TIF") > -1) return SaveTIF(lpszFileName);
//	else if(filetype.Find(".GIF") > -1) return SaveGIF(lpszFileName);
//	else if(filetype.Find(".JPG") > -1) return SaveJPG(lpszFileName);
	else return FALSE;
}

BOOL CNEImage::Load(LPCTSTR lpszFileName)
{
	CString filetype;
	filetype = lpszFileName;
	filetype.MakeUpper();

	if(filetype.Find(L".BMP") > -1) 
		return LoadBMP(lpszFileName);
//	else if(filetype.Find(".TIF") > -1) return LoadTIF(lpszFileName);
//	else if(filetype.Find(".GIF") > -1) return LoadGIF(lpszFileName);
//	else if(filetype.Find(".JPG") > -1) return LoadJPG(lpszFileName);
	else return FALSE;
}

/******************************************************
  실제 그래픽 파일을 읽어오는 루틴은 다음 파일에 있음
 
	ImageFileBmp.cpp : BMP 파일 (LoadBMP, SaveBMP)
	ImageFileGif.cpp : GIF 파일 (LoadGIF, SaveGIF)
	ImageFileTif.cpp : TIFF 파일 (LoadTIF, SaveTIF)
	ImageFileJpg.cpp : JPEG 파일 (LoadJPG, SaveJPG)

******************************************************/


/******************************************************
				DIB와 관련된 전역 함수
******************************************************/

LPSTR WINAPI FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + ::PaletteSize(lpbi));
}


DWORD WINAPI DIBWidth(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether Win 3.0 and old) */

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	/* return the DIB width if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDIB))
		return lpbmi->biWidth;
	else  /* it is an other-style DIB, so return its width */
		return (DWORD)lpbmc->bcWidth;
}


DWORD WINAPI DIBHeight(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether old or Win 3.0 */

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	/* return the DIB height if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDIB))
		return lpbmi->biHeight;
	else  /* it is an other-style DIB, so return its height */
		return (DWORD)lpbmc->bcHeight;
}

WORD WINAPI PaletteSize(LPSTR lpbi)
{
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpbi))
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBQUAD));
   else
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}

WORD WINAPI DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;  // DIB bit count

	/*  If this is a Windows-style DIB, the number of colors in the
	 *  color table can be less than the number of bits per pixel
	 *  allows for (i.e. lpbi->biClrUsed can be set to some value).
	 *  If this is the case, return the appropriate value.
	 */

	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	/*  Calculate the number of colors in the color table based on
	 *  the number of bits per pixel for the DIB.
	 */
	if (IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	/* return number of colors based on bits per pixel */
	switch (wBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}



BOOL CNEImage::SaveBMP(LPCTSTR lpszFileName)
{
	CFile file;
	CFileException fe;
	BITMAPFILEHEADER bmfHdr;
	LPBITMAPINFOHEADER lpBI;
	DWORD dwDIBSize;
	

	// 쓰기 모드로 파일 열기
	if (!file.Open(lpszFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite, &fe)) return FALSE;
	
	// 메모리 핸들이 유효한지 확인
	if (m_hImage == NULL) return FALSE;

	lpBI = (LPBITMAPINFOHEADER)m_hImage;
	if (lpBI == NULL) return FALSE;

	// 비트맵 파일 헤더 정보를 설정
	bmfHdr.bfType = DIB_HEADER_MARKER;  // "BM"
	dwDIBSize = *(LPDWORD)lpBI + ::PaletteSize((LPSTR)lpBI);
	if((lpBI->biCompression==BI_RLE8) || (lpBI->biCompression==BI_RLE4))
		dwDIBSize += lpBI->biSizeImage;
	else 
	{
		DWORD dwBmBitsSize;  // Size of Bitmap Bits only
		dwBmBitsSize = WIDTHBYTES_IMG((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) * lpBI->biHeight;
		dwDIBSize += dwBmBitsSize;
		lpBI->biSizeImage = dwBmBitsSize;
	}

	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits=(DWORD)sizeof(BITMAPFILEHEADER)+lpBI->biSize + PaletteSize((LPSTR)lpBI);
	TRY
	{
		// 비트맵 파일 헤더를 파일에 쓰기
		file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));
		// 나머지 데이터를 파일에 쓰기
//		file.Write((LPSTR)&lpBI, bmfHdr.bfOffBits - sizeof (BITMAPFILEHEADER));
//		file.Write((LPVOID)m_lpImage, dwDIBSize-bmfHdr.bfOffBits+sizeof (BITMAPFILEHEADER));
		file.Write(lpBI, dwDIBSize);
	}
	CATCH (CFileException, e)
	{
		THROW_LAST();
	}
	END_CATCH
	
	return TRUE;
}


BOOL CNEImage::LoadBMP(LPCTSTR lpszFileName)
{
	CFile file;
	CFileException fe;
	LPSTR pDIB;
	DWORD dwBitsSize;
	BITMAPFILEHEADER bmfHeader;

	// 읽기 모드로 파일 열기
	if(!file.Open(lpszFileName, CFile::modeRead|CFile::shareDenyWrite, &fe))
		return FALSE;

	// 파일의 길이를 구함
	dwBitsSize = (DWORD)file.GetLength();

	// 파일 헤더 읽기
	if(file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader))!=sizeof(bmfHeader))
		return FALSE;

	// BMP 파일임을 나타내는 "BM" 마커가 있는지 확인
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
		return FALSE;

	// 메모리 할당
	if(m_hImage)
		delete [] m_hImage;
	m_hImage = (LPVOID)new BYTE[dwBitsSize];
	if(m_hImage== NULL) return FALSE;

	
	pDIB = (LPSTR) m_hImage;

	// 파일 읽기
	if (file.Read(pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) != dwBitsSize - sizeof(BITMAPFILEHEADER) ) 
	{	
		delete  m_hImage;
		return FALSE;
	}

	// DIB 초기화
	InitDIB();

	return TRUE;
}

