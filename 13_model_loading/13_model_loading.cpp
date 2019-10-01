// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
/*#include <glm/ext/matrix_clip_space.hpp>*/
#include <glm/gtc/type_ptr.hpp>


#include "shader_class.h"
#include "utils.h"
#include "camera.h"
#include "model.h"


using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
STATUS init();
void showFrameRate();
unsigned int loadTexture(const char *path);

typedef struct {    
    glm::vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    glm::vec3 rotationVector;
}PointLight;  

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.7f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

GLFWwindow* window;


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
	glEnable(GL_DEPTH_TEST);  


    Shader ourShader("shaders/lightingShader.vs", "shaders/lightingShader.fs");
    //Shader ourShader("shaders/test.vs", "shaders/test.fs");


    // load models
    // -----------
    Model ourModel("../models/nanosuit/nanosuit.obj");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        //material properties
        //ourShader.setFloat("material.shininess", 16.0f);
        //directional light properties
        ourShader.setVec3("dirLight.direction", glm::vec3(0.5f, -1.0f, -1.0f));
        ourShader.setVec3("dirLight.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
        ourShader.setVec3("dirLight.diffuse",  glm::vec3(0.6f, 0.6f, 0.6f)); // darken the light a bit to fit the scene
        ourShader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f)); 
        ourShader.setVec3("viewPos", camera.Position);   

        //point light properties
/*        ourShader.setVec3( "pointLights[0].position", glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setFloat( "pointLights[0].constant", 1.0f);
        ourShader.setFloat( "pointLights[0].linear", 0.045f);
        ourShader.setFloat( "pointLights[0].quadratic", 0.0075f);
        ourShader.setVec3( "pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        ourShader.setVec3( "pointLights[0].diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
        ourShader.setVec3( "pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));*/

        //spot light properties
        ourShader.setVec3("spotLight.position",  camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setFloat("spotLight.cutOff",   glm::cos(glm::radians(12.5f))); //sending cosine to save performance in FS
        ourShader.setFloat("spotLight.outerCutOff",   glm::cos(glm::radians(17.5f))); //sending cosine to save performance in FS
        ourShader.setFloat("spotLight.constant",   1.0f);
        ourShader.setFloat("spotLight.linear",   0.045f); 
        ourShader.setFloat("spotLight.quadratic",   0.0075);
        ourShader.setVec3("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        ourShader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);

        ourShader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
        ourModel.Draw(ourShader);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        showFrameRate();
    }

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
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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