#include "vertextable.h"

vertexTable::vertexTable(boolTable& theDisplay):
    display(theDisplay),
    width(display.size()),
    height(display[0].size())
{
    window.create(width,height);
    sprite.setTexture(window.getTexture());

    for (int i=0; i<width; ++i)
    {
        for (int j=0; j<height; ++j)
        {
            pixels[height*i+j].position = sf::Vector2f(i,j+1);
        }
    }
}

sf::Sprite &vertexTable::getSprite()
{
    return sprite;
}

void vertexTable::update()
{
    for(unsigned int x = 0; x < display.size(); ++x)
    {
        for(unsigned int y = 0; y < display[x].size(); ++y)
        {
            if (display[x][y])
                pixels[height*x+y].color = sf::Color::White;
            else pixels[height*x+y].color = sf::Color::Black;
        }
    }
    window.clear();
    window.draw(pixels.data(),pixels.size(),sf::Points);
    window.display();
}
