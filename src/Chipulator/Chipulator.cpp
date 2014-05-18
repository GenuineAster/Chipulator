#include <Chipulator/Chipulator.hpp>

void Chipulator::load_program(std::string fname)
{
	std::ifstream program_file(fname, std::ios::binary | std::ios::in);
	if(!program_file.is_open())
		return false;

	int i=0;

	while(program_file.good())
	{
		//Read file into memory
		byte &curr_byte = memory[memory_start+i];
		curr_byte = program_file.get();
		std::cout<<std::hex<<std::uppercase<<(int)curr_byte;
		if(i%2 == 1) std::cout<<" ";
		if(i%16 == 15) std::cout<<"\n";
		i++;
	}
	std::cout<<"\n";
	program_size = i;
}