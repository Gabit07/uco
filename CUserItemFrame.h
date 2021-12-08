#pragma once

#include "../pch.h"

/* Product by Kim Jin woo 2020.08.19
   �׸� �� ���� �������� �ѹ��� ���� �׷����� ���� ���� */

class CUserItemFrame : public CStatic
{
private:

	CRect m_rectArea;
	CString m_strText;
	
	int     m_nfontsize;
	int     m_nsizey;

	COLORREF m_colorTitleBase;
	COLORREF m_colorText;
	COLORREF m_colorBackground;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

public:

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();

public:

	void SetUserItemFrame(CString strText, int fontsize, int sizey, COLORREF clrTitleBase, COLORREF clrText, COLORREF clrBackground);
	void ChangeSimBol(CString strText);
};

