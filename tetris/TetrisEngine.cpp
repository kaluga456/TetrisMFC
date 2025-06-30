#include "stdafx.h"
#pragma hdrstop
#include "TetrisEngine.h"

#ifdef ASSERT
#define TETRIS_ASSERT ASSERT
#elif defined Q_ASSERT
#define TETRIS_ASSERT Q_ASSERT
#else
#define TETRIS_ASSERT assert
#endif

using namespace tetris;

//all shapes matrices
using shape_layouts_t = shape_matrix_t[SHAPE_LAYOUT_COUNT];
constexpr shape_layouts_t TETRIS_SHAPES[]  =
{
    //SHAPE_TYPE_I
    {
        {
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0}
        }
    },

    //SHAPE_TYPE_T
    {
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {1,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    },

    //SHAPE_TYPE_S
    {
        {
            {0,0,0,0},
            {0,1,1,0},
            {1,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {0,1,1,0},
            {1,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        }
    },

    //SHAPE_TYPE_Z
    {
        {
            {0,0,0,0},
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    },

    //SHAPE_TYPE_O
    {
        {
            {1,1,0,0},
            {1,1,0,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {1,1,0,0},
            {1,1,0,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {1,1,0,0},
            {1,1,0,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {1,1,0,0},
            {1,1,0,0},
            {0,0,0,0},
            {0,0,0,0}
        }
    },

    //SHAPE_TYPE_L
    {
        {
            {0,0,0,0},
            {1,1,1,0},
            {1,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {1,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        },
    },

    //SHAPE_TYPE_J
    {
        {
            {1,0,0,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {1,1,0,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        },
    },
};

static_assert(SHAPE_TYPE_COUNT == sizeof(TETRIS_SHAPES) / sizeof(shape_layouts_t));

shape_t::shape_t() : Matrix{TETRIS_SHAPES[0]}
{
}
shape_t::shape_t(const shape_t &rop) : Matrix{rop.Matrix}, BlockType{rop.BlockType}
{
}
shape_t::shape_t(int shape_type) : Matrix{&TETRIS_SHAPES[shape_type][0]}
{
    TETRIS_ASSERT(0 <= shape_type && shape_type < SHAPE_TYPE_COUNT);
}
shape_t &shape_t::operator=(const shape_t &rop)
{
    Matrix = rop.Matrix;
    BlockType = rop.BlockType;
    return *this;
}
void shape_t::reset(int shape_type, block_t block_type)
{
    Matrix = &TETRIS_SHAPES[shape_type][0];
    BlockType = block_type;
}
int shape_t::get_type() const
{
    const shape_matrix_t* pos = Matrix;
    return static_cast<int>(pos - TETRIS_SHAPES[0]) / SHAPE_LAYOUT_COUNT;
}

int shape_t::get_layout() const
{
    const int type = get_type();
    const shape_matrix_t* begin = TETRIS_SHAPES[type];
    const shape_matrix_t* pos = Matrix;
    return static_cast<int>(pos - begin);
}

void shape_t::rotate_left()
{
    const int type = get_type();
    int layout = get_layout();
    ++layout;
    if(layout >= SHAPE_LAYOUT_COUNT)
        layout = 0;
    Matrix = &TETRIS_SHAPES[type][layout];
}

void shape_t::rotate_right()
{
    const int type = get_type();
    int layout = get_layout();
    --layout;
    if(layout < 0)
        layout = SHAPE_LAYOUT_COUNT - 1;
    Matrix = &TETRIS_SHAPES[type][layout];
}

class default_shape_generator : public shape_generator_t
{
public:
    void generate(shape_t& shape)
    {
        shape.reset(SHAPE_TYPE_O, static_cast<block_t>(0x808080));
    }
};
static default_shape_generator default_generator;

engine::engine() : ShapeGenerator{&default_generator}
{
    clear();
}
void engine::clear()
{
    Score = 0;
	for(size_t x = 0; x < GAME_FIELD_WIDTH; ++x)
		for(size_t y = 0; y < GAME_FIELD_HEIGHT; ++y)
			Blocks[x][y] = BLOCK_NONE;
}

void engine::set_generator(shape_generator_t *generator /*= nullptr*/)
{
    ShapeGenerator = generator ? generator : &default_generator;
}
block_t engine::get_block(int x, int y) const
{
    if(Blocks[x][y] != BLOCK_NONE)
        return Blocks[x][y];
    return get_shape_block(x, y, Shape, ShapePos);
}
block_t engine::get_shape_block(int x, int y, const shape_t& shape, const point_t& pos) const
{
    const short x_pos = x - pos.X;
    if(x_pos < 0 || SHAPE_MATRIX_SIZE <= x_pos)
        return BLOCK_NONE;
    const short y_pos = y - pos.Y;
    if(y_pos < 0 || SHAPE_MATRIX_SIZE <= y_pos)
        return BLOCK_NONE;

    const shape_matrix_t& matrix = *shape.get_matrix();
    const block_t matrix_block = matrix[x_pos][y_pos];

    return (BLOCK_NONE == matrix_block) ? BLOCK_NONE : shape.get_block_type();
}
int engine::new_game()
{
    clear();
    ShapeGenerator->generate(NextShape);
    return new_shape();
}
int engine::new_shape()
{
    //TODO: new shape pos
    ShapePos.X = GAME_FIELD_WIDTH / 2 - 1;
    ShapePos.Y = 0;

    if(RESULT_NONE == test_shape(NextShape))
        return RESULT_GAME_OVER;

    ShapeGenerator->generate(NextShape);
    return RESULT_SHAPE;
}
int engine::test_shape(const shape_t& shape, short x_offset /*= 0*/, short y_offset /*= 0*/)
{
    const point_t pos{ShapePos.X + x_offset, ShapePos.Y + y_offset};
    const shape_matrix_t& matrix = *shape.get_matrix();
    for(int x = 0; x < SHAPE_MATRIX_SIZE; ++x)
    {
        for(int y = 0; y < SHAPE_MATRIX_SIZE; ++y)
        {
            const block_t shape_block = matrix[x][y];
            if(BLOCK_NONE == shape_block)
                continue;

            //game field coords
            const short x_pos = pos.X + x;
            const short y_pos = pos.Y + y;

            //test walls
            if(x_pos < 0 || GAME_FIELD_WIDTH <= x_pos)
                return RESULT_NONE;

            //test floor
            if(y_pos >= GAME_FIELD_HEIGHT)
                return RESULT_NONE;

            //test game field blocks
            const block_t field_block = Blocks[x_pos][y_pos];
            if(field_block != BLOCK_NONE)
                return RESULT_NONE;
        }
    }

    if(&Shape != &shape)
        Shape = shape;
    ShapePos = pos;
    return RESULT_CHANGED;
}
int engine::move_left()
{
    return test_shape(Shape, -1, 0);
}
int engine::move_right()
{
    return test_shape(Shape, +1, 0);
}
int engine::move_down()
{
    if(RESULT_CHANGED == test_shape(Shape, 0, +1))
        return RESULT_CHANGED;
    insert_shape();
    return new_shape();
}
int engine::drop()
{
    int result{RESULT_NONE};
    for(;;)
    {
        result = move_down();
        if(RESULT_CHANGED != result)
            return result;
    }
    return result;
}
int engine::rotate_left()
{
    shape_t shape(Shape);
    shape.rotate_left();
    return test_rotation(shape);
}
int engine::rotate_right()
{
    shape_t shape(Shape);
    shape.rotate_right();
    return test_rotation(shape);
}
int engine::test_rotation(const shape_t &shape)
{
    //test in place
    if(RESULT_CHANGED == test_shape(shape))
        return RESULT_CHANGED;

    //test against left wall
    if(ShapePos.X < 0)
    {
        if(RESULT_CHANGED == test_shape(shape, +1, 0))
            return RESULT_CHANGED;
    }

    //test against right wall
    else if(ShapePos.X + SHAPE_MATRIX_SIZE > GAME_FIELD_WIDTH)
    {
        if(RESULT_CHANGED == test_shape(shape, -1, 0))
            return RESULT_CHANGED;
        if(RESULT_CHANGED == test_shape(shape, -2, 0))
            return RESULT_CHANGED;
    }

    return RESULT_NONE;
}
bool engine::is_solid_line(int y) const
{
    TETRIS_ASSERT(0 <= y && y <= GAME_FIELD_HEIGHT);
	if (y < 0 || GAME_FIELD_HEIGHT <= y)
		return false;

    for (int x = 0; x < GAME_FIELD_WIDTH; ++x)
		if (BLOCK_NONE == Blocks[x][y])
			return false;
	return true;
}
void engine::insert_shape()
{
    //add shape blocks to game field
    const shape_matrix_t& matrix = *Shape.get_matrix();
    for(int x = 0; x < SHAPE_MATRIX_SIZE; ++x)
    {
        for(int y = 0; y < SHAPE_MATRIX_SIZE; ++y)
        {
            const block_t shape_block = matrix[x][y];
            if(BLOCK_NONE == shape_block)
                continue;

            //game field coords
            const short x_pos = ShapePos.X + x;
            const short y_pos = ShapePos.Y + y;
            TETRIS_ASSERT(x_pos < GAME_FIELD_WIDTH && y_pos < GAME_FIELD_HEIGHT);

            Blocks[x_pos][y_pos] = Shape.get_block_type();
        }
    }

    //check for solid lines
    for(int y = 0; y < GAME_FIELD_HEIGHT; ++y)
    {
        if (false == is_solid_line(y))
            continue;

        remove_line(y);
        ++Score;
    }
}
void engine::remove_line(int line_y)
{
    TETRIS_ASSERT(line_y >= 0 && line_y < GAME_FIELD_HEIGHT);

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

