#include "ppu.h"

// Dot 4.194MHz
// CPU cycle = dot / 4
// PPU cycle = dot / 2 == 2 * CPU == 2 dots 
// OAM Scan 80 dots == 20 CPU cycles == 40 PPU cycles
// Scanline 456 dots == 114 CPU cycles == 228 PPU cycles

static u8 vram[MEM_EXT_RAM - MEM_VRAM] = {};

inline static u8 vram_read(u8 const *vram, u16 address) {
   GB_ASSERT(MEM_VRAM <= address && address < MEM_EXT_RAM, "VRAM out of range read address");

   return vram[address - MEM_VRAM];
}

inline static lcd_status set_mode(lcd_status stat, ppu_mode mode) {
    return (stat & ~LCD_STATUS_PPU_MODE_MASK) | (mode & LCD_STATUS_PPU_MODE_MASK);
}

inline static void pixel_fetcher_cycle(ppu* ppu);
inline static u16  calculate_tile_address(lcd_control lcdc, u8 tile_index, u8 tile_y);

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
              | LCD_CONTROL_BG_WIN_ENABLE,

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
                } else {
                    self->stat = set_mode(self->stat, PPU_MODE_OAM_SCAN);
                }
                self->lx = 0;
                self->scanline_cycle = 0;
            }
        } break;

        case PPU_MODE_V_BLANK: {
            if (self->scanline_cycle != scanline_cycle_count) return;

            if (++(self->ly) >= LCD_HEIGHT + 10) {
                self->ly = 0;
                self->stat = set_mode(self->stat, PPU_MODE_OAM_SCAN);
            }
            self->scanline_cycle = 0;
        } break;

        case PPU_MODE_OAM_SCAN: {
            const u32 oam_cycle_count = 40;

            if (self->scanline_cycle != oam_cycle_count) return;

            self->stat = set_mode(self->stat, PPU_MODE_DRAWING);
            self->pixel_fetcher     = (pixel_fetcher){};
            self->background_pixels = (pixel_fifo){};
        } break;

        case PPU_MODE_DRAWING: {
            if (self->lx == LCD_WIDTH) {
                self->stat = set_mode(self->stat, PPU_MODE_H_BLANK);
                break;
            }
            
            pixel_fetcher_cycle(self);

            // put pixel to lcd
            if (self->background_pixels.size) {
                if (self->lx == 0) {
                    u8 const discarded_pixels = self->scx % TILE_WIDTH;

                    GB_ASSERT(self->background_pixels.size > 0, "Discarding nonexistant pixels");
                    self->background_pixels.first = (self->background_pixels.first + discarded_pixels) % PIXEL_FIFO_SIZE;
                    self->background_pixels.size -= discarded_pixels;
                }

                // FIFO_POP
                GB_ASSERT(self->background_pixels.size > 0, "Popping nonexistant pixels");
                u8 const color_id = self->background_pixels.pixels[self->background_pixels.first].color_id;
                self->background_pixels.first = (self->background_pixels.first + 1) % PIXEL_FIFO_SIZE;
                self->background_pixels.size -= 1;

                GB_ASSERT(color_id < 4, "FIFO color id out of range");

                // TODO Proper palette lookup
                static u32 const palette[4] = { 0x081820FF, 0x346856FF, 0x88C070FF, 0xE0F8D0FF };

                display->framebuffer[self->lx + (self->ly * LCD_WIDTH)] = palette[color_id];
                self->lx += 1;
            }
        } break;
    }
}

inline static u16 calculate_tile_address(lcd_control lcdc, u8 tile_index, u8 tile_y) {
    u16 block_start;
    u16 tile_offset;

    static const u16 tile_bytes = 16u;

    if (lcdc & LCD_CONTROL_BG_WIN_TILE_BANK) {
        block_start = VRAM_TILE_DATA0;
        tile_offset = (u16)(tile_index) * tile_bytes;
    } else {
        block_start = VRAM_TILE_DATA1;
        tile_offset = tile_index < 128u
                    ? (u16)(tile_index + 128u) * tile_bytes
                    : (u16)(tile_index - 128u) * tile_bytes;
    }

    u16 const line_offset = (u16)(tile_y % 8) * 2u;

    return block_start + tile_offset + line_offset;
}

inline static void pixel_fetcher_cycle(ppu* ppu) {
    pixel_fetcher *fetcher = &ppu->pixel_fetcher;
    switch(fetcher->step) {

        case FETCH_STEP_TILE_ID: {
            u16 const x = ((u16)fetcher->tile_x + (u16)ppu->scx / 8) % TILE_MAP_WIDTH;
            u16 const y = (u16)(TILE_MAP_HEIGHT * (u16)(ppu->ly + ppu->scy) / 8u);
            u16 const address = VRAM_TILE_MAP0 + ((x + y) & 0x03FF);
            fetcher->tile_index = vram_read(vram, address);
            fetcher->step = FETCH_STEP_TILE_LOW;
        } break;

        case FETCH_STEP_TILE_LOW: {
            fetcher->tile_low = vram_read(vram, calculate_tile_address(ppu->lcdc, fetcher->tile_index, (ppu->ly + ppu->scy) % 8));
            fetcher->step = FETCH_STEP_TILE_HIGH;
        } break;

        case FETCH_STEP_TILE_HIGH: {
            fetcher->tile_high = vram_read(vram, calculate_tile_address(ppu->lcdc, fetcher->tile_index, (ppu->ly + ppu->scy) % 8) + 1);
            fetcher->step = FETCH_STEP_FIFO_PUSH;
        } break;

        case FETCH_STEP_FIFO_PUSH: {
            pixel_fifo* output = &ppu->background_pixels; //fetcher->mode == FETCH_MODE_OBJ ? &ppu->object_pixels : &ppu->background_pixels;

            if (output->size > 0) {
                return; // BG mode waits for fifo to be empty
            }

            u8 const low  = fetcher->tile_low;
            u8 const high = fetcher->tile_high;

            for (i8 bit = 7; bit > -1; --bit) {
                u8 const msb_bit = high >> bit & 0x01;
                u8 const lsb_bit = low  >> bit & 0x01;
                u8 const color   = (msb_bit << 1) | lsb_bit;

                // FIFO push
                GB_ASSERT(output->size <= 8, "FIFO OVERFLOW");
                output->pixels[(output->first + output->size) % PIXEL_FIFO_SIZE] = (pixel){ .color_id = color };
                output->size += 1;
            }

            fetcher->tile_x = (fetcher->tile_x + 1);
            fetcher->step = FETCH_STEP_TILE_ID;
        } break;
    }
}
