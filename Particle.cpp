Particle::Particle(const char *fileName, const Cartesian3& direction, float speed, float r)
{
        lavaBombModel.ReadFileTriangleSoup(fileName);
        Sphere.ReadFileTriangleSoup(fileName);

        this->direction = direction;
        position = Cartesian3(-38500.0f, 1000.0f, -4000); // default position
        velocity.x = direction.x;
        velocity.y = direction.y;
        velocity.z = direction.z;
        life = 10000.0f; // life of the particle
        radius = r;
        gravity = -19.81f;
        mass = 1.0f;
        shouldRender = true;
        std::srand(std::time(nullptr));
}