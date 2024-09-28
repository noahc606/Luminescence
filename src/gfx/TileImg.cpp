#include "TileImg.h"
#include <iostream>
#include <nch/sdl-utils/texture-utils.h>
#include <nch/sdl-utils/timer.h>
#include "Color.h"
#include "TileGrid.h"
#include "Main.h"
#include "Resources.h"

void TileImg::drawChainTex(SDL_Renderer* rend, SDL_Rect tile, double gridScale)
{
	double s = gridScale;

	uint64_t t = nch::Timer::getTicks64()/10;

	nch::Color c1 = nch::Color(255-(t%64)*2, 255-(t%64)*2, 255-(t%64)*2);	//Light bezel (top left)
	nch::Color c2 = nch::Color(0, 64-(t%64), 64-(t%64));					//Dark bezel (bottom right)
	nch::Color c3 = nch::Color(0, 0, 64+(t%64)*3);						//Base


	SDL_Rect cpr;

	//Top rectangle
	cpr.x = tile.x+14.*s;	cpr.w = 4.*s;
	cpr.y = tile.y+3.*s;	cpr.h = 10.*s;
	cpr.x -= std::ceil(1*s); cpr.y -= std::ceil(1*s);	SDL_SetRenderDrawColor(rend, c1.r, c1.g, c1.b, c1.a); SDL_RenderFillRect(rend, &cpr);
	cpr.x += std::ceil(2*s); cpr.y += std::ceil(2*s);	SDL_SetRenderDrawColor(rend, c2.r, c2.g, c2.b, c2.a); SDL_RenderFillRect(rend, &cpr);
	cpr.x -= std::ceil(1*s); cpr.y -= std::ceil(1*s);	SDL_SetRenderDrawColor(rend, c3.r, c3.g, c3.b, c3.a); SDL_RenderFillRect(rend, &cpr);

	//Lower rectangle
	cpr.x = tile.x+14.*s;	cpr.w = 4.*s;
	cpr.y = tile.y+19.*s;	cpr.h = 10.*s;
	cpr.x -= std::ceil(1*s); cpr.y -= std::ceil(1*s);	SDL_SetRenderDrawColor(rend, c1.r, c1.g, c1.b, c1.a); SDL_RenderFillRect(rend, &cpr);
	cpr.x += std::ceil(2*s); cpr.y += std::ceil(2*s);	SDL_SetRenderDrawColor(rend, c2.r, c2.g, c2.b, c2.a); SDL_RenderFillRect(rend, &cpr);
	cpr.x -= std::ceil(1*s); cpr.y -= std::ceil(1*s);	SDL_SetRenderDrawColor(rend, c3.r, c3.g, c3.b, c3.a); SDL_RenderFillRect(rend, &cpr);

	//Left rectangle
	cpr.x = tile.x+3.*s;	cpr.w = 10.*s;
	cpr.y = tile.y+14.*s;	cpr.h = 4.*s;
	cpr.x -= std::ceil(1*s); cpr.y -= std::ceil(1*s);	SDL_SetRenderDrawColor(rend, c1.r, c1.g, c1.b, c1.a); SDL_RenderFillRect(rend, &cpr);
	cpr.x += std::ceil(2*s); cpr.y += std::ceil(2*s);	SDL_SetRenderDrawColor(rend, c2.r, c2.g, c2.b, c2.a); SDL_RenderFillRect(rend, &cpr);
	cpr.x -= std::ceil(1*s); cpr.y -= std::ceil(1*s);	SDL_SetRenderDrawColor(rend, c3.r, c3.g, c3.b, c3.a); SDL_RenderFillRect(rend, &cpr);

	//Right rectangle
	cpr.x = tile.x+19.*s;	cpr.w = 10.*s;
	cpr.y = tile.y+14.*s;	cpr.h = 4.*s;
	cpr.x -= std::ceil(1*s); cpr.y -= std::ceil(1*s);	SDL_SetRenderDrawColor(rend, c1.r, c1.g, c1.b, c1.a); SDL_RenderFillRect(rend, &cpr);
	cpr.x += std::ceil(2*s); cpr.y += std::ceil(2*s);	SDL_SetRenderDrawColor(rend, c2.r, c2.g, c2.b, c2.a); SDL_RenderFillRect(rend, &cpr);
	cpr.x -= std::ceil(1*s); cpr.y -= std::ceil(1*s);	SDL_SetRenderDrawColor(rend, c3.r, c3.g, c3.b, c3.a); SDL_RenderFillRect(rend, &cpr);
}

void TileImg::buildSquareTex(SDL_Renderer* rend, SDL_Texture*& tex, Skin* skin, int type)
{
	//Store last target
	SDL_Texture* lastTarget = SDL_GetRenderTarget(rend);

	//Build and draw square texture (tex should be 256x256 and a TARGET texture)
	tex = SDL_CreateTexture(rend, Main::getWindowPixelFormat()->format, SDL_TEXTUREACCESS_TARGET, 256, 256);
	SDL_SetRenderTarget(rend, tex);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
	SDL_RenderFillRect(rend, NULL);

	drawSquare(rend, 4, 0, 0, skin->getColorFromTileType(type));

	//Switch render target to last target
	SDL_SetRenderTarget(rend, lastTarget);
}

void TileImg::buildTileTex(SDL_Renderer* rend, SDL_Texture*& tex, std::string skinPD, std::string skinID, int type)
{
	//Store last target
	SDL_Texture* lastTarget = SDL_GetRenderTarget(rend);
	SDL_BlendMode lastBM; SDL_GetRenderDrawBlendMode(rend, &lastBM);

	//Build and draw tile texture (tex should be 128x128 and a TARGET texture)
	tex = SDL_CreateTexture(rend, Main::getWindowPixelFormat()->format, SDL_TEXTUREACCESS_TARGET, 128, 128);
	nch::TexUtils::clearTexture(rend, tex);

	//Draw tile
	SDL_Texture* tileTex = nullptr;
	SDL_Rect tileR;
	tileR.x = 4;	tileR.w = 120;
	tileR.y = 4;	tileR.h = 120;
	
	if(type%2==1) {
		tileTex = Resources::getTex(skinPD+"/"+skinID+"/tile_a");
		if(tileTex==nullptr) {
			tileTex = Resources::getTex("res/skins/default_skin/tile_a");
		}
	} else {
		tileTex = Resources::getTex(skinPD+"/"+skinID+"/tile_b");
		if(tileTex==nullptr) {
			tileTex = Resources::getTex("res/skins/default_skin/tile_b");
		}
	}

	SDL_SetRenderTarget(rend, tex);
	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
	SDL_RenderCopy(rend, tileTex, NULL, &tileR);

	//Switch render target to last target
	SDL_SetRenderTarget(rend, lastTarget);
	SDL_SetRenderDrawBlendMode(rend, lastBM);
}
void TileImg::buildTileTex(SDL_Renderer* rend, SDL_Texture*& tex, Skin* skin, int type)
{
	buildTileTex(rend, tex, skin->getParentDir(), skin->getID(), type);
}

void TileImg::drawSquare(SDL_Renderer* rend, double scale, double dstX, double dstY, nch::Color c)
{
	double s = scale;

	//Main rectangle (outline)
	SDL_Rect mr0;
	mr0.x = dstX;				mr0.y = dstY;
	mr0.w = 64*s;				mr0.h = 64*s;
	nch::Color mr0c = c; mr0c.brighten(30);
	SDL_SetRenderDrawColor(rend, mr0c.r, mr0c.g, mr0c.b, 255);
	SDL_RenderFillRect(rend, &mr0);

	//Main rectangle (inner): Light inner bezels
	SDL_Rect mr1;
	mr1.x = mr0.x+2*s;			mr1.y = mr0.y+2*s;
	mr1.w = mr0.w-4*s;			mr1.h = mr0.h-4*s;
	nch::Color mr1c = c; mr1c.brighten(0);
	SDL_SetRenderDrawColor(rend, mr1c.r, mr1c.g, mr1c.b, 255);
	SDL_RenderFillRect(rend, &mr1);

	//Four inner squares
	for(int ix = 0; ix<2; ix++)
	for(int iy = 0; iy<2; iy++) {
		//Darker inner square: Darker inner bezels
		SDL_Rect dis;
		dis.x = mr1.x+(1+30*ix)*s;
		dis.y = mr1.y+(1+30*iy)*s;
		dis.w = 29*s;
		dis.h = 29*s;
		nch::Color disc = c; disc.brighten(-15);
		SDL_SetRenderDrawColor(rend, disc.r, disc.g, disc.b, 255);
		SDL_RenderFillRect(rend, &dis);

		//Lighter inner square: Neutral base
		SDL_Rect lis;
		lis.x = mr1.x+(1+30*ix)*s;
		lis.y = mr1.y+(1+30*iy)*s;
		lis.w = 28*s;
		lis.h = 28*s;
		nch::Color lisc = c; lisc.brighten(-10);
		drawGradientSquare(rend, &lis, lisc);
	}
}

void TileImg::drawSquare(SDL_Renderer* rend, Skin* skin, double gridX, double gridY, int type)
{
	SDL_Rect* gridR = skin->getGridRect();
	double s = skin->getGridScale();

	//Square params
	SDL_Rect sq;
	sq.x = gridR->x+gridX*32*s;
	sq.y = gridR->y+gridY*32*s;
	sq.w = 64*s;
	sq.h = 64*s;
	
	SDL_Texture* sqTex = skin->getSquareTexByType(type);
	if(sqTex!=nullptr) {
		SDL_RenderCopy(rend, sqTex, NULL, &sq);
	}
}

void TileImg::drawTile(SDL_Renderer* rend, Skin* skin, double gridX, double gridY, Tile t)
{
	SDL_Rect* gridR = skin->getGridRect();
	double scale = skin->getGridScale();

	bool drawNothing = false;
	if(t.type==0) {
		drawNothing = true;
	}

	SDL_Rect squareR;
	squareR.x = gridR->x+gridX*32*scale;	squareR.w = 32.*scale;
	squareR.y = gridR->y+gridY*32*scale;	squareR.h = 32.*scale;
	if(!drawNothing) {		
		if(t.complete) {
			drawSquare(rend, skin, gridX, gridY, t.type);
		} else {
			SDL_Texture* tex = skin->getTileTexByType(t.type);
			if(tex!=nullptr) {
				SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
				SDL_RenderCopy(rend, tex, NULL, &squareR);
			}

			if(t.chainStart) {
				drawChainTex(rend, squareR, scale);
			}
			if(t.chainPart) {
				uint64_t t = (nch::Timer::getTicks64()/10)%64;
				SDL_SetRenderDrawColor(rend, 0, 255-t, 255-t, 64-t/2);
				SDL_RenderFillRect(rend, &squareR);
			}
		}

		if(t.faded) {
			nch::Color c(15, 15, 15);
			SDL_SetRenderDrawColor(rend, c.r, c.g, c.b, c.a);
			SDL_RenderFillRect(rend, &squareR);

			Resources::getTexMakeException();
			SDL_Texture* delBlockTex = Resources::getTex("data/misc/delete_blk01");
			if(delBlockTex!=nullptr) {
				squareR.x += 1*scale; squareR.y += 1*scale;
				squareR.w -= 2*scale; squareR.h -= 2*scale;
				
				SDL_Rect src;
				src.x = 0; src.y = 0;
				src.w = 136; src.h = 136;
				SDL_RenderCopy(rend, delBlockTex, &src, &squareR);
			}


		}
	}
}

nch::Color TileImg::getTileCompositeColor(SDL_Renderer* rend, std::string tileAsset)
{
    SDL_Texture* tex = Resources::getTex(tileAsset);
    if(tex!=nullptr) {
        //Get rect, pitch of tex
		int width = 0, height = 0;
		SDL_QueryTexture(tex, NULL, NULL, &width, &height);
        SDL_Rect r; r.x = 0; r.y = 0; r.w = width; r.h = height;
        int pitch = SDL_BYTESPERPIXEL(Main::getWindowPixelFormat()->format); //Should be 4

		//Set render target to tex and read its pixels into 'pixels'
		SDL_Texture* oldTarget = SDL_GetRenderTarget(rend);
        SDL_SetRenderTarget(rend, tex);
        uint32_t* pixels = (uint32_t*)std::malloc(width*height*sizeof(*pixels));
		
		//If reading of pixels successful...
        if(SDL_RenderReadPixels(rend, &r, Main::getWindowPixelFormat()->format, pixels, width*pitch)==0) {
			//Calculate average RGB of all non-transparent pixels
			uint64_t avgR=0, avgG=0, avgB=0;
			uint64_t divisor = 0;
			for(int px = 0; px<width; px++) {
				for(int py = 0; py<height; py++) {
					nch::Color c(pixels[py*width+px]);
					if(c.a==0) {
						continue;
					} else {
						avgR += c.r;
						avgG += c.g;
						avgB += c.b;
						divisor++;
					}
				}
			}
			if(divisor==0) divisor = 1;
			avgR = avgR/divisor;
			avgG = avgG/divisor;
			avgB = avgB/divisor;
			
			//Set render target back, free pixel data, return result.
			SDL_SetRenderTarget(rend, oldTarget);
			std::free(pixels);
			return nch::Color(avgR, avgG, avgB);
		} else {
			printf("Failed to SDL_RenderReadPixels(): %s\n", SDL_GetError());
		}
    } else {
		printf("tex is nullptr!\n");
	}
	
	return nch::Color(0, 0, 0);
}

void TileImg::drawGradientSquare(SDL_Renderer* rend, SDL_Rect* r, nch::Color c)
{
	double v = c.getHSV()[2];
	for(double iy = r->y; iy<r->y+r->h; iy++) {
		//Gradient base color
		nch::Color gbc = c;

		double brightness = -( (iy-r->y)/(r->h) )*25.;
		gbc.setBrightness(v+12+brightness);
		SDL_SetRenderDrawColor(rend, gbc.r, gbc.g, gbc.b, 255);
		SDL_RenderDrawLine(rend, r->x, iy, r->x+r->w, iy);
	}
}