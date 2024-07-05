#include <SDL.h>
#include <SDL_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "png.hpp"

#include <chrono>
#include <iostream>
#include <filesystem>

using namespace std::chrono_literals;

int main(int argc, char* argv[]) {
  FT_Library library;
  if ( FT_Init_FreeType( &library ) ) {
    std::cerr << "Could not initialize FreeType library" << std::endl;
    return 1;
  }

  const char* font_path = "KAISG.ttf";
  unsigned wa = 0;
  unsigned h = 0;
  unsigned w[7] = { 0 };

  FT_Face face[7];
  const char name[] { "vastina" };
  FT_Bitmap* bitmaps[7];
  for ( int i = 0; i < 7; i++ ) {
    if ( FT_New_Face( library, font_path, 0, &face[i] ) ) {
      std::cerr << "Could not open font" << std::endl;
      FT_Done_FreeType( library );
      return 1;
    }
    FT_Set_Char_Size( face[i], 0, 48 * 64, 0, 0 );
    if ( FT_Load_Char( face[i], name[i], FT_LOAD_RENDER ) ) {
      std::cerr << "Could not load character" << name[i] << std::endl;
      FT_Done_Face( face[i] );
      FT_Done_FreeType( library );
      return 1;
    }
    bitmaps[i] = &face[i]->glyph->bitmap;
    wa += bitmaps[i]->width;
    if ( i > 0 )
      w[i] = w[i - 1] + bitmaps[i]->width;
    h = h > bitmaps[i]->rows ? h : bitmaps[i]->rows;
  }
  // 初始化 SDL
  if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  // 创建窗口
  const unsigned ww = wa * 8;
  const unsigned hh = h * 8;
  SDL_Window* window = SDL_CreateWindow(
    "SDL2 Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ww, hh, SDL_WINDOW_SHOWN );
  if ( window == nullptr ) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  // 创建渲染器
  SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
  if ( renderer == nullptr ) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow( window );
    SDL_Quit();
    return 1;
  }

  // 初始化 SDL_image
  if ( !( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) ) {
    std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();
    return 1;
  }

  // 主循环
  bool quit = false;
  SDL_Event e;
  
  unsigned r = 5;
  auto start { std::chrono::system_clock::now() };
  const std::string filename { "./test/window.png" };
  vastina::png::png p { filename, ww, hh };
  std::filesystem::create_directories( "./test" );
  while ( !quit ) {  
    std::filesystem::remove( filename );
    const unsigned xoffset = ww / 10;
    const unsigned yoffset = hh / 2;
    for ( int i = 0; i < 7; i++ ) {
      for ( unsigned y = h - bitmaps[i]->rows, yi = 0; y < h; y++, yi++ ) {
        for ( unsigned x = 0; x < bitmaps[i]->width; x++ ) {
          auto val { bitmaps[i]->buffer[yi * bitmaps[i]->width + x] };
          p.setIndex( x + w[i] + xoffset, y + yoffset, { val, val, val } );
        }
      }
    }
    r = ( r + 1 ) % hh;
    r = r == 0 ? 5 : r;
    unsigned rr1 = ( r - 1 ) * ( r - 1 ) - r*2;
    unsigned rr2 = ( r - 1 ) * ( r - 1 ) + r*2;
    for ( unsigned y = 0; y < p.height; y++ ) {
      for ( unsigned x = 0; x < p.width; x++ ) {
        unsigned rr = x * x + y * y;
        if ( rr > rr1 && rr < rr2 )
          p.setIndex( x, y, { 0, 0, 0xff } );
      }
    }
    p.write();
    SDL_Surface* loadedSurface = IMG_Load( filename.data() );
    if ( loadedSurface == nullptr ) {
      std::cerr << "Unable to load image! SDL_image Error: " << IMG_GetError() << std::endl;
      IMG_Quit();
      SDL_DestroyRenderer( renderer );
      SDL_DestroyWindow( window );
      SDL_Quit();
      return 1;
    }
    // 创建纹理
    SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
    SDL_FreeSurface( loadedSurface );
    if ( texture == nullptr ) {
      std::cerr << "Unable to create texture from image! SDL Error: " << SDL_GetError() << std::endl;
      IMG_Quit();
      SDL_DestroyRenderer( renderer );
      SDL_DestroyWindow( window );
      SDL_Quit();
      return 1;
    }

    while ( SDL_PollEvent( &e ) != 0 ) {
      if ( e.type == SDL_QUIT ) {
        quit = true;
      }
    }

    // 清屏
    SDL_RenderClear( renderer );
    // 绘制纹理
    SDL_RenderCopy( renderer, texture, nullptr, nullptr );
    // 更新屏幕
    SDL_RenderPresent( renderer );

    auto now { std::chrono::system_clock::now() };
    while ( now - start < 5ms )
      now = std::chrono::system_clock::now();
    start = now;

    std::fill(p.data.begin(), p.data.end(), 0);
  }

  // 释放资源并关闭 SDL
  IMG_Quit();
  SDL_DestroyRenderer( renderer );
  SDL_DestroyWindow( window );
  SDL_Quit();
  for( int i = 0; i < 7; i++ )
    FT_Done_Face( face[i] );
  FT_Done_FreeType( library );

  return 0;
}
