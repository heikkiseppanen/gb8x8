#include "ppu.h"

// static u8 vram[MEM_VRAM_END - MEM_VRAM_END] = {};

void ppu_cycle(ppu *ppu) {
    switch (ppu->stat & LCD_STATUS_PPU_MODE_MASK) {
        case PPU_MODE_H_BLANK: {
        }
        case PPU_MODE_V_BLANK: {
        }
        case PPU_MODE_OAM_SCAN: {
        }
        case PPU_MODE_DRAWING: {
        }
    }
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
