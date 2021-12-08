//dib.h.
//header file for the CDIBitmap class

#ifndef __DIB__
#define __DIB__


class CDIBitmap : public CObject
{
    DECLARE_SERIAL(CDIBitmap)

public:
	
	 CDIBitmap();
    ~CDIBitmap();
protected:
    BITMAPINFO* m_pBMI;      //Pointer to the BITMAPINFO struct.
    BYTE* m_pBits;           //Pointer to the bitmap bits.
public:
	BOOL DrawCross(CPoint p,int wSize,int color);
    BITMAPINFO* GetBMHdrPtr() {return m_pBMI;} // Pointer to bitmap header.
    BYTE* GetBMBitsPtr() {return m_pBits;}     // Pointer to bitmap bits.
    RGBQUAD* GetBMColorTabPtr(); 	           // Pointer to color table.
    int GetNumClrEntries();         	       // Number of color table entries.
	BOOL CreateDIB(BITMAPINFO* pBMI, BYTE* pBitmapBits);
    int GetBMWidth();
    int GetBMHeight();
	int GetBMStorageWidth();
    virtual BOOL LoadDIB(CFile* fp);  
	virtual BOOL SaveDIB(CFile* fp);
    virtual void Serialize(CArchive& ar);
 	virtual void DrawBitmap(CDC* pDC, CRect rtSrc,CRect rtDest, CPalette* pPal);
    virtual void DrawBitmap(CDC* pDC, int x, int y,int right,int bottom, CPalette* pPal);
	BOOL MapBitmapToIdentityPalette(CPalette* pPal);
	virtual void CopyBits(CDIBitmap* pDestination, int xDest, int yDest, int width, int height, int xSrc, int ySrc);
	BOOL CopySmallWindow(CDIBitmap* pDestination, int xSrc, int ySrc, int width, int height);
	void CopyLineUpBits(CRect rect,BYTE* image);
	void CopyLineUpBits(BYTE* image);
	void* GetAddressOfPixel(int x, int y);
	BOOL MakeGrayImage(int xSize,int ySize);
	BOOL LoadSmallWindow(CDIBitmap* pSource,int xPos,int yPos,double ratio);
};

#endif // __DIB__
