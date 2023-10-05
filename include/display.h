#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <vector>


struct GLFWwindow;
struct Model;

namespace Engine {

    class Shape;

    class Display {

        public:
            Display(const std::string& title = "Window",
                    int width = 640,
                    int height = 480
                    );

            virtual ~Display();
            void run();

            static void framebuffer_size_callback(GLFWwindow* window,
                    int width,
                    int height);


        private:


            Display(const Display&);
            Display &operator=(const Display&);

            bool init(const std::string& title);


            void draw();
            void update();
            void handle_input();

            int width;
            int height;

            GLFWwindow* m_window;

            std::vector<Shape*> shapes;

            Model* model;
    };

};

#endif
