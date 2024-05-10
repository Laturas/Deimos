typedef enum PanelSide {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
} PanelSide; 

typedef struct Panel {
    PanelSide side;
    unsigned int width;
    //Widget* widgets;
} Panel;