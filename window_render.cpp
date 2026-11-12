// File: window_render.cpp
// Commit: Split window rendering implementation into clear unity-included files

#include "window_render.h"
#include "building/province_screen_capital_render.h"
#include "map_province_environment.h"

#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "window_render_helpers.cpp"
#include "window_render_environment_text.cpp"
#include "window_render_discovery_text.cpp"
#include "window_render_primitives.cpp"
#include "window_render_colors.cpp"
#include "window_render_selection.cpp"
#include "window_render_map_shell_rects.cpp"
#include "window_render_province_rects.cpp"
#include "window_render_hud.cpp"
#include "window_render_overlay_helpers.cpp"
#include "window_render_overlay_tab_helpers.cpp"
#include "window_render_overlay.cpp"
