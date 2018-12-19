/*
* Author: David Wong
* Email: david.wongcascante@digipen.edu
* File name: Graphics.cpp
* Description: Write the functionality of the window and the renderer under the same class
* Created: 20-Apr-2018
* Last Modified: 29-Oct-2018
*/

// Includes //
#include "Graphics.h"

#include <iostream>			// IO streams

#include "OpenGLBundle.h"	// OpenGL operations
#include "DrawProgram.h"	// Draw Program
#include "RenderJob.h"		// Render Jobs
#include "LinearMath.h"		// Linear Math Helper Functions
#include "Input.h"			// Class Input
#include "Texture.h"		// Texture
#include "Vec3.h"			// Vec3
#include "Mat3.h"			// Mat3

// Public Member Functions //

Junior::Graphics::Graphics()
	: GameSystem("Graphics")
{
}

bool Junior::Graphics::Load()
{
	// Load the window and the context
	// Set the default window width and height
	windowWidth_ = 1920;
	windowHeight_ = 1080;

	// Initialize GLFW so that we can use its library
	if (!glfwInit())
	{
#ifdef DEBUG
		std::cout << "Failed to create the window" << std::endl;
#endif
		return 0;
	}

	// Set the window's hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the window
	windowHandle_ = glfwCreateWindow(windowWidth_, windowHeight_, "Junior Game Engine", NULL, NULL);
	if (!windowHandle_)
	{
		return 0;
	}

	// Make the window's context
	glfwMakeContextCurrent(windowHandle_);

	// Initialize GLAD after we created the context
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
#ifdef DEBUG
		std::cout << "Failed to initialize GLEW" << std::endl;
#endif
		return 0;
	}

	// Set up the input callbacks
	glfwSetKeyCallback(windowHandle_, Junior::KeyCallback);
	glfwSetMouseButtonCallback(windowHandle_, Junior::MouseButtonCallback);
	glfwSetCursorPosCallback(windowHandle_, Junior::MouseCursorCallback);
	glfwSetScrollCallback(windowHandle_, Junior::MouseScrollCallback);
	glfwSetWindowSizeCallback(windowHandle_, WindowResizeCallback);

	// Set the back buffer's clear color
	glClearColor(0.2f, 0.4f, 0.6f, 1);

	// Set up a pointer to make sure we can access this instance of the class whenever we need to change something graphics-wise
	glfwSetWindowUserPointer(windowHandle_, this);

	// defaultProgram_ = 0;
	// Set up the draw program used to draw the triangle
	defaultProgram_ = new /*(manager_->Allocate(sizeof(Junior::DrawProgram)))*/ Junior::DrawProgram;
	defaultProgram_->LoadFromDisk("..//..//Junior_Core//Src//Shaders//starter");
	// Generate the texture atlas
	//textureAtlas_ = new /*(manager_->Allocate(sizeof(Junior::Texture)))*/ Junior::Texture;
	textureAtlas_ = new Texture;
	textureAtlas_->LoadFromDisk("..//..//Junior_Core//Src//Private_Images//Logo.png");

	return true;
}

bool Junior::Graphics::Initialize()
{
	// Set up the Vertex Array Object to draw a single triangle
	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);

	glGenBuffers(1, &transformationBuffer_);

	glBindVertexArray(vao_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);

	float randomVertices[] = {
		// Vertices,			Texture Coords
		-0.5f,-0.5f, 0.0f,		0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(randomVertices), randomVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void*>(3 * sizeof(float)));

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 0);

	glBindBuffer(GL_ARRAY_BUFFER, transformationBuffer_);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(Junior::Mat3), 0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(Junior::Mat3), reinterpret_cast<void*>(sizeof(float) * 4));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, false, sizeof(Junior::Mat3), reinterpret_cast<void*>(sizeof(float) * 2 * 4));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, false, sizeof(Junior::Mat3), reinterpret_cast<void*>(sizeof(float) * 3 * 4));

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Set the orthographic matrix at the start
	orthographicMatrix_ = Orthographic(-windowWidth_/2.0f, windowWidth_/2.0f, windowHeight_/2.0f, -windowHeight_/2.0f, -5.0f, 5.0f);
	//orthographicMatrix_ = Perspective(90.0f, static_cast<float>(windowWidth_), static_cast<float>(windowHeight_), 0.01f, 1000.f);
	//orthographicMatrix_ = Identity();

	return true;
}

bool Junior::Graphics::WindowRequestClosed()
{
	return glfwWindowShouldClose(windowHandle_);
}

void Junior::Graphics::SetDimensions(int width, int height)
{
	this->windowWidth_ = width;
	this->windowHeight_ = height;
	float aspectRatio = (float)windowHeight_ / (float)windowWidth_;
	orthographicMatrix_ = Orthographic(-windowWidth_ / 2.0f, windowWidth_ / 2.0f, windowHeight_ / 2.0f, -windowHeight_ / 2.0f, -5.0f, 5.0f);
	//orthographicMatrix_ = Perspective(90.0f, static_cast<float>(width), static_cast<float>(height), 0.01f, 1000.f);
	glViewport(0, 0, width, height);
}

void Junior::Graphics::Update(double)
{
}

void Junior::Graphics::Render()
{
	// Clear the back buffer if necessary
	// Which for now, its pretty much always
	glClear(GL_COLOR_BUFFER_BIT);

	// Bind the program and draw the triangle
	
	if (defaultProgram_)
	{
#if 0
		BindProgram(defaultProgram_);
		GLuint projLoc = glGetUniformLocation(defaultProgram_->programID_, "orthographic");
		GLuint translationLoc = glGetUniformLocation(defaultProgram_->programID_, "translation");
		GLuint scalingLoc = glGetUniformLocation(defaultProgram_->programID_, "scaling");
		GLuint rotationLoc = glGetUniformLocation(defaultProgram_->programID_, "rotation");

		for (RenderJob* renderJob : renderJobs_)
		{
			glBindVertexArray(vao_);
			if (renderJob->translation_)
			{
				glUniform3fv(translationLoc, 1, renderJob->translation_->m_);
			}

			if (renderJob->scaling_)
			{
				glUniform3fv(scalingLoc, 1, renderJob->scaling_->m_);
			}

			if (renderJob->rot_)
			{
				glUniform1fv(rotationLoc, 1, renderJob->rot_);
			}
			//Set up the orthographic matrix
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, orthographicMatrix_.m_);

			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		}
#endif
#if 1
		// BindProgram(defaultProgram_);
		defaultProgram_->Bind();
		// Do the instanced rendering
		// Clear all of the previous instanced data
		renderJobTransformations_.clear();

		for (unsigned i = 0; i < renderJobs_.size(); ++i)
		{
			if (renderJobs_[i]->transformation_ == nullptr)
			{
				// Then subtract i by one, get rid of the render job, and move on
				renderJobs_.erase(renderJobs_.begin() + i);
				--i;
				continue;
			}
			// Fill out per matrix
			renderJobTransformations_.push_back(*renderJobs_[i]->transformation_);
		}

		// Bind the translation, scale, and rotation buffers
		// Use glBufferData to put new info
		glBindVertexArray(vao_);

		size_t bufferSize = sizeof(Mat3) * renderJobTransformations_.size();
		glBindBuffer(GL_ARRAY_BUFFER, transformationBuffer_);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, renderJobTransformations_.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Render the all of the render jobs (instanced)
		GLuint projLoc = glGetUniformLocation(defaultProgram_->programID_, "orthographic");
		glUniformMatrix4fv(projLoc, 1, GL_TRUE, orthographicMatrix_.m_);

		// Set the temporal texture
		GLuint textureLoc = glGetUniformLocation(defaultProgram_->programID_, "diffuse");
		glUniform1i(textureLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		textureAtlas_->BindTexture();
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(renderJobs_.size()));

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(5);

		textureAtlas_->UnbindTexture();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
#endif
	}

	// And swap the buffers
	glfwSwapBuffers(windowHandle_);
}

void Junior::Graphics::PollWindow()
{
	glfwPollEvents();
}

void Junior::Graphics::Shutdown()
{
	// Delete all the Vertex Buffer stuff
	glDeleteBuffers(1, &transformationBuffer_);
	glDeleteBuffers(1, &vbo_);
	glDeleteVertexArrays(1, &vao_);
}

void Junior::Graphics::Unload()
{
	// Delete the shader program
	defaultProgram_->CleanUp();
	delete defaultProgram_;
	//manager_->DeAllocate(defaultProgram_);
	// Delete the texture
	textureAtlas_->CleanUp();
	delete textureAtlas_;
	//manager_->DeAllocate(textureAtlas_);
	// Terminate all the GLFW stuff
	glfwWindowShouldClose(windowHandle_);
	glfwTerminate();
}

Junior::RenderJob * Junior::Graphics::GetNewRenderJob()
{
	RenderJob* newJob = new /*(manager_->Allocate(sizeof(RenderJob)))*/ RenderJob();
	newJob->transformation_ = new Mat3;
	renderJobs_.push_back(newJob);
	return newJob;
}

Junior::GameSystem& Junior::Graphics::GetInstance()
{
	static Graphics graphics;
	return graphics;
}

void Junior::WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	// Change the dimensions of the window via the pointer
	Graphics* g = (Graphics*) glfwGetWindowUserPointer(window);
	g->SetDimensions(width, height);
}