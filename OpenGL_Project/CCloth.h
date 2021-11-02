#pragma once
#include "Utility.h"
#include "CShape.h"

class CConstraint;
class CCloth;

class CParticle
{
private:
    CCloth* m_parentCloth = nullptr;

    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_prevPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec2 m_textureCoord = glm::vec2(0.0f, 0.0f);

    float m_mass = 1.0f;
    float m_damping = 0.91f;

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
    void addForce(glm::vec3 _force) { m_acceleration += _force / m_mass; };
    void resetForce() { m_acceleration = glm::vec3(0.0f, 0.0f, 0.0f); };

    CCloth* GetClothParent() { return m_parentCloth; };
    glm::vec3 GetPosition() { return m_position; }
    glm::vec3 GetPrevPosition() { return m_prevPosition; }
    glm::vec3 GetVelocity() { return m_velocity; }
    glm::vec3 GetAcceleration() { return m_acceleration; }
    glm::vec2 GetTextureCoord() { return m_textureCoord; }
    float GetMass() { return m_mass; }
    float GetDamping() { return m_damping; }
    std::vector<CConstraint*> GetConstraints() { return m_constraints; }
    glm::vec3 GetNormal() { return m_normal; }
    glm::vec3 GetColor() { return m_color; }
    bool GetIsFixed() { return m_isFixed; }
    bool GetIsSelected() { return m_isSelected; }
    bool GetIsColliding() { return m_isColliding; }

    void SetClothParent(CCloth* _parentCloth) { m_parentCloth = _parentCloth; };
    void SetPosition(glm::vec3 _position) { m_position = _position; }
    void SetPosition(float _x, float _y, float _z) { m_position = glm::vec3(_x, _y, _z); }
    void SetPrevPosition(glm::vec3 _prevPosition) { m_prevPosition = _prevPosition; }
    void SetVelocity(glm::vec3 _velocity) { m_velocity = _velocity; }
    void SetAcceleration(glm::vec3 _acceleration) { m_acceleration = _acceleration; }
    void SetTextureCoord(glm::vec2 _textureCoord) { m_textureCoord = _textureCoord; }
    void SetMass(float _mass) { m_mass = _mass; }
    void SetDamping(float _damping) { m_damping = _damping; }
    void SetConstraints(std::vector<CConstraint*> _constraints) { m_constraints = _constraints; }
    void SetNormal(glm::vec3 _normal) { m_normal = _normal; }
    void SetColor(glm::vec3 _color) { m_color = _color; }
    void SetIsFixed(bool _isFixed) { m_isFixed = _isFixed; }
    void SetIsSelected(bool _isSelected) { m_isSelected = _isSelected; }
    void SetIsColliding(bool _isColliding) { m_isColliding = _isColliding; }

};

class CConstraint
{
private:
    CParticle* m_pParticle1 = nullptr;
    CParticle* m_pParticle2 = nullptr;

    float m_restLength = 1.0f;
    float m_stiffness = 0.8f;
    float m_damping = 0.99f;

    glm::vec3 m_normal = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 m_force = glm::vec3(0.0f, 0.0f, 0.0f);

public:
    CConstraint(CParticle* _pParticle1, CParticle* _pParticle2);
    ~CConstraint();

    void Update(float _deltaTime);
    void AddForce(glm::vec3 _force);
    void ResetForce();

    CParticle* GetParticle1() { return m_pParticle1; }
    CParticle* GetParticle2() { return m_pParticle2; }
    void SetParticle1(CParticle* _pParticle1) { m_pParticle1 = _pParticle1; }
    void SetParticle2(CParticle* _pParticle2) { m_pParticle2 = _pParticle2; }

    float GetRestLength() { return m_restLength; }
    float GetStiffness() { return m_stiffness; }
    float GetDamping() { return m_damping; }
    void SetRestLength(float _restLength) { m_restLength = _restLength; }
    void SetStiffness(float _stiffness) { m_stiffness = _stiffness; }
    void SetDamping(float _damping) { m_damping = _damping; }

    glm::vec3 GetNormal() { return m_normal; }
    glm::vec3 GetForce() { return m_force; }
    void SetNormal(glm::vec3 _normal) { m_normal = _normal; }
    void SetForce(glm::vec3 _force) { m_force = _force; }
};

class CCloth
{
private:
    CShape* m_shape = nullptr;

    CShape* m_floor = nullptr;

    static glm::vec3 g_gravity;
    static glm::vec3 g_wind;

    std::vector<std::vector<CParticle*>> m_particles;
    std::vector<CConstraint*> m_constraints;

public:
    CCloth(CShape* _shape, int _width, int _height);
    ~CCloth();

    //GUI variables
    float clothLength = 20.0f;
    float clothWidth = 20.0f;
    int clothWidthDivisions = 20;
    int clothHeightDivisions = 20;

    int numberOfHooks = 3;
    float hookDistance = 10.0f;
    float clothStiffness = 0.5f;

    const char* mouseModeItems[5]{ "Pull", "Push", "Tear", "Fire", "Pin" };
    int selectedMouseMode = 0;

    const char* collisionItems[4]{ "No Object", "Sphere", "Capsule", "Pyramid" };
    int selectedCollision = 0;

    float windDirection = 270.0f;
    float windStrength = 10.0f;

    void Rebind();
    void AddVertexData(std::vector<float>& vertexData, CParticle* particle);

    void Update(float deltaTime);
    void Render();

    void AddConstraint(CConstraint constraint);
    void AddParticle(CParticle particle);

    void Rebuild();
    void Reset();

    CShape* GetFloorShape() { return m_floor; };
    void SetFloorShape(CShape* _floor) { m_floor = _floor; };

    void SetGravity(glm::vec3 _gravity) { g_gravity = _gravity; };
    glm::vec3 GetGravity() { return g_gravity; };
    void SetWind(glm::vec3 _wind) { g_wind = _wind; };
    glm::vec3 GetWind() { return g_wind; };

    std::vector<std::vector<CParticle*>> GetParticles() { return m_particles; };
    std::vector<CConstraint*> GetConstraints() { return m_constraints; };

    int GetWidth() { return clothWidthDivisions; };
    int GetHeight() { return clothHeightDivisions; };

    glm::vec3 GetPosition() { return m_shape->GetPosition(); };
    void SetPosition(glm::vec3 position) { m_shape->SetPosition(position); };

    void SetWidth(int width) { clothWidthDivisions = width; };
    void SetHeight(int height) { clothHeightDivisions = height; };

    void SetParticles(std::vector<std::vector<CParticle*>> particles);
    void SetConstraints(std::vector<CConstraint*> constraints);

    //void SetParticle(int x, int y, CParticle* particle);
};