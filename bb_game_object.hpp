#pragma once

#include "bb_model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace bb {
struct TransformComponent {
  glm::vec3 translation{};
  glm::vec3 scale{1.0f, 1.0f, 1.0f};
  glm::vec3 rotation{};

  glm::mat4 mat4();
  glm::mat3 normalMatrix();
};

class BbGameObject {
public:
  using id_t = unsigned int;

  static BbGameObject createGameObject() {
    static id_t currentId = 0;
    return BbGameObject{currentId++};
  }
  BbGameObject(const BbGameObject &) = delete;
  BbGameObject &operator=(const BbGameObject &) = delete;
  BbGameObject(BbGameObject &&) = default;
  BbGameObject &operator=(BbGameObject &&) = default;

  const id_t getId() { return id; }

  std::shared_ptr<BbModel> model{};
  glm::vec3 color{};
  TransformComponent transform{};

private:
  BbGameObject(id_t objId) : id{objId} {}

  id_t id;
};
} // namespace bb
