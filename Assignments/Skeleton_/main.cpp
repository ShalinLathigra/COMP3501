/*
 *
 * A program that demonstrates how to render two different shapes stored
 * as triangle meshes
 *
 * Copyright (c) 2018-2019 Oliver van Kaick <Oliver.vanKaick@carleton.ca>
 *
 */

#include <iostream>
#include <stdexcept>

#include "Application.h"

// Macro for printing exceptions
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl

int main(void)
{
	Application app;

	try 
	{
		app.Initialize();
		app.SetupGameManager();
		app.SetupScene();

		app.StartGame();
	} 
	catch (std::exception &e)
	{
		PrintException(e);
	}

	return 0;
}