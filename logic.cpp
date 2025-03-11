#include "stdafx.h"
#pragma hdrstop
#include "random.h"
#include "logic.h"

#ifndef ASSERT
#define ASSERT assert
#endif

//shape types
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
static app::random_value<int> RandomShape(0, SHAPE_TYPE_COUNT - 1);

void CShape::Assign(const CShape& shape)
{
	Size = shape.GetSize();
	BlocksCount = shape.GetBlocksCount();
	Context = shape.GetContext();
	Pos = shape.Pos;
	for(int i = 0; i < BlocksCount; ++i)
		Blocks[i] = shape.GetBlockPoint(i, true);
}
void CShape::Generate(const Point& p, block_t context)
{
	Pos = p;

	//create shape
	BlocksCount = SHAPE_BLOCKS_COUNT;
	Context = context;
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
			ASSERT(0);
	}
}
block_t CShape::GetBlockAt(const Point& p) const
{
	for (size_t i = 0; i < BlocksCount; ++i)
		if (GetBlockPoint(i, false) == p)
			return Context;
	return BLOCK_NONE;
}
Point CShape::GetBlockPoint(size_t block_index, bool relative) const
{
	ASSERT((block_index >= 0) && (block_index < BlocksCount));
	if (relative)
		return Point(Blocks[block_index].X, Blocks[block_index].Y);
	return Point(Blocks[block_index].X + Pos.X, Blocks[block_index].Y + Pos.Y);
}
void CShape::Rotate(int direction)
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

bool CGameField::CCallbackBlock::Blocked = false;

CGameField::CGameField() :	callback_GetBLockContext{ nullptr }, 
							callback_EventProc{ nullptr }
{
	Clear();
}
CGameField::~CGameField()
{
}
void CGameField::Clear()
{
	for(size_t x = 0; x < GAME_FIELD_WIDTH; ++x)
		for(size_t y = 0; y < GAME_FIELD_HEIGHT; ++y)
			Blocks[x][y] = BLOCK_NONE;
}
void CGameField::Initialize(FGetBlockContext get_block_context, FEventProc event_proc)
{
	callback_GetBLockContext = get_block_context;
	callback_EventProc = event_proc;
	Clear();
}
block_t CGameField::GetBLockContext() const
{
	if (nullptr == callback_GetBLockContext)
		return BLOCK_DEFAULT;

	CCallbackBlock cb;
	return callback_GetBLockContext();
}
void CGameField::EventProc(int event, int param /*= 0*/) const
{
	if (nullptr == callback_EventProc)
		return;

	CCallbackBlock cb;
	callback_EventProc(event, param);
}
block_t CGameField::GetBlockAt(int x, int y) const
{
	if (Blocks[x][y])
		return Blocks[x][y];
	return CurrentShape.GetBlockAt(Point(x, y));
}
int CGameField::OnNewGame()
{
	if (CCallbackBlock::IsBlocked())
		return RESULT_BLOCKED;
	Clear();
	NextShape.Generate(Point(GAME_FIELD_WIDTH / 2 - 1, 0), GetBLockContext());
	CShape new_shape;
	TestMove(MT_NEW_SHAPE, true);
	return RESULT_OK;
}
int CGameField::OnMoveShape(int type)
{
	if (CCallbackBlock::IsBlocked())
		return RESULT_BLOCKED;
	if (MT_UNDEFINED == type)
		return RESULT_FAIL;
	TestMove(type, true);
	return RESULT_OK;
}
int CGameField::OnDrop()
{
	if (CCallbackBlock::IsBlocked())
		return RESULT_BLOCKED;
	CShape new_shape;
	while (TestMove(MT_MOVE_DOWN, true))
		;
	return RESULT_OK;
}
int CGameField::OnTick()
{
	return OnMoveShape(MT_MOVE_DOWN);
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
bool CGameField::TestMove(int type, bool move)
{
	CShape new_shape;
	if(type != MT_NEW_SHAPE)
		new_shape.Assign(CurrentShape);

	switch(type)
	{
	case MT_NEW_SHAPE:
		new_shape.Assign(NextShape);
		NextShape.Generate(Point(5, 0), GetBLockContext());
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
		ASSERT(0);	//move type is undefined
		return false;
	}
	if(CheckShape(new_shape))
	{
		if(move)
		{
			CurrentShape.Assign(new_shape);
			if(type == MT_NEW_SHAPE)
				EventProc(ON_NEW_SHAPE);
			EventProc(ON_SHAPE_MOVE);
		}
		return true;
	}
	if(type == MT_MOVE_DOWN)
		ShapeLanded();
	else if(type == MT_NEW_SHAPE)
		EventProc(ON_GAME_OVER);
	return false;
}
bool CGameField::IsLineSolid(int y) const
{
	ASSERT(0 <= y && y <= GAME_FIELD_HEIGHT);
	if (y < 0 || GAME_FIELD_HEIGHT <= y)
		return false;

	for (coord_t x = 0; x < GAME_FIELD_WIDTH; ++x)
		if (BLOCK_NONE == Blocks[x][y])
			return false;
	return true;
}
void CGameField::ShapeLanded()	//called after each shape changing
{
	//paint landed shape first
	EventProc(ON_SHAPE_LANDED);

	//add landed shape
	for(int i = 0; i < CurrentShape.GetBlocksCount(); ++i)
	{
		ASSERT((CurrentShape.GetBlockPoint(i, false).X >= 0) && (CurrentShape.GetBlockPoint(i, false).X < GAME_FIELD_WIDTH));
		ASSERT((CurrentShape.GetBlockPoint(i, false).Y >= 0) && (CurrentShape.GetBlockPoint(i, false).Y < GAME_FIELD_HEIGHT));
		Blocks[CurrentShape.GetBlockPoint(i, false).X][CurrentShape.GetBlockPoint(i, false).Y] = CurrentShape.GetContext();
	}
	CurrentShape.Clear();

	//check for solid lines
	int solid_lines[SHAPE_BLOCKS_COUNT]{};
	int solid_lines_count = 0;
	for(int y = 0; y < GAME_FIELD_HEIGHT; ++y)
	{
		if (false == IsLineSolid(y))
			continue;

		solid_lines[solid_lines_count] = y;
		++solid_lines_count;

		//TODO:
		//EventProc(ON_LINE_DELETE, y);

		ASSERT(solid_lines_count <= SHAPE_BLOCKS_COUNT);
	}

	//remove solid lines
	for(int i = 0; i < solid_lines_count; ++i)
		EraseLine(solid_lines[i]);

	if(solid_lines_count)
		EventProc(ON_LINES_DELETE, solid_lines_count);

	//create next shape
	TestMove(MT_NEW_SHAPE, true);
}
void CGameField::EraseLine(int line_y)
{
	ASSERT((line_y >= 0) && (line_y < GAME_FIELD_HEIGHT));

	//remove line
	for(int x = 0; x < GAME_FIELD_WIDTH; ++x)
		Blocks[x][line_y] = BLOCK_NONE;

	//move down upper blocks
	for(int y = line_y - 1; y >= 0; --y)
	{
		for(int x = 0; x < GAME_FIELD_WIDTH; ++x)
		{
			Blocks[x][y + 1] = Blocks[x][y];
			Blocks[x][y] = BLOCK_NONE;
		}
	}
}
