#ifndef _SDL_CXX_H_
#define _SDL_CXX_H_

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>

#include "vasdef.hpp"

namespace vas {

class img_player
{
private:
  SDL_Window* wd { nullptr };
  u32 width {};
  u32 height {};
  SDL_Renderer* renderer { nullptr };
  bool quit { false };

public:
  img_player( const char* title, u32 w = 800, u32 h = 600 ) : width( w ), height( h )
  {
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
      std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
      throw std::runtime_error( "sdl" );
    }
    wd = SDL_CreateWindow( title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN );
    if ( wd == nullptr ) {
      std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
      SDL_Quit();
      throw std::runtime_error( "sdl" );
    }
    renderer = SDL_CreateRenderer( wd, -1, SDL_RENDERER_ACCELERATED );
    if ( renderer == nullptr ) {
      std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
      SDL_DestroyWindow( wd );
      SDL_Quit();
      throw std::runtime_error( "sdl" );
    }
    if ( !( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) ) {
      std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
      SDL_DestroyRenderer( renderer );
      SDL_DestroyWindow( wd );
      SDL_Quit();
      throw std::runtime_error( "sdl" );
    }
  }
  ~img_player()
  {
    IMG_Quit();
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( wd );
    SDL_Quit();
  }

  void LoadImg( const char* filename )
  {
    SDL_Surface* loadedSurface = IMG_Load( filename );
    if ( loadedSurface == nullptr ) {
      std::cerr << "Unable to load image! SDL_image Error: " << IMG_GetError() << std::endl;
      IMG_Quit();
      SDL_DestroyRenderer( renderer );
      SDL_DestroyWindow( wd );
      SDL_Quit();
      throw std::runtime_error( "sdl_img" );
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
    if ( texture == nullptr ) {
      std::cerr << "Unable to create texture from image! SDL Error: " << SDL_GetError() << std::endl;
      IMG_Quit();
      SDL_DestroyRenderer( renderer );
      SDL_DestroyWindow( wd );
      SDL_Quit();
      throw std::runtime_error( "sdl_img" );
    }
    SDL_RenderCopy( renderer, texture, nullptr, nullptr );
    SDL_RenderPresent( renderer );

    SDL_FreeSurface( loadedSurface );
    SDL_DestroyTexture( texture );
  }
  void Clear() { SDL_RenderClear( renderer ); }
  // todo
  void addhandle() {}
  void HandleEvent()
  {
    SDL_Event e;
    while ( SDL_PollEvent( &e ) != 0 ) {
      if ( e.type == SDL_QUIT ) {
        quit = true;
      }
    }
  }
  inline bool ShouldQuit() const { return quit; }
};

}

#endif