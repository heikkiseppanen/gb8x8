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

inline static u8 vram_read(u8 const *vram, u16 address) {
   GB_ASSERT(MEM_VRAM <= address && address < MEM_EXT_RAM, "VRAM out of range read address");

   return vram[address - MEM_VRAM];
}

inline static lcd_status set_mode(lcd_status stat, ppu_mode mode) {
    return (stat & ~LCD_STATUS_PPU_MODE_MASK) | (mode & LCD_STATUS_PPU_MODE_MASK);
}

inline static u16 calculate_tile_address(lcd_control lcdc, u8 tile_index, u8 tile_y) {
    // TODO Y scroll

    u16 block_start;
    u16 tile_offset;

    if (lcdc & LCD_CONTROL_BG_WIN_TILE_BANK) {
        block_start = VRAM_TILE_DATA0;
        tile_offset = (u16)(tile_index) * 16u;
    } else {
        block_start = VRAM_TILE_DATA1;
        tile_offset = tile_index < 128u
                    ? (u16)(tile_index + 128u) * 16u
                    : (u16)(tile_index - 128u) * 16u;
    }

    u16 const line_offset = (u16)(tile_y % 8) * 2u;

    return block_start + tile_offset + line_offset;
}

ppu ppu_init() {
    u8 *it = vram;
    u8 *end = vram + (VRAM_TILE_DATA1 - MEM_VRAM);

    while (it != end) {
        // Fake tile0
        *it     = 0b00000000; *(++it) = 0b00000000;
        *(++it) = 0b00100100; *(++it) = 0b00000000; 
        *(++it) = 0b01011010; *(++it) = 0b00000000; 
        *(++it) = 0b01011010; *(++it) = 0b00000000; 
        *(++it) = 0b01011010; *(++it) = 0b00000000;
        *(++it) = 0b01011010; *(++it) = 0b00000000; 
        *(++it) = 0b00100100; *(++it) = 0b00000000;
        *(++it) = 0b00000000; *(++it) = 0b00000000; 

        *(++it) = 0b00000000; *(++it) = 0b00000000;
        *(++it) = 0b00000000; *(++it) = 0b00100100; 
        *(++it) = 0b00000000; *(++it) = 0b01011010; 
        *(++it) = 0b00000000; *(++it) = 0b01011010; 
        *(++it) = 0b00000000; *(++it) = 0b01011010;
        *(++it) = 0b00000000; *(++it) = 0b01011010; 
        *(++it) = 0b00000000; *(++it) = 0b00100100;
        *(++it) = 0b00000000; *(++it) = 0b00000000; 
        ++it;
    }

    end = vram + (VRAM_TILE_DATA2 - MEM_VRAM);

    while (it != end) {
        // Fake tile0
        *it     = 0b00000000; *(++it) = 0b00000000;
        *(++it) = 0b00100110; *(++it) = 0b00000000; 
        *(++it) = 0b01010010; *(++it) = 0b00000000; 
        *(++it) = 0b01010010; *(++it) = 0b00000000; 
        *(++it) = 0b01010010; *(++it) = 0b00000000;
        *(++it) = 0b01010010; *(++it) = 0b00000000; 
        *(++it) = 0b00100010; *(++it) = 0b00000000;
        *(++it) = 0b00000000; *(++it) = 0b00000000; 

        *(++it) = 0b00000000; *(++it) = 0b00000000;
        *(++it) = 0b00000000; *(++it) = 0b00100110; 
        *(++it) = 0b00000000; *(++it) = 0b01010010; 
        *(++it) = 0b00000000; *(++it) = 0b01010010; 
        *(++it) = 0b00000000; *(++it) = 0b01010010;
        *(++it) = 0b00000000; *(++it) = 0b01010010; 
        *(++it) = 0b00000000; *(++it) = 0b00100010;
        *(++it) = 0b00000000; *(++it) = 0b00000000; 
        ++it;
    }

    end = vram + (VRAM_TILE_MAP0 - MEM_VRAM);

    while (it != end) {
        *it     = 0b00000000; *(++it) = 0b00000000;
        *(++it) = 0b00100110; *(++it) = 0b00000000; 
        *(++it) = 0b01010010; *(++it) = 0b00000000; 
        *(++it) = 0b01010110; *(++it) = 0b00000000; 
        *(++it) = 0b01010100; *(++it) = 0b00000000;
        *(++it) = 0b01010100; *(++it) = 0b00000000; 
        *(++it) = 0b00100110; *(++it) = 0b00000000;
        *(++it) = 0b00000000; *(++it) = 0b00000000; 

        *(++it) = 0b00000000; *(++it) = 0b00000000;
        *(++it) = 0b00000000; *(++it) = 0b00100110; 
        *(++it) = 0b00000000; *(++it) = 0b01010010; 
        *(++it) = 0b00000000; *(++it) = 0b01010110; 
        *(++it) = 0b00000000; *(++it) = 0b01010100;
        *(++it) = 0b00000000; *(++it) = 0b01010100; 
        *(++it) = 0b00000000; *(++it) = 0b00100110;
        *(++it) = 0b00000000; *(++it) = 0b00000000; 
        ++it;
    }

    end = vram + (VRAM_END - MEM_VRAM);

    while (it != end) {
        *(it++) = 0;
        *(it++) = 1;
        *(it++) = 128;
        *(it++) = 129;
    }

    return (ppu){
        // Documented to power up with 0x91
        .lcdc = LCD_CONTROL_LCD_PPU_ENABLE
              | LCD_CONTROL_WIN_TILE_MAP_BANK
              | LCD_CONTROL_BG_WIN_PRIORITY,

        // Documented to power up with 0x81 (DMG0) and 0x85 (DMG)
        .stat = LCD_STATUS_LYC_EQ_LY_INTERRUPT
              | LCD_STATUS_COINCIDENCE
              | (LCD_STATUS_PPU_MODE_MASK & PPU_MODE_V_BLANK),
    };
}

void ppu_cycle(ppu *self, lcd* display) {
    static const u32 scanline_cycle_count = 228;

    self->scanline_cycle += 1;

    switch (self->stat & LCD_STATUS_PPU_MODE_MASK) {
        case PPU_MODE_H_BLANK: {
            if (self->scanline_cycle >= scanline_cycle_count) {
                if (++(self->ly) >= LCD_HEIGHT) {
                    self->stat = set_mode(self->stat, PPU_MODE_V_BLANK);
                    // GB_INFO("HBLANK -> VBLANK");
                } else {
                    self->stat = set_mode(self->stat, PPU_MODE_OAM_SCAN);
                    // GB_INFO("HBLANK -> OAM");
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
                        // GB_INFO("VBLANK -> OAM");
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
                // GB_INFO("OAM -> DRAW");
            }
        } break;

        case PPU_MODE_DRAWING: {
            if (self->lx == LCD_WIDTH) {
                self->stat = set_mode(self->stat, PPU_MODE_H_BLANK);
                // GB_INFO("DRAW -> HBLANK");
                break;
            }
            
            fifo *bg = &self->background_pixels;
            switch(bg->step) {
                case 0: {
                    u16 const x = ((u16)bg->tile_x + (u16)self->scx / 8) & 0x001f;
                    bg->tile_y  = (u16)(self->ly + self->scy) & 0xFF;

                    u16 const address = VRAM_TILE_MAP0 + ((x + bg->tile_y * TILE_MAP_WIDTH) & 0x03FF);
                    bg->tile_index = vram_read(vram, address);
                    bg->step += 1;
                } break;

                case 1: {
                    bg->tile_low = vram_read(vram, calculate_tile_address(self->lcdc, bg->tile_index, bg->tile_y));
                    bg->step += 1;
                } break;

                case 2: {
                    bg->tile_high = vram_read(vram, calculate_tile_address(self->lcdc, bg->tile_index, bg->tile_y) + 1);
                    bg->step += 1;
                } break;

                case 3: bg->step += 1; break; // Sleep

                case 4: {
                    if (bg->size > 0) break; // BG fetcher waits for fifo to be empty

                    u8 const low = bg->tile_low;
                    u8 const high = bg->tile_high;

                    for (i8 bit = 7; bit > -1; --bit) {
                        u8 const msb_bit = high >> bit & 0x01;
                        u8 const lsb_bit = low  >> bit & 0x01;
                        u8 const color = (msb_bit << 1) | lsb_bit;

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
                if (self->lx == 0) {
                    u8 const discarded_pixels = self->scx & 0x07;
                    bg->first = (bg->first + discarded_pixels) & 0x0F;
                    bg->size -= discarded_pixels;
                }
                // FIFO_POP
                u8 const color_id = bg->pixels[bg->first].color_id;
                bg->first = (bg->first + 1) & 0x0F;
                bg->size  -= 1;

                GB_ASSERT(color_id < 4, "FIFO color id out of range");


                // TODO Proper palettes
                static u32 const palette[4] = {
                    0x081820FF,
                    0x346856FF,
                    0x88C070FF,
                    0xE0F8D0FF
                };

                // GB_INFO("Putting pixel %d %d", self->lx, self->ly);
                display->framebuffer[self->lx + (self->ly * LCD_WIDTH)] = palette[color_id];
                self->lx += 1;
            }
        } break;
    }
}
