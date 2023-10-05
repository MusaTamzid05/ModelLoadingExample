#include "camera.h"


Camera* Camera::instance = nullptr;

Camera::Camera(int width, int height):
    width(width),
    height(height) {

        view = glm::mat4(1.0f);
        projection = glm::mat4(1.0f);


        projection = glm::perspective(
                glm::radians(45.0f),
                (float)width / (float)height,
                0.1f,
                100.0f
                );

        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

}

Camera* Camera::get_instance() {
    if(instance == nullptr)
        instance = new Camera(640, 480);

    return instance;
}
