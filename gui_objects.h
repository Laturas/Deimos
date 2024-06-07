typedef enum PanelSide {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
} PanelSide; 

typedef struct Line {
    int start_x_pos;
    int start_y_pos;
    int end_x_pos;
    int end_y_pos;
} Line;

typedef struct Panel {
    PanelSide side;
    unsigned int width;
    //Widget* widgets;
} Panel;