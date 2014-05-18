#include <ctime>
#include <cstring>
#include <Chipulator/Chipulator.hpp>

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
		;

	int i=0x200;
	ip = i;

	while(program_file.good())
	{
		//Read file into memory
		byte &curr_byte = memory[i];
		curr_byte = program_file.get();
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
	word opcode = memory[ip]<<8 | memory[ip+1];
	ip+=2;
	switch(opcode>>12)
	{
		case 0x0:
		{
			switch(opcode)
			{
				case 0x00E0:
				{
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
			return;
		}
		case 0x1:
		{
			ip = opcode & 0xFFF;
			return;
		}
		case 0x2:
		{
			cs.push(ip);
			ip = opcode & 0xFFF;
			return;
		}
		case 0x3:
		{
			if(regs.V[(opcode>>8)&0xF] == opcode & 0xFF)
				ip+=2;
			return;
		}
		case 0x4:
		{
			if(regs.V[(opcode>>8)&0xF] != opcode & 0xFF)
				ip+=2;
			return;
		}
		case 0x5:
		{
			if(regs.V[(opcode>>8)&0xF] == regs.V[(opcode>>4)&0xF])
				ip+=2;
			return;
		}
		case 0x6:
		{
			regs.V[(opcode>>8)&0xF] = opcode&0xFF;
			return;
		}
		case 0x7:
		{
			regs.V[(opcode>>8)&0xF] += opcode&0xFF;
			return;
		}
		case 0x8:
		{
			switch(opcode&0xF)
			{
				case 0x0:
				{
					regs.V[(opcode>>8)&0xF] = regs.V[(opcode>>4)&0xF];
					break;
				}
				case 0x1:
				{
					regs.V[(opcode>>8)&0xF] = regs.V[(opcode>>8)&0xF] | regs.V[(opcode>>4)&0xF];
					break;
				}
				case 0x2:
				{
					regs.V[(opcode>>8)&0xF] = regs.V[(opcode>>8)&0xF] & regs.V[(opcode>>4)&0xF];
					break;
				}
				case 0x3:
				{
					regs.V[(opcode>>8)&0xF] = regs.V[(opcode>>8)&0xF] ^ regs.V[(opcode>>4)&0xF];
					break;
				}
				case 0x4:
				{
					if((uint)regs.V[(opcode>>8)&0xF] + (uint)regs.V[(opcode>>4)&0xF] > 0xFF)
						regs.V[0xF] = true;
					regs.V[(opcode>>8)&0xF] = regs.V[(opcode>>8)&0xF] + regs.V[(opcode>>4)&0xF];
					break;
				}
				case 0x5:
				{
					if((int)regs.V[(opcode>>8)&0xF] - (int)regs.V[(opcode>>4)&0xF] < 0)
						regs.V[0xF] = true;
					regs.V[(opcode>>8)&0xF] = regs.V[(opcode>>8)&0xF] - regs.V[(opcode>>4)&0xF];
					break;
				}
				case 0x6:
				{
					regs.V[0xF] = regs.V[(opcode>>8)&0xF]&1;
					regs.V[(opcode>>8)&0xF] = regs.V[(opcode>>8)&0xF]>>1;
					break;
				}
				case 0x7:
				{
					if((int)regs.V[(opcode>>4)&0xF] - (int)regs.V[(opcode>>8)&0xF] < 0)
						regs.V[0xF] = true;
					regs.V[(opcode>>8)&0xF] = regs.V[(opcode>>4)&0xF] - regs.V[(opcode>>8)&0xF];	
					break;
				}
				case 0xE:
				{
					regs.V[0xF] = (regs.V[(opcode>>8)&0xF]>>7)&1;
					regs.V[(opcode>>8)&0xF] = regs.V[(opcode>>8)&0xF]<<1;
					break;
				}
			}
			return;
		}
		case 0x9:
		{
			if(regs.V[(opcode>>8)&0xF] != regs.V[(opcode>>4)&0xF])
				ip += 2;
			return;
		}
		case 0xA:
		{
			regs.I = opcode&0xFFF;
			return;
		}
		case 0xB:
		{
			ip = opcode&0xFFF + regs.V[0];
			return;
		}
		case 0xC:
		{
			regs.V[(opcode>>8)&0xF] = rand() & (opcode&0xFF);
			return;
		}
		case 0xD:
		{
			auto x = regs.V[(opcode>>8)&0xF];
			auto y = regs.V[(opcode>>4)&0xF];
			auto height = opcode & 0xF;
			for(auto i = 0; i < height; ++i)
			{
				byte spr = memory[regs.I+height];
				for(auto j = 0; j < 8; ++j)
				{
					auto val = (spr>>j)&1;
					bool *pixel = &display[y+height][x+j];
					if(val)
						if(*pixel)
							*pixel = 0;
					*pixel = val;
				}
			}
			return;

		}
		case 0xE:
		{
			switch(opcode&0xFF)
			{
				case 0x9E:
					if(keys[regs.V[(opcode>>8)&0xF]])
						ip+=2;
					break;
				case 0xA1:
					if(!keys[regs.V[(opcode>>8)&0xF]])
						ip+=2;
					break;
			}
			return;
		}
		case 0xF:
		{
			switch(opcode & 0xFF)
			{
				case 0x07:
				{
					regs.V[(opcode>>8)&0xF] = timers.delay;
					break;
				}
				case 0x0A:
				{
					throw Exception::WaitKey;
					break;
				}
				case 0x15:
				{
					timers.delay = regs.V[(opcode>>8)&0xF];
					break;
				}
				case 0x18:
				{
					timers.sound = regs.V[(opcode>>8)&0xF];
					break;
				}
				case 0x1E:
				{
					regs.I += regs.V[(opcode>>8)&0xF];
					break;
				}
				case 0x29:
				{
					regs.I = 0x0+0x5*regs.V[(opcode>>8)&0xF];
					break;
				}
				case 0x33:
				{
					auto val = regs.V[(opcode>>8)&0xF];
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
			return;
		}
		default:
			return;
	}
}

Chipulator::Chipulator()
{
	srand(time(NULL));
	load_font();
	for(auto y=0;y<32;++y)
		for(auto x=0;x<64;++x)
			display[y][x] = false;
	for(auto i=0;i<0xF;++i)
		keys[i]=false;
	ip=0;
	timers.delay = 0;
	timers.sound = 0;	
}