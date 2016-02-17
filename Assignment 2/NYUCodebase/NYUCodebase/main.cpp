#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

//float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	SDL_FreeSurface(surface);
	return textureID;
}
float vertices1[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
SDL_Window* displayWindow;
float inFrameTicks = 0.0f;
// paddle y-positions
float xPos = 0.0f;
float yPos = 0.0f;

//ball position to move
float ballX = 0.2f;
float ballY = -0.07f;

float ballIX = 0.0f;
float ballIY = 0.0f;

//ball collission
float ballL = -0.125f;
float ballR = 0.125f;
float ballU = 0.125f;
float ballD = -0.125f;

//left paddle collission
float lpR = -1.7f;
float lpU = 0.5f;
float lpD = -0.5f;

//right paddle collission
float rpL = 1.7f;
float rpU = 0.5f;
float rpD = -0.5f;

// color values to indicate who scored, red is for the right, blue is for the left
float red = 0.0;
float blue = 0.0;
//bool moveRight = true;

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	
	#ifdef _WINDOWS
		glewInit();
	
	#endif
		glViewport(0, 0, 640, 360);
		ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
		GLuint smile = LoadTexture("emoji.png");
		
		Matrix projectionMatrix;
		Matrix modelMatrix;
		Matrix viewMatrix;

		
		projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
		glUseProgram(program.programID);
		
	SDL_Event event;

	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			const Uint8 *keys = SDL_GetKeyboardState(NULL);
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			if (yPos < 2.0 && event.key.keysym.scancode == SDL_SCANCODE_W){  // move right paddle up
					yPos += 0.2;
					rpD += 0.2; //update coordinates
					rpU += 0.2;
				}
			if (yPos > -2.0 && event.key.keysym.scancode == SDL_SCANCODE_S){  // move right paddle down
					yPos -= 0.2;
					rpD -= 0.2;
					rpU -= 0.2;
				}
			if (xPos < 2.0 && event.key.keysym.scancode == SDL_SCANCODE_Q){  // move left paddle up
					xPos += 0.2;
					lpD += 0.2; // update coordinates
					lpU += 0.2;
				}
			if (xPos > -2.0 && event.key.keysym.scancode == SDL_SCANCODE_A){  // move left paddle down
					xPos -= 0.2;
					lpD += 0.2;
					lpU += 0.2;
				}
			if ((ballU < rpU) && (ballD > rpD) && (ballR > rpL) ||  // calculate if right or left
				(ballU < lpU) && (ballD > lpD) && (ballL < lpR)     // paddles collided with ball
				){
				ballX *= -1;  // just reverse the direction of the ball is going in the x direction
			}
			if (ballU > 1.0f || ballL < -1.0f){ // ball bouncing off the ceiling or floor
				ballY *= -1; // reverse the ball's y direction
			}
			if (ballL < -1.8f){ // show that right paddle scored, turn bg blue
				red = 0.0f;
				blue = 1.0f;

				ballX = 0.2f; // reset ball
				ballY = -0.07f;

				ballIX = 0.0f;
				ballIY = 0.0f;

				ballL = -0.125f;
				ballR = 0.125f;
				ballU = 0.125f;
				ballD = -0.125f;
			}
			if (ballR > 1.8f){ // show that left paddle scored, turn bg red
				red = 1.0f;
				blue = 0.0f;

				ballX = 0.2f; // reset ball
				ballY = -0.07f;

				ballIX = 0.0f;
				ballIY = 0.0f;

				ballL = -0.125f;
				ballR = 0.125f;
				ballU = 0.125f;
				ballD = -0.125f;
			}

	//if (elapsed < 2.0f) elapsed += 0.1;
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - inFrameTicks;
		inFrameTicks = ticks;
		/*
		To-Do
		
		4 - make ball move opposite x-direction when touches a pddle
		5 - make ball move opposite y-direction when touches ceiling or floor
		6 - change color of ball if it hits either walls
		7 - detection for walls
		  
		*/
		// calculate the ball's position to translate by
		ballIX += elapsed * ballX;
		ballIY += elapsed * ballY;

		// calculate the ball's coordinates for collission 
		ballD += elapsed * ballX;
		ballU += elapsed * ballX;
		ballL += elapsed * ballY;
		ballR += elapsed * ballY;

		
		}
		glClearColor(red, 0.0f, blue, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);



		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);



		modelMatrix.identity();
		modelMatrix.Translate(ballIX, ballIY, 0.0);
		modelMatrix.Scale(0.25, 0.25, 0.0);
		program.setModelMatrix(modelMatrix);
		glBindTexture(GL_TEXTURE_2D, smile);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		///////////////////////////////////////////////////////////
		modelMatrix.identity();
		modelMatrix.Translate(-3.5, 0.0, 0.0);
		modelMatrix.Translate(0.0, xPos, 0.0);
		modelMatrix.Scale(0.25, 1.0, 0.0);
		program.setModelMatrix(modelMatrix);
		glBindTexture(GL_TEXTURE_2D, smile);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		//////////////////////////////////////////////////////////////
		modelMatrix.identity();
		modelMatrix.Translate(3.5, 0.0, 0.0);
		modelMatrix.Translate(0.0, yPos, 0.0);
		modelMatrix.Scale(0.25, 1.0, 0.0);
		program.setModelMatrix(modelMatrix);
		glBindTexture(GL_TEXTURE_2D, smile);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);



		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
