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
	WorldMatrix = columnMajorMatrix::Identity();
	WorldMatrix = columnMajorMatrix::RotateX(90.0f);
	//m_camera->GetViewMatrix() = Matrix4::Identity();

	cameraPos = Cartesian3(-38500, 1000.0f, -3800);
	cameraTarget = Cartesian3(0.0f,0.0f, -1.0f);
	cameraUp = Cartesian3(0.0f,1.0f,0.0f);
	planepos = Cartesian3(0,0,0);


	cameraPos = Cartesian3(-271.4, 3634, -2855);
	cameraTarget = cameraTarget.unit();	

	m_camera = new Camera(Cartesian3(-271.4, 3634, -2855), Cartesian3(0.0f,0.0f, -1.0f));

	//m_camera->GetViewMatrix() = columnMajorMatrix::constructView(cameraPos, cameraPos + cameraTarget, cameraUp);
	objectModelMatrix = columnMajorMatrix::Identity();

	pitch = 0.0f;
	yaw = -90.0f;

	float offset = 0.0f;
	Plane* plane1 = new Plane("./models/planeModel.tri", Cartesian3(0.0f, 4000.0f, 0.0f), Cartesian3(-1.0f, 0.0f, 0.0f), 90.0f);
	Plane* plane2 = new Plane("./models/planeModel.tri", Cartesian3(-9000.0f, 4000.0f, 0.0f), Cartesian3(1.0f, 0.0f, 0.0f), -90.0f);
	planes.push_back(plane1);
	planes.push_back(plane2);

	RandomDirections();

	timer.start();

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
	srand(time(0));

	for(int i = 0; i < RANDOM_AMOUNT; i++)
	{	
		// random angle within 45 degrees
		float angle = (rand()/ (float)RAND_MAX) * (M_PI / 4.0f);

		Cartesian3 point = Cartesian3(2.0*rand()/RAND_MAX - 1.0, 2.0*rand()/RAND_MAX - 1.0, 2.0*rand()/RAND_MAX - 1.0);
		point = point.unit();
		Cartesian3 dir = Cartesian3(point.x, sin(angle), sin(angle));
		float speed = 60.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(300.0f-60.0f)));
		dir = {dir.x * speed, dir.y * speed, dir.z * speed};

		auto h = (speed * speed) * (sin(angle) * sin(angle)) / (2 * -9.81f);
		 
		//std::cout << "Altitude: " << h << ", Speed: " << speed << ", Angle: " << angle << std::endl;

		auto newdir = RandomUnitVectorInUpwardsCone(45.0f, 0.0, 1.0f);
		//newdir = {newdir.x * speed, newdir.y * speed, newdir.z * speed};
		random_directions.push_back(newdir);
 	}
}

void SceneModel::calculateDirection()
{
	// Cartesian3 up = Cartesian3(0, 1.0f, 0);
	// Cartesian3 x = Cartesian3(1.0f, 0.0f, 0.0f);
	// x.Rotate(yaw, up);
	// x = x.unit();

	// Cartesian3 z = up.cross(x);
	// z = z.unit();
	// x.Rotate(pitch, z);

	// cameraTarget = x;
	// cameraTarget = cameraTarget.unit();

	// cameraUp = cameraTarget.cross(z);
	// cameraUp = cameraUp.unit();

	// new 
	// auto up = Cartesian3(0, 1, 0);
	// auto forward = Cartesian3(1,0,0);
	// forward.Rotate(yaw, up);
	// forward = forward.unit();

	// Cartesian3 r = up;
	// r = r.cross(forward);
	// r = r.unit();
	// forward.Rotate(pitch, r);
	
	// Cartesian3 u = forward.cross(r);
	// u = u.unit();

	// cameraTarget = forward;
	// cameraUp = u;

	// if(pitch >= 89.0f) pitch = 89.0f;
	// if(pitch <= -89.0f) pitch = -89.0f;

	// Cartesian3 direction;
	// direction.x = std::cos(toRadians(yaw)) * std::cos(toRadians(pitch));
	// direction.y = std::sin(toRadians(pitch));
	// direction.z = std::sin(toRadians(yaw)) * std::cos(toRadians(pitch));
	// cameraTarget = direction.unit();

	// auto right = Cartesian3(0.0f, 1.0f, 0.0f).cross(cameraTarget);
	// right = right.unit();

	// cameraUp = cameraTarget.cross(right);
	// cameraUp = cameraUp.unit();

	// m_camera->GetViewMatrix() = columnMajorMatrix::constructView(cameraPos, (cameraPos + cameraTarget), cameraUp);
}

// routine that updates the scene for the next frame
void SceneModel::Update()
	{ // Update()

		deltaTime = timer.restart() / 1000.0f;

		//calculateDirection();
		m_camera->Update();
		//m_camera->GetViewMatrix() = columnMajorMatrix::constructView(cameraPos, (cameraPos + cameraTarget), cameraUp);
		planepos = Cartesian3(2.0f, 100.0f, 3.0f);
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
			//cameraPos = planes[i]->position;
		}

		for(int i = 0; i < planes.size(); i++)
		{
			for(int j = i + 1; j < planes.size(); j++)
			{
				if(planes[i]->isColliding(*planes[j]))
				{
					std::cout << "Plane collision!" << std::endl;
					// planes[i]->shouldRender = false;
					// planes[j]->shouldRender = false;
				}
			}
		}

		//std::cout << "Camera pos: " << cameraPos << std::endl;

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
	auto groundMatrix = m_camera->GetViewMatrix() * WorldMatrix * columnMajorMatrix::Scale(Cartesian3(1, 1, -1));
	groundModel.Render(groundMatrix);

	// translate, rotate, scale
	auto forward = ((cameraTarget - cameraPos).unit()) * 14.0f;
	float scale = 1.0f;

	//std::cout << offset << std::endl;

	// auto axis = Cartesian3(0.0f, 1.0f, 0.0f);
	// Quaternion rotation(180.0f, axis);
	// rotation.Normalize();
	// Quaternion result = rotation * offset * rotation.Conjugate();
	// result.Normalize();
	// columnMajorMatrix rot = result.ToRotationMatrix();
//* columnMajorMatrix::RotateX(180.0f)

//look matrix for plane?
	
	Cartesian3 direction;
	direction.x = std::cos(DEG2RAD(yaw)) * std::cos(DEG2RAD(pitch));
	direction.y = std::sin(DEG2RAD(pitch));
	direction.z = std::sin(DEG2RAD(yaw)) * std::cos(DEG2RAD(pitch));
	auto planeTarget = direction.unit();

	auto right = Cartesian3(0.0f, 1.0f, 0.0f).cross(cameraTarget);
	right = right.unit();

	auto planeUp = cameraTarget.cross(right);
	planeUp = cameraUp.unit();

	auto rot = columnMajorMatrix::Look(cameraPos, (cameraPos + planeTarget), planeUp);

	objectModelMatrix = m_camera->GetViewMatrix() * columnMajorMatrix::Translate(cameraPos) *
	rot *  WorldMatrix * 
	columnMajorMatrix::Scale(Cartesian3(scale, scale, scale));

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, planeColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);
	planeModel.Render(objectModelMatrix);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lavaBombColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

	auto curr = std::chrono::high_resolution_clock::now();
	float timeSinceLastSpawn = std::chrono::duration<float, std::chrono::seconds::period>(curr - lastSpawnTime).count();
	if(timeSinceLastSpawn >= 3.0f)
	{	
		particles.push_back(new Particle("./models/lavaBombModel.tri", random_directions[lastIndex], 2.0f, 1.0f));
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

			auto mat = m_camera->GetViewMatrix() * (particles[i]->modelMatrix * WorldMatrix);	
			particles[i]->lavaBombModel.Render(particles[i]->modelMatrix);
			

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lavaBombColour2);
			glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
			glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

			float scale = 1.1f; // scaled to 1.1 so we can see the collision shape
			auto temp = particles[i]->modelMatrix * columnMajorMatrix::Scale(Cartesian3(scale, scale, scale));
			particles[i]->Sphere.Render(temp);

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
	
