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
void CControl::ControlToParent(CRect& r)
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
	const CString& text) : CControl(parent, x, y, width, height)
{
	Font.CreateFont(	30,                        // nHeight
						0,                         // nWidth
						0,                         // nEscapement
						0,                         // nOrientation
						FW_BOLD,					// nWeight
						FALSE,                     // bItalic
						FALSE,                     // bUnderline
						0,                         // cStrikeOut
						ANSI_CHARSET,              // nCharSet
						OUT_DEFAULT_PRECIS,        // nOutPrecision
						CLIP_DEFAULT_PRECIS,       // nClipPrecision
						DEFAULT_QUALITY,           // nQuality
						DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
						_T("Comic Sans MS"));          // lpszFacename
	Text = text;
}
void CTextView::DrawText(CDC* dc)
{
	ASSERT(dc);
	dc->FillSolidRect(&Rect, NULL);
	dc->SetBkColor(COLOR_BLACK);
	CFont* old_font = dc->SelectObject(&Font);
	dc->SetTextColor(RGB(255,0,0));
	dc->DrawText(Text, &Rect, DT_CENTER|DT_SINGLELINE);
	dc->SelectObject(old_font);
}
void CTextView::SetText(const CString& text)
{
	Text = text;
	CDC* dc = ParentWnd->GetDC();
	if(dc)
	{
		DrawText(dc);
		ParentWnd->ReleaseDC(dc);
	}
}
void CTextView::SetText(int i)
{
	CString s;
	s.Format(_T("%d"), i);
	SetText(s);
}
void CTextView::OnPaint(CDC* dc)
{
	DrawText(dc);
}

CNextShapeView::CNextShapeView(CWnd* parent, int x, int y) : 
	CControl(parent, x, y, NEXT_SHAPE_VIEW_WIDTH, NEXT_SHAPE_VIEW_HEIGHT)
{
	Shape.Clear();
	ShowShape = true;
}
void CNextShapeView::DrawBlock(CDC* dc, int x, int y)
{
	ASSERT(dc);
	CRect r;
	r.SetRect(	spacing + x*NEXT_SHAPE_BLOCK_SIZE + 1, 
				spacing + y*NEXT_SHAPE_BLOCK_SIZE + 1, 
				spacing + x*NEXT_SHAPE_BLOCK_SIZE + NEXT_SHAPE_BLOCK_SIZE - 1, 
				spacing + y*NEXT_SHAPE_BLOCK_SIZE + NEXT_SHAPE_BLOCK_SIZE - 1);
	ControlToParent(r);
	dc->FillSolidRect(&r, Shape.GetColor());
	dc->Draw3dRect(&r, COLOR_WHITE, COLOR_BLACK);
}
void CNextShapeView::SetShape(const CShape& shape)
{
	Shape.Assign(shape);
	spacing = (NEXT_SHAPE_VIEW_WIDTH - NEXT_SHAPE_BLOCK_SIZE*Shape.GetSize())/2;
	RePaint();
}
void CNextShapeView::OnPaint(CDC* dc)
{
	dc->FillSolidRect(&Rect, RGB(0x00,0x00,0x00));

	if(ShowShape)
	{
		CPoint block;
		for(int i=0;i<Shape.GetBlocksCount();i++)
		{
			block = Shape.GetBlockPoint(i, true);
			DrawBlock(dc, block.x, block.y);
		}
	}
}
void CNextShapeView::OnPause(bool pause)
{
	ShowShape = !pause;
	RePaint();
}
CGameFieldView::CGameFieldView(CWnd* parent, int x, int y, CGameField* game_field) : 
	CControl(parent, x, y, GAME_FIELD_VIEW_WIDTH, GAME_FIELD_VIEW_HEIGHT)
{
	ASSERT(game_field);
	GameField = game_field;
	State = GS_NO_GAME;
}
void CGameFieldView::DrawBlock(CDC* dc, int x, int y, COLORREF color)
{
	ASSERT(dc);
	CRect r;
	r.SetRect(x*BLOCK_VIEW_SIZE + 1, y*BLOCK_VIEW_SIZE + 1, x*BLOCK_VIEW_SIZE + 
		BLOCK_VIEW_SIZE - 1, y*BLOCK_VIEW_SIZE + BLOCK_VIEW_SIZE - 1);
	ControlToParent(r);
	dc->FillSolidRect(&r, color);
	if(color)
		dc->Draw3dRect(&r, COLOR_WHITE, COLOR_BLACK);
}
void CGameFieldView::PaintShape(CDC* dc, const CShape& shape, COLORREF color)
{
	ASSERT(dc);
	for(int i=0;i<shape.GetBlocksCount();i++)
		DrawBlock(dc, shape.GetBlockPoint(i, false).x, shape.GetBlockPoint(i, false).y, color);
}
void CGameFieldView::PaintText(CDC* dc, CString text)
{	
	CFont font;
	font.CreateFont(	64,                        // nHeight
						0,                         // nWidth
						0,                         // nEscapement
						0,                         // nOrientation
						FW_BOLD,					// nWeight
						FALSE,                     // bItalic
						FALSE,                     // bUnderline
						0,                         // cStrikeOut
						ANSI_CHARSET,              // nCharSet
						OUT_DEFAULT_PRECIS,        // nOutPrecision
						CLIP_DEFAULT_PRECIS,       // nClipPrecision
						DEFAULT_QUALITY,           // nQuality
						DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
						_T("Comic Sans MS"));          // lpszFacename
	CFont* oldfont = dc->SelectObject(&font);
	dc->SetBkColor(RGB(200, 150, 50));
	dc->DrawText(text, &Rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	dc->SelectObject(oldfont);
}
void CGameFieldView::OnPaint(CDC* dc)
{
	dc->FillSolidRect(&Rect, NULL);
	
	if(State == GS_PAUSED)
	{
		PaintText(dc, CString("Paused"));
		return;
	}
	COLORREF color;
	for(int x=0;x<GAME_FIELD_WIDTH;x++)
	{
		for(int y=0;y<GAME_FIELD_HEIGHT;y++)
		{
			color = GameField->GetBlockAt(x, y);
			if(color)
				DrawBlock(dc, x, y, color);
		}
	}
	if(State == GS_GAME_OVER)
		PaintText(dc, CString("Game over"));
}
void CGameFieldView::OnNewGame()
{
	State = GS_RUNNING;
	RePaint();
}
void CGameFieldView::OnShapeMove()
{
	CDC* dc = ParentWnd->GetDC();
	//erase previous shape
	PaintShape(dc, PrevShape, NULL);
	//paint current shape
	PaintShape(dc, GameField->Shape, GameField->Shape.GetColor());
	//save shape
	PrevShape.Assign(GameField->Shape);
	ParentWnd->ReleaseDC(dc);
}
void CGameFieldView::OnShapeLanded()
{
	CDC* dc = ParentWnd->GetDC();
	//paint current shape
	PaintShape(dc, GameField->Shape, GameField->Shape.GetColor());
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
	if(pause)
		State = GS_PAUSED;
	else
		State = GS_RUNNING;
	RePaint();
}