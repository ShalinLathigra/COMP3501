#include "asteroid.h"

namespace game {

Asteroid::Asteroid(const std::string name, const Resource *geometry, const Resource *material) : SceneNode(name, geometry, material) {
}


Asteroid::~Asteroid(){
}


glm::quat Asteroid::GetAngM(void) const {

    return angm_;
}


void Asteroid::SetAngM(glm::quat angm){

    angm_ = angm;
}


void Asteroid::Update(float deltaTime){

    Rotate(angm_ * deltaTime);
}
            
} // namespace game
