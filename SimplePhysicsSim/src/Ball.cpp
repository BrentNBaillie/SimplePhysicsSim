#pragma once
#include <iostream>
#include "Header.h"
#include <cmath>
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>

Ball::Ball(float xInitial, float yInitial, float xiVelocity,
    float yiVelocity, int ID)

    :x(xInitial), y(yInitial), Vy(-yiVelocity),
     Vx(xiVelocity), ID(ID), velocity(0), colorBrightness(0.8)
{
    circle.setRadius(ballRadius);
    circle.setOrigin(ballRadius, ballRadius);
    sf::Color ballColor(255, 0, 0);
    circle.setFillColor(ballColor);
}

Ball::~Ball(){}

void Ball::draw(sf::RenderWindow& window)
{
    window.draw(circle);
}

void Ball::Set_Position()
{
    circle.setPosition(x,y);
}

//Set ball color based off velocity
void Ball::ChangeColor()
{
    circle.setFillColor(ballColor);
    ballColor.b = colorBrightness * std::max(-0.102 * velocity * velocity + 255,0.0);
    ballColor.g = colorBrightness * std::max(-0.102 * (velocity - 50) * (velocity - 50) + 255,0.0);
    ballColor.r = colorBrightness * std::max(-0.102 * (velocity - 100) * (velocity - 100) + 255,0.0);
}





