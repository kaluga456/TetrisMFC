#include "stdafx.h"
#pragma hdrstop
#include "resource.h"
#include "TetrisEngine.h"
#include "options.h"
#include "Graphics.h"

extern tetris::engine TetrisGame;
extern COptions Options;
//////////////////////////////////////////////////////////////////////////////
//CGraphicView
BOOL CGraphicView::Create(const RECT& rect, CWnd* pParentWnd, COLORREF bg_color, UINT nID /*= 0xffff*/)
{
	BgColor = bg_color;
	return CStatic::Create(_T(""), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, rect, pParentWnd, nID);
}
BOOL CGraphicView::Create(const RECT& parent_rect, int x, int y, int width, int height, CWnd* pParentWnd, COLORREF bg_color, UINT nID /*= 0xffff*/)
{
	BgColor = bg_color;
	CRect r(x, y, x + width, y + height);
	r.OffsetRect(parent_rect.left, parent_rect.top);
	return CStatic::Create(_T(""), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, r, pParentWnd, nID);
}
int CGraphicView::Width() const
{
	CRect r;
	GetWindowRect(r);
	return r.Width();
}
int CGraphicView::Height() const
{
	CRect r;
	GetWindowRect(r);
	return r.Height();
}
void CGraphicView::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	Draw(dc, lpDrawItemStruct->rcItem);
	dc.Detach();
}
void CGraphicView::Repaint()
{
	Invalidate();
	UpdateWindow();
}
//////////////////////////////////////////////////////////////////////////////
//CTextView
void CTextView::Init(int font_size, LPCTSTR font_name, COLORREF font_color)
{
	Font.CreateFont(font_size,    // nHeight
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
		font_name);      // lpszFacename

	FontColor = font_color;
}
void CTextView::SetText(LPCTSTR text /*= L""*/)
{
	Text = text ? text : L"";
	Repaint();
}
void CTextView::SetText(LPCTSTR format, int value)
{
	Text.Format(format, value);
	Repaint();
}
void CTextView::Draw(CDC& dc, const RECT& rect)
{
	dc.FillSolidRect(&rect, BgColor);
	dc.SetBkColor(BgColor);
	CFont* old_font = dc.SelectObject(&Font);
	dc.SetTextColor(FontColor);
	RECT r(rect);
	r.left += APPWND_PADDING;
	dc.DrawText(Text, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	dc.SelectObject(old_font);
}
//////////////////////////////////////////////////////////////////////////////
//CMultiTextView
void CMultiTextView::Draw(CDC& dc, const RECT& rect)
{
	dc.FillSolidRect(&rect, BgColor);
	dc.SetBkColor(BgColor);
	CFont* old_font = dc.SelectObject(&Font);
	dc.SetTextColor(FontColor);
	RECT r(rect);
	r.left += APPWND_PADDING;
	r.top += APPWND_PADDING;
	dc.DrawText(Text, &r, DT_LEFT | DT_NOPREFIX | DT_EXPANDTABS);
	dc.SelectObject(old_font);
}
//////////////////////////////////////////////////////////////////////////////
//CShapeView
void CShapeView::GetShapeRect(const tetris::shape_matrix_t* matrix, CRect& rect)
{
	ASSERT(matrix);
	const tetris::shape_matrix_t& mx = *matrix;
	rect.left = tetris::SHAPE_MATRIX_SIZE - 1;
	rect.top = tetris::SHAPE_MATRIX_SIZE - 1;
	rect.right = 0;
	rect.bottom = 0;
	for (int x = 0; x < tetris::SHAPE_MATRIX_SIZE; ++x)
	{
		for (int y = 0; y < tetris::SHAPE_MATRIX_SIZE; ++y)
		{
			if (0 == mx[x][y])
				continue;
			if (rect.left > x) rect.left = x;
			if (rect.top > y) rect.top = y;
			if (rect.right < x) rect.right = x;
			if (rect.bottom < y) rect.bottom = y;
		}
	}
}
void CShapeView::SetShape(const tetris::shape_t* shape /*= nullptr*/)
{
	//no shape
	if (nullptr == shape)
	{
		Matrix = nullptr;
		Repaint();
		return;
	}

	//new shape
	Matrix = shape->get_matrix();
	Color = shape->get_block_type();

	Repaint();
}
void CShapeView::Draw(CDC& dc, const RECT& rect)
{
	//draw background
	dc.FillSolidRect(&rect, BgColor);

	//no shape to draw
	if (nullptr == Matrix)
		return;

	//draw shape
	CRect client_rect;
	GetClientRect(client_rect);
	const tetris::shape_matrix_t& matrix = *Matrix;

	//center shape in client rectangle
	CRect shape_rect;
	GetShapeRect(Matrix, shape_rect);

	//left offset + text width
	const int left_offset = 30 + (client_rect.Width() - (SHAPE_VIEW_BLOCK_SIZE * (shape_rect.Width() + 1))) / 2 - shape_rect.left * SHAPE_VIEW_BLOCK_SIZE;
	const int top_offset = (client_rect.Height() - (SHAPE_VIEW_BLOCK_SIZE * (shape_rect.Height() + 1))) / 2;

	for (int x = 0; x < tetris::SHAPE_MATRIX_SIZE; ++x)
	{
		for (int y = 0; y < tetris::SHAPE_MATRIX_SIZE; ++y)
		{
			
			if (0 == matrix[x][y])
				continue;

			CRect r{ 
				left_offset + x * SHAPE_VIEW_BLOCK_SIZE,
				top_offset + y * SHAPE_VIEW_BLOCK_SIZE,
				left_offset + (x + 1) * SHAPE_VIEW_BLOCK_SIZE,
				top_offset + (y + 1) * SHAPE_VIEW_BLOCK_SIZE };
			r.DeflateRect(BLOCK_DEFLATION, BLOCK_DEFLATION);

			dc.FillSolidRect(&r, Color);
			dc.Draw3dRect(&r, COLOR_WHITE, COLOR_BLACK);
		}
	}

	//draw text
	CFont* old_font = dc.SelectObject(&Font);
	dc.SetTextColor(SCORE_TEXT_COLOR);
	RECT r(rect);
	r.left += APPWND_PADDING;
	dc.DrawText(L"Next", &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	dc.SelectObject(old_font);
}
//////////////////////////////////////////////////////////////////////////////
//CGameView
void CGameView::Init()
{
	Font.CreateFont(FIELD_TEXT_SIZE,	// nHeight
		0,								// nWidth
		0,								// nEscapement
		0,								// nOrientation
		FW_BOLD,						// nWeight
		FALSE,							// bItalic
		FALSE,							// bUnderline
		0,								// cStrikeOut
		ANSI_CHARSET,					// nCharSet
		OUT_DEFAULT_PRECIS,				// nOutPrecision
		CLIP_DEFAULT_PRECIS,			// nClipPrecision
		DEFAULT_QUALITY,				// nQuality
		DEFAULT_PITCH | FF_SWISS,		// nPitchAndFamily
		FIELD_TEXT_FONT);				// lpszFacename
	IsGrid = Options.ShowGrid;
}
void CGameView::SetMainText(LPCTSTR text /*= nullptr*/)
{
	Text = text ? text : L"";
	Repaint();
}
void CGameView::ShowGrid()
{
	ShowGrid(!IsGrid);
}
void CGameView::ShowGrid(bool enable)
{
	if (IsGrid == enable)
		return;
	IsGrid = enable;
	Repaint();
}
void CGameView::DrawBlock(CDC& dc, int x, int y, tetris::block_t color)
{
	CRect r;
	if (tetris::BLOCK_NONE == color)
	{
		r.SetRect(x * BLOCK_VIEW_SIZE, y * BLOCK_VIEW_SIZE,
			x * BLOCK_VIEW_SIZE + BLOCK_VIEW_SIZE, y * BLOCK_VIEW_SIZE + BLOCK_VIEW_SIZE);
		dc.FillSolidRect(&r, BgColor);
		return;
	}

	r.SetRect(x * BLOCK_VIEW_SIZE, y * BLOCK_VIEW_SIZE,
		x * BLOCK_VIEW_SIZE + BLOCK_VIEW_SIZE, y * BLOCK_VIEW_SIZE + BLOCK_VIEW_SIZE);
	r.DeflateRect(BLOCK_DEFLATION, BLOCK_DEFLATION);

	dc.FillSolidRect(&r, color);
	dc.Draw3dRect(&r, COLOR_WHITE, COLOR_BLACK);
}
void CGameView::Draw(CDC& dc, const RECT& rect)
{
	//draw background
	dc.FillSolidRect(&rect, BgColor);

	//TEST: draw frame
	//CRect client_rect;
	//GetClientRect(&client_rect);
	//dc.DrawEdge(client_rect, EDGE_SUNKEN, BF_RECT);

	//draw grid
	if (true == IsGrid)
	{
		CPen pen(PS_SOLID, 1, COLOR_GRID);
		CPen* old_pen = dc.SelectObject(&pen);
		for (int x = 1; x < tetris::GAME_FIELD_WIDTH ; ++x)
		{
			dc.MoveTo(BLOCK_VIEW_SIZE * x, 0);
			dc.LineTo(BLOCK_VIEW_SIZE * x, GAME_FIELD_VIEW_HEIGHT);
		}
		for (int y = 1; y < tetris::GAME_FIELD_HEIGHT; ++y)
		{
			dc.MoveTo(0, BLOCK_VIEW_SIZE * y);
			dc.LineTo(GAME_FIELD_VIEW_WIDTH, BLOCK_VIEW_SIZE * y);
		}
		dc.SelectObject(old_pen);
	}

	//draw game blocks
	for (int x = 0; x < tetris::GAME_FIELD_WIDTH; ++x)
	{
		for (int y = 0; y < tetris::GAME_FIELD_HEIGHT; ++y)
		{
			const tetris::block_t new_block = TetrisGame.get_block(x, y);
			if (tetris::BLOCK_NONE == new_block)
				continue;
			DrawBlock(dc, x, y, new_block);
		}
	}

	//draw main text
	if (false == Text.IsEmpty())
	{
		const int SHADOW_OFFSET = 4;
		dc.SetBkMode(TRANSPARENT);
		CFont* oldfont = dc.SelectObject(&Font);
		CRect r(rect);
		r.OffsetRect(SHADOW_OFFSET, SHADOW_OFFSET);
		dc.SetTextColor(COLOR_BLACK);
		dc.DrawText(Text.GetString(), r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		r.OffsetRect(-SHADOW_OFFSET, -SHADOW_OFFSET);
		dc.SetTextColor(SCORE_TEXT_COLOR);
		dc.DrawText(Text.GetString(), r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		dc.SelectObject(oldfont);
	}
}
//////////////////////////////////////////////////////////////////////////////
//CGameTab
int CGameTab::GetWidth()
{
	return APPWND_PADDING + GAME_FIELD_VIEW_WIDTH + APPWND_PADDING + RIGHT_SIDE_WIDTH + APPWND_PADDING;
}
int CGameTab::GetHeight()
{
	return APPWND_PADDING + GAME_FIELD_VIEW_HEIGHT + APPWND_PADDING;
}
void CGameTab::SetScore(int score /*= -1*/)
{
	ScoreView.SetText(score < 0 ? L"Score          -" : L"Score       %4d", score);
}
void CGameTab::SetSpeed(int speed /*= -1*/)
{
	SpeedView.SetText(speed < 0 ? L"Speed          -" : L"Speed      %4d%%", speed);
}
void CGameTab::SetTime(int value /*= -1*/)
{
	if (value < 0)
	{
		TimeView.SetText(L"Time       --:--");
		return;
	}

	CString str;
	value /= 1000;
	int hours = value / 3600;
	int mins = (value / 60) % 60;
	int secs = value % 60;
	if(hours)
		str.Format(_T("Time    %02u:%02u:%02u"), hours, mins, secs);
	else
		str.Format(_T("Time       %02u:%02u"), mins, secs);
	TimeView.SetText(str);
}
BOOL CGameTab::Init(const CRect& rect, CWnd* parent)
{
	//init game view
	GameView.Create(rect, APPWND_PADDING, APPWND_PADDING, GAME_FIELD_VIEW_WIDTH, GAME_FIELD_VIEW_HEIGHT, parent, COLOR_GAME_BKGROUND);
	GameView.Init();

	//right side
	const int left_offset = APPWND_PADDING + GameView.Width() + APPWND_PADDING;
	const int right_side_width = rect.Width() - left_offset - APPWND_PADDING;
	int top_offset = 0;
	ShapeView.Create(rect, left_offset, APPWND_PADDING, right_side_width, SHAPE_VIEW_HEIGHT, parent, COLOR_GAME_BKGROUND);
	top_offset += APPWND_PADDING + ShapeView.Height();
	ScoreView.Create(rect, left_offset, top_offset + APPWND_PADDING, right_side_width, TEXT_VIEW_HEIGHT, parent, COLOR_GAME_BKGROUND);
	top_offset += ScoreView.Height();
	SpeedView.Create(rect, left_offset, top_offset + APPWND_PADDING, right_side_width, TEXT_VIEW_HEIGHT, parent, COLOR_GAME_BKGROUND);
	top_offset += SpeedView.Height();
	TimeView.Create(rect, left_offset, top_offset + APPWND_PADDING, right_side_width, TEXT_VIEW_HEIGHT, parent, COLOR_GAME_BKGROUND);
	top_offset += APPWND_PADDING + TimeView.Height();
	HelpView.Create(rect, left_offset, top_offset + APPWND_PADDING, right_side_width, 
		GameView.Height() - top_offset, parent, COLOR_GAME_BKGROUND);

	//set fonts
	ShapeView.Init(SCORE_TEXT_SIZE, SCORE_TEXT_FONT, SCORE_TEXT_COLOR);
	ScoreView.Init(SCORE_TEXT_SIZE, SCORE_TEXT_FONT, SCORE_TEXT_COLOR);
	SpeedView.Init(SCORE_TEXT_SIZE, SCORE_TEXT_FONT, SCORE_TEXT_COLOR);
	TimeView.Init(SCORE_TEXT_SIZE, SCORE_TEXT_FONT, SCORE_TEXT_COLOR);
	HelpView.Init(HELP_TEXT_SIZE, HELP_TEXT_FONT, SCORE_TEXT_COLOR);

	//set text
	SetScore();
	SetSpeed();
	SetTime();

	//help text
	constexpr LPCTSTR help_text =
		L"Controls:\n"\
		L"------------------\n"\
		L"New\tENTER\n"\
		L"Pause\tESC\n"\
		L"Rotate\tW or UP\n"\
		L"Left\tA or LEFT\n"\
		L"Down\tS or DOWN\n"\
		L"Right\tD or RIGHT\n"\
		L"Drop\tSPACE\n"\
		L"Grid\tG\n"\

		L"\n"\
		L"Bonus points:\n"\
		L"------------------\n"\
		L"1 line  - 0 points\n"\
		L"2 lines - 1 points\n"\
		L"3 lines - 2 points\n"\
		L"4 lines - 3 points\n";

	HelpView.SetText(help_text);

	return TRUE;
}
void CGameTab::Show(BOOL val /*= TRUE*/)
{
	const BOOL show = val ? SW_SHOW : SW_HIDE;
	GameView.ShowWindow(show);
	ShapeView.ShowWindow(show);
	ScoreView.ShowWindow(show);
	SpeedView.ShowWindow(show);
	TimeView.ShowWindow(show);
}

