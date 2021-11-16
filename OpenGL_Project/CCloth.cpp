#include "CCloth.h"
#include "CObjectManager.h"


glm::vec3 CCloth::g_gravity = glm::vec3(0.0f, -9.81f, 0.0f);
glm::vec3 CCloth::g_wind = glm::vec3(0.0f, 0.0f, 0.0f) ;

/// <summary>
/// Particle Constructor
/// </summary>
/// <param name="_position"></param>
CParticle::CParticle(glm::vec3 _position) {
    m_position = _position;
    m_prevPosition = _position;
}

/// <summary>
/// Particle constructor
/// </summary>
/// <param name="_x"></param>
/// <param name="_y"></param>
/// <param name="_z"></param>
CParticle::CParticle(float _x, float _y, float _z) : CParticle(glm::vec3(_x, _y, _z)) {}

/// <summary>
/// Particle deconstructor
/// </summary>
CParticle::~CParticle()
{
}

/// <summary>
/// The main update function of the particles, manages the forces being applied to the particles,a s well as the verlet integration
/// </summary>
/// <param name="_deltaTime"></param>
void CParticle::Update(float _deltaTime)
{
    if (m_isFixed) return;

    //add gravity force
    AddForce(m_parentCloth->GetGravity() * m_mass);
    //add wind force, and multiply by random float between 0.75 and 1.5
    AddForce(m_parentCloth->GetWind() * (0.75f + (rand() % 50) / 100.0f));

    //add damping force
    AddForce(-m_velocity * m_damping);

    //Update position using Verlet integration method, using acceleration and damping 
    glm::vec3 temp = m_position;
    m_position = (1.0f + m_damping) * m_position - (m_damping * m_prevPosition) + m_acceleration * (_deltaTime * _deltaTime);
    m_prevPosition = temp;

    //calculate the velocity of the particle based on the difference between the current and previous position 
    m_velocity = (m_position - m_prevPosition);

    //clear the acceleration
    m_acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
}

void CParticle::CheckCollisions() {
    //calcualte the world position of the particle based on the parent cloth object position
    glm::vec3 worldPos = m_parentCloth->GetPosition() + m_position;
    glm::vec3 nextPos = worldPos + m_velocity;
    glm::vec3 floorPos = m_parentCloth->GetFloorShape()->GetPosition();
    float thickness = 0.01f;

    //check if the world position is below the ground plane and if so, set the position to the ground plane position and set the acceleration to zero
    if (worldPos.y < floorPos.y + thickness)
    {
        //Normal Force
        AddForce(glm::vec3(0, -m_acceleration.y * m_mass, 0));

        //But also set the position to avoid clipping
        m_position.y = (floorPos.y + thickness - m_parentCloth->GetPosition()).y;
        m_prevPosition = m_position;
    }

    //get sphere
    CShape* sphere = CObjectManager::GetShape("sphere1");
    float padding = 0.3f;

    //check if the particle is within the sphere radius
    if (glm::distance(worldPos, sphere->GetPosition()) < (sphere->GetScale().x / 2 + padding))
    {
        //calcualte the normal
        glm::vec3 normal = glm::normalize(worldPos - sphere->GetPosition());

        //add the normal force 
        m_acceleration = glm::vec3(0, 0, 0);
        AddForce(normal * m_mass);

        //set the position to the edge of the sphere using normal
        glm::vec3 newWorldPos = sphere->GetPosition() + normal * (sphere->GetScale().x / 2 + padding);
        m_position = newWorldPos - m_parentCloth->GetPosition();
        m_prevPosition = m_position;
    }
}

/// <summary>
/// Calculates the normal of the partcile, based on the connected triangles (Smooth Normals)
/// </summary>
/// <param name="_x"></param>
/// <param name="_y"></param>
void CParticle::CalculateNormal(int _x, int _y)
{
    if (!m_parentCloth){
        m_normal = glm::vec3(0.0f, 0.0f, 1.0f);
        return;
    }

    //Getting all connected particles to the current particle
    CParticle* middle =         m_parentCloth->GetParticle(_x, _y);
    CParticle* topLeft =        m_parentCloth->GetParticle(_x - 1, _y - 1);
    CParticle* top =            m_parentCloth->GetParticle(_x, _y - 1);
    CParticle* topRight =       m_parentCloth->GetParticle(_x + 1, _y - 1);
    CParticle* right =          m_parentCloth->GetParticle(_x + 1, _y);
    CParticle* bottomRight =    m_parentCloth->GetParticle(_x + 1, _y + 1);
    CParticle* bottom =         m_parentCloth->GetParticle(_x, _y + 1);
    CParticle* bottomLeft =     m_parentCloth->GetParticle(_x - 1, _y + 1);
    CParticle* left =           m_parentCloth->GetParticle(_x - 1, _y);

    //If midle is not valid, exit
    if (!middle)
    {
        m_normal = glm::vec3(0.0f, 0.0f, 1.0f);
        return;
    }

    //calculate the normal of each triangle connected to the current particle
    glm::vec3 normal1 = (topLeft && top) ? glm::normalize(glm::cross(middle->m_position - topLeft->m_position, middle->m_position - top->m_position)) : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 normal2 = (top && topRight) ? glm::normalize(glm::cross(middle->m_position - top->m_position, middle->m_position - topRight->m_position)) : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 normal3 = (topRight && right) ? glm::normalize(glm::cross(middle->m_position - topRight->m_position, middle->m_position - right->m_position)) : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 normal4 = (right && bottomRight) ? glm::normalize(glm::cross(middle->m_position - right->m_position, middle->m_position - bottomRight->m_position)) : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 normal5 = (bottomRight && bottom) ? glm::normalize(glm::cross(middle->m_position - bottomRight->m_position, middle->m_position - bottom->m_position)) : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 normal6 = (bottom && bottomLeft) ? glm::normalize(glm::cross(middle->m_position - bottom->m_position, middle->m_position - bottomLeft->m_position)) : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 normal7 = (bottomLeft && left) ? glm::normalize(glm::cross(middle->m_position - bottomLeft->m_position, middle->m_position - left->m_position)) : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 normal8 = (left && topLeft) ? glm::normalize(glm::cross(middle->m_position - left->m_position, middle->m_position - topLeft->m_position)) : glm::vec3(0.0f, 0.0f, 0.0f);

    //add all the normals together
    m_normal = normal1 + normal2 + normal3 + normal4 + normal5 + normal6 + normal7 + normal8;

    //normalize the normal
    m_normal = glm::normalize(-m_normal);
}

bool CParticle::GetIsBroken()
{
    bool isBroken = false;
    //loop through all constraints
    for (int i = 0; i < m_constraints.size(); i++)
    {
        //if the constraint is broken, set the bool to true
        if (m_constraints[i]->GetIsBroken())
        {
            isBroken = true;
            break;
        }
    }

    return isBroken;
}

/// <summary>
/// Constraint constructor
/// </summary>
/// <param name="_pParticle1"></param>
/// <param name="_pParticle2"></param>
CConstraint::CConstraint(CParticle* _pParticle1, CParticle* _pParticle2){
    m_pParticle1 = _pParticle1;
    m_pParticle2 = _pParticle2;

    m_restLength = glm::distance(m_pParticle1->GetPosition(), m_pParticle2->GetPosition());

    //add the constraint to the particle
    m_pParticle1->AddConstraint(this);
}

/// <summary>
/// Constraint Deconstructor
/// </summary>
CConstraint::~CConstraint()
{
}

/// <summary>
/// Update function for constraints, applies movement to the connected particles
/// </summary>
/// <param name="_deltaTime"></param>
void CConstraint::Update(float _deltaTime)
{
    if (m_isBroken) return;

    //if distance between particles is greater than the rest length, break
    if (glm::distance(m_pParticle1->GetPosition(), m_pParticle2->GetPosition()) > m_restLength * 2)
    {
        this->Break();
        return;
    }

    glm::vec3 delta = m_pParticle1->GetPosition() - m_pParticle2->GetPosition();
    float deltaLength = glm::length(delta);
    float difference = (m_restLength - deltaLength) / deltaLength;
    float m1 = 1.0f / m_pParticle1->GetMass();
    float m2 = 1.0f / m_pParticle2->GetMass();
    if (!m_pParticle1->GetIsFixed()) m_pParticle1->SetPosition(m_pParticle1->GetPosition() + delta * (m1 / (m1 + m2)) * m_stiffness * difference);
    if (!m_pParticle2->GetIsFixed()) m_pParticle2->SetPosition(m_pParticle2->GetPosition() - delta * (m2 / (m2 + m1)) * m_stiffness * difference);
}

void CConstraint::Break(){
    if (m_isBroken) return;

    m_isBroken = true;
    //loop through first particle's constraints and break them all
    for (int i = 0; i < m_pParticle1->GetConstraints().size(); i++){
        m_pParticle1->GetConstraints()[i]->Break();
    }
}

/// <summary>
/// Basic cloth constructor, takes in width and height divisions, and builds a cloth mesh.
/// </summary>
/// <param name="_shape"></param>
/// <param name="_width"></param>
/// <param name="_height"></param>
CCloth::CCloth(CShape* _shape, int _width, int _height)
{
    m_shape = _shape;
    clothWidthDivisions = _width;
    clothHeightDivisions = _height;
    
    Rebuild();
}

/// <summary>
/// Rebinds all verticies and indicies to the Vertex Buffer and Element Buffer
/// </summary>
void CCloth::Rebind()
{
    //Get mesh being used
    CMesh* mesh = m_shape->GetMesh();

    //create a vector of floats to be used as the vertex data, then fill it with the verticies create above
    std::vector<float> vertexData = {};
    std::vector< int > indices = {};

    //Calculate Smooth Normals
    if (smoothNormals) {
        for (int y = 0; y < clothHeightDivisions; y++)
        {
            for (int x = 0; x < clothWidthDivisions; x++)
            {
                m_particles[y][x]->CalculateNormal(x, y);
            }
        }
    }
    
    int index = 0;
    //Loop through each particle
	for (int y = 0; y < clothHeightDivisions - 1; y++) {
		for (int x = 0; x < clothWidthDivisions - 1; x++) {
            //Top Left Half of quad
            CParticle* p1 = m_particles[y][x];
            CParticle* p2 = m_particles[y + 1][x];
            CParticle* p3 = m_particles[y][x + 1];

            //check if the particle constraints are broken
            if (p1->GetIsBroken() || p2->GetIsBroken() || p3->GetIsBroken()){
                //Add data to buffer
                //AddVertexData(vertexData, p1);
                //AddVertexData(vertexData, p2);
                //AddVertexData(vertexData, p3);

                //indices.push_back(0);
                //indices.push_back(0);
                //indices.push_back(0);

            }
            else{
                //calculate the normal of the triangle formed by the three verticies
                if (!smoothNormals) {
                    glm::vec3 normal1 = glm::normalize(glm::cross(p2->GetPosition() - p1->GetPosition(), p3->GetPosition() - p1->GetPosition()));

                    p1->SetNormal(normal1);
                    p2->SetNormal(normal1);
                    p3->SetNormal(normal1);
                }

                //Add data to buffer
                AddVertexData(vertexData, p1);
                AddVertexData(vertexData, p2);
                AddVertexData(vertexData, p3);

                indices.push_back(index++);
                indices.push_back(index++);
                indices.push_back(index++);
            }

            //Bottom Right Half of quad
            CParticle* p4 = m_particles[y][x + 1];
            CParticle* p5 = m_particles[y + 1][x];
            CParticle* p6 = m_particles[y + 1][x + 1];

            //check if the particle constraints are broken
            if (p4->GetIsBroken() || p5->GetIsBroken() || p6->GetIsBroken()){
                //Add data to buffer
                //AddVertexData(vertexData, p4);
                //AddVertexData(vertexData, p5);
                //AddVertexData(vertexData, p6);

                //indices.push_back(0);
                //indices.push_back(0);
                //indices.push_back(0);

            }
            else{
                //calculate the normal of the triangle formed by the three verticies
                if (!smoothNormals) {
                    glm::vec3 normal2 = glm::normalize(glm::cross(p5->GetPosition() - p4->GetPosition(), p6->GetPosition() - p4->GetPosition()));

                    p4->SetNormal(normal2);
                    p5->SetNormal(normal2);
                    p6->SetNormal(normal2);
                }

                AddVertexData(vertexData, p4);
                AddVertexData(vertexData, p5);
                AddVertexData(vertexData, p6);

                indices.push_back(index++);
                indices.push_back(index++);
                indices.push_back(index++);
            }


		}
	}

    mesh->SetIndices(indices);

    //Update EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());
    int* ind = &indices[0];
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), nullptr, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), ind, GL_DYNAMIC_DRAW);

    //Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVBO());
    float* verts = &vertexData[0];
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), verts, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/// <summary>
/// Add the current particle to the vertex data
/// </summary>
/// <param name="vertexData"></param>
/// <param name="particle"></param>
void CCloth::AddVertexData(std::vector<float>& vertexData, CParticle* particle)
{
    //position data (x,y,z)
    vertexData.push_back(particle->GetPosition().x);
    vertexData.push_back(particle->GetPosition().y);
    vertexData.push_back(particle->GetPosition().z);

    //Texture Coordinates (u,v)
    vertexData.push_back(particle->GetTextureCoord().x);
    vertexData.push_back(particle->GetTextureCoord().y);

    //normal data (x,y,z)
    vertexData.push_back(particle->GetNormal().x);
    vertexData.push_back(particle->GetNormal().y);
    vertexData.push_back(particle->GetNormal().z);
}

/// <summary>
/// Cloth deconstructor
/// </summary>
CCloth::~CCloth()
{
}

/// <summary>
/// Main update function for cloth, updates all particles and constraints, then rebinds
/// </summary>
/// <param name="deltaTime"></param>
void CCloth::Update(float deltaTime)
{
    //calcualte the wind vector based on the wind direction and strength
    g_wind.x = cos(glm::radians(windDirection)) * windStrength;
    g_wind.y = 0;
    g_wind.z = sin(glm::radians(windDirection)) * windStrength;

    //Update all particles
    for (int y = 0; y <= clothHeightDivisions - 1; y++) {
        int hookNum = 0;
        for (int x = 0; x <= clothWidthDivisions - 1; x++) {
            //Top Left Half of quad
            CParticle* part = m_particles[y][x];

            //Update hook positions
            if (part->GetIsFixed()) {
                part->SetPosition(glm::vec3(hookNum * hookDistance, part->GetPosition().y, part->GetPosition().z));
                hookNum++;
            }

            part->SetDamping(clothDampening);

            part->Update(deltaTime);
        }
    }

    //make a temporary vector of constraints
    //std::vector<CConstraint*> tempConstraints = m_constraints;
    //iterate through the temporary vector of constraints randomly updating, then removing the constraint from the temporary vector
    //while (tempConstraints.size() > 0) {
    //    int index = rand() % tempConstraints.size();
    //    tempConstraints[index]->Update(deltaTime);
    //    tempConstraints.erase(tempConstraints.begin() + index);
    //}

    float m_iterations = 4;
    //perform the following code x times
    for (int i = 0; i < m_iterations; i++)
    {
        //for every partcile in the cloth, check collisions
        for (int y = 0; y <= clothHeightDivisions - 1; y++) {
            for (int x = 0; x <= clothWidthDivisions - 1; x++) {
                CParticle* part = m_particles[y][x];
                part->CheckCollisions();
            }
        }

        //loop through the constraints and update them
        for (int i = 0; i < m_constraints.size(); i++) {
            CConstraint* c = m_constraints[i];
            c->SetStiffness(clothStiffness * (c->GetType() == CConstraint::Type::BEND ? 0.15f : 1.0f));
            c->Update(deltaTime / m_iterations);
        }
    }

    

    Rebind();
}

/// <summary>
/// Renders Mesh
/// </summary>
void CCloth::Render()
{
    m_shape->Render();
}

/// <summary>
/// Resets to normal settings
/// </summary>
void CCloth::Reset()
{
    clothLength = 20.0f;
    clothWidth = 20.0f;
    clothWidthDivisions = 20;
    clothHeightDivisions = 20;
    
    numberOfHooks = 3;
    hookDistance = 10.0f;
    clothStiffness = 0.5f;

    Rebuild();
}

/// <summary>
/// Removes all fixed points (Drop)
/// </summary>
void CCloth::UnFixAll()
{
    //loop through all fixed particles (m_fixedParts) and set them to not fixed
    for (CParticle* p : m_fixedParts) {
        p->SetIsFixed(false);
    }
}

/// <summary>
/// Returns a specific particle
/// </summary>
/// <param name="_x"></param>
/// <param name="_y"></param>
/// <returns></returns>
CParticle* CCloth::GetParticle(int _x, int _y)
{
    //check if the x and y are within the bounds of the cloth
    if (_x < 0 || _x >= clothWidthDivisions || _y < 0 || _y >= clothHeightDivisions) {
        return nullptr;
    }

    return m_particles[_y][_x];
}

/// <summary>
/// Clears all particles, and creates new ones based on cloth parameters
/// </summary>
void CCloth::Rebuild() {

    //loop through each particle and delete it
    for (std::vector<CParticle*> _row : m_particles) {
        for (CParticle* _part : _row) {
            delete _part;
        }
    }

    //clear the vector of particles
    m_particles.clear();

    //loop through each constraint and delete it
    for (CConstraint* _con : m_constraints) {
        delete _con;
    }

    //clear the vector of constraints
    m_constraints.clear();


    //create vertices
    for (int y = 0; y < clothHeightDivisions; y++) {
        std::vector<CParticle*> row = {};
        for (int x = 0; x < clothWidthDivisions; x++) {
            CParticle* p1 = new CParticle(x * (clothWidth / clothWidthDivisions), -y * (clothLength / clothHeightDivisions), 0);
            p1->SetClothParent(this);
            p1->SetTextureCoord(glm::vec2(float(x) / float(clothWidthDivisions), float(-y) / float(clothWidthDivisions)));
            row.push_back(p1);
        }
        m_particles.push_back(row);
    }

    //create constraints 
    for (int y = 0; y < clothHeightDivisions; y++) {
        for (int x = 0; x < clothWidthDivisions; x++) {
            //Create STRUCTURAL constraints

            //check if the particle has a neighbour to the right, make horizontal STRUCTURAL constraint
            if (x < clothWidthDivisions - 1) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y][x + 1]);
                c->SetType(CConstraint::Type::STRUCTURAL);
                m_constraints.push_back(c);
            }

            //check if the particle has a neighbour below, make vertical STRUCTURAL constraint
            if (y < clothHeightDivisions - 1) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y + 1][x]);
                c->SetType(CConstraint::Type::STRUCTURAL);
                m_constraints.push_back(c);
            }


            //Create SHEAR constraints

            //check if the particle has a neighbour to the right and up, make diagonal SHEAR constraint
            if (x < clothWidthDivisions - 1 && y > 0) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y - 1][x + 1]);
                c->SetType(CConstraint::Type::SHEAR);
                m_constraints.push_back(c);
            }

            //check if the particle has a neighbour to the right and down, make diagonal SHEAR constraint
            if (x < clothWidthDivisions - 1 && y < clothHeightDivisions - 1) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y + 1][x + 1]);
                c->SetType(CConstraint::Type::SHEAR);
                m_constraints.push_back(c);
            }


            //Create BEND constraints

            //check if the particle has a neighbour TWO to the right and TWO up, make diagonal BEND constraint
            if (x < clothWidthDivisions - 2 && y > 1) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y - 2][x + 2]);
                c->SetType(CConstraint::Type::BEND);
                m_constraints.push_back(c);
            }

            //check if the particle has a neighbour TWO to the right and TWO down, make diagonal BEND constraint
            if (x < clothWidthDivisions - 2 && y < clothHeightDivisions - 2) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y + 2][x + 2]);
                c->SetType(CConstraint::Type::BEND);
                m_constraints.push_back(c);
            }

            //check if the particle has a neighbour TWO to the right, make horizontal BEND constraint
            if (x < clothWidthDivisions - 2) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y][x + 2]);
                c->SetType(CConstraint::Type::BEND);
                m_constraints.push_back(c);
            }

            //check if the particle has a neighbour TWO below, make vertical BEND constraint
            if (y < clothHeightDivisions - 2) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y + 2][x]);
                c->SetType(CConstraint::Type::BEND);
                m_constraints.push_back(c);
            }
        }
    }

    //Fix the top row of particles based on the number of hooks
    float hookSpace = (float(clothWidthDivisions - 1)) / (float(numberOfHooks - 1));

    m_fixedParts.clear();
    for (int i = 0; i < numberOfHooks; i++) {
        CParticle* part = m_particles[0][int(i * hookSpace)];
        part->SetPosition(part->GetPosition() + glm::vec3(0, 0, i % 2 == 0 ? 0.5f: - 0.5f));
        part->SetIsFixed(true);
        m_fixedParts.push_back(part);
    }

    //loop through every particle and give them a random z position between -0.5 and 0.5
    for (int y = 0; y < clothHeightDivisions; y++) {
        for (int x = 0; x < clothWidthDivisions; x++) {
            if (m_particles[y][x]->GetIsFixed()) continue;

            m_particles[y][x]->SetPosition(m_particles[y][x]->GetPosition() + glm::vec3(0.0f, 0.0f, (rand() % 100) / 100.0f - 0.5f));
        }
    }

    Rebind();
}