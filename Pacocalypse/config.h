#ifndef _CONFIG_H_
#define _CONFIG_H_

/** @brief Timer interrupt frequency used by the game loop. */
#define TICKRATE 60

/** @brief Number of timer ticks between game updates. */
#define TICKS_PER_UPDATE 1

/** @brief VBE video mode used by the renderer. */
#define VIDEO_MODE 0x14C

/** @brief Logical screen width in pixels. */
#define SCREEN_WIDTH 1152

/** @brief Logical screen height in pixels. */
#define SCREEN_HEIGHT 864

#endif // _CONFIG_H_
