#pragma once

namespace tetris
{

//block data defined by outer code (usually color)
using block_t = unsigned int;
constexpr block_t BLOCK_NONE = static_cast<block_t>(0);

//shape
enum : int
{
    SHAPE_TYPE_I = 0,
    SHAPE_TYPE_T,
    SHAPE_TYPE_S,
    SHAPE_TYPE_Z,
    SHAPE_TYPE_O,
    SHAPE_TYPE_L,
    SHAPE_TYPE_J,

    SHAPE_TYPE_COUNT
};
constexpr int SHAPE_MATRIX_SIZE = 4;
constexpr int SHAPE_BLOCKS_COUNT = 4;
constexpr int SHAPE_LAYOUT_COUNT = 4;
using shape_matrix_t = unsigned char[SHAPE_BLOCKS_COUNT][SHAPE_BLOCKS_COUNT];

class shape_t
{
public:
    shape_t();
    shape_t(const shape_t& rop);
    explicit shape_t(int shape_type);

    //assign
    shape_t& operator=(const shape_t& rop);
    void reset(int shape_type, block_t block_type);

    //access
    const shape_matrix_t* get_matrix() const {return Matrix;}
    int get_type() const;
    int get_layout() const;
    block_t get_block_type() const {return BlockType;}

    void rotate_left();
    void rotate_right();

private:
    const shape_matrix_t* Matrix{nullptr};
    block_t BlockType{ BLOCK_NONE };
};

//shape generator
class shape_generator_t
{
public:
    virtual void generate(shape_t& shape) = 0;
};

//game field sizes
constexpr int GAME_FIELD_WIDTH = 12;
constexpr int GAME_FIELD_HEIGHT = 24;

//game field coordinates
struct point_t
{
    point_t() : X{0}, Y{0} {}
    point_t(int x, int y) : X{static_cast<short>(x)}, Y{static_cast<short>(y)} {}
    point_t(const point_t& fp) : X{fp.X}, Y{fp.Y} {}
    point_t& operator=(const point_t& rop) {X = rop.X; Y = rop.Y; return *this;}

    short X{0};
    short Y{0};
};

//action result
enum : int
{
    RESULT_NONE = 0,    //nothing happen
    RESULT_SHAPE,       //new shape appear on field and probably some lines removed
    RESULT_CHANGED,     //game field changed
    RESULT_GAME_OVER
};

//game field
struct engine
{
public:
	//init
    engine();
    void set_generator(shape_generator_t* generator = nullptr);
    void clear();

    //access
    block_t get_block(int x, int y) const;
    block_t get_shape_block(int x, int y, const shape_t& shape, const point_t& pos) const;
    const shape_t& get_next_shape() const { return NextShape; }
    int get_score() const {return Score;}

    //actions, return RESULT_ values
    int new_game();
    int move_left();
    int move_right();
    int move_down();
    int drop();
    int rotate_left();
    int rotate_right();

private:
    shape_generator_t* ShapeGenerator{nullptr};
    block_t Blocks[GAME_FIELD_WIDTH][GAME_FIELD_HEIGHT]; //game field

    //shape
    point_t ShapePos;
    shape_t Shape; //shape on game field
    shape_t NextShape; //next shape
    int new_shape(); //place new shape on game field
    int test_shape(const shape_t& shape, short x_offset = 0, short y_offset = 0);
    int test_rotation(const shape_t& shape);

    //remove lines
    int Score{0}; //removed lines count
    void insert_shape(); //add current shape to game field blocks and remove solid lines if any
    bool is_solid_line(int y) const;
    void remove_line(int line_y);

};

} //namespace tetris
