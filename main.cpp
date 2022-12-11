#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>

#include "components/enemy.hpp"

using namespace sf;
using namespace std;

int main()
{
  srand(time(NULL));

  RenderWindow window(VideoMode(1280, 720), "SFML | Starter Template", Style::Close);
  window.setFramerateLimit(60);

render_game:
  double rectSize = 56;
  RectangleShape rect{};
  rect.setFillColor(Color::Red);
  rect.setSize(Vector2f(rectSize, rectSize));

  rect.setPosition(Vector2f(
      rectSize / 2, window.getView().getCenter().y - rectSize / 2));

  double rectVelocity = 5;
  double circleVelocity = 5;
  double enemyVelocity = 2;

  double enemySpawnSpeed = 1000;

  vector<CircleShape> vectorCicle{};
  vector<Enemy> vectorEnemy{};

  Clock clock{}, clockEnemy{}, clockFps{};
  double timeInterval, timeIntervalEnemy, timeIntervalFps;
  double circleDelay = 100;

  string gameState = "play";
  int point = 0;

  chrono::high_resolution_clock::time_point start;
  chrono::high_resolution_clock::time_point end;
  float fps;

  Font font{};
  font.loadFromFile("assets/fonts/Poppins-Regular.ttf");

  while (window.isOpen())
  {
    timeInterval = clock.getElapsedTime().asMilliseconds();
    timeIntervalEnemy = clockEnemy.getElapsedTime().asMilliseconds();
    timeIntervalFps = clockFps.getElapsedTime().asSeconds();

    Event event{};
    while (window.pollEvent(event))
    {
      if (event.type == Event::Closed)
        window.close();

      if (gameState == "over")
      {
        if (event.type == Event::KeyPressed)
          if (event.key.code == Keyboard::Enter)
            // restart game
            goto render_game;
          else if (event.key.code == Keyboard::Escape)
            window.close();
      }
      else if (gameState == "play")
      {
        if (event.key.code == Keyboard::Escape)
          gameState = "pause";
      }
      else if (gameState == "pause")
      {
        if (event.type == Event::KeyPressed)
          if (event.key.code == Keyboard::Enter)
            gameState = "play";
          else if (event.key.code == Keyboard::Backspace)
            goto render_game;
          else if (event.key.code == Keyboard::Escape)
            window.close();
      }
    }

    start = std::chrono::high_resolution_clock::now();
    if (gameState == "play")
    {

      // handle rect move
      if (Keyboard::isKeyPressed(Keyboard::Up))
      {
        if (rect.getPosition().y > 0)
          rect.setPosition(Vector2f(rect.getPosition().x, rect.getPosition().y - rectVelocity));
      }
      if (Keyboard::isKeyPressed(Keyboard::Down))
      {
        if (rect.getPosition().y < window.getSize().y - rectSize)
          rect.setPosition(Vector2f(rect.getPosition().x, rect.getPosition().y + rectVelocity));
      }
      if (Keyboard::isKeyPressed(Keyboard::Left))
      {
        if (rect.getPosition().x > 0)
          rect.setPosition(Vector2f(rect.getPosition().x - rectVelocity, rect.getPosition().y));
      }
      if (Keyboard::isKeyPressed(Keyboard::Right))
      {
        if (rect.getPosition().x < window.getSize().x - rectSize)
          rect.setPosition(Vector2f(rect.getPosition().x + rectVelocity, rect.getPosition().y));
      }

      if (Keyboard::isKeyPressed(Keyboard::Space))
      {
        double circleRadius = 12;
        CircleShape circle{};
        circle.setRadius(circleRadius);
        circle.setFillColor(Color::Black);
        circle.setPosition(Vector2f(
            rect.getPosition().x + ((rectSize - (circleRadius * 2)) / 2), rect.getPosition().y + (rectSize - (circleRadius * 2)) / 2));

        if (timeInterval >= circleDelay)
        {
          vectorCicle.push_back(circle);
          clock.restart();
        }
      }
    }

    window.clear(Color::White);

    if (gameState == "play")
    {
      // text point
      Text textPoint{"Point: " + to_string(point), font, 24};
      textPoint.setCharacterSize(32);
      textPoint.setStyle(Text::Bold);
      textPoint.setFillColor(Color(0, 0, 0, 150));
      textPoint.setPosition(Vector2f(window.getView().getCenter().x - textPoint.getGlobalBounds().width / 2, 16));
      window.draw(textPoint);

      for (int a = 0; a < vectorCicle.size(); a++)
      {
        // draw circle
        window.draw(vectorCicle.at(a));

        // move circle
        Vector2f currPos = vectorCicle.at(a).getPosition();
        vectorCicle.at(a).setPosition(
            Vector2f(currPos.x + circleVelocity, currPos.y));

        // delete overflow circle
        double windowH = window.getSize().y;
        double windowW = window.getSize().x;
        if (currPos.x < 0 || currPos.x > windowW || currPos.y < 0 || currPos.y > windowH)
          vectorCicle.erase(vectorCicle.begin() + a);
      }

      window.draw(rect);

      // spawn enemy
      if (timeIntervalEnemy >= enemySpawnSpeed)
      {
        Enemy newEnemy{};
        double maxRandY = window.getSize().y - newEnemy.getSize();
        int randY = rand() % int(maxRandY);
        newEnemy.setPosition(
            Vector2f(window.getSize().x, randY));

        if (vectorEnemy.size() <= 1)
          vectorEnemy.push_back(newEnemy);
        clockEnemy.restart();
      }

      for (int a = 0; a < vectorEnemy.size(); a++)
      {
        // draw enemy
        window.draw(vectorEnemy.at(a));
        vectorEnemy.at(a).drawHeal(window);

        auto enemy = vectorEnemy.at(a);
        auto currPos = enemy.getPosition();

        // move enemy
        vectorEnemy.at(a).setPosition(
            Vector2f(currPos.x - enemyVelocity, currPos.y));

        // remove overflow enemy
        if (currPos.x < -1 * enemy.getSize())
          vectorEnemy.erase(vectorEnemy.begin() + a);
        if (currPos.x <= 0)
          gameState = "over";
      }

      // check enemy and rect
      for (int a = 0; a < vectorEnemy.size(); a++)
      {
        auto enemy = vectorEnemy.at(a);
        auto currPos = enemy.getPosition();

        if (enemy.getGlobalBounds().intersects(rect.getGlobalBounds()))
        {
          gameState = "over";
        }
      }

      // check enemy and circle
      for (int a = 0; a < vectorEnemy.size(); a++)
      {
        auto enemy = vectorEnemy.at(a);
        if (enemy.getHeal() <= 0)
        {
          vectorEnemy.erase(vectorEnemy.begin() + a);
          point++;
        }
        else
        {
          for (int b = 0; b < vectorCicle.size(); b++)
          {
            auto circle = vectorCicle.at(b);
            if (enemy.getGlobalBounds().intersects(circle.getGlobalBounds()))
            {
              vectorEnemy.at(a).reduce();
              vectorCicle.erase(vectorCicle.begin() + b);
            }
          }
        }
      }
    }
    else if (gameState == "over")
    {
      // game over
      Text text{"Game Over!", font, 72}, textCaption{"Total point : " + to_string(point) + "\n\n[ENTER] restart the game\n[ESC] close game", font, 16};
      textCaption.setFillColor(Color(0, 0, 0, 150));
      text.setFillColor(Color::Black);
      text.setStyle(Text::Bold);

      double windowCX = window.getView().getCenter().x;
      double windowCY = window.getView().getCenter().y;

      text.setPosition(
          Vector2f(windowCX - text.getGlobalBounds().width / 2, windowCY - text.getGlobalBounds().height / 2 - 36));

      textCaption.setPosition(Vector2f(
          windowCX - textCaption.getGlobalBounds().width / 2, window.getSize().y - textCaption.getGlobalBounds().height * 2));

      window.draw(text);
      window.draw(textCaption);
    }
    else
    {
      // pause
      Text text{"Paused!", font, 72}, textCaption{"[ENTER] resume game\n[Backspace] restart game\n[ESC] exit game", font, 16};
      textCaption.setFillColor(Color(0, 0, 0, 150));
      text.setFillColor(Color::Black);
      text.setStyle(Text::Bold);

      double windowCX = window.getView().getCenter().x;
      double windowCY = window.getView().getCenter().y;

      text.setPosition(
          Vector2f(windowCX - text.getGlobalBounds().width / 2, windowCY - text.getGlobalBounds().height / 2 - 36));

      textCaption.setPosition(Vector2f(
          windowCX - textCaption.getGlobalBounds().width / 2, window.getSize().y - textCaption.getGlobalBounds().height * 2));

      window.draw(text);
      window.draw(textCaption);
    }

    end = std::chrono::high_resolution_clock::now();

    if (timeIntervalFps >= 1)
    {
      fps = (float)1e9 / (float)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

      clockFps.restart();
    }

    Text textFps{"FPS: " + to_string(int(fps)), font, 16};
    textFps.setFillColor(Color(0, 0, 0, 150));
    textFps.setPosition(Vector2f(16, 16));
    window.draw(textFps);

    window.display();
  }
  return 0;
}