/*
Chipulate, a Chip-8 emulator written in C++ with SFML, by Mischa 'Aster' Alff.
*/

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

class Chip8
{
public:
	enum error_code
	{
		NONE,
		LOADING_FILE,
		INSTRUCTION_NOT_FOUND
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
	std::string filename;

	//Graphics stuff
	sf::RenderWindow &window;
	sf::Texture render_target;
	sf::Sprite screen;

	//The actual emulator.
	// Store Opcodes. Two bytes because Wikipedia said so.
	std::vector<double_byte> program;
	//The cursor position when reading the program
	int cursor_pos;
	// RAM. 3584 bytes because Wikipedia said so.
	byte memory[4096];
	// 16 registers ( 0 through F ) including flags.
	byte registers[16];
	double_byte address_register;
	//Timers, both decremented at 60Hz
	//When timer_sound reaches 0, play a beep.
	//These are stored in the Chip-8 RAM, so
	// we'll keep them as references.
	byte &timer_delay{memory[0x8]}, timer_sound{memory[0x9]};
	//Fonts are stored in the Chip-8 RAM,
	// since we define them, we also keep
	// their position.
	byte font_pos = 0xF;



	//Just a few helper functions to make
	// code cleaner
	inline const bool get_bit(const byte &a, const int &pos)
	{
		return ((a>>(0x8-pos))&1);
	}
	inline const bool get_bit(const double_byte &a, const int &pos)
	{
		return ((a>>(0xF-pos))&1);
	}
	inline const byte get_4bit(const byte &a, const int &pos)
	{
		return ((a>>((0x1-pos)*4))&0xF);
	}
	inline const byte get_4bit(const double_byte &a, const int &pos)
	{
		return ((a>>((0x3-pos)*4))&0xF);
	}
	inline const byte get_byte(const double_byte &a, const int &pos)
	{
		return ((a>>((0x1-pos)*8))&0xFF);
	}

	//Returns the appropriate byte in RAM.
	// This function is crucial to RAM access
	//  because the program doesn't start
	//  accessing RAM at 0x00, but at memory_start.
	inline byte &access_memory(const double_byte &address)
	{
		return memory[address];
	}

	std::map<byte, sf::Keyboard::Key> keymap
	{
		{0x0, sf::Keyboard::Num0},
		{0x1, sf::Keyboard::Num1},
		{0x2, sf::Keyboard::Num2},
		{0x3, sf::Keyboard::Num3},
		{0x4, sf::Keyboard::Num4},
		{0x5, sf::Keyboard::Num5},
		{0x6, sf::Keyboard::Num6},
		{0x7, sf::Keyboard::Num7},
		{0x8, sf::Keyboard::Num8},
		{0x9, sf::Keyboard::Num9},
		{0xA, sf::Keyboard::A},
		{0xB, sf::Keyboard::B},
		{0xC, sf::Keyboard::C},
		{0xD, sf::Keyboard::D},
		{0xE, sf::Keyboard::E},
		{0xF, sf::Keyboard::F}
	};


	//Translates a Chip-8 keycode into an
	// SFML Keycode.
	inline const sf::Keyboard::Key &translate_key(const byte &keycode)
	{
		return keymap[keycode];
	}

	//Translates an SFML keycode into a
	// Chip-8 keycode
	inline const byte translate_key(const sf::Keyboard::Key &keycode)
	{
		//Iterate through the map until
		// we find the corresponding key
		for(auto &it : keymap)
			if(it.second == keycode)
				return it.first;
		return 0x0;
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

			std::cout<<std::hex<<opcode;
			((program.size()%8)==0)?(std::cout<<"\n"):(std::cout<<" ");

			program.push_back(opcode);
		}
		std::cout<<"\n";

		return true;
	}

	//Chip-8 has built in 4x5 fonts that
	// work just like sprites. Therefore
	// you need 5 bytes to represent each
	// one, and there are 16 of them.
	//They should be stored below
	// memory_start.
	void load_font()
	{
		std::vector<byte> font =
		{0xF0,0x90,0x90,0x90,0xF0,
		 0x20,0x60,0x20,0x20,0x70,
		 0xF0,0x10,0xF0,0x80,0xF0,
		 0xF0,0x10,0xF0,0x10,0xF0,
		 0x90,0x90,0xF0,0x10,0x10,
		 0xF0,0x80,0xF0,0x10,0xF0,
		 0xF0,0x80,0xF0,0x90,0xF0,
		 0xF0,0x10,0x20,0x40,0x40,
		 0xF0,0x90,0xF0,0x90,0xF0,
		 0xF0,0x90,0xF0,0x10,0xF0,
		 0xF0,0x90,0xF0,0x90,0x90,
		 0xE0,0x90,0xE0,0x90,0xE0,
		 0xF0,0x80,0x80,0x80,0xF0,
		 0xE0,0x90,0x90,0x90,0xE0,
		 0xF0,0x80,0xF0,0x80,0xF0,
		 0xF0,0x80,0xF0,0x80,0x80};

		for(unsigned int i = 0; i < font.size(); ++i)
		{
			memory[font_pos+i] = font[i];
		}
	}

	error_code execute_opcode(const double_byte &opcode)
	{
		// 0x00E0
		// Clear window opcode
		if(opcode == 0x00E0)
		{
			window.clear(sf::Color::Black);
			return error_code::NONE;
		}

		// 0x00EE
		// Return from subroutine
		if(opcode == 0x00EE)
		{
			// TBI
			std::cout<<"Ran TBI opcode, 0x00EE\n";
			return error_code::NONE;
		}

		// 0x0NNN
		//Call RCA program at NNN or something
		if(opcode <= 0x0FFF)
		{
			// TBI
			std::cout<<"Ran TBI opcode, 0x0NNN\n";
			return error_code::NONE;
		}

		// 0x1NNN
		// Jump to address at NNN
		if(opcode >= 0x1000 && opcode <= 0x1FFF)
		{
			// TBI
			std::cout<<"Ran TBI opcode, 0x1NNN\n";
			return error_code::NONE;
		}

		// 0x2NNN
		// Call subroutine at NNN
		if(opcode >= 0x2000 && opcode <= 0x2FFF)
		{
			// TBI
			std::cout<<"Ran TBI opcode, 0x2NNN\n";
			return error_code::NONE;
		}

		// 0x3XNN
		// Skip next instruction if registers[X]
		// is equal to NN
		if(opcode >= 0x3000 && opcode <= 0x3FFF)
		{
			if(registers[get_4bit(opcode, 1)] == (opcode<<8)>>8)
				++cursor_pos;
			return error_code::NONE;
		}

		// 0x4XNN
		// Skip next instruction if registers[X]
		// isn't equal to NN
		if(opcode >= 0x4000 && opcode <= 0x4FFF)
		{
			if(registers[get_4bit(opcode, 1)] != (opcode<<8)>>8)
				++cursor_pos;
			return error_code::NONE;
		}

		// 0x5XY0
		// Skip next instruction if registers[X]
		// is equal to registers[Y]
		if(opcode >= 0x5000 && opcode <= 0x5FF0)
		{
			if(registers[get_4bit(opcode, 1)] == registers[(opcode>>4)&0xF])
				++cursor_pos;
			return error_code::NONE;
		}

		// 0x6XNN
		// Set registers[X] to NN
		if(opcode >= 0x6000 && opcode <= 0x6FFF)
		{
			registers[get_4bit(opcode, 1)] = (opcode)&0xFF;
			return error_code::NONE;
		}

		// 0x7XNN
		// Add NN to registers[X]
		if(opcode >= 0x7000 && opcode <= 0x7FFF)
		{
			registers[(opcode>>8)&0xF] += (opcode)&0xFF;
			return error_code::NONE;
		}

		// 0x8XY[0-E]
		// Arithmetics Stuff
		if(opcode >= 0x8000 && opcode <= 0x8FFE)
		{
			byte type = opcode;
			type <<= 4;
			type >>= 4;
			int temp;
			unsigned int utemp;
			byte x = (opcode>>8)&0xF,
			     y = (opcode>>4)&0xF;
			switch(type)
			{
			case 0x0:
				// registers[X] = registers[Y]
				registers[x] = registers[y];
				break;
			case 0x1:
				// registers[X] = registers[X] OR registers[Y]
				registers[x] |= registers[y];
				break;
			case 0x2:
				// registers[X] = registers[X] AND registers[Y]
				registers[x] &= registers[y];
				break;
			case 0x3:
				// registers[X] = registers[X] XOR registers[Y]
				registers[x] ^= registers[y];
				break;
			case 0x4:
				// registers[X] = registers[X] + registers[Y] with carry
				utemp = registers[x] + registers[y];
				registers[0xF] = ((utemp&255) != utemp);
				registers[x] = utemp;
				break;
			case 0x5:
				// registers[X] = registers[X] - registers[Y] with borrow
				temp = registers[x] - registers[y];
				registers[0xF] = (temp < 0);
				registers[x] = temp;
				break;
			case 0x6:
				// registers[X] = registers[X] >> 1,
				// Flags register gets set to least significant bit
				registers[0xF] = (registers[x] << 7)>>7;
				registers[x] >>= 1;
				break;
			case 0x7:
				// registers[X] = registers[Y] - registers[X] with borrow
				temp = registers[y] - registers[x];
				registers[0xF] = (temp < 0);
				registers[x] = temp;
				break;
			case 0xE:
				// registers[X] = registers[X] << 1,
				// Flags register gets set to most significant bit
				registers[0xF] = (registers[x] >> 7);
				registers[x] <<= 1;
				break;
			default:
				return error_code::INSTRUCTION_NOT_FOUND;
				break;
			}
			return error_code::NONE;
		}

		//0x9XY0
		//Skips next instruction if
		// registers[X] isn't equal to
		// registers[Y]
		if(opcode >= 0x9000 && opcode <= 0x9FF0)
		{
			if(registers[(opcode>>8)&0xF] != registers[(opcode>>4)&0xF])
				++cursor_pos;
			return error_code::NONE;
		}

		//0xANNN
		//Sets the address register to NNN
		if(opcode >= 0xA000 && opcode <= 0xAFFF)
		{
			address_register = opcode - 0xA000;
			return error_code::NONE;
		}

		//0xBNNN
		//Jumps to NNN + register[0x0]
		if(opcode >= 0xB000 && opcode <= 0xBFFF)
		{
			//TBI
			std::cout<<"Ran TBI opcode, 0xBNNN\n";
			return error_code::NONE;
		}

		//0xCXNN
		//Sets X to a random number
		// between 0 and NN
		if(opcode >= 0xC000 && opcode <= 0xCFFF)
		{
			registers[(opcode>>8)&0xF] = (rand() & ((opcode)&0xFF));
			return error_code::NONE;
		}

		//0xDXYN
		//Draws a sprite at coord x,y of width 8 and height N
		//The sprite is stored as a series of bits at the
		// address pointer.
		if(opcode >= 0xD000 && opcode <= 0xDFFF)
		{
			//Get the position and height
			byte pos_x,pos_y,height;
			std::cout<<opcode - 0xD000<<": ";
			pos_x = (opcode>>8)&0xF;
			pos_y = (opcode<<4)&0xF;
			height = (opcode)&0xF;
			std::cout<<"pos_x="<<(int)pos_x<<" pos_y="<<(int)pos_y<<" height="<<(int)height<<"\n";

			//Get an image of what's being rendered to screen
			sf::Image image = render_target.copyToImage();

			//Create array of pixels. height*8*4 because 8 pixels
			// wide and each pixel is 4 bytes (one for each color)
			byte pixels[height*8*4];

			registers[0xF] = 0;

			//Commence drawing loop
			for(int y=0;y<height;++y)
			{
				//Get the byte that represents this row of the sprite.
				byte &subject = access_memory(address_register+y);

				//Iterate through each bit of the byte
				for(int x=7;x>=0;--x)
				{
					//Check if the bit is set to 1 or not
					if(get_bit(subject, x))
					{
						//Get the current pixel we're working on.
						sf::Color curr_pixel = image.getPixel(pos_x+x, pos_y+y);

						//If the pixels are overwritten, set the flag register to one.
						if(registers[0xF] == 0 && curr_pixel.a == 255)
							registers[0xF] = 1;

						//Set all the pixels to max
						// to get a white color
						pixels[((y*8+x)*1)]= 255;
						pixels[((y*8+x)*2)]= 255;
						pixels[((y*8+x)*3)]= 255;
						pixels[((y*8+x)*4)]= 255;
					}
				}
			}
			render_target.update(pixels,pos_x,pos_y,8,height);
			window.draw(screen);
			window.display();
			return error_code::NONE;
		}

		//0xEX9E
		//Skips the next instruction if
		//the key stored in registers[X]
		//is pressed
		if(get_4bit(opcode,0) == 0xE && get_byte(opcode,1) == 0x9E)
		{
			if(sf::Keyboard::isKeyPressed(translate_key(get_4bit(opcode,1))))
				cursor_pos++;
			return error_code::NONE;
		}

		//0xEXA1
		//Skipe the next instruction if
		//the key stored in registers[X]
		//isn't pressed
		if(get_4bit(opcode,0) == 0xE && get_byte(opcode,1) == 0xA1)
		{
			if(!sf::Keyboard::isKeyPressed(translate_key(get_4bit(opcode,1))))
				cursor_pos++;
			return error_code::NONE;
		}


		//All instructions starting 
		// with 0xF
		if(get_4bit(opcode,0) == 0xF)
		{
			//Get the argument in the opcode
			//All 0xF opcodes are in the
			//0xFX.. format, with the two
			//last .. being the instruction
			byte arg_x = get_4bit(opcode, 1);
			byte instruction = get_byte(opcode, 1);

			switch(instruction)
			{
			case 0x07:
				//Sets registers[X] to
				// the delay timer
				registers[arg_x] = timer_delay;
				return error_code::NONE;
			case 0x0A:
				//Waits for a keypress, then
				// stores it in registers[X]
				sf::Event event;
				do	
					window.waitEvent(event); 
				while(event.type != sf::Event::KeyPressed && translate_key(event.key.code) != 0);
				registers[arg_x] = event.key.code;
				return error_code::NONE;
			case 0x15:
				//Sets the delay timer to
				// registers[X]
				timer_delay = registers[arg_x];
				return error_code::NONE;
			case 0x18:
				//Sets the sound timer to
				// registers[X]
				timer_sound = registers[arg_x];
				return error_code::NONE;
			case 0x1E:
				//Adds registers[X]
				// to address_register
				address_register += registers[arg_x];
				return error_code::NONE;
			case 0x29:
				//Sets the address register to the
				// location of the sprite for the
				// character stored in registers[X]
				address_register = font_pos+5*registers[arg_x];
				return error_code::NONE;
			case 0x33:
				//Stores the BCD of registers[X]
				// starting at address_register
				// and ending at address_register+3
				access_memory(address_register  ) = (registers[arg_x]/100)&9;
				access_memory(address_register+1) = (registers[arg_x]/10 )&9;
				access_memory(address_register+2) = (registers[arg_x]    )&9;
				return error_code::NONE;
			case 0x55:
				//Stores all the registers 
				// from 0 to X in RAM, starting
				// at address_register
				for(int i = 0; i < arg_x; ++i)
					access_memory(address_register+i) = registers[i];
				return error_code::NONE;
			case 0x65:
				//Grabs registers 0 to X 
				// from RAM, starting at 
				// address_register.
				for(int i = 0; i < arg_x; ++i)
					registers[i] = access_memory(address_register+i);
				return error_code::NONE;
			default:
				break;
			}
		}



		return error_code::INSTRUCTION_NOT_FOUND;
	}

public:

	error_code run(const std::string &f_)
	{
		//Set up rendering stuff
		screen.setTexture(render_target);
		//64x32 is the screen size of a Chip-8
		render_target.create(64,32);

		window.clear(sf::Color::Black);


		//Load the file, return error if any
		if(!load_program(f_))
			return error_code::LOADING_FILE;

		//Set the window title
		window.setTitle("Chipulator: " + filename);

		//Seed rand, for random number generation
		srand(time(NULL));

		//Load font
		//This is done by the Chip-8
		// emulator, and the fonts are hardcoded
		load_font();


		cursor_pos = 0;
		//Commence loop
		while(cursor_pos < static_cast<int>(program.size()))
		{
			//Run the desired opcode and increment cursor
			error_code execution_return = execute_opcode(program[cursor_pos++]);
			if(execution_return != error_code::NONE)
			{
				std::cout<<"Error! Code: "
				         <<std::hex<<std::uppercase<<execution_return
				         <<std::nouppercase<<", Instruction: "
				         <<std::uppercase<<program[cursor_pos-1]<<"\n";
				;
				return execution_return;
			}
			sf::sleep(sf::milliseconds(16));
		}


		//We made it through! Return no error.
		return error_code::NONE;
	}

	Chip8(sf::RenderWindow &w) : window(w) {}
};


int main()
{
	sf::RenderWindow window{{800,600}, "Chipulator!"};
	Chip8 emulator(window);
	int emulator_status = emulator.run("roms/TETRIS");
	if(emulator_status != Chip8::error_code::NONE)
		return emulator_status;
}