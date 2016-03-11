/*
Assignment #4 - Sound
Ryan Goudjil
N18559209
CS3113

*/
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include <vector>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
using namespace std;
#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


// GRAPHICS
float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
SDL_Window* displayWindow;

// MATRICES
Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;

// MUSIC
Mix_Chunk *ballCollide;
Mix_Chunk *ballScore;
Mix_Music *music;

// GAMELOOP
//float toShoot = 2.0f;
float moveDown = 3.0f;
float lastFrameTicks = 0.0f;
float angle = 0.0f;

// SCORES
int score1 = 0;
int score2 = 0;
string scoreA;
string scoreB;
string scoreC;

// STATES
bool title = true;
bool game = true;

// TEXTURES
GLuint example;
GLuint textTexture;
GLuint paddleTexture;
GLuint ballTexture;

// GENERAL
bool done = false;
SDL_Event event;

ShaderProgram* program;
void DrawSpriteSheetSprite(ShaderProgram* program, int index, int spriteCountX, int spriteCountY, float x, float y, GLuint textID);

// classes 
class Entity{
public:
	
	void Draw(ShaderProgram* program){}
	void Update(float elapsed){}

	float position_x;
	float position_y;

	int textureID;

	float width;
	float height;

	float velocity_x;
	float velocity_y;
};

class Paddle:public Entity{
public:
	Paddle(float px, float py) :position_x(px), position_y(py), velocity_y(2.0f), width(0.5f), height(3.0f), textureID(paddleTexture){}
	float position_x;
	float position_y;

	int textureID;
	// if i scale x by 0.5, width should be 0.5
	// if i scale y by 3, height should b 3
	float width;
	float height;

	float velocity_y;
	void Draw(ShaderProgram* program){
		program->setModelMatrix(modelMatrix);
		program->setProjectionMatrix(projectionMatrix);
		program->setViewMatrix(viewMatrix);

		modelMatrix.identity();
		modelMatrix.Translate(position_x, position_y, 0.0);
		modelMatrix.Scale(width, height, 0.0);
		program->setModelMatrix(modelMatrix);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);		
	}
	void Update(float elapsed){}
};

class Ball :public Entity{
public:
	Ball():position_x(0.0f),position_y(0.0f),velocity_x(2.0f),velocity_y(0.5f),width(1.0f),height(1.0f),textureID(ballTexture){}
	float position_x;
	float position_y;

	int textureID;

	// if i scale x by 0.5, width should be 0.5;
	float width;
	float height;

	float velocity_x;
	float velocity_y;
	void Draw(ShaderProgram* program){
		program->setModelMatrix(modelMatrix);
		program->setProjectionMatrix(projectionMatrix);
		program->setViewMatrix(viewMatrix);

		modelMatrix.identity();
		modelMatrix.Translate(position_x, position_y, 0.0);
		modelMatrix.Scale(width, height, 0.0);
		program->setModelMatrix(modelMatrix);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);

	}
	void Update(float elapsed){
		position_x += elapsed * velocity_x;
		position_y += elapsed * velocity_y;
	}
};

class SpriteClass{};

vector<Entity*> entities;
// functions

Ball * ball;
Paddle * lPad;
Paddle * rPad;
float red = 0.5f;
float blue = 0.5f; // these are to show who's winning

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

void SetUp(){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 1280, 720);
	projectionMatrix.setOrthoProjection(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f);
	
	ballTexture = LoadTexture(RESOURCE_FOLDER"emoji.png");
	paddleTexture = LoadTexture(RESOURCE_FOLDER"emoji.png");
	example = LoadTexture(RESOURCE_FOLDER"emoji.png");
	textTexture = LoadTexture(RESOURCE_FOLDER"font1.png");
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	lPad = new Paddle(-5.0f, 0.0f);
	ball = new Ball();
	rPad = new Paddle(5.0f, 0.0f);

	ballCollide = Mix_LoadWAV("blip.wav");
	ballScore = Mix_LoadWAV("bicycle_bell.wav");
	music = Mix_LoadMUS("mariomusic.mp3");
	Mix_PlayMusic(music, -1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Update(float elapsed){
	/*for (int i = 0; i < entities.size(); i++){
		entities[i]->Update(elapsed);
	}*/
	ball->Update(elapsed);
	lPad->Update(elapsed);
	rPad->Update(elapsed);
}

void ProcessEvents(){
	//Mix_PlayMusic(music,-1);
	while (SDL_PollEvent(&event)){
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE){
			done = true;
		}
		else if (event.type == SDL_KEYDOWN){
			if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
				score1 = 0;
				score2 = 0;
				ball->position_x = 0.0f;
				ball->position_y = 0.0f;
				
			}
		}

	}
	float ticks = (float)SDL_GetTicks() / 1000.0F;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	glClearColor(0.4f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_LEFT] && lPad->position_y > -4.0f){
		lPad->position_y -= lPad->velocity_y * elapsed;
	}

	if (keys[SDL_SCANCODE_RIGHT] && lPad->position_y < 4.0f){
		lPad->position_y += lPad->velocity_y * elapsed;
	}	
	if (keys[SDL_SCANCODE_UP] && rPad->position_y < 4.0f){
		rPad->position_y += rPad->velocity_y * elapsed;
	}

	if (keys[SDL_SCANCODE_DOWN] && rPad->position_y > -4.0f){
		rPad->position_y -= rPad->velocity_y * elapsed;
	}

	//check if hit player

	if (((ball->position_y - (ball->height / 2.0f)) > (lPad->position_y + (lPad->height / 2.0f)) ||
		(ball->position_y + (ball->height / 2.0f)) < (lPad->position_y - (lPad->height / 2.0f)) ||
		(ball->position_x - (ball->width / 2.0f)) > (lPad->position_x + (lPad->width / 2.0f)) ||
		(ball->position_x + (ball->width / 2.0f)) < (lPad->position_x - (lPad->width / 2.0f)))
		== false){
		Mix_PlayChannel(-1, ballCollide, 0);
		ball->velocity_x *= -1.0f;
	}
	if (((ball->position_y - (ball->height / 2.0f)) >(rPad->position_y + (rPad->height / 2.0f)) ||
		(ball->position_y + (ball->height / 2.0f)) < (rPad->position_y - (rPad->height / 2.0f)) ||
		(ball->position_x - (ball->width / 2.0f)) > (rPad->position_x + (rPad->width / 2.0f)) ||
		(ball->position_x + (ball->width / 2.0f)) < (rPad->position_x - (rPad->width / 2.0f)))
		== false){
		Mix_PlayChannel(-1, ballCollide, 0);
		ball->velocity_x *= -1.0f;
	}
	// if ball hits the left wall
	if (ball->position_x < -6.5){
			score1++;
			Mix_PlayChannel(-1, ballScore, 0);
			ball->position_x = 0.0f;
			ball->position_y = 0.0f;
			if (red < 1.0f) red += 0.05f;
			if (blue > 1.0f) blue -= 0.05f;
		}
		// if ball hits the right wall
	if (ball->position_x > 6.5){
			score2++;
			Mix_PlayChannel(-1, ballScore, 0);
			ball->position_x = 0.0f;
			ball->position_y = 0.0f;
			if (blue < 1.0f) blue += 0.05f;
			if (red > 1.0f) red -= 0.05f;
		}
		
	// check ceiling
	if (ball->position_y > 4.0f || ball->position_y < -4.0f){
		ball->velocity_y *= -1.0f;
		Mix_PlayChannel(-1, ballCollide, 0);

	}
		/**/
			
			//check if hit enemy

		Update(elapsed);
}

void Render(ShaderProgram* program){
	glClearColor(red, 0.0f, blue, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	scoreA = "P1: " + score1;
	scoreB = "P2: " + score2;
	scoreC = scoreA + "  " + scoreB;
	modelMatrix.identity();
	modelMatrix.Translate(0.0f, 6.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	
	lPad->Draw(program);
	rPad->Draw(program);

	ball->Draw(program);
		/*for (int i = 0; i < entities.size(); i++){
			entities[i]->Draw(program);
		}*/
	SDL_GL_SwapWindow(displayWindow);

}

void CleanUp(){
// need this to delete everything and reset values
	Mix_FreeChunk(ballCollide);
	Mix_FreeChunk(ballScore);
	Mix_FreeMusic(music);
}


int main(int argc, char *argv[])
{
	SetUp();
	program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	while (!done){
		ProcessEvents();
		Render(program);

	}
	SDL_Quit();
	return 0;
}
