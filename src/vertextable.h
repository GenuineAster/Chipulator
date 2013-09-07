#ifndef VERTEXTABLE_H
#define VERTEXTABLE_H
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <array>

class vertexTable
{
public:
    typedef std::array<std::array<bool, 32>, 64> boolTable;

    vertexTable(boolTable &theDisplay);

    sf::Sprite& getSprite();
    void update();

private:
    std::array<sf::Vertex, 2048> pixels; //size is 32*64, technically it should be obtained from display
    boolTable& display;
    sf::RenderTexture window;
    sf::Sprite sprite;
    int width;
    int height;
};

#endif // VERTEXTABLE_H
