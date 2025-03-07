#pragma once

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

//DEPRECATE: use return value
typedef void (*EVENTPROC)(int event, int param);

//game field sizes
constexpr std::byte MIN_FIELD_SIZE = static_cast<std::byte>(12);
constexpr std::byte MAX_FIELD_SIZE = std::numeric_limits<std::byte>::max();
constexpr UINT GAME_FIELD_WIDTH = 12;
constexpr UINT GAME_FIELD_HEIGHT = 24;

//game field coordinates
using coord_t = unsigned char;
struct Point
{
	Point() {}
	Point(coord_t x, coord_t y) : X{ x }, Y{ y } {}
	Point(int x, int y) : X{ static_cast<coord_t>(x) }, Y{ static_cast<coord_t>(y) } {}

	coord_t X{0};
	coord_t Y{0};

	Point& operator=(const Point& rop)
	{
		X = rop.X;
		Y = rop.Y;
		return *this;
	}
	bool operator==(const Point& rop) const
	{
		return (X == rop.X) && (Y == rop.Y);
	}
};

constexpr COLORREF NO_BLOCK = 0;
constexpr UINT SHAPE_MAX_SIZE = 4;
constexpr UINT SHAPE_BLOCKS_COUNT = 4;

class CShape
{
public:
	CShape() {}

	void Assign(const CShape& shape);
	void Generate(const Point& p);
	void Clear() {BlocksCount = 0;}
	int GetSize() const {return Size;}
	int GetBlocksCount() const {return BlocksCount;}
	COLORREF GetColor() const {return Color;}
	COLORREF GetBlockAt(const Point& p) const;
	Point GetBlockPoint(size_t block_index, bool relative) const;
	void Rotate(UINT direction);

	Point Pos;

private:
	int Size{};							//matrix size
	int BlocksCount{};					//TODO: always 4
	COLORREF Color{};					//context
	Point Blocks[SHAPE_BLOCKS_COUNT];	//relative coordinates
};

struct CGameField
{
public:
	enum EVENTS
	{	
		ON_NEW_SHAPE, 
		ON_SHAPE_MOVE, 
		ON_SHAPE_LANDED, 
		ON_LINES_DELETE, 
		ON_GAME_OVER
	};
	COLORREF GetLandedBlockAt(int x, int y) const { return Blocks[x][y]; }
	COLORREF GetBlockAt(int x, int y) const
	{
		if(Blocks[x][y])
			return Blocks[x][y];
		return CurrentShape.GetBlockAt(Point(x, y));
	}
	void Initialize(EVENTPROC event_proc);

	//IN events
	void OnShapeMove(UINT type);
	void OnDrop();
	void OnTimer();
	void OnNewGame();

	//TODO: input events
	//void OnTick() {}
	//void OnShapeMove() {}
	//void OnDrop();
	//void OnRotate();
	//void OnNewGame();
	//void GetState() const {}

	//TODO: move to statistic module
	int Score{};
	int LinesCount{};
	int BonusPercent{};
	int GetTotalScore() const { return Score; }
	int GetLinesCount() const { return LinesCount; }
	int GetBonusPercent() const { return BonusPercent; }

	const CShape& GetCurrentShape() const { return CurrentShape; }
	const CShape& GetNextShape() const { return NextShape; }

private:
	bool CheckShape(const CShape& shape) const;
	bool TestMove(int type, bool move, CShape& new_shape);
	void ShapeLanded();
	bool IsLineSolid(int y) const;
	void EraseLine(int line_y);
	void Clear();

	EVENTPROC EventProc;
	COLORREF Blocks[GAME_FIELD_WIDTH][GAME_FIELD_HEIGHT];	//NULL is no block

	CShape CurrentShape;
	CShape NextShape;
};
