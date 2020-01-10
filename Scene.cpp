#include "Scene.h"
#include "Window.h"

std::vector<Transform*> capsuleTransforms;
Transform* sphereTransform;

Scene::Scene() {
	//setup a camera
	Camera* mainCamera = new Camera(glm::vec3(0, 0, 20), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 100, 0.001, 100);
	cameraList.push_back(mainCamera);
	activeCamera = mainCamera;
	
	//setup basic transforms
	sceneTransform = new Transform(glm::mat4(1));

	//load in sphere
	Material* sphereMaterial = new Material("textures/orb.jpg", glm::vec3(), 1, glm::vec3(1,1,1), 0.1f, 2.0f, 0.2f, 4.0f);
	Geometry* sphere = new Geometry("models/blob.obj", sphereMaterial);
	glm::mat4 mSphereTranslation = glm::translate(glm::mat4(1), glm::vec3(0, 15, 0));
	sphereTransform = new Transform(mSphereTranslation * glm::scale(glm::mat4(1), glm::vec3(0.5, 0.5, 0.5)));
	sphereTransform->addChild(sphere);
	sceneTransform->addChild(sphereTransform);
	
	//load in capsule
	Material* capsuleMaterial = new Material("textures/stone.jpg", glm::vec3(), 1, glm::vec3(1,1,1), 0.1f, 2.0f, 0.2f, 0.0f);
	Geometry* capsule = new Geometry("models/capsule.obj", capsuleMaterial);
	glm::mat4 mCapsuleTranslation = glm::translate(glm::mat4(1), glm::vec3(5, 10, 5));
	Transform *capsuleTransform = new Transform(mCapsuleTranslation * glm::scale(glm::mat4(1), glm::vec3(0.1, 0.1, 0.1)));
	capsuleTransform->addChild(capsule);
	sceneTransform->addChild(capsuleTransform);
	capsuleTransforms.push_back(capsuleTransform);

	//load in another capsule
	glm::mat4 mCapsuleRotation = glm::rotate(glm::mat4(1), 2.0f, glm::vec3(0, 1, 0));
	mCapsuleTranslation = glm::translate(glm::mat4(1), glm::vec3(-5, 12, 3));
	capsuleTransform = new Transform(mCapsuleTranslation * mCapsuleRotation * glm::scale(glm::mat4(1), glm::vec3(0.1, 0.1, 0.1)));
	capsuleTransform->addChild(capsule);
	sceneTransform->addChild(capsuleTransform);
	capsuleTransforms.push_back(capsuleTransform);

	//another one
	mCapsuleRotation = glm::rotate(glm::mat4(1), 2.8f, glm::vec3(0, 1, 0));
	mCapsuleTranslation = glm::translate(glm::mat4(1), glm::vec3(-6, 12, 0));
	capsuleTransform = new Transform(mCapsuleTranslation * mCapsuleRotation * glm::scale(glm::mat4(1), glm::vec3(0.1, 0.1, 0.1)));
	capsuleTransform->addChild(capsule);
	sceneTransform->addChild(capsuleTransform);
	capsuleTransforms.push_back(capsuleTransform);

	//another one
	mCapsuleRotation = glm::rotate(glm::mat4(1), 6.0f, glm::vec3(0, 1, 0));
	mCapsuleTranslation = glm::translate(glm::mat4(1), glm::vec3(4, 6, -3));
	capsuleTransform = new Transform(mCapsuleTranslation * mCapsuleRotation * glm::scale(glm::mat4(1), glm::vec3(0.1, 0.1, 0.1)));
	capsuleTransform->addChild(capsule);
	sceneTransform->addChild(capsuleTransform);
	capsuleTransforms.push_back(capsuleTransform);

	//another one
	mCapsuleRotation = glm::rotate(glm::mat4(1), 1.0f, glm::vec3(0, 1, 0));
	mCapsuleTranslation = glm::translate(glm::mat4(1), glm::vec3(-4, 7, 4));
	capsuleTransform = new Transform(mCapsuleTranslation * mCapsuleRotation * glm::scale(glm::mat4(1), glm::vec3(0.1, 0.1, 0.1)));
	capsuleTransform->addChild(capsule);
	sceneTransform->addChild(capsuleTransform);
	capsuleTransforms.push_back(capsuleTransform);

	//load in actual terrain
	Terrain* generatedTerrain = new Terrain("textures/heightmaps/compel.png", "", 2047, 2047, 0.15f);
	//Terrain* generatedTerrain = new Terrain("text", "", 4, 4, 5.0f);
	glm::mat4 mTerrainTranslation = glm::translate(glm::mat4(1), glm::vec3(-50, 0, -50));
	Transform* terrainTransform = new Transform(mTerrainTranslation * glm::scale(glm::mat4(1), glm::vec3(100, 100, 100)));
	terrainTransform->addChild(generatedTerrain);
	sceneTransform->addChild(terrainTransform);

	//flipped terrain
	/*mTerrainTranslation = glm::translate(glm::mat4(1), glm::vec3(-50, -20, -50));
	glm::mat4 mTerrainRotation = glm::rotate(glm::mat4(1), 3.141f, glm::vec3(1,0,0));
	terrainTransform = new Transform(mTerrainRotation * mTerrainTranslation * glm::scale(glm::mat4(1), glm::vec3(100, 100, 100)));
	terrainTransform->addChild(generatedTerrain);
	sceneTransform->addChild(terrainTransform);*/

	//setup skybox for background
	std::vector<std::string> skyTextures
	{
		"textures/skyboxes/sunset/right.jpg",
		"textures/skyboxes/sunset/left.jpg",
		"textures/skyboxes/sunset/top.jpg",
		"textures/skyboxes/sunset/bottom.jpg",
		"textures/skyboxes/sunset/front.jpg",
		"textures/skyboxes/sunset/back.jpg",
	};
	skyBox = new CubeMap(skyTextures);

	//generate lights
	LightSource* mainLight = new LightSource(glm::vec3(), 4.0f * glm::vec3(1,0.84,0.65), 0, 1, glm::vec3(1.0,0,0));
	lightList.push_back(mainLight);

	LightSource* orbLight = new LightSource(glm::vec3(0, 15, 0), glm::vec3(0.3, 0.1, 0.1), 0.0005f, 0, glm::vec3());
	lightList.push_back(orbLight);

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			LightSource* ls = new LightSource(glm::vec3(i / (float)4, 30, j / (float)4), glm::vec3(i / (float)10, 1, j / (float)10), 0.1f, 0, glm::vec3());
			lightList.push_back(ls);
		}
	}

	//setup light uniforms
	for (int i = 0; i < lightList.size(); i++) {
		std::string number = std::to_string(i);
		glUseProgram(Window::deferredLightingProgram);
		glUniform3fv(glGetUniformLocation(Window::deferredLightingProgram, ("lights[" + number + "].Position").c_str()), 1, glm::value_ptr(lightList[i]->pos));
		glUniform3fv(glGetUniformLocation(Window::deferredLightingProgram, ("lights[" + number + "].Color").c_str()), 1, glm::value_ptr(lightList[i]->col));
		glUniform3fv(glGetUniformLocation(Window::deferredLightingProgram, ("lights[" + number + "].direction").c_str()), 1, glm::value_ptr(lightList[i]->direction));
		glUniform1f(glGetUniformLocation(Window::deferredLightingProgram, ("lights[" + number + "].attenuation").c_str()), lightList[i]->attenuation);
		glUniform1i(glGetUniformLocation(Window::deferredLightingProgram, ("lights[" + number + "].directional").c_str()), lightList[i]->directional);
	}
}

Scene::~Scene() {}

//RENDER THE SCENE
void Scene::draw() {
	sceneTransform->draw(sceneTransform->M, glm::mat4(1));
	
	//update the cameras last view
	activeCamera->lastFrameView = activeCamera->view;
}

void Scene::update(float deltaTime) {
	glm::mat4 mRotation = glm::rotate(glm::mat4(1), 0.3f * deltaTime, glm::vec3(0, 1, 0));
	glm::mat4 mTranslation = glm::translate(glm::mat4(1), 0.15f * deltaTime * glm::vec3(0,0,0));
	for (int i = 0; i < capsuleTransforms.size(); i++) {
		capsuleTransforms.at(i)->updateModel(mTranslation * mRotation * capsuleTransforms.at(i)->M);
	}

	mRotation = glm::rotate(glm::mat4(1), 0.1f * deltaTime, glm::vec3(0, 1, 0));
	sphereTransform->updateModel(mRotation * sphereTransform->M);
}