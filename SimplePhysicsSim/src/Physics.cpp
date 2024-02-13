#pragma once
#include <iostream>
#include "Header.h"
#include <cmath>
#include <vector>

enum NormalTangent { Xn, Yn, Xt, Yt };

Physics::Physics()
	:xGravity(0), yGravity(70), collCoefficient(0.9), step(0.01),
	distance(0), dragCoefficient(0.05), dX(0), dY(0), Unit{ 0 }, Vel{ 0 },
	BallOneVel{ 0 }, BallTwoVel{ 0 }, ballOneFacing(false), ballTwoFacing(false),
	maxVel(2), Overlap{ 0 }, addVel(0), addVelFactor{0}, velScale(8)
{
	//Distance intervals for collision math below
	Overlap[0] = 0.25 * diameter;
	Overlap[1] = 0.5 * diameter;
	Overlap[2] = 0.75 * diameter;

	//Velocity intervals that use Overlap when calculating collision effects
	addVelFactor[0] = velScale;
	addVelFactor[1] = velScale / 4;
	addVelFactor[2] = velScale / 9;
	addVelFactor[3] = velScale / 16;
}

Physics::~Physics() {}

//Updates balls new position and velocity
void Physics::UpdatePosition(Ball &ball)
{
	ball.x = ball.x + ball.Vx * step + (xGravity - ball.Vx * dragCoefficient) * step * step;
	ball.Vx = ball.Vx + (xGravity - ball.Vx * dragCoefficient) * step; 
	ball.y = ball.y + ball.Vy * step + (yGravity - ball.Vy * dragCoefficient) * step * step;
	ball.Vy = ball.Vy + (yGravity - ball.Vy * dragCoefficient) * step; 
}

//Handles boundry collisions
void Physics::WallCollision(Ball &ball)
{
	if (ball.x < ballRadius)
	{
		ball.Vx *= -collCoefficient;
		ball.x = ballRadius; 
	}
	else if (ball.x > screenWidth - ballRadius)
	{
		ball.Vx *= -collCoefficient;
		ball.x = screenWidth - ballRadius; 
	}
	if (ball.y > screenHeight - ballRadius)
	{
		ball.Vy *= -collCoefficient;
		ball.y = screenHeight - ballRadius; 
	}
	else if (ball.y < ballRadius)
	{
		ball.Vy *= -collCoefficient;
		ball.y = ballRadius; 
	}
}

//Handles ball on ball collisions
void Physics::BallCollide(Ball &ballOne, Ball &ballTwo, float dist, float deltaX, float deltaY)
{
	//Imports values from main
	distance = dist;
	dX = deltaX;
	dY = deltaY;
	//Calculates unit vectors
	UnitVectors();

	//Calculates new post collision velocities for both balls
	VelocityVectors(ballOne);
	BallOneVel[Xn] = Vel[Xn];
	BallOneVel[Yn] = Vel[Yn];
	BallOneVel[Xt] = Vel[Xt];
	BallOneVel[Yt] = Vel[Yt];
	
	VelocityVectors(ballTwo); 
	BallTwoVel[Xn] = Vel[Xn];
	BallTwoVel[Yn] = Vel[Yn];
	BallTwoVel[Xt] = Vel[Xt];
	BallTwoVel[Yt] = Vel[Yt];

	//Checks if either ball is moving towards the other
	ballOneFacing = BallOneVel[Xn] * dX + BallOneVel[Yn] * dY > 0;
	ballTwoFacing = BallTwoVel[Xn] * dX + BallTwoVel[Yn] * dY < 0;

	//Checks if the three situations for collision are met
	if ((ballOneFacing && ballTwoFacing) || 
		(ballOneFacing && (BallOneVel[Xn] > BallTwoVel[Xn] || BallOneVel[Yn] > BallTwoVel[Yn])) ||
		(ballTwoFacing && (BallTwoVel[Xn] > BallOneVel[Xn] || BallTwoVel[Yn] > BallOneVel[Yn])))
	{
		//Sets the post collision velocities
		ballOne.Vx = (collCoefficient * BallTwoVel[Xn]) + BallOneVel[Xt];
		ballOne.Vy = (collCoefficient * BallTwoVel[Yn]) + BallOneVel[Yt];
		ballTwo.Vx = (collCoefficient * BallOneVel[Xn]) + BallTwoVel[Xt];
		ballTwo.Vy = (collCoefficient * BallOneVel[Yn]) + BallTwoVel[Yt];

		//A method of two collision effects put together
		if (+ballOne.Vx > maxVel && +ballOne.Vy > maxVel && +ballTwo.Vx > maxVel && +ballTwo.Vy > maxVel)
		{
			//Forced displacement
			ballTwo.x = 2.01 * ballRadius * Unit[Xn] + ballOne.x;
			ballTwo.y = 2.01 * ballRadius * Unit[Yn] + ballOne.y;
		}
		else
		{
			//Increase in velocities based off overlap amount
			if (distance > Overlap[2])
			{
				addVel = addVelFactor[3];
			}
			else if (distance <= Overlap[2] && distance > Overlap[1])
			{
				addVel = addVelFactor[2];
			}
			else if (distance <= Overlap[1] && distance > Overlap[0])
			{
				addVel = addVelFactor[1];
			}
			else
			{
				addVel = addVelFactor[0];
			}

			ballTwo.Vx += addVel * Unit[Xn];
			ballTwo.Vy += addVel * Unit[Yn];
		}
	}
}

//Calculates the tangent and normal unit vectors between both balls
void Physics::UnitVectors()
{
	Unit[Xn] = dX / distance; 
	Unit[Yn] = dY / distance;
	Unit[Xt] = Unit[Yn];      
	Unit[Yt] = -Unit[Xn];
}

//Calculates a tangent and normal velocity vector that adds up to the balls main velocity vector
void Physics::VelocityVectors(Ball ball)
{
	Vel[Xn] = Unit[Xn] * (ball.Vx * Unit[Xn] + ball.Vy * Unit[Yn]);
	Vel[Yn] = Unit[Yn] * (ball.Vx * Unit[Xn] + ball.Vy * Unit[Yn]);
	Vel[Xt] = Unit[Xt] * (ball.Vx * Unit[Xt] + ball.Vy * Unit[Yt]);
	Vel[Yt] = Unit[Yt] * (ball.Vx * Unit[Xt] + ball.Vy * Unit[Yt]);
}