#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include "obj_model.h"

namespace NycaTech::Renderer {

class Renderer {
public:
               Renderer() = default;
  virtual ~    Renderer() = default;
  virtual void RenderModel(const ObjModel& model) = 0;
  virtual void DrawFrame() = 0;

public:
  SDL_Window* window;
};

}  // namespace NycaTech::Renderer

#endif  // RENDERER_H
