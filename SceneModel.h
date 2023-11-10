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
	SceneModel(float x, float y, float z);
	~SceneModel();

	// routine that updates the scene for the next frame
	void Update();

	// routine to tell the scene to render itself
	void Render();

	// Create the random directions for the particles up to max count
	void RandomDirections();

	// Camera is part of the scene so we can switch between cameras
	// using boolean
	void SwitchCamera();

	Camera* m_camera;
	std::vector<Particle*> particles;
	std::vector<Cartesian3> random_directions;
	QElapsedTimer timer;
	std::vector<Plane*> planes;
	Plane* m_player;
	float deltaTime;
	int lastIndex = 0;
	bool m_switchCamera;
	
	}; // class SceneModel

#endif
