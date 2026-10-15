///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
	m_arcadeScreenActive = false;
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/


/***********************************************************
 *  LoadSceneTextures()
 *
 *  Load and bind the texture images used in the arcade scene.
 ***********************************************************/
void SceneManager::LoadSceneTextures()
{
	CreateGLTexture(
		"textures/CabinetBody.jpg",
		"cabinetBody");

	CreateGLTexture(
		"textures/ControlPanel.jpg",
		"controlPanel");

	CreateGLTexture(
		"textures/ArcadeMarquee.jpg",
		"arcadeMarquee");

	CreateGLTexture(
		"textures/ArcadeCarpet.jpg",
		"arcadeCarpet");

	CreateGLTexture(
		"textures/GameOver.jpg",
		"gameOver");

	CreateGLTexture(
		"textures/StartScreen.jpg",
		"startScreen");

	BindGLTextures();
}

/***********************************************************
 *  DefineObjectMaterials()
 *
 *  Define the materials used in the arcade scene
 ***********************************************************/
void SceneManager::DefineObjectMaterials()
{
	OBJECT_MATERIAL arcadeMaterial;

	arcadeMaterial.ambientColor = glm::vec3(0.3f, 0.3f, 0.3f);
	arcadeMaterial.ambientStrength = 0.4f;
	arcadeMaterial.diffuseColor = glm::vec3(0.8f, 0.8f, 0.8f);
	arcadeMaterial.specularColor = glm::vec3(0.3f, 0.3f, 0.3f);
	arcadeMaterial.shininess = 16.0f;
	arcadeMaterial.tag = "arcadeMaterial";

	m_objectMaterials.push_back(arcadeMaterial);
}

/***********************************************************
 *  SetupSceneLights()
 *
 *  Configure the lighting used in the arcade scene
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	m_pShaderManager->setBoolValue("bUseLighting", true);

	// Light source 1 - main white light above and to the right
	m_pShaderManager->setVec3Value(
		"lightSources[0].position",
		5.0f, 10.0f, 5.0f);

	m_pShaderManager->setVec3Value(
		"lightSources[0].ambientColor",
		0.15f, 0.15f, 0.15f);

	m_pShaderManager->setVec3Value(
		"lightSources[0].diffuseColor",
		0.45f, 0.45f, 0.45f);

	m_pShaderManager->setVec3Value(
		"lightSources[0].specularColor",
		0.35f, 0.35f, 0.35f);

	m_pShaderManager->setFloatValue(
		"lightSources[0].focalStrength",
		32.0f);

	m_pShaderManager->setFloatValue(
		"lightSources[0].specularIntensity",
		0.25f);

	// Light source 2 - soft blue light from the left
	m_pShaderManager->setVec3Value(
		"lightSources[1].position",
		-5.0f, 6.0f, 4.0f);

	m_pShaderManager->setVec3Value(
		"lightSources[1].ambientColor",
		0.05f, 0.07f, 0.10f);

	m_pShaderManager->setVec3Value(
		"lightSources[1].diffuseColor",
		0.15f, 0.25f, 0.40f);

	m_pShaderManager->setVec3Value(
		"lightSources[1].specularColor",
		0.15f, 0.25f, 0.40f);

	m_pShaderManager->setFloatValue(
		"lightSources[1].focalStrength",
		32.0f);

	m_pShaderManager->setFloatValue(
		"lightSources[1].specularIntensity",
		0.20f);

	// Light source 3 - fill light from the front
	m_pShaderManager->setVec3Value(
		"lightSources[2].position",
		0.0f, 5.0f, 8.0f);

	m_pShaderManager->setVec3Value(
		"lightSources[2].ambientColor",
		0.08f, 0.08f, 0.08f);

	m_pShaderManager->setVec3Value(
		"lightSources[2].diffuseColor",
		0.10f, 0.10f, 0.10f);

	m_pShaderManager->setVec3Value(
		"lightSources[2].specularColor",
		0.20f, 0.20f, 0.20f);

	m_pShaderManager->setFloatValue(
		"lightSources[2].focalStrength",
		32.0f);

	m_pShaderManager->setFloatValue(
		"lightSources[2].specularIntensity",
		0.15f);

	// Light source 4 - fill light from the opposite side
	m_pShaderManager->setVec3Value(
		"lightSources[3].position",
		4.0f, 4.0f, -5.0f);

	m_pShaderManager->setVec3Value(
		"lightSources[3].ambientColor",
		0.06f, 0.06f, 0.06f);

	m_pShaderManager->setVec3Value(
		"lightSources[3].diffuseColor",
		0.08f, 0.08f, 0.08f);

	m_pShaderManager->setVec3Value(
		"lightSources[3].specularColor",
		0.15f, 0.15f, 0.15f);

	m_pShaderManager->setFloatValue(
		"lightSources[3].focalStrength",
		32.0f);

	m_pShaderManager->setFloatValue(
		"lightSources[3].specularIntensity",
		0.15f);
}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	LoadSceneTextures();

	DefineObjectMaterials();
	SetupSceneLights();

	// Load each mesh once for use throughout the scene
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadSphereMesh();
}


/***********************************************************
 *  RenderEnvironment()
 *
 *  Renders the environment surrounding the arcade cabinet.
 ***********************************************************/
void SceneManager::RenderEnvironment()
{
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// ---------- Floor ----------
	scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);

	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("arcadeCarpet");
	SetTextureUVScale(4.0f, 4.0f);
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawPlaneMesh();
}

/***********************************************************
 *  RenderArcadeCabinet()
 *
 *  Renders the main structure and marquee of the
 *  arcade cabinet.
 ***********************************************************/
void SceneManager::RenderArcadeCabinet()
{
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// ---------- Main Cabinet Body ----------
	scaleXYZ = glm::vec3(2.8f, 5.0f, 2.5f);
	positionXYZ = glm::vec3(0.0f, 2.5f, 0.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("cabinetBody");
	SetTextureUVScale(1.0f, 1.0f);
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawBoxMesh();

	// ---------- Upper Cabinet Section ----------
	scaleXYZ = glm::vec3(2.8f, 2.4f, 2.5f);
	positionXYZ = glm::vec3(0.0f, 6.2f, 0.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("cabinetBody");
	SetTextureUVScale(1.0f, 1.0f);
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawBoxMesh();

	// ---------- Marquee ----------
	scaleXYZ = glm::vec3(2.45f, 0.65f, 0.18f);
	positionXYZ = glm::vec3(0.0f, 7.0f, 1.34f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("arcadeMarquee");
	SetTextureUVScale(1.0f, 1.0f);
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawBoxMesh();
}


/***********************************************************
 *  RenderArcadeScreen()
 *
 *  Renders the arcade screen and surrounding bezel.
 ***********************************************************/
void SceneManager::RenderArcadeScreen()
{
	glm::vec3 scaleXYZ;
	float XrotationDegrees = -8.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// ---------- Screen Bezel ----------
	scaleXYZ = glm::vec3(2.3f, 1.45f, 0.18f);
	positionXYZ = glm::vec3(0.0f, 5.85f, 1.34f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.05f, 0.05f, 0.05f, 1.0f);
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawBoxMesh();

	// ---------- Screen ----------
	scaleXYZ = glm::vec3(2.0f, 1.2f, 0.10f);
	positionXYZ = glm::vec3(0.0f, 5.85f, 1.46f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	if (m_arcadeScreenActive)
	{
		SetShaderTexture("gameOver");
	}
	else
	{
		SetShaderTexture("startScreen");
	}
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawBoxMesh();
}


/***********************************************************
 *  RenderArcadeControls()
 *
 *  Renders the control panel, joystick, buttons,
 *  and coin door for the arcade cabinet.
 ***********************************************************/
void SceneManager::RenderArcadeControls()
{
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// ---------- Control Panel ----------
	scaleXYZ = glm::vec3(2.9f, 0.45f, 1.25f);
	XrotationDegrees = -8.0f;
	positionXYZ = glm::vec3(0.0f, 4.65f, 1.20f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("controlPanel");
	SetTextureUVScale(1.0f, 1.0f);
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawBoxMesh();

	// Reset rotation for remaining controls
	XrotationDegrees = 0.0f;

	// ---------- Joystick Stem ----------
	scaleXYZ = glm::vec3(0.06f, 0.32f, 0.06f);
	positionXYZ = glm::vec3(-0.85f, 4.88f, 1.58f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.10f, 0.10f, 0.10f, 1.0f);
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawCylinderMesh();

	// ---------- Joystick Ball ----------
	scaleXYZ = glm::vec3(0.13f, 0.13f, 0.13f);
	positionXYZ = glm::vec3(-0.85f, 5.25f, 1.58f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.85f, 0.10f, 0.10f, 1.0f);
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawSphereMesh();

	// ---------- Button 1 ----------
	scaleXYZ = glm::vec3(0.14f, 0.16f, 0.14f);
	positionXYZ = glm::vec3(0.40f, 4.88f, 1.65f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.95f, 0.85f, 0.10f, 1.0f);
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawCylinderMesh();

	// ---------- Button 2 ----------
	scaleXYZ = glm::vec3(0.14f, 0.16f, 0.14f);
	positionXYZ = glm::vec3(0.85f, 4.88f, 1.65f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.10f, 0.85f, 0.25f, 1.0f);
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawCylinderMesh();

	// ---------- Coin Door ----------
	scaleXYZ = glm::vec3(1.0f, 1.2f, 0.12f);
	positionXYZ = glm::vec3(0.0f, 2.35f, 1.32f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.08f, 0.08f, 0.08f, 1.0f);
	SetShaderMaterial("arcadeMaterial");

	m_basicMeshes->DrawBoxMesh();
}

/***********************************************************
 *  UpdateArcadeInteraction()
 *
 *  Checks the camera's distance from the arcade cabinet
 *  and changes the screen state when interaction occurs.
 ***********************************************************/
void SceneManager::UpdateArcadeInteraction(
	glm::vec3 cameraPosition,
	bool interactionRequested)
{
	// Approximate interaction point in front of the cabinet
	glm::vec3 arcadePosition = glm::vec3(0.0f, 5.0f, 2.0f);

	// Calculate the distance between the camera and cabinet
	float distanceFromArcade =
		glm::distance(cameraPosition, arcadePosition);

	// Only allow interaction when the camera is close enough
	if (distanceFromArcade <= 4.0f && interactionRequested)
	{
		m_arcadeScreenActive = !m_arcadeScreenActive;
	}
}


/***********************************************************
 *  RenderScene()
 *
 *  Coordinates rendering of the major components of
 *  the arcade scene.
 ***********************************************************/
void SceneManager::RenderScene()
{
	RenderEnvironment();
	RenderArcadeCabinet();
	RenderArcadeScreen();
	RenderArcadeControls();
}