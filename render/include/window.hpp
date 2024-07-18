#ifndef _SDL_CXX_H_
#define _SDL_CXX_H_

#include <SDL.h>
// #include <SDL_image.h>

#include <iostream>
#include <functional>
#include <utility>

#include "vasdef.hpp"
#include "DrawBoard.hpp"

namespace vas {

class Player
{
private:
  SDL_Window* wd { nullptr };
  u32 width {};
  u32 height {};
  SDL_Renderer* renderer { nullptr };
  bool quit { false };

public:
  Player( const char* title, u32 w = 800, u32 h = 600 ) : width( w ), height( h )
  {
    wd = SDL_CreateWindow( title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN );
    if ( wd == nullptr ) {
      std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << '\n';
      throw std::runtime_error( "sdl" );
    }
    renderer = SDL_CreateRenderer( wd, -1, SDL_RENDERER_ACCELERATED );
    if ( renderer == nullptr ) {
      std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << '\n';
      SDL_DestroyWindow( wd );
      throw std::runtime_error( "sdl" );
    }
  }
  ~Player()
  {
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( wd );
  }

  void Render( const DrawBoard& b )
  {
    SDL_Texture* texture = b.pic.texture;

    SDL_UnlockTexture( texture );

    SDL_RenderClear( renderer );
    SDL_RenderCopy( renderer, texture, nullptr, nullptr );
    SDL_RenderPresent( renderer );
  }
  SDL_Texture* CreateTexture( u32 format = SDL_PIXELFORMAT_RGB888, int access = SDL_TEXTUREACCESS_STREAMING )
  {
    return SDL_CreateTexture( renderer, format, access, width, height );
  }
  // void LoadImg( const char* filename )
  // {
  //   SDL_Surface* loadedSurface = IMG_Load( filename );
  //   if ( loadedSurface == nullptr ) {
  //     std::cerr << "Unable to load image! SDL_image Error: " << IMG_GetError() << '\n';
  //     throw std::runtime_error( "sdl_img" );
  //   }
  //   SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
  //   if ( texture == nullptr ) {
  //     std::cerr << "Unable to create texture from image! SDL Error: " << SDL_GetError() << '\n';
  //     throw std::runtime_error( "sdl_img" );
  //   }
  //   SDL_RenderCopy( renderer, texture, nullptr, nullptr );
  //   SDL_RenderPresent( renderer );

  //   SDL_FreeSurface( loadedSurface );
  //   SDL_DestroyTexture( texture );
  // }
  void Clear() { SDL_RenderClear( renderer ); }
  // todo

private:
  std::unordered_map<u32, std::function<void( const SDL_Event& )>> handles {
    { SDL_QUIT, [this]( const SDL_Event& ) { quit = true; } } };

  std::vector<std::pair<std::function<bool()>, std::function<void()>>> statehandles { 0 };

public:
  void addhandle( u32 EventType, const std::function<void( const SDL_Event& )>& handle )
  {
    handles.insert_or_assign( EventType, handle );
  }
  void addStatehandle( const std::function<bool()>& InStatejudge, const std::function<void()>& handle )
  {
    statehandles.push_back( std::make_pair( InStatejudge, handle ) );
  }
  void HandleEvent()
  {
    SDL_Event e;
    while ( SDL_PollEvent( &e ) != 0 ) {
      if ( handles.contains( e.type ) ) {
        handles[e.type]( e );
      }
    }
  }
  void HandleState()
  {
    for ( const auto& [j, h] : statehandles ) {
      if ( j() )
        h();
    }
  }
  inline bool ShouldQuit() const { return quit; }
};

struct mousehandle
{
  vas::DrawBoard& b;

  bool down { false };
  bool moved_last_frame { false };
  int xfirst { 0 };
  int xcur { 0 };
  int yfirst { 0 };
  int ycur { 0 };
  void DealDown( const SDL_Event& e )
  {
    down = true;
    xfirst = e.button.x;
    yfirst = e.button.y;
    xcur = xfirst;
    ycur = yfirst;
    // if ( down ) {
    //   // error, if down, why there is another down
    // } else {
    // }
  }
  void DealUp( const SDL_Event& e )
  {
    down = false;
    if ( e.button.x < 0 ) {
      xcur = 0;
    } else if ( e.button.x > static_cast<int>( b.pic.width ) ) {
      xcur = b.pic.width;
    } else {
      xcur = e.button.x;
    }
    if ( e.button.y < 0 ) {
      ycur = 0;
    } else if ( e.button.y > static_cast<int>( b.pic.height ) ) {
      ycur = b.pic.height;
    } else {
      ycur = e.button.y;
    }
    // (int)-1 > (u32)1
    b.Clear_Coverable( xfirst, yfirst, xcur, ycur );
  }

  void DealMove( const SDL_Event& e )
  {
    if ( down ) {
      if ( e.button.x < 0 || e.button.y < 0 || static_cast<u64>( e.button.x ) > b.pic.width
           || static_cast<u64>( e.button.y ) > b.pic.height ) {
        return;
      }
      b.Clear_Coverable( xfirst, yfirst, xcur, ycur );
      xcur = e.button.x;
      ycur = e.button.y;
      b.DrawRect_Coverable( xfirst, yfirst, xcur, ycur, { 0, 0, 0xff } );
    }
    moved_last_frame = true;
  }
  void DealDownState() { b.DrawRect_Coverable( xfirst, yfirst, xcur, ycur, { 0, 0, 0xff } ); }
  void freshState() { moved_last_frame = false; }
};

}

#endif