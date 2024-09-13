#include "GridImg.h"
#include "Main.h"

void GridImg::buildGridTex(SDL_Renderer* rend, SDL_Texture*& gridTex, int gridCols, int gridRows)
{
	gridTex = SDL_CreateTexture(rend, Main::getWindowPixelFormat()->format, SDL_TEXTUREACCESS_TARGET, 64*gridCols, 64*gridRows);
	SDL_SetTextureBlendMode(gridTex, SDL_BLENDMODE_BLEND);
	
	//Grid (giant gray rectangle)
	SDL_SetRenderTarget(rend, gridTex);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
	SDL_RenderFillRect(rend, NULL);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

	//Gray rectangle behind grid
	SDL_SetRenderDrawColor(rend, 25, 25, 25, 140);
	SDL_RenderFillRect(rend, NULL);
	
	//Grid squares
	for( int x = 0; x<gridCols; x++ ) {
		for( int y = 0; y<gridRows; y++ ) {
			int xl = x*64;
			int yl = y*64;

			//Vertical lines
            SDL_SetRenderDrawColor(rend, 25, 25, 25, 200);
			SDL_RenderDrawLine(rend, xl+00, yl+00, xl+00, yl+64);
			SDL_RenderDrawLine(rend, xl+63, yl+00, xl+63, yl+64);
            SDL_SetRenderDrawColor(rend, 15, 15, 15, 225);
			SDL_RenderDrawLine(rend, xl+01, yl+00, xl+01, yl+64);
            SDL_SetRenderDrawColor(rend, 35, 35, 35, 225);
			SDL_RenderDrawLine(rend, xl+62, yl+00, xl+62, yl+64);

			//Horizontal lines
            SDL_SetRenderDrawColor(rend, 25, 25, 25, 200);
			SDL_RenderDrawLine(rend, xl+00, yl+00, xl+64, yl+00);
			SDL_RenderDrawLine(rend, xl+00, yl+63, xl+64, yl+63);
            SDL_SetRenderDrawColor(rend, 15, 15, 15, 225);
			SDL_RenderDrawLine(rend, xl+00, yl+01, xl+64, yl+01);
            SDL_SetRenderDrawColor(rend, 35, 35, 35, 225);
			SDL_RenderDrawLine(rend, xl+00, yl+62, xl+64, yl+62);

            //Small rectangles
            SDL_SetRenderDrawColor(rend, 25, 25, 25, 200);
            SDL_Rect r; r.w = 6; r.h = 6;
            r.x = xl; 	 r.y = yl;    SDL_RenderFillRect(rend, &r);
            r.x = xl+58; r.y = yl;    SDL_RenderFillRect(rend, &r);
            r.x = xl;    r.y = yl+58; SDL_RenderFillRect(rend, &r);
            r.x = xl+58; r.y = yl+58; SDL_RenderFillRect(rend, &r);    
		}
	}

	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	SDL_SetRenderTarget(rend, NULL);
}