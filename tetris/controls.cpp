#include "stdafx.h"
#pragma hdrstop
#include "resource.h"
#include "logic.h"
#include "controls.h"

CControl::CControl(CWnd* parent, int x, int y, int width, int height)
{
	ParentWnd = parent;
	Rect.left = x;
	Rect.top = y;
	Rect.right = x + width;
	Rect.bottom = y + height;
}
void CControl::ControlToParent(CRect& r) const
{
	r.left = Rect.left + r.left;
	r.right = Rect.left + r.right;
	r.top = Rect.top + r.top;
	r.bottom = Rect.top + r.bottom;
}
void CControl::RePaint()
{
	ParentWnd->InvalidateRect(Rect);
	ParentWnd->UpdateWindow();
}
CTextView::CTextView(CWnd* parent, int x, int y, int width, int height, 
	const LPCWSTR text /*= nullptr*/) : CControl(parent, x, y, width, height), Text(text ? text : L"")
{
	Font.CreateFont(	30,                        // nHeight
						0,                         // nWidth
						0,                         // nEscapement
						0,                         // nOrientation
						FW_BOLD,				   // nWeight
						FALSE,                     // bItalic
						FALSE,                     // bUnderline
						0,                         // cStrikeOut
						ANSI_CHARSET,              // nCharSet
						OUT_DEFAULT_PRECIS,        // nOutPrecision
						CLIP_DEFAULT_PRECIS,       // nClipPrecision
						DEFAULT_QUALITY,           // nQuality
						DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
						_T("Comic Sans MS"));      // lpszFacename
}
void CTextView::DrawText(CDC* dc)
{
	ASSERT(dc);
	dc->FillSolidRect(&Rect, NULL);
	dc->SetBkColor(COLOR_BLACK);
	CFont* old_font = dc->SelectObject(&Font);
	dc->SetTextColor(RGB(200, 150, 50));
	dc->DrawText(Text, &Rect, DT_CENTER|DT_SINGLELINE);
	dc->SelectObject(old_font);
}
void CTextView::SetText(const CString& text)
{
	Text = text;
	RePaint();
}
void CTextView::SetText(int i)
{
	CString s;
	s.Format(_T("%d"), i);
	SetText(s);
}
void CTextView::SetTime(UINT time)
{
	CString s;
	time /= 1000;
	UINT hours = time / 3600;
	UINT mins = (time / 60) % 60;
	UINT secs = time % 60;
	s.Format(_T("%02u:%02u:%02u"), hours, mins, secs);
	SetText(s);
}
void CTextView::OnPaint(CDC* dc)
{
	DrawText(dc);
}

CNextShapeView::CNextShapeView(CWnd* parent, int x, int y) : CControl(parent, x, y, NEXT_SHAPE_VIEW_WIDTH, NEXT_SHAPE_VIEW_HEIGHT)
{
	Shape.Clear();
	ShowShape = true;
}
void CNextShapeView::DrawBlock(CDC* dc, int x, int y)
{
	ASSERT(dc);
	CRect r;
	r.SetRect(	Spacing + x * NEXT_SHAPE_BLOCK_SIZE + 1, 
				Spacing + y * NEXT_SHAPE_BLOCK_SIZE + 1, 
				Spacing + x * NEXT_SHAPE_BLOCK_SIZE + NEXT_SHAPE_BLOCK_SIZE - 1, 
				Spacing + y * NEXT_SHAPE_BLOCK_SIZE + NEXT_SHAPE_BLOCK_SIZE - 1);
	ControlToParent(r);
	dc->FillSolidRect(&r, Shape.GetContext());
	dc->Draw3dRect(&r, COLOR_WHITE, COLOR_BLACK);
}
void CNextShapeView::SetShape(const CShape& shape)
{
	Shape.Assign(shape);
	Spacing = (NEXT_SHAPE_VIEW_WIDTH - NEXT_SHAPE_BLOCK_SIZE * Shape.GetSize()) / 2;
	RePaint();
}
void CNextShapeView::OnPaint(CDC* dc)
{
	dc->FillSolidRect(&Rect, COLOR_BLACK);

	if (false == ShowShape)
		return;

	for(int i = 0; i < Shape.GetBlocksCount(); ++i)
	{
		const Point block = Shape.GetBlockPoint(i, true);
		DrawBlock(dc, block.X, block.Y);
	}
}
CGameFieldView::CGameFieldView(CWnd* parent, int x, int y, const CGameField* game_field) :
	CControl(parent, x, y, GAME_FIELD_VIEW_WIDTH, GAME_FIELD_VIEW_HEIGHT), GameField{game_field}, State{GS_NO_GAME}
{
	ASSERT(game_field);
	Font.CreateFont(64,            // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,				   // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Comic Sans MS"));      // lpszFacename

	//TODO:
	//game field bitmap background
	//if (Backgorund.LoadBitmap(IDB_BACKGROUND))
	//{
	//	Backgorund.GetBitmap(&BgInfo);

	//	CDC* dc = ParentWnd->GetDC();
	//	BgMemory.CreateCompatibleDC(dc);
	//	ParentWnd->ReleaseDC(dc);
	//	BgMemory.SelectObject(&Backgorund);
	//}

}
void CGameFieldView::DrawBackground(CDC* dc)
{
	if (NULL == Backgorund.m_hObject)
	{
		dc->FillSolidRect(&Rect, NULL);
		return;
	}
	dc->StretchBlt(Rect.left, Rect.top, Rect.Width(), Rect.Height(), &BgMemory, 0, 0, BgInfo.bmWidth, BgInfo.bmHeight, SRCCOPY);
}
void CGameFieldView::DrawBlock(CDC* dc, int x, int y, COLORREF color)
{
	ASSERT(dc);
	CRect r;
	r.SetRect(	x * BLOCK_VIEW_SIZE + 1, y * BLOCK_VIEW_SIZE + 1, 
				x * BLOCK_VIEW_SIZE + BLOCK_VIEW_SIZE - 1, y * BLOCK_VIEW_SIZE + BLOCK_VIEW_SIZE - 1);
	ControlToParent(r);
	dc->FillSolidRect(&r, color);
	if(color)
		dc->Draw3dRect(&r, COLOR_WHITE, COLOR_BLACK);
}
void CGameFieldView::PaintShape(CDC* dc, const CShape& shape, COLORREF color)
{
	ASSERT(dc);
	for(int i = 0; i < shape.GetBlocksCount(); ++i)
		DrawBlock(dc, shape.GetBlockPoint(i, false).X, shape.GetBlockPoint(i, false).Y, color);
}
void CGameFieldView::PaintText(CDC* dc, CString text)
{	
	CFont* oldfont = dc->SelectObject(&Font);
	dc->SetBkColor(RGB(200, 150, 50));
	dc->DrawText(text, &Rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	dc->SelectObject(oldfont);
}
void CGameFieldView::OnPaint(CDC* dc)
{
	DrawBackground(dc);
	
	COLORREF color;
	for(int x = 0; x < GAME_FIELD_WIDTH; ++x)
	{
		for(int y = 0; y < GAME_FIELD_HEIGHT; ++y)
		{
			color = GameField->GetBlockAt(x, y);
			if(color)
				DrawBlock(dc, x, y, color);
		}
	}
	if(GS_PAUSED == State)
		PaintText(dc, L"Paused");
	else if(GS_GAME_OVER == State)
		PaintText(dc, L"Game over");
}
void CGameFieldView::OnNewGame()
{
	State = GS_RUNNING;
	RePaint();
}
void CGameFieldView::OnShapeMove()
{
	if (Backgorund.m_hObject)
	{
		RePaint();
		return;
	}

	CDC* dc = ParentWnd->GetDC();
	//erase previous shape
	PaintShape(dc, PrevShape, NULL);
	//paint current shape
	PaintShape(dc, GameField->GetCurrentShape(), GameField->GetCurrentShape().GetContext());
	//save shape
	PrevShape.Assign(GameField->GetCurrentShape());
	ParentWnd->ReleaseDC(dc);
}
void CGameFieldView::OnShapeLanded()
{
	if (Backgorund.m_hObject)
	{
		RePaint();
		return;
	}

	CDC* dc = ParentWnd->GetDC();
	//paint current shape
	PaintShape(dc, GameField->GetCurrentShape(), GameField->GetCurrentShape().GetContext());
	//no prev shapes expected
	PrevShape.Clear();
	ParentWnd->ReleaseDC(dc);
}
void CGameFieldView::OnLinesDelete()
{
	RePaint();
}
void CGameFieldView::OnGameOver()
{
	State = GS_GAME_OVER;
	RePaint();
}
void CGameFieldView::OnPause(bool pause)
{
	State = pause ? GS_PAUSED  : GS_RUNNING;
	RePaint();
}

CGameControl::CGameControl() : CStatic()
{
	//create controls
	NextShapeView = new CNextShapeView(this,
		APPWND_PADDING,
		APPWND_PADDING);
	LinesLabel = new CTextView(this,
		APPWND_PADDING,
		NextShapeView->Bottom() + APPWND_PADDING,
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"SCORE");
	LinesView = new CTextView(this,
		APPWND_PADDING,
		LinesLabel->Bottom(),
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"");
	SpeedLabel = new CTextView(this,
		APPWND_PADDING,
		LinesView->Bottom() + APPWND_PADDING,
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"SPEED");
	SpeedView = new CTextView(this,
		APPWND_PADDING,
		SpeedLabel->Bottom(),
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"");
	TimeLabel = new CTextView(this,
		APPWND_PADDING,
		SpeedView->Bottom() + APPWND_PADDING,
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"TIME");
	TimeView = new CTextView(this,
		APPWND_PADDING,
		TimeLabel->Bottom(),
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"");
	GameFieldView = new CGameFieldView(this,
		APPWND_PADDING + NextShapeView->Width() + APPWND_PADDING,
		APPWND_PADDING, &GameField);
}
CGameControl::~CGameControl()
{
	delete LinesView;
	delete LinesLabel;
	delete SpeedView;
	delete SpeedLabel;
	delete TimeLabel;
	delete TimeView;
	delete GameFieldView;
	delete NextShapeView;
}
void CGameControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	//draw background
	CRect client_rect;
	GetClientRect(&client_rect);
	dc.FillSolidRect(&client_rect, ::GetSysColor(COLOR_WINDOW));

	//draw controls
	GameFieldView->OnPaint(&dc);
	NextShapeView->OnPaint(&dc);
	LinesView->OnPaint(&dc);
	LinesLabel->OnPaint(&dc);
	SpeedView->OnPaint(&dc);
	SpeedLabel->OnPaint(&dc);
	TimeLabel->OnPaint(&dc);
	TimeView->OnPaint(&dc);

	dc.Detach();
}
