//dib.cpp
//implementation file for CDIBitmap class

#include "../pch.h"
#include "dib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDIBitmap

IMPLEMENT_SERIAL(CDIBitmap, CObject, 0 )

CDIBitmap::CDIBitmap()
{
    m_pBMI = NULL;
    m_pBits = NULL;
}

CDIBitmap::~CDIBitmap()
{
    // Free memory.
    if (m_pBMI != NULL)
    	free(m_pBMI);
    if (m_pBits != NULL)
    	free(m_pBits);
}

/////////////////////////////////////////////////////////////////////////////

//Serialization not supported. Use the ASSERT() statement
//to prevent accidental overwriting.
void CDIBitmap::Serialize(CArchive& ar)
{	  

	if (ar.IsStoring())
	{
		// 저장
		ar.Write(m_pBMI,sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
		ar.Write(m_pBits,GetBMStorageWidth()*GetBMHeight());
	}
	else
	{
		if(!m_pBMI)
		{
		// 읽기
			m_pBMI = (BITMAPINFO*) malloc(sizeof(BITMAPINFOHEADER) +
				256 * sizeof(RGBQUAD));
			ar.Read(m_pBMI,sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

			m_pBits = (BYTE* )malloc(GetBMStorageWidth()*GetBMHeight());
			ar.Read(m_pBits,GetBMStorageWidth()*GetBMHeight());			
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Private functions

static BOOL IsPMDIB(BITMAPINFOHEADER* pBIH)
{
//Returns TRUE if the bitmap information header pointed to by 
//pBIH is a Presentation Manager style. 
    ASSERT(pBIH);
    if (((BITMAPCOREHEADER*)pBIH)->bcSize == sizeof(BITMAPCOREHEADER))
        return TRUE;
    
    return FALSE;
}

static int GetColorEntriesInDIB(BITMAPINFO* pBmpInfo) 
{
    BITMAPINFOHEADER* pBIH;
    BITMAPCOREHEADER* pBCH;
    int iColors = 0, iBitsPerPixel;

    ASSERT(pBmpInfo);

    pBIH = &(pBmpInfo->bmiHeader);
    pBCH = (BITMAPCOREHEADER*) pBIH;

	//In Windows DIB's, the color table length is specified
	//by the biClrUsed field, but only if that field is non-zero.
    if ((!IsPMDIB(pBIH)) && (pBIH->biClrUsed != 0))
    	{
        iColors = pBIH->biClrUsed;
		return iColors;
    	}

    // Otherwise we can assume that the color table size depends
    // the number of bits per pixel.
    if (IsPMDIB(pBIH))
    	iBitsPerPixel = pBCH->bcBitCount;
    else 
        iBitsPerPixel = pBIH->biBitCount;

	if (iBitsPerPixel == 1)
		iColors = 2;

	if (iBitsPerPixel == 4)
		iColors = 16;

	if (iBitsPerPixel == 8)
		iColors = 256;

	return iColors;
}

BOOL CDIBitmap::SaveDIB(CFile* fp)
{
	BITMAPFILEHEADER bfh; // Header for Bitmap file
	bfh.bfType = 0x4d42;  // "BM"
	bfh.bfSize = sizeof(BITMAPFILEHEADER)+
		sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+
		GetBMStorageWidth()*GetBMHeight();
	bfh.bfReserved1=0;
	bfh.bfReserved2=0;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+
		sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD);

	UINT nSize = sizeof(bfh);
	TRY
	{
		fp->Write(&bfh,nSize);
	}CATCH(CFileException, e) {
//		TRACE("Fail to write file header");
		return FALSE;
	}END_CATCH

//	nSize = sizeof(BITMAPINFO);
	nSize = sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD);
	TRY
	{
		fp->Write(GetBMHdrPtr(),nSize);
	}CATCH(CFileException, e) {
//		TRACE("Fail to write BITMAPINFO");
		return FALSE;
	}END_CATCH

	nSize = GetBMStorageWidth()*GetBMHeight();
	TRY
	{
		fp->Write(GetBMBitsPtr(),nSize);
	}CATCH(CFileException, e) {
//		TRACE("Fail to write bits");
		return FALSE;
	}END_CATCH
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CDIBitmap commands

BOOL CDIBitmap::LoadDIB(CFile* fp)
{
//Load a DIB from a file. The file will already have been
//opened by the framework's serialization code.

    BOOL bIsPM = FALSE;
    BITMAPINFO* pBmpInfo = NULL;
    BYTE* pBits = NULL;
	int iColors, iColorTableSize, iBitsSize, iBISize;

    // Get the current file position.
    DWORD dwFileStart = (DWORD)fp->GetPosition();

    //Read the file header. This will tell us the file size and
    //where the data bits start in the file.
    BITMAPFILEHEADER BmpFileHdr;
    int iBytes;
    iBytes = fp->Read(&BmpFileHdr, sizeof(BmpFileHdr));
    if (iBytes != sizeof(BmpFileHdr))
    	{
		AfxMessageBox(L"Error reading file header");
        goto error;
    	}

    //Do we have "BM" at the start indicating a bitmap file?.
    if (BmpFileHdr.bfType != 0x4D42)
    	{
		AfxMessageBox(L"Not a bitmap file");
        goto error;
	    }

    //Assume for now that the file is a Windows DIB.
    //Read the BITMAPINFOHEADER. If the file is a PM DIB
    //file we'll convert it later.
    BITMAPINFOHEADER BmpInfoHdr;
    iBytes = fp->Read(&BmpInfoHdr, sizeof(BmpInfoHdr)); 
    if (iBytes != sizeof(BmpInfoHdr))
    	{
		AfxMessageBox(L"Error reading header information");
        goto error;
    	}

    //Was it a real Windows DIB file?
    if (BmpInfoHdr.biSize != sizeof(BITMAPINFOHEADER))
		{
    	//Not a Windows DIB. Set a flag indicating it's
    	//a PM format DIB.

        bIsPM = TRUE;
    
		//Is it really a PM DIB?
		if (BmpInfoHdr.biSize != sizeof(BITMAPCOREHEADER))
       		{
			//No!
			AfxMessageBox(L"Unknown DIB format");
       		goto error;
       		}

	    //We reach here only if it's a PM format DIB.
	    //Rewind the file pointer to read the BITMAPCOREHEADER
    	//data, then create the BITMAPINFOHEADER from it.
        fp->Seek(dwFileStart + sizeof(BITMAPFILEHEADER), CFile::begin);
        BITMAPCOREHEADER BmpCoreHdr;
        iBytes = fp->Read(&BmpCoreHdr, sizeof(BmpCoreHdr)); 
        if (iBytes != sizeof(BmpCoreHdr))
        	{
			AfxMessageBox(L"Error reading header information");
           	goto error;
        	}

        BmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
        BmpInfoHdr.biWidth = (int) BmpCoreHdr.bcWidth;
        BmpInfoHdr.biHeight = (int) BmpCoreHdr.bcHeight;
        BmpInfoHdr.biPlanes = BmpCoreHdr.bcPlanes;
        BmpInfoHdr.biBitCount = BmpCoreHdr.bcBitCount;
        BmpInfoHdr.biCompression = BI_RGB;
        BmpInfoHdr.biSizeImage = 0;
        BmpInfoHdr.biXPelsPerMeter = 0;
        BmpInfoHdr.biYPelsPerMeter = 0;
        BmpInfoHdr.biClrUsed = 0;
        BmpInfoHdr.biClrImportant = 0;
    }
	    
	//Determine memory needed.
    iColors = GetColorEntriesInDIB((LPBITMAPINFO) &BmpInfoHdr);
    iColorTableSize = iColors * sizeof(RGBQUAD);
    
    //Allocate memory for 256 entries.
    iBISize = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
    iBitsSize = BmpFileHdr.bfSize - BmpFileHdr.bfOffBits;

    //Allocate memory for header.
    pBmpInfo = (LPBITMAPINFO) malloc(iBISize);
    if (!pBmpInfo)
    	{
		AfxMessageBox(L"Insufficient memory for DIB header");
        goto error;
    	}

    //Copy header to newly allocated memory.
    memcpy(pBmpInfo, &BmpInfoHdr, sizeof(BITMAPINFOHEADER));

    //Read the color table data from the file.
    if (bIsPM == FALSE)
    	{
        iBytes = fp->Read(((LPBYTE) pBmpInfo) + sizeof(BITMAPINFOHEADER),
                             iColorTableSize);
        if (iBytes != iColorTableSize)
        	{
			AfxMessageBox(L"Error reading color table");
            goto error;
        	}
		}
	else
		{
        //Read each PM color table entry and convert it to Windows
        //DIB format.
        LPRGBQUAD lpRGB;
        lpRGB = (LPRGBQUAD) ((LPBYTE) pBmpInfo + sizeof(BITMAPINFOHEADER));
        RGBTRIPLE rgbt;
        for (int i = 0; i < iColors; i++)
        	{
            iBytes = fp->Read(&rgbt, sizeof(RGBTRIPLE));
            if (iBytes != sizeof(RGBTRIPLE))
            	{
				AfxMessageBox(L"Error reading color table data.");
                goto error;
            	}
            lpRGB->rgbBlue = rgbt.rgbtBlue;
            lpRGB->rgbGreen = rgbt.rgbtGreen;
            lpRGB->rgbRed = rgbt.rgbtRed;
            lpRGB->rgbReserved = 0;
            lpRGB++;
        	}
    	}

    // Allocate memory for the bitmap bits.
    pBits = (BYTE*) malloc(iBitsSize);
    if (pBits == NULL)
    	{
		AfxMessageBox(L"Insufficient memory to read DIB bits");
        goto error;
    	}

    //Move file pointer to file location of bits.
    fp->Seek(dwFileStart + BmpFileHdr.bfOffBits, CFile::begin);

    // Read data.
    iBytes = fp->Read(pBits, iBitsSize);
    if (iBytes != iBitsSize)
    	{
		AfxMessageBox(L"Error reading bitmap data");
        goto error;
    	}

// bitmap 헤더에 어떠한 데이터들이 있는지 알아보자
/*
	char ma[200];
		wsprintf(ma,"S:%d,W:%d,H:%d,P:%d,B:%d,C:%d,S:%d,X:%d,Y:%d,C:%d,c:%d",
			BmpInfoHdr.biSize ,
			BmpInfoHdr.biWidth ,
			BmpInfoHdr.biHeight ,
			BmpInfoHdr.biPlanes ,
			BmpInfoHdr.biBitCount ,
			BmpInfoHdr.biCompression ,
			BmpInfoHdr.biSizeImage ,
			BmpInfoHdr.biXPelsPerMeter ,
			BmpInfoHdr.biYPelsPerMeter,
			BmpInfoHdr.biClrUsed ,
			BmpInfoHdr.biClrImportant);
		AfxMessageBox(ma);
*/
  //We reach here only if no errors occurred.
    if (m_pBMI != NULL)
    	free(m_pBMI);
    m_pBMI = pBmpInfo; 
    if (m_pBits != NULL)
    	free(m_pBits);
    m_pBits = pBits;
    return TRUE;
                
//Oops!
error: 
    if (pBmpInfo)
    	free(pBmpInfo);
    if (pBits)
    	free (pBits);
    return FALSE;    
}

RGBQUAD* CDIBitmap::GetBMColorTabPtr()
{
//Returns a pointer to the bitmap's color table data.
	return (LPRGBQUAD)(((BYTE*)(m_pBMI)) + sizeof(BITMAPINFOHEADER));
}
   
void CDIBitmap::DrawBitmap(CDC* pDC, int x, int y,int right ,int bottom, CPalette* pPal)
{
//Draw the DIB to the specified device context at position (x,y).
	HPALETTE hPal=NULL;           // Our DIB's palette
	HPALETTE hOldPal=NULL;        // Previous palette

	if (m_pBMI == NULL)
		return;
	HDC     hDC = pDC->GetSafeHdc();

	if (pPal != NULL)
	{
		hPal = (HPALETTE) pPal->m_hObject;

		// Select as background since we have
		// already realized in forground if needed
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
		pDC->RealizePalette();  // 이것은 매우 중요한 팩터임 7.1
	}
	
          

	int k = m_pBMI->bmiHeader.biWidth;
  
	::SetStretchBltMode(hDC, COLORONCOLOR);
    ::StretchDIBits(hDC,
                  x,                        // Destination x
                  y,                        // Destination y
                  right-x,  // Destination width
                  bottom-y, // Destination height
                  0,                        // Source x
                  0,                        // Source y
                  m_pBMI->bmiHeader.biWidth,   // Source width
                  m_pBMI->bmiHeader.biHeight,  // Source height
                  GetBMBitsPtr(),           // Pointer to bits
                  GetBMHdrPtr(),		    // ptr to BITMAPINFO
                  DIB_RGB_COLORS,           // Options
                  SRCCOPY);		            // Raster operation code (ROP)
	
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}
}

// Get the number of color table entries.
int CDIBitmap::GetNumClrEntries()
{
    return GetColorEntriesInDIB(m_pBMI);
}

int CDIBitmap::GetBMWidth()
{
//Return bitmap width.
	return m_pBMI->bmiHeader.biWidth;
} 

int CDIBitmap::GetBMHeight()
{
//Return bitmap height.
	return m_pBMI->bmiHeader.biHeight;
}

BOOL CDIBitmap::MapBitmapToIdentityPalette(CPalette* pPal)
{
//Maps a palette bit values to point to the correct colors
//in an identity palette.
	int i;
	BYTE map[256];

	//Is there a palette to map to?
	if (!pPal)
		{
		AfxMessageBox(L"No palette to map!");
		return FALSE;
		}

	//Get the address of the bitmap's color table.
	LPRGBQUAD pColorTable = GetBMColorTabPtr();

	//Go through the bitmap's color table, and for each entry find
	//the index in the palette that most closely matches.
 	for (i = 0; i < 256; i++)
		{
		map[i] = (BYTE) pPal->GetNearestPaletteIndex(RGB(pColorTable->rgbRed,
													 	 pColorTable->rgbGreen,
														 pColorTable->rgbBlue));
		pColorTable++;
		}

	//Now set the DIB bits to point to the new index values.
	BYTE* pBitmapBits = (BYTE*)GetBMBitsPtr();
	int numBytes = GetBMStorageWidth() * GetBMHeight();
	for (i = numBytes; i > 0; i--)
		{
		*pBitmapBits = map[*pBitmapBits];
		pBitmapBits++;
		}

	//Finally, modify the DIB's color table to agree with the new
	//DIB bytes. If this is not done, and subsequent palettes cvreated
	//from the DIB's color table will not be accurate.

	PALETTEENTRY pal[256];
	pPal->GetPaletteEntries(0, 256, pal);
	pColorTable = GetBMColorTabPtr();
	for (i = 0; i < 256; i++)
		{
		pColorTable->rgbRed = pal[i].peRed;
		pColorTable->rgbGreen = pal[i].peGreen;
		pColorTable->rgbBlue = pal[i].peBlue;
		pColorTable++;
		}

	return TRUE;
}
int CDIBitmap::GetBMStorageWidth()
{
//Returns the number of bytes used to store
//each scan line of the bitmap.

	return (m_pBMI->bmiHeader.biWidth + 3) & ~3;
}

BOOL CDIBitmap::CreateDIB(BITMAPINFO* pBMInfo, BYTE* pBitmapBits)
{
//Initializes a CDIBitmap object

	if (m_pBMI)
		free(m_pBMI);

	m_pBMI = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

	if (!m_pBMI)
		{
		AfxMessageBox(L"Could not allocate memory for buffer.");
		return FALSE;
		}

	memcpy(m_pBMI, pBMInfo, sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

	m_pBits = pBitmapBits;
	return TRUE;
}

// 일정크기의 영상을 앍어간다.
void CDIBitmap::CopyBits(CDIBitmap* pDestination, int xDest, int yDest, int width, int height, int xSrc, int ySrc)
{
//Copies bits from this CDIBitmap object to the destination CDIBitmap object.

if (width ==0 || height == 0)
	return;


//Get addresses of starting pixels in each bitmap.
BYTE* pDest = (BYTE*)pDestination->GetAddressOfPixel(xDest, yDest + height - 1);
BYTE* pSrc = (BYTE*)GetAddressOfPixel(xSrc, ySrc + height - 1);

//Get width of scan lines.
int srcScan = GetBMStorageWidth();
int destScan = pDestination->GetBMStorageWidth();

//Perform the copy operation.
for ( ; height > 0; height--)
	{
	memcpy(pDest, pSrc, width);
	pSrc += srcScan;
	pDest += destScan;
	}
}

/*
// CDIBITMAP 의 복사본을 생성시킨다.
BOOL CDIBitmap::CopySmallWindow(CDIBitmap* pDestination, int xSrc, int ySrc, int width, int height)
{
//Copies bits from this CDIBitmap object to the destination CDIBitmap object.

	if (width ==0 || height == 0)
		return FALSE;

		// 헤더부의 메모리를 잡는다.
		BITMAPINFO*	 pDesHead = pDestination->GetBMHdrPtr();
		pDesHead = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
		if (!pDesHead)
			{
			AfxMessageBox("Could not allocate memory for buffer.");
			return  FALSE;
			}
		


		memcpy(pDesHead, m_pBMI, sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

//	   	pDestination->m_pBMI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pDesHead->bmiHeader.biWidth  = width;
		pDesHead->bmiHeader.biHeight = height;
//		pDestination->m_pBMI->bmiHeader.biPlanes = 1;
//		pDestination->m_pBMI->bmiHeader.biBitCount = 8;
//		pDestination->m_pBMI->bmiHeader.biCompression = BI_RGB;
//		pDestination->m_pBMI->bmiHeader.biSizeImage = 0;
//		pDestination->m_pBMI->bmiHeader.biXPelsPerMeter = 0;
//		pDestination->m_pBMI->bmiHeader.biYPelsPerMeter = 0;
//		pDestination->m_pBMI->bmiHeader.biClrUsed = 0;
//		pDestination->m_pBMI->bmiHeader.biClrImportant = 0;

		char buff[50];
		wsprintf(buff,"%d  : Current Active Sheet",(int)	pDesHead->bmiHeader.biBitCount);
		AfxMessageBox(buff);
		// 데이터부의 메모리를 잡는다.
		BYTE* pDesData = 	pDestination->GetBMBitsPtr() ;
		pDesData = (BYTE*) malloc(pDestination->GetBMStorageWidth()*height);
	    if (pDesData == NULL)
    	{
		   AfxMessageBox("Insufficient memory to read DIB bits in CopySmallWindow");
			return FALSE;
    	}


//Get addresses of starting pixels in each bitmap.
	BYTE* pDest = (BYTE*)pDestination->GetAddressOfPixel(0, height - 1);
	BYTE* pSrc = (BYTE*)GetAddressOfPixel(xSrc, ySrc + height - 1);

//Get width of scan lines.
	int srcScan = GetBMStorageWidth();
	int destScan = pDestination->GetBMStorageWidth();

//Perform the copy operation.
	for ( ; height > 0; height--)
	{
		memcpy(pDest, pSrc, width);
		pSrc += srcScan;
		pDest += destScan;
	}
	return TRUE;
}
*/


// 작은 영상을 일정비율로 원 영상에 복사해 넣는 것
// ratio는 보통 1 이하로 사용한다.
//1999.2.7
BOOL CDIBitmap::LoadSmallWindow(CDIBitmap* pSource,int xPos,int yPos,double ratio)
{
	//Get width of scan lines.
	int i, j;
	int srcScan = pSource->GetBMStorageWidth();
	int destScan = GetBMStorageWidth();

	int height = pSource->GetBMHeight();
	int width = pSource->GetBMWidth();
	int desHeight = GetBMHeight();
	int desWidth = GetBMWidth();

	if((desHeight<((yPos+height)*ratio)||(desWidth<(xPos+width)*ratio)))
	{
		AfxMessageBox(L"Soo");
		return false;
	}
	BYTE* pSrc = (BYTE*)pSource->GetAddressOfPixel(0,0);
	BYTE* pDest = (BYTE*)GetAddressOfPixel(xPos, yPos);
	
	for(i=0;i<(int)(height);i++)
	{
		for(j=0;j<(int)(width);j++)
		{
			*(pDest+destScan*(int)(-i*ratio) +(int)(j*ratio)) = *(pSrc+srcScan*(int)(-i) +(int)(j));	
		}
	}
	return TRUE;
}


// CDIBITMAP 의 복사본을 생성시킨다.
// 1996.6.26   흑백영상만되고 그외는 잘안됨
BOOL CDIBitmap::CopySmallWindow(CDIBitmap* pDestination, int xSrc, int ySrc, int width, int height)
{
//Copies bits from this CDIBitmap object to the destination CDIBitmap object.

		if (width ==0 || height == 0)
			return FALSE;
		if(pDestination->m_pBMI) delete pDestination->m_pBMI;
		// 헤더부의 메모리를 잡는다.
		pDestination->m_pBMI = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
		if (!pDestination->m_pBMI)
			{
			AfxMessageBox(L"Could not allocate memory for buffer.");
			return  FALSE;
			}
		


		memcpy(pDestination->m_pBMI, m_pBMI, sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

//	   	pDestination->m_pBMI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pDestination->m_pBMI->bmiHeader.biWidth  = width;
		pDestination->m_pBMI->bmiHeader.biHeight = height;
/*
		pDestination->m_pBMI->bmiHeader.biPlanes = 1;
		pDestination->m_pBMI->bmiHeader.biBitCount = 8;
		pDestination->m_pBMI->bmiHeader.biCompression = BI_RGB;
		pDestination->m_pBMI->bmiHeader.biSizeImage = 0;
		pDestination->m_pBMI->bmiHeader.biXPelsPerMeter = 0;
		pDestination->m_pBMI->bmiHeader.biYPelsPerMeter = 0;
		pDestination->m_pBMI->bmiHeader.biClrUsed = 0;
		pDestination->m_pBMI->bmiHeader.biClrImportant = 0;

  */
		// 데이터부의 메모리를 잡는다.
		if(pDestination->m_pBits) delete pDestination->m_pBits;
		pDestination->m_pBits = (BYTE*) malloc(pDestination->GetBMStorageWidth()*height);
	    if (pDestination->m_pBits == NULL)
    	{
			AfxMessageBox(L"Insufficient memory to read DIB bits in CopySmallWindow");
			return FALSE;
    	}


//Get addresses of starting pixels in each bitmap.
	BYTE* pDest = (BYTE*)pDestination->GetAddressOfPixel(0, height - 1);
	BYTE* pSrc = (BYTE*)GetAddressOfPixel(xSrc, ySrc + height - 1);

//Get width of scan lines.
	int srcScan = GetBMStorageWidth();
	int destScan = pDestination->GetBMStorageWidth();

//Perform the copy operation.
	for ( ; height > 0; height--)
	{
		memcpy(pDest, pSrc, width);
		pSrc += srcScan;
		pDest += destScan;
	}
	return TRUE;
}

void* CDIBitmap::GetAddressOfPixel(int x, int y)
{
//Returns the address in the bitmap of the specified pixel.

	return (m_pBits + (GetBMHeight() - y - 1) * GetBMStorageWidth() + x);
}

// bmp는 y 축으로 돌아가니ㅆ으므로  다시 회전시켜야한다.
// rect 크기의 영상을 짤라간다.(image로)
void CDIBitmap::CopyLineUpBits(CRect rect,BYTE* image)
{
	int width = rect.Width();
	int height= rect.Height();

	BYTE* pSrc = (BYTE*)GetAddressOfPixel(rect.left, rect.top + height - 1);
	int srcScan = GetBMStorageWidth();
	

	BYTE* pDest = image;
	pDest += (width*(height-1)); 
	
	for ( ; height > 0; height--)
	{
		memcpy(pDest, pSrc, width);
		pSrc += srcScan;
		pDest -= width;
	}
}

// 통째로 카피해감
void CDIBitmap::CopyLineUpBits(BYTE* image)
{
	CRect rect(0,0,	GetBMWidth(),GetBMHeight());
	CopyLineUpBits(rect,image);
}


BOOL CDIBitmap::MakeGrayImage(int xSize,int ySize)
{
	int i;
	if (m_pBMI)
	{
		free(m_pBMI);
		free(m_pBits);
	}

	m_pBMI = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
	if (!m_pBMI)
	{
		AfxMessageBox(L"Could not allocate memory for buffer.");
		return FALSE;
	}
		m_pBMI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        m_pBMI->bmiHeader.biWidth = xSize;
        m_pBMI->bmiHeader.biHeight = ySize;
        m_pBMI->bmiHeader.biPlanes = 1;  // must set to 1
        m_pBMI->bmiHeader.biBitCount = 8; // Gray scale image must be 8.
        m_pBMI->bmiHeader.biCompression = BI_RGB; // uncompressed format
        m_pBMI->bmiHeader.biSizeImage = 0;
        m_pBMI->bmiHeader.biXPelsPerMeter = 0;
        m_pBMI->bmiHeader.biYPelsPerMeter = 0;
        m_pBMI->bmiHeader.biClrUsed = 0;
        m_pBMI->bmiHeader.biClrImportant = 0;

	for (i = 0; i < 256; i++)
	{
		m_pBMI->bmiColors[i].rgbRed = i;
		m_pBMI->bmiColors[i].rgbGreen = i;
		m_pBMI->bmiColors[i].rgbBlue = i;
		m_pBMI->bmiColors[i].rgbReserved=0;
	}
	int diffWidth = xSize%4;
	if(diffWidth)
		xSize += (4-diffWidth);
	m_pBits = (BYTE *)malloc(xSize*ySize);
	if (!m_pBits)
	{
		AfxMessageBox(L"Could not allocate memory for buffer.");
		return FALSE;
	}
/*
	for(i=0;i<ySize;i++)
	{
		for(int j=0;j<xSize;j++)
		{
			m_pBits[i*xSize+j]=(i*5)%255;
			if(j==25) m_pBits[i*xSize+j]=0;
			if(j==49) m_pBits[i*xSize+j]=0;
		}
	}
*/
	return TRUE;
}

void CDIBitmap::DrawBitmap(CDC *pDC, CRect Src, CRect Dest, CPalette *pPal)
{
	HPALETTE hPal=NULL;           // Our DIB's palette
	HPALETTE hOldPal=NULL;        // Previous palette

	if (m_pBMI == NULL)
		return;
	HDC     hDC = pDC->GetSafeHdc();

	if (pPal != NULL)
	{
		hPal = (HPALETTE) pPal->m_hObject;

		// Select as background since we have
		// already realized in forground if needed
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
		pDC->RealizePalette();  // 이것은 매우 중요한 팩터임 7.1
	}
	
          

	int k = m_pBMI->bmiHeader.biWidth;
  
	::SetStretchBltMode(hDC, COLORONCOLOR);
  	::StretchDIBits(hDC,                    // hDC
								   Dest.left,             // DestX
								   Dest.top,              // DestY
								   Dest.Width(),        // nDestWidth
								   Dest.Height(),       // nDestHeight
								   Src.left,            // SrcX
								   Src.top,				// SrcY
								   Src.Width()-1,
								   Src.Height()-1,
								   GetBMBitsPtr(),           // Pointer to bits
								   GetBMHdrPtr(),		    // ptr to BITMAPINFO
 								   DIB_RGB_COLORS,
								   SRCCOPY);            // wUsage

	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}
}
