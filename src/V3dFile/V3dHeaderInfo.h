#include "V3dTypes.h"
#include "xstream.h"

enum HeaderTypes {
    CANVAS_WIDTH = 1,           // Canvas width
    CANVAS_HEIGHT = 2,          // Canvas heighot
    ABSOLUTE = 3,               // true: absolute size; false: scale to canvas
    MIN_BOUND = 4,              // Scene minimum bounding box corners
    MAX_BOUND = 5,              // Scene maximum bounding box corners
    ORTHOGRAPHIC = 6,           // true: orthographic; false: perspective
    ANGLE_OF_VIEW = 7,          // Field of view angle (in radians)
    INITIAL_ZOOM = 8,           // Initial zoom
    VIEWPORT_SHIFT = 9,         // Viewport shift (for perspective projection)
    VIEWPORT_MARGIN = 10,       // Margin around viewport
    LIGHT = 11,                 // Direction and color of each point light source
    BACKGROUND = 12,            // Background color
    ZOOM_FACTOR = 13,           // Zoom base factor
    ZOOM_PINCH_FACTOR = 14,     // Zoom pinch factor
    ZOOM_PINCH_CAP = 15,        // Zoom pinch limit
    ZOOM_STEP = 16,             // Zoom power step
    SHIFT_HOLD_DISTANCE = 17,   // Shift-mode maximum hold distance (pixels)
    SHIFT_WAIT_TIME = 18,       // Shift-mode hold time (milliseconds)
    VIBRATE_TIME = 19           // Shift-mode vibrate time (milliseconds)
};

struct V3dHeaderInfo {
public:
    struct Light {
        TRIPLE direction;
        RGB color;
    };

    UINT canvasWidth;
    UINT canvasHeight;
    BOOL absolute;
    TRIPLE minBound;
    TRIPLE maxBound;
    BOOL orthographic;
    REAL angleOfView;
    REAL initialZoom;
    PAIR viewportShift;
    PAIR viewportMargin;
    Light light;
    RGBA background;
    REAL zoomFactor;
    REAL zoomPinchFactor;
    REAL zoomPinchCap;
    REAL zoomStep;
    REAL shiftHoldDistance;
    REAL shiftWaitTime;
    REAL vibrateTime;
};