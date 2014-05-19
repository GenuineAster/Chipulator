#include <stack>
#include <string>
#include <fstream>

using byte = uint8_t;
using word = uint16_t;
using dword = uint32_t;


class Chipulator
{
private:
	byte memory[0x1000];
	struct 
	{
		byte V[16];
		word I;
	} regs;

	word ip, pend;
	std::stack<word> cs;

	struct 
	{
		byte delay;
		byte sound;
	} timers;

	bool display[32][64];
	bool keys[0xF];
	bool should_draw;
	word font_pos;

	void load_font();

public:
	enum Exception
	{
		WaitKey
	};

	bool (&get_display())[32][64];
	void key_pressed(byte key);
	void key_released(byte key);
	void load_program(std::string fname);
	void run_opcode();
	bool running();
	void decrement_timers();
	bool get_draw();

	Chipulator();
};