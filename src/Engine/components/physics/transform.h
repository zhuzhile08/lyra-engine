#pragma once

#include <glm.hpp>
#include <math/math.h>
#include <math.h>

namespace lyra {
    class TransformComponent {
    private:
        // variables
        // everything is local
        glm::vec3 position, rotation = {0, 0, 0}, scale = {1, 1, 1};

    private:
        // functions
        void translate(glm::vec3 velocity);
        void rotate(glm::vec3 rotation);
        void translate(glm::vec3 velocity, glm::vec3 point);
        void rotate(glm::vec3 rotation, glm::vec3 point);

        // getters
        glm::vec3 get_local_position();
        glm::vec3 get_local_rotation();
        glm::vec3 get_local_scale();

        // setters
        void set_position(glm::vec3 newPosition);
        void set_rotation(glm::vec3 newRotation);
        void set_rotation(glm::vec3 rotation, glm::vec3 point);
        void set_scale(glm::vec3 newScale);
    };
}
