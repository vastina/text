#ifndef _VAS_GL_BASE_H_
#define _VAS_GL_BASE_H_

#include <SDL.h>
#include <SDL_opengl.h>

#include <GL/gl.h>
#ifdef _WIN32
#include <GL/GLU.h>
#elif __linux__
#include <GL/glu.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>
#include <filesystem>

#include "vasdef.hpp"

namespace vas {
namespace gl {

struct Vertex
{
  glm::vec4 pos;
  glm::vec4 color;

  virtual void setPoint() const
  {
    glColor4f( color.r, color.g, color.b, color.a );
    glVertex4f( pos.x, pos.y, pos.z, pos.w );
  }
};

struct TexCoordPoint : public Vertex
{
  glm::vec2 tex;

  virtual void setPoint() const override
  {
    glColor4f( color.r, color.g, color.b, color.a );
    glTexCoord2f( tex.x, tex.y );
    glVertex4f( pos.x, pos.y, pos.z, pos.w );
  }
};

struct shape
{
  virtual void draw() const = 0;
  virtual ~shape() = default;
};

struct Triangle : public shape
{
  std::array<Vertex*, 3> points { new Vertex(), new Vertex(), new Vertex() };

  virtual ~Triangle() override = default;
  void draw() const override
  {
    glBegin( GL_TRIANGLES );
    for ( const auto& p : points ) {
      p->setPoint();
    }
    glEnd();
  }
  void setTriangle( const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2 )
  {
    points[0]->pos = p0;
    points[1]->pos = p1;
    points[2]->pos = p2;
  }
};

struct Quad : public shape
{
  std::array<Vertex*, 4> points { new Vertex(), new Vertex(), new Vertex(), new Vertex() };

  virtual ~Quad() override = default;
  virtual void draw() const override
  {
    glBegin( GL_QUADS );
    for ( const auto& p : points ) {
      p->setPoint();
    }
    glEnd();
  }
  void setQuad( const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2, const glm::vec4& p3 )
  {
    points[0]->pos = p0;
    points[1]->pos = p1;
    points[2]->pos = p2;
    points[3]->pos = p3;
  }
  void setQuad( glm::vec3 center, float halfSize )
  {
    points[0]->pos = { center.x - halfSize, center.y - halfSize, center.z, 1.0f };
    points[1]->pos = { center.x + halfSize, center.y - halfSize, center.z, 1.0f };
    points[2]->pos = { center.x + halfSize, center.y + halfSize, center.z, 1.0f };
    points[3]->pos = { center.x - halfSize, center.y + halfSize, center.z, 1.0f };
  }
};

struct TexCoord : public Quad
{
  TexCoord()
  {
    for ( auto& p : points ) {
      p = new TexCoordPoint();
    }
  }
  ~TexCoord() override
  {
    for ( auto& p : points )
      delete dynamic_cast<TexCoordPoint*>( p );
  }

  u32 id;
  void loadTexture( const char* filename )
  {
    if ( !std::filesystem::exists( filename ) ) {
      std::cerr << "File not exists" << std::endl;
      return;
    }
    glGenTextures( 1, &id );
    glBindTexture( GL_TEXTURE_2D, id );
    // 设置纹理参数
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    // 加载图像数据并生成纹理
    // 假设使用了stb_image库进行图像加载
    int width;
    int height;
    int nrChannels;
    unsigned char* data = stbi_load( filename, &width, &height, &nrChannels, 0 );
    if ( data ) {
      GLenum format = GL_RGB;
      if ( nrChannels == 1 )
        format = GL_RED;
      else if ( nrChannels == 3 )
        format = GL_RGB;
      else if ( nrChannels == 4 )
        format = GL_RGBA;
      glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data );
    } else {
      std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free( data );
  }
  void draw() const override
  {
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, id );
    glBegin( GL_QUADS );
    for ( const auto& p : points ) {
      p->setPoint();
    }
    glEnd();
    glDisable( GL_TEXTURE_2D );
  }
  void setTexCoord( const glm::vec2& p0 = { 0.f, 0.f },
                    const glm::vec2& p1 = { 1.f, 0.f },
                    const glm::vec2& p2 = { 1.f, 1.f },
                    const glm::vec2& p3 = { 0.f, 1.f } )
  {
    dynamic_cast<TexCoordPoint*>( points[0] )->tex = p0;
    dynamic_cast<TexCoordPoint*>( points[1] )->tex = p1;
    dynamic_cast<TexCoordPoint*>( points[2] )->tex = p2;
    dynamic_cast<TexCoordPoint*>( points[3] )->tex = p3;
    // return this;
  }
};

struct Cube : public shape
{
  std::array<Quad*, 6> faces;

  virtual ~Cube() override
  {
    for ( auto& f : faces )
      delete f;
  }
  void draw() const override
  {
    for ( const auto& f : faces ) {
      f->draw();
    }
  }
  void setCube( const glm::vec3& center, float halfSize )
  {
    faces[0]->setQuad( { center.x - halfSize, center.y - halfSize, center.z + halfSize, 1.0f },
                       { center.x + halfSize, center.y - halfSize, center.z + halfSize, 1.0f },
                       { center.x + halfSize, center.y + halfSize, center.z + halfSize, 1.0f },
                       { center.x - halfSize, center.y + halfSize, center.z + halfSize, 1.0f } );
    faces[1]->setQuad( { center.x + halfSize, center.y - halfSize, center.z + halfSize, 1.0f },
                       { center.x + halfSize, center.y - halfSize, center.z - halfSize, 1.0f },
                       { center.x + halfSize, center.y + halfSize, center.z - halfSize, 1.0f },
                       { center.x + halfSize, center.y + halfSize, center.z + halfSize, 1.0f } );
    faces[2]->setQuad( { center.x + halfSize, center.y - halfSize, center.z - halfSize, 1.0f },
                       { center.x - halfSize, center.y - halfSize, center.z - halfSize, 1.0f },
                       { center.x - halfSize, center.y + halfSize, center.z - halfSize, 1.0f },
                       { center.x + halfSize, center.y + halfSize, center.z - halfSize, 1.0f } );
    faces[3]->setQuad( { center.x - halfSize, center.y - halfSize, center.z - halfSize, 1.0f },
                       { center.x - halfSize, center.y - halfSize, center.z + halfSize, 1.0f },
                       { center.x - halfSize, center.y + halfSize, center.z + halfSize, 1.0f },
                       { center.x - halfSize, center.y + halfSize, center.z - halfSize, 1.0f } );
    faces[4]->setQuad( { center.x - halfSize, center.y + halfSize, center.z + halfSize, 1.0f },
                       { center.x + halfSize, center.y + halfSize, center.z + halfSize, 1.0f },
                       { center.x + halfSize, center.y + halfSize, center.z - halfSize, 1.0f },
                       { center.x - halfSize, center.y + halfSize, center.z - halfSize, 1.0f } );
    faces[5]->setQuad( { center.x - halfSize, center.y - halfSize, center.z - halfSize, 1.0f },
                       { center.x + halfSize, center.y - halfSize, center.z - halfSize, 1.0f },
                       { center.x + halfSize, center.y - halfSize, center.z + halfSize, 1.0f },
                       { center.x - halfSize, center.y - halfSize, center.z + halfSize, 1.0f } );
  }
};

} // namespace gl
} // namespace vas

#endif // _VAS_GL_BASE_H_