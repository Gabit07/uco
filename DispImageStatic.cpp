// DispImageStatic.cpp : 구현 파일입니다.
//


#include "../resource.h"
#include "DispImageStatic.h"
#include "CNEImage.h"


// CDispImageStatic

IMPLEMENT_DYNAMIC(CDispImageStatic, CStatic)

CDispImageStatic::CDispImageStatic()
{
	m_hDIB = NULL;
	m_palDIB = NULL;
	m_nIMGX = 128;
	m_nIMGY = 128;
	m_hBitmapBg = NULL;
	m_bIsNoImage = true;
}

CDispImageStatic::~CDispImageStatic()
{
	if (m_hBitmapBg)
	{
		::DeleteObject(m_hBitmapBg);
		m_hBitmapBg = NULL;
	}

	delete[] m_hDIB;
	delete[] m_palDIB;
}


BEGIN_MESSAGE_MAP(CDispImageStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CDispImageStatic 메시지 처리기입니다.

void CDispImageStatic::Init(int nImageSizeX, int nImageSizeY)
{
	m_bIsNoImage = TRUE;

	m_nIMGX = nImageSizeX;
	m_nIMGY = nImageSizeY;

	if (!m_hBitmapBg)
	{
		m_hBitmapBg = ::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_NO_IMAGE));
	}
	

	if (!m_hDIB)
	{
		m_hDIB = new CDIBitmap;
		m_hDIB->MakeGrayImage(m_nIMGX, m_nIMGY);
	}
	if (!m_palDIB)
	{
		m_palDIB = new CDIBPalette;
		m_palDIB->CreatePal(m_hDIB);
	}
	reset();
	
}

void CDispImageStatic::CopyImage(BYTE* src)
{
	if (src)
	{
		m_bIsNoImage = FALSE;
		memcpy(m_hDIB->GetBMBitsPtr(), src, m_nIMGX*m_nIMGY);
	}
	else
	{
		m_bIsNoImage = TRUE;
	}
	Invalidate();
}

void CDispImageStatic::GetImagePath(CString strSrcImgPath)
{
	CNEImage Img;
	BYTE* dDest = NULL;

	Img.Create(m_nIMGX, m_nIMGY, 8);
	Img.InitDIB();

	if (Img.Load(strSrcImgPath))
	{
		m_bIsNoImage = FALSE;
		BYTE * pImg = Img.GetImagePtr();
		memcpy(m_hDIB->GetBMBitsPtr(), pImg, m_nIMGX*m_nIMGY);
	}
	else
	{
		m_bIsNoImage = TRUE;
	}
	Invalidate();

}

void CDispImageStatic::reset()
{
	m_bIsNoImage = TRUE;
	memset(m_hDIB->GetBMBitsPtr(), 0, m_nIMGX*m_nIMGY);
	Invalidate();
}

void CDispImageStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 __super::OnPaint()을(를) 호출하지 마십시오.
	CRect rect;
	if (m_hDIB->GetBMBitsPtr() != NULL)
	{
		m_hDIB->DrawBitmap(&dc, 0, 0, m_nIMGX, m_nIMGY, m_palDIB);
	}
	if (m_bIsNoImage)
	{
		
		GetClientRect(rect);
		/*if (m_nIMGX == NG_IMAGE_X)
			rect.SetRect(0, 0, 128, 128);
		else if (m_nIMGX == NG_IMAGE_X180)
			rect.SetRect(0, 0, NG_IMAGE_X180, NG_IMAGE_Y180);*/
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		BITMAP BitImage;
		HBITMAP hOldBitmap;

		GetObject(m_hBitmapBg, sizeof(BITMAP), &BitImage);
		hOldBitmap = (HBITMAP)SelectObject(dcMem, m_hBitmapBg);

		dc.SetStretchBltMode(COLORONCOLOR);
		dc.StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, BitImage.bmWidth, BitImage.bmHeight, SRCCOPY);
		SelectObject(dcMem, hOldBitmap);
		DeleteDC(dcMem);
	}
}

