#include "CCloth.h"

//CParticle Constructor
CParticle::CParticle(glm::vec3 _position){
    m_position = _position;

    m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    m_force = glm::vec3(0.0f, 0.0f, 0.0f);
    
    m_mass = 1.0f;
    m_damping = 0.99f;
}

CParticle::CParticle(float _x, float _y, float _z) : CParticle(glm::vec3(_x, _y, _z)){}

CConstraint::CConstraint(CParticle* _pParticle1, CParticle* _pParticle2){
    m_pParticle1 = _pParticle1;
    m_pParticle2 = _pParticle2;

    m_restLength = glm::distance(m_pParticle1->GetPosition(), m_pParticle2->GetPosition());

    m_stiffness = 0.9f;
    m_damping = 0.99f;
}

//Cloth constructor that takes in the position of the cloth in the world, as well as the number of rows and columns of the cloth
CCloth::CCloth(CShape* _shape, int _width, int _height)
{
    m_shape = _shape;
    m_width = _width;
    m_height = _height;

    std::vector< int > indices = {};
    
    int index = 0;
	for (int y = _height; y > 1; y--) {
		std::vector<CParticle*> row = {};
		for (int x = _width; x > 1; x--) {
            //Top Left Half of quad
            CParticle* p1 = new CParticle(x, y, sin(utils::currentTime + y));
			CParticle* p2 = new CParticle(x, y + 1, sin(utils::currentTime + y + 1));
			CParticle* p3 = new CParticle(x + 1, y, sin(utils::currentTime + y));

            row.push_back(p1);
            row.push_back(p2);
            row.push_back(p3);

            CConstraint* c1 = new CConstraint(p1, p2);
            CConstraint* c2 = new CConstraint(p1, p3);
            CConstraint* c3 = new CConstraint(p2, p3);

            m_constraints.push_back(c1);
            m_constraints.push_back(c2);
            m_constraints.push_back(c3);

            indices.push_back(index++);
            indices.push_back(index++);
            indices.push_back(index++);


            //Bottom Right Half of quad
			CParticle* p4 = new CParticle(x + 1, y, sin(utils::currentTime + y));
			CParticle* p5 = new CParticle(x, y + 1, sin(utils::currentTime + y + 1));
			CParticle* p6 = new CParticle(x + 1, y + 1, sin(utils::currentTime + y + 1));

            row.push_back(p4);
            row.push_back(p5);
            row.push_back(p6);

            CConstraint* c4 = new CConstraint(p4, p5);
            CConstraint* c5 = new CConstraint(p4, p6);
            CConstraint* c6 = new CConstraint(p5, p6);

            m_constraints.push_back(c4);
            m_constraints.push_back(c5);
            m_constraints.push_back(c6);

            indices.push_back(index++);
            indices.push_back(index++);
            indices.push_back(index++);
		}
		m_particles.push_back(row);
	}

    Rebind();
}

void CCloth::Rebind()
{
    CMesh* mesh = m_shape->GetMesh();

    //create a vector of floats to be used as the vertex data, then fill it with the verticies create above
    std::vector<float> vertexData = {};
    for (int i = 0; i < m_particles.size(); i++) {
        for (int j = 0; j < m_particles[i].size(); j++) {
            //position data (x,y,z)
            vertexData.push_back(m_particles[i][j]->GetPosition().x);
            vertexData.push_back(m_particles[i][j]->GetPosition().y);
            vertexData.push_back(m_particles[i][j]->GetPosition().z);

            //Texture Coordinates (u,v)
            vertexData.push_back(m_particles[i][j]->GetPosition().x / float(m_width));
            vertexData.push_back(m_particles[i][j]->GetPosition().y / float(m_height));

            //normal data (x,y,z)
            vertexData.push_back(0.0f);
            vertexData.push_back(0.0f);
            vertexData.push_back(1.0f);
        }
    }


    glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVBO());
    float* verts = &vertexData[0];
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), verts, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CCloth::Update(float deltaTime)
{
    for (int y = m_height; y > 1; y--) {
        for (int x = m_width; x > 1; x--) {
            //Top Left Half of quad
            m_particles[y][x]->SetPosition(x, y, sin(utils::currentTime + y));
            m_particles[y][x+1]->SetPosition(x, y + 1, sin(utils::currentTime + y + 1));
            m_particles[y][x+2]->SetPosition(x + 1, y, sin(utils::currentTime + y));

            //Bottom Right Half of quad
            m_particles[y][x+3]->SetPosition(x + 1, y, sin(utils::currentTime + y));
            m_particles[y][x]->SetPosition(x, y + 1, sin(utils::currentTime + y + 1));
            m_particles[y][x]->SetPosition(x + 1, y + 1, sin(utils::currentTime + y + 1));
        }
    }
}
