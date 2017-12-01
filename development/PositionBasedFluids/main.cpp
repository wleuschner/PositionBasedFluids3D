#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    if(!glfwInit())
    {
        std::cerr<<"Could not init GLFW"<<std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    GLFWwindow* window = glfwCreateWindow(800,600,"Position Based Fluids",NULL,NULL);
    if(!window)
    {
        std::cerr<<"Could not create window"<<std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
