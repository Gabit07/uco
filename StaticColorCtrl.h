#pragma once


// CStaticColorCtrl

class CStaticColorCtrl : public CStatic
{
	DECLARE_DYNAMIC(CStaticColorCtrl)

public:
	CStaticColorCtrl();
	virtual ~CStaticColorCtrl();

protected:
	DECLARE_MESSAGE_MAP()

private:

	CWnd* m_pParent = nullptr;
	CRect m_rectArea;
	COLORREF m_colorBase;
	COLORREF m_colorText;
	CString m_strText;
	BOOL    m_bReverse;
	int     m_nfontsize;

public:

	void SetColor(CString strText, int fontsize, COLORREF clrBase, COLORREF clrText);
	void SetColor(CString strText, COLORREF clrBase, COLORREF clrText, BOOL bReverse);
	void ChangeSimBol(CString strText);
	CString GetSimbol() { return m_strText;}
	COLORREF GetColor() {return m_colorText;}
	void SetReverse(BOOL bRet);
	BOOL GetReverseFlag(); 
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


