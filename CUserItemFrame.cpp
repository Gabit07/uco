#include "CUserItemFrame.h"
#include "../common/configure.h"
#include "../CommonData.h"



BEGIN_MESSAGE_MAP(CUserItemFrame, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CUserItemFrame::SetUserItemFrame(CString strText, int fontsize, int sizey, COLORREF clrTitleBase, COLORREF clrText, COLORREF clrBackground)
{
	m_strText = strText;
	m_colorTitleBase = clrTitleBase;
	m_colorText = clrText;
	m_colorBackground = clrBackground;
	m_nfontsize = fontsize;
	m_nsizey = sizey;
}

void CUserItemFrame::ChangeSimBol(CString strText)
{
	m_strText = strText;
	CRect rect;
	GetClientRect(rect);
	InvalidateRect(rect, false);
}


void CUserItemFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CStatic::OnPaint()��(��) ȣ������ ���ʽÿ�.

	CRect rectFrame;
	GetWindowRect(rectFrame);
	ScreenToClient(rectFrame);
	dc.FillSolidRect(rectFrame, m_colorTitleBase);
	
	CCommonData* pCommonData = CSingleToneCommonData::GetInstance();
	
	dc.SetTextColor(m_colorText);
	dc.SelectObject(pCommonData->GetFont(m_nfontsize));
	dc.DrawTextW(m_strText, rectFrame, DT_SINGLELINE | DT_VCENTER | DT_LEFT/*DT_CENTER*/);
	
}


BOOL CUserItemFrame::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CRect rectFrame;
	GetWindowRect(rectFrame);
	ScreenToClient(rectFrame);
	rectFrame.bottom += m_nsizey;
	pDC->FillSolidRect(rectFrame, m_colorBackground);

	return TRUE;
}
