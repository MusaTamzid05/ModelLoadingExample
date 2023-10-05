#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {

    public:
        static Camera* get_instance();

        glm::mat4 view;
        glm::mat4 projection;

    private:
        Camera(int width, int height);

        static Camera* instance;

        int width;
        int height;



};


#endif
