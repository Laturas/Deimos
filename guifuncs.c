#include "gui_objects.h"
#include <math.h>
#include "personal_utils.c"

struct {
    int width;
    int height;
    unsigned int *pixels;
} frame = {0};

int cam_x_pos = 0;
int cam_y_pos = 0;
int prev_mouse_x = 0;
int prev_mouse_y = 0;

#define WINWIDTH frame.width
#define WINHEIGHT frame.height
#define OUTOFBOUNDS(i) (i >= WINWIDTH * WINHEIGHT || i < 0)

#define ROWCOL(x,y) x + y*WINWIDTH
#define ROW(n) WINWIDTH * n
#define HORIZONTAL 0
#define VERTICAL 1
#define NOT_AXIS 0
#define AXIS 1


/// @brief Draws a graph line.
/// @param position The row if it's a horizontal line, or a column if it's a vertical line.
/// @param axes_color The color of the line.
/// @param orientation 0 for horizontal, 1 for vertical
void DRAW_GRAPH_LINE(int position, int axes_color, char orientation) {
    int draw_increment = (orientation) ? WINWIDTH : 1;
    int start_pos = (orientation) 
        ? (position)      // Vertical
        : ROW((position));    // Horizontal
    int end_pos = (orientation) ? (position) + (WINWIDTH * WINHEIGHT) : ROW((position)) + WINWIDTH * 2;

    int i = start_pos;
    while (i < end_pos) {
        if (WINWIDTH * WINHEIGHT == 0) {return;}
        if (i >= WINWIDTH * WINHEIGHT) {return;}
        if (OUTOFBOUNDS(i)) {return;}
        frame.pixels[i] = axes_color;
        i += draw_increment;
    }
}

void DRAW_AXIS(int position, int axes_color, char orientation) {
    int draw_increment = (orientation) ? WINWIDTH : 1;
    int start_pos = (orientation) 
        ? position     // Vertical
        : ROW(position);    // Horizontal
    int end_pos = (orientation) ? (position) + (WINWIDTH * WINHEIGHT) : ROW(position) + WINWIDTH * 2;

    int i = start_pos;
    while (i < end_pos) {
        if (WINWIDTH * WINHEIGHT == 0) {return;}
        if (OUTOFBOUNDS(i)) {return;}
        
        if (orientation) {frame.pixels[i + 1] = axes_color;}
        frame.pixels[i] = axes_color;
        i += draw_increment;
    }
}

inline int intpart(float value) {
    return (int)value;
}
inline float fracpart(float value) {
    return value - (float)intpart(value);
}

//void draw_line(const Line input_line) {
//    bool steep = abs(input_line.end_y_pos - input_line.start_y_pos) > abs(input_line.end_x_pos - input_line.start_x_pos);
//
//    Line mutable_line = {input_line.start_x_pos, input_line.start_y_pos, input_line.end_x_pos, input_line.end_y_pos};
//
//    if (steep) {
//        SWAP(mutable_line.start_x_pos, mutable_line.start_y_pos);
//        SWAP(mutable_line.end_x_pos, mutable_line.end_y_pos);
//    }
//    if (mutable_line.start_x_pos > mutable_line.end_x_pos) {
//        SWAP(mutable_line.start_x_pos, mutable_line.end_x_pos);
//        SWAP(mutable_line.start_y_pos, mutable_line.end_y_pos);
//    }
//
//    int dx = mutable_line.end_x_pos - mutable_line.start_x_pos;
//    int dy = mutable_line.end_y_pos - mutable_line.start_y_pos;
//
//    float gradient;
//
//    if (!dx) {gradient = 0.0;}
//    else {gradient = dy / dx;}
//
//    float xend = round(mutable_line.start_x_pos);
//    float yend = mutable_line.start_y_pos + gradient * (xend - (float)mutable_line.start_x_pos);
//}

#define ABS(i) {int sign = i >> 31; int tmp = i ^ sign; i = tmp - sign;}
#define RED(i) ((i << 16) | (0x002222));

/// @brief This function assumes that the two points are separated by one x pos.
void draw_line(int start_x, int start_y, int end_y) {
    int diff = end_y - start_y;

    if (!diff) {return;}

    int red_increment = 255 / diff;
    ABS(red_increment);
    int sign = diff >> 31;
    if (!sign) {sign = 1;}
    int total_incs = 0;
    int total_decs = 255;

    ABS(diff);

    for (int i = 0; i <= diff; i++) {
        int pos = start_y + (i * sign);
        if (!OUTOFBOUNDS(ROWCOL(start_x,pos))) {frame.pixels[ROWCOL(start_x,pos)] = RED(total_decs);}
        if (!OUTOFBOUNDS(ROWCOL((start_x + 1),pos))) {frame.pixels[ROWCOL((start_x + 1),pos)] = RED(total_incs);}
        
        total_decs -= red_increment;
        total_decs = (total_decs < 22) ? 22 : total_decs;
        total_incs += red_increment;
    }
}

void draw_all_lines(int spacing, int line_color) {
    for (int i = cam_x_pos; i < WINWIDTH; i += spacing) {
        DRAW_GRAPH_LINE(i, line_color, VERTICAL);
    }
    for (int i = cam_x_pos; i > 0; i -= spacing) {
        DRAW_GRAPH_LINE(i, line_color, VERTICAL);
    }

    for (int i = cam_y_pos; i < (WINWIDTH * WINHEIGHT); i += spacing) {
        DRAW_GRAPH_LINE(i, line_color, HORIZONTAL);
    }
    for (int i = cam_y_pos; i > 0; i -= spacing) {
        DRAW_GRAPH_LINE(i, line_color, HORIZONTAL);
    }
}

#define GREY(n) (n) | (n << 4) | (n << 8) | (n << 12) | (n << 16) | (n << 20) | (n << 24) | (n << 28)

int color_id(int ID) {
    if (ID < 0) {return GREY(0x2);}
    if (ID < 2) {return GREY(0x3);}
    if (ID < 4) {return GREY(0x4);}
    if (ID < 6) {return GREY(0x5);}
    if (ID < 8) {return GREY(0x6);}
    return GREY(0x7);
}

void draw_panel(const Panel panel_to_draw) {
    // We are sticking with only left panels right now.
    if (panel_to_draw.side != LEFT) {return;}
    int column = 0;
    while (column < panel_to_draw.width) {
        int color = color_id(panel_to_draw.width - column);
        DRAW_GRAPH_LINE(column, GREY(color), VERTICAL);
        column++;
    }
    DRAW_GRAPH_LINE(panel_to_draw.width, GREY(0xF), VERTICAL);
}

void REDRAW_ALL(int bg_color, int axes_color) {
    Panel input_panel = {
        .side = LEFT,
        .width = 250,
    };

    for (int i = 0; i < WINWIDTH * WINHEIGHT; i++) {
        frame.pixels[i] = bg_color;
    }
    draw_all_lines(50, 0x66666666);
    DRAW_AXIS(cam_y_pos, axes_color, HORIZONTAL);
    DRAW_AXIS(cam_x_pos, axes_color, VERTICAL);

    for (int i = 0; i < 50; i++) {
        int sqr = i*i;
        int prev_sqr = (i-1)*(i-1);
        draw_line(i + cam_x_pos, prev_sqr + cam_y_pos, sqr + cam_y_pos);
    }

    draw_panel(input_panel);
}