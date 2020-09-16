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

#include "PerlinNoise.h"

using namespace std;
using namespace glm;



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
STATUS init();
void showFrameRate();
unsigned int loadTexture(const char *path);
void drawSphere();

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

GLFWwindow* window;

Shader *planetShader;
Shader *oceanShader;

//light position
glm::vec3 lightPos(0.0f, 0.0f, 8.0f);



struct {
    vec3 position;
    vec3 normal;
} vertices[0x100000] = {

    //  <---Position------->    <----Normal------>      <TexCoord>
        vec3(-1.0f, 0.0f, 0.0f),     vec3(-1.0f,  0.0f, 0.0f),  //0
        vec3(1.0f, 0.0f, 0.0f),      vec3(1.0f,  0.0f, 0.0f),  //1
        vec3(0.0f, 1.0f, 0.0f),      vec3(0.0f,  1.0f, 0.0f),   //2
        vec3(0.0f, -1.0f, 0.0f),     vec3(0.0f,  -1.0f, 0.0f),  //3
        vec3(0.0f, 0.0f, 1.0f),      vec3(0.0f,  0.0f, 1.0f),   //4
        vec3(0.0f, 0.0f, -1.0f),     vec3(0.0f,  0.0f, -1.0f),  //5
};


struct {
    vec3 position;
    vec3 normal;
} noisedVertices[0x100000];

struct {
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
} faces[0x100000] = {  // note that we start from 0!
    0, 4, 2,    //0
    4, 1, 2,    //1
    0, 3, 4,    //2
    4, 3, 1,    //3
    0, 2, 5,    //4
    1, 5, 2,    //5
    0, 5, 3,    //6
    3, 5, 1     //7
};
int numVertices = 6;
int numFaces = 8;


unsigned int sphereVBO, sphereVAO, sphereEBO;
unsigned int oceanVBO, oceanVAO, oceanEBO;

void initializeSphere(){
    numVertices = 6;
    numFaces = 8;
    //generate sphere vertices
    for(int iteration = 0; iteration < 7; iteration++){
        unsigned int currentNumFaces = numFaces;
        for(int face = 0; face < currentNumFaces; face++){
            /*     p3
                   /\
               n6 /__\ n5
                 /\  /\
                /__\/__\
               p1  n4   p2
            */
            unsigned int p1 = faces[face].v1;
            unsigned int p2 = faces[face].v2;
            unsigned int p3 = faces[face].v3;
            unsigned int n4 = numVertices;
            unsigned int n5 = numVertices + 1;
            unsigned int n6 = numVertices + 2;

            vec3 v1 = vertices[p1].position;
            vec3 v2 = vertices[p2].position;
            vec3 v3 = vertices[p3].position;

            vertices[n4].position = vertices[n4].normal = normalize((v2 - v1) * 0.5f + v1);
            vertices[n5].position = vertices[n5].normal = normalize((v3 - v2) * 0.5f + v2);
            vertices[n6].position = vertices[n6].normal = normalize((v1 - v3) * 0.5f + v3);
            
            //old big face becomes the middle triangle
            faces[face].v1 = n4;
            faces[face].v2 = n5;
            faces[face].v3 = n6;
            // 3 new faces are added
            faces[numFaces].v1 = p1;
            faces[numFaces].v2 = n4;
            faces[numFaces].v3 = n6;

            faces[numFaces + 1].v1 = n4;
            faces[numFaces + 1].v2 = p2;
            faces[numFaces + 1].v3 = n5;

            faces[numFaces + 2].v1 = n6;
            faces[numFaces + 2].v2 = n5;
            faces[numFaces + 2].v3 = p3;
            
            numVertices += 3;
            numFaces += 3;
        }
    }
}

void applyNoise(){
    int seed = rand()/10000 % 10000;
    printf("seed = %d\n", seed);
    PerlinNoise pn(seed);

    for(int v = 0; v < numVertices; v++){
        vec3 pos = vertices[v].position;

        float noise = 0;
        float amplitude = 0.6f;
        float frequency = 1.0f;
/*
        float dx =0.001f, dy = 0.001f, dz = 0.001f;
        float nx0 = 0, nx1 = 0, ny0 = 0 , ny1 = 0, nz0 = 0, nz1 = 0;*/


        for(int i = 0; i < 5; i++){
            noise += amplitude * (pn.noise(pos.x * frequency , pos.y * frequency , pos.z * frequency) + 0.3);
/*            nx0 += amplitude * pn.noise((pos.x - dx) * frequency , pos.y * frequency , pos.z * frequency);
            nx1 += amplitude * pn.noise((pos.x + dx) * frequency , pos.y * frequency , pos.z * frequency);

            ny0 += amplitude * pn.noise((pos.x) * frequency , (pos.y - dy) * frequency , pos.z * frequency);
            ny1 += amplitude * pn.noise((pos.x) * frequency , (pos.y + dy) * frequency , pos.z * frequency);

            nz0 += amplitude * pn.noise((pos.x) * frequency , (pos.y) * frequency , (pos.z - dz) * frequency);
            nz1 += amplitude * pn.noise((pos.x) * frequency , (pos.y) * frequency , (pos.z + dz)* frequency);
*/
            frequency *= 2;
            amplitude *= 0.5f;
        }

/*        vec3 vx0 = (pos - vec3(dx, 0, 0)) * nx0;
        vec3 vx1 = (pos + vec3(dx, 0, 0)) * nx1;
        
        vec3 vy0 = (pos - vec3(0, dy, 0)) * ny0;
        vec3 vy1 = (pos + vec3(0, dy, 0)) * ny1;
        
        vec3 vz0 = (pos - vec3(0, 0, dz)) * nz0;
        vec3 vz1 = (pos + vec3(0, 0, dz)) * nz1;

        vec3 c1 = -cross (vx1 - vx0 , vy1 - vy0);
*/

        //float f = 10.0f;
        //float ridge = 1 + ridgeAmp * (1 - (std::abs(2*pn.noise(pos.x * f, pos.y * f, pos.z * f )-0.5f)));
        
        noisedVertices[v].position = vertices[v].position * noise;
        noisedVertices[v].normal = normalize(noisedVertices[v].position);
        //noisedVertices[v].normal = normalize(c1);
        

        //make some waves in the ocean
        frequency = 40.0f;
        vertices[v].position = vertices[v].position + vec3(0.005f * pn.noise(pos.x * frequency , pos.y * frequency , pos.z * frequency));
        
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
	glEnable(GL_DEPTH_TEST);  

    
    planetShader = new Shader("shaders/planetShader2.vs", "shaders/planetShader2.fs"/*, "shaders/planetShader2.gs"*/);
    oceanShader =  new Shader("shaders/oceanShader.vs", "shaders/oceanShader.fs"/*, "shaders/planetShader2.gs"*/);


    // load models
    // -----------
    Model ourModel("../models/nanosuit/nanosuit.obj");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    initializeSphere();
    applyNoise();


    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    glGenVertexArrays(1, &sphereVAO);

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(noisedVertices), noisedVertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);
    //vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &oceanVBO);
    glGenBuffers(1, &oceanEBO);
    glGenVertexArrays(1, &oceanVAO);

    glBindVertexArray(oceanVAO);
    glBindBuffer(GL_ARRAY_BUFFER, oceanVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oceanEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);
    //vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
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
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSphere();

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

void drawSphere(){
    int numElements = (sizeof(faces)/sizeof(int));
    static float perlinOffset = 1.0;
    perlinOffset += 0.001;

    //applyNoise();


    //filled or wireframe triangles
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    //glm::mat4 lightPosTransform = glm::rotate(glm::mat4(1.0f),0.01f, glm::vec3(0.0f, -1.0f, 0.0f));
    //lightPos = lightPosTransform * glm::vec4(lightPos,1.0f);

    //create model, view and projection matrices for box
    mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, (float)glfwGetTime()/5, glm::vec3(0.0f, 1.0f, 0.0f));
    mat4 view = camera.GetViewMatrix();
    mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    mat4 normalMat = glm::transpose(glm::inverse(model));

    // activate shader
    planetShader->use();
    planetShader->setVec3("objectColor", glm::vec3(0.4f, 0.4f, 0.4f));
    planetShader->setVec3("lightColor",  glm::vec3(1.0f, 1.0f, 1.0f));
    planetShader->setVec3("lightPos", lightPos);
    planetShader->setVec3("viewPos", camera.Position);   

    // set the model, view and projection matrix uniforms
    planetShader->setMat4("model", model);
    planetShader->setMat4("view", view);
    planetShader->setMat4("projection", projection);
    planetShader->setMat4("normalMat", normalMat);

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(noisedVertices), noisedVertices, GL_DYNAMIC_DRAW);
//    glDrawArrays(GL_TRIANGLES, 0, numTriangles);
    glDrawElements(GL_TRIANGLES, sizeof(faces), GL_UNSIGNED_INT, 0);


    // activate shader
    oceanShader->use();
    oceanShader->setVec3("lightColor",  glm::vec3(1.0f, 1.0f, 1.0f));
    oceanShader->setVec3("lightPos", lightPos);
    oceanShader->setVec3("viewPos", camera.Position);   

    // set the model, view and projection matrix uniforms
    oceanShader->setMat4("model", model);
    oceanShader->setMat4("view", view);
    oceanShader->setMat4("projection", projection);
    oceanShader->setMat4("normalMat", normalMat);

    oceanShader->setVec3("objectColor", glm::vec3(0.0f, 0.0f, 0.5f));
    oceanShader->setFloat("time", (float)glfwGetTime()/100);

    glBindVertexArray(oceanVAO);
    glDrawElements(GL_TRIANGLES, sizeof(faces), GL_UNSIGNED_INT, 0);



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