#include "Player.h"
#include "Main.h"
#include <nch/cpp-utils/timer.h>
#include "TileImg.h"

Player::Player(){}
Player::Player(int x, int y, bool isAIControlled)
{
	reset(x, y);
	if(isAIControlled) {
		id = AI_PLAYER;
	} else {
		id = PLAYER1;
	}
}
Player::Player(int x, int y): Player(x, y, false)
{}

void Player::reset(int x, int y, Player p)
{
	cooldown = -60;
	this->x = x;
	this->y = y;
	
	std::srand(nch::Timer::getTicks()+std::rand());
	tiles.clear();
	for(int i = 0; i<4; i++) {
		tiles.push_back(p.getTile(i));
	}
}

void Player::reset(int x, int y)
{
	Player p;
	p.setX(x); p.setY(y);
	p.setTilesRandom();
	reset(x, y, p);
}

void Player::tick()
{
	cooldown++;	
}

void Player::drawQueuePiece(SDL_Renderer* rend, Skin* skin, double x, double y)
{
	SDL_Rect bgR = Main::getBGRect();
	double scale = skin->getGridScale()*0.90;

	int dx[] = {0, 1, 0, 1};
	int dy[] = {0, 0, 1, 1};

	for(int i = 0; i<4; i++) {
		if(tiles[i].type!=0) {
			SDL_Rect squareR;
			squareR.x = (bgR.x+bgR.w*(1.5/16.))+(x+dx[i])*32.*scale; squareR.w = 32.*scale*1.025;
			squareR.y = (bgR.y+bgR.h*(2.4/16.))+(2.25+y+dy[i])*32.*scale; squareR.h = 32.*scale*1.025;


			SDL_Texture* tex = skin->getTileTexByType(tiles[i].type);
			if(tex!=nullptr) {
				SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
				SDL_RenderCopy(rend, tex, NULL, &squareR);
			}

			if(tiles[i].chainStart) {
				TileImg::drawChainTex(rend, squareR, scale);
			}
		}
	}
}

void Player::draw(SDL_Renderer* rend, Skin* skin, double tx, double ty)
{
	std::vector<int> asdf;
	TileImg::drawTile(rend, skin, tx+0, ty+0, tiles[0]);
	TileImg::drawTile(rend, skin, tx+1, ty+0, tiles[1]);
	TileImg::drawTile(rend, skin, tx+0, ty+1, tiles[2]);
	TileImg::drawTile(rend, skin, tx+1, ty+1, tiles[3]);
}

void Player::draw(SDL_Renderer* rend, Skin* skin)
{
	draw(rend, skin, x, y);
}

int Player::getX() { return x; }
int Player::getY() { return y; } 

Tile Player::getTile(int index)
{
	if(index<0 || index>=4) return Tile();
	return tiles[index];
}
bool Player::areAllTilesSame()
{
	int seenType = getTile(0).type;
	for(int i = 1; i<4; i++) {
		if(getTile(i).type!=seenType) {
			return false;
		}
	}
	return true;
}
int Player::getCooldown() { return cooldown; }
bool Player::isAIControlled() { return id==AI_PLAYER; }
bool Player::isMainPlayer() { return id==PLAYER1; }
uint64_t Player::getLastFallMS() { return lastFallMS; }

void Player::setX(int x) { Player::x = x; }
void Player::setY(int y) { Player::y = y; }
void Player::rotate(bool clockwise)
{
	auto tilesOrig = tiles;

	if(clockwise) {
		tiles[0] = tilesOrig[2];
		tiles[1] = tilesOrig[0];
		tiles[2] = tilesOrig[3];
		tiles[3] = tilesOrig[1];
	} else {
		tiles[0] = tilesOrig[1];
		tiles[1] = tilesOrig[3];
		tiles[2] = tilesOrig[0];
		tiles[3] = tilesOrig[2];
	}
}

void Player::setTilesRandom()
{
	//Which of the 4 tiles should be a chain tile? If -1, none.
	int chainTile = -1;
	if(std::rand()%100==0) {
		chainTile = std::rand()%4;
	}

	int col = std::rand()%2+1;
	for(int i = 0; i<4; i++) {
		if(true) {
			tiles.push_back(Tile( std::rand()%2+1 ));
		} else {
			tiles.push_back(Tile( col ));
		}
	}

	//Modify one of the 'tiles' to be a chain tile
	if(chainTile!=-1) {
		tiles[chainTile].chainStart = true;
	}
}

void Player::updateLastFallMS(uint64_t ingameTimeMS)
{
	lastFallMS = ingameTimeMS;
}