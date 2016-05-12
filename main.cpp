#include <iostream>

// GLEW and GLM are included in the stdinc.h.
// GLEW is for loading OpenGL extensions.
// GLM is OpenGL Maths library with vector and matrix supports.
#include "stdinc.h"

// GLFW is for creating and managing windows with OpenGL Context.
#include <GLFW/glfw3.h>

// SOIL is Simple OpenGL Image Library used to load textures.
#include <SOIL/SOIL.h>
bool firstMouse = true;
GLfloat fov =  45.0f;
double lastX=400.0, lastY=300.0;
GLfloat yaw=-90.0f, pitch=0.0f;    
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  9.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLfloat mixValue = 0.2f;
//Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 position;\n"
   //     "layout (location = 1) in vec3 ourcolor;\n"
	"layout (location = 2) in vec2 textCoor;\n"
        "out vec3 vColor;\n"
	"out vec2 texCoor;\n"
	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"uniform mat4 projection;\n"
	"void main()\n"
	"{\n"
	"gl_Position = projection * view * model * vec4(position, 1.0);\n"
     //   "vColor = ourcolor;\n"
	"texCoor = vec2(textCoor.x, 1.0f-textCoor.y);\n"
	"}\0";

const GLchar* fragmentShaderSource= "#version 330 core\n"
	"in vec3 vColor;\n"
	"in vec2 texCoor;\n"
	"out vec4 color;\n"

	"uniform float mixValue;\n"

	"uniform sampler2D ourTexture;\n"
	"uniform sampler2D ourTexture1;\n"
	"void main()\n"
	"{\n"
//	"color = vec4(vColor, 1.0f);\n"
//	"color = texture(ourTexture, texCoor)* vec4(vColor, 1.0f);\n"
	"color = mix(texture(ourTexture, texCoor), texture(ourTexture1, texCoor), mixValue);\n"
	"}\0";

int main(int argc, char** argv)
{
    glfwInit();

    // Create window with hint to use OpenGL 3.3 context.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Test", 0, 0);

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
    glViewport(0, 0, 800, 600);

    //Build and compiler our shaders
    //Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    //Check for errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
	 glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    //Fragment shaders
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for compile time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //Link Shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glm::mat4 trans;
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));

//Setup the vertex data
//GLfloat vertices[] = {
//  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Left  
//    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Right 
//     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // Top   
//};
//
    int w, h;
    unsigned char* image = SOIL_load_image("container.jpg", &w, &h, 0, SOIL_LOAD_RGB);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
        // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
   //next image    
    image = SOIL_load_image("arrow.jpg", &w, &h, 0, SOIL_LOAD_RGB);
    GLuint texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
        // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);

//    GLfloat vertices[] = {
//         //position		//color		//texture
//	0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // Top Right
//         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,// Bottom Right
//        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,// Bottom Left
//        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,  0.0f, 1.0f// Top Left 
//    };
//    GLuint indices[] = {  // Note that we start from 0!
//        0, 1, 3,  // First Triangle
//        1, 2, 3   // Second Triangle
//    };
    GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

  //  glm::mat4 model;
  //  model = glm::rotate(model, (GLfloat)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
  //  glm::mat4 view;
  //  view = glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  //  glm::mat4 projection;
  //  projection = glm::perspective(glm::radians(45.0f), (float)800/600, 0.1f, 100.0f);
 glEnable(GL_DEPTH_TEST);
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  //  glGenBuffers(1, &EBO);
    //Bind
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  //  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  //  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  //position 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
//color
  //  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
  //  glEnableVertexAttribArray(1);
    // textures
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5* sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

   // glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
   // The main loop ends when the window should close.
    while (!glfwWindowShouldClose(window)) {
        
        // Fetch and process any queued events on the window.
        glfwPollEvents();
        // Actual opengl rendering code.
	 glm::mat4 model;
    model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    glm::mat4 view;
   // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(fov), (float)800/600, 0.1f, 100.0f);

GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        // Set clear (background) color to (0.3, 0.3, 0.3).
        glClearColor(0.2f, 0.6f, 1.0f, 1.0f);
        // Clear the background as well as depth buffers.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
         // Draw our first triangle
        glUseProgram(shaderProgram);

    // glBindTexture(GL_TEXTURE_2D, texture);   
    // Bind Textures using texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture1"), 1);       

	glUniform1f(glGetUniformLocation(shaderProgram, "mixValue"), mixValue);
        
	GLuint transloc = glGetUniformLocation(shaderProgram, "transform");
	glUniformMatrix4fv(transloc, 1, GL_FALSE, glm::value_ptr(trans));	
	
	glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
       
    //    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap the rendered screen to window.
        glfwSwapBuffers(window);
    }
     // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
  //  glDeleteBuffers(1, &EBO);
    
    // Once the main loop ends, exit by destroying the window.
    glfwTerminate();
    return 0;
}
    void key_callback(GLFWwindow* window,int key,int scancode,int action,int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_UP && action == GLFW_PRESS){
			mixValue += 0.1f;
			if (mixValue >= 1.0f)
				mixValue = 1.0f;
    }	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
			mixValue -= 0.1f;
			if (mixValue <= 0.0f)
				mixValue = 0.0f;
}
}
    void mouse_callback(GLFWwindow* window, double xpos, double ypos){
 if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    GLfloat sensitivity = 0.5;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  if(fov >= 1.0f && fov <= 45.0f)
  	fov -= yoffset;
  if(fov <= 1.0f)
  	fov = 1.0f;
  if(fov >= 45.0f)
  	fov = 45.0f;
}	
