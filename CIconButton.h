#pragma once

#include <afxwin.h>

// ���� MFC ��ư�� �����ܸ� �߰��Ǵ� ���

class CIconButton : public CButton
{
private:

	HICON m_hIcon;
	bool m_bClicked;

public:

	void SetButtonIcon(UINT nID, int _sizex, int _sizey);
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

