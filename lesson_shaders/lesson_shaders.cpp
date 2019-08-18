// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include "shader_class.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;




/*const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  \n"
    "void main()\n"
    "{\n"
    "   FragColor = vertexColor;\n"
    "}\n\0";*/



int main()
{
    // glfw: initialize and configure
    // ------------------------------
	glfwInit();
	//the following 3 lines define opengl version 4.5 and the core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	 Shader ourShader("shaders/shader.vs", "shaders/shader.fs"); // you can name your shader files however you like

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
    	//positions				//colors
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // top
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f, // bottom left
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//define the position attribute for the shaders
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  
	//define the color attribute for the shaders
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);  

    //filled or wireframe triangles
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	while(!glfwWindowShouldClose(window))
	{        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.setFloat("horizontatlPos", -0.1f);
        ourShader.use();

	    //update 'ourColor' uniform location
	    /*
	    float timeValue = glfwGetTime();
	    float greenValue = (sin(timeValue)/2.0f) + 0.5f;
	    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
	    glUseProgram(shaderProgram);
	    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);*/

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
	}

	return 0;
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