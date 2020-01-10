//the scene contains all the objects and lights to render and update

#pragma once

#include <vector>
#include "Transform.h"
#include "LightSource.h"
#include "Geometry.h"
#include "CubeMap.h"
#include "Camera.h"
#include "Terrain.h"
#include "Particle.h"

class Scene
{
private:
	//camera attributes
	std::vector<Camera*> cameraList;

	//object attributes
	Transform* sceneTransform;

	//various lists
	std::vector<LightSource*> lightList;
	std::vector<Particle*> particleList;

public:
	Camera* activeCamera;
	//skybox 
	CubeMap* skyBox;
	Scene();
	~Scene();
	void draw();
	void update(float deltaTime);
};

