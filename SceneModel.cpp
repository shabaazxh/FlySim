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
#include <cstdlib>
#include <ctime>

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
const GLfloat planeColour[4] = {0.1, 0.1, 0.5, 1.0};
const GLfloat planeRadius = 2.0;
const GLfloat lavaBombRadius = 100.0;
const Cartesian3 chaseCamVector(0.0, -2.0, 0.5);

// Set up timer so we can incrementally spawn particles in the scene over time
auto lastSpawnTime = std::chrono::high_resolution_clock::now() - std::chrono::seconds(3);

// constructor
SceneModel::SceneModel(float x, float y, float z)
	{ // constructor
	// this is not the best place to put this in general, but this is a quick and dirty hack
	// we start by loading three files: one for each model
	groundModel.ReadFileTerrainData(groundModelName, 500);	
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
	m_player = new Plane("./models/planeModel.tri", Cartesian3(x, y, z), 200.f, false, PlaneRole::Controller);

	Plane* plane1 = new Plane("./models/planeModel.tri", Cartesian3(0.0f, 4000.0f, 0.0f), 200.0f, true, PlaneRole::AI);
	Plane* plane2 = new Plane("./models/planeModel.tri", Cartesian3(0.0f, 4000.0f, 0.0f), 200.0f, false, PlaneRole::AI);
	planes.push_back(plane1);
	planes.push_back(plane2);

	m_switchCamera = false; // start by using pilot camera, set follow camera to false

	// Seed for random number generation 
	// Will use this to change planes to random colour when they collide
	srand(static_cast<unsigned int>(time(0)));

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
		particles[i] = nullptr;
	}

	for(int i = 0; i < planes.size(); i++)
	{
		delete planes[i];
		planes[i] = nullptr;
	}

	delete m_camera;
	m_camera = nullptr;
}

// Called in the constructor of the SceneModel to set up random directions
void SceneModel::RandomDirections()
{	
	// Create randomised directions for lava bombs 
	// Store directions into vector 
	for(int i = 0; i < RANDOM_AMOUNT; i++)
	{	
		auto direction = RandomUnitVectorInUpwardsCone(45.0f, 0.0, 1.0f);
		random_directions.push_back(direction);
 	}
}

// routine that updates the scene for the next frame
void SceneModel::Update()
	{ // Update()

		// Calculate delta time to ensure movements in the world are consistent with frame time 
		// https://doc.qt.io/qt-6/qelapsedtimer.html
		deltaTime = timer.restart() / 1000.0f;
		m_player->Forward(); // move the player forward each frame

		// Check if the value is set to switch between follow or pilot camera
		if(m_switchCamera)
		{
			m_camera->SetCameraMode(CameraMode::Follow);
		} else {
			m_camera->SetCameraMode(CameraMode::Pilot);
		}

		// Set the correct variables for the camera depending on which mode the
		if(m_camera->GetCameraMode() == CameraMode::Pilot)
		{
			// Since camera is in pilot mode, have camera mimic the plane movement
			// Set same position, direction and pass yaw, pitch and roll to have camera behave the same
			m_camera->SetPosition(m_player->GetPostion());
			m_camera->SetDirection(m_player->GetDirection());
			m_camera->SetRotations(m_player->GetYaw(), m_player->GetPitch(), m_player->GetRoll());
			m_camera->SetUp(m_player->GetUp());
		} else { // If the camera is in follow mode, place the camera above the plane
			// Get some distance behind the plane and set the camera to look down from above to follow the plane
			auto position = m_player->GetPostion() - Cartesian3(1.0f, -1.0f, 1.0f);
			
			// Use new position to calculate distance to player 
			auto dist = m_player->GetPostion() - position;
			dist = dist.unit(); // normalize 

			// Set camera to the new position and direction
			m_camera->SetPosition(position);
			m_camera->SetDirection(dist);
		}

		// Update the camera and the player
		m_camera->Update();
		m_player->Update(deltaTime, WorldMatrix, m_camera->GetViewMatrix());

		// Update particles data over each frame to ensure calculations are correct
		for(int i = 0; i < particles.size(); i++)
		{
			particles[i]->Update(deltaTime, WorldMatrix, m_camera->GetViewMatrix());
			// Update child particles for the particle to ensure they have the data required to render
			if(particles[i]->GetChildren().size() > 0) // safety check incase we don't want child smoke particles, we dont want to try and update nullptr's
			{
				for(auto& child : particles[i]->GetChildren())
				{
					child->Update(deltaTime, WorldMatrix, m_camera->GetViewMatrix());
				}
			}
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
				particle->SetShouldRender(false); // if the particle hit the floor, it expires
			}
		}

		// Call update for the plane objects to give them latest
		// deltatime view and world matrices
		for(int i = 0; i < planes.size(); i++)
		{
			planes[i]->Update(deltaTime, WorldMatrix, m_camera->GetViewMatrix());
		}

		// Check if the planes flying in the air are colliding with one another
		// If so, give the colliding planes a random generated color r,g,b 
		for(int i = 0; i < planes.size(); i++)
		{
			for(int j = i + 1; j < planes.size(); j++)
			{
				if(planes[i]->isCollidingWithAnotherPlane(*planes[j]))
				{
					// When planes collide, they change colour.
					// I decided to do this instead of destroying them when they crashed since
					// that would require a restart of the game if you missed it, this way the collision can 
					// continiously be observed 
					// Assign a random color to the first plane
					float randRed = static_cast<float>(rand()) / RAND_MAX; 
					float randGreen = static_cast<float>(rand()) / RAND_MAX; 
					float randBlue = static_cast<float>(rand()) / RAND_MAX; 
					planes[i]->SetColor(randRed, randGreen, randBlue, 1.0f);

					// Assign a different random color to the second plane
					float randRed2 = static_cast<float>(rand()) / RAND_MAX; 
					float randGreen2 = static_cast<float>(rand()) / RAND_MAX; 
					float randBlue2 = static_cast<float>(rand()) / RAND_MAX; 
					planes[j]->SetColor(randRed2, randGreen2, randBlue2, 1.0f);
				}
			}
		}

		// Check if the player plane collided with another plane in the scene
		for(auto& plane : planes)
		{
			if(m_player->isCollidingWithAnotherPlane(*plane))
			{
				std::cout << "You crashed into another plane. " << std::endl;
				exit(0); // exit the program if player plane hits another plane
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
		if(m_player->isCollidingWithFloor(groundModel.getHeight(m_player->GetPostion().x, m_player->GetPostion().z)))
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

	// Render the player
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, planeColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);
	columnMajorMatrix playerMatrix;

	// Set scale of the player and use it's model matrix, consisting of it's transformations for 
	// rendering
	m_player->SetScale(1.0f);
	m_player->planeModel.Render(m_player->modelMatrix);
	
	// Render lava bombs
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lavaBombColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

	// Set a timer to count 3 seconds and then spawn a new lava bomb
	auto curr = std::chrono::high_resolution_clock::now();
	float timeSinceLastSpawn = std::chrono::duration<float, std::chrono::seconds::period>(curr - lastSpawnTime).count();
	if(timeSinceLastSpawn >= 3.0f)
	{	
		Particle* p = new Particle("./models/lavaBombModel.tri", random_directions[lastIndex], 2.0f, 1.0f);
		p->CreateChildren(); // create children creates a smoke like particle effect
		particles.push_back(p);
		lastSpawnTime = curr; // set the spawn time
		lastIndex >= random_directions.size() ? lastIndex = 0 : lastIndex++; // prepare a new position for next lava bomb
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
			
			particles[i]->lavaBombModel.Render(particles[i]->modelMatrix);

			// Render child particles
			for(auto& child : particles[i]->GetChildren())
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, child->GetChildColor());
				glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
				glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);
				
				child->SetScale(0.5f);
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

	// Render AI like planes in the sky 
	for(int i = 0; i < planes.size(); i++)
	{
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, planes[i]->GetColor());
		glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
		glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);
		planes[i]->planeModel.Render(planes[i]->modelMatrix);
	}
} // Render()	

// Switches between follow camera and pilot camera
void SceneModel::SwitchCamera()
{
	m_switchCamera = m_switchCamera == false ? true : false;
}
	
