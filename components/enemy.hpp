#ifndef enemy_hpp
#define enemy_hpp

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

class Enemy : public RectangleShape
{
private:
  double size = 56;
  int heal = 10;

public:
  Enemy()
  {
    srand(time(NULL));
    this->heal = ((rand() % 5) + 1) * 5;
    this->setFillColor(Color::Blue);
    this->setSize(Vector2f(this->size, this->size));
  }

  void drawHeal(RenderWindow &window)
  {
    auto pos = this->getPosition();

    Font font{};
    font.loadFromFile("assets/fonts/Poppins-Regular.ttf");

    Text text{to_string(this->heal), font, 24};

    double textW = text.getGlobalBounds().width;
    double textH = text.getGlobalBounds().height;

    text.setFillColor(Color::White);
    text.setPosition(Vector2f(pos.x + (this->size - textW) / 2, pos.y + (this->size - textH) / 2 - 6));
    text.setStyle(Text::Bold);

    window.draw(text);
  }

  double getSize() { return this->size; }

  void reduce() { this->heal--; }

  int getHeal() { return this->heal; }
};

#endif