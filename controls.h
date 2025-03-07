#pragma once

constexpr UINT BLOCK_VIEW_SIZE = 24;
constexpr UINT GAME_FIELD_VIEW_WIDTH = (BLOCK_VIEW_SIZE * GAME_FIELD_WIDTH);
constexpr UINT GAME_FIELD_VIEW_HEIGHT = (BLOCK_VIEW_SIZE * GAME_FIELD_HEIGHT);
constexpr UINT TEXT_VIEW_WIDTH = 100;
constexpr UINT TEXT_VIEW_HEIGHT = 30;

constexpr UINT NEXT_SHAPE_BLOCK_SIZE = 16;
constexpr UINT NEXT_SHAPE_VIEW_WIDTH = 100;
constexpr UINT NEXT_SHAPE_VIEW_HEIGHT = 100;

//colors
constexpr COLORREF COLOR_WHITE = RGB(255, 255, 255);
constexpr COLORREF COLOR_BLACK = RGB(0, 0, 0);

class CControl
{
protected:
	CRect Rect;
	CWnd* ParentWnd;
	void ControlToParent(CRect& r);
public:
	CControl(CWnd* parent, int x, int y, int width, int height);
//events
	virtual void OnPaint(CDC* dc) = 0;
	void RePaint();
};
class CTextView : public CControl
{
private:
	CFont Font;
	CString Text;
	void DrawText(CDC* dc);
public:
	CTextView(CWnd* parent, int x, int y, int width, int height, const CString& text);
	void SetText(const CString& text);
	void SetText(int i);
	virtual void OnPaint(CDC* dc);
};
class CNextShapeView : public CControl
{
private:
	CShape Shape;
	bool ShowShape;
	int spacing;
	void DrawBlock(CDC* dc, int x, int y);
public:
	CNextShapeView(CWnd* parent, int x, int y);
	void SetShape(const CShape& shape);
	virtual void OnPaint(CDC* dc);
	void OnPause(bool pause);
};
class CGameFieldView : public CControl
{
private:
	int State;
	CShape PrevShape;
	CGameField* GameField;
	void DrawBlock(CDC* dc, int x, int y, COLORREF color);
	void PaintText(CDC* dc, CString text);
	void PaintShape(CDC* dc, const CShape& shape, COLORREF color);
public:
	CGameFieldView(CWnd* parent, int x, int y, CGameField* game_field);
	virtual void OnPaint(CDC* dc);
	void OnNewGame();
	void OnPause(bool pause);
	void OnShapeMove();
	void OnShapeLanded();
	void OnLinesDelete();
	void OnGameOver();
};