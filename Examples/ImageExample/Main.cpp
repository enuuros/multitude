#ifndef WIN32
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif
#include <Luminous/Luminous.hpp>
#include <Luminous/Texture.hpp>

int main(int , char ** )
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );
  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1 );

  SDL_SetVideoMode(400 , 400, 0, SDL_OPENGL);

  Luminous::initLuminous();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  Luminous::Texture2D tex;
  if(!tex.loadImage("test.png")) return 1;

  tex.bind();

  bool running = true;

  while(running) {
    SDL_Event event;
    
    if(SDL_PollEvent(&event)) {
      switch(event.type) {
      case SDL_QUIT:
        running = false;
        break;
      };
    }

    glClearColor(1.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
			
    glColor3f(1.f, 1.f, 1.f);
    glBegin(GL_QUADS);

    glTexCoord2f(0.f, 1.f);
    glVertex2f(0.f, 0.f);

    glTexCoord2f(1.f, 1.f);
    glVertex2f(1.f, 0.f);

    glTexCoord2f(1.f, 0.f);
    glVertex2f(1.f, 1.f);

    glTexCoord2f(0.f, 0.f);
    glVertex2f(0.f, 1.f);
	
    glEnd();
	
    SDL_GL_SwapBuffers();
  }

  return 0;
}
