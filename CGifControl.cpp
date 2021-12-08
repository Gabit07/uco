#include "CGifControl.h"


CGifControl::CGifControl()
{
	this->_image = nullptr;
	this->_currentFrame = 0;
	this->_framecount = 0;
	this->_totalFrame = 0;
	this->_IsPlay = false;
	this->_demension_ids = nullptr;
	this->_property_item = nullptr;
}
CGifControl::~CGifControl()
{
	_Stop();
	_Clear();
}

BEGIN_MESSAGE_MAP(CGifControl, CStatic)
	ON_WM_TIMER()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

bool CGifControl::verify() const 
{

	bool result = false;

	do {

		if (nullptr == _image) {

			break;

		}

		result = true;

	} while (false);

	return result;

}



bool CGifControl::LoadGifImage(LPCTSTR _filename)
{
	bool result = false;

	do {

		if (this->_image != nullptr)
		{
			_Clear();
		}

		this->_image = Gdiplus::Image::FromFile(_filename);

		if (Gdiplus::Ok != this->_image->GetLastStatus())
		{
			delete this->_image;
			this->_image = nullptr;
			break;
		}

		UINT frame_dimension_count = this->_image->GetFrameDimensionsCount();
		if (frame_dimension_count <= 0) {
			break;
		}
		_demension_ids = (GUID*)new GUID[frame_dimension_count];
		if (_demension_ids == nullptr)
			break;
		_image->GetFrameDimensionsList(_demension_ids, frame_dimension_count);

		wchar_t guid_string[39] = L"";

		StringFromGUID2(_demension_ids[0], guid_string, 39);

		_framecount = _image->GetFrameCount(&_demension_ids[0]);

		UINT total_buffer = _image->GetPropertyItemSize(PropertyTagFrameDelay);

		_property_item = (Gdiplus::PropertyItem*)new char[total_buffer];

		_image->GetPropertyItem(PropertyTagFrameDelay, total_buffer, _property_item);

		result = true;
	
	} while (false);

	return result;
}

void CGifControl::_Clear()
{
	if(this->m_hWnd)
	    KillTimer(_TIMER_GIF);
	
	if (nullptr != _image) {

		delete _image;

		_image = nullptr;

	}

	if (nullptr != _demension_ids) {

		delete _demension_ids;

		_demension_ids = nullptr;

	}

	if (nullptr != _property_item) {

		delete _property_item;

		_property_item = nullptr;

	}

}

void CGifControl::Play()
{
	do {

		if (_IsPlay) {

			break;

		}

		if (nullptr == _image) {

			break;

		}

		_currentFrame = 0;

		GUID guid = Gdiplus::FrameDimensionTime;

		_image->SelectActiveFrame(&guid, _currentFrame);

		UINT elapse = ((UINT*)(_property_item->value))[_currentFrame] * 10;

		SetTimer(_TIMER_GIF, elapse, nullptr);

		_currentFrame++;

		Invalidate(FALSE);

		_IsPlay = true;

	} while (0);
	
}
void CGifControl::_Stop()
{
	do {
		if (false == _IsPlay) {

			return;

		}

		if(this->m_hWnd)
		    KillTimer(_TIMER_GIF);

		_IsPlay = false;

	} while (0);
	
}

void CGifControl::PreSubclassWindow()
{
	ModifyStyle(0, SS_OWNERDRAW | SS_NOTIFY);

	CStatic::PreSubclassWindow();
}


void CGifControl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == _TIMER_GIF) {

		KillTimer(nIDEvent);

		GUID guid = Gdiplus::FrameDimensionTime;

		_image->SelectActiveFrame(&guid, _currentFrame);

		UINT elapse = ((UINT*)(_property_item->value))[_currentFrame] * 10;

		SetTimer(_TIMER_GIF, elapse, nullptr);

		_currentFrame = (++_currentFrame) % _framecount;
		++_totalFrame;
		Invalidate(FALSE);

	}
	CStatic::OnTimer(nIDEvent);
}


void CGifControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	Gdiplus::Graphics grapics(lpDrawItemStruct->hDC);

	CRect rect;

	GetClientRect(&rect);

	grapics.DrawImage(_image, rect.left, rect.top, rect.Width(), rect.Height());

}
