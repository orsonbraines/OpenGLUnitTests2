// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <common/shader.hpp>

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

#define DEBUG_OPENGL(x) { x; GLenum error = glGetError(); if (error) { std::cerr << "Error on line " << __LINE__ << " : " << (int) error << std::endl; exit(1); } }


int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Team Objective Test 3", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// During init, enable debug output
	// glEnable(GL_DEBUG_OUTPUT);
	// glDebugMessageCallback(MessageCallback, 0);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	while (glGetError()) {}

	// Dark blue background
	DEBUG_OPENGL(glClearColor(0.0f, 0.0f, 0.4f, 0.0f));

	GLuint VertexArrayID;
	DEBUG_OPENGL(glGenVertexArrays(1, &VertexArrayID));
	DEBUG_OPENGL(glBindVertexArray(VertexArrayID));

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );


	static GLfloat g_vertex_buffer_data[] = { 
		-0.2f, 0.4f, 0.1f, // Triangle 1
		 0.2f, 0.4f, 0.2f, // Triangle 1
		 0.0f,  0.9f, 0.3f, // Triangle 1
		-0.5f, -0.3f, 0.1f, // Triangle 2
		 -0.1f, -0.3f, 0.2f, // Triangle 2
		 -0.3f,  0.3f, 0.3f, // Triangle 2
		 0.1f, -0.3f, 0.1f, // Triangle 3
		 0.5f, -0.3f, 0.2f, // Triangle 3
		 0.3f,  0.3f, 0.3f, // Triangle 3
		-0.9f, -1.0f, 0.1f, // Triangle 4
		 -0.5f, -1.0f, 0.2f, // Triangle 4
		 -0.7f,  -0.4f, 0.3f, // Triangle 4
		 -0.4f, -1.0f, 0.1f, // Triangle 5
		 -0.0f, -1.0f, 0.2f, // Triangle 5
		 -0.2f,  -0.4f, 0.3f, // Triangle 5
		0.1f, -1.0f, 0.1f, // Triangle 6
		 0.5f, -1.0f, 0.2f, // Triangle 6
		 0.3f,  -0.4f, 0.3f, // Triangle 6
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

	GLuint colourLoc = glGetUniformLocation(programID, "uColour");

	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	GLuint textures[4];
	DEBUG_OPENGL(glGenTextures(4, textures));

	DEBUG_OPENGL(glBindTexture(GL_TEXTURE_2D, textures[0]));
	GLenum internalFormat = GL_RGBA8;
	GLenum attachment = GL_COLOR_ATTACHMENT0;

	DEBUG_OPENGL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 1024, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
	//DEBUG_OPENGL(glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	//DEBUG_OPENGL(glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DEBUG_OPENGL(glFramebufferTexture(GL_FRAMEBUFFER, attachment, textures[0], 0));

	DEBUG_OPENGL(glBindTexture(GL_TEXTURE_2D, textures[1]));
	internalFormat = GL_DEPTH_COMPONENT16;
	attachment = GL_DEPTH_ATTACHMENT;

	DEBUG_OPENGL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 1024, 768, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr));
	DEBUG_OPENGL(glFramebufferTexture(GL_FRAMEBUFFER, attachment, textures[1], 0));

	DEBUG_OPENGL(glBindTexture(GL_TEXTURE_2D, textures[2]));
	internalFormat = GL_DEPTH_COMPONENT24;
	attachment = GL_DEPTH_ATTACHMENT;

	DEBUG_OPENGL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 1024, 768, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr));

	DEBUG_OPENGL(glBindTexture(GL_TEXTURE_2D, textures[3]));
	internalFormat = GL_DEPTH_COMPONENT32F;
	attachment = GL_DEPTH_ATTACHMENT;

	DEBUG_OPENGL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 1024, 768, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr));

//	internalFormat = GL_DEPTH_COMPONENT32F;
//	attachment = GL_DEPTH_ATTACHMENT;

	DEBUG_OPENGL(GLenum check = glCheckFramebufferStatus(GL_FRAMEBUFFER); std::cerr << "GL check: " << check << std::endl; assert(check == GL_FRAMEBUFFER_COMPLETE););

	do{
		DEBUG_OPENGL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

		// Clear the screen
		DEBUG_OPENGL(glClear( GL_COLOR_BUFFER_BIT));

		// Use our shader
		DEBUG_OPENGL(glUseProgram(programID));

		// 1rst attribute buffer : vertices
		DEBUG_OPENGL(glEnableVertexAttribArray(0));
		DEBUG_OPENGL(glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer));
		DEBUG_OPENGL(glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		));


		DEBUG_OPENGL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textures[1], 0));
		DEBUG_OPENGL(glClear(GL_DEPTH_BUFFER_BIT));
		DEBUG_OPENGL(glUniform3f(colourLoc, 0.3f, 0.3f, 0.3f));
		DEBUG_OPENGL(glDrawArrays(GL_TRIANGLES, 0, 3)); // 6 indices starting at 0 -> 1 triangle

		DEBUG_OPENGL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textures[2], 0));
		DEBUG_OPENGL(glClear(GL_DEPTH_BUFFER_BIT));
		DEBUG_OPENGL(glUniform3f(colourLoc, 0.5f, 0.0f, 0.0f));
		DEBUG_OPENGL(glDrawArrays(GL_TRIANGLES, 3, 3)); 
		DEBUG_OPENGL(glDrawArrays(GL_TRIANGLES, 6, 3));

		DEBUG_OPENGL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textures[3], 0));
		DEBUG_OPENGL(glClear(GL_DEPTH_BUFFER_BIT));
		DEBUG_OPENGL(glUniform3f(colourLoc, 0.0f, 0.5f, 0.0f));
		DEBUG_OPENGL(glDrawArrays(GL_TRIANGLES, 9, 3));
		DEBUG_OPENGL(glDrawArrays(GL_TRIANGLES, 12, 3));
		DEBUG_OPENGL(glDrawArrays(GL_TRIANGLES, 15, 3));

		DEBUG_OPENGL(glDisableVertexAttribArray(0));

		DEBUG_OPENGL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
		DEBUG_OPENGL(glBlitFramebuffer(0, 0, 1024, 768, 0, 0, 1024, 768, GL_COLOR_BUFFER_BIT /* | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT*/, GL_NEAREST));

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		// std::cout << (int) glGetError() << std::endl;
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);
	assert(glGetError() == 0);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

