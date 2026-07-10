#ifndef LV_CONF_H
#define LV_CONF_H

/* Browser display uses RGBA8888 through Emscripten's SDL2 port. */
#define LV_COLOR_DEPTH 32

#define LV_USE_OS LV_OS_NONE
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

#define LV_USE_SDL 1
#define LV_USE_DRAW_SDL 0

/* Fonts used by the example UI. */
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14

#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0

#define LV_USE_DEMO_WIDGETS 0
#define LV_USE_DEMO_BENCHMARK 0
#define LV_USE_DEMO_STRESS 0
#define LV_USE_DEMO_RENDER 0

#endif /* LV_CONF_H */

