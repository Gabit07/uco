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
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CButton::OnPaint()��(��) ȣ������ ���ʽÿ�.
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
