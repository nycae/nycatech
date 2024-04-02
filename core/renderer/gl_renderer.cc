//
// Created by rplaz on 2024-04-02.
//

#include "gl_renderer.h"

#include <gl/glew.h>

namespace NycaTech::Renderer {

Renderer& GlRenderer::Instance()
{
  static GlRenderer instance;
  return instance;
}

void GlRenderer::RenderModel(const ObjModel& model)
{
}

void GlRenderer::DrawFrame()
{
  SDL_GL_SwapWindow(window);
}

GlRenderer::GlRenderer()
{
  glewInit();
}

}  // namespace NycaTech::Renderer