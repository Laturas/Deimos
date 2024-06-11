#include "gui_objects.h"
#include <math.h>
#include "personal_utils.c"
#include <windef.h>
#include <winuser.h>

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
    unsigned int draw_increment = (orientation) ? WINWIDTH : 1;
    unsigned int start_pos = (orientation) 
        ? (position)      // Vertical
        : ROW((position));    // Horizontal
    unsigned int end_pos = (orientation) ? (position) + (WINWIDTH * WINHEIGHT) : ROW((position)) + WINWIDTH * 2;

    unsigned int i = start_pos;
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

bool rclick = false;

#define ABS(i) {int sign = i >> 31; int tmp = i ^ sign; i = tmp - sign;}
#define RED(i) ((i << 16) | (0x002222));

/// @brief This function assumes that the two points are separated by one x pos.
void draw_line(int start_x, int start_y, int end_y) {
    int diff = end_y - start_y;

    if (!diff) {
        if (!OUTOFBOUNDS(ROWCOL(start_x,start_y))) {frame.pixels[ROWCOL(start_x,start_y)] = 0xFF2222;}
        if (!OUTOFBOUNDS(ROWCOL((start_x + 1),start_y))) {frame.pixels[ROWCOL((start_x + 1),start_y)] = 0xFF2222;}
        return;
    }

    int red_increment = 255 / diff;
    ABS(red_increment);
    int sign = diff >> 31;
    if (!sign) {sign = 1;}
    int total_incs = 0;
    int total_decs = 255;

    if (diff > WINHEIGHT) {diff = WINHEIGHT;}

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

int scale = 50;

#define SCALED_SCALE (1.0 / (float)scale)

#define SCALE(i) ((i) / SCALED_SCALE)
#define UNSCALE(i) ((i) * SCALED_SCALE)

typedef struct AdvancedNumber {
    unsigned char type;
    float value;
} AdvancedNumber;

AdvancedNumber advlog(float i) {
    AdvancedNumber return_number = {0};
    if (i <= 0.0) {goto DROP;}

    return_number.value = log((double)i);
    return_number.type = 1;

    DROP:
    return return_number;
}

AdvancedNumber advsqrt(float i) {
    AdvancedNumber return_number = {0};
    if (i < 0.0) {goto DROP;}

    return_number.value = sqrt((double)i);
    return_number.type = 1;

    DROP:
    return return_number;
}

AdvancedNumber input_function(float i) {
    return advlog(i);
}

int m_x_pos;
int m_y_pos;

void draw_circle(unsigned int x_pos, unsigned int y_pos, unsigned int color, int radius) {
    // Define a square and color each pixel in there by some amount
    unsigned long long sqrradius = radius * radius;
    for (int i = (-radius); i < radius; i++) {
        unsigned long long sqr_i = i * i;
        for (int j = (-radius); j < radius; j++) {
            if (sqrradius > (sqr_i + j * j)) {
                if (OUTOFBOUNDS((ROWCOL((x_pos + i), (y_pos + j))))) {continue;}
                frame.pixels[(ROWCOL((x_pos + i), (y_pos + j)))] = color;
            }
        }
    }
}

void safedraw(int x_pos, int y_pos, int value) {
    if (OUTOFBOUNDS((ROWCOL(x_pos, y_pos)))) {return;}
    frame.pixels[(ROWCOL(x_pos, y_pos))] = value;
}

void draw_weird_box() {
    
    draw_circle(m_x_pos +  20, m_y_pos, GREY(0xF), 5);
    draw_circle(m_x_pos +  20, m_y_pos - 30, GREY(0xF), 5);
    draw_circle(m_x_pos + 120, m_y_pos, GREY(0xF), 5);
    draw_circle(m_x_pos + 120, m_y_pos - 30, GREY(0xF), 5);
    for (int i = 0; i < 109; i++) {
        for (int j = 0; j < 30; j++) {
            safedraw(i + m_x_pos + 16,m_y_pos - j,GREY(0xF));
        }
    }
    for (int i = 3; i < 106; i++) {
        for (int j = -5; j < 35; j++) {
            safedraw(i + m_x_pos + 16,m_y_pos - j,GREY(0xF));
        }
    }

    int fill_color = GREY(0x2);

    draw_circle(m_x_pos +  21, m_y_pos - 1, fill_color, 5);
    draw_circle(m_x_pos +  21, m_y_pos - 29, fill_color, 5);
    draw_circle(m_x_pos + 119, m_y_pos - 1, fill_color, 5);
    draw_circle(m_x_pos + 119, m_y_pos - 29, fill_color, 5);
    for (int i = 1; i < 108; i++) {
        for (int j = 1; j < 29; j++) {
            safedraw(i + m_x_pos + 16,m_y_pos - j,fill_color);
        }
    }
    for (int i = 4; i < 105; i++) {
        for (int j = -4; j < 34; j++) {
            safedraw(i + m_x_pos + 16,m_y_pos - j,fill_color);
        }
    }
}

void rclick_draws() {
    if (!rclick) {return;}
        DRAW_GRAPH_LINE(m_x_pos, 0x33333333, VERTICAL);
        float pstn = (float)(m_x_pos - cam_x_pos);
        AdvancedNumber reslt = input_function(UNSCALE(pstn));

        //printf("(%d, %d)  |  (0, %d)\n",m_x_pos - cam_x_pos, (unsigned int)reslt.value, cam_y_pos); fflush(stdout);
        if (reslt.type) {
            draw_circle(
                m_x_pos,
                (int)SCALE(reslt.value) + cam_y_pos,
                0xFF2222, 5
            );
        }
    draw_weird_box();
}

void REDRAW_ALL(int bg_color, int axes_color) {
    Panel input_panel = {
        .side = LEFT,
        .width = 250,
    };

    for (int i = 0; i < WINWIDTH * WINHEIGHT; i++) {
        frame.pixels[i] = bg_color;
    }
    if (scale > 50)
    {
        draw_all_lines(scale, 0x66666666);
    }
    else if (scale > 5) {
        draw_all_lines(scale, 0x33333333);
        draw_all_lines(scale * 10, 0x66666666);
    }
    else {
        draw_all_lines(scale * 10, 0x33333333);
        draw_all_lines(scale * 100, 0x66666666);
    }
    
    DRAW_AXIS(cam_y_pos, axes_color, HORIZONTAL);
    DRAW_AXIS(cam_x_pos, axes_color, VERTICAL);

    for (int i = 0; i < (WINWIDTH); i++) {
        float val = (float)(i - cam_x_pos);

        int sqr, prev_sqr;
        AdvancedNumber res_1 = input_function(UNSCALE(val));
        if (!res_1.type) {continue;}
        AdvancedNumber res_2 = input_function(UNSCALE(val - 1));
        if (!res_2.type) {continue;}
        sqr = (int)SCALE(res_1.value);
        prev_sqr = (int)SCALE(res_2.value);
        draw_line(i, prev_sqr + cam_y_pos, sqr + cam_y_pos);
    }

    rclick_draws();

    draw_panel(input_panel);
}