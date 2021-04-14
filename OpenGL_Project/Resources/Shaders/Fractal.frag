// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// 
// (c) 2021 Media Design School
//
// File Name   : Fractal.frag
// Description : Some fractal stuff on the GPU
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

#version 460 core

in vec2 FragTexCoords;

uniform float CurrentTime;

out vec4 FinalColor;

//How deep to calcualte for each point
#define MAX_IT 1000

int itterate()
{
    //Zoom (make it pow to keep const) and position data (not sure if doubles are actually worth it here...)
    double zoom = pow(1.5, mod(CurrentTime,45));
    double xpos = -0.7611251856176778352;
    double ypos = -0.084749953259429494645;

    //The real and imaginary coords
    double real = ((FragTexCoords.x - 0.5f) / zoom) + xpos;
    double imag = ((FragTexCoords.y - 0.5f) / zoom) + ypos;
 
    int iterations = 0;
    double constReal = real;
    double constImag = imag;
 
    //Repeat until hitting max depth, or dist too great
    while (iterations < MAX_IT)
    {
        double tempReal = real;

        //Actual fractal calculation vvv
        real = (real * real - imag * imag) + constReal;
        imag = (2.0 * tempReal * imag) + constImag;
         
        double dist = real * real + imag * imag;
        
        //Break if not converging/oscilating
        if (dist > 4.0)
        break;
 
        ++iterations;
    }
    return iterations;
}
 

void main() 
{
    int itt = itterate();
    //Colour a range of red based on itterations
    FinalColor = vec4(float(itt * pow(1.01f, mod(CurrentTime,45))) / MAX_IT, 0 , 0, 1);
}