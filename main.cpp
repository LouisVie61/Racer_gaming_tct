#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int SCREEN_WIDTH                    =          875;
const int SCREEN_HEIGHT                   =          480;
int characterX_first                      =            0;
int characterY_first                      =          240;
const int shift_lane_up                   =          120;
const int shift_lane_down                 =          120;
const int shift_inLane_right              =           20;
const int shift_inLane_left               =           20;
const int MAX_SIZE_OP_CAR                 =            5;
const int MAX_SPEED                       =            6;
const int MIN_SPEED                       =            2;
const int NUM_LANE_SIZE                   =            3;
int POS_OPP_CAR_BEGIN_X                   =          900;
bool car_crashed = false;
int crashX, crashY;

class LTexture {
	public:
		LTexture();
		~LTexture();
		bool loadFromFile(string path );
		void free();
		void render( int x, int y, SDL_Rect* clip = NULL );
		int getWidth();
		int getHeight();
        void resetPosition(int x, int y);
	private:
		SDL_Texture* mTexture;
		int mWidth;
		int mHeight;
};
LTexture::LTexture() {
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}
LTexture::~LTexture() {
	free();
}

class opposite_car_struct {
public:
    void render(SDL_Rect* clip = NULL);
    bool loadFromFile(string path);
    void update();
    int getWidth_OP();
    int getHeight_OP();
    int speed;
    int getX();
    int getY();
    bool active;
    void resetPosition();
private:
    int pos_x;
    int pos_y;
    int dimension_width;
    int dimension_height;
    SDL_Texture* texture;
};
opposite_car_struct op_car[MAX_SIZE_OP_CAR];
int LANE_Y[NUM_LANE_SIZE] = {120, 240, 360};
vector<int> active_pos;
//28/03/2024
class startScene {
public:
    startScene();
    ~startScene();
    bool loadMedia();
    bool loadFromFile(string path);
    void render( int x, int y, SDL_Rect* clip = NULL );
    bool handleEventMouseButton(SDL_Event& e);

private:
    SDL_Texture* mStartScene;
    SDL_Texture* mStartButton;
    //may need
    int positionX_but;
    int positionY_but;
    //dimensions
    int dm_width;
    int dm_height;

};
startScene::startScene() : mStartScene(nullptr), mStartButton(nullptr) {}
startScene::~startScene() {
    if (mStartScene != nullptr) {
        SDL_DestroyTexture(mStartScene);
        mStartScene = nullptr;
    }
    if (mStartButton != nullptr) {
        SDL_DestroyTexture(mStartButton);
        mStartButton = nullptr;
    }
}
bool startScene::handleEventMouseButton(SDL_Event& e){
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (mouseX >= positionX_but && mouseX <= positionX_but + dm_width &&
            mouseY >= positionY_but && mouseY <= positionY_but + dm_height)
        {
            return true;
        }
    }
    return false;
}

class endScene {
public:
    endScene();
    ~endScene();
    bool loadMedia();
    bool loadFromFile(string path);
    void render( int x, int y, SDL_Rect* clip = NULL );
    bool handleEventMouseButton(SDL_Event& e);
private:
    SDL_Texture* mEndScene;
    SDL_Texture* mPlayAgainButton;
    //may need
    int positionX_but_E;
    int positionY_but_E;
    //dimensions
    int dmE_width;
    int dmE_height;
};
endScene::endScene() : mEndScene(nullptr), mPlayAgainButton(nullptr) {}
endScene::~endScene() {
    if (mEndScene != nullptr) {
        SDL_DestroyTexture(mEndScene);
        mEndScene = nullptr;
    }
    if (mPlayAgainButton != nullptr) {
        SDL_DestroyTexture(mPlayAgainButton);
        mPlayAgainButton = nullptr;
    }
}
bool endScene::handleEventMouseButton(SDL_Event &e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= positionX_but_E && mouseX <= positionX_but_E + dmE_width &&
            mouseY >= positionY_but_E && mouseY <= positionY_but_E + dmE_height)
        {
            return true;
        }
    }
    return false;
}

bool init();
bool loadMedia();
void close();
SDL_Texture* loadTexture( string path );
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
LTexture gBackground;
LTexture gBackground_end;
LTexture player_main_car;
LTexture player_main_car_crash;
Mix_Chunk *gGS = NULL; //game start
Mix_Chunk *gGP = NULL; //game play
Mix_Chunk *gCC = NULL; //crashed car
Mix_Chunk *gGO = NULL; //game over

bool init(){
	bool success = true;
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		gWindow = SDL_CreateWindow( "Race Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }
			}
		}
	}

	return success;
}
bool loadMedia(){
	bool success = true;
	if( gBackground.loadFromFile("background.jpg") == false ) {
		printf( "Failed to load background image!\n" );
		success = false;
	}
	if( player_main_car.loadFromFile("player_main_car.png") ==false ) {
		printf( "Failed to load player_main_car image!\n" );
		success = false;
	}
    if( player_main_car_crash.loadFromFile("player_main_car_crash.png") ==false) {
		printf( "Failed to load player_main_car_crash image!\n" );
		success = false;
	}
    for(int i = 0; i < MAX_SIZE_OP_CAR; i++) {
        string path = "opposite_car_" + to_string(i) + ".png";
        if(!op_car[i].loadFromFile(path.c_str()))
        {
            printf("Failed to load opposite car image %d\n", i);
            success = false;
        }
        //logic opposite car speed
        op_car[i].speed = rand() % (MAX_SPEED - MIN_SPEED) + MIN_SPEED;
        op_car[i].active = false;
    }
    gGS = Mix_LoadWAV("start_music.mp3");
    if( gGS == NULL ) {
        printf( "Failed to load GS music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    gGP = Mix_LoadWAV("gamePlay_music.mp3");
    if( gGP == NULL ) {
        printf( "Failed to load GP music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    gCC = Mix_LoadWAV("crash_music.mp3");
    if( gCC == NULL ) {
        printf( "Failed to load CC music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    gGO = Mix_LoadWAV("over_music.mp3");
    if( gGO == NULL ) {
        printf( "Failed to load GO music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
	return success;
}
void close(){
	gBackground.free();
	player_main_car.free();
	player_main_car_crash.free();
	SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
    Mix_FreeChunk( gGS );
    Mix_FreeChunk( gGP );
    Mix_FreeChunk( gCC );
    Mix_FreeChunk( gGO );
    gGS = NULL;
    gGP = NULL;
    gCC = NULL;
    gGO = NULL;
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

void opposite_car_struct::render( SDL_Rect* clip ){
    SDL_Rect renderQuad = { pos_x, pos_y, dimension_width, dimension_height };
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RenderCopy( gRenderer, texture, clip, &renderQuad );
}
bool opposite_car_struct::loadFromFile( string path ){

	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			dimension_width = loadedSurface->w;
			dimension_height = loadedSurface->h;
		}
		SDL_FreeSurface( loadedSurface );
	}
	texture = newTexture;
	return texture != NULL;
}
//update 19/3/2024
//logic opposite car, this also has been in the loadMedia215
//Base Function
void opposite_car_struct::update(){
    if (active) {
        pos_x -= speed;
        if (pos_x < -dimension_width) {
            active = false;
        }
    } else {
        int random_lane = rand() % NUM_LANE_SIZE;
        pos_x = POS_OPP_CAR_BEGIN_X;
        pos_y = LANE_Y[random_lane];
        active = true;
    }
}

bool LTexture::loadFromFile( string path ){
	free();
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}
		SDL_FreeSurface( loadedSurface );
	}
	mTexture = newTexture;
	return mTexture != NULL;
}
void LTexture::free(){
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}
void LTexture::render( int x, int y, SDL_Rect* clip ){
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad );
}
//update 28/3/2024
bool startScene::loadMedia(){
    if (!loadFromFile("start_scene.png"))
    {
        cout << "Failed to load start scene image!\n";
        return false;
    }
    if (!loadFromFile("start_button.png"))
    {
        cout << "Failed to load start button  image!\n";
        return false;
    }
    return true;
}
bool startScene::loadFromFile(string path){
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        cout << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << endl;
        return false;
    }

    if (path == "start_scene.png")
    {
        mStartScene = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (mStartScene == nullptr)
        {
            cout << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << endl;
            return false;
        }

        dm_width = loadedSurface->w;
        dm_height = loadedSurface->h;
    }
    else if (path == "start_button.png")
    {
        mStartButton = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (mStartButton == nullptr)
        {
            cout << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << endl;
            return false;
        }
        positionX_but = (875 - loadedSurface->w) / 2;
        positionY_but = (480 - loadedSurface->h) / 2;
    }
    SDL_FreeSurface(loadedSurface);
    return true;
}
void startScene::render(int x, int y, SDL_Rect* clip) {
    SDL_Rect renderQuad = { x, y, dm_width, dm_height };
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopy(gRenderer, mStartScene, clip, &renderQuad);
}
//update 30/03/2024
bool endScene::loadMedia() {
    if (!loadFromFile("end_scene.png")) {
        cout << "Failed to load end scene image!\n";
        return false;
    }
    if (!loadFromFile("play_again_button.png")) {
        cout << "Failed to load play again button  image!\n";
        return false;
    }
    return true;
}
bool endScene::loadFromFile(string path){
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        cout << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << endl;
        return false;
    }

    if (path == "end_scene.png")
    {
        mEndScene = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (mEndScene == nullptr)
        {
            cout << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << endl;
            return false;
        }

        dmE_width = loadedSurface->w;
        dmE_height = loadedSurface->h;
    }
    else if (path == "play_again_button.png")
    {
        mPlayAgainButton = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (mPlayAgainButton == nullptr)
        {
            cout << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << endl;
            return false;
        }
        positionX_but_E = (875 - loadedSurface->w) / 2;
        positionY_but_E = (480 - loadedSurface->h) / 2;

    }
    SDL_FreeSurface(loadedSurface);
    return true;
}
void endScene::render(int x, int y, SDL_Rect* clip) {
    SDL_Rect renderQuad = { x, y, dmE_width, dmE_height };
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopy(gRenderer, mEndScene, clip, &renderQuad);
}

//the dimensions
int LTexture::getWidth(){
	return mWidth;
}
int LTexture::getHeight(){
    return mHeight;
}
int opposite_car_struct::getWidth_OP(){
    return dimension_width;
}
int opposite_car_struct::getHeight_OP(){
    return dimension_height;
}
int opposite_car_struct::getX(){
    return pos_x;
}
int opposite_car_struct::getY(){
    return pos_y;
}
//logic for collision
bool collisionRaw(SDL_Rect a, SDL_Rect b){
    int leftA = a.x;
    int rightA = a.x + a.w;
    int bottomA = a.y + a.h;
    int topA = a.y;

    int leftB = b.x;
    int rightB = b.x + b.w;
    int topB = b.y;
    int bottomB = b.y + b.h;

    if(rightA <= leftB || leftA >= rightB || bottomA <= topB || topA >= bottomB){
        return false;
    }
    return true;
}
bool collisionMain(opposite_car_struct opponent){
    SDL_Rect playerRect = {characterX_first, characterY_first, player_main_car.getWidth(), player_main_car.getHeight()};
    SDL_Rect opponentRect = {opponent.getX(), opponent.getY(), opponent.getWidth_OP(), opponent.getHeight_OP()};
    if(collisionRaw(playerRect, opponentRect)){
        cout<<"Found collision"<<endl;
        car_crashed = true;
        crashX = characterX_first;
        crashY = characterY_first;
        return true;
    }
    return false;
}
//update 01/04/2024
void LTexture::resetPosition(int x, int y) {
    characterX_first = x;
    characterY_first = y;
}
void opposite_car_struct::resetPosition() {
    pos_x = POS_OPP_CAR_BEGIN_X;
    int random_lane = rand() % NUM_LANE_SIZE;
    pos_y = LANE_Y[random_lane];
}

int main( int argc, char* args[] ) {
	if( !init() ) {
		printf( "Failed to initialize!\n" );
	} else {
		if( !loadMedia() ){
			printf( "Failed to load media!\n" );
		}
		startScene scene;
		if(!scene.loadMedia()){
            printf( "Failed to load startScenen!\n");
		}
        endScene eScene;
        if(!eScene.loadMedia()) {
            printf( "Failed to load endScenen!\n");
        }
        else {
			bool quit = false;
			SDL_Event e;
			int scrolling = 0;
			bool gameStarted = false;
			bool gameOver = false;
			while( !quit ) {
				while( SDL_PollEvent( &e ) != 0 ) {
					if( e.type == SDL_QUIT ){
						quit = true;
					}
                    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
                    if (currentKeyStates[SDL_SCANCODE_UP] ) {
                        if(characterY_first - shift_lane_up >= 120) {
                            characterY_first -= shift_lane_up;
                        }
                    }
                    if (currentKeyStates[SDL_SCANCODE_DOWN] ) {
                        if(characterY_first + shift_lane_down <= 400) {
                            characterY_first += shift_lane_down;
                        }
                    }
                    if (currentKeyStates[SDL_SCANCODE_LEFT] ) {
                        if(characterX_first - shift_inLane_left > 0) {
                            characterX_first -= shift_inLane_left;
                        }
                    }
                    if (currentKeyStates[SDL_SCANCODE_RIGHT] ) {
                        if(characterX_first + shift_inLane_right < 720 ) {
                            characterX_first += shift_inLane_right;
                        }
                    }
                }
                SDL_RenderClear( gRenderer );
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                // - logic game - //
                if(!gameStarted) {
                    scene.render(0, 0);
                    SDL_RenderPresent( gRenderer );
                    Mix_PlayChannel(-1, gGS, 0);
                    SDL_PollEvent(&e);
                    if(scene.handleEventMouseButton(e)) {
                        gameStarted = true;
                        Mix_HaltChannel(-1);
                    }
                } else if(gameOver) {
                    eScene.render(0, 0);
                    SDL_RenderPresent( gRenderer );
                    Mix_PlayChannel(-1, gGO, 0);
                    SDL_PollEvent(&e);
                    if(eScene.handleEventMouseButton(e)) {
                        gameStarted = false;
                        gameOver = false;
                        Mix_HaltChannel(-1);
                        player_main_car.resetPosition(0, 240);
                        for(int i = 0; i < MAX_SIZE_OP_CAR; i++) {
                            op_car[i].resetPosition(); //
                        }
                    }
                } else {
                    for(int i = 0; i < MAX_SIZE_OP_CAR; i++) {
                        op_car[i].update();
                    }
                    --scrolling;
                    if( scrolling < -gBackground.getWidth() ) {
                        scrolling = 1;
                    }
                    SDL_RenderClear( gRenderer );
                    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                    gBackground.render( scrolling, 0 );
                    gBackground.render( scrolling + gBackground.getWidth(), 0 );
                    player_main_car.render(characterX_first, characterY_first);
                    Mix_PlayChannel(-1, gGP, 0);
                    //modify the code to render the opposite car
                    for(int i = 0; i < MAX_SIZE_OP_CAR; i++) {
                        op_car[i].render();
                    }
                    //collision part is added in 22/03/2024
                    for (int i = 0; i < MAX_SIZE_OP_CAR; i++) {
                        if (collisionMain(op_car[i])) {
                            car_crashed = true;
                            crashX = characterX_first;
                            crashY = characterY_first;
                            player_main_car_crash.render(crashX, crashY);
                            break;
                        }
                    }
                    if(car_crashed) {
                        SDL_RenderPresent( gRenderer );
                        SDL_Delay(2000);
                        eScene.render(0, 0);
                        SDL_RenderPresent( gRenderer );
                        Mix_HaltChannel(-1);
                        Mix_PlayChannel(-1, gCC, 0);
                        gameOver = true;
                    }
                    car_crashed = false;
                    SDL_RenderPresent( gRenderer );
                    // ~ end logic ~ //
                }
            }
        }
	}
	close();
	return 0;
}
//END_GAME 681
