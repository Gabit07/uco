// DefectDispClass.cpp : 구현 파일입니다.
//

#include "../pch.h"
#include "../CommonData.h"
#include "../common/CommonDefine.h"
#include "DefectDispStatic.h"
#include "../common/configure.h"

#include "afxwin.h"


// CDefectDispStatic

IMPLEMENT_DYNAMIC(CDefectDispStatic, CStatic)

CDefectDispStatic::CDefectDispStatic()
{

}

CDefectDispStatic::~CDefectDispStatic()
{
}


BEGIN_MESSAGE_MAP(CDefectDispStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CDefectDispStatic 메시지 처리기입니다.
void CDefectDispStatic::Init()
{
	Invalidate();
}

void CDefectDispStatic::reset()
{
	//memset(&m_tagDefectData, 0, sizeof(PST_DEFECTDATA));
	m_tagDefectData.group_id = 0;
	m_tagDefectData.defect_posx = 0;
	m_tagDefectData.defect_posy = 0;
	m_tagDefectData.encoder_pos = 0;
	m_tagDefectData.headerline = 0;
	m_tagDefectData.cmdUpDown = 0;

	Invalidate();
}

void CDefectDispStatic::SetDefect(PST_DEFECTDATA p)
{
	reset();
	CopyMemory(&m_tagDefectData, p, sizeof(ST_DEFECTDATA));
	Invalidate();
	
}

PST_DEFECTDATA CDefectDispStatic::GetDataPtr()
{
	return &m_tagDefectData;
}


void CDefectDispStatic::SetBigSizeText()
{
	m_bBigSize = FALSE;
}

void CDefectDispStatic::DispText(CDC* pDC)
{
	CString str;
	CPoint  pt(5, 0);
	int nPitch = 17, nStty = 5;

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(COLOR_BLACK);
	
	CCommonData* pCommonData = CSingleToneCommonData::GetInstance();
	pDC->SelectObject(pCommonData->GetFont(FONT_9NORMAL));

	
	str.Format(L"GROUP ID: %d", m_tagDefectData.group_id);
	pDC->TextOut(pt.x, pt.y + nPitch * 0 + nStty, str);

	str.Format(L"Pos x: %.2fmm", m_tagDefectData.defect_posx);
	pDC->TextOut(pt.x, pt.y + nPitch * 1 + nStty, str);

	str.Format(L"Pos y: %.2fM", m_tagDefectData.defect_posy/1000);
	pDC->TextOut(pt.x, pt.y + nPitch * 2 + nStty, str);

	str.Format(L"Encoder pos: %.2fM", m_tagDefectData.encoder_pos / 1000.);
	pDC->TextOut(pt.x, pt.y + nPitch * 3 + nStty, str);

	if (m_tagDefectData.headerline == _UPPER_LINE)
	{
		str.Format(L"Header: Up", m_tagDefectData.headerline);
	}
	else if(m_tagDefectData.headerline == _UNDER_LINE)
	{
		str.Format(L"Header: Down", m_tagDefectData.headerline);
	}

	pDC->TextOut(pt.x, pt.y + nPitch * 4 + nStty, str);

	if (m_tagDefectData.cmdUpDown == eCMD_UPPER_NOZZLE)
	{
		str.Format(L"Nozzle: Up", m_tagDefectData.cmdUpDown);
	}
	else if(m_tagDefectData.cmdUpDown == eCMD_LOWER_NOZZLE)
	{
		str.Format(L"Nozzle: Down", m_tagDefectData.cmdUpDown);
	}

	pDC->TextOut(pt.x, pt.y + nPitch * 5 + nStty, str);

}

void CDefectDispStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
	DispText(&dc);
}


BOOL CDefectDispStatic::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rect;
	GetClientRect(rect);
	pDC->FillSolidRect(rect, RGB(255, 255, 255));


	return TRUE;
}
