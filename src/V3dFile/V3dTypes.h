#pragma once

#include <glm/glm.hpp>

// #define SINGLE_PRECISION

using UINT = uint32_t;
using BOOL = uint32_t;

#ifdef SINGLE_PRECISION
using REAL = float; // TODO should change based on doublePrecision flag
using PAIR = glm::vec2;    // REAL  * 2
using TRIPLE = glm::vec3;  // REAL  * 3
#else
using REAL = double; // TODO should change based on doublePrecision flag
using PAIR = glm::dvec2;    // REAL  * 2
using TRIPLE = glm::dvec3;  // REAL  * 3
#endif

using FLOAT = float;

using RGB = glm::vec3;      // FLOAT * 3
using RGBA = glm::vec4;     // FLOAT * 4
using WORD = uint32_t;