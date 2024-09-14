#include "ppu.h"

// static u8 vram[MEM_VRAM_END - MEM_VRAM_END] = {};

// Dot 4.194MHz
// CPU cycle = dot / 4
// PPU cycle = dot / 2 == 2 * CPU == 2 dots 
// OAM Scan 80 dots == 20 CPU cycles == 40 PPU cycles
// Scanline 456 dots == 114 CPU cycles == 228 PPU cycles

static u8 vram[MEM_EXT_RAM - MEM_VRAM] = {};

//inline static const char *mode_to_string(ppu_mode mode) {
//    switch (mode) {
//        case PPU_MODE_H_BLANK:  return "H_BLANK";
//        case PPU_MODE_V_BLANK:  return "V_BLANK";
//        case PPU_MODE_OAM_SCAN: return "OAM_SCAN";
//        case PPU_MODE_DRAWING:  return "DRAWING";
//
//        default:                return "NULL";
//    }
//}

inline static lcd_status set_mode(lcd_status stat, ppu_mode mode) {
    return (stat & ~LCD_STATUS_PPU_MODE_MASK) | (mode & LCD_STATUS_PPU_MODE_MASK);
}

ppu ppu_init() {
    u8 *it = vram;

    // Fake tile0
    *it     = 0b11110000; *(++it) = 0b00001111;
    *(++it) = 0b11110000; *(++it) = 0b00001111; 
    *(++it) = 0b11110000; *(++it) = 0b00001111; 
    *(++it) = 0b11110000; *(++it) = 0b00001111; 
    *(++it) = 0b11110000; *(++it) = 0b11110000;
    *(++it) = 0b11110000; *(++it) = 0b11110000; 
    *(++it) = 0b11110000; *(++it) = 0b11110000;
    *(++it) = 0b11110000; *(++it) = 0b11110000; 

    // Fake tile1
    *(++it) = 0b00000000; *(++it) = 0b00000000;
    *(++it) = 0b00000000; *(++it) = 0b01010100; 
    *(++it) = 0b00000000; *(++it) = 0b01110100; 
    *(++it) = 0b00000000; *(++it) = 0b01010100; 
    *(++it) = 0b00000000; *(++it) = 0b00000000; 
    *(++it) = 0b00101000; *(++it) = 0b00000000;
    *(++it) = 0b10000010; *(++it) = 0b00000000; 
    *(++it) = 0b11111110; *(++it) = 0b00000000;

    u8 *idx = vram + (VRAM_TILE_MAP0 - MEM_VRAM);
    u8 *end = vram + (VRAM_TILE_MAP1 - MEM_VRAM);
    while (idx != end) {
        *(idx++) = 0;
        *(idx++) = 1;
    }

    return (ppu){
        .lcdc = LCD_CONTROL_BG_WIN_TILE_BANK, // To use bank 0
        .stat = set_mode(LCD_STATUS_NULL, PPU_MODE_DRAWING),
    };
}

void ppu_cycle(ppu *self, lcd* display) {
    static const u32 scanline_cycle_count = 228;
    switch (self->stat & LCD_STATUS_PPU_MODE_MASK) {
        case PPU_MODE_H_BLANK: {
            if (self->scanline_cycle >= scanline_cycle_count) {
                if (++(self->ly) >= LCD_HEIGHT) {
                    self->stat = set_mode(self->stat, PPU_MODE_V_BLANK);
                    GB_INFO("HBLANK -> VBLANK");
                } else {
                    self->stat = set_mode(self->stat, PPU_MODE_OAM_SCAN);
                    GB_INFO("HBLANK -> OAM");
                }
                self->lx = 0;
                self->scanline_cycle = 0;
            }
        } break;

        case PPU_MODE_V_BLANK: {
            if (self->scanline_cycle == scanline_cycle_count) {
                    if (++(self->ly) >= LCD_HEIGHT + 10) {
                        self->ly = 0;
                        self->stat = set_mode(self->stat, PPU_MODE_OAM_SCAN);
                        GB_INFO("VBLANK -> OAM");
                    }
                    self->scanline_cycle = 0;
            }
        } break;

        case PPU_MODE_OAM_SCAN: {
            const u32 oam_cycle_count = 40;
            if (self->scanline_cycle == oam_cycle_count) {
                self->stat = set_mode(self->stat, PPU_MODE_DRAWING);
                // TODO Clear FIFOs
                self->background_pixels = (fifo){};
                GB_INFO("OAM -> DRAW");
            }
        } break;

        case PPU_MODE_DRAWING: {
            if (self->lx == LCD_WIDTH) {
                self->stat = set_mode(self->stat, PPU_MODE_H_BLANK);
                GB_INFO("DRAW -> HBLANK");
                break;
            }
            
            fifo *bg = &self->background_pixels;
            switch(bg->step) {
                case 0: {
                    // TODO banking and scrolling
                    const u16 tile_x = self->lx / 8;
                    const u16 tile_y = self->ly / 8;
                    const u16 address = VRAM_TILE_MAP0 + ((tile_x + (TILE_MAP_WIDTH * tile_y)) & 0x03ff);
                    bg->tile_index = vram[address - MEM_VRAM];
                    bg->step += 1;
                } break;

                case 1: {
                    // TODO banking and scrolling
                    const u16 tile_offset = (u16)bg->tile_index * 16u;
                    const u16 line_offset = 2u * ((u16)self->ly & 0x07);
                    const u16 address = VRAM_TILE_DATA0 + tile_offset + line_offset;
                    bg->tile_low = vram[address - MEM_VRAM];
                    bg->step += 1;
                } break;

                case 2: {
                    // TODO banking and scrolling
                    const u16 tile_offset = (u16)bg->tile_index * 16u;
                    const u16 line_offset = 2u * ((u16)self->ly & 0x07);
                    const u16 address = VRAM_TILE_DATA0 + tile_offset + line_offset + 1;
                    bg->tile_high = vram[address - MEM_VRAM];
                    bg->step += 1;
                } break;

                case 3: {
                    if (bg->size > 0) break; // BG fetcher waits for fifo to be empty

                    u8 lsb = bg->tile_low;
                    u8 msb = bg->tile_high;

                    for (i8 bit = 7; bit > -1; --bit) {
                        u8 msb_bit = msb >> bit & 0x1;
                        u8 lsb_bit = lsb >> bit & 0x1;
                        u8 color = (msb_bit << 1) | lsb_bit;

                        // FIFO push
                        bg->pixels[(bg->first + bg->size) & 0x0F] = (pixel){ .color_id = color };
                        bg->size += 1;
                    }

                    bg->tile_x = (bg->tile_x + 1) & 0x1F;
                    bg->step = 0;
                } break;
            }

            // put pixel to lcd
            if (bg->size) {
                // FIFO_POP
                const u8 color_id = bg->pixels[bg->first].color_id;

                GB_ASSERT(color_id < 4, "FIFO color id out of range");

                bg->first = (bg->first + 1) & 0x0F;
                bg->size  -= 1;

                // TODO Proper palettes
                static const u32 palette[4] = {
                    0x081820FF,
                    0x346856FF,
                    0x88C070FF,
                    0xE0F8D0FF
                };

                GB_INFO("Putting pixel %d %d", self->lx, self->ly);
                display->framebuffer[self->lx + (self->ly * LCD_WIDTH)] = palette[color_id];
                self->lx += 1;
            }
        } break;
    }

    self->scanline_cycle += 1;
}
