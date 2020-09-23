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

#include "PerlinNoise.h"
#include "planet.h"

using namespace std;
using namespace glm;


void Planet::toggleNormals(bool state){
    showNormals = state;
}

void Planet::toggleOcean(bool state){
    showOcean = state;
}

void Planet::toggleWireframe(bool state){
    showWireframe = state;   
}

void Planet::setNoiseFrequency(float frequency){
    noiseFrequency = frequency;

    initializePlanet();
}

void Planet::setDetalization(int detalization){
    this->detalization = detalization;
    initializePlanet();
}

void Planet::setNoiseAmplitude(float amplitude){
    noiseAmplitude = amplitude;
    initializePlanet();
}

void Planet::setNoiseSeed(int seed){
    perlinSeed = seed;
    initializePlanet();
}

int Planet::randomizePlanet(int detalization, float frequency, float amplitude){
    //update random seed
    perlinSeed = rand()/10000 % 10000;
    printf("perlinSeed = %d\n", perlinSeed);
    //recreate planet
    initializePlanet();

    return perlinSeed;
}

int Planet::getMiddleVertex(int p1, int p2){
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

void Planet::initializeSphere(){
    numVertices = 6;
    numFaces = 8;

    //generate sphere vertices
    for(int iteration = 0; iteration <= detalization; iteration++){
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

void Planet::applyNoise(){
    PerlinNoise pn(perlinSeed);

    for(int v = 0; v < numVertices; v++){
        vec3 pos = vertices[v].position;

        float noise = 0;
        float frequency = noiseFrequency;
        float amplitude = noiseAmplitude;

        //mountain ridge
 //       float ridgeNoise = amplitude*(1-(std::abs(2*(pn.noise(pos.x * frequency , pos.y * frequency , pos.z * frequency)-0.5f)))) - 0.2f;

        for(int i = 0; i < 5; i++){
            frequency *= 2;
            amplitude *= 0.5f;
            noise += amplitude * 2*((pn.noise(pos.x * frequency , pos.y * frequency , pos.z * frequency)-0.5f));            
        }

        noisedVertices[v].position = vertices[v].position * (1 + noise);
        noisedVertices[v].normal = normalize(noisedVertices[v].position);
        //noisedVertices[v].normal = normalize(c1);
        
        //make some waves in the ocean
        frequency = 40.0f;
        vertices[v].position = vertices[v].position + vec3(0.005f * pn.noise(pos.x * frequency , pos.y * frequency , pos.z * frequency));
        
        normalsPositions[2 * v].position = noisedVertices[v].position;
        normalsPositions[2 * v + 1].position = noisedVertices[v].position + normalize(noisedVertices[v].position)/30.0f;

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
        normalsPositions[2 * v + 1].position = noisedVertices[v].position + noisedVertices[v].normal/30.0f;

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

STATUS init()
{

    srand(time(0));

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return ERROR;
    }

    return OK;
}


void Planet::initializePlanet()
{
    STATUS status = OK;

    NUM_FACES = (1<<(5+detalization*2)); //calculate expected number of faces
    NUM_VERTICES = ((1<<(4+detalization*2)) + 2); //calculate expected number of vertices

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

    return;
}


void Planet::setupDrawParameters(){

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glEnable(GL_DEPTH_TEST);

    //filled or wireframe triangles
    if(showWireframe){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else{
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    //glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

}

void Planet::drawPlanet(){
        // activate shader
        planetShader->use();
        planetShader->setVec3("objectColor", glm::vec3(0.4f, 0.4f, 0.4f));
        planetShader->setVec3("lightColor",  glm::vec3(1.0f, 1.0f, 1.0f));
        planetShader->setVec3("lightPos", lightPos);
        planetShader->setVec3("viewPos", camera->Position);   

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

}

void Planet::drawOcean(){
    // activate shader
    oceanShader->use();
    oceanShader->setVec3("lightColor",  glm::vec3(1.0f, 1.0f, 1.0f));
    oceanShader->setVec3("lightPos", lightPos);
    oceanShader->setVec3("viewPos", camera->Position);   

    // set the model, view and projection matrix uniforms
    oceanShader->setMat4("model", model);
    oceanShader->setMat4("view", view);
    oceanShader->setMat4("projection", projection);
    oceanShader->setMat4("normalMat", normalMat);

    oceanShader->setVec3("objectColor", glm::vec3(0.0f, 0.0f, 0.5f));
    oceanShader->setFloat("time", (float)glfwGetTime()/100);

    glBindVertexArray(oceanVAO);
    glDrawElements(GL_TRIANGLES, NUM_FACES * sizeof(FACE), GL_UNSIGNED_INT, 0);

}

void Planet::drawNormals(){
    // activate shader
    normalsShader->use();
    // set the model, view and projection matrix uniforms
    normalsShader->setMat4("model", model);
    normalsShader->setMat4("view", view);
    normalsShader->setMat4("projection", projection);
    normalsShader->setMat4("normalMat", normalMat);

    glBindVertexArray(normalsVAO);
    glDrawArrays(GL_LINES, 0, 2*numVertices);
}

void Planet::planetDraw(){
    
    // per-frame time logic
    // --------------------
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    setupDrawParameters();

    glClearColor(0.7f, 0.7f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //glm::mat4 lightPosTransform = glm::rotate(glm::mat4(1.0f),0.01f, glm::vec3(0.0f, -1.0f, 0.0f));
    //lightPos = lightPosTransform * glm::vec4(lightPos,1.0f);

    //create model, view and projection matrices
    model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime()/5, glm::vec3(0.0f, 1.0f, 0.0f));
    view = camera->GetViewMatrix();
    projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    normalMat = glm::transpose(glm::inverse(model));

    drawPlanet();

    if(showOcean)
        drawOcean();
    if(showNormals)
        drawNormals();
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
        printf("FPS = %d\n", frameCount);
		//glfwSetWindowTitle(window, ss.str().c_str());

		frameCount = 0;
		previousTime = currentTime;
	}
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Planet::processInput(int key, int action)
{
    if (key == GLFW_KEY_W && action/* == GLFW_PRESS*/)
        camera->ProcessKeyboard(FORWARD, deltaTime);

    if (key == GLFW_KEY_S && action)
        camera->ProcessKeyboard(BACKWARD, deltaTime);

    if (key == GLFW_KEY_A && action)
        camera->ProcessKeyboard(LEFT, deltaTime);
    
    if (key == GLFW_KEY_D && action)
        camera->ProcessKeyboard(RIGHT, deltaTime);



    int cameraSpeed = 3.0f;
    if (key == GLFW_KEY_LEFT && action)
        camera->ProcessMouseMovement(-cameraSpeed, 0.0f);
    if (key == GLFW_KEY_RIGHT && action)
        camera->ProcessMouseMovement(cameraSpeed, 0.0f);
    if (key == GLFW_KEY_UP && action)
        camera->ProcessMouseMovement(0.0f, cameraSpeed);
    if (key == GLFW_KEY_DOWN && action)
        camera->ProcessMouseMovement(0.0f, -cameraSpeed);
    
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

Planet::Planet() {
    //init project
    STATUS status = init();    
    if(status != OK)
    {
        cout << "init() failed at " << __FILE__  << ":" << __LINE__ << __FUNCTION__ << endl;
        return;
    }

    planetShader = new Shader("shaders/planetShader2.vs", "shaders/planetShader2.fs"/*, "shaders/planetShader2.gs"*/);
    oceanShader =  new Shader("shaders/oceanShader.vs", "shaders/oceanShader.fs"/*, "shaders/planetShader2.gs"*/);
    normalsShader = new Shader("shaders/normalsShader.vs", "shaders/normalsShader.fs"/*, "shaders/planetShader2.gs"*/);

    camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
    lightPos = vec3(0.0f, 0.0f, 8.0f);

    perlinSeed = 1;
    printf("perlinSeed = %d\n", perlinSeed);

    detalization = 5;
    noiseFrequency = 1.0f;
    noiseAmplitude = 0.6f;
    initializePlanet();
}