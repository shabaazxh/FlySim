#include "Particle.h"

Particle::Particle(const char *fileName, const Cartesian3& direction, float speed, float s)
{
        // Load lava bomb model and init default particle values
        lavaBombModel.ReadFileTriangleSoup(fileName);
        m_direction = direction;
        m_position = Cartesian3(-38500.0f, 1000.0f, -4000); // default position
        m_velocity.x = direction.x;
        m_velocity.y = direction.y;
        m_velocity.z = direction.z;
        m_scale = s;
        m_gravity = -19.81f;
        m_mass = 1.0f;
        m_shouldRender = true;
        std::srand(std::time(nullptr)); // set up random seed
}

// Destructor to clean up heap allocated resources within the class
Particle::~Particle()
{
    // Free heap allocated memory for the child particles when main particle is destroyed
    for(auto& child : children)
    {
        if(child != nullptr)
        {
            delete child;
            child = nullptr;
        }
    }
}

// Create child objects of main particle to create smoke effect
void Particle::CreateChildren()
{
    // Create 5 child particles for the main particle and push into childrens vector array
    // we use the same direction as main particle 
    for(int i = 0; i < 5; i++)
    {   
        Particle* p = new Particle("./models/lavaBombModel.tri", m_direction, 2.0f, 1.0f);
        children.push_back(p);
    }
}

// Update particle data each frame to ensure the particle physics and movement are correct 
void Particle::Update(float dt, const columnMajorMatrix& worldMatrix, const columnMajorMatrix& viewMatrix)
{
    // Update position (s = ut + 1/2at^2)
    // Gibbs, K. (2016). schoolphysics ::Welcome:: [online] www.schoolphysics.co.uk. Available at: https://www.schoolphysics.co.uk/age14-16/Mechanics/Motion/text/Equations_of_motion/index.html.
    m_position.x += m_velocity.x * dt;
    m_position.y += m_velocity.y * dt + 0.5 * m_gravity * dt * dt;
    m_position.z += m_velocity.z * dt;

    // Update velocity (v = u + at) // only affect y axis
    // mathcentre (2009). Substitution & Formulae. [online] Available at: http://www.mathcentre.ac.uk/resources/workbooks/mathcentre/mc-TY-subsandformulae-2009-1.pdf.
    m_velocity.x = m_velocity.x;
    m_velocity.y = m_velocity.y + m_gravity * dt;
    m_velocity.z = m_velocity.z;

    // Update the children which create a "smoke" like trail
    for(int i = 0; i < children.size(); i++)
    {
        // Calculate the position of the child particle relative to the main particle
        Cartesian3 childPosition = m_position - m_direction * (i + 1) * 1.0f; // 0.1f is the distance between particles

        // introduce some randomness to the angle for child particles
        float randomAngle = static_cast<float>(std::rand() % 360); // get a random angle in degrees
        float randomAngleRad = randomAngle * M_PI / 180.0f; // convert random angle to radians

        // Add the random angle to give some randomness to the particles
        float angle = atan2(m_direction.z, m_direction.x) - M_PI / 2.0f + randomAngleRad;

        float r = 20.0f * (i + 1); // define some radius to give a swirl with i'th particle some distance away

        // Apply it to the position to create a kind of smoke effect behind the main particle
        childPosition.x += r * cos(angle);
        childPosition.z += r * sin(angle);

        // Update the position and velocity of the current child particle
        children[i]->SetPosition(childPosition);
        children[i]->SetVelocity(m_velocity);
    }
    // construct the model matrix using the matrices 
    modelMatrix = viewMatrix * columnMajorMatrix::Translate(m_position) * worldMatrix * columnMajorMatrix::Scale(Cartesian3(m_scale, m_scale, m_scale));
}

// Push a particle in the air. Used for collision between particles
void Particle::Push(const Cartesian3& pushAmount)
{
    // Flexbooks (2022). CK12-Foundation. [online] flexbooks.ck12.org. Available at: https://flexbooks.ck12.org/cbook/ck-12-middle-school-physical-science-flexbook-2.0/section/11.4/primary/lesson/calculating-acceleration-from-force-and-mass-ms-ps/.
    Cartesian3 acceleration = pushAmount / m_mass;
    m_velocity = m_velocity + acceleration;
}

// Check if the current particle is colliding with another using sphere collision 
bool Particle::isColliding(const Particle& other)
{
    float d = (m_position - other.GetPosition()).length();
    return d < (m_collisionSphereRadius + other.GetCollisionSphereRadius());
}

// Check if the particle is colliding with the floor
bool Particle::isCollidingWithFloor(float height)
{   
    return m_position.y - m_collisionSphereRadius <= height;
}

// Set the colour of the particle
void Particle::SetColor(float r, float g, float b, float a)
{
    lavaBombColour[0] = r;
    lavaBombColour[1] = g;
    lavaBombColour[2] = b;
    lavaBombColour[3] = a;
}

// Set the scale of the particle
void Particle::SetScale(float scale)
{
    m_scale = scale;
}

// Change bool value indicating whether this particle should be rendered
void Particle::SetShouldRender(bool value) 
{ 
    m_shouldRender = value; 
}

// Set the position of the particle
void Particle::SetPosition(Cartesian3 position)
{
    m_position = position;
}
// Ser the velocity of the particle
void Particle::SetVelocity(Cartesian3 velocity)
{
    m_velocity = velocity;
}



