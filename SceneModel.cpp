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

// Set up timers so we can incrementally spawn particles in the scene over time
static auto startTime = std::chrono::high_resolution_clock::now();
auto lastSpawnTime = std::chrono::high_resolution_clock::now() - std::chrono::seconds(5);

// constructor
SceneModel::SceneModel(float x, float y, float z)
	{ // constructor
	// this is not the best place to put this in general, but this is a quick and dirty hack
	// we start by loading three files: one for each model
	groundModel.ReadFileTerrainData(groundModelName, 500);
	planeModel.ReadFileTriangleSoup(planeModelName);
	
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
	WorldMatrix = columnMajorMatrix::RotateX(90.0f);
	// Instantiate the camera, player and plane objects.
	// Heap allocate to ensure they live until the end of the program
	// Destructor will return all heap allocated memory for these objects

	// Set up camera position can be anywhere since it will recalculate its position relative to the player plane 
	m_camera = new Camera(Cartesian3(0.0f, 0.0f, 0.0f), Cartesian3(0.0f,0.0f, -1.0f), CameraMode::Pilot);
	m_player = new Plane("./models/planeModel.tri", Cartesian3(x, y, z), 200.f, PlaneRole::Controller);

	Plane* plane1 = new Plane("./models/planeModel.tri", Cartesian3(0.0f, 4000.0f, 0.0f), 344.0f, PlaneRole::Particle);
	Plane* plane2 = new Plane("./models/planeModel.tri", Cartesian3(0.0f, 5000.0f, 0.0f), 344.0f, PlaneRole::Particle);
	planes.push_back(plane1);
	planes.push_back(plane2);

	m_switchCamera = false; // start by using pilot camera

	RandomDirections();

	// Start the timer to calculatr deltaTime 
	timer.start();


	} // constructor


// Release all heap allocated memory used 
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

		// Calculate delta time to ensure movements in the world are consistent with frame time 
		deltaTime = timer.restart() / 1000.0f;
		
		// Check if the value is set to switch between follow or pilot camera
		if(m_switchCamera)
		{
			m_camera->SetCameraMode(CameraMode::Follow);
		} else {
			m_camera->SetCameraMode(CameraMode::Pilot);
		}

		// Set the correct variables for the camera depending on which mode the
		// Camera is set to
		if(m_camera->GetCameraMode() == CameraMode::Pilot)
		{
			m_camera->SetPosition(m_player->position);
			m_camera->SetDirection(m_player->direction);
			m_camera->SetRotations(m_player->yaw, m_player->pitch, m_player->roll);
			m_camera->SetUp(m_player->up);
		} else {
			// Get some distance behind the plane and set the camera to look down from above to follow the plane


			auto pos = m_player->position - Cartesian3(300.0f, 0.0f, 300.0f);
			pos.y = pos.y + 2000.0f;

			auto dist = m_player->position - pos;
			dist = dist.unit();

			m_camera->SetPosition(pos);
			m_camera->SetDirection(dist);
		}

		// Update the camera and the player
		m_camera->Update();
		m_player->Update(deltaTime, WorldMatrix, viewMatrix);

		// Update particles data over each frame to ensure the physics calculations are correct
		for(int i = 0; i < particles.size(); i++)
		{
			particles[i]->Update(deltaTime, WorldMatrix, m_camera->GetViewMatrix());
		}

		// PARTICLE TO PARTICLE COLLISION - Check if particles collide with each other, if they do, add some push force to them and change 
		// their colour to red to indicate that the interation is heated them both up even more 
		for(int i = 0; i < particles.size(); i++)
		{
			for(int j = i + 1; j < particles.size(); j++)
			{
				if(particles[i]->isColliding(*particles[j]))
				{
					Cartesian3 pushDirection = particles[i]->GetPosition() - particles[j]->GetPosition();
					pushDirection = pushDirection.unit();
					float magnitude = 30.0f;
					// If the particles collide, make them push in opposite directions
					particles[i]->Push(pushDirection * magnitude);
					particles[j]->Push(pushDirection * -magnitude);

					// Also change their colour to red to indicate they've become hotter from colliding
					particles[i]->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
					particles[j]->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
				}
			}
		}

		// IMAPCT WITH GROUND
		// Check if the particles impact the ground, if they do, deform the mesh and recompute normals
		for(auto& particle: particles)
		{
			// get height wants x,y but z is up for the terrain in local space
			// impact point y value
			auto groundMatrix = WorldMatrix * columnMajorMatrix::Scale(Cartesian3(1, -1, 1));
			// Get the height of the terrain where the particle's position is
			float groundHeight = groundModel.getHeight(particle->GetPosition().x, particle->GetPosition().z);
			Homogeneous4 end = Homogeneous4(particle->GetPosition().x, groundHeight, particle->GetPosition().z, 1.0); // end is the hitpoint of particle

			if(particle->isCollidingWithFloor(groundHeight))
			{
				// Edit mesh will deform the mesh where the impact of the particle happens
				groundModel.EditMesh(Cartesian3(end.x, end.y, end.z), 1.1f * 100.0f, groundMatrix);
				particle->SetColor(0.2f, 0.3f, 0.7f, 1.0f); // change colour when hitting the floor (this is mostly unnoticeable but when visible looks good)
				groundModel.ComputeUnitNormalVectors(); //re-compute normals since ground mesh has changed to ensure lighting looks correct
			}
		}

		for(int i = 0; i < planes.size(); i++)
		{
			planes[i]->Update(deltaTime, WorldMatrix, m_camera->GetViewMatrix());
		}

		// Check if the planes flying in the air are colliding with one another 
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

		// Check if the plane collides with a flying particle lava bomb 
		// if so exit the game since the plane will be destroyed
		for(int i = 0; i < particles.size(); i++)
		{
			if(m_player->isCollidingWithParticle(*particles[i]))
			{
				std::cout << "You crashed the plane into a lava bomb, which destroyed it." << std::endl;
				exit(0);
			}
		}

		// Check the players collision with the floor. If they collide, exit the game 
		if(m_player->isCollidingWithFloor(groundModel.getHeight(m_player->position.x, m_player->position.z)))
		{
			std::cout << "You hit the floor and crashed the plane." << std::endl;
			exit(0);
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
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, planeColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

	// If the third person camera is enabled, render some distance behind the plane
	// otherwise the plane remains in First person mode
	columnMajorMatrix playerMatrix;

	// if(m_camera->isThirdPerson())
	// {
	// 	auto direction = (m_camera->GetPosition() - m_player->position).unit();

	// 	m_camera->SetDirection(direction);
	// 	auto newposition = m_player->position  - direction * 3.0f;
	// 	m_camera->SetPosition(newposition);
	// }
	// i think im setting distance from top down camera too not just main camera since there is only one
	// when it changes to top down, it sets distance for the plane from there
	float scale = 1.0f;
	float distance = 3.0f;
	Cartesian3 offset = m_camera->GetPosition() + m_camera->GetDirection().unit() * distance;
	playerMatrix = m_camera->GetViewMatrix() * columnMajorMatrix::Translate(offset) * m_player->modelMatrix *
	WorldMatrix * columnMajorMatrix::Scale(Cartesian3(scale, scale, scale));
	m_player->planeModel.Render(playerMatrix);

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

	// Loop through all particles and render them
	for(int i = 0; i < particles.size();)
	{
		// If the particle has life, it should be rendered 
		if(particles[i]->GetShouldRender() == true)
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, particles[i]->GetColor());
			glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
			glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);
			
			auto transformationMatrix = particles[i]->GetModelMatrix();
			particles[i]->lavaBombModel.Render(transformationMatrix);

			// Render child particles
			for(auto& child : particles[i]->GetChildren())
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, child->GetChildColor());
				glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
				glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);
				
				child->SetScale(0.5f);
				auto matrix = m_camera->GetViewMatrix() * 
				columnMajorMatrix::Translate(child->GetPosition()) * 
				WorldMatrix * columnMajorMatrix::Scale(Cartesian3(child->GetScale(), child->GetScale(), child->GetScale()));
				child->lavaBombModel.Render(matrix);
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
		if(planes[i]->shouldRender == true)
		{
			planes[i]->planeModel.Render(planes[i]->modelMatrix);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lavaBombColour2);
			glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
			glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);
			planes[i]->Sphere.Render(planes[i]->sphereMatrix); // render the collision sphere for the plane

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
} // Render()	

// Switches between follow camera and pilot camera
void SceneModel::SwitchCamera()
{
	m_switchCamera = m_switchCamera == false ? true : false;
}
	
