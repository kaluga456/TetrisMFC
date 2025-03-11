#pragma once

constexpr UINT BLOCK_VIEW_SIZE = 28;
constexpr UINT GAME_FIELD_VIEW_WIDTH = (BLOCK_VIEW_SIZE * GAME_FIELD_WIDTH);
constexpr UINT GAME_FIELD_VIEW_HEIGHT = (BLOCK_VIEW_SIZE * GAME_FIELD_HEIGHT);
constexpr UINT TEXT_VIEW_WIDTH = 100;
constexpr UINT TEXT_VIEW_HEIGHT = 30;

constexpr UINT NEXT_SHAPE_BLOCK_SIZE = 16;
constexpr UINT NEXT_SHAPE_VIEW_WIDTH = 100;
constexpr UINT NEXT_SHAPE_VIEW_HEIGHT = 100;

constexpr UINT APPWND_PADDING = 12;
constexpr UINT APPWND_WIDTH = (APPWND_PADDING + TEXT_VIEW_WIDTH + APPWND_PADDING + GAME_FIELD_VIEW_WIDTH + APPWND_PADDING);
constexpr UINT APPWND_HEIGHT = (APPWND_PADDING + GAME_FIELD_VIEW_HEIGHT + APPWND_PADDING);

//colors
constexpr COLORREF COLOR_WHITE = RGB(255, 255, 255);
constexpr COLORREF COLOR_BLACK = RGB(0, 0, 0);

class CControl
{
public:
	CControl(CWnd* parent, int x, int y, int width, int height);
	virtual void OnPaint(CDC* dc) = 0;
	void RePaint();

	int Top() const { return Rect.top; }
	int Bottom() const { return Rect.bottom; }
	int Left() const { return Rect.left; }
	int RIght() const { return Rect.right; }
	int Width() const { return Rect.Width(); }
	int Height() const { return Rect.Height(); }

protected:
	CRect Rect;
	CWnd* ParentWnd;
	void ControlToParent(CRect& r) const;
};

class CTextView : public CControl
{
public:
	CTextView(CWnd* parent, int x, int y, int width, int height, const LPCWSTR text = nullptr);
	void SetText(const CString& text);
	void SetText(int i);
	void SetTime(UINT time); //ms
	virtual void OnPaint(CDC* dc);

private:
	CFont Font;
	CString Text;
	void DrawText(CDC* dc);
};

class CNextShapeView : public CControl
{
public:
	CNextShapeView(CWnd* parent, int x, int y);
	void SetShape(const CShape& shape);
	virtual void OnPaint(CDC* dc);

private:
	CShape Shape;
	bool ShowShape;
	int Spacing{0};
	void DrawBlock(CDC* dc, int x, int y);
};
class CGameFieldView : public CControl
{
public:
	CGameFieldView(CWnd* parent, int x, int y, const CGameField* game_field);
	virtual void OnPaint(CDC* dc);

	//TODO: move to main module
	void OnNewGame();
	void OnPause(bool pause);
	void OnShapeMove();
	void OnShapeLanded();
	void OnLinesDelete();
	void OnGameOver();

private:
	int State;
	CFont Font;
	CShape PrevShape;
	const CGameField* GameField;

	void DrawBlock(CDC* dc, int x, int y, COLORREF color);
	void PaintText(CDC* dc, CString text);
	void PaintShape(CDC* dc, const CShape& shape, COLORREF color);
};