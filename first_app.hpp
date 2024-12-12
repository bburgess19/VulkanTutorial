#pragma once

#include "bb_game_object.hpp"
#include "bb_renderer.hpp"
#include "bb_window.hpp"

#include <memory>
#include <vector>

namespace bb {
class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();

private:
  void loadGameObjects();
  void sierpinski(std::vector<BbModel::Vertex> &vertices, int depth, glm::vec2 left,
                  glm::vec2 right, glm::vec2 top);

  BbWindow bbWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
  BbDevice bbDevice{bbWindow};
  BbRenderer bbRenderer{bbWindow, bbDevice};

  std::vector<BbGameObject> gameObjects;
};
} // namespace bb
