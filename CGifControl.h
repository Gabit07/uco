#pragma once

#include "../pch.h"
#include "../common/configure.h"
#include "../common/CommonDefine.h"



class CGifControl : public CStatic
{
public:

	CGifControl();
	virtual ~CGifControl();

private:

	Gdiplus::Image* _image;
	GUID* _demension_ids;
	UINT _framecount;
	UINT _currentFrame;
	UINT _totalFrame;
	Gdiplus::PropertyItem* _property_item;
	bool _IsPlay;

	bool verify() const;
	void _Clear();
	void _Stop();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	bool LoadGifImage(LPCTSTR _filename);
	void Play();
	int  getStopFrame() const { return _totalFrame; }

protected:

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct = NULL);
	virtual void PreSubclassWindow();

};

