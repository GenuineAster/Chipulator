#include <ctime>
#include <cstring>
#include <iostream>
#include <Chipulator/Chipulator.hpp>

bool Chipulator::get_draw()
{
	return should_draw;
}

bool Chipulator::running()
{
	return ip != pend;
}

bool (&Chipulator::get_display())[32][64]
{
	return display;
}

void Chipulator::decrement_timers()
{
	if(timers.sound > 0)
		--timers.sound;
	if(timers.delay > 0)
		--timers.delay;
}

void Chipulator::load_program(std::string fname)
{
	std::ifstream program_file(fname, std::ios::binary | std::ios::in);
	if(!program_file.is_open())
		exit(-1);

	int i=0x200;
	ip = i;

	while(program_file.good())
	{
		//Read file into memory
		memory[i] = program_file.get();
		++i;
	}
	pend = i;
}

void Chipulator::load_font()
{
	byte font[] =
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

	auto len = std::strlen((char*)font);
	for(auto i = 0; i < len; ++i)
	{
		memory[i] = font[i];
	}
	font_pos = 0;
}

void Chipulator::key_pressed(byte key)
{
	keys[key] = true;
}

void Chipulator::key_released(byte key)
{
	keys[key] = false;
}

void Chipulator::run_opcode()
{
	should_draw = false;

	word opcode = memory[ip]<<8 | memory[ip+1];
	auto VX  = (opcode>>8)&0xF;
	auto VY  = (opcode>>4)&0xF;
	auto N   = opcode & 0xF;
	auto NN  = opcode & 0xFF;
	auto NNN = opcode & 0xFFF;

	std::cout<<"IP: "<<std::hex<<ip<<std::dec<<std::endl;
	std::cout<<"Opcode: "<<std::hex<<opcode<<std::dec<<std::endl;
	std::cout<<"Timers: D"<<(int)timers.delay<<", S"<<(int)timers.sound<<std::endl;
	std::cout<<"Registers:\n"<<std::hex;
	for(auto i=0;i<0xF;++i)
		std::cout<<"\t"<<(int)regs.V[i]<<"\n";
	std::cout<<"\t"<<(int)regs.I<<"\n";
	std::cout<<"VX  : "<<VX  <<std::endl;
	std::cout<<"VY  : "<<VY  <<std::endl;
	std::cout<<"N   : "<<N   <<std::endl;
	std::cout<<"NN  : "<<NN  <<std::endl;
	std::cout<<"NNN : "<<NNN <<std::endl;
	std::cout<<std::dec;
	std::cout<<"\n";

	switch(opcode>>12)
	{
		case 0x0:
		{
			switch(opcode)
			{
				case 0x00E0:
				{
					should_draw = true;
					for(auto y=0;y<32;++y)
						for(auto x=0;x<64;++x)
							display[y][x] = false;
					break;
				}
				case 0x00EE:
				{
					ip = cs.top();
					cs.pop();
					break;
				}
				default:
					break;//TBI

			}
			break;
		}
		case 0x1:
		{
			ip = NNN;
			ip -= 2;
			break;
		}
		case 0x2:
		{
			cs.push(ip);
			ip = NNN;
			ip -= 2;
			break;
		}
		case 0x3:
		{
			if(regs.V[VX] == NN)
				ip+=2;
			break;
		}
		case 0x4:
		{
			if(regs.V[VX] != NN)
				ip+=2;
			break;
		}
		case 0x5:
		{
			if(regs.V[VX] == regs.V[VY])
				ip+=2;
			break;
		}
		case 0x6:
		{
			regs.V[VX] = NN;
			break;
		}
		case 0x7:
		{
			regs.V[VX] += NN;
			break;
		}
		case 0x8:
		{
			switch(opcode&0xF)
			{
				case 0x0:
				{
					regs.V[VX] = regs.V[VY];
					break;
				}
				case 0x1:
				{
					regs.V[VX] = regs.V[VX] | regs.V[VY];
					break;
				}
				case 0x2:
				{
					regs.V[VX] = regs.V[VX] & regs.V[VY];
					break;
				}
				case 0x3:
				{
					regs.V[VX] = regs.V[VX] ^ regs.V[VY];
					break;
				}
				case 0x4:
				{
					if((uint)regs.V[VX] + (uint)regs.V[VY] > 0xFF)
						regs.V[0xF] = true;
					regs.V[VX] = regs.V[VX] + regs.V[VY];
					break;
				}
				case 0x5:
				{
					if((int)regs.V[VX] - (int)regs.V[VY] < 0)
						regs.V[0xF] = true;
					regs.V[VX] = regs.V[VX] - regs.V[VY];
					break;
				}
				case 0x6:
				{
					regs.V[0xF] = regs.V[VX]&1;
					regs.V[VX] = regs.V[VX]>>1;
					break;
				}
				case 0x7:
				{
					if((int)regs.V[VY] - (int)regs.V[VX] < 0)
						regs.V[0xF] = true;
					regs.V[VX] = regs.V[VY] - regs.V[VX];	
					break;
				}
				case 0xE:
				{
					regs.V[0xF] = (regs.V[VX]>>7)&1;
					regs.V[VX] = regs.V[VX]<<1;
					break;
				}
			}
			break;
		}
		case 0x9:
		{
			if(regs.V[VX] != regs.V[VY])
				ip += 2;
			break;
		}
		case 0xA:
		{
			regs.I = NNN;
			break;
		}
		case 0xB:
		{
			ip = NNN + regs.V[0];
			ip -= 2;
			break;
		}
		case 0xC:
		{
			regs.V[VX] = rand() & (NN);
			break;
		}
		case 0xD:
		{
			regs.V[0xF] = 0;
			auto x = regs.V[VX];
			auto y = regs.V[VY];
			auto height = N;
			auto row = 0;

			while(row<height)
			{
				auto crow = memory[row+regs.I];
				auto pixel_offset = 0;
				while(pixel_offset < 8)
				{
					bool &pixel = display[y+row][x+pixel_offset];
					pixel_offset++;
					if(y+row >= 32 || x+pixel_offset-1 >= 64)
						continue;
					auto mask = 1 << (8-pixel_offset);
					bool curr_pixel = (crow & mask) >> (8-pixel_offset);
					pixel ^= curr_pixel;
					if(pixel==0)
						regs.V[0xF] = 1;
					else
						regs.V[0xF] = 0;
				}
				row++;
			}
			should_draw = true;
			break;
		}
		case 0xE:
		{
			switch(NN)
			{
				case 0x9E:
					if(keys[regs.V[VX]])
						ip+=2;
					break;
				case 0xA1:
					if(!keys[regs.V[VX]])
						ip+=2;
					break;
			}
			break;
		}
		case 0xF:
		{
			switch(NN)
			{
				case 0x07:
				{
					regs.V[VX] = timers.delay;
					break;
				}
				case 0x0A:
				{
					throw Exception::WaitKey;
					break;
				}
				case 0x15:
				{
					timers.delay = regs.V[VX];
					break;
				}
				case 0x18:
				{
					timers.sound = regs.V[VX];
					break;
				}
				case 0x1E:
				{
					regs.I += regs.V[VX];
					break;
				}
				case 0x29:
				{
					regs.I = (font_pos+(0x5*regs.V[VX]))&0xFFF;
					break;
				}
				case 0x33:
				{
					auto val = regs.V[VX];
					memory[regs.I]   = val %= 10;
					memory[regs.I+1] = val %= 10;
					memory[regs.I+2] = val %= 10;
					break;
				}
				case 0x55:
				{
					for(auto i=0;i<0xF;++i)
						memory[regs.I+i] = regs.V[i];
					break;
				}
				case 0x65:
				{
					for(auto i=0;i<0xF;++i)
						regs.V[i] = memory[regs.I+i];
					break;
				}
			}
			break;
		}
		default:
			break;
	}
	ip+=2;
}

Chipulator::Chipulator()
{
	srand(time(NULL));
	for(auto y=0;y<32;++y)
		for(auto x=0;x<64;++x)
			display[y][x] = false;
	for(auto i=0;i<0xF;++i)
		keys[i]=false;
	ip=0x200;
	pend=0x0;
	timers.delay = 0;
	timers.sound = 0;
	should_draw = true;
	font_pos=0;
	load_font();
}