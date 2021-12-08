// UserButton.cpp : ���� �����Դϴ�.
//

#include "../pch.h"
#include "../CommonData.h"
#include "UserButton.h"
#include "../control/memdc.h"

// CUserButton

IMPLEMENT_DYNAMIC(CUserButton, CStatic)

CUserButton::CUserButton()
{
	m_bMouseOn = FALSE;
	m_bOldFlag = FALSE;
}

CUserButton::~CUserButton()
{
}


BEGIN_MESSAGE_MAP(CUserButton, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CUserButton �޽��� ó�����Դϴ�.

void CUserButton::SetOn(bool bRet)
{
	m_bMouseOn = bRet;
	if(m_bOldFlag == m_bMouseOn)
		return;
	else
	{
		m_bOldFlag = bRet;
	    Invalidate(false);
	}
}

/************************************************************************/
/* ���� ���۸��� �������� �ձ� �簢���� �ȱ׷�����
   ����� ����*/
/************************************************************************/
void CUserButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CPoint point(10,10);
    CRect rect;
	GetClientRect(rect);

	CPen pen, *oldpen;
	CMemdc pDC(&dc, &rect, TRUE);
	CCommonData* pCommonData = CSingleToneCommonData::GetInstance();

	CFont *OldFont = pDC->SelectObject(pCommonData->GetFont(FONT_12BOLD));

	if(m_bMouseOn)
	{
		pen.CreatePen(PS_SOLID, 11, RGB(255,255,255));
		oldpen = pDC->SelectObject(&pen);
		
		//dc.RoundRect(rect, point);
		pDC->FillSolidRect(&rect, COLOR_TITLE);
		pDC->SetTextColor(COLOR_WHITE);
	}
	else
	{
		pen.CreatePen(PS_SOLID, 11, RGB(204,204,204));
		oldpen = pDC->SelectObject(&pen);
		
		//dc.RoundRect(rect, point);
		pDC->FillSolidRect(&rect, COLOR_BLACK_LITTLE/*COLOR_BACKGRND*/);
		pDC->SetTextColor(COLOR_WHITE);
	}
	
	

	CString str;
	this->GetWindowText(str);
	pDC->DrawText(str, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(OldFont);

	pen.DeleteObject();
}

BOOL CUserButton::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	

	return FALSE;//CStatic::OnEraseBkgnd(pDC);
}
