#pragma once

#include <afxwin.h>

/* ������ ���� �ֱ����� ����*/

class CIcon : public CStatic
{
private:

	HICON hIcon;

public:

	CIcon() : hIcon(nullptr) {}
	virtual~CIcon() = default;

	void SetICon(UINT _ResId, int _sizex, int _sizey);

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};

