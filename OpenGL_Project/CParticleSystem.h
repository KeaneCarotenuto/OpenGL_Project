#pragma once
#include "utility.h"
#include "CCamera.h"

//forward dec
class CParticleSystem;

class CParticle{
private:
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    float m_lifeTime = 0.0f;
    float m_elapsedTime = 0.0f;


public:
    CParticle(glm::vec3 _position, glm::vec3 _velocity, glm::vec3 _acceleration, float _lifeTime = 5.0f);
    ~CParticle();

    CParticleSystem* m_parentSystem = nullptr;

    void Update(float _deltaTime);
    void Reset();

    glm::vec3 GetPosition() { return m_position; };
    void SetPosition(glm::vec3 _position) { m_position = _position; };

    glm::vec3 GetVelocity() { return m_velocity; };
    void SetVelocity(glm::vec3 _velocity) { m_velocity = _velocity; };

    glm::vec3 GetAcceleration() { return m_acceleration; };
    void SetAcceleration(glm::vec3 _acceleration) { m_acceleration = _acceleration; };

    float GetElapsedTime() { return m_elapsedTime; };
};

class CParticleSystem
{
private:
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);

    std::vector<CParticle*> m_particles;
    float m_elapsedTime = 0.0f;

    CCamera* m_camera = nullptr;
    glm::mat4 m_PVMMat = glm::mat4(0.0f);

    GLuint m_VAO = NULL;
    GLuint m_VBO = NULL;
    GLuint m_EBO = NULL;
    GLuint m_texture = NULL;
    GLuint m_program = NULL;

public:
    CParticleSystem(glm::vec3 _position, GLuint _texture, GLuint _program, CCamera* _cam, int _numOfParts = 100);
    ~CParticleSystem();

    void Update(float _deltaTime);
    void Rebind();
    void Render(float _deltaTime);

    void GenBindVerts();

    void SetPosition(glm::vec3 _position) { m_position = _position; };
    glm::vec3 GetPosition() { return m_position; };

    void SetTexture(GLuint _texture) { m_texture = _texture; };
    GLuint GetTexture() { return m_texture; };

    void SetProgram(GLuint _program) { m_program = _program; };
    GLuint GetProgram() { return m_program; };
};