#pragma once

#include "gb8x8.h"

#include "graphics.h"


#define MEM_VRAM_BANK_SWITCH     0xFF4F // CBG only

#define MEM_VRAM                 0x8000
#define MEM_EXT_RAM              0xA000

// VRAM OFFSETS

#define VRAM_TILE_DATA0          MEM_VRAM
#define VRAM_TILE_DATA1          0x8800
#define VRAM_TILE_DATA2          0x9000
#define VRAM_TILE_MAP0           0x9800
#define VRAM_TILE_MAP1           0x9C00
#define VRAM_END                 MEM_EXT_RAM

#define TILE_MAP_WIDTH           32
#define TILE_MAP_HEIGHT          32

#define PALETTE_SIZE             4

#define LCD_WIDTH                160
#define LCD_HEIGHT               144

typedef enum {
    LCD_CONTROL_NULL              = 0,        //
    LCD_CONTROL_BG_WIN_PRIORITY   = 1u,       // OFF             ON        -> DIFFERENT FOR CGB
    LCD_CONTROL_OBJ_ENABLE        = 1u << 1u, // Disabled        Enabled
    LCD_CONTROL_OBJ_SIZE          = 1u << 2u, // 8x8             8x16
    LCD_CONTROL_BG_TILE_MAP_BANK  = 1u << 3u, // VRAM_TILE_MAP0  VRAM_TILE_MAP1
    LCD_CONTROL_BG_WIN_TILE_BANK  = 1u << 4u, // VRAM_TILE_DATA0 VRAM_TILE_DATA1
    LCD_CONTROL_WIN_ENABLE        = 1u << 5u, // OFF             ON
    LCD_CONTROL_WIN_TILE_MAP_BANK = 1u << 6u, // 9800-0BFF       9C00-9FFF
    LCD_CONTROL_LCD_PPU_ENABLE    = 1u << 7u, // OFF             ON
} lcd_control_bits;
typedef u8 lcd_control;

typedef enum {
    LCD_STATUS_NULL         = 0,        //

    LCD_STATUS_PPU_MODE_MASK = 0x03,

    LCD_STATUS_COINCIDENCE         = 1u << 2u, // OFF         ON
    LCD_STATUS_MODE_0_INTERRUPT    = 1u << 3u, // 8x8         8x16
    LCD_STATUS_MODE_1_INTERRUPT    = 1u << 4u, // 9800-9BFF   9C00-9FFF
    LCD_STATUS_MODE_2_INTERRUPT    = 1u << 5u, // 8800-97FF   8000-8FFF
    LCD_STATUS_LYC_EQ_LY_INTERRUPT = 1u << 6u, // 8800-97FF   8000-8FFF
} lcd_status_bits;
typedef u8 lcd_status;

typedef enum {
    OBJ_ATTR_NULL             = 0,

    OBJ_ATTR_CGB_PALETTE_MASK = 0x07,     // 0-7 value formed by bits 0-2

    OBJ_ATTR_CGB_VRAM_BANK    = 1u << 3u, // VRAM0 or VRAM1
    OBJ_ATTR_DMG_PALETTE      = 1u << 4u, // OBP0 or OBP1
    OBJ_ATTR_X_FLIP           = 1u << 5u, // 1 = Vertical mirroring
    OBJ_ATTR_Y_FLIP           = 1u << 6u, // 1 = Horizontal mirroring
    OBJ_ATTR_PRIORITY         = 1u << 7u, // 1 = BG and Window colors 1-3 drawn over OBJ
} object_attribute_flag_bits;
typedef u8 object_attribute_flags;

typedef enum {
    PPU_MODE_H_BLANK = 0,
    PPU_MODE_V_BLANK,
    PPU_MODE_OAM_SCAN,
    PPU_MODE_DRAWING,
} ppu_mode;

typedef struct {
    u8 y;
    u8 x;
    u8 tile_index;
    object_attribute_flag_bits flags;
} object_attribute;

typedef struct {
    u32 framebuffer[LCD_WIDTH * LCD_HEIGHT];

    u32 shader;
    u32 texture;
} lcd;

typedef struct {
    u8  color_id;
//  TODO
//  u8  palette;
//  u8  sprite_priority;
//  u8  background_priority;
} pixel;

typedef struct {
    // Ring buffer
    pixel pixels[16];
    u8    first;
    u8    size;

    // Fetcher
    u8  step;
    u8  tile_x;
    u8  tile_y;
    u8  tile_index;
    u8  tile_low;
    u8  tile_high;
} fifo;

typedef struct {
    // Registers
    lcd_control lcdc; // LCD Control
    lcd_status  stat; // LCD Status
    u8 scy;           // Viewport Y
    u8 scx;           // Viewport X
    u8 ly;            // LCD Y
    u8 lyc;           // LY compare

    // Internal
    u8 lx;   // Internal scanline counter
    u32 scanline_cycle;

    fifo background_pixels;
    fifo object_pixels;
} ppu;

ppu  ppu_init();
void ppu_cycle(ppu *self, lcd* display);
