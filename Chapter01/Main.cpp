/*
	Guilherme Chagas / 22119013-5
	Luca Milla / 22119002-8
	Matheus Pelicer / 22119024-2
	Thiago Soares / 22119044-0
*/

// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "GamePong.h"
#include "GameArkanoid.h"
#include <iostream>

int main(int argc, char** argv)
{
	int option;
	while (true) {
		std::cout << "Select a option:" << std::endl;
		std::cout << "1 - Pong" << std::endl;
		std::cout << "2 - Arkanoid" << std::endl;
		std::cout << "3 - Exit" << std::endl;
		std::cin >> option;

		if (option == 1) {
			std::cout << "You are playing pong. Enjoy." << std::endl;
			GamePong pong;
			bool success = pong.Initialize();
			if (success)
			{
				pong.RunLoop();
			}
			pong.Shutdown();
		}
		else if (option == 2) {
			std::cout << "You are playing arkanoid. Enjoy." << std::endl;
			GameArkanoid arkanoid;
			bool success = arkanoid.Initialize();
			if (success)
			{
				arkanoid.RunLoop();
			}
			arkanoid.Shutdown();
		}
		else if (option == 3) {
			break;
		}
		else {
			std::cout << "Invalid input. Try Again" << std::endl;
		}
	}
	
	return 0;
}
