/*
Chipulate, a Chip-8 emulator written in C++ with SFML, by Mischa 'Aster' Alff.
*/

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

class Chip8
{
public:
	enum error_codes
	{
		NONE,
		LOADING_FILE,
		INTERPRETTING 
	};

private:

	//Constants. Define what a byte is, 
	// and where the program's RAM accessing 
	// will start and stop.
	using byte        = uint8_t;
	using double_byte = uint16_t;
	const double_byte memory_start = 0x200;
	const double_byte memory_end   = 0xFFF;

	//Miscellaneous stuff
	sf::RenderWindow &window;
	std::string filename;

	//The actual emulator.
	// Store Opcodes. Two bytes because Wikipedia said so.
	std::vector<double_byte> program;
	// RAM. 3584 bytes because Wikipedia said so.
	byte memory[3584];
	// 16 registers ( 0 through F ) including flags.
	byte registers[16];
	double_byte address_register;


	//Returns the appropriate byte in RAM.
	// This function is crucial to RAM access
	//  because the program doesn't start
	//  accessing RAM at 0x00, but at memory_start.
	byte &access_memory(const double_byte &address)
	{
		return memory[address-memory_start];
	}


	//Loads the program into the program variable
	bool load_program(const std::string &f_)
	{
		//Set the filename
		filename = f_;

		//Create a set of two bytes to load opcodes in.
		double_byte opcode{0};

		//Commence file reading
		std::ifstream program_file(filename, std::ios::binary | std::ios::in);
		if(!program_file.is_open())
			return false;

		while(program_file.good())
		{
			//Get a byte and assign it to the opcode
			opcode = program_file.get();
			//Shift the byte 8 bits to the right
			// to make space for the next one
			opcode <<= 8;
			//Add the next byte to the opcode,
			// successfully completing the opcode
			opcode += program_file.get();

			program.push_back(opcode);
		}

		return true;
	}

	void run_opcode(const double_byte &opcode)
	{
		if(opcode < 0xFFF)
		{
			//Call RCA program or something
			return;
		}
	}

public:

	bool run(const std::string &f_)
	{
		//Load the file, return error if any
		if(!load_program(f_))
			return error_codes::LOADING_FILE;


		int cursor_pos = 0;
		//Commence loop
		while(cursor_pos < program.size())
		{
			//Run the desired opcode and increment cursor
			execute_opcode(program[cursor_pos++]);
		}


		//We made it through! Return no error.
		return error_codes::NONE;
	}

	Chip8(sf::RenderWindow &w) : window(w) {}
};


int main()
{
	sf::RenderWindow window;
	Chip8 emulator(window);
	int emulator_status = emulator.run("roms/TETRIS");
	if(emulator_status != Chip8::error_codes::NONE)
		return emulator_status;
}