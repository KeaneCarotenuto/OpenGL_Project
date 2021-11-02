#pragma once
#include "Utility.h"
#include "CShape.h"

class CConstraint;

class CParticle
{
private:
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_prevPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_force = glm::vec3(0.0f, 0.0f, 0.0f);

    float m_mass = 1.0f;
    float m_damping = 0.99f;

    std::vector<CConstraint*> m_constraints;

    glm::vec3 m_normal = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 m_color = glm::vec3(0.0f, 0.0f, 0.0f);

    bool m_isFixed = false;

    bool m_isSelected = false;

    bool m_isColliding = false;

public:
    CParticle(glm::vec3 _position);
    CParticle(float _x, float _y, float _z);
    ~CParticle();

    void update(float _deltaTime);
    void addForce(glm::vec3 _force);
    void resetForce();

    glm::vec3 getPosition() { return m_position; }
    glm::vec3 getPrevPosition() { return m_prevPosition; }
    glm::vec3 getVelocity() { return m_velocity; }
    glm::vec3 getAcceleration() { return m_acceleration; }
    glm::vec3 getForce() { return m_force; }
    float getMass() { return m_mass; }
    float getDamping() { return m_damping; }

    void setPosition(glm::vec3 _position) { m_position = _position; }
    void setPrevPosition(glm::vec3 _prevPosition) { m_prevPosition = _prevPosition; }
    void setVelocity(glm::vec3 _velocity) { m_velocity = _velocity; }
    void setAcceleration(glm::vec3 _acceleration) { m_acceleration = _acceleration; }
    void setForce(glm::vec3 _force) { m_force = _force; }
    void setMass(float _mass) { m_mass = _mass; }
    void setDamping(float _damping) { m_damping = _damping; }

};

class CConstraint
{
private:
    CParticle* m_pParticle1 = nullptr;
    CParticle* m_pParticle2 = nullptr;

    float m_restLength = 0.0f;
    float m_stiffness = 1.0f;
    float m_damping = 0.99f;

    glm::vec3 m_normal = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 m_force = glm::vec3(0.0f, 0.0f, 0.0f);

    float m_distance = 0.0f;

public:
    CConstraint(CParticle* _pParticle1, CParticle* _pParticle2);
    ~CConstraint();

    void update(float _deltaTime);
    void addForce(glm::vec3 _force);
    void resetForce();

    glm::vec3 getNormal() { return m_normal; }
    glm::vec3 getForce() { return m_force; }
    float getDistance() { return m_distance; }

    void setNormal(glm::vec3 _normal) { m_normal = _normal; }
    void setForce(glm::vec3 _force) { m_force = _force; }
    void setDistance(float _distance) { m_distance = _distance; }

};

class CCloth
{
private:
    CShape* m_shape = nullptr;

    int m_width;
    int m_height;

    std::vector<std::vector<CParticle*>> m_particles;
    std::vector<CConstraint*> m_constraints;

public:
    CCloth(CShape* _shape, int width, int height);
    ~CCloth();

    void Update(float deltaTime);
    void Render();

    void AddConstraint(CConstraint constraint);
    void AddParticle(CParticle particle);

    void Reset();

    std::vector<std::vector<CParticle*>> GetParticles() { return m_particles; };
    std::vector<CConstraint*> GetConstraints() { return m_constraints; };

    int GetWidth() { return m_width; };
    int GetHeight() { return m_height; };

    glm::vec3 GetPosition() { return m_shape->GetPosition(); };

    void SetPosition(glm::vec3 position) { m_shape->SetPosition(position); };

    void SetWidth(int width) { m_width = width; };
    void SetHeight(int height) { m_height = height; };

    void SetParticles(std::vector<std::vector<CParticle*>> particles);
    void SetConstraints(std::vector<CConstraint*> constraints);

    //void SetParticle(int x, int y, CParticle* particle);
};