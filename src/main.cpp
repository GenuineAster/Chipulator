/*
Chipulator, a Chip-8 emulator written in C++ with SFML, by Mischa 'Aster' Alff.
*/

#include <Chipulator/Chipulator.hpp>

int main(int argc, char** argv)
{
	Chipulator chip8;
	if(argc > 0)
		chip8.load_program({argv[1]});
	while(chip8.running())
	{
		try
		{
			chip8.run_opcode();
		}
		catch(Chipulator::Exception e)
		{
			switch(e)
			{
				case Chipulator::WaitKey:
					break;
			}
		}
	}
}



