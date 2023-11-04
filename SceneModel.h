///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	SceneModel.h
//	------------------------
//	
//	The model of the scene
//	
///////////////////////////////////////////////////

#ifndef __SCENE_MODEL_H
#define __SCENE_MODEL_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "HomogeneousFaceSurface.h"
#include "Terrain.h"

#include "Matrix4.h"
#include "Quaternion.h"
#include "Particle.h"
#include "Plane.h"
#include "Camera.h"

#include <random>
#include <functional>
#include "Random.h"
#include <memory>

#include <QElapsedTimer>
#define RANDOM_AMOUNT 100

class SceneModel										
	{ // class SceneModel
	public:	
	// we'll have three geometric surfaces: one for the ground (and volcano)
	// one for the plane 
	// and one for the lava bomb(s)
	Terrain groundModel;
	HomogeneousFaceSurface planeModel;
	HomogeneousFaceSurface lavaBombModel;
	HomogeneousFaceSurface terrainAABBB;

	// a matrix that specifies the mapping from world coordinates to those assumed
	// by OpenGL
	columnMajorMatrix WorldMatrix;
	columnMajorMatrix viewMatrix;
	
	// constructor
	SceneModel();
	~SceneModel();

	// routine that updates the scene for the next frame
	void Update();

	// routine to tell the scene to render itself
	void Render();

	float movement = 500.0f;
	float rot = 6.0f;
	float speed = 1.0f;
	// adding pitch, yaw and has ruined the movement
	void Forward() { cameraPos = cameraPos + movement * cameraTarget;}
	void LeftTurn() {yaw -= rot; }
	void RightTurn() {yaw += rot; }
	void PitchUp() { pitch += rot; }
	void PitchDown() { pitch -= rot; }
	void Back() { cameraPos = cameraPos - movement * cameraTarget; }

	void IncreaseSpeed() { movement >= 299.0f ? movement == movement : movement += 10.0f; std::cout << "Speed increased: " << movement << std::endl; }
	void DecreaseSpeed() { movement <= 20.0f ? movement == movement : movement -= 10.0f; std::cout << "Speed decrease: " << movement << std::endl; }

	void calculateDirection();
	void reduce()
	{
		for(auto& plane : planes)
		{
			plane->radius = plane->radius += 1.0f;
		}
	}
	void CreateParticles();
	void RandomDirections();

	Camera* m_camera;

	columnMajorMatrix objectModelMatrix;
	Cartesian3 planepos;

	std::vector<Particle*> particles;
	std::vector<Cartesian3> random_directions;
	float deltaTime;
	float oldTime;
	int lastIndex = 0;
	QElapsedTimer timer;
	std::vector<Plane*> planes;
	
	}; // class SceneModel

#endif
