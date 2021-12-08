#pragma once


// CUserButton

class CUserButton : public CStatic
{
	DECLARE_DYNAMIC(CUserButton)

public:
	CUserButton();
	virtual ~CUserButton();

private:
	BOOL m_bMouseOn;
	BOOL m_bOldFlag;

public:
	void SetOn(bool bRet);
	
protected:
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


