#pragma once

#include <afxwin.h>

// 기존 MFC 버튼에 아이콘만 추가되는 방식

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

