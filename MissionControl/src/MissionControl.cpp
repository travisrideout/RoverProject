#include "ServerSocket.h"
#include <iostream>
#include <stdio.h>			// For _sleep()
#include <conio.h>
#include <SDL2/SDL.h>
#include <windows.h>		// For Sleep()
#include <Rover5Coms.h>
#include "CollisionAvoidance.h"

int main(int argc, char *argv[]) {
	ServerSocket server;			// create a server object for communication
	CollisionAvoidance collAv(server);	//create a collision avoidance object

	data_struct scratch_vars;

	SDL_Window *window = NULL;
	SDL_Surface *windowSurface = NULL;
	SDL_Surface *currentImage = NULL;

	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK );  //
	SDL_Joystick *joystick = SDL_JoystickOpen(0);

	window = SDL_CreateWindow("Rover5", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
	windowSurface = SDL_GetWindowSurface(window);
	currentImage = SDL_LoadBMP("Screen.bmp");

	std::cout << "Controller Name: " << SDL_JoystickName(joystick) << std::endl;
	std::cout << "Num axes: " << SDL_JoystickNumAxes(joystick) << std::endl;
	std::cout << "Num Buttons: " << SDL_JoystickNumButtons(joystick) << std::endl;

	SDL_Event ev;

	bool isRunning = true;

	while(isRunning){
		Sleep(30);		// Sleep for 30ms to reduce CPU usage
		SDL_BlitSurface(currentImage, NULL, windowSurface, NULL);
		SDL_UpdateWindowSurface(window);
		server.GetSendMessageVars(&scratch_vars);

		while(SDL_PollEvent(&ev) != 0){
			//Getting quit and the keyboard events
			if(ev.type == SDL_KEYDOWN){
				if (ev.key.keysym.sym == SDLK_ESCAPE){
					isRunning = false;
				}
			}else if(ev.type == SDL_QUIT){
				isRunning = false;
			}else if (ev.type == SDL_JOYAXISMOTION){
				if(ev.jaxis.which == 0){
					if(ev.jaxis.axis == 1){
						if(ev.jaxis.value < 0){
							scratch_vars.lDirCmd = 1;
						}else{
							scratch_vars.lDirCmd = 0;
						}
						if(abs(ev.jaxis.value) > 5000){
							scratch_vars.lDutyCmd = (abs(ev.jaxis.value)/32767.0)*100;
						}else{
							scratch_vars.lDutyCmd = 0;
						}
					}
					if(ev.jaxis.axis == 4){
						if(ev.jaxis.value < 0){
							scratch_vars.rDirCmd = 1;
						}else{
							scratch_vars.rDirCmd = 0;
						}
						if(abs(ev.jaxis.value) > 5000){
							scratch_vars.rDutyCmd = (abs(ev.jaxis.value)/32767.0)*100;
						}else{
							scratch_vars.rDutyCmd = 0;
						}
					}

					//collAv.ProximityHalt(&scratch_vars);

					//server.SetSendMessageVars(&scratch_vars);
					/*std::cout << "L_DIR = " << scratch_vars.lDirCmd << "\tR_DIR = " << scratch_vars.rDirCmd
							<< "\tL_Duty = " << scratch_vars.lDutyCmd << "\tR_Duty = " << scratch_vars.rDutyCmd << std::endl;*/

				}
			}
		}

		collAv.ProximityHalt(&scratch_vars);

		server.SetSendMessageVars(&scratch_vars);

	}

	SDL_FreeSurface(currentImage);
	currentImage = NULL;
	SDL_DestroyWindow(window);
	window = NULL;
	windowSurface = NULL;
	SDL_Quit();

	std::cout << "end program" << std::endl;

	return 0;
}
