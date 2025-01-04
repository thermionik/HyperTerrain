#ifndef UTILITY_H
#define UTILITY_H

#include "types.h"
#include <math.h>
void ReduceToUnit(float *);

class Vector3 {
    public:
        Vector3();
        Vector3(float x1, float y1, float z1, float x2, float y2, float z2);
        Vector3(Point3d, Point3d);
        void set(float u1,float u2,float u3){ x = u1; y = u2; z = u3; };
        void Normalize();
        Vector3 operator*(const Vector3&) const;
        void operator*(float);
        float operator[](int);
    
    public:
        float x;
        float y;
        float z;
};


#endif