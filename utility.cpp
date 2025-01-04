#include "utility.h"

Vector3::Vector3(){};

Vector3::Vector3(float x1, float y1, float z1, float x2, float y2, float z2)
{
    x = x2 - x1;
    y = y2 - y1;
    z = z2 - z1;
};

Vector3::Vector3(Point3d p1, Point3d p2)
{
    x = p1.x - p2.x;
    y = p1.y - p2.y;
    z = p1.z - p2.z;
};
 
Vector3 Vector3::operator*(const Vector3& vector) const
{
    Vector3 cross;

/*    cross[0] = y * vector.z - z * vector.y;
    cross[1] = z * vector.x - x * vector.z;
    cross[2] = x * vector.y - y * vector.x;
*/
    return cross;
};

void Vector3::operator*(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
};

float Vector3::operator[](int index)
{
    switch (index)
    {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
		default:
			return -1.0;
    }
};

void Vector3::Normalize()
{
    float length;
    length = (float)sqrt(x*x + y*y + z*z);
    x /= length;
    y /= length;
    z /= length;
}

void ReduceToUnit(float vector[3])
	{
	float length;
	
	// Calculate the length of the vector		
	length = (float)sqrt((vector[0]*vector[0]) + 
				   (vector[1]*vector[1]) +
				   (vector[2]*vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if(length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
	}

