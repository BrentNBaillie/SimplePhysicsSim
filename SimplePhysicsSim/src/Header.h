#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

extern int ballCount;
extern float ballRadius;
extern float screenWidth;
extern float screenHeight;
extern float diameter;

class Ball
{
	public:
		sf::CircleShape circle;
		sf::Color ballColor;
		int
			ID;
		float
			Vy, Vx,
			y, x, velocity, colorBrightness;
		Ball(float xInitial, float yInitial, float xiVelocity, float yiVelocity, int ID);
		~Ball();
		void draw(sf::RenderWindow& window);
		void Set_Position();
		void ChangeColor();
};

class Physics
{
private:
	float dX, dY;
	bool ballOneFacing, ballTwoFacing;

public:
	float
		xGravity, yGravity, collCoefficient, step, distance, dragCoefficient, Unit[4], 
		Vel[4], BallOneVel[4], BallTwoVel[4], maxVel, Overlap[3], addVel, addVelFactor[4], velScale;

		Physics();
		~Physics();
		void UpdatePosition(Ball &ball);
		void WallCollision(Ball& ball);
		void BallCollide(Ball& ballOne, Ball& ballTwo, float dist, float deltaX, float deltaY);
		void UnitVectors();
		void VelocityVectors(Ball ball);
};

float Magnitude(float x, float y);

