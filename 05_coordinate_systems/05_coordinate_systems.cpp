// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include "shader_class.h"
#include "utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
STATUS init();
void showFrameRate();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

	 Shader ourShader("shaders/shader.vs", "shaders/shader.fs"); // you can name your shader files however you like

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
/*	float vertices[] = {
	    // positions          // colors           // texture coords
	     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f    // top left 
	};*/
	 float vertices[] = {
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

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    unsigned int VBO, VAO,EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//define the position attribute for the shaders
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  
	//define the color attribute for the shaders
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	// glEnableVertexAttribArray(1); 
	//define the texture coord attribute for the shaders
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);   

    //filled or wireframe triangles
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //create texture 
    unsigned int texture0;
    glGenTextures(1, &texture0);
    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, texture0);

    //set texture wraping/filtering options (on currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load and generate exture
    int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);  
    unsigned char *data = stbi_load("../images/container1.jpg", &width, &height, &nrChannels, 0);
    if(data)
    {
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    	glGenerateMipmap(GL_TEXTURE_2D);
    }else{
		cout << "[ " << __FILE__  << ":" << __FUNCTION__ << ":" << __LINE__ <<  " ] ";
		cout << "Failed to load texture" << endl;
		return ERROR;
    }
    stbi_image_free(data);


    unsigned int texture1;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, texture1);

    data = stbi_load("../images/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}else{
		cout << "[ " << __FILE__  << ":" << __FUNCTION__ << ":" << __LINE__ <<  " ] ";
		cout << "Failed to load texture" << endl;
		return ERROR;
    }
    stbi_image_free(data);

    glm::vec3 cubePositions[] = {
	  glm::vec3( 0.0f,  0.0f,  0.0f), 
	  glm::vec3( 2.0f,  5.0f, -15.0f), 
	  glm::vec3(-1.5f, -2.2f, -2.5f),  
	  glm::vec3(-3.8f, -2.0f, -12.3f),  
	  glm::vec3( 2.4f, -0.4f, -3.5f),  
	  glm::vec3(-1.7f,  3.0f, -7.5f),  
	  glm::vec3( 1.3f, -2.0f, -2.5f),  
	  glm::vec3( 1.5f,  2.0f, -2.5f), 
	  glm::vec3( 1.5f,  0.2f, -1.5f), 
	  glm::vec3(-1.3f,  1.0f, -1.5f)  
	};

	while(!glfwWindowShouldClose(window))
	{   

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     
		// input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        
        ourShader.use();
		glUniform1i(glGetUniformLocation(ourShader.ID, "texture0"), 0);
		glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 1);
		
        // create transformations
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        //rotate over time
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));  
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");
        unsigned int projLoc  = glGetUniformLocation(ourShader.ID, "projection");
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        //ourShader.setMat4("projection", projection);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		for(unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 25 * i; 
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			ourShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        showFrameRate();
	}

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
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}