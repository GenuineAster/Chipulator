/*
Chipulator, a Chip-8 emulator written in C++ with SFML, by Mischa 'Aster' Alff.
*/
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <Chipulator/Chipulator.hpp>

int translate_key(sf::Keyboard::Key key)
{
	switch(key)
	{
		case sf::Keyboard::Numpad0:
			return 0x0;
		case sf::Keyboard::Numpad1:
			return 0x7;
		case sf::Keyboard::Numpad2:
			return 0x8;
		case sf::Keyboard::Numpad3:
			return 0x9;
		case sf::Keyboard::Numpad4:
			return 0x4;
		case sf::Keyboard::Numpad5:
			return 0x5;
		case sf::Keyboard::Numpad6:
			return 0x6;
		case sf::Keyboard::Numpad7:
			return 0x1;
		case sf::Keyboard::Numpad8:
			return 0x2;
		case sf::Keyboard::Numpad9:
			return 0x3;
		case sf::Keyboard::Num1:
			return 0xA;
		case sf::Keyboard::Num2:
			return 0xB;
		case sf::Keyboard::Num3:
			return 0xC;
		case sf::Keyboard::Num4:
			return 0xD;
		case sf::Keyboard::Num5:
			return 0xE;
		case sf::Keyboard::Num6:
			return 0xF;
	}
	return 0x0;
}

void handle_events(sf::Event &event, Chipulator &chip8, sf::RenderWindow &window)
{
	switch(event.type)
	{
		case sf::Event::KeyPressed:
			chip8.key_pressed(translate_key(event.key.code));
			//std::cout<<"Key pressed: "<<std::hex<<translate_key(event.key.code)<<std::endl;
			break;
		case sf::Event::KeyReleased:
			chip8.key_released(translate_key(event.key.code));
			//std::cout<<"Key released: "<<std::hex<<translate_key(event.key.code)<<std::endl;
			break;
		case sf::Event::Closed:
			//window.close();
			break;
		default:
			break;
	}
	return;
}

int main(int argc, char** argv)
{
	if(argc < 2)
		return -1;

	std::string name{argv[1]};
	name = "Chipulator :: "+name;

	sf::RenderWindow window{{640, 320}, name};
	Chipulator chip8;
		chip8.load_program({argv[1]});

	sf::RectangleShape pixel({10.f,10.f});
	pixel.setFillColor(sf::Color::White);

	auto timer_period = 1e6f/60.f;
	sf::Clock timer, total;
	timer.restart();
	total.restart();

	window.clear(sf::Color::Black);

	while(window.isOpen() && chip8.running())
	{
		sf::Event event;
		while(window.pollEvent(event))
			handle_events(event, chip8, window);

		if(timer.getElapsedTime().asMicroseconds() > timer_period)
		{
			timer.restart();
			chip8.decrement_timers();
		}

		try
		{
			chip8.run_opcode();
		}
		catch(Chipulator::Exception e)
		{
			switch(e)
			{
				case Chipulator::WaitKey:
				{
					do
					{
						window.waitEvent(event);
						handle_events(event, chip8, window);
					}
					while(event.type != sf::Event::KeyPressed);
					break;
				}
			}
		}
		if(chip8.get_draw())
		{
			window.clear(sf::Color::Black);
			auto display = chip8.get_display();
			for(auto y=0;y<32;++y)
			{
				for(auto x=0;x<64;++x)
				{
					if(display[y][x])
						pixel.setFillColor(sf::Color::White);
					else
						pixel.setFillColor(sf::Color::Black);				
					pixel.setPosition(x*10.f, y*10.f);
					window.draw(pixel);
				}
			}
			window.display();
		}
		//std::cout<<std::dec<<total.getElapsedTime().asMicroseconds()<<std::endl;
		//sf::sleep(sf::microseconds(1000));
	}

	return 0;
}



