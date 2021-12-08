#include "CIcon.h"
#include "../control/memdc.h"

void CIcon::SetICon(UINT _ResId, int _sizex, int _sizey)
{
	hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(_ResId), IMAGE_ICON, _sizex, _sizey, 0);
	this->SetIcon(hIcon);
}

BEGIN_MESSAGE_MAP(CIcon, CStatic)
ON_WM_ERASEBKGND()
ON_WM_PAINT()
END_MESSAGE_MAP()


BOOL CIcon::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return TRUE;
}


void CIcon::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   
	CRect rect;
	GetClientRect(&rect);
	CMemdc pDC(&dc, &rect, TRUE);

	pDC->SetBkMode(TRANSPARENT);

	if (hIcon)
		pDC->DrawIcon(0, 0, hIcon);
}
