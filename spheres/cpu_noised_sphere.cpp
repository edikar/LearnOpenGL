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
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

#define DETALIZATION_ITERATIONS 6
#define NUM_FACES (1<<(5+DETALIZATION_ITERATIONS*2))
#define NUM_VERTICES ((1<<(4+DETALIZATION_ITERATIONS*2)) + 2)
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
Shader *oceanShader;;
Shader *normalsShader;

//light position
glm::vec3 lightPos(0.0f, 0.0f, 8.0f);



typedef struct {
    vec3 position;
    vec3 normal;
} VERTEX;
VERTEX *vertices;

int *normalsPerVertex; //this is needed to count number of faces each vertex involved in to average the normal

typedef struct {
    int numNeighbours;
    int neighbours[6];
    int middleVertex[6];
} USED_VERTICES;
USED_VERTICES *usedVertices;   //this array stores the vertices which were already split in current iteration - to prevent creating same vertex twice

typedef struct {
    vec3 position;
    vec3 normal;
} NOISED_VERTEX;
NOISED_VERTEX *noisedVertices;


typedef struct {
    vec3 position;
} NORMAL_POSITION;
NORMAL_POSITION *normalsPositions;

typedef struct {
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
} FACE;
FACE *faces;

int numVertices = 6;
int numFaces = 8;


unsigned int sphereVBO, sphereVAO, sphereEBO;
unsigned int oceanVBO, oceanVAO, oceanEBO;
unsigned int normalsVBO, normalsVAO;

int getMiddleVertex(int p1, int p2){
    for(int i = 0; i < usedVertices[p1].numNeighbours; i++){
        if(usedVertices[p1].neighbours[i] == p2){
            return usedVertices[p1].middleVertex[i];
        }
    }


    int nextNeighbour1 = usedVertices[p1].numNeighbours;
    int nextNeighbour2 = usedVertices[p2].numNeighbours;

    vec3 v1 = vertices[p1].position;
    vec3 v2 = vertices[p2].position;
    
    vertices[numVertices].position = vertices[numVertices].normal = normalize((v2 - v1) * 0.5f + v1);

    usedVertices[p1].neighbours[nextNeighbour1] = p2;
    usedVertices[p1].middleVertex[nextNeighbour1] = numVertices;
    usedVertices[p1].numNeighbours++;
    usedVertices[p2].neighbours[nextNeighbour2] = p1;
    usedVertices[p2].middleVertex[nextNeighbour2] = numVertices;
    usedVertices[p2].numNeighbours++;
    return numVertices++;
}

void initializeSphere(){
    numVertices = 6;
    numFaces = 8;

    //generate sphere vertices
    for(int iteration = 0; iteration <= DETALIZATION_ITERATIONS; iteration++){
        memset(usedVertices, 0, NUM_VERTICES * sizeof(USED_VERTICES));
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

            vec3 v1 = vertices[p1].position;
            vec3 v2 = vertices[p2].position;
            vec3 v3 = vertices[p3].position;

            unsigned int n4, n5, n6; // new vertices

            n4 = getMiddleVertex(p1,p2);
            n5 = getMiddleVertex(p2,p3);
            n6 = getMiddleVertex(p3,p1);

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

            numFaces += 3;
        }

        printf("iteration: %d, numFaces = %d (0x%x), numVertices = %d (0x%x)\n", iteration, numFaces, numFaces, numVertices, numVertices);
    }
    printf("numVertices = %d NUM_VERTICES = %d\n", numVertices, NUM_VERTICES);
    printf("max vertices = 0x%x\n", NUM_VERTICES);
    printf("max faces = 0x%x\n", NUM_FACES);
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

        for(int i = 0; i < 5; i++){
            noise += amplitude * (pn.noise(pos.x * frequency , pos.y * frequency , pos.z * frequency) + 0.3);
            frequency *= 2;
            amplitude *= 0.5f;
        }

        noisedVertices[v].position = vertices[v].position * noise;
        noisedVertices[v].normal = normalize(noisedVertices[v].position);
        //noisedVertices[v].normal = normalize(c1);
        
        //make some waves in the ocean
        frequency = 40.0f;
        vertices[v].position = vertices[v].position + vec3(0.005f * pn.noise(pos.x * frequency , pos.y * frequency , pos.z * frequency));
        
        normalsPositions[2 * v].position = noisedVertices[v].position;
        normalsPositions[2 * v + 1].position = noisedVertices[v].position + normalize(noisedVertices[v].position)/40.0f;

    }

    //land normals calculation
    memset(normalsPerVertex, 0, NUM_VERTICES * sizeof(int));
    for(int f = 0; f < numFaces; f++){
        vec3 v1 = noisedVertices[faces[f].v1].position;
        vec3 v2 = noisedVertices[faces[f].v2].position;
        vec3 v3 = noisedVertices[faces[f].v3].position;
        noisedVertices[faces[f].v1].normal += normalize(cross(v2-v1 , v3 - v1));
        normalsPerVertex[faces[f].v1]++;
        noisedVertices[faces[f].v2].normal += normalize(cross(v3-v2 , v1 - v2));
        normalsPerVertex[faces[f].v2]++;
        noisedVertices[faces[f].v3].normal += normalize(cross(v1-v3 , v2 - v3));
        normalsPerVertex[faces[f].v3]++;
    }

    for(int v = 0; v < numVertices; v++){
        noisedVertices[v].normal /= normalsPerVertex[v];
        normalsPositions[2 * v].position = noisedVertices[v].position;
        normalsPositions[2 * v + 1].position = noisedVertices[v].position + noisedVertices[v].normal/40.0f;

        vertices[v].normal /= normalsPerVertex[v];
    }

    //ocean normals calculations
    memset(normalsPerVertex, 0, NUM_VERTICES * sizeof(int));
    for(int f = 0; f < numFaces; f++){
         vec3 v1 = vertices[faces[f].v1].position;
         vec3 v2 = vertices[faces[f].v2].position;
         vec3 v3 = vertices[faces[f].v3].position;
         vertices[faces[f].v1].normal += normalize(cross(v2-v1 , v3 - v1));
         normalsPerVertex[faces[f].v1]++;
         vertices[faces[f].v2].normal += normalize(cross(v3-v2 , v1 - v2));
         normalsPerVertex[faces[f].v2]++;
         vertices[faces[f].v3].normal += normalize(cross(v1-v3 , v2 - v3));
         normalsPerVertex[faces[f].v3]++;       
    }

    for(int v = 0; v < numVertices; v++){
        vertices[v].normal /= normalsPerVertex[v];
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
    normalsShader = new Shader("shaders/normalsShader.vs", "shaders/normalsShader.fs"/*, "shaders/planetShader2.gs"*/);


    //alloate all memories
    faces = new FACE[NUM_FACES];// (FACE*)malloc(NUM_FACES * sizeof(FACE));
    memset(faces, 0, NUM_FACES * sizeof(FACE));
    faces[0] = { 0, 4, 2 };    //0
    faces[1] = { 4, 1, 2 };    //1
    faces[2] = { 0, 3, 4 };    //2
    faces[3] = { 4, 3, 1 };  //3
    faces[4] = { 0, 2, 5 };   //4
    faces[5] = { 1, 5, 2 };    //5
    faces[6] = { 0, 5, 3 };   //6
    faces[7] = { 3, 5, 1 };    //7

    vertices = new VERTEX[NUM_VERTICES]; //(VERTEX*)malloc(NUM_VERTICES * sizeof(VERTEX));
    memset(vertices, 0, NUM_VERTICES * sizeof(VERTEX));
    vertices[0] = { vec3(-1.0f, 0.0f, 0.0f),     vec3(-1.0f,  0.0f, 0.0f) };  //0
    vertices[1] = { vec3(1.0f, 0.0f, 0.0f),      vec3(1.0f,  0.0f, 0.0f) };  //1
    vertices[2] = { vec3(0.0f, 1.0f, 0.0f),      vec3(0.0f,  1.0f, 0.0f) };   //2
    vertices[3] = { vec3(0.0f, -1.0f, 0.0f),     vec3(0.0f,  -1.0f, 0.0f) };  //3
    vertices[4] = { vec3(0.0f, 0.0f, 1.0f),      vec3(0.0f,  0.0f, 1.0f) };   //4
    vertices[5] = { vec3(0.0f, 0.0f, -1.0f),     vec3(0.0f,  0.0f, -1.0f) };  //5

    normalsPerVertex = new int[NUM_VERTICES]; //(int*)malloc(NUM_VERTICES * sizeof(int));
    memset(normalsPerVertex, 0, NUM_VERTICES * sizeof(int));

    usedVertices = new USED_VERTICES[NUM_VERTICES];
    memset(usedVertices, 0, NUM_VERTICES * sizeof(USED_VERTICES));

    noisedVertices = new NOISED_VERTEX[NUM_VERTICES];
    memset(noisedVertices, 0, NUM_VERTICES * sizeof(NOISED_VERTEX));

    normalsPositions = new NORMAL_POSITION[ 2 * NUM_VERTICES];
    memset(normalsPositions, 0, 2 * NUM_VERTICES * sizeof(NORMAL_POSITION));

    
    initializeSphere();
    applyNoise();


    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    glGenVertexArrays(1, &sphereVAO);

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(NOISED_VERTEX), noisedVertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_FACES * sizeof(FACE), faces, GL_STATIC_DRAW);
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
    glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(VERTEX), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oceanEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_FACES * sizeof(FACE), faces, GL_STATIC_DRAW);
    
    //vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glGenBuffers(1, &normalsVBO);
    glGenVertexArrays(1, &normalsVAO);

    glBindVertexArray(normalsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
    glBufferData(GL_ARRAY_BUFFER, 2 * NUM_VERTICES * sizeof(NORMAL_POSITION), normalsPositions, GL_STATIC_DRAW);
    //vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //free malloced arrays
    delete[] faces;
    delete[] vertices;
    delete[] normalsPerVertex;
    delete[] usedVertices;
    delete[] noisedVertices;
    delete[] normalsPositions;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glEnable(GL_DEPTH_TEST);

    //filled or wireframe triangles
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
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
    static float perlinOffset = 1.0;
    perlinOffset += 0.001;

    //applyNoise();


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
    //glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(NOISED_VERTEX), noisedVertices, GL_DYNAMIC_DRAW);
//    glDrawArrays(GL_TRIANGLES, 0, numTriangles);
    glDrawElements(GL_TRIANGLES, NUM_FACES * sizeof(FACE), GL_UNSIGNED_INT, 0);


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
    glDrawElements(GL_TRIANGLES, NUM_FACES * sizeof(FACE), GL_UNSIGNED_INT, 0);

    // activate shader
    normalsShader->use();
    // set the model, view and projection matrix uniforms
    normalsShader->setMat4("model", model);
    normalsShader->setMat4("view", view);
    normalsShader->setMat4("projection", projection);
    normalsShader->setMat4("normalMat", normalMat);

    glBindVertexArray(normalsVAO);
    //glDrawArrays(GL_LINES, 0, 2*numVertices);



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