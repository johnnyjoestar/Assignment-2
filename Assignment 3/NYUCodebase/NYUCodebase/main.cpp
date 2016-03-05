/*
Assignment #2
Ryan Goudjil
N18559209
CS3113

Space Invaders
2 scenes, title and game
Must use sprite sheets
Must use text

TO-DO
1) Set up rendering  
	- Player, Enemy, and Bullet needs a draw 
	  function to render in the global render function
	- Using spritesheet function to change the textID for 
	  each one
	+ Write a function that makes it so in a for loop
	  each textID cycles through every 2 seconds
	  for a simple animation

2) Set up cases
	- Using bool flags instead of enums, for now
	- put cases in the process function
	- title case just renders the title text
	- if space is pressed, change bool flags
	- game case is everything else

3) Set up win/loss case
	- if aliens get to a certain y value
	- check for that in process
	- if all aliens are shot
	- check for that in process with for loop
	- if all aliens are !alive, then change to title

4) Move aliens
	- aliens move to the left x direction until
	  time counts down
	- when timer is 0, move down by y
	  and reverse direction (multiply x by -1)

5) Collission
    - Check collissions on every bullet in the game
	- in process
	- if source is enemy, check if colliding with player
	- if player, check if colliding with enemy
	- if yes, die(player/enemy)
	- isAlive = false -> in respective Update, delete.this



*/
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
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

// GAMELOOP
//float toShoot = 2.0f;
float moveDown = 3.0f;
float lastFrameTicks = 0.0f;
float angle = 0.0f;

// STATES
bool title = true;
bool game = true;

// TEXTURES
GLuint example;
GLuint textTexture;
GLuint playerTexture;
GLuint enemyTexture;
GLuint bulletTexture;

// GENERAL
bool done = false;
SDL_Event event;

ShaderProgram program();
void DrawSpriteSheetSprite(ShaderProgram program, int index, int spriteCountX, int spriteCountY, float x, float y, GLuint textID);

// classes 
class Entity{
public:
	
	void Draw(ShaderProgram program){}
	void Update(float Elapsed){}

	float position_x;
	float position_y;

	int textureID;
	bool isPlayer;

	float width;
	float height;

	float velocity_x;
	float velocity_y;
};
void shoot(Entity* entity);
class Player:public Entity{
public:
	float position_x;
	float position_y;
	float toShoot;

	bool isAlive;
	bool canShoot;
	int textureID;
	bool isPlayer;
	float spriteTimer = 0.0f;

	float width;
	float height;

	float velocity_x;
	float velocity_y;
	int index = 0;

	Player(float x, float y, float h, float w) : position_x(x), position_y(y), height(h), width(w), isPlayer(true), canShoot(true), isAlive(true), toShoot(0.0f) {}
	
	void Draw(ShaderProgram program){
		if (isAlive == false)return;
		/*glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);


		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		modelMatrix.identity();
		modelMatrix.Translate(position_x, position_y, 0);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		glBindTexture(GL_TEXTURE_2D, playerTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);*/
		DrawSpriteSheetSprite(program, index, 2, 1, position_x, position_y, playerTexture);
	}

	void Update(float elapsed){
		if (isAlive == false) return;

		if (spriteTimer >= 2.0f) index = 0;
		else { index = 1; spriteTimer = 0.0f; }
		spriteTimer += elapsed;
		toShoot += elapsed;
		if (toShoot >= 3.0f) {
			canShoot == true;
		}
	}
};

class Enemy :public Entity{
public:
	float position_x;
	float position_y;
	float toShoot;
	float moveTimer;


	int textureID;
	bool isPlayer;
	bool isAlive;

	float width;
	float height;
	int index = 0;
	float spriteTimer = 0.0f;
	
	float velocity_y;

	Enemy(float x, float y, float h, float w) :
		position_x(x), position_y(y), height(h), width(w), velocity_y(2.0f) , isPlayer(false), isAlive(true), moveTimer(0.0f){}
	
	void Draw(ShaderProgram program){
		if (isAlive == false) return;
		DrawSpriteSheetSprite(program, index, 2, 1, position_x, position_y, enemyTexture);

	/*	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		modelMatrix.identity();
		modelMatrix.Translate(position_x, position_y, 0);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		glBindTexture(GL_TEXTURE_2D, enemyTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);*/
	}

	void Update(float elapsed){
	// check to see if the enemy can shoot
	// by checking if the toShoot >= 3
	// if it is
	// random chance to shoot
	// then reset to 0
	// increment by elapsed
		if (isAlive == false) return;
		toShoot += elapsed;
		if (spriteTimer >= 1.0f) index = 0;
		else { index = 1; spriteTimer = 0.0f; }
		spriteTimer += elapsed;
		if (toShoot >= rand() % 5 + 2){
			if(4 > rand() % 8)shoot(this);
			toShoot = 0.0f;
		}
		moveTimer += elapsed;
		if (moveTimer <= 3.0f){ 
			position_y += elapsed*velocity_y;
		}
		else{
			position_x -= 1.0f;
			moveTimer = 0.0f;
			velocity_y *= -1.0f;
		}

	}
};

class Bullet{
public:
	float posX, posY, height, width;
	float velY;
	float timeActive;
	bool shotByPlayer;
	GLuint bulletTexture;
	int index = 0;
	float spriteTimer = 0.0f;
	Bullet(float vy, float xp, float xy, bool sbp) :velY(vy), posX(xp), posY(xy), shotByPlayer(sbp), timeActive(5.0f), height(1.0f), width(1.0f)
	{
		if (shotByPlayer == false) vy *= -1;
	}

	void Draw(ShaderProgram program){
		DrawSpriteSheetSprite(program, index, 2, 1, posX, posY, bulletTexture);

		/*glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);


		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		modelMatrix.identity();
		modelMatrix.Translate(posX, posY, 0);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		glBindTexture(GL_TEXTURE_2D, bulletTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);*/
	}

	void Update(float elapsed){
		posY += elapsed*velY;
		if (spriteTimer >= 1.0f) index = 0;
		else { index = 1; spriteTimer = 0.0f; }
		spriteTimer += elapsed;
	}

};

class SpriteClass{};

Player *player;

// VECTORS
vector<Enemy*> enemies;
vector<Bullet*> bullets;
vector<Entity*> entities;
// functions
void shoot(Entity* entity){
	if (entity->isPlayer && player->canShoot)
	{
		bullets.push_back(new Bullet(10, entity->position_x, entity->position_y + 1.0f, true));
		player->toShoot = 0.0f;
	}
	else bullets.push_back(new Bullet(10, entity->position_x, entity->position_y - 1.0f, false));
}

void deleteBullets(){
	if (bullets.size() >= 30)
	for (int i = 0; i < bullets.size(); i++){
		if (bullets[i]->timeActive <= 0.0f){
			bullets[i] = nullptr;
			delete bullets[i];
		}
	}
}

void DrawText(ShaderProgram program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program.programID);
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program.texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}

void DrawSpriteSheetSprite(ShaderProgram program, int index, int spriteCountX, int spriteCountY, float x, float y, GLuint textID) {
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;
	GLfloat texCoords[] = {
		u, v + spriteHeight,
		u + spriteWidth, v,
		u, v,
		u + spriteWidth, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight
	};
	float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f };

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	modelMatrix.identity();
	modelMatrix.Translate(x, y, 0.0);
	program.setModelMatrix(modelMatrix);

	glBindTexture(GL_TEXTURE_2D, textID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}

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
	displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 1280, 720);
	// ratio is 1.77779 : 1 but for calculation's sake i use another ratio [1280/80:720/80]
	projectionMatrix.setOrthoProjection(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f);
	
	player = new Player(0.0f, -7.5f, 1.0f, 1.0f);
	example = LoadTexture(RESOURCE_FOLDER"emoji.png");
	playerTexture = LoadTexture(RESOURCE_FOLDER"player.png");
	enemyTexture = LoadTexture(RESOURCE_FOLDER"enemy.png");
	bulletTexture = LoadTexture(RESOURCE_FOLDER"bullet.png");
	textTexture = LoadTexture(RESOURCE_FOLDER"font1.png");


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// setup enemies

	float xDummy = -10.0f;
	float yDummy = 7.0f;

	for (int i = 0; i < 30; i++){
		enemies.push_back(new Enemy(xDummy, yDummy, 1.0f, 1.0f));
		xDummy += 2.0f;
		if (i != 0 && (i % 6 == 0)){
			xDummy = -10.0f;
			yDummy -= 2.0f;
		}
	}

}

bool aliensWin(){
	for (int i = 0; i < enemies.size(); i++){
		if (enemies[i]->position_y <= -7) return true;
	}
	return false;
}

bool aliensDead(){
	for (int i = 0; i < enemies.size(); i++)
	if (enemies[i]->isAlive) return true;
	return false;
}

void checkGameOver(){
	//check if player is dead
	//check if all are dead
	//check if any touch threshold
	if (player->isAlive == false){
		title = true;
		game = false;
	}
	if (aliensDead() == false){
		title = true;
		game = false;
	}
	if (aliensWin() == true){
		title = true;
		game = false;
	}

}

void Update(float elapsed){
	if (title) return;
	for (int i = 0; i < entities.size(); i++){
		entities[i]->Update(elapsed);
	}
}

void ProcessEvents(){
	if (title == true){
		while (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE){
				done = true;
			}
			else if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
					game = true;
					title = false;
				}
			}

		}
	}
	if (game == true){

		while (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE){
				done = true;
			}
			else if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
					shoot(player);
				}
			}

		}
		float ticks = (float)SDL_GetTicks() / 1000.0F;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		glClearColor(0.4f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_LEFT]){
			player->position_x -= player->velocity_x * elapsed;
		}

		if (keys[SDL_SCANCODE_RIGHT]){
			player->position_x += player->velocity_x * elapsed;
		}

		for (int i = 0; i < bullets.size(); i++){
			//check if hit player
			if (bullets[i]->shotByPlayer == false){
				if (((bullets[i]->posY - (bullets[i]->height / 2.0f)) >(player->position_y + (player->height / 2.0f)) ||
					(bullets[i]->posY + (bullets[i]->height / 2.0f)) < (player->position_y - (player->height / 2.0f)) ||
					(bullets[i]->posX - (bullets[i]->width / 2.0f)) > (player->position_x + (player->width / 2.0f)) ||
					(bullets[i]->posX + (bullets[i]->width / 2.0f)) < (player->position_x - (player->width / 2.0f))) 
					== false){
					player->isAlive = false;
				}
			}
			//check if hit enemy
			else {
				for (int i = 0; i< enemies.size(); i++)
				if (((bullets[i]->posY - (bullets[i]->height / 2.0f)) >(enemies[i]->position_y + (enemies[i]->height / 2.0f)) ||
					(bullets[i]->posY + (bullets[i]->height / 2.0f)) < (enemies[i]->position_y - (enemies[i]->height / 2.0f)) ||
					(bullets[i]->posX - (bullets[i]->width / 2.0f)) > (enemies[i]->position_x + (enemies[i]->width / 2.0f)) ||
					(bullets[i]->posX + (bullets[i]->width / 2.0f)) < (enemies[i]->position_x - (enemies[i]->width / 2.0f)))
					== false){
					enemies[i]->isAlive = false;
					delete enemies[i];
				}
			}
		}

		Update(elapsed);
	}
}

void Render(ShaderProgram program){
	if (title){
		modelMatrix.identity();
		modelMatrix.Translate(-8.0f, 5.0f, 0.0f);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		DrawText(program, textTexture, "Space Invaders", 2.0f, 1.0f);
	}
	if (game){
		for (int i = 0; i < entities.size(); i++){
			entities[i]->Draw(program);
		}
	}
}

void CleanUp(){
// need this to delete everything and reset values
}

int main(int argc, char *argv[])
{
	SetUp();
	ShaderProgram program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	while (!done){
		ProcessEvents();
		Render(program);
	}
	SDL_Quit();
	return 0;
}
