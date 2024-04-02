//
// Created by rplaz on 2024-04-02.
//

#ifndef NYCATECH_GL_RENDERER_H
#define NYCATECH_GL_RENDERER_H

#include "renderer.h"

namespace NycaTech::Renderer {

class GlRenderer final : public Renderer {
public:
  static Renderer& Instance();
  virtual void     RenderModel(const ObjModel& model);
  virtual void     DrawFrame();

private:
  GlRenderer();
};

}  // namespace NycaTech::Renderer

#endif  // NYCATECH_GL_RENDERER_H
