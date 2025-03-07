#include "stdafx.h"
#pragma hdrstop
#include "random.h"
#include "logic.h"

static app::random_value<int> RandomColor(100, 255);
static COLORREF GetRandomColor()
{
	const int r = RandomColor.get();
	const int g = RandomColor.get();
	const int b = RandomColor.get();
	return RGB(r, g, b);
}

enum : int
{
	SHAPE_TYPE_LINE = 0,
	SHAPE_TYPE_T,
	SHAPE_TYPE_LZ,
	SHAPE_TYPE_RZ,
	SHAPE_TYPE_SQUARE,
	SHAPE_TYPE_LL,
	SHAPE_TYPE_RL,

	SHAPE_TYPE_COUNT
};

const Point SHAPES[SHAPE_TYPE_COUNT][SHAPE_BLOCKS_COUNT] =
{
	{ {1,0}, {1,1},  {1,2},  {1,3} } //TYPE_LINE
	//TYPE_T
};

static app::random_value<int> RandomShape(0, SHAPE_TYPE_COUNT - 1);
void CShape::Assign(const CShape& shape)
{
	Size = shape.GetSize();
	BlocksCount = shape.GetBlocksCount();
	Color = shape.GetColor();
	Pos = shape.Pos;
	for(int i = 0; i < BlocksCount; ++i)
		Blocks[i] = shape.GetBlockPoint(i, true);
}
void CShape::Generate(const Point& p)
{
	Pos = p;
	//create shape
	BlocksCount = 4;
	switch(RandomShape.get())
	{
		case SHAPE_TYPE_LINE:
			Size = 4;
			Blocks[0] = {1,0};
			Blocks[1] = {1,1};
			Blocks[2] = {1,2};
			Blocks[3] = {1,3};
			break;
		case SHAPE_TYPE_T:
			Size = 3;
			Blocks[0] = {1,0};
			Blocks[1] = {0,1};
			Blocks[2] = {1,1};
			Blocks[3] = {2,1};
			break;
		case SHAPE_TYPE_LZ:
			Size = 3;
			Blocks[0] = {0,0};
			Blocks[1] = {0,1};
			Blocks[2] = {1,1};
			Blocks[3] = {1,2};
			break;
		case SHAPE_TYPE_RZ:
			Size = 3;
			Blocks[0] = {1,0};
			Blocks[1] = {0,1};
			Blocks[2] = {1,1};
			Blocks[3] = {0,2};
			break;
		case SHAPE_TYPE_SQUARE:
			Size = 2;
			Blocks[0] = {0,0};
			Blocks[1] = {0,1};
			Blocks[2] = {1,0};
			Blocks[3] = {1,1};
			break;
		case SHAPE_TYPE_LL:
			Size = 3;
			Blocks[0] = {0,0};
			Blocks[1] = {1,0};
			Blocks[2] = {1,1};
			Blocks[3] = {1,2};
			break;
		case SHAPE_TYPE_RL:
			Size = 3;
			Blocks[0] = {2,0};
			Blocks[1] = {1,0};
			Blocks[2] = {1,1};
			Blocks[3] = {1,2};
			break;
		default:
			ASSERT(FALSE);
	}

	Color = GetRandomColor();
}
COLORREF CShape::GetBlockAt(const Point& p) const
{
	for (size_t i = 0; i < BlocksCount; ++i)
		if (GetBlockPoint(i, false) == p)
			return Color;
	return NO_BLOCK;
}
Point CShape::GetBlockPoint(size_t block_index, bool relative) const
{
	ASSERT((block_index >= 0) && (block_index < BlocksCount));
	if (relative)
		return Point(Blocks[block_index].X, Blocks[block_index].Y);
	return Point(Blocks[block_index].X + Pos.X, Blocks[block_index].Y + Pos.Y);
}
void CShape::Rotate(UINT direction)
{
	ASSERT((MT_ROTATE_LEFT == direction) || (MT_ROTATE_RIGHT == direction));
	
	Point new_block;	//rotated block
	for(size_t i = 0; i < BlocksCount; ++i)
	{
		if(direction == MT_ROTATE_LEFT)
		{
			new_block.X = Size - (Blocks[i].Y + 1);
			new_block.Y = Blocks[i].X;
		}
		else if(direction == MT_ROTATE_RIGHT)
		{
			new_block.X = Blocks[i].Y;
			new_block.Y = Size - (Blocks[i].X + 1);
		}
		Blocks[i] = new_block;
	}
}
void CGameField::Clear()
{
	for(size_t x = 0; x < GAME_FIELD_WIDTH; ++x)
		for(size_t y = 0; y < GAME_FIELD_HEIGHT; ++y)
			Blocks[x][y] = NO_BLOCK;
}
void CGameField::Initialize(EVENTPROC event_proc)
{
	ASSERT(event_proc);
	EventProc = event_proc;
	Clear();
	Score = 0;
	LinesCount = 0;
	BonusPercent = 0;
}
void CGameField::OnNewGame()
{
	Clear();
	NextShape.Generate(Point(5, 0));
	CShape new_shape;
	TestMove(MT_NEW_SHAPE, true, new_shape);
	Score = 0;
	LinesCount = 0;
	BonusPercent = 0;
}
bool CGameField::CheckShape(const CShape& shape) const
{
	for(size_t i = 0; i < shape.GetBlocksCount(); ++i)
	{
		if(shape.GetBlockPoint(i, false).X < 0 || shape.GetBlockPoint(i, false).X >= GAME_FIELD_WIDTH)
			return false;
 		if(shape.GetBlockPoint(i, false).Y < 0 || shape.GetBlockPoint(i, false).Y >= GAME_FIELD_HEIGHT)
			return false;
		if(Blocks[shape.GetBlockPoint(i, false).X][shape.GetBlockPoint(i, false).Y])
			return false;
	}
	return true;
}
bool CGameField::TestMove(int type, bool move, CShape& new_shape)
{
	if(type != MT_NEW_SHAPE)
		new_shape.Assign(CurrentShape);

	switch(type)
	{
	case MT_NEW_SHAPE:
		new_shape.Assign(NextShape);
		NextShape.Generate(Point(5, 0));
		break;
	case MT_MOVE_LEFT:
		new_shape.Pos.X = CurrentShape.Pos.X - 1;
		break;
	case MT_MOVE_RIGHT:
		new_shape.Pos.X = CurrentShape.Pos.X + 1;
		break;
	case MT_MOVE_DOWN:
		new_shape.Pos.Y = CurrentShape.Pos.Y + 1;
		break;
	case MT_ROTATE_LEFT:
		new_shape.Rotate(MT_ROTATE_LEFT);
		break;
	case MT_ROTATE_RIGHT:
		new_shape.Rotate(MT_ROTATE_RIGHT);
		break;
	default:
		ASSERT(FALSE);	//move type is undefined
		return false;
	}
	if(CheckShape(new_shape))
	{
		if(move)
		{
			CurrentShape.Assign(new_shape);
			if(type == MT_NEW_SHAPE)
				EventProc(ON_NEW_SHAPE, NULL);
			EventProc(ON_SHAPE_MOVE, NULL);
		}
		return true;
	}
	if(type == MT_MOVE_DOWN)
		ShapeLanded();
	else if(type == MT_NEW_SHAPE)
		EventProc(ON_GAME_OVER, NULL);
	return false;
}
void CGameField::OnShapeMove(UINT type)
{
	if (MT_UNDEFINED == type)
		return;
	CShape new_shape;
	TestMove(type, true, new_shape);
}
void CGameField::ShapeLanded()	//called after each shape changing
{
	EventProc(ON_SHAPE_LANDED, NULL);	//paint landed shape first
	//fix landed shape
	for(int i = 0;i < CurrentShape.GetBlocksCount(); ++i)
	{
		ASSERT((CurrentShape.GetBlockPoint(i, false).X >= 0) && (CurrentShape.GetBlockPoint(i, false).X < GAME_FIELD_WIDTH));
		ASSERT((CurrentShape.GetBlockPoint(i, false).Y >= 0) && (CurrentShape.GetBlockPoint(i, false).Y < GAME_FIELD_HEIGHT));
		Blocks[CurrentShape.GetBlockPoint(i, false).X][CurrentShape.GetBlockPoint(i, false).Y] = CurrentShape.GetColor();
	}
	//check for solid lines
	int solid_lines[SHAPE_BLOCKS_COUNT];
	int solid_lines_count = 0;
	for(int y = 0; y < GAME_FIELD_HEIGHT; ++y)
	{
		if(IsLineSolid(y))
		{
			solid_lines[solid_lines_count] = y;
			++solid_lines_count;
			ASSERT(solid_lines_count <= SHAPE_BLOCKS_COUNT);
		}
	}
	CurrentShape.Clear();	//painting wotkarround

	//cut solid lines
	for(int i = 0;i < solid_lines_count; ++i)
		EraseLine(solid_lines[i]);
	if(solid_lines_count)
	{
		if(solid_lines_count < 1 || solid_lines_count > 5)
			return;
		LinesCount += solid_lines_count;
		switch(solid_lines_count)
		{
		case 1:
			Score += 1;
			break;
		case 2:
			Score += 3;
			break;
		case 3:
			Score += 5;
			break;
		case 4:
			Score += 7;
			break;
		case 5:
			Score += 9;
			break;
		}
		BonusPercent = (Score - LinesCount)*100 / LinesCount;
		EventProc(ON_LINES_DELETE, NULL);
	}
	//create next shape
	CShape new_shape;
	TestMove(MT_NEW_SHAPE, true, new_shape);
}
void CGameField::OnDrop()
{
	CShape new_shape;
	while(TestMove(MT_MOVE_DOWN, true, new_shape))
		;
}
void CGameField::OnTimer()
{
	OnShapeMove(MT_MOVE_DOWN);
}
bool CGameField::IsLineSolid(int y) const
{
	ASSERT(0 <= y && y <= GAME_FIELD_HEIGHT);
	if(y < 0 || y >= GAME_FIELD_HEIGHT)
		return false;

	for(int x = 0; x < GAME_FIELD_WIDTH; ++x)
		if(NO_BLOCK == Blocks[x][y])
			return false;
	return true;
}
void CGameField::EraseLine(int line_y)
{
	ASSERT((line_y >= 0) && (line_y < GAME_FIELD_HEIGHT));

	//remove line
	for(int x = 0; x < GAME_FIELD_WIDTH; ++x)
		Blocks[x][line_y] = NO_BLOCK;

	//move down upper blocks
	for(int y = line_y - 1; y >= 0; --y)
	{
		for(int x = 0; x < GAME_FIELD_WIDTH; ++x)
		{
			Blocks[x][y + 1] = Blocks[x][y];
			Blocks[x][y] = NO_BLOCK;
		}
	}
}
