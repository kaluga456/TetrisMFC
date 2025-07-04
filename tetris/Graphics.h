#pragma once
//////////////////////////////////////////////////////////////////////////////
//sizes
constexpr int BLOCK_VIEW_SIZE = 22;
constexpr int APPWND_PADDING = 8;
constexpr int GAME_FIELD_VIEW_WIDTH = BLOCK_VIEW_SIZE * tetris::GAME_FIELD_WIDTH;
constexpr int GAME_FIELD_VIEW_HEIGHT = BLOCK_VIEW_SIZE * tetris::GAME_FIELD_HEIGHT;
constexpr int RIGHT_SIDE_WIDTH = 200;
constexpr int TEXT_VIEW_HEIGHT = 28;
constexpr int SHAPE_VIEW_BLOCK_SIZE = 18;
constexpr int SHAPE_VIEW_HEIGHT = APPWND_PADDING + 4 * SHAPE_VIEW_BLOCK_SIZE + APPWND_PADDING;
constexpr int BLOCK_DEFLATION = 1;
//////////////////////////////////////////////////////////////////////////////
//colors
enum : COLORREF
{
	COLOR_BLACK = RGB(0, 0, 0),
	COLOR_WHITE = RGB(255, 255, 255),
	COLOR_GAME_BKGROUND = RGB(0x20, 0x20, 0x20),
	COLOR_GRID = RGB(0x60, 0x60, 0x60)
};
//////////////////////////////////////////////////////////////////////////////
//fonts
constexpr LPCTSTR FIELD_TEXT_FONT = _T("Tahoma");
constexpr int FIELD_TEXT_SIZE = 48;
constexpr LPCTSTR SCORE_TEXT_FONT = _T("Lucida Console");
constexpr int SCORE_TEXT_SIZE = 16;
constexpr COLORREF SCORE_TEXT_COLOR = RGB(0xE0, 0xE0, 0xE0);
constexpr LPCTSTR HELP_TEXT_FONT = _T("Consolas");
constexpr int HELP_TEXT_SIZE = 17;
//////////////////////////////////////////////////////////////////////////////
//base owner draw control
class CGraphicView : public CStatic
{
public:
	CGraphicView() : CStatic() {}

	BOOL Create(const RECT& rect, CWnd* pParentWnd, COLORREF bg_color, UINT nID = 0xFFFF);
	BOOL Create(const RECT& parent_rect, int x, int y, int width, int height, CWnd* pParentWnd, COLORREF bg_color, UINT nID = 0xFFFF);

	int Width() const;
	int Height() const;

	//draw
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override final;
	virtual void Draw(CDC& dc, const RECT& rect) = 0;
	void Repaint();

protected:
	COLORREF BgColor{};
};
//////////////////////////////////////////////////////////////////////////////
//owner draw text control
class CTextView : public CGraphicView
{
public:
	void Init(int font_size, LPCTSTR font_name, COLORREF font_color = COLOR_BLACK);
	void SetText(LPCTSTR text = L"");
	void SetText(LPCTSTR format, int value);

	void Draw(CDC& dc, const RECT& rect) override;

protected:
	CString Text;
	COLORREF FontColor{};
	CFont Font;
};
//////////////////////////////////////////////////////////////////////////////
//owner draw multiline text control
class CMultiTextView : public CTextView
{
public:
	void Draw(CDC& dc, const RECT& rect) override;
};
//////////////////////////////////////////////////////////////////////////////
//next shape view
class CShapeView : public CTextView
{
public:
	void SetShape(const tetris::shape_t* shape = nullptr);
	void Draw(CDC& dc, const RECT& rect) override;

private:
	const tetris::shape_matrix_t* Matrix{ nullptr };
	tetris::block_t Color{};

	static void GetShapeRect(const tetris::shape_matrix_t* matrix, CRect& rect);
};
//////////////////////////////////////////////////////////////////////////////
//game field view
class CGameView : public CGraphicView
{
public:
	CGameView() : CGraphicView() {}

	void Init();
	void SetMainText(LPCTSTR text = nullptr);
	void ShowGrid(); //switch
	void ShowGrid(bool enable);
	bool GetShowGrid() const { return IsGrid; }
	void Draw(CDC& dc, const RECT& rect) override;

private:
	//main text
	CFont Font;
	CString Text;

	bool IsGrid{ true };
	void DrawBlock(CDC& dc, int x, int y, tetris::block_t color);
};
//////////////////////////////////////////////////////////////////////////////
//game tab container
class CGameTab
{
public:
	//preferred size
	static int GetWidth();
	static int GetHeight();

	//views
	CGameView GameView;
	CShapeView ShapeView;

	CGameTab() {}

	void SetScore(int score = -1);
	void SetSpeed(int speed = -1);
	void SetTime(int value = -1); //ms

	BOOL Init(const CRect& rect, CWnd* pParentWnd);
	void Show(BOOL val = TRUE);

private:
	CTextView ScoreView;
	CTextView SpeedView;
	CTextView TimeView;
	CMultiTextView HelpView;
};
//////////////////////////////////////////////////////////////////////////////