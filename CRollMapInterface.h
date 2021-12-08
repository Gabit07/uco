#pragma once

#include <afxwin.h>
#include "../CommonData.h"
#include "../common/configure.h"
#include "../CVector.h"
#include <vector>

class CRollMapInterface : public CStatic
{
public:
	CRollMapInterface();
	virtual ~CRollMapInterface() = default;
	
	void SetMapSize(CRect rect) {
		m_rectRollmap = rect;
		m_dSfx = (double)(m_rectRollmap.Width() / m_dViewsizex);
		m_dSfy = (double)(m_rectRollmap.Height() / m_dViewsizey);
	}

	void SetConnectDefectBuffer(CVector<PST_DEFECTDATA>* buffer) {
		rollmapbuffer = buffer->getbufferptr();
	}
private:

	std::vector<PST_DEFECTDATA> rollmapbuffer;

	CRect m_rectRollmap;
	BOOL  m_bDisplayMode;
	
	
	double	m_dViewSizeSuby;
	double	m_dViewSizeSubx;

	double m_dOldX;
	double m_dOldY;

	PST_DEFECTDATA	m_pBadpointClick;

	int m_nRollMapOriX = 0;
	

	int m_nIjpIntervalOffset;

public:

	int m_nMarkLine;
	int m_nMax_xzone;
	int m_nMax_yzone;
	float	m_fViewSttPosY;
	double	m_dViewsizex;
	double	m_dViewsizey;

	double m_dSfx; // roll map x scale
	double m_dSfy;
	bool m_bViewIjpDotLine; // x: 10mm 
	bool m_bViewText;
	bool m_bView70mmLine;
	bool m_bHeadSplicView;
	bool m_bDotClashView;
	bool m_bHeadClashView;

	double m_dMovingCurrentPositionY;

	void _SetScrollPosXY(BOOL bUseScroll);
	void _SetScrollBar(double dx, double dy = 0);

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void OnDrawPaint(CDC* pDC);

private:

	void _RedrawMap(CDC* pDC);
	void _DrawFrameItem(CDC* pDC);
	BOOL _DrawXMark(CDC* pDC, CPoint pt, PST_DEFECTDATA d);
	CPoint _GetFrameViewPos(double dSrcx, double dSrcy);
	PST_DEFECTDATA _GetNearestBadPoint(CPoint point, double& dApproch_dist);
	
};

