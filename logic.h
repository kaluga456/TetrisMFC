#pragma once

//game state
enum : int
{
	GS_NO_GAME = 0,
	GS_RUNNING,
	GS_GAME_OVER,

	//TODO: move out
	GS_PAUSED
};

//shape movement types
enum : int
{
	MT_UNDEFINED = 0,
	MT_NEW_SHAPE,

	MT_MOVE_DOWN,
	MT_MOVE_RIGHT,
	MT_MOVE_LEFT,

	MT_ROTATE_LEFT,
	MT_ROTATE_RIGHT
};

//block context for outer code
using block_t = unsigned int;
constexpr block_t BLOCK_NONE = static_cast<block_t>(0);
constexpr block_t BLOCK_DEFAULT = static_cast<block_t>(1);

//game field coordinate
using coord_t = unsigned char;

//game field sizes
constexpr coord_t MIN_FIELD_SIZE = static_cast<coord_t>(12);
constexpr coord_t MAX_FIELD_SIZE = std::numeric_limits<coord_t>::max();
constexpr coord_t GAME_FIELD_WIDTH = 12;
constexpr coord_t GAME_FIELD_HEIGHT = 24;

//game field coordinates
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

constexpr coord_t SHAPE_MAX_SIZE = 4;
constexpr coord_t SHAPE_BLOCKS_COUNT = 4;

class CShape
{
public:
	CShape() {}

	void Assign(const CShape& shape);
	void Generate(const Point& p, block_t context);
	void Clear() {BlocksCount = 0;}
	int GetSize() const {return Size;}
	int GetBlocksCount() const {return BlocksCount;}
	block_t GetContext() const {return Context;}
	block_t GetBlockAt(const Point& p) const;
	Point GetBlockPoint(size_t block_index, bool relative) const;
	void Rotate(int direction);

	Point Pos;

private:
	int Size{};							//matrix size
	int BlocksCount{};					//TODO: always 4
	block_t Context{};					//blocks context
	Point Blocks[SHAPE_BLOCKS_COUNT];	//relative coordinates
};

struct CGameField
{
public:
	//event callback
	enum : int
	{	
		ON_NEW_SHAPE, 
		ON_SHAPE_MOVE, 
		ON_SHAPE_LANDED,
		ON_LINE_DELETE,		//param - deleted line y coord
		ON_LINES_DELETE,	//param - deleted lines count
		ON_GAME_OVER
	};
	typedef void (*FEventProc)(int event, int param);

	//callback for block context
	typedef block_t (*FGetBlockContext)();

	//init
	CGameField();
	~CGameField();
	void Initialize(FGetBlockContext get_block_context, FEventProc event_proc);

	//input events
	enum : int
	{
		//input event result
		RESULT_OK = 0,
		RESULT_BLOCKED,
		RESULT_FAIL
	};
	int OnNewGame();
	int OnMoveShape(int type);
	int OnDrop();
	int OnTick();

	//state
	block_t GetLandedBlockAt(int x, int y) const { return Blocks[x][y]; }
	block_t GetBlockAt(int x, int y) const;
	const CShape& GetCurrentShape() const { return CurrentShape; }
	const CShape& GetNextShape() const { return NextShape; }

private:
	//blocks input events while processing callback to prevent recursion
	class CCallbackBlock 
	{
	public:
		CCallbackBlock() { Blocked = true; }
		~CCallbackBlock() { Blocked = false; }
		static bool IsBlocked() { return Blocked; }
	private:
		static bool Blocked;
	};

	void Clear();

	//logic
	bool CheckShape(const CShape& shape) const;
	bool TestMove(int type, bool move);
	void ShapeLanded();
	bool IsLineSolid(int y) const;
	void EraseLine(int line_y);

	//callbacks
	FEventProc callback_EventProc{ nullptr };
	FGetBlockContext callback_GetBLockContext{ nullptr };
	void EventProc(int event, int param = 0) const;
	block_t GetBLockContext() const;

	CShape CurrentShape;
	CShape NextShape;
	block_t Blocks[GAME_FIELD_WIDTH][GAME_FIELD_HEIGHT];
};
