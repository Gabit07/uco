// Image.h : header file

#ifndef __IMAGE_H__
#define __IMAGE_H__

#define HDIB HGLOBAL

// DIB constants
#define PALVERSION   0x300

// DIB Macros
#define IS_WIN30_DIB(lpbi) ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)  ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect) ((lpRect)->bottom - (lpRect)->top)
#define WIDTHBYTES_IMG(bits)   (((bits) + 31) / 32 * 4)

//BMP 파일 
//BITMAPFILEHEADER  ---Image Save할때 만둘어서 저장한다.
//BITMAPINFOHEADER  ---|
//BITMAP DATA		---|>	m_hImage

class CNEImage
{
/******************************************************
						멤버 변수
******************************************************/
protected:
	LPVOID m_hImage;	//image handle 
	CSize m_Size;		//image size
	CPalette * m_pPal;	//image palette
	

/******************************************************
						멤버 함수
******************************************************/
public:
	// 생성자, 초기화 함수
	CNEImage();
    CNEImage( CNEImage & );
	BOOL InitDIB(BOOL bCreatePalette = TRUE);
	void SetHandle(HANDLE hHandle);
	BOOL Create(int width, int height, int depth);
	BOOL CreateDIBPalette();

	// 소멸자, 정리 함수
	virtual ~CNEImage();
	void Free();


	// 이미지 정보를 얻는 함수
	int GetBitCount();	  //Image Depth 
	LPVOID GetHandle()		{return m_hImage;}
	BOOL IsDataNull()		{return (m_hImage == NULL);}
	CSize GetSize()			{return m_Size;}	
	int GetHeight()			{return m_Size.cy;}
	int GetWidth()			{return m_Size.cx;}
	int GetRealWidth()		{return WIDTHBYTES_IMG((GetWidth()*GetBitCount()));}
	CPalette *GetPalette()	{return m_pPal;}
	LPBYTE GetImagePtr();

	// 그리기
	BOOL Draw(HDC hDC, LPRECT sourceRect, LPRECT destRect);
	
	// 파일 읽어오기, 저장하기
	BOOL Save(LPCTSTR lpszFileName);
	BOOL Load(LPCTSTR lpszFileName);

protected:
	// 실제 파일 읽어오기
	BOOL LoadBMP(LPCTSTR lpszFileName);
	BOOL LoadJPG(LPCTSTR lpszFileName);
//	BOOL LoadGIF(LPCTSTR lpszFileName);
//	BOOL LoadTIF(LPCTSTR lpszFileName);

	// 실제 파일 저장하기
	BOOL SaveBMP(LPCTSTR lpszFileName);
	BOOL SaveJPG(LPCTSTR lpszFileName);
//	BOOL SaveGIF(LPCTSTR lpszFileName);
//	BOOL SaveTIF(LPCTSTR lpszFileName);
};

/******************************************************
						전역 함수
******************************************************/
LPSTR WINAPI FindDIBBits(LPSTR lpbi);
DWORD WINAPI DIBWidth(LPSTR lpDIB);
DWORD WINAPI DIBHeight(LPSTR lpDIB);
WORD WINAPI PaletteSize(LPSTR lpbi);
WORD WINAPI DIBNumColors(LPSTR lpbi);
/////////////////////////////////////////////////////////////////////////////
#endif