#include "V3dHeaderEntries.h"

// canvasWidth,1,UINT        // Canvas width
// canvasHeight,2,UINT       // Canvas heighot
// absolute,3,BOOL           // true: absolute size; false: scale to canvas
// minBound,4,TRIPLE         // Scene minimum bounding box corners
// maxBound,5,TRIPLE         // Scene maximum bounding box corners
// orthographic,6,BOOL       // true: orthographic; false: perspective
// angleOfView,7,REAL        // Field of view angle (in radians)
// initialZoom,8,REAL        // Initial zoom
// viewportShift,9,PAIR      // Viewport shift (for perspective projection)
// viewportMargin,10,PAIR    // Margin around viewport
// light,11,TRIPLE,RGB       // Direction and color of each point light source
// background,12,RGBA        // Background color
// zoomFactor,13,REAL        // Zoom base factor
// zoomPinchFactor,14,REAL   // Zoom pinch factor
// zoomPinchCap,15,REAL      // Zoom pinch limit
// zoomStep,16,REAL          // Zoom power step
// shiftHoldDistance,17,REAL // Shift-mode maximum hold distance (pixels)
// shiftWaitTime,18,REAL     // Shift-mode hold time (milliseconds)
// vibrateTime,19,REAL       // Shift-mode vibrate time (milliseconds)

const std::array<size_t, 20> g_HeaderEntries = {
    0,                                   // Filler
    sizeof(UINT) / 4z,                   // Canvas width
    sizeof(UINT) / 4z,                   // Canvas heighot
    sizeof(BOOL) / 4z,                   // true: absolute size; false: scale to canvas
    sizeof(TRIPLE) / 4z,                 // Scene minimum bounding box corners
    sizeof(TRIPLE) / 4z,                 // Scene maximum bounding box corners
    sizeof(BOOL) / 4z,                   // true: orthographic; false: perspective
    sizeof(REAL) / 4z,                   // Field of view angle (in radians)
    sizeof(REAL) / 4z,                   // Initial zoom
    sizeof(PAIR) / 4z,                   // Viewport shift (for perspective projection)
    sizeof(PAIR) / 4z,                   // Margin around viewport
    (sizeof(TRIPLE) + sizeof(RGB)) / 4z,   // Direction and color of each point light source
    sizeof(RGBA) / 4z,                   // Background color
    sizeof(REAL) / 4z,                   // Zoom base factor
    sizeof(REAL) / 4z,                   // Zoom pinch factor
    sizeof(REAL) / 4z,                   // Zoom pinch limit
    sizeof(REAL) / 4z,                   // Zoom power step
    sizeof(REAL) / 4z,                   // Shift-mode maximum hold distance (pixels)
    sizeof(REAL) / 4z,                   // Shift-mode hold time (milliseconds)
    sizeof(REAL) / 4z,                   // Shift-mode vibrate time (milliseconds)
};