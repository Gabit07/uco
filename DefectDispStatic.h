#pragma once


// CDefectDispStatic

class CDefectDispStatic : public CStatic
{
	DECLARE_DYNAMIC(CDefectDispStatic)

public:
	CDefectDispStatic();
	virtual ~CDefectDispStatic();

protected:
	DECLARE_MESSAGE_MAP()

private:
	CWnd* m_pMain;
	CRect m_rtArea;
	ST_DEFECTDATA m_tagDefectData;
	BOOL m_bBigSize;
	
	

public:
	void Init();
	void SetBigSizeText();
	void reset();
	void SetDefect(PST_DEFECTDATA p);
	void DispText(CDC* pDC);

	PST_DEFECTDATA GetDataPtr();
	
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


