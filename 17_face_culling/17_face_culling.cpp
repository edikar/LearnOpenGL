// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include "shader_class.h"
#include "utils.h"
#include "camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
STATUS init();
void showFrameRate();
unsigned int loadTexture(char const * path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

GLFWwindow* window;

int front_or_back = 1; //draw front faces first

int main()
{
	STATUS status = OK;

	//init project
	status = init();	
	if(status != OK)
	{
		cout << "init() failed at " << __FILE__  << ":" << __LINE__ << __FUNCTION__ << endl;
		return ERROR;
	}

	 Shader ourShader("shaders/shader.vs", "shaders/shader.fs"); // you can name your shader files however you like

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
	 float vertices[] = {
	     // back face
	     -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
	      0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right    
	      0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right              
	      0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	     -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left                
	     // front face
	     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	      0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right        
	      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	     -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left        
	     // left face
	     -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
	     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	     -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left       
	     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	     -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
	     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	     // right face
	      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	      0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right      
	      0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right          
	      0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	      0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	     // bottom face          
	     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	      0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	      0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left        
	      0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	     // top face
	     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	      0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right                 
	      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // bottom-right
	     -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // bottom-left  
	     -0.5f,  0.5f, -0.5f,  0.0f, 0.0f  // top-left              
	 };


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//define the position attribute for the shaders
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  
	//define the texture
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); 

    //filled or wireframe triangles
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    //glCullFace(GL_FRONT); //
    glCullFace(GL_BACK);


    //create texture 
    unsigned int texture0 = loadTexture("../images/metal.png");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //transform and rotate by time
        glm::mat4 model = glm::rotate(glm::mat4(1.0), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (float)glm::radians(glfwGetTime()) * 5, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        ourShader.use();
        // set the model, view and projection matrix uniforms
        ourShader.setMat4("model", model);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

		ourShader.setInt("texture0", 0);
		ourShader.setInt("front_or_back", front_or_back);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void showFrameRate(){

	static double previousTime;
	static int frameCount;
	double currentTime = glfwGetTime();
	double deltaMs = (currentTime - previousTime) * 1000;
	frameCount++;
	if ( currentTime - previousTime >= 1.0 )
	{
		// Display the frame count here any way you want.
		std::stringstream ss;
		ss << "FPS: " << frameCount;
		glfwSetWindowTitle(window, ss.str().c_str());

		frameCount = 0;
		previousTime = currentTime;
	}
}

//Init glfw, create and init window, init glew
STATUS init()
{
    // glfw: initialize and configure
    // ------------------------------
	glfwInit();
	//the following 3 lines define opengl version 4.5 and the core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return ERROR;
	}
	
	glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback); 

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return ERROR;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	return OK;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        glCullFace(GL_BACK);
        front_or_back = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS){
        glCullFace(GL_FRONT);
        front_or_back = 0;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    
    if (data)
    {
        GLenum format;
        if (nrComponents == 1){
            std::cout << "GL_RED\n";
            format = GL_RED;
        }
        else if (nrComponents == 3){
            std::cout << "GL_RGB\n";
            format = GL_RGB;
        }
        else if (nrComponents == 4){
            std::cout << "GL_RGBA\n";
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //clamp to edge in order to prevent edge alpha value interpolated with repeated bottom
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);   
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);  

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}