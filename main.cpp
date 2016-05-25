#include <iostream>

#define GLM_FORCE_RADIANS
// GLEW is for loading OpenGL extensions.
// GLM is OpenGL Maths library with vector and matrix supports.
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

// GLFW is for creating and managing windows with OpenGL Context.
#include <GLFW/glfw3.h>

// SOIL is Simple OpenGL Image Library used to load textures.
#include <SOIL/SOIL.h>
//include shader and camera class
#include <Shader.h>
#include <Camera.h>
#include <Model.h>

const GLuint SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
bool firstMouse = true;
double lastX=400.0, lastY=300.0;
glm::vec3 lightPos(1.2f, 2.0f, 7.0f);

Camera cam(glm::vec3(0.0f, 0.0f, 3.0f));
void key_callback(GLFWwindow* window,int key,int scancode,int action,int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main(int argc, char** argv)
{
    glfwInit();

    // Create window with hint to use OpenGL 3.3 context.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Dimesnsion223 Test", 0, 0);
    //If error, show error message
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize glew to load OpenGL extensions supported by drivers.
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Set OpenGL viewport size to match that of window.
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    //Build and compiler our shamders
    Shader shader("shaders/vertex.shader", "shaders/fragment.shader");
    Shader lampShader("shaders/lampVertex.shader", "shaders/lampFrag.shader");
    Model nanoSuit("Joanna_Dark_Bounty_Hunter_rigged/Joanna_Dark.obj");
    // Model nanoSuit("homer_x/homer.x");

   // The main loop ends when the window should close.
    while (!glfwWindowShouldClose(window)) {

      // Fetch and process any queued events on the window.
      glfwPollEvents();
      // Actual opengl rendering code.
      // Set clear (background) color to (0.3, 0.3, 0.3).
      glClearColor(0.2f, 0.6f, 1.0f, 1.0f);
      // Clear the background as well as depth buffers.
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      //Draw the conatiner cube first
  	  glm::mat4 model;
      model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
      model = glm::scale(model, glm::vec3(0.1f));
      glm::mat4 view;
      view = cam.getViewMatrix();
      glm::mat4 projection;
      projection = glm::perspective(120.0f,
            (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 10000.0f);
      glm::mat4 mvp = projection * view * model;
      GLint modelLoc = glGetUniformLocation(shader.Program, "model");
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
      GLint mvpLoc = glGetUniformLocation(shader.Program, "mvp");
      glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

      GLint lightPosPos = glGetUniformLocation(shader.Program,
            "light.position");
      GLint viewPosPos = glGetUniformLocation(shader.Program, "viewPos");
      glUniform3f(lightPosPos, lightPos.x, lightPos.y, lightPos.z);
      glUniform3f(viewPosPos, cam.mPosition.x, cam.mPosition.y,
            cam.mPosition.z);

     // glm::vec3 lightColor;
    //  lightColor.x = sin(glfwGetTime() * 2.0f);
    //  lightColor.y = sin(glfwGetTime() * 0.7f);
    //  lightColor.z = sin(glfwGetTime() * 1.3f);
    //  glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // Decrease the influence
     // glm::vec3 ambientColor = diffuseColor * glm::vec3(0.8f); // Low influence
    //  glUniform3f(glGetUniformLocation(shader.Program, "light.ambient"),  ambientColor.x, ambientColor.y, ambientColor.z);
    //  glUniform3f(glGetUniformLocation(shader.Program, "light.diffuse"),  diffuseColor.x, diffuseColor.y, diffuseColor.z);
    //  glUniform3f(glGetUniformLocation(shader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);

    //  glUniform3f(glGetUniformLocation(shader.Program, "material.ambient"),   1.0f, 0.5f, 0.31f);
    //  glUniform3f(glGetUniformLocation(shader.Program, "material.diffuse"),   1.0f, 0.5f, 0.31f);
    //  glUniform3f(glGetUniformLocation(shader.Program, "material.specular"),  0.5f, 0.5f, 0.5f); // Specular doesn't have full effect on this object's material
    //  glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 50.0f);

      shader.use();
  	  nanoSuit.Draw(shader);


      glfwSwapBuffers(window);

    }
    // Once the main loop ends, exit by destroying the window.
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window,int key,int scancode,int action,int mode){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = (xpos - lastX);
    GLfloat yoffset = (lastY - ypos);
    lastX = xpos;
    lastY = ypos;

    cam.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  cam.processMouseScroll(yoffset);
}
