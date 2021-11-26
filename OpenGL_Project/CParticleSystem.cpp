#include "CParticleSystem.h"

/// <summary>
/// Particle constructor
/// </summary>
/// <param name="_position"></param>
/// <param name="_velocity"></param>
/// <param name="_acceleration"></param>
/// <param name="_lifeTime"></param>
CParticle::CParticle(glm::vec3 _position, glm::vec3 _velocity, glm::vec3 _acceleration, float _lifeTime)
{
    m_position = _position;
    m_velocity = _velocity;
    m_acceleration = _acceleration;
    m_lifeTime = _lifeTime;
    m_elapsedTime = 0.0f;
}

CParticle::~CParticle()
{
}


/// <summary>
/// Update method for particle, does movement and stuff
/// </summary>
/// <param name="_deltaTime"></param>
void CParticle::Update(float _deltaTime)
{
    //dampen the velocity
    m_velocity -= m_velocity * 0.1f * _deltaTime;
    m_acceleration.x = (m_parentSystem->GetPosition().x - m_position.x) / 2.0f;
    m_acceleration.z = (m_parentSystem->GetPosition().z - m_position.z) / 2.0f;

    m_velocity += m_acceleration * _deltaTime;
    m_position += m_velocity * _deltaTime;
    m_elapsedTime += _deltaTime;

    if (m_elapsedTime > m_lifeTime) {
        Reset();
    }
}


/// <summary>
/// Resets particle to start pos
/// </summary>
void CParticle::Reset()
{
    m_position = m_parentSystem->GetPosition();
    m_elapsedTime = 0;

    m_velocity = glm::vec3(utils::RandomFloat(-1.0f, 1.0f), utils::RandomFloat(-1.0f, 1.0f), utils::RandomFloat(-1.0f, 1.0f));
    m_velocity = glm::normalize(m_velocity);
    m_acceleration = glm::vec3(0, utils::RandomFloat(1.0f, 2.0f), 0);
}

/// <summary>
/// Particle system constructor
/// </summary>
/// <param name="_position"></param>
/// <param name="_texture"></param>
/// <param name="_program"></param>
/// <param name="_cam"></param>
/// <param name="_numOfParts"></param>
CParticleSystem::CParticleSystem(glm::vec3 _position, GLuint _texture, GLuint _program, CCamera* _cam, int _numOfParts)
{
    m_position = _position;
    m_texture = _texture;
    m_program = _program;
    m_elapsedTime = 0.0f;

    m_camera = _cam;

    for (int i = 0; i < _numOfParts; i++)
    {
        CParticle* part = new CParticle(m_position, glm::vec3(utils::RandomFloat(-1,1), utils::RandomFloat(-1,1), utils::RandomFloat(-1,1)), glm::vec3(0.0f, 0.0f, 0.0f), utils::RandomFloat(0.0f, 5.0f));
        part->m_parentSystem = this;
        part->Reset();
        m_particles.push_back(part);
    }

    GenBindVerts();
}

CParticleSystem::~CParticleSystem()
{
}

/// <summary>
/// Updates all particles and rebinds
/// </summary>
/// <param name="_deltaTime"></param>
void CParticleSystem::Update(float _deltaTime)
{
    //update particles
    for (CParticle* part : m_particles)
    {
        part->Update(_deltaTime);
    }

    Rebind();
}

/// <summary>
/// Rebinds data to buffers
/// </summary>
void CParticleSystem::Rebind()
{
    //create a temporary vector to store the vertices
    std::vector<float> tempVerts;
    //create a temporary vector to store the indices
    std::vector<int> tempIndices;

    //loop through all the particles
    for (unsigned int i = 0; i < m_particles.size(); i++)
    {
        //add the vertices to the temporary vector
        tempVerts.push_back(m_particles[i]->GetPosition().x);
        tempVerts.push_back(m_particles[i]->GetPosition().y);
        tempVerts.push_back(m_particles[i]->GetPosition().z);

        //add the indices to the temporary vector
        tempIndices.push_back(i);
    }

    //Update EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    int* ind = &tempIndices[0];
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, tempIndices.size() * sizeof(int), ind, GL_DYNAMIC_DRAW);

    //Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    float* verts = &tempVerts[0];
    glBufferData(GL_ARRAY_BUFFER, tempVerts.size() * sizeof(float), verts, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


}

/// <summary>
/// Render particle system
/// </summary>
/// <param name="_deltaTime"></param>
void CParticleSystem::Render(float _deltaTime)
{

    glUseProgram(m_program);

    //Update PVM matrix
	glm::mat4 translationMat = glm::translate(glm::mat4(), m_position);

    glm::mat4 view = m_camera->GetCameraViewMat();

    glm::mat4 projection = m_camera->GetCameraProjectionMat();

    m_PVMMat = projection * view;

    //send uniform
    glUniformMatrix4fv(glGetUniformLocation(m_program, "vp"), 1, GL_FALSE, glm::value_ptr(m_PVMMat));

    //billboarding
    glm::vec3 vQuad1, vQuad2;
    glm::vec3 camFront = m_camera->GetCameraForwardDir();
    camFront = glm::normalize(camFront);
    vQuad1 = glm::cross(camFront, m_camera->GetCameraUpDir());
    vQuad1 = glm::normalize(vQuad1);
    vQuad2 = glm::cross(camFront, vQuad1);
    vQuad2 = glm::normalize(vQuad2);

    glUniform3f(glGetUniformLocation(m_program, "vQuad1"), vQuad1.x, vQuad1.y,
    vQuad1.z);
    glUniform3f(glGetUniformLocation(m_program, "vQuad2"), vQuad2.x, vQuad2.y,
    vQuad2.z);

    //texture
    glActiveTexture(GL_TEXTURE0 + m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(glGetUniformLocation(m_program, "Texture"), m_texture);

    glUniform1f(glGetUniformLocation(m_program, "CurrentTime"), utils::currentTime);

    glDepthMask(GL_FALSE);

    glBindVertexArray(m_VAO);

    glDrawElements(GL_POINTS, m_particles.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);

    glDepthMask(GL_TRUE);
}

/// <summary>
/// Generates and binds verticies for the first time
/// </summary>
void CParticleSystem::GenBindVerts()
{
    //create a temporary vector to store the vertices
    std::vector<float> tempVerts;
    //create a temporary vector to store the indices
    std::vector<int> tempIndices;

    //loop through all the particles
    for (unsigned int i = 0; i < m_particles.size(); i++)
    {
        //add the vertices to the temporary vector
        tempVerts.push_back(m_particles[i]->GetPosition().x);
        tempVerts.push_back(m_particles[i]->GetPosition().y);
        tempVerts.push_back(m_particles[i]->GetPosition().z);

        //add the indices to the temporary vector
        tempIndices.push_back(i);
    }

    //Create a Vertex Array
    glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//Gen EBO 
	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	int* ind = &tempIndices[0];
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tempIndices.size() * sizeof(int), ind, GL_DYNAMIC_DRAW);

	//Gen VBO 
	glGenBuffers(1, &m_VBO);
	//copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	float* verts = &tempVerts[0];
	glBufferData(GL_ARRAY_BUFFER, tempVerts.size() * sizeof(float), verts, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
}
