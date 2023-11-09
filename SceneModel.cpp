///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	SceneModel.cpp
//	------------------------
//	
//	The model of the scene
//
//	
///////////////////////////////////////////////////

#include "SceneModel.h"
#include <math.h>
#include <chrono>

// three local variables with the hardcoded file names
const char *groundModelName 	= "./models/landscape.dem";
const char *planeModelName 		= "./models/planeModel.tri";
const char *lavaBombModelName 	= "./models/lavaBombModel.tri";

const Homogeneous4 sunDirection(0.0, 0.3, -0.3, 1.0);
const GLfloat groundColour[4] = { 0.2, 0.6, 0.2, 1.0 };
const GLfloat sunAmbient[4] = {0.2, 0.2, 0.2, 1.0 };
const GLfloat sunDiffuse[4] = {0.7, 0.7, 0.7, 1.0 };
const GLfloat blackColour[4] = {0.0, 0.0, 0.0, 1.0};
const GLfloat lavaBombColour[4] = {0.5, 0.3, 0.0, 1.0};
const GLfloat lavaBombColour2[4] = {0.0, 0.3, 1.0, 1.0};
const GLfloat planeColour[4] = {0.1, 0.1, 0.5, 1.0};
const GLfloat planeRadius = 2.0;
const GLfloat lavaBombRadius = 100.0;
const Cartesian3 chaseCamVector(0.0, -2.0, 0.5);

static auto startTime = std::chrono::high_resolution_clock::now();

auto lastSpawnTime = std::chrono::high_resolution_clock::now() - std::chrono::seconds(5);

// constructor
SceneModel::SceneModel()
	{ // constructor
	// this is not the best place to put this in general, but this is a quick and dirty hack
	// we start by loading three files: one for each model
	groundModel.ReadFileTerrainData(groundModelName, 500);
	planeModel.ReadFileTriangleSoup(planeModelName);
	lavaBombModel.ReadFileTriangleSoup(lavaBombModelName);
	
//	When modelling, z is commonly used for "vertical" with x-y used for "horizontal"
//	When rendering, the default is that we render using screen coordinates, so x is to the right,
//	y is up, and z points behind us by the right hand rule.  That means when looking into the screen,
//	we are actually looking out along the z-axis
//	That means we will have to start off with a view matrix that compensates for this
//	Assume that we want to look out along the y axis with the z axis pointing up instead
//	Then the x-axis is off to the right.  This means that our mapping is as follows:
//		x is unchanged	from WCS					to VCS
//		y was forward 	in WCS		is now up		in VCS
//  	z was up		in WCS		is now back		in VCS
//	because x is unchanged, this is a rotation around x, with y moving towards z, so it is a
//	rotation of 90 degrees CCW

	// set the world to opengl matrix

// 	auto axis = Cartesian3(1.0f, 0.0f, 0.0f);
// 	Quaternion rotation(90.0f, axis);
// 	rotation.Normalize();
// 	Quaternion result = rotation * offset * rotation.Conjugate();
// 	result.Normalize();
// 	columnMajorMatrix rot = result.ToRotationMatrix();
// * columnMajorMatrix::RotateX(180.0f)

	WorldMatrix = columnMajorMatrix::RotateX(90.0f);

	planepos = Cartesian3(0,0,0); 
	////-271.4, 3634, -2855
	m_camera = new Camera(Cartesian3(-38500, 3634, -4000), Cartesian3(0.0f,0.0f, -1.0f), CameraMode::Pilot);
	m_player = new Plane("./models/planeModel.tri", Cartesian3(-38500, 2000, -4000), Cartesian3(-1.0f, 0.0f, 0.0f), 90.0f, PlaneRole::Controller);

	float offset = 0.0f;
	Plane* plane1 = new Plane("./models/planeModel.tri", Cartesian3(0.0f, 4000.0f, 0.0f), Cartesian3(-1.0f, 0.0f, 0.0f), 90.0f, PlaneRole::Particle);
	Plane* plane2 = new Plane("./models/planeModel.tri", Cartesian3(-9000.0f, 4000.0f, 0.0f), Cartesian3(1.0f, 0.0f, 0.0f), -90.0f, PlaneRole::Particle);
	planes.push_back(plane1);
	planes.push_back(plane2);

	RandomDirections();

	timer.start();

	second = false;

	} // constructor


SceneModel::~SceneModel()
{
	for(int i = 0; i < particles.size(); i++)
	{
		delete particles[i];
	}

	for(int i = 0; i < planes.size(); i++)
	{
		delete planes[i];
	}

	delete m_camera;
}

void SceneModel::RandomDirections()
{
	for(int i = 0; i < RANDOM_AMOUNT; i++)
	{	
		auto newdir = RandomUnitVectorInUpwardsCone(45.0f, 0.0, 1.0f);
		random_directions.push_back(newdir);
 	}
}

// routine that updates the scene for the next frame
void SceneModel::Update()
	{ // Update()

		deltaTime = timer.restart() / 1000.0f;
		if(second)
		{
			m_camera->SetCameraMode(CameraMode::Follow);
		} else {
			m_camera->SetCameraMode(CameraMode::Pilot);
		}

		if(m_camera->GetCameraMode() == CameraMode::Pilot)
		{
			m_camera->SetPosition(m_player->position);
			m_camera->SetDirection(m_player->direction);
			m_camera->SetRotations(m_player->yaw, m_player->pitch, m_player->roll);
			m_camera->SetUp(m_player->up);
		} else {
			auto dist = m_player->position - m_camera->GetPosition();
			dist = dist.unit();

			auto dir = Cartesian3(-300, -2000, -300);
			dir = dir.unit();
			auto pos = m_player->position - Cartesian3(300, 0.0f, 300.0f);
			pos.y = pos.y + 2000.0f;
			m_camera->SetPosition(m_player->position + dir);
			m_camera->SetDirection(dir);
		}

		m_camera->Update();
		m_player->update(deltaTime, WorldMatrix, viewMatrix);

		for(int i = 0; i < particles.size(); i++)
		{
			particles[i]->life -= 1.0f;
			particles[i]->update(deltaTime, WorldMatrix, m_camera->GetViewMatrix());
		}

		// PARTICLE TO PARTICLE COLLISION
		for(int i = 0; i < particles.size(); i++)
		{
			for(int j = i + 1; j < particles.size(); j++)
			{
				if(particles[i]->isColliding(*particles[j]))
				{
					Cartesian3 pushDirection = particles[i]->position - particles[j]->position;
					pushDirection = pushDirection.unit();

					float magnitude = 30.0f;

					particles[i]->Push(pushDirection * magnitude);
					particles[j]->Push(pushDirection * -magnitude);

					particles[i]->lavaBombColour[0] = 1.0;
					particles[i]->lavaBombColour[1] = 0.0;
					particles[i]->lavaBombColour[2] = 0.0;
					particles[i]->lavaBombColour[3] = 1.0;

					particles[j]->lavaBombColour[0] = 1.0;
					particles[j]->lavaBombColour[1] = 0.0;
					particles[j]->lavaBombColour[2] = 0.0;
					particles[j]->lavaBombColour[3] = 1.0;
				}
			}
		}

		// IMAPCT WITH GROUND
		for(auto& particle: particles)
		{
			// get height wants x,y but z is up for the terrain in local space
			// impact point y value
			auto groundMatrix = WorldMatrix * columnMajorMatrix::Scale(Cartesian3(1, -1, 1));
			float groundHeight = groundModel.getHeight(particle->position.x, particle->position.z);
			Homogeneous4 vec4GroundHeight = Homogeneous4(0.0f, groundHeight, 0.0f, 1.0f);
			vec4GroundHeight = groundMatrix * vec4GroundHeight;
			Homogeneous4 end = Homogeneous4(particle->position.x, groundHeight, particle->position.z, 1.0);

			if(particle->isCollidingWithFloor(groundHeight))
			{
				groundModel.EditMesh(Cartesian3(end.x, end.y, end.z), 1.1f * 100.0f, groundMatrix);
				particle->lavaBombColour[0] = 0.2;
				particle->lavaBombColour[1] = 0.3;
				particle->lavaBombColour[2] = 0.7;
				particle->lavaBombColour[3] = 1.0;

				groundModel.ComputeUnitNormalVectors();
				//particle->life = -1.0f;
			}
		}

		for(int i = 0; i < planes.size(); i++)
		{
			planes[i]->update(deltaTime, WorldMatrix, m_camera->GetViewMatrix());
		}

		for(int i = 0; i < planes.size(); i++)
		{
			for(int j = i + 1; j < planes.size(); j++)
			{
				if(planes[i]->isColliding(*planes[j]))
				{
					// std::cout << "Plane collision!" << std::endl;
					// planes[i]->shouldRender = false;
					// planes[j]->shouldRender = false;
				}
			}
		}

		for(int i = 0; i < particles.size(); i++)
		{
			if(m_player->isCollidingWithParticle(*particles[i]))
			{
				std::cout << "Plane collided with particle" << std::endl;
			}
		}

		if(m_player->isCollidingWithFloor(groundModel.getHeight(m_player->position.x, m_player->position.z)))
		{
			std::cout << "Player plane hit floor" << std::endl;
		}
	} // Update()

// routine to tell the scene to render itself
void SceneModel::Render()
	{ // Render()
	// enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	
	// set lighting parameters
	glShadeModel(GL_FLAT);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, blackColour);
	glLightfv(GL_LIGHT0, GL_EMISSION, blackColour);
	
	// background is sky-blue
	glClearColor(0.8, 0.7, 1.0, 1.0);

	// clear the buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	columnMajorMatrix rotationMatrix;
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			rotationMatrix.coordinates[j * 4 + i] = m_camera->GetViewMatrix().coordinates[j * 4 + i];
		}
	}
	// compute the light position, need to extract the top 3x3 matrix of view matrix
  	Homogeneous4 lightDirection = rotationMatrix * WorldMatrix * sunDirection;
	// and set the w to zero to force infinite distance
 	lightDirection.w = 0.0;
 	 	
	// pass it to OpenGL
	glLightfv(GL_LIGHT0, GL_POSITION, &(lightDirection.x));

	// and set a material colour for the ground
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

	// actual render code goes here
	// flip z in local space so positive z is up  so when we rotate 90 ccw from world matrix
	// positive z points out of the screen	
	columnMajorMatrix groundMatrix;
	groundMatrix = m_camera->GetViewMatrix() * WorldMatrix * columnMajorMatrix::Scale(Cartesian3(1, 1, -1));
	groundModel.Render(groundMatrix);

	// translate, rotate, scale

	// auto axis = Cartesian3(0.0f, 1.0f, 0.0f);
	// Quaternion rotation(180.0f, axis);
	// rotation.Normalize();
	// Quaternion result = rotation * offset * rotation.Conjugate();
	// result.Normalize();
	// columnMajorMatrix rot = result.ToRotationMatrix();
//* columnMajorMatrix::RotateX(180.0f)

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, planeColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

	float scale = 1.0f;
	float distance = 2.0f;
	Cartesian3 offset = m_camera->GetPosition() + m_camera->GetDirection().unit() * distance;
	auto m = m_camera->GetViewMatrix() * columnMajorMatrix::Translate(offset) * m_player->modelMatrix *
	WorldMatrix * columnMajorMatrix::Scale(Cartesian3(scale, scale, scale));
	m_player->planeModel.Render(m);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lavaBombColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

	auto curr = std::chrono::high_resolution_clock::now();
	float timeSinceLastSpawn = std::chrono::duration<float, std::chrono::seconds::period>(curr - lastSpawnTime).count();
	if(timeSinceLastSpawn >= 3.0f)
	{	
		Particle* p = new Particle("./models/lavaBombModel.tri", random_directions[lastIndex], 2.0f, 1.0f);
		p->CreateChildren();
		particles.push_back(p);
		lastSpawnTime = curr;
		lastIndex >= random_directions.size() ? lastIndex = 0 : lastIndex++;
	}
	// maybe only add to particles vector as new Particle() when spawning based on time
	for(int i = 0; i < particles.size();)
	{
		if(particles[i]->life > 0.0f)
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, particles[i]->lavaBombColour);
			glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
			glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);
			
			particles[i]->lavaBombModel.Render(particles[i]->modelMatrix);
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lavaBombColour2);
			glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
			glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

			float scale = 1.1f; // scaled to 1.1 so we can see the collision shape
			auto temp = particles[i]->modelMatrix * columnMajorMatrix::Scale(Cartesian3(scale, scale, scale));
			particles[i]->Sphere.Render(temp);

			// Render child particles
			for(auto& child : particles[i]->children)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, child->childSmoke);
				glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
				glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);
				
				child->radius = 1.5f;
				child->modelMatrix = m_camera->GetViewMatrix() * 
				columnMajorMatrix::Translate(child->position) * 
				WorldMatrix * columnMajorMatrix::Scale(Cartesian3(child->radius, child->radius, child->radius));
				child->lavaBombModel.Render(child->modelMatrix);
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			i++;
		} else 
		{
			delete particles[i]; 
			particles.erase(particles.begin() + i);
		}
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, planeColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

	for(int i = 0; i < planes.size(); i++)
	{
		if(planes[i]->shouldRender)
		{
			planes[i]->planeModel.Render(planes[i]->modelMatrix);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lavaBombColour2);
			glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
			glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);
			planes[i]->Sphere.Render(planes[i]->sphereMatrix);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
} // Render()	
	
