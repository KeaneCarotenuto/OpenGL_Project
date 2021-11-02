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
    m_pParticle1 = _pParticle2;

    m_restLength = glm::distance(m_pParticle1->getPosition(), m_pParticle2->getPosition());

    m_stiffness = 0.9f;
    m_damping = 0.99f;
}

//Cloth constructor that takes in the position of the cloth in the world, as well as the number of rows and columns of the cloth
CCloth::CCloth(CShape* _shape, int width, int height)
{
    m_shape = _shape;

    std::vector< int > indices = {};
    
    int index = 0;
	for (int y = height; y > 1; y--) {
		std::vector<CParticle*> row = {};
		for (int x = width; x > 1; x--) {
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

    CMesh* mesh = m_shape->GetMesh();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());
	int* ind = &indices[0];
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), ind, GL_DYNAMIC_DRAW);

	//create a vector of floats to be used as the vertex data, then fill it with the verticies create above
	std::vector<float> vertexData = {};
	for (int i = 0; i < m_particles.size(); i++) {
		for (int j = 0; j < m_particles[i].size(); j++) { 
            //position data (x,y,z)
			vertexData.push_back(m_particles[i][j]->getPosition().x);
            vertexData.push_back(m_particles[i][j]->getPosition().y);
            vertexData.push_back(m_particles[i][j]->getPosition().z);

            //Texture Coordinates (u,v)
            vertexData.push_back(m_particles[i][j]->getPosition().x / m_width);
            vertexData.push_back(m_particles[i][j]->getPosition().y / m_height);

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
