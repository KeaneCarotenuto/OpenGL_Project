#include "CCloth.h"


glm::vec3 CCloth::g_gravity = glm::vec3(0.0f, -9.81f, 0.0f) ;
glm::vec3 CCloth::g_wind = glm::vec3(0.0f, 0.0f, 0.0f) ;

//CParticle Constructor
CParticle::CParticle(glm::vec3 _position) {
    m_position = _position;
    m_prevPosition = _position;
}

CParticle::CParticle(float _x, float _y, float _z) : CParticle(glm::vec3(_x, _y, _z)) {}

CParticle::~CParticle()
{
}

void CParticle::update(float _deltaTime)
{
    if (m_isFixed) return;

    //add gravity force
    addForce(m_parentCloth->GetGravity());
    addForce(m_parentCloth->GetWind());

    //add damping force
    addForce(-m_velocity * m_damping);

    //calcualte the world position of the particle based on the parent cloth object position
    glm::vec3 worldPos = m_parentCloth->GetPosition() + m_position;
    glm::vec3 floorPos = m_parentCloth->GetFloorShape()->GetPosition();
    float thickness = 0.01f;

    //check if the world position is below the ground plane and if so, set the position to the ground plane position and set the acceleration to zero
    if (worldPos.y < floorPos.y + thickness)
    {
        //Normal Force
        addForce(glm::vec3(0, -m_acceleration.y * m_mass, 0));

        //But also set the position to avoid clipping
        m_position.y = (floorPos.y + thickness - m_parentCloth->GetPosition()).y;
        m_prevPosition = m_position;
    }

    //Update position using Verlet integration method, using acceleration and damping 
    glm::vec3 temp = m_position;
    m_position = (1.0f + m_damping) * m_position - (m_damping * m_prevPosition) + m_acceleration * (_deltaTime * _deltaTime);
    m_prevPosition = temp;

    //calculate the velocity of the particle based on the difference between the current and previous position 
    m_velocity = (m_position - m_prevPosition);

    //clear the acceleration
    m_acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
}

CConstraint::CConstraint(CParticle* _pParticle1, CParticle* _pParticle2){
    m_pParticle1 = _pParticle1;
    m_pParticle2 = _pParticle2;

    m_restLength = glm::distance(m_pParticle1->GetPosition(), m_pParticle2->GetPosition());
}

CConstraint::~CConstraint()
{
}

void CConstraint::Update(float _deltaTime)
{
    glm::vec3 delta = m_pParticle1->GetPosition() - m_pParticle2->GetPosition();
    float deltaLength = glm::length(delta);
    float difference = (m_restLength - deltaLength) / deltaLength;
    float m1 = 1.0f / m_pParticle1->GetMass();
    float m2 = 1.0f / m_pParticle2->GetMass();
    if (!m_pParticle1->GetIsFixed()) m_pParticle1->SetPosition(m_pParticle1->GetPosition() + delta * (m1 / (m1 + m2)) * m_stiffness * difference);
    if (!m_pParticle2->GetIsFixed()) m_pParticle2->SetPosition(m_pParticle2->GetPosition() - delta * (m2 / (m2 + m1)) * m_stiffness * difference);
}

//Cloth constructor that takes in the position of the cloth in the world, as well as the number of rows and columns of the cloth
CCloth::CCloth(CShape* _shape, int _width, int _height)
{
    m_shape = _shape;
    clothWidthDivisions = _width;
    clothHeightDivisions = _height;
    
    Rebuild();
}

void CCloth::Rebind()
{
    CMesh* mesh = m_shape->GetMesh();

    //create a vector of floats to be used as the vertex data, then fill it with the verticies create above
    std::vector<float> vertexData = {};

    std::vector< int > indices = {};
    
    int index = 0;
	for (int y = 0; y < clothHeightDivisions - 1; y++) {
		for (int x = 0; x < clothWidthDivisions - 1; x++) {
            //Top Left Half of quad
            CParticle* p1 = m_particles[y][x];
            CParticle* p2 = m_particles[y + 1][x];
            CParticle* p3 = m_particles[y][x + 1];

            //calculate the normal of the triangle formed by the three verticies
            glm::vec3 normal1 = glm::normalize(glm::cross(p2->GetPosition() - p1->GetPosition(), p3->GetPosition() - p1->GetPosition()));

            p1->SetNormal(normal1);
            p2->SetNormal(normal1);
            p3->SetNormal(normal1);

            AddVertexData(vertexData, p1);
            AddVertexData(vertexData, p2);
            AddVertexData(vertexData, p3);

            indices.push_back(index++);
            indices.push_back(index++);
            indices.push_back(index++);

            //Bottom Right Half of quad
            CParticle* p4 = m_particles[y][x + 1];
            CParticle* p5 = m_particles[y + 1][x];
            CParticle* p6 = m_particles[y + 1][x + 1];

            //calculate the normal of the triangle formed by the three verticies
            glm::vec3 normal2 = glm::normalize(glm::cross(p5->GetPosition() - p4->GetPosition(), p6->GetPosition() - p4->GetPosition()));

            p4->SetNormal(normal2);
            p5->SetNormal(normal2);
            p6->SetNormal(normal2);

            AddVertexData(vertexData, p4);
            AddVertexData(vertexData, p5);
            AddVertexData(vertexData, p6);

            indices.push_back(index++);
            indices.push_back(index++);
            indices.push_back(index++);
		}
	}

    mesh->SetIndices(indices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());
    int* ind = &indices[0];
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), nullptr, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), ind, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVBO());
    float* verts = &vertexData[0];
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), verts, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

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

CCloth::~CCloth()
{
}

void CCloth::Update(float deltaTime)
{
    //calcualte the wind vector based on the wind direction and strength
    g_wind.x = cos(glm::radians(windDirection)) * windStrength;
    g_wind.y = 0;
    g_wind.z = sin(glm::radians(windDirection)) * windStrength;

    for (int y = 0; y <= clothHeightDivisions - 1; y++) {
        for (int x = 0; x <= clothWidthDivisions - 1; x++) {
            //Top Left Half of quad
            CParticle* part = m_particles[y][x];

            part->update(deltaTime);
        }
    }

    //make a temporary vector of constraints
    //std::vector<CConstraint*> tempConstraints = m_constraints;

    ////iterate through the temporary vector of constraints randomly updating, then removing the constraint from the temporary vector
    //while (tempConstraints.size() > 0) {
    //    int index = rand() % tempConstraints.size();
    //    tempConstraints[index]->Update(deltaTime);
    //    tempConstraints.erase(tempConstraints.begin() + index);
    //}

    //loop through the constraints and update them
    for (int i = 0; i < m_constraints.size(); i++) {
        m_constraints[i]->Update(deltaTime);
    }
    

    Rebind();
}

void CCloth::Render()
{
    m_shape->Render();
}

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
                m_constraints.push_back(c);
            }

            //check if the particle has a neighbour below, make vertical STRUCTURAL constraint
            if (y < clothHeightDivisions - 1) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y + 1][x]);
                m_constraints.push_back(c);
            }


            //Create SHEAR constraints

            //check if the particle has a neighbour to the right and up, make diagonal SHEAR constraint
            if (x < clothWidthDivisions - 1 && y > 0) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y - 1][x + 1]);
                m_constraints.push_back(c);
            }

            //check if the particle has a neighbour to the right and down, make diagonal SHEAR constraint
            if (x < clothWidthDivisions - 1 && y < clothHeightDivisions - 1) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y + 1][x + 1]);
                m_constraints.push_back(c);
            }


            //Create BEND constraints

            //check if the particle has a neighbour TWO to the right and TWO up, make diagonal BEND constraint
            if (x < clothWidthDivisions - 2 && y > 1) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y - 2][x + 2]);
                c->SetStiffness(0.25f);
                m_constraints.push_back(c);
            }

            //check if the particle has a neighbour TWO to the right and TWO down, make diagonal BEND constraint
            if (x < clothWidthDivisions - 2 && y < clothHeightDivisions - 2) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y + 2][x + 2]);
                c->SetStiffness(0.25f);
                m_constraints.push_back(c);
            }

            //check if the particle has a neighbour TWO to the right, make horizontal BEND constraint
            if (x < clothWidthDivisions - 2) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y][x + 2]);
                c->SetStiffness(0.25f);
                m_constraints.push_back(c);
            }

            //check if the particle has a neighbour TWO below, make vertical BEND constraint
            if (y < clothHeightDivisions - 2) {
                CConstraint* c = new CConstraint(m_particles[y][x], m_particles[y + 2][x]);
                c->SetStiffness(0.25f);
                m_constraints.push_back(c);
            }
        }
    }

    //Along the top row, starting at the left, set every 5th particle to be fixed
    for (int x = 0; x < clothWidthDivisions; x++) {
        if (x % 5 == 0 || x == clothWidthDivisions - 1) {
            m_particles[0][x]->SetIsFixed(true);
        }
    }

    //loop through every particle and give them a random z position between -0.5 and 0.5
    for (int y = 0; y < clothHeightDivisions; y++) {
        for (int x = 0; x < clothWidthDivisions; x++) {
            m_particles[y][x]->SetPosition(m_particles[y][x]->GetPosition() + glm::vec3(0.0f, 0.0f, (rand() % 100) / 100.0f - 0.5f));
        }
    }

    Rebind();
}