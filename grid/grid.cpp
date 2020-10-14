// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_class.h"
#include "utils.h"
#include "camera.h"
#include "model.h"


using namespace std;
using namespace glm;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
STATUS init();
void showFrameRate();
unsigned int loadTexture(const char *path);
void draw();

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;
#define GRID_SIZE (10)

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

GLFWwindow* window;

Shader *gridShader;
Shader *playerShader;

//light position
glm::vec3 lightPos(1.0f, 2.0f, 1.0f);
//player position
glm::vec3 playerPos(100.0f, 200.0f, 1.0f);
float playerSpeed = 3;
float playerAngle = M_PI_2;



 struct {
    vec3 position;
    vec3 normal;
} gridVertices[SCR_WIDTH * SCR_HEIGHT];

struct {
    unsigned int v1;
    unsigned int v2;
} gridLines[SCR_WIDTH * SCR_HEIGHT] = {  // note that we start from 0!
    0, 1, 1, 2, 2, 3, 3, 4,    //0
};


 struct {
    vec3 position;
    vec3 normal;
} playerVertices[3] = {
    { vec3(-20.0f, -20.0f, 0.0f),     vec3(0.0f,  0.0f, 1.0f) },  //0
    { vec3(20.0f, 0.0f, 0.0f),     vec3(0.0f,  0.0f, 1.0f) },  //0
    { vec3(-20.0f, 20.0f, 0.0f),     vec3(0.0f,  0.0f, 1.0f) },  //0
};

struct {
    unsigned int v1;
    unsigned int v2;
} playerFace[3] = {  // note that we start from 0!
    0, 1, 2,    //0
};


unsigned int gridVBO, gridVAO, gridEBO;
unsigned int playerVBO, playerVAO, playerEBO;

void initializeGrid(){
    
    int nx = 1 + SCR_WIDTH / GRID_SIZE;
    int ny = 1 + SCR_HEIGHT / GRID_SIZE;
    printf("nx = %d ny = %d\n", nx, ny);
    int t = 0;
    for(int y = 0; y <= ny; y++){
        for(int x = 0; x <= nx; x++){
            gridVertices[y * nx + x].position = vec3(x * GRID_SIZE, y * GRID_SIZE, 0);
            gridVertices[y * nx + x].normal = vec3(0, 0, 1);

            gridLines[2*(y * nx + x)].v1 = y * nx + x;
            if(x == nx-1)
                gridLines[2*(y * nx + x)].v2 = y * nx + x;
            else
                gridLines[2*(y * nx + x)].v2 = y * nx + x + 1;
            gridLines[2*(y * nx + x)+1].v1 = y * nx + x;
            gridLines[2*(y * nx + x)+1].v2 = (y + 1) * nx + x;
/*
            printf("%d: [%d %d] = %d %d\n", t, x,y, x*GRID_SIZE, y*GRID_SIZE);
            printf("[%d] = %d----%d\n", 2*(y * ny + x), gridLines[2*(y * nx + x)].v1, gridLines[2*(y * nx + x)].v2);
            printf("[%d] = %d----%d\n", 2*(y * ny + x)+1, gridLines[2*(y * nx + x)+1].v1, gridLines[2*(y * nx + x)+1].v2);*/
            //t++;
        }
    }


    
}

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
    
    gridShader = new Shader("shaders/gridShader.vs", "shaders/gridShader.fs");
    playerShader = new Shader("shaders/playerShader.vs", "shaders/playerShader.fs");
    

    // load models
    // -----------
    Model ourModel("../models/nanosuit/nanosuit.obj");

    initializeGrid();

    glGenBuffers(1, &gridVBO);
    glGenBuffers(1, &gridEBO);
    glGenVertexArrays(1, &gridVAO);

    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gridLines), gridLines, GL_STATIC_DRAW);
    //vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glGenBuffers(1, &playerVBO);
    glGenBuffers(1, &playerEBO);
    glGenVertexArrays(1, &playerVAO);

    glBindVertexArray(playerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(playerFace), playerFace, GL_STATIC_DRAW);
    //vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glEnable(GL_DEPTH_TEST); 

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
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw();

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

void draw(){

    glEnable(GL_CULL_FACE);  

    //filled or wireframe triangles
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    //glm::mat4 lightPosTransform = glm::rotate(glm::mat4(1.0f),0.01f, glm::vec3(0.0f, -1.0f, 0.0f));
    //lightPos = lightPosTransform * glm::vec4(lightPos,1.0f);

    //create model, view and projection matrices for box
    mat4 model = glm::mat4(1.0f);
    mat4 view = camera.GetViewMatrix();
    mat4 projection = glm::ortho( 0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT, -10.0f, 10.0f );//glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    mat4 normalMat = glm::transpose(glm::inverse(model));

    // activate shader
    gridShader->use();

    // set the model, view and projection matrix uniforms
    gridShader->setMat4("model", model);
    gridShader->setMat4("view", view);
    gridShader->setMat4("projection", projection);
    gridShader->setVec3("playerPos", playerPos);

    glBindVertexArray(gridVAO);
    glDrawElements(GL_LINES, 2*2*(1 + SCR_WIDTH/GRID_SIZE) * (1 + SCR_HEIGHT/GRID_SIZE), GL_UNSIGNED_INT, 0);


    playerShader->use();
    model = glm::translate(model, playerPos);
    model = glm::rotate(model, playerAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    playerShader->setMat4("model", model);
    playerShader->setMat4("view", view);
    playerShader->setMat4("projection", projection);

    glBindVertexArray(playerVAO);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    
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
    srand(time(0));

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        playerPos += playerSpeed * vec3(glm::cos(playerAngle), glm::sin(playerAngle), 0.0f);
        //camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        playerPos += playerSpeed * (-vec3(glm::cos(playerAngle), glm::sin(playerAngle), 0.0f));
        //camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        playerAngle += playerSpeed * 0.01;
        //camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        playerAngle += playerSpeed * (-0.01);
        //camera.ProcessKeyboard(RIGHT, deltaTime);
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