#include "ppu.h"

// static u8 vram[MEM_VRAM_END - MEM_VRAM_END] = {};

// Dot 4.194MHz
// CPU dot / 4
// PPU dot / 2 == 2 * CPU
// OAM Scan 80 dots == 20 CPU cycles == 40 PPU cycles
// Scanline 456 dots == 114 CPU cycles == 228 PPU cycles

inline static const char *mode_to_string(ppu_mode mode) {
    switch (mode) {
        case PPU_MODE_H_BLANK:  return "H_BLANK";
        case PPU_MODE_V_BLANK:  return "V_BLANK";
        case PPU_MODE_OAM_SCAN: return "OAM_SCAN";
        case PPU_MODE_DRAWING:  return "DRAWING";

        default:                return "NULL";
    }
}

inline static lcd_status set_mode(lcd_status stat, ppu_mode mode) {
    const lcd_status new_stat = (stat & ~LCD_STATUS_PPU_MODE_MASK) | (mode & LCD_STATUS_PPU_MODE_MASK);
    return new_stat;
}

//bool ppu_create(ppu* self) {
//    *self = (ppu){
//        .
//    };
//}

void ppu_cycle(ppu *ppu) {
    const u32 scanline_cycles = 228;
    switch (ppu->stat & LCD_STATUS_PPU_MODE_MASK) {
        case PPU_MODE_H_BLANK: {
            if (ppu->cycle_count == scanline_cycles) {
                ppu_mode next_mode = ++(ppu->ly) < LCD_HEIGHT
                    ? PPU_MODE_OAM_SCAN
                    : PPU_MODE_V_BLANK;
                ppu->stat = set_mode(ppu->stat, next_mode);
                ppu->cycle_count = 0;
            }
        } break;

        case PPU_MODE_V_BLANK: {
            if (ppu->cycle_count == scanline_cycles && ++(ppu->ly) >= LCD_HEIGHT + 10) {
                    ppu->stat = set_mode(ppu->stat, PPU_MODE_OAM_SCAN);
                    ppu->cycle_count = 0;
            }
        } break;

        case PPU_MODE_OAM_SCAN: {
            const u32 oam_cycle_count = 40;
            if (ppu->cycle_count == oam_cycle_count) {
                ppu->stat = set_mode(ppu->stat, PPU_MODE_DRAWING);
            }
        } break;

        case PPU_MODE_DRAWING: {
            const u32 random_cycle_count = 40 + 100;
            if (ppu->cycle_count == random_cycle_count) {
                ppu->stat = set_mode(ppu->stat, PPU_MODE_H_BLANK);
            }
        } break;
    }

    ppu->cycle_count += 1;
}

//}
//    u8 bytes[16] = {0x3C, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x5E, 0x7E, 0x0A, 0x7C, 0x56, 0x38, 0x7C};
//
//
//    // Conversion of 2 bytes into lines of 2 bit color IDs
//    for (u32 y = 0; y < 8; ++y) {
//        u16 lsb = bytes[y * 2];
//        u16 msb = bytes[y * 2 + 1];
//
//        printf("%08b %c %08b %c ",
//            lsb, y == 4 ? '+' : ' ',
//            msb, y == 4 ? '=' : ' ');
//
//        u16 line = 0;
//        for (u8 bit = 0; bit < 8; ++bit) {
//            ids |= ((lsb & 1) | ((msb & 1) << 1)) << (bit * 2);
//            lsb >>= 1;
//            msb >>= 1;
//        }
//
//        printf("%016b\n", ids);
//    }
//};
