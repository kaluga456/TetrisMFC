#pragma once

constexpr UINT SHAPE_MAX_SIZE = 4;
constexpr UINT SHAPE_MAX_BLOCKS = 4;
constexpr UINT GAME_FIELD_WIDTH = 12;
constexpr UINT GAME_FIELD_HEIGHT = 24;

//game state
enum : UINT
{
	GS_NO_GAME = 0,
	GS_RUNNING,
	GS_PAUSED,
	GS_GAME_OVER
};

//shape movement types
enum : UINT
{
	MT_UNDEFINED = 0,
	MT_NEW_SHAPE,
	MT_MOVE_DOWN,
	MT_MOVE_RIGHT,
	MT_MOVE_LEFT,
	MT_ROTATE_LEFT,
	MT_ROTATE_RIGHT
};

typedef void (*EVENTPROC)(int event, int param);

class CShape
{
public:
	CPoint Point;

	CShape() {}
	void Assign(const CShape& shape);
	void Generate(const CPoint& p);
	void Clear() {BlocksCount = 0;}
	int GetSize() const {return Size;}
	int GetBlocksCount() const {return BlocksCount;}
	COLORREF GetColor() const {return Color;}
	COLORREF GetBlockAt(const CPoint& p) const
	{
		for(int i = 0; i < BlocksCount; ++i)
			if(GetBlockPoint(i, false) == p)
				return Color;
		return 0;
	}
	CPoint GetBlockPoint(int block_index, bool relative) const
	{
		ASSERT((block_index >= 0) && (block_index < BlocksCount));
		if(relative)
			return CPoint(Blocks[block_index].x, Blocks[block_index].y);
		return CPoint(Blocks[block_index].x + Point.x, Blocks[block_index].y + Point.y);
	}
	void Rotate(UINT direction);

private:
	int Size{};							//matrix size
	int BlocksCount{};
	COLORREF Color{};
	CPoint Blocks[SHAPE_MAX_BLOCKS];	//relative coordinates
};

struct CGameField
{
public:
	int Score{};
	int LinesCount{};
	int BonusPercent{};
	CShape Shape;
	CShape NextShape;
	int GetTotalScore() const {return Score;}
	int GetLinesCount() const {return LinesCount;}
	int GetBonusPercent() const {return BonusPercent;}
	enum EVENTS
	{	
		ON_NEW_SHAPE, 
		ON_SHAPE_MOVE, 
		ON_SHAPE_LANDED, 
		ON_LINES_DELETE, 
		ON_GAME_OVER
	};
	COLORREF GetLandedBlockAt(int x, int y) const
	{
		if(LandedBlocks[x][y])
			return LandedBlocks[x][y];
		return NULL;
	}
	COLORREF GetBlockAt(int x, int y) const
	{
		if(LandedBlocks[x][y])
			return LandedBlocks[x][y];
		return Shape.GetBlockAt(CPoint(x, y));
	}
	void Initialize(EVENTPROC event_proc);

	//IN events
	void OnShapeMove(UINT type);
	void OnDrop();
	void OnTimer();
	void OnNewGame();

private:
	EVENTPROC EventProc;
	COLORREF LandedBlocks[GAME_FIELD_WIDTH][GAME_FIELD_HEIGHT];	//NULL is no block
	bool CheckShape(const CShape& shape) const;
	bool TestMove(int type, bool move, CShape& new_shape);
	void ShapeLanded();
	bool IsLineSolid(int y) const;
	void CutSolidLine(int line_y);
	void Clear();
};

struct CGameTimer
{
public:
	void OnNewGame();
	void OnPause(bool pause);
	CTimeSpan GetGameSpan() const;
	CTime GetStartTime() const;
	CString GetGameSpanString() const;

private:
	CTime StartTime;
	CTime PauseTime;
	CTimeSpan PauseSpan;
};