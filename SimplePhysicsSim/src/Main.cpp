#pragma once
#include <iostream>
#include "Header.h"
#include <random>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <mutex>
#include <string>
#include <chrono>
#include <future>
#include <condition_variable>
#include <iomanip>
#include <thread>

//Global Simulation Constants and Setup
int ballCount = 1000;
float ballRadius = 3;
float screenWidth = 500;
float screenHeight = 700;
float diameter = 2 * ballRadius;
std::mutex ballMutex;
std::condition_variable physicsCondition;
bool physicsCompleted = false;

void ThreadPhysics(Physics& physics, std::vector<Ball>& ball, float distance, float dX, float dY, int start, int finish);

int main(int argc, char** argv)
{
	//Main variable setup
	screenWidth = (std::fmod(screenWidth,4.0f) != 0.0f) ? (screenWidth = 4 * std::floor(screenWidth / 4)) : screenWidth;
	std::cout << screenWidth << std::endl;
	int i = 0;
	float dX = 0; 
	float dY = 0; 
	float distance = 0; 
	int frameTime = 0; 
	int threadOffset = screenWidth / 4;

	//Simulation bake prompt (not implemented)
	std::cout << "Would you like to bake the simulation? (y/n)" << std::endl;
	std::string bake;
	std::cin >> bake;

	//Random number generator
	std::random_device rd; 
	std::mt19937 gen(rd()); 
	std::uniform_real_distribution<float> rnd_xPosition(ballRadius, screenWidth - ballRadius); 
	std::uniform_real_distribution<float> rnd_yPosition(ballRadius, screenHeight - ballRadius); 
	std::uniform_real_distribution<float> rnd_Velocity(-100, 100); 

	//Build required objects
	Physics physics;
	std::vector<Ball> ball;

	//Creates the balls
	for (i = 0; i < ballCount; i++)
	{
		ball.emplace_back(Ball(rnd_xPosition(gen), rnd_yPosition(gen),  
							   rnd_Velocity(gen),  rnd_Velocity(gen), i));
	}

	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Ball Physics"); 
	
	//Simulation loop
    while (window.isOpen())
    {

		auto start = std::chrono::high_resolution_clock::now(); 
        sf::Event event;
        while (window.pollEvent(event)) 
		{
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
		
		//Multihtreading ball physics calculations based off horizontal position
		auto t1 = std::async(std::launch::async, ThreadPhysics, std::ref(physics), std::ref(ball),
			std::ref(distance), std::ref(dX), std::ref(dY), 0, threadOffset);
		auto t2 = std::async(std::launch::async, ThreadPhysics, std::ref(physics), std::ref(ball), 
			std::ref(distance), std::ref(dX), std::ref(dY), threadOffset,     2 * threadOffset);
		auto t3 = std::async(std::launch::async, ThreadPhysics, std::ref(physics), std::ref(ball), 
			std::ref(distance), std::ref(dX), std::ref(dY), 2 * threadOffset, 3 * threadOffset);
		auto t4 = std::async(std::launch::async, ThreadPhysics, std::ref(physics), std::ref(ball), 
			std::ref(distance), std::ref(dX), std::ref(dY), 3 * threadOffset, screenWidth+1);
		

		window.clear();

		//Updates kinematics, color, and then draws every ball
		for (i = 0; i < ballCount; i++)
		{
			physics.UpdatePosition(ball[i]);
			ball[i].Set_Position();
			ball[i].ChangeColor();
			ball[i].draw(window); 
		}
		window.display();
		
		//Makes multithreading work
		std::unique_lock<std::mutex> lock(ballMutex); 
		physicsCondition.wait(lock, [&] { return physicsCompleted; }); 
		physicsCompleted = false;

		//Prints frame time to console and forces frame rate
		auto stop = std::chrono::high_resolution_clock::now(); 
		frameTime = (std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count(); 
		std::system("cls");
		std::cout << "" << std::endl;
		std::cout << " Frame Time: " << frameTime << "ms" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(16 - std::min(frameTime,16)));
    }
	return 0;
}

//Method to decide when to perform collision calculation
void ThreadPhysics(Physics& physics, std::vector<Ball>& ball, float distance, float dX, float dY, int start, int finish)
{
	for (int i = 0; i < ballCount; i++)
	{
		//Picks ball via x position
		if (ball[i].x >= start && ball[i].x < finish)
		{
			physics.WallCollision(ball[i]);
			for (int j = 0; j < ballCount; j++) 
			{
				if (j != i) 
				{
					dX = ball[j].x - ball[i].x; 
					dY = ball[j].y - ball[i].y; 

					//Rough distance check
					if (+dX <= diameter && +dY <= diameter) 
					{
						distance = Magnitude(dX, dY);
						//Exact distance check
						if (distance <= diameter)
						{
							physics.BallCollide(ball[i], ball[j], distance, dX, dY); 
							std::lock_guard<std::mutex> lock(ballMutex);
						}
					}
				}
			}
		}
		ball[i].velocity = std::min(100.0f, Magnitude(ball[i].Vx, ball[i].Vy));
	}
	physicsCompleted = true;
	physicsCondition.notify_one();
}


