#include "gui_objects.h"
#include <stdio.h>

struct {
    int width;
    int height;
    unsigned int *pixels;
} frame = {0};

int cam_x_pos = 0;
int cam_y_pos = 0;

#define WINWIDTH frame.width
#define WINHEIGHT frame.height

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
        
        if (orientation) {frame.pixels[i + 1] = axes_color;}
        frame.pixels[i] = axes_color;
        i += draw_increment;
    }
}

void draw_all_lines(int spacing, int line_color) {
    for (int i = WINWIDTH / 2; i < WINWIDTH; i += spacing) {
        DRAW_GRAPH_LINE(i, line_color, VERTICAL);
    }
    for (int i = WINWIDTH / 2; i > 0; i -= spacing) {
        DRAW_GRAPH_LINE(i, line_color, VERTICAL);
    }

    for (int i = (WINHEIGHT) / 2; i < (WINWIDTH * WINHEIGHT); i += spacing) {
        DRAW_GRAPH_LINE(i, line_color, HORIZONTAL);
    }
    for (int i = (WINHEIGHT) / 2; i > 0; i -= spacing) {
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

#define GUARDED_DRAW(pixel, color) if ((pixel) >= 0 && (pixel) < (WINWIDTH * WINHEIGHT)) {frame.pixels[(pixel)] = color;}

void draw_sqr(const unsigned int pixel, int color) {
    GUARDED_DRAW(pixel, color)
    GUARDED_DRAW(pixel-1, color)
    GUARDED_DRAW(pixel+1, color)
    GUARDED_DRAW(pixel+ROW(1), color)
    GUARDED_DRAW(pixel-ROW(1), color)
    GUARDED_DRAW(pixel+ROW(1)+1, color)
    GUARDED_DRAW(pixel+ROW(1)-1, color)
    GUARDED_DRAW(pixel-ROW(1)+1, color)
    GUARDED_DRAW(pixel-ROW(1)-1, color)
}

void draw_path() {
    for (int i = (WINWIDTH) / 2; i < WINWIDTH; i++) {
        int num = i - (WINWIDTH) / 2;
        int fn = ROW((num)) + ROW(((WINHEIGHT) / 2));
        int pix = i + fn;
        if (pix > (WINWIDTH * WINHEIGHT)) {continue;}
        draw_sqr(pix, 0xFFFF0000);
    }
    for (int i = 0; i < (WINWIDTH) / 2; i++) {
        int num = i - (WINWIDTH) / 2;
        int fn = ROW((num)) + ROW(((WINHEIGHT) / 2));
        int pix = i + fn;
        if (pix < 0) {continue;}
        if (pix > (WINWIDTH * WINHEIGHT)) {continue;}
        draw_sqr(pix, 0xFFFF0000);
    }
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
    DRAW_AXIS(WINHEIGHT / 2, axes_color, HORIZONTAL);
    DRAW_AXIS(WINWIDTH / 2, axes_color, VERTICAL);
    draw_path();
    draw_panel(input_panel);
}