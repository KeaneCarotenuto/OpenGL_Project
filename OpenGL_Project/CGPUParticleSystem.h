// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// 
// (c) 2021 Media Design School
//
// File Name   : CParticleSystem.h
// Description : Contains data and logic to make GPU particles
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com


#pragma once
#include "utility.h"
#include "CCamera.h"

class CGPUParticleSystem
{
private:

    std::vector<glm::vec4> m_initialPositions;
    std::vector<glm::vec4> m_initialVelocities;
    
    GLuint m_texture = NULL;

    CCamera* m_camera = nullptr;
    glm::mat4 m_PVMMat = glm::mat4(0.0f);

    const int m_numOfParts;

    GLuint m_posVBO = NULL;
    GLuint m_velVBO = NULL;
    GLuint m_initVelVBO = NULL;
    GLuint m_VAO = NULL;

    GLuint m_computeProgram = NULL;
    GLuint m_renderProgram = NULL;

public:
    CGPUParticleSystem(GLuint _texture, GLuint _renderProgram, GLuint _computeProgram, CCamera* _cam, int _numOfParts = 1000);
    ~CGPUParticleSystem();

    void Update(float _deltaTime);
    void Render(); 
};

