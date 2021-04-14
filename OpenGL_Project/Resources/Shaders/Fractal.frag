#version 460 core

in vec2 FragTexCoords;

//uniform float zoom;
//uniform vec2 pos;
uniform float CurrentTime;

out vec4 FinalColor;

#define MAX_ITERATIONS 1000

int get_iterations()
{
    double zoom = pow(1.5, mod(CurrentTime,45));
    double xpos = -0.7611251856176778352;
    double ypos = -0.084749953259429494645;

    double real = ((FragTexCoords.x - 0.5f) / zoom) + xpos;
    double imag = ((FragTexCoords.y - 0.5f) / zoom) + ypos;
 
    int iterations = 0;
    double const_real = real;
    double const_imag = imag;
 
    while (iterations < MAX_ITERATIONS)
    {
        double tmp_real = real;
        real = (real * real - imag * imag) + const_real;
        imag = (2.0 * tmp_real * imag) + const_imag;
         
        double dist = real * real + imag * imag;
         
        if (dist > 4.0)
        break;
 
        ++iterations;
    }
    return iterations;
}
 

void main() 
{
    int itt = get_iterations();
    FinalColor = vec4(float(itt * pow(1.01f, mod(CurrentTime,45))) / MAX_ITERATIONS, 0 , 0, 1);
}