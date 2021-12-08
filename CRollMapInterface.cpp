#include "CRollMapInterface.h"
#include "../CVector.h"

CRollMapInterface::CRollMapInterface()
{
	m_nMax_xzone = 16;
	m_nMax_yzone = 20;
	m_dViewsizex = (double)m_nMax_xzone * 100;
	m_dViewsizey = (double)(static_cast<double>(m_nMax_yzone) * 100000); //(100M)
	m_nMarkLine = 300; //300mm
	
}



BOOL CRollMapInterface::_DrawXMark(CDC* pDC, CPoint pt, PST_DEFECTDATA d)
{
	CCommonData* pCommonData = CSingleToneCommonData::GetInstance();

	CString strDefectSimbol;

	int		nOffsetX = 0, nOffsetY = 0;


	CFont* pOldFont = pDC->SelectObject(pCommonData->GetFont(FONT_10BOLD));

	if (d->bMark)
	{
		pDC->SetTextColor(COLOR_WHITE);
		strDefectSimbol = L"U";
	}
	else
	{
		
	    // 롤맵 좌표
		int nMarkLine = int(m_dSfy * (m_dMovingCurrentPositionY + m_nMarkLine - m_fViewSttPosY));
		
		if (pt.y > nMarkLine)
		{
			pDC->SetTextColor(COLOR_GREEN);
			strDefectSimbol = L"M";
		}
		else
		{
			pDC->SetTextColor(COLOR_RED);
			strDefectSimbol = L"B";
		}


		/* 헤더의 지그재그 위치대로 표현가능한지 알기 위한 색상 적용*/
		if (m_bHeadSplicView)
		{
			if (d->headerline == 100)
			{
				pDC->SetTextColor(COLOR_SKY_BLUE);
				strDefectSimbol = L"UH";
			}
			else if (d->headerline == 200)
			{
				pDC->SetTextColor(COLOR_BORA_REV);
				strDefectSimbol = L"DH";
			}

		}

		if (m_bDotClashView)
		{
			if (d->bColor == _eDOT_CLASH)
			{
				pDC->SetTextColor(COLOR_YELLOW);
				strDefectSimbol = L"DC";
			}
		}

		if (m_bHeadClashView)
		{
			if (d->bColor == _eHEADER_CLASH)
			{
				pDC->SetTextColor(COLOR_PUPLE);
				strDefectSimbol = L"HC";
			}
		}

	}

	pDC->SetBkMode(TRANSPARENT);

	pDC->SetTextAlign(TA_CENTER);

	pt.y = (m_rectRollmap.Height() - pt.y);// *m_dSfy;
	pDC->TextOut(pt.x, pt.y, strDefectSimbol);
	pDC->SelectObject(pOldFont);

	return TRUE;

}

CPoint CRollMapInterface::_GetFrameViewPos(double dSrcx, double dSrcy)
{
	CPoint pt;

	int nOffsetX = 0;
	int nOffsetY = 10;

	if (m_fViewSttPosY == 0)
	{
		pt.y = int(m_rectRollmap.Height() - dSrcy * m_dSfy);
		pt.y += nOffsetY;
	}
	else
	{
		pt.y = int(m_rectRollmap.Height() - (m_dSfy * (dSrcy - m_fViewSttPosY)) + 0.5);
		pt.y += nOffsetY;
	}


	pt.x = int(dSrcx * m_dSfx) - m_nRollMapOriX;

	return pt;
}


void CRollMapInterface::_DrawFrameItem(CDC* pDC)
{
	int i;
	int width = m_rectRollmap.Width();
	int height = m_rectRollmap.Height();
	double dViewSizeX = m_dViewsizex;
	double dViewSizeY = m_dViewsizey;
	
	double dXscale = m_dSfx; //1mm당 pixel scalefactor
	double dYscale = m_dSfy;

	CString str;

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(COLOR_GRAY2);
	CPen* pOldPen, * pGrid, *pDotLine = nullptr, *p70mmLine = nullptr, *pStartLine = nullptr, *pMarkLine = nullptr;


	pGrid = new CPen(PS_DOT, 1, RGB(43, 36, 21));
	pOldPen = pDC->SelectObject(pGrid);

	for (i = 0; i < m_nMax_yzone; i++)
	{
		pDC->MoveTo(0, (int)(i * (dViewSizeY / m_nMax_yzone) * dYscale));
		pDC->LineTo(width, (int)(i * (dViewSizeY / m_nMax_yzone) * dYscale));
		str.Format(_T("%.2fM"), (float)(m_fViewSttPosY / 1000 + (i) * (dViewSizeY / (static_cast<double>(m_nMax_yzone) * 1000))));

		pDC->TextOut(width - 43, (int)(i * (dViewSizeY / m_nMax_yzone) * dYscale) - 15, str);
	}

	for (i = 0; i < m_nMax_xzone + 1; i++)
	{
		pDC->MoveTo((int)(i * 100 * dXscale) - m_nRollMapOriX, 0);
		pDC->LineTo((int)(i * 100 * dXscale) - m_nRollMapOriX, height);

		if (m_bViewText)
		{
			str.Format(L"%dmm", (int)(i * 100));
			pDC->TextOut((int)(i * 100 * dXscale) - m_nRollMapOriX, 5, str);
		}

	}

	/* IJP 10mm 피치 경계선 */
	if (m_bViewIjpDotLine)
	{
		pDotLine = new CPen(PS_DOT, 1, RGB(255, 0, 0));
		pOldPen = pDC->SelectObject(pDotLine);

		for (i = 0; i < (m_nMax_xzone + 1) * 1000; i++)
		{
			pDC->MoveTo((int)(i * 10 * dXscale) - m_nRollMapOriX + static_cast<int>(m_nIjpIntervalOffset * dXscale), 0);
			pDC->LineTo((int)(i * 10 * dXscale) - m_nRollMapOriX + static_cast<int>(m_nIjpIntervalOffset * dXscale), height);
		}
	}

	/* BCR MARK 70mm 바코드 헤더리미트 경계선 (물리적인 좌표라 임의로 변경될 값 아님)*/
	if (m_bView70mmLine)
	{
		p70mmLine = new CPen(PS_DOT, 1, RGB(0, 220, 0));
		pOldPen = pDC->SelectObject(p70mmLine);

		for (i = 0; i < (m_nMax_xzone + 1) * 1000; i++)
		{
			pDC->MoveTo((int)(i * 70 * dXscale) - m_nRollMapOriX, 0);
			pDC->LineTo((int)(i * 70 * dXscale) - m_nRollMapOriX, height);
		}
	}

	// mark line
	pMarkLine = new CPen(PS_SOLID, 1, RGB(0, 255, 0));// green
	pOldPen = pDC->SelectObject(pMarkLine);
	int nMarkLine =  int(dYscale * (m_dMovingCurrentPositionY - m_nMarkLine - m_fViewSttPosY));  //int(dYscale * m_nViewSttPosY * -1);

	pDC->MoveTo(0, nMarkLine);
	pDC->LineTo(width, nMarkLine);

	// start line
	pStartLine = new CPen(PS_SOLID, 1, RGB(255, 0, 0)); // red
	pOldPen = pDC->SelectObject(pStartLine);
	int nStartLineY = int(dYscale * (m_dMovingCurrentPositionY - m_fViewSttPosY));

	pDC->MoveTo(0, nStartLineY);
	pDC->LineTo(width, nStartLineY);

	pDC->SelectObject(pOldPen);

	delete pStartLine;
	delete pMarkLine;
	delete pDotLine;
	delete p70mmLine;
	delete pGrid;

}

PST_DEFECTDATA CRollMapInterface::_GetNearestBadPoint(CPoint point, double& dApproch_dist)
{
	CCommonData* pCommonData = CSingleToneCommonData::GetInstance();

	PST_DEFECTDATA	pBadpoint = NULL;
	double dX = 0, dY = 0;
	double dtmpdist = 0;
	int	   nRet = 0;

	CPoint ptList;

	for (auto& i : rollmapbuffer)
	{
		PST_DEFECTDATA data = i;
		ptList = _GetFrameViewPos(data->defect_posx, data->defect_posy);
		dtmpdist = DISTANCE(static_cast<double>(ptList.x), static_cast<double>(ptList.y), static_cast<double>(point.x), static_cast<double>(point.y));

		if (m_dOldX == data->defect_posx && m_dOldY == data->defect_posy)
			continue;

		if (dtmpdist > 0 && dtmpdist < 15)
		{
			if (pBadpoint == NULL)
			{
				dApproch_dist = dtmpdist;
				pBadpoint = data;
			}
			else if (dApproch_dist > dtmpdist)
			{
				pBadpoint = data;
				dApproch_dist = dtmpdist;
			}
		}
	}

	return pBadpoint;
}


void CRollMapInterface::_RedrawMap(CDC* pDC)
{

	if (rollmapbuffer.size() > 0)
	{
		m_dMovingCurrentPositionY = rollmapbuffer[rollmapbuffer.size() - 1]->defect_posy; // 가장 마지막 y 좌표
	}
	
	_DrawFrameItem(pDC);

	for (auto& i : rollmapbuffer)
	{
		CPoint point = _GetFrameViewPos(i->defect_posx, i->defect_posy);
		_DrawXMark(pDC, point, i);
	}

	ReleaseDC(pDC);
}

void CRollMapInterface::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	USES_CONVERSION;
	CString strfilepath;

	double dApproch_dist = 0.;
	CPoint ptList;
	PST_DEFECTDATA	pBadpoint;
	BOOL bFlagBadPoint = FALSE;

	point.y = m_rectRollmap.Height() - point.y;

	pBadpoint = _GetNearestBadPoint(point, dApproch_dist);

	if (pBadpoint && dApproch_dist < 15)
	{
		bFlagBadPoint = TRUE;
	}


	CDC* pDC = GetDC();

	if (m_pBadpointClick)
	{
		CPoint ptNg;

		m_pBadpointClick->bMark = FALSE;
		ptNg = _GetFrameViewPos(m_pBadpointClick->defect_posx, m_pBadpointClick->defect_posy);

		_DrawXMark(pDC, ptNg, m_pBadpointClick);
	}

	if (pBadpoint && dApproch_dist < 15)
	{
		CPoint ptNg;

		m_dOldX = pBadpoint->defect_posx;
		m_dOldY = pBadpoint->defect_posy;


		//이전 표시를 지운다.

		//새로운 표시를 한다.
		m_pBadpointClick = pBadpoint;
		m_pBadpointClick->bMark = TRUE;

		ptNg = _GetFrameViewPos(m_pBadpointClick->defect_posx, m_pBadpointClick->defect_posy);

		BOOL bOK = _DrawXMark(pDC, ptNg, m_pBadpointClick);

		::PostMessage(GetParent()->m_hWnd, MSG_UPDATEVIEWER_DEFECTDATA, (WPARAM)m_pBadpointClick, NULL);
	}
	else
	{
		::PostMessage(GetParent()->m_hWnd, MSG_UPDATEVIEWER_DEFECTDATA, NULL, NULL);
	}


	ReleaseDC(pDC);

	__super::OnLButtonDown(nFlags, point);
}


void CRollMapInterface::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (m_dViewsizey == static_cast<double>(m_nMax_yzone) * 100)
	{
		m_fViewSttPosY = 0;
		m_dViewsizey = (double)m_nMax_yzone * 100000;
		m_dSfy = m_rectRollmap.Height() / m_dViewsizey;
	}
	else
	{
		m_dViewsizey /= 1000;//(1M)

		m_dSfy = m_rectRollmap.Height() / m_dViewsizey;

		m_fViewSttPosY = 0;
		
	}
	
	SendMessage(WM_INVALIDATE, 0, 0);

	__super::OnLButtonDblClk(nFlags, point);
}


BOOL CRollMapInterface::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (zDelta < 0)
	{
		m_fViewSttPosY += (int)m_dViewsizey;
		
	}
	else
	{
		m_fViewSttPosY -= (int)m_dViewsizey;
		
	}
	return TRUE;
}


void CRollMapInterface::OnDrawPaint(CDC* pDC)
{

	CDC memDC;
	CBitmap bitmap;

	memDC.CreateCompatibleDC(pDC);
	bitmap.CreateCompatibleBitmap(pDC, m_rectRollmap.Width(), m_rectRollmap.Height());
	memDC.SelectObject(&bitmap);

	memDC.PatBlt(m_rectRollmap.left, m_rectRollmap.top, m_rectRollmap.Width(), m_rectRollmap.Height(), BLACKNESS);

	_RedrawMap(&memDC);

	pDC->BitBlt(m_rectRollmap.left, m_rectRollmap.top, m_rectRollmap.Width(), m_rectRollmap.Height(), &memDC, 0, 0, SRCCOPY);


	memDC.DeleteDC();
	bitmap.DeleteObject();

	DeleteDC(memDC);
}

BOOL CRollMapInterface::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return __super::PreTranslateMessage(pMsg);
}

void CRollMapInterface::_SetScrollPosXY(BOOL bUseScroll)
{
	m_nRollMapOriX = GetScrollPos(SB_HORZ);
}

void CRollMapInterface::_SetScrollBar(double dx, double dy)
{
	BOOL bUseScroll = FALSE;
	CRect rect;
	GetClientRect(rect);

	m_nRollMapOriX = GetScrollPos(SB_HORZ);

	if (m_dViewsizex * m_dSfx - rect.Width() > 0)
	{
		SetScrollRange(SB_HORZ, 0, int(m_dViewsizex * m_dSfx - rect.Width()), TRUE);
		SetScrollPos(SB_HORZ, m_nRollMapOriX);

		bUseScroll = true;
	}
	else
	{
		SetScrollRange(SB_HORZ, 0, 0, FALSE);
		bUseScroll = false;
		m_nRollMapOriX = 0;
	}

	_SetScrollPosXY(bUseScroll);
}



void CRollMapInterface::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_ENDSCROLL) return;

	switch (nSBCode)
	{
	case SB_LINEDOWN: // 왼쪽 끝의 버튼을 누른경우
	{
		SetScrollPos(SB_HORZ, (int)(GetScrollPos(SB_HORZ) + 1));

		break;
	}

	case SB_LINEUP: // 오른쪽 끝을 누른경우
	{
		SetScrollPos(SB_HORZ, (int)(GetScrollPos(SB_HORZ) - 1));

		break;
	}

	case SB_PAGEDOWN: // 왼쪽 몸통부분을 누른경우
	{
		//scriInfo.nPos -= scriInfo.nPage;
		break;
	}

	case SB_PAGEUP: // 오른쪽 몸통부분을 누른경우
	{
		//scriInfo.nPos += scriInfo.nPage;
		break;
	}

	case SB_THUMBTRACK: // 절대적인 위치로 스크롤된경우
	{
		SetScrollPos(SB_HORZ, nPos);
		//scriInfo.nPos = scriInfo.nTrackPos;
		break;
	}
	}

	m_nRollMapOriX = nPos;

	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}

