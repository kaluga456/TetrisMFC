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
	TYPE_LINE = 0,
	TYPE_T,
	TYPE_LZ,
	TYPE_RZ,
	TYPE_SQUARE,
	TYPE_LL,
	TYPE_RL,

	TYPE_COUNT
};

static app::random_value<int> RandomShape(0, TYPE_COUNT);
void CShape::Assign(const CShape& shape)
{
	Size = shape.GetSize();
	BlocksCount = shape.GetBlocksCount();
	Color = shape.GetColor();
	Point = shape.Point;
	for(int i = 0; i < BlocksCount; ++i)
		Blocks[i] = shape.GetBlockPoint(i, true);
}
void CShape::Generate(const CPoint& p)
{
	Point = p;
	//create shape
	BlocksCount = 4;
	switch(RandomShape.get())
	{
		case 0:	//TYPE_LINE
			Size = 4;
			Blocks[0] = CPoint(1,0); 
			Blocks[1] = CPoint(1,1);
			Blocks[2] = CPoint(1,2); 
			Blocks[3] = CPoint(1,3);
			break;
		case 1:	//TYPE_T;
			Size = 3;
			Blocks[0] = CPoint(1,0);
			Blocks[1] = CPoint(0,1);
			Blocks[2] = CPoint(1,1);
			Blocks[3] = CPoint(2,1);
			break;
		case 2:	//TYPE_LZ;
			Size = 3;
			Blocks[0] = CPoint(0,0);
			Blocks[1] = CPoint(0,1);
			Blocks[2] = CPoint(1,1);
			Blocks[3] = CPoint(1,2);
			break;
		case 3: //TYPE_RZ; 
			Size = 3;
			Blocks[0] = CPoint(1,0);
			Blocks[1] = CPoint(0,1);
			Blocks[2] = CPoint(1,1);
			Blocks[3] = CPoint(0,2);
			break;
		case 4: //TYPE_SQUARE; 
			Size = 2;
			Blocks[0] = CPoint(0,0);
			Blocks[1] = CPoint(0,1);
			Blocks[2] = CPoint(1,0);
			Blocks[3] = CPoint(1,1);
			break;
		case 5: //TYPE_LL; 
			Size = 3;
			Blocks[0] = CPoint(0,0);
			Blocks[1] = CPoint(1,0);
			Blocks[2] = CPoint(1,1);
			Blocks[3] = CPoint(1,2);
			break;
		case 6: //TYPE_RL; 
			Size = 3;
			Blocks[0] = CPoint(2,0);
			Blocks[1] = CPoint(1,0);
			Blocks[2] = CPoint(1,1);
			Blocks[3] = CPoint(1,2);
			break;
		default:
			ASSERT(FALSE);
	}

	Color = GetRandomColor();
}
void CShape::Rotate(UINT direction)
{
	ASSERT((direction == MT_ROTATE_LEFT) || (direction == MT_ROTATE_RIGHT));
	
	CPoint new_block;	//rotated block
	for(int i = 0; i < BlocksCount; ++i)
	{
		if(direction == MT_ROTATE_LEFT)
		{
			new_block.x = Size - (Blocks[i].y + 1);
			new_block.y = Blocks[i].x;
		}
		else if(direction == MT_ROTATE_RIGHT)
		{
			new_block.x = Blocks[i].y;
			new_block.y = Size - (Blocks[i].x + 1);
		}
		Blocks[i].x = new_block.x;
		Blocks[i].y = new_block.y;
	}
}
void CGameField::Clear()
{
	for(int x = 0; x < GAME_FIELD_WIDTH; ++x)
		for(int y = 0; y < GAME_FIELD_HEIGHT; ++y)
			LandedBlocks[x][y] = false;
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
	NextShape.Generate(CPoint(5, 0));
	CShape new_shape;
	TestMove(MT_NEW_SHAPE, true, new_shape);
	Score = 0;
	LinesCount = 0;
	BonusPercent = 0;
}
bool CGameField::CheckShape(const CShape& shape) const
{
	for(int i=0;i<shape.GetBlocksCount();i++)
	{
		if(shape.GetBlockPoint(i, false).x < 0 || shape.GetBlockPoint(i, false).x >= GAME_FIELD_WIDTH)
			return false;
 		if(shape.GetBlockPoint(i, false).y < 0 || shape.GetBlockPoint(i, false).y >= GAME_FIELD_HEIGHT)
			return false;
		if(LandedBlocks[shape.GetBlockPoint(i, false).x][shape.GetBlockPoint(i, false).y])
			return false;
	}
	return true;
}
bool CGameField::TestMove(int type, bool move, CShape& new_shape)
{
	if(type != MT_NEW_SHAPE)
		new_shape.Assign(Shape);
	switch(type)
	{
	case MT_NEW_SHAPE:
		new_shape.Assign(NextShape);
		NextShape.Generate(CPoint(5, 0));
		break;
	case MT_MOVE_LEFT:
		new_shape.Point.x = Shape.Point.x - 1;
		break;
	case MT_MOVE_RIGHT:
		new_shape.Point.x = Shape.Point.x + 1;
		break;
	case MT_MOVE_DOWN:
		new_shape.Point.y = Shape.Point.y + 1;
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
			Shape.Assign(new_shape);
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
	for(int i=0;i<Shape.GetBlocksCount();i++)
	{
		ASSERT((Shape.GetBlockPoint(i, false).x >= 0) && (Shape.GetBlockPoint(i, false).x < GAME_FIELD_WIDTH));
		ASSERT((Shape.GetBlockPoint(i, false).y >= 0) && (Shape.GetBlockPoint(i, false).y < GAME_FIELD_HEIGHT));
		LandedBlocks[Shape.GetBlockPoint(i, false).x][Shape.GetBlockPoint(i, false).y] = Shape.GetColor();
	}
	//check for solid lines
	int solid_lines[SHAPE_MAX_BLOCKS];
	int solid_lines_count = 0;
	for(int y=0;y<GAME_FIELD_HEIGHT;y++)
	{
		if(IsLineSolid(y))
		{
			solid_lines[solid_lines_count] = y;
			solid_lines_count++;
			ASSERT(solid_lines_count <= SHAPE_MAX_BLOCKS);
		}
	}
	Shape.Clear();	//painting wotkarround
	//cut solid lines
	for(int i=0;i<solid_lines_count;i++)
		CutSolidLine(solid_lines[i]);
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
	if((y < 0) || (y >= GAME_FIELD_HEIGHT))
		return false;
	for(int x=0;x<GAME_FIELD_WIDTH;x++)
		if(LandedBlocks[x][y]==NULL)
			return false;
	return true;
}
void CGameField::CutSolidLine(int line_y)
{
	ASSERT((line_y >= 0) && (line_y < GAME_FIELD_HEIGHT));
	//remove line
	for(int x=0;x<GAME_FIELD_WIDTH;x++)
		LandedBlocks[x][line_y] = NULL;
	//move down upper blocks
	for(int y=line_y-1;y>=0;y--)
	{
		for(int x=0;x<GAME_FIELD_WIDTH;x++)
		{
			LandedBlocks[x][y+1] = LandedBlocks[x][y];
			LandedBlocks[x][y] = NULL;
		}
	}
}
void CGameTimer::OnNewGame()
{
	StartTime = CTime::GetCurrentTime();
	PauseSpan = 0;
}
void CGameTimer::OnPause(bool pause)
{
	if(pause)	//pause
		PauseTime = CTime::GetCurrentTime();
	else		//resume
		PauseSpan += CTime::GetCurrentTime() - PauseTime;
}
CTimeSpan CGameTimer::GetGameSpan() const
{
	return CTime::GetCurrentTime() - PauseSpan - StartTime;
}
CTime CGameTimer::GetStartTime() const
{
	return StartTime;
}
CString CGameTimer::GetGameSpanString() const
{
	CTimeSpan game_span = CTime::GetCurrentTime() - PauseSpan - StartTime;
	return game_span.Format("%M:%S");
}