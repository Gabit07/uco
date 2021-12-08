#pragma once

#include "../pch.h"
#include "dib.h"
#include "dibpal.h"
// CDispImageStatic


class CDispImageStatic : public CStatic
{
	DECLARE_DYNAMIC(CDispImageStatic)

public:
	CDispImageStatic();
	virtual ~CDispImageStatic();

protected:
	DECLARE_MESSAGE_MAP()

private:

	CString m_strPath;
	CRect m_rect;
	CDIBPalette *m_palDIB;
	CDIBitmap	*m_hDIB;
	HBITMAP m_hBitmapBg;
	BOOL m_bIsNoImage;

	int m_nIMGX;
	int m_nIMGY;

	afx_msg void OnPaint();

public:

	void Init(int nImageSizeX, int nImageSizeY);
	void reset();
	void GetImagePath(CString strSrcImgPath);
	LPBYTE GetBitmapPtr() { return m_hDIB->GetBMBitsPtr(); }
	void CopyImage(BYTE* src);
	
	
};


