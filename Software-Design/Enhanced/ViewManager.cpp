///////////////////////////////////////////////////////////////////////////////
// viewmanager.cpp
// ===============
// manage the viewing of 3D objects within the viewport
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "ViewManager.h"

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// declaration of the global variables and defines
namespace
{
	// Variables for window width and height
	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 800;

	const char* g_ViewName = "view";
	const char* g_ProjectionName = "projection";

	// Camera object used for viewing and interacting with the 3D scene
	Camera* g_pCamera = nullptr;

	// Variables used for mouse movement processing
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;

	// Time between the current frame and the previous frame
	float gDeltaTime = 0.0f;
	float gLastFrame = 0.0f;

	// Controls the speed of keyboard camera movement
	float gMovementSpeed = 1.0f;

	// False uses perspective projection and true uses orthographic projection
	bool bOrthographicProjection = false;

	// Tracks whether the arcade interaction key was pressed
	bool gArcadeInteractionRequested = false;

	// Prevents one key press from triggering repeatedly every frame
	bool gInteractionKeyPressed = false;
}

/***********************************************************
 *  ViewManager()
 *
 *  The constructor for the class
 ***********************************************************/
ViewManager::ViewManager(
	ShaderManager* pShaderManager)
{
	// Initialize the member variables
	m_pShaderManager = pShaderManager;
	m_pWindow = NULL;
	g_pCamera = new Camera();

	// Default camera view parameters
	g_pCamera->Position = glm::vec3(0.0f, 5.0f, 12.0f);
	g_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);
	g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
	g_pCamera->Zoom = 80.0f;
}

/***********************************************************
 *  ~ViewManager()
 *
 *  The destructor for the class
 ***********************************************************/
ViewManager::~ViewManager()
{
	// Free allocated memory
	m_pShaderManager = NULL;
	m_pWindow = NULL;

	if (NULL != g_pCamera)
	{
		delete g_pCamera;
		g_pCamera = NULL;
	}
}

/***********************************************************
 *  CreateDisplayWindow()
 *
 *  This method is used to create the main display window.
 ***********************************************************/
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
	GLFWwindow* window = nullptr;

	// Try to create the displayed OpenGL window
	window = glfwCreateWindow(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		windowTitle,
		NULL,
		NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);

	// The cursor can remain visible while testing the scene.
	// Remove the comment below to capture and hide the cursor.
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Receive mouse-position events
	glfwSetCursorPosCallback(
		window,
		&ViewManager::Mouse_Position_Callback);

	// Receive mouse-wheel events
	glfwSetScrollCallback(
		window,
		&ViewManager::Mouse_Scroll_Callback);

	// Enable blending to support transparent rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pWindow = window;

	return window;
}

/***********************************************************
 *  Mouse_Position_Callback()
 *
 *  This method is automatically called from GLFW whenever
 *  the mouse is moved within the active display window.
 ***********************************************************/
void ViewManager::Mouse_Position_Callback(
	GLFWwindow* window,
	double xMousePos,
	double yMousePos)
{
	// Prevent a sudden camera movement during the first mouse event
	if (gFirstMouse)
	{
		gLastX = static_cast<float>(xMousePos);
		gLastY = static_cast<float>(yMousePos);
		gFirstMouse = false;
	}

	// Calculate how far the mouse moved since the previous event
	float xOffset = static_cast<float>(xMousePos) - gLastX;
	float yOffset = gLastY - static_cast<float>(yMousePos);

	// Store the current position for the next mouse event
	gLastX = static_cast<float>(xMousePos);
	gLastY = static_cast<float>(yMousePos);

	// Change the camera orientation based on mouse movement
	if (NULL != g_pCamera)
	{
		g_pCamera->ProcessMouseMovement(xOffset, yOffset);
	}
}

/***********************************************************
 *  Mouse_Scroll_Callback()
 *
 *  This method adjusts the speed at which the camera moves
 *  when the mouse wheel is scrolled.
 ***********************************************************/
void ViewManager::Mouse_Scroll_Callback(
	GLFWwindow* window,
	double xOffset,
	double yOffset)
{
	// Scroll upward to increase movement speed
	if (yOffset > 0.0)
	{
		gMovementSpeed += 0.25f;
	}

	// Scroll downward to decrease movement speed
	if (yOffset < 0.0)
	{
		gMovementSpeed -= 0.25f;
	}

	// Keep the movement speed within a usable range
	if (gMovementSpeed < 0.25f)
	{
		gMovementSpeed = 0.25f;
	}

	if (gMovementSpeed > 5.0f)
	{
		gMovementSpeed = 5.0f;
	}
}

/***********************************************************
 *  ProcessKeyboardEvents()
 *
 *  This method processes keyboard input used to move the
 *  camera and switch between projection modes.
 ***********************************************************/
void ViewManager::ProcessKeyboardEvents()
{
	// Close the window when Escape is pressed
	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_pWindow, true);
	}

	if (NULL == g_pCamera)
	{
		return;
	}

	// Apply the mouse-wheel speed setting to keyboard movement
	float adjustedDeltaTime = gDeltaTime * gMovementSpeed;

	// Move forward and backward through the scene
	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(FORWARD, adjustedDeltaTime);
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(BACKWARD, adjustedDeltaTime);
	}

	// Move left and right through the scene
	if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(LEFT, adjustedDeltaTime);
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(RIGHT, adjustedDeltaTime);
	}

	// Move vertically upward
	if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		g_pCamera->Position +=
			g_pCamera->Up * adjustedDeltaTime * 2.5f;
	}

	// Move vertically downward
	if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		g_pCamera->Position -=
			g_pCamera->Up * adjustedDeltaTime * 2.5f;
	}

	// Use perspective projection when P is pressed
	if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS)
	{
		bOrthographicProjection = false;
	}

	// Use orthographic projection when O is pressed
	if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS)
	{
		bOrthographicProjection = true;
	}

	// Request interaction with the arcade cabinet when F is pressed
	if (glfwGetKey(m_pWindow, GLFW_KEY_F) == GLFW_PRESS)
	{
		if (!gInteractionKeyPressed)
		{
			gArcadeInteractionRequested = true;
			gInteractionKeyPressed = true;
		}
	}
	else
	{
		gInteractionKeyPressed = false;
	}
}

/***********************************************************
 *  PrepareSceneView()
 *
 *  This method prepares the camera view and projection
 *  matrices used to render the 3D scene.
 ***********************************************************/
void ViewManager::PrepareSceneView()
{
	glm::mat4 view;
	glm::mat4 projection;

	// Calculate the amount of time between rendered frames
	float currentFrame = static_cast<float>(glfwGetTime());
	gDeltaTime = currentFrame - gLastFrame;
	gLastFrame = currentFrame;

	// Process keyboard input
	ProcessKeyboardEvents();

	// Keep the same camera position and orientation
	// for both perspective and orthographic views
	view = g_pCamera->GetViewMatrix();

	if (bOrthographicProjection)
	{
		// Orthographic projection
		projection = glm::ortho(
			-10.0f,
			10.0f,
			-8.0f,
			8.0f,
			0.1f,
			100.0f);
	}
	else
	{
		// Perspective projection
		projection = glm::perspective(
			glm::radians(g_pCamera->Zoom),
			static_cast<GLfloat>(WINDOW_WIDTH) /
			static_cast<GLfloat>(WINDOW_HEIGHT),
			0.1f,
			100.0f);
	}

	// Send the view and projection information to the shader
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(
			g_ViewName,
			view);

		m_pShaderManager->setMat4Value(
			g_ProjectionName,
			projection);

		m_pShaderManager->setVec3Value(
			"viewPosition",
			g_pCamera->Position);
	}
}

/***********************************************************
 *  GetCameraPosition()
 *
 *  Returns the current position of the scene camera.
 ***********************************************************/
glm::vec3 ViewManager::GetCameraPosition()
{
	if (NULL != g_pCamera)
	{
		return g_pCamera->Position;
	}

	return glm::vec3(0.0f);
}

/***********************************************************
 *  GetArcadeInteractionRequested()
 *
 *  Returns true once when the user presses the arcade
 *  interaction key.
 ***********************************************************/
bool ViewManager::GetArcadeInteractionRequested()
{
	if (gArcadeInteractionRequested)
	{
		gArcadeInteractionRequested = false;
		return true;
	}

	return false;
}