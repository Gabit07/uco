// StaticColorCtrl.cpp : ���� �����Դϴ�.
//

#include "../pch.h"
#include "../CommonData.h"
#include "StaticColorCtrl.h"


// CStaticColorCtrl

IMPLEMENT_DYNAMIC(CStaticColorCtrl, CStatic)


CStaticColorCtrl::CStaticColorCtrl()
{
	m_colorText = 0;
	m_colorBase = 0;
	m_bReverse = FALSE;
	m_nfontsize = FONT_11NORMAL;
}

CStaticColorCtrl::~CStaticColorCtrl()
{
}


BEGIN_MESSAGE_MAP(CStaticColorCtrl, CStatic)
	ON_WM_PAINT()
//	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CStaticColorCtrl �޽��� ó�����Դϴ�.


void CStaticColorCtrl::SetColor(CString strText, int fontsize, COLORREF clrBase, COLORREF clrText)
{
	m_strText = strText;
	m_colorBase = clrBase;
	m_colorText = clrText;
	m_nfontsize = fontsize;

	InvalidateRect(m_rectArea, false);
}

void CStaticColorCtrl::SetColor(CString strText, COLORREF clrBase, COLORREF clrText, BOOL bReverse)
{
	m_bReverse = bReverse;
	m_strText = strText;
	m_colorBase = clrBase;
	m_colorText = clrText;
}

void CStaticColorCtrl::ChangeSimBol(CString strText)
{
	m_strText = strText;
	Invalidate();
}

void CStaticColorCtrl::SetReverse(BOOL bRet) 
{ 
	m_bReverse = bRet;
	Invalidate();
}

BOOL CStaticColorCtrl::GetReverseFlag() 
{ 
	return m_bReverse;
}

void CStaticColorCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.

	CCommonData* pCommonData = CSingleToneCommonData::GetInstance();
	GetClientRect(m_rectArea);

	if(m_bReverse)
	{
		dc.FillSolidRect(m_rectArea, RGB(240,240,240));
		dc.SetTextColor(COLOR_BLACK);
		dc.SelectObject(pCommonData->GetFont(m_nfontsize));
		dc.DrawTextW(m_strText, m_rectArea, DT_SINGLELINE | DT_VCENTER | DT_LEFT/*DT_CENTER*/);
	}
	else
	{
		dc.FillSolidRect(m_rectArea, m_colorBase);
		dc.SetTextColor(m_colorText);
		dc.SelectObject(pCommonData->GetFont(m_nfontsize));
		dc.DrawTextW(m_strText, m_rectArea, DT_SINGLELINE | DT_VCENTER | DT_LEFT/*DT_CENTER*/);
	}
	
	
}

BOOL CStaticColorCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	return TRUE;
}
