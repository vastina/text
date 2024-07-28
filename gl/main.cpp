#include "util.hpp"
#include <iostream>
#include <array>
#include <chrono>

using namespace vas;

constexpr i32 WinWidth = 1000;
constexpr i32 WinHeight = 1000;

void setupProjectionMatrix()
{
  glMatrixMode( GL_PROJECTION );                                   // 选择投影矩阵
  glLoadIdentity();                                                // 重置投影矩阵
  gluPerspective( 45.0, WinWidth / (float)WinHeight, 1.0, 100.0 ); // 设置透视投影
}

void setupModelViewMatrix( const glm::vec3& cameraPos )
{
  glMatrixMode( GL_MODELVIEW ); // 选择模型视图矩阵
  glLoadIdentity();             // 重置模型视图矩阵
  gluLookAt( cameraPos.x,
             cameraPos.y,
             cameraPos.z, // 观察者位置
             0.0,
             0.0,
             0.0, // 观察目标点
             0.0,
             1.0,
             0.0 ); // 上方向
}

int main( int argc, char* argv[] )
{
  u32 WindowFlags = SDL_WINDOW_OPENGL;

  SDL_Window* Window
    = SDL_CreateWindow( argc > 0 ? argv[1] : argv[0], 100, 50, WinWidth, WinHeight, WindowFlags );
  if ( nullptr == Window ) {
    std::cerr << "Failed to create window" << std::endl;
    return 1;
  }
  SDL_GLContext Context = SDL_GL_CreateContext( Window );
  if ( nullptr == Context ) {
    std::cerr << "Failed to create OpenGL context" << std::endl;
    return 1;
  }

  glViewport( 0, 0, WinWidth, WinHeight );

  gl::TexCoord apng;
  gl::TexCoord container;
  gl::Cube cube;
  {
    apng.loadTexture( "./static/pictures/a.png" );
    apng.setQuad( glm::vec3 { 0.5f, 0.5f, 0.f }, 0.25f );
    apng.setTexCoord();
    for ( auto& p : apng.points )
      p->color = { 1.f, 1.f, 1.f, 1.f };

    container.loadTexture( "./static/pictures/container2.png" );
    container.setQuad( glm::vec3 { -0.5f, -0.5f, 0.f }, 0.25f );
    container.setTexCoord();
    for ( auto& p : container.points )
      p->color = { 1.f, 1.f, 1.f, 1.f };

    for ( auto& f : cube.faces ) {
      // f = (new gl::TexCoord())->setTexCoord();
      auto* p = new gl::TexCoord();
      p->setTexCoord();
      p->loadTexture( "./static/pictures/container1.jpg" );
      f = p;
    }
    cube.setCube( { -1.f, 0.f, 0.f }, 0.5f );
    srand( static_cast<u32>(std::chrono::system_clock::now().time_since_epoch().count()) );
    constexpr auto genColor = []() -> glm::vec4 {
      return { (rand() % 256) / 255.f, (rand() % 256) / 255.f, (rand() % 256) / 255.f, 1.f };
    };
    for ( auto& f : cube.faces ) {
      for ( auto& p : f->points ) {
        p->color = genColor();
      }
    }
  }

  glm::vec3 camera { -2.0f, 0.0f, 5.0f };
  glm::vec3 xaxis { 1.0f, 0.0f, 0.0f };
  glm::vec3 zaxis { 0.0f, 0.0f, -1.0f };
  float speed = 0.1f;
  float lastX = WinWidth / 2.f;
  float lastY = WinHeight / 2.f;
  float yaw = -90.0f;
  float pitch = 0.0f;
  // float fov = 45.0f;
  // todo...

  i32 Running = 1;
  i32 FullScreen = 0;
  SDL_Event Event;
  while ( Running ) {
    while ( SDL_PollEvent( &Event ) ) {
      if ( Event.type == SDL_KEYDOWN ) {
        switch ( Event.key.keysym.sym ) {
          case SDLK_ESCAPE:
            Running = 0;
            break;
          case SDLK_UP:
          case 'w':
            camera += zaxis * speed;
            break;
          case SDLK_DOWN:
          case 's':
            camera -= zaxis * speed;
            break;
          case SDLK_LEFT:
          case 'a':
            camera -= xaxis * speed;
            break;
          case SDLK_RIGHT:
          case 'd':
            camera += xaxis * speed;
            break;
          case 'f':
            std::cout << Event.key.keysym.sym << std::endl;
            FullScreen = !FullScreen;
            if ( FullScreen ) {
              SDL_SetWindowFullscreen( Window, WindowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP );
            } else {
              SDL_SetWindowFullscreen( Window, WindowFlags );
            }
            break;
          default:
            break;
        }
      } else if ( Event.type == SDL_MOUSEMOTION ) {
        float xNow = static_cast<float>( Event.button.x );
        float yNow = static_cast<float>( Event.button.y );
        constexpr float sensitivity = 0.05f;
        float dyaw = ( xNow - lastX ) * sensitivity;
        float dpitch = ( yNow - lastY ) * sensitivity;
        yaw += dyaw;
        pitch += dpitch;
        // xaxis = glm::vec3();
        // pitch = std::min( 89.f, std::max( -89.f, pitch ) );
        // zaxis = glm::vec3();
        // todo...
        lastX = xNow;
        lastY = yNow;
      } else if ( Event.type == SDL_QUIT ) {
        Running = 0;
      }
    }
    glClearColor( 0.30f, 0.15f, 0.65f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    setupProjectionMatrix();
    setupModelViewMatrix( camera );
    
    apng.draw();
    container.draw();
    cube.draw();

    SDL_GL_SwapWindow( Window );
  }

  SDL_GL_DeleteContext( Context );
  SDL_DestroyWindow( Window );
  return 0;
}