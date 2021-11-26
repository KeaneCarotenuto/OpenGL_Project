#include "CGPUParticleSystem.h"

/// <summary>
/// GPU particle constructor
/// </summary>
/// <param name="_texture"></param>
/// <param name="_renderProgram"></param>
/// <param name="_computeProgram"></param>
/// <param name="_cam"></param>
/// <param name="_numOfParts"></param>
CGPUParticleSystem::CGPUParticleSystem(GLuint _texture, GLuint _renderProgram, GLuint _computeProgram, CCamera* _cam, int _numOfParts) : m_numOfParts(_numOfParts)
{
    m_texture = _texture;
    m_renderProgram = _renderProgram;
    m_computeProgram = _computeProgram;

    m_camera = _cam;

    for (int i = 0; i < m_numOfParts; i++)
    {
        m_initialPositions.push_back(glm::vec4(
            utils::RandomFloat(0.0f, 200.0f) - 100.0f, 
            0.0f,
            utils::RandomFloat(0.0f, 200.0f) - 100.0f,
            utils::RandomFloat(0.0f, 1.0f) + 0.125f));

        m_initialVelocities.push_back(glm::vec4(
            utils::RandomFloat(0.0f, 0.1f), 
            utils::RandomFloat(-1.0f, 1.0f), 
            utils::RandomFloat(-0.1f, 0.1f), 
            utils::RandomFloat(0.0f, 1.0f) + 0.125f));
    }

    //store positions in buffers
    glGenBuffers(1, &m_posVBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_posVBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_initialPositions.size() * sizeof(glm::vec4), &m_initialPositions[0], GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_posVBO);

    //store velocities in buffers
    glGenBuffers(1, &m_velVBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_velVBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_initialVelocities.size() * sizeof(glm::vec4), &m_initialVelocities[0], GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_velVBO);

    //store initial velocities in buffers
    glGenBuffers(1, &m_initVelVBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_initVelVBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_initialVelocities.size() * sizeof(glm::vec4), &m_initialVelocities[0], GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_initVelVBO);

    //create vao
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, NULL, 0);
    glEnableVertexAttribArray(0);

    //unbind
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

CGPUParticleSystem::~CGPUParticleSystem()
{
}

void CGPUParticleSystem::Update(float _deltaTime)
{
}

//render
void CGPUParticleSystem::Render()
{
    //computer shader
    glUseProgram(m_computeProgram);

    glDispatchCompute(m_numOfParts / 128, 1, 1);
    //sync, wait for the compute shader to finish
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //render shader
    glUseProgram(m_renderProgram);


    //view matrix uniform
    glm::mat4 view = m_camera->GetCameraViewMat();

    glm::mat4 projection = m_camera->GetCameraProjectionMat();

    m_PVMMat = projection * view;

    glUniformMatrix4fv(glGetUniformLocation(m_renderProgram, "vp"), 1, GL_FALSE, glm::value_ptr(m_PVMMat));

    //billboarding uniform
    glm::vec3 vQuad1, vQuad2;
    glm::vec3 camFront = m_camera->GetCameraForwardDir();
    camFront = glm::normalize(camFront);
    vQuad1 = glm::cross(camFront, m_camera->GetCameraUpDir());
    vQuad1 = glm::normalize(vQuad1);
    vQuad2 = glm::cross(camFront, vQuad1);
    vQuad2 = glm::normalize(vQuad2);

    glUniform3f(glGetUniformLocation(m_renderProgram, "vQuad1"), vQuad1.x, vQuad1.y,
    vQuad1.z);
    glUniform3f(glGetUniformLocation(m_renderProgram, "vQuad2"), vQuad2.x, vQuad2.y,
    vQuad2.z);

    //Texture uniform
    glActiveTexture(GL_TEXTURE0 + m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(glGetUniformLocation(m_renderProgram, "Texture"), m_texture);

    glUniform1f(glGetUniformLocation(m_renderProgram, "CurrentTime"), utils::currentTime);

    //bind position buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, NULL, 0);
    glEnableVertexAttribArray(0);

    glDepthMask(GL_FALSE);

    //render
    glDrawArrays(GL_POINTS, 0, m_numOfParts);

    glDepthMask(GL_TRUE);

    //unbind
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}