// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// (c) 2021 Media Design School
//
// File Name   : CCloth.h
// Description : Manages all of the relevant data and classes to make a cloth
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

#pragma once
#include "Utility.h"
#include "CShape.h"

class CConstraint;
class CCloth;

/// <summary>
/// The particles of the cloth, they manage movement, weight, and most interactions
/// </summary>
class CParticle
{
private:
    CCloth* m_parentCloth = nullptr;

    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_prevPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec2 m_indexPos = { 0,0 };

    glm::vec3 m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec2 m_textureCoord = glm::vec2(0.0f, 0.0f);

    float m_mass = 1.0f;
    float m_damping = 0.99f;

    std::vector<CConstraint*> m_constraints;
    std::vector<CConstraint*> m_secondConstraints;

    glm::vec3 m_normal = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 m_color = glm::vec3(0.0f, 0.0f, 0.0f);

    bool m_isFixed = false;

    bool m_isBroken = false;

    bool m_isSelected = false;

    bool m_isColliding = false;

public:
    CParticle(glm::vec3 _position);
    CParticle(float _x, float _y, float _z);
    ~CParticle();

    void Update(float _deltaTime);
    void CheckCollisions();
    void AddForce(glm::vec3 _force) { m_acceleration += _force / m_mass; };
    void ResetForce() { m_acceleration = glm::vec3(0.0f, 0.0f, 0.0f); };

    void CalculateNormal(int _x, int _y);

    CCloth* GetClothParent() { return m_parentCloth; };
    glm::vec3 GetPosition() { return m_position; }
    glm::vec2 GetIndexPos() { return m_indexPos; }
    glm::vec3 GetPrevPosition() { return m_prevPosition; }
    glm::vec3 GetVelocity() { return m_velocity; }
    glm::vec3 GetAcceleration() { return m_acceleration; }
    glm::vec2 GetTextureCoord() { return m_textureCoord; }
    float GetMass() { return m_mass; }
    float GetDamping() { return m_damping; }
    std::vector<CConstraint*> GetConstraints() { return m_constraints; }
    std::vector<CConstraint*> GetSecondConstraints() { return m_secondConstraints; }
    glm::vec3 GetNormal() { return m_normal; }
    glm::vec3 GetColor() { return m_color; }
    bool GetIsFixed() { return m_isFixed; }
    bool GetIsSelected() { return m_isSelected; }
    bool GetIsColliding() { return m_isColliding; }
    bool GetIsBroken() { return m_isBroken; }

    void SetClothParent(CCloth* _parentCloth) { m_parentCloth = _parentCloth; };
    void SetPosition(glm::vec3 _position) { m_position = _position; }
    void SetPosition(float _x, float _y, float _z) { m_position = glm::vec3(_x, _y, _z); }
    void SetIndexPos(glm::vec2 _pos) { m_indexPos = _pos; }
    void SetPrevPosition(glm::vec3 _prevPosition) { m_prevPosition = _prevPosition; }
    void SetVelocity(glm::vec3 _velocity) { m_velocity = _velocity; }
    void SetAcceleration(glm::vec3 _acceleration) { m_acceleration = _acceleration; }
    void SetTextureCoord(glm::vec2 _textureCoord) { m_textureCoord = _textureCoord; }
    void SetMass(float _mass) { m_mass = _mass; }
    void SetDamping(float _damping) { m_damping = _damping; }
    void SetConstraints(std::vector<CConstraint*> _constraints) { m_constraints = _constraints; }
    void SetSecondConstraints(std::vector<CConstraint*> _secondConstraints) { m_secondConstraints = _secondConstraints; }
    void SetNormal(glm::vec3 _normal) { m_normal = _normal; }
    void SetColor(glm::vec3 _color) { m_color = _color; }
    void SetIsFixed(bool _isFixed) { m_isFixed = _isFixed; }
    void SetIsSelected(bool _isSelected) { m_isSelected = _isSelected; }
    void SetIsColliding(bool _isColliding) { m_isColliding = _isColliding; }
    void Break();

    void AddConstraint(CConstraint* _constraint) { m_constraints.push_back(_constraint); };
    void AddSecondConstraint(CConstraint* _constraint) { m_secondConstraints.push_back(_constraint); };

};

/// <summary>
/// The constraints of the cloth, manages how particles interact with eacother, and keeps the cloth together, behaving like cloth
/// </summary>
class CConstraint
{
private:
    enum class Type;

    CParticle* m_pParticle1 = nullptr;
    CParticle* m_pParticle2 = nullptr;

    float m_restLength = 1.0f;
    float m_stiffness = 0.8f;
    float m_damping = 0.99f;

    bool m_isBroken = false;

    CConstraint::Type m_type = CConstraint::Type::STRUCTURAL;

    glm::vec3 m_normal = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 m_force = glm::vec3(0.0f, 0.0f, 0.0f);

public:
    enum class Type {
        STRUCTURAL,
        SHEAR,
        BEND,
    };

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
    CConstraint::Type GetType() { return m_type; }
    void SetRestLength(float _restLength) { m_restLength = _restLength; }
    void SetStiffness(float _stiffness) { m_stiffness = _stiffness; }
    void SetDamping(float _damping) { m_damping = _damping; }
    void SetType(CConstraint::Type _type) { m_type = _type; }

    bool GetIsBroken() { return m_isBroken; }
    void Break();

    glm::vec3 GetNormal() { return m_normal; }
    glm::vec3 GetForce() { return m_force; }
    void SetNormal(glm::vec3 _normal) { m_normal = _normal; }
    void SetForce(glm::vec3 _force) { m_force = _force; }
};

/// <summary>
/// The actual cloth object that keeps track of particles, constraints, and most global variables
/// </summary>
class CCloth
{
private:
    CShape* m_shape = nullptr;

    CShape* m_floor = nullptr;

    static glm::vec3 g_gravity;
    static glm::vec3 g_wind;

    std::vector<std::vector<CParticle*>> m_particles;
    std::vector<CParticle*> m_hooks;
    std::vector<CConstraint*> m_constraints;

public:
    CCloth(CShape* _shape, int _width, int _height);
    ~CCloth();

    //GUI variables
    float clothLength = 20.0f;
    float clothWidth = 20.0f;
    int clothWidthDivisions = 20;
    int clothHeightDivisions = 20;
    bool smoothNormals = true;

    int numberOfHooks = 3;
    float hookDistance = 10.0f;
    float clothStiffness = 0.5f;
    float clothDampening = 0.99f;

    const char* mouseModeItems[6]{ "Grab", "Pull", "Push", "Tear", "Fire", "Pin" };
    int selectedMouseMode = 0;

    const char* collisionItems[4]{ "No Object", "Sphere", "Capsule", "Pyramid" };
    int selectedCollision = 0;

    float windDirection = 90.0f;
    float windStrength = 5.0f;

    void Rebind();
    void CreateTri(CParticle* p1, CParticle* p2, CParticle* p3, std::vector<float>& vertexData, std::vector<int>& indices, int& index);
    void AddVertexData(std::vector<float>& vertexData, CParticle* particle);

    void Update(float deltaTime);
    void Render();

    void AddConstraint(CConstraint constraint);
    void AddParticle(CParticle particle);

    void Rebuild();
    void Reset();

    void AutoDistanceHooks();
    void UnFixAll();

    CShape* GetFloorShape() { return m_floor; };
    void SetFloorShape(CShape* _floor) { m_floor = _floor; };

    void SetGravity(glm::vec3 _gravity) { g_gravity = _gravity; };
    glm::vec3 GetGravity() { return g_gravity; };
    void SetWind(glm::vec3 _wind) { g_wind = _wind; };
    glm::vec3 GetWind() { return g_wind; };

    std::vector<std::vector<CParticle*>> GetParticles() { return m_particles; };
    std::vector<CConstraint*> GetConstraints() { return m_constraints; };

    CParticle* GetParticle(int _x, int _y);

    int GetWidth() { return clothWidth; };
    void SetWidth(float _height) { clothWidth = _height; };
    int GetLength() { return clothLength; };
    void SetLength(float _Length) { clothLength = _Length; };

    int GetWidthDivisions() { return clothWidthDivisions; };
    int GetHeightDivisions() { return clothHeightDivisions; };

    glm::vec3 GetPosition() { return m_shape->GetPosition(); };
    void SetPosition(glm::vec3 position) { m_shape->SetPosition(position); };

    void SetWidthDivisions(int width) { clothWidthDivisions = width; };
    void SetHeightDivisions(int height) { clothHeightDivisions = height; };

    void SetParticles(std::vector<std::vector<CParticle*>> particles);
    void SetConstraints(std::vector<CConstraint*> constraints);
};