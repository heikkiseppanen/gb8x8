#pragma once

#include "gb8x8.h"

#define MEM_EXT_RAM_BEGIN    0xA000

#define MEM_VRAM_BEGIN       0x8000
#define MEM_VRAM_END         MEM_EXT_RAM_BEGIN

#define MEM_VRAM_BANK_SWITCH 0xFF4F // CBG only

#define VRAM_TILE_DATA_BEGIN MEM_VRAM_BEGIN
#define VRAM_TILE_DATA_END   0x9800

#define VRAM_TILE_MAP0_BEGIN VRAM_TILE_DATA_END
#define VRAM_TILE_MAP0_END   0x9C00

#define VRAM_TILE_MAP1_BEGIN VRAM_TILE_MAP0_END
#define VRAM_TILE_MAP1_END   0xA000

#define TILE_SIZE            16

#define PALETTE_SIZE         4

typedef enum {
    LCD_CONTROL_NULL            = 0,        //
    LCD_CONTROL_BG_WIN_PRIORITY = 1u,       // OFF         ON        -> DIFFERENT FOR CGB
    LCD_CONTROL_OBJ_ENABLE      = 1u << 1u, // OFF         ON
    LCD_CONTROL_OBJ_SIZE        = 1u << 2u, // 8x8         8x16
    LCD_CONTROL_BG_TILE_MAP     = 1u << 3u, // 9800-9BFF   9C00-9FFF
    LCD_CONTROL_BG_WIN_TILES    = 1u << 4u, // 8800-97FF   8000-8FFF
    LCD_CONTROL_WIN_ENABLE      = 1u << 5u, // OFF         ON
    LCD_CONTROL_WIN_TILE_MAP    = 1u << 6u, // 9800-0BFF   9C00-9FFF
    LCD_CONTROL_LCD_PPU_ENABLE  = 1u << 7u, // OFF         ON
} LCDControlBits;
typedef u8 LCDControl;

typedef enum {
    LCD_STATUS_NULL         = 0,        //

    LCD_STATUS_PPU_MODE_MASK = 0x03,

    LCD_STATUS_COINCIDENCE         = 1u << 2u, // OFF         ON
    LCD_STATUS_MODE_0_INTERRUPT    = 1u << 3u, // 8x8         8x16
    LCD_STATUS_MODE_1_INTERRUPT    = 1u << 4u, // 9800-9BFF   9C00-9FFF
    LCD_STATUS_MODE_2_INTERRUPT    = 1u << 5u, // 8800-97FF   8000-8FFF
    LCD_STATUS_LYC_EQ_LY_INTERRUPT = 1u << 6u, // 8800-97FF   8000-8FFF
} LCDStatusBits;
typedef u8 LCDStatus;

typedef enum {
    PPU_MODE_H_BLANK = 0,
    PPU_MODE_V_BLANK,
    PPU_MODE_OAM_SCAN,
    PPU_MODE_DRAWING,
} PPUMode;

typedef enum {
    OBJ_ATTR_NULL             = 0,

    OBJ_ATTR_CGB_PALETTE_MASK = 0x07,     // 0-7 value formed by bits 0-2
                                          //
    OBJ_ATTR_CGB_VRAM_BANK    = 1u << 3u, // VRAM0 or VRAM1
    OBJ_ATTR_DMG_PALETTE      = 1u << 4u, // OBP0 or OBP1
    OBJ_ATTR_X_FLIP           = 1u << 5u, // 1 = Vertical mirroring
    OBJ_ATTR_Y_FLIP           = 1u << 6u, // 1 = Horizontal mirroring
    OBJ_ATTR_PRIORITY         = 1u << 7u, // 1 = BG and Window colors 1-3 drawn over OBJ
} ObjectAttributeFlagBits;
typedef u8 ObjectAttributeFlags;

typedef struct {
    LCDControl lcdc; // LCD Control
    LCDStatus  stat; // LCD Status
    u8 scy;  // Viewport Y
    u8 scx;  // Viewport X
    u8 ly;   // LCD Y, read-only
    u8 lyc;  // LY compare?

    u8 lx;   // Internal scanline counter
} PPU;

typedef struct {
    u8 y;
    u8 x;
    u8 tile_index;
    ObjectAttributeFlagBits flags;
} ObjectAttribute;


void ppu_cycle(PPU *ppu);
