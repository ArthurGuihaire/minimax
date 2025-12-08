#include <GLFW/glfw3.h>

enum renderState {
    waiting, validMoves, selected
};

class Renderer {
    public:
        Renderer();
    private:
        GLFWwindow* window;

};