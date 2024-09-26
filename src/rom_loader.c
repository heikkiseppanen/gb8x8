#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define MAGIC_BYTE_LOCATION 0x0147
#define GAME_NAME_LOCATION 0x0134
#define MEMORY_BANK_INFO_LOCATION 0x148
#define GAME_BOY_COLOUR_MAGIC_BYTE 0x0143
#define ROM_BLOCK_SIZE 0x4000
#define RAM_BLOCK_SIZE 0x2000
#define	MAX_RAM_SIZE 0x8000

//https://gbdev.gg8.se/wiki/articles/The_Cartridge_Header

typedef enum {
    ROM_ONLY = 0x00,
    MBC1 = 0x01,
    MBC1_RAM = 0x02,
    MBC1_RAM_BATTERY = 0x03,
	NOT_SUPPORTED = 0x04, 
    MBC2 = 0x05,
    MBC2_BATTERY = 0x06,
    ROM_RAM = 0x08,
    ROM_RAM_BATTERY = 0x09,
    MMM01 = 0x0B,
    MMM01_RAM = 0x0C,
    MMM01_RAM_BATTERY = 0x0D,
    MBC3_TIMER_BATTERY = 0x0F,
    MBC3_TIMER_RAM_BATTERY = 0x10,
    MBC3 = 0x11,
    MBC3_RAM = 0x12,
    MBC3_RAM_BATTERY = 0x13,
    MBC4 = 0x15,
    MBC4_RAM = 0x16,
    MBC4_RAM_BATTERY = 0x17,
    MBC5 = 0x19,
    MBC5_RAM = 0x1A,
    MBC5_RAM_BATTERY = 0x1B,
    MBC5_RUMBLE = 0x1C,
    MBC5_RUMBLE_RAM = 0x1D,
    MBC5_RUMBLE_RAM_BATTERY = 0x1E,
	CGB_ENABLED_ONE = 0x80,
	CGB_ENABLED_TWO = 0xC0,
    POCKET_CAMERA = 0xFC,
    BANDAI_TAMA5 = 0xFD,
    HuC3 = 0xFE,
    HuC1_RAM_BATTERY = 0xFF
} e_rom_types;

typedef struct{
	e_rom_types	rom_type;
	size_t		rom_size;
	char		*rom_name;
	uint8_t		*raw_data;
	uint16_t	num_of_blocks;
	uint16_t	memory_bank_selected;
	uint8_t		ram[MAX_RAM_SIZE];
	
}	t_memory;

e_rom_types get_rom_type(uint8_t* rom)
{
  switch((e_rom_types)rom[MAGIC_BYTE_LOCATION]) 
  {
    case ROM_ONLY:
      return ROM_ONLY;

    case MBC1:
    case MBC1_RAM:
      return MBC1;
	// TODO: Unleash support for other types, when fully implemented
    case MBC1_RAM_BATTERY:
    case MBC2:
    case MBC2_BATTERY:
    case ROM_RAM:
    case ROM_RAM_BATTERY:
    case MMM01:
    case MMM01_RAM:
    case MMM01_RAM_BATTERY:
    case MBC3_TIMER_BATTERY:
    case MBC3_TIMER_RAM_BATTERY:
	case MBC3:
    case MBC3_RAM:
    case MBC3_RAM_BATTERY:
    case MBC4:
    case MBC4_RAM:
    case MBC4_RAM_BATTERY:
    case MBC5:
    case MBC5_RAM:
    case MBC5_RAM_BATTERY:
    case MBC5_RUMBLE:
    case MBC5_RUMBLE_RAM:
    case MBC5_RUMBLE_RAM_BATTERY:
    case POCKET_CAMERA:
    case BANDAI_TAMA5:
    case HuC3:
    case HuC1_RAM_BATTERY:
    default:
     return NOT_SUPPORTED;
  }
}


void load_rom(char const *file_name, t_memory *memory)
{
	int			file_descriptor;
	struct stat	file_stats;

	*memory = (t_memory){0};
	if (!file_name) return; //just to be sure
	stat(file_name, &file_stats);
	memory->rom_size = file_stats.st_size;
	if (memory->rom_size <= 0x014F){
		write(2, "Error: File size incorrect!\n", 28);//TODO: use our gameboys logging and better error
		return;
	}
	file_descriptor = open(file_name, O_RDONLY | __O_CLOEXEC);
	if (file_descriptor < 0){
		write(2, "Error: Failed to open the inputed rom file!\n", 44);//TODO: use our gameboys logging and better error
		return;
	}
	memory->raw_data = (uint8_t *)malloc(sizeof(uint8_t) * memory->rom_size);
	if (!memory->raw_data){ //TODO: move this to the stack possibly
		write(2, "Error\n", 6);//TODO: use our gameboys logging and better error
		close(file_descriptor);
		return;
	}
	if (read(file_descriptor, memory->raw_data, memory->rom_size) != memory->rom_size){
		write(2, "Error reading the file\n", 23);//TODO: use our gameboys logging and better error
		close(file_descriptor);
		free(memory->raw_data);
		return;
	}
	memory->rom_type = get_rom_type(memory->raw_data);
	memory->num_of_blocks = memory->rom_size / ROM_BLOCK_SIZE;
	memory->memory_bank_selected = 1; //0 is fixed to 0x0-0x3FFF
	memory->rom_name = (char *)&memory->raw_data[GAME_NAME_LOCATION];
	close(file_descriptor);
	return;
}
uint8_t	fetch_high_memory_byte(t_memory *memory, uint16_t const address)
{
	if (address < 0xFE00){
		return memory->ram[address - 0xE000];
	} else if (address < 0xFEA0){
		return 0; //TODO: OAM memory (Object Attribute Memory)
    }else if (address < 0xFF00){
		return 0; //TODO: low empty implementation
    }else if (address == 0xFF00){
		return 0; //TODO: handle keys
    }else if (address < 0xFF80){
		return 0; //TODO: low high implementation
    }else if (address < 0xFFFF){
		return 0; //TODO: low running ram implementation
    }else{
		return 0; //TODO: return IR register
    }
}

uint8_t	fetch_byte(t_memory *memory, uint16_t const address)
{
  switch(address & 0xF000)
    {
	//Bootloader case TODO: make bootloader
    case 0x0000:
      return 0;
    case 0x1000:
    case 0x2000:
    case 0x3000:
      return memory->raw_data[address];
    case 0x4000:
    case 0x5000:
    case 0x6000:
    case 0x7000:
      return memory->raw_data[address + memory->memory_bank_selected * ROM_BLOCK_SIZE]; //Have not implemented ROM bank switching, it was breaking
	//BELOW ARE RAM CASES
	//TODO: Implement ram and ram banking
    case 0x8000:
    case 0x9000:
    case 0xA000:
    case 0xB000:
    case 0xC000:
    case 0xD000:
    case 0xE000:
		return 0;
    default:
      return fetch_high_memory_byte(memory, address);
    }
  return 0;
}

int main(int ac, char **av)
{
	t_memory	game_memory;
	e_rom_types	rom_type;

	if (ac != 2){
		write(2, "No input files\n", 13); //TODO: use our gameboys logging
		return EXIT_FAILURE;
	}
		
	load_rom(av[1], &game_memory);
	if (game_memory.raw_data == NULL){
		write(2, "Error\n", 6); //TODO: use our gameboys logging
		return EXIT_FAILURE;
	}
	write(1, "Game loaded: ", 13);
	write(1, game_memory.rom_name, strlen(game_memory.rom_name));
	write(1, "\n", 1);

	if (game_memory.rom_type == NOT_SUPPORTED){
		write(2, "Error: Unsupported rom type!\n", 29); //TODO: use our gameboys logging
		free(game_memory.raw_data);
		return EXIT_FAILURE;
	}

	free(game_memory.raw_data);
	return EXIT_SUCCESS;
}

