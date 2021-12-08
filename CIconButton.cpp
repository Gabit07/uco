#include "CIconButton.h"


void CIconButton::SetButtonIcon(UINT nID, int _sizex, int _sizey)
{
	m_bClicked = false;
	m_hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(nID), IMAGE_ICON, _sizex, _sizey, 0);
	this->SetIcon(m_hIcon);
}


BEGIN_MESSAGE_MAP(CIconButton, CButton)
	ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CIconButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CButton::OnPaint()을(를) 호출하지 마십시오.
	CRect rect;
	GetClientRect(rect);

	if (m_bClicked)
	{
		dc.DrawFrameControl(rect, DFC_BUTTON | DFCS_BUTTONPUSH, DFCS_PUSHED);
	}
	else
	{
		dc.DrawFrameControl(rect, DFC_BUTTON , DFCS_BUTTONPUSH);
	}
	

	if (m_hIcon)
	{
		CString strText;
		dc.SetBkMode(TRANSPARENT);
		dc.DrawIcon(5, 5, m_hIcon);
		this->GetWindowTextW(strText);
		dc.TextOutW(35, 10, strText);
	}
}


void CIconButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bClicked = true;

	CButton::OnLButtonDown(nFlags, point);
}


void CIconButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bClicked = false;

	CButton::OnLButtonUp(nFlags, point);
}
