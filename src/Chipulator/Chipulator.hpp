#include <stack>
#include <string>
#include <ifstream>

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

	std::stack<word> cs;

	struct 
	{
		byte delay;
		byte sound;
	} timers;

	bool display[32][64];

public:
	void load_program(std::string fname);
};