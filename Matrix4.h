//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  Matrix4.h
//  ------------------------
//  
//  A minimal class for a homogeneous 4x4 matrix
//  
///////////////////////////////////////////////////

// include guard
#ifndef MATRIX4_H
#define MATRIX4_H

#include <iostream>
#include "Cartesian3.h"
#include "Homogeneous4.h"

#define DEG2RAD(x) (M_PI*(float)(x)/180.0)

// forward declaration
class Matrix4;

// this allows us to get a matrix in the 
// column-major form preferred by OpenGL
class columnMajorMatrix
    { // class columnMajorMatrix
    public:
    float coordinates[16];
    columnMajorMatrix()
    {
        for(int row = 0; row < 4; row++)
        {
            for(int col = 0; col < 4; col++)
            {
                coordinates[col * 4 + row] = 0.0f;
            }
        }
    }
    Homogeneous4 operator*(const Homogeneous4& v) const;
    columnMajorMatrix operator*(const columnMajorMatrix& other) const;

    static columnMajorMatrix Identity()
    {
        columnMajorMatrix ret;

        for(int i = 0; i < 16; i++)
        {
            if(i % 5 == 0)
            {
                ret.coordinates[i] = 1.0f;
            } else 
            {
                ret.coordinates[i] = 0.0f;
            }
        }
    }

    static columnMajorMatrix Translate(const Cartesian3& vector)
    {
        columnMajorMatrix ret;

        ret.coordinates[0] = 1.0f;
        ret.coordinates[1] = 0.0f;
        ret.coordinates[2] = 0.0f;
        ret.coordinates[3] = 0.0f;

        ret.coordinates[4] = 0.0f;
        ret.coordinates[5] = 1.0f;
        ret.coordinates[6] = 0.0f;
        ret.coordinates[7] = 0.0f;

        ret.coordinates[8] = 0.0f;
        ret.coordinates[9] = 0.0f;
        ret.coordinates[10] = 1.0f;
        ret.coordinates[11] = 0.0f;

        ret.coordinates[12] = vector.x;
        ret.coordinates[13] = vector.y;
        ret.coordinates[14] = vector.z;
        ret.coordinates[15] = 1.0f;

        return ret;
    }

    static columnMajorMatrix Scale(const Cartesian3& scale)
    {
        columnMajorMatrix ret;

        ret.coordinates[0] = scale.x;
        ret.coordinates[1] = 0.0f;
        ret.coordinates[2] = 0.0f;
        ret.coordinates[3] = 0.0f;

        ret.coordinates[4] = 0.0f;
        ret.coordinates[5] = scale.y;
        ret.coordinates[6] = 0.0f;
        ret.coordinates[7] = 0.0f;

        ret.coordinates[8] = 0.0f;
        ret.coordinates[9] = 0.0f;
        ret.coordinates[10] = scale.z;
        ret.coordinates[11] = 0.0f;

        ret.coordinates[12] = 0.0f;
        ret.coordinates[13] = 0.0f;
        ret.coordinates[14] = 0.0f;
        ret.coordinates[15] = 1.0f;

        return ret;
    }

    static columnMajorMatrix RotateX(float degrees)
    {
        float toRadians = DEG2RAD(degrees);

        columnMajorMatrix rotationMatrix = Identity();
        rotationMatrix.coordinates[0] = 1.0f;
        rotationMatrix.coordinates[1] = 0.0f;
        rotationMatrix.coordinates[2] = 0.0f;
        rotationMatrix.coordinates[3] = 0.0f;

        rotationMatrix.coordinates[4] = 0.0f;
        rotationMatrix.coordinates[5] = std::cos(toRadians);
        rotationMatrix.coordinates[6] = std::sin(toRadians);
        rotationMatrix.coordinates[7] = 0.0f;

        rotationMatrix.coordinates[8] = 0.0f;
        rotationMatrix.coordinates[9] = -std::sin(toRadians);
        rotationMatrix.coordinates[10] = std::cos(toRadians);
        rotationMatrix.coordinates[11] = 0.0f;

        rotationMatrix.coordinates[12] = 0.0f;
        rotationMatrix.coordinates[13] = 0.0f;
        rotationMatrix.coordinates[14] = 0.0f;
        rotationMatrix.coordinates[15] = 1.0f;

        return rotationMatrix;
    }

    static columnMajorMatrix RotateY(float degrees)
    {
        float toRadians = DEG2RAD(degrees);

        columnMajorMatrix rotationMatrix;
        rotationMatrix.coordinates[0] = std::cos(toRadians);
        rotationMatrix.coordinates[1] = 0.0f;
        rotationMatrix.coordinates[2] = -std::sin(toRadians);
        rotationMatrix.coordinates[3] = 0.0f;

        rotationMatrix.coordinates[4] = 0.0f;
        rotationMatrix.coordinates[5] = 1.0f;
        rotationMatrix.coordinates[6] = 0.0f;
        rotationMatrix.coordinates[7] = 0.0f;

        rotationMatrix.coordinates[8] = std::sin(toRadians);
        rotationMatrix.coordinates[9] = 0.0f;
        rotationMatrix.coordinates[10] = -std::cos(toRadians);
        rotationMatrix.coordinates[11] = 0.0f;

        rotationMatrix.coordinates[12] = 0.0f;
        rotationMatrix.coordinates[13] = 0.0f;
        rotationMatrix.coordinates[14] = 0.0f;
        rotationMatrix.coordinates[15] = 1.0f;

        return rotationMatrix;
    }

    static columnMajorMatrix RotateZ(float degrees)
    {
        float toRadians = DEG2RAD(degrees);

        columnMajorMatrix rotationMatrix;
        rotationMatrix.coordinates[0] = std::cos(toRadians);
        rotationMatrix.coordinates[1] = std::sin(toRadians);
        rotationMatrix.coordinates[2] = 0.0f;
        rotationMatrix.coordinates[3] = 0.0f;

        rotationMatrix.coordinates[4] = -std::sin(toRadians);
        rotationMatrix.coordinates[5] = std::cos(toRadians);
        rotationMatrix.coordinates[6] = 0.0f;
        rotationMatrix.coordinates[7] = 0.0f;

        rotationMatrix.coordinates[8] = 0.0f;
        rotationMatrix.coordinates[9] = 0.0f;
        rotationMatrix.coordinates[10] = 1.0f;
        rotationMatrix.coordinates[11] = 0.0f;

        rotationMatrix.coordinates[12] = 0.0f;
        rotationMatrix.coordinates[13] = 0.0f;
        rotationMatrix.coordinates[14] = 0.0f;
        rotationMatrix.coordinates[15] = 1.0f;

        return rotationMatrix;
    }

    static columnMajorMatrix constructView(const Cartesian3& camerpos, const Cartesian3& target, const Cartesian3& up)
    {	
        Cartesian3 forward, Up, right;
        Cartesian3 x, y, z;

        forward = camerpos - target;
        forward.unit();

        right = up.cross(forward);
        right.unit();

        Up = forward.cross(right);
        Up.unit();

        z = forward;
        y = Up;
        x = right;

        // Set up rotation as a row major matrix
        columnMajorMatrix rotation;
        // x
        rotation.coordinates[0] = x.x;
        rotation.coordinates[1] = y.x;
        rotation.coordinates[2] = z.x;
        rotation.coordinates[3] = 0.0f;

        // y
        rotation.coordinates[4] = x.y;
        rotation.coordinates[5] = y.y;
        rotation.coordinates[6] = z.y;
        rotation.coordinates[7] = 0.0f;

        // z
        rotation.coordinates[8] = x.z;
        rotation.coordinates[9] = y.z;
        rotation.coordinates[10] = z.z;
        rotation.coordinates[11] = 0.0f;

        rotation.coordinates[12] = 0.0f;
        rotation.coordinates[13] = 0.0f;
        rotation.coordinates[14] = 0.0f;
        rotation.coordinates[15] = 1.0f;

        columnMajorMatrix translation;
        translation.coordinates[0] = 1.0f;
        translation.coordinates[1] = 0.0f;
        translation.coordinates[2] = 0.0f;
        translation.coordinates[3] = 0.0f;

        translation.coordinates[4] = 0.0f;
        translation.coordinates[5] = 1.0f;
        translation.coordinates[6] = 0.0f;
        translation.coordinates[7] = 0.0f;

        translation.coordinates[8] = 0.0f;
        translation.coordinates[9] = 0.0f;
        translation.coordinates[10] = 1.0f;
        translation.coordinates[11] = 0.0f;

        translation.coordinates[12] = -camerpos.x;
        translation.coordinates[13] = -camerpos.y;
        translation.coordinates[14] = -camerpos.z;
        translation.coordinates[15] = 1.0f;
        
        return rotation * translation;
    }

    static columnMajorMatrix Look(const Cartesian3& camerpos, const Cartesian3& target, const Cartesian3& up)
    {	
        Cartesian3 forward, Up, right;
        Cartesian3 x, y, z;

        forward = target - camerpos;
        forward.unit();

        right = up.cross(forward);
        right.unit();

        Up = forward.cross(right);
        Up.unit();

        z = forward;
        y = Up;
        x = right;

        // Set up rotation as a row major matrix
        columnMajorMatrix rotation;
        // x
        rotation.coordinates[0] = x.x;
        rotation.coordinates[1] = x.y;
        rotation.coordinates[2] = x.z;
        rotation.coordinates[3] = 0.0f;

        // y
        rotation.coordinates[4] = y.x;
        rotation.coordinates[5] = y.y;
        rotation.coordinates[6] = y.z;
        rotation.coordinates[7] = 0.0f;

        // z
        rotation.coordinates[8] = z.x;
        rotation.coordinates[9] = z.y;
        rotation.coordinates[10] = z.z;
        rotation.coordinates[11] = 0.0f;

        rotation.coordinates[12] = 0.0f;
        rotation.coordinates[13] = 0.0f;
        rotation.coordinates[14] = 0.0f;
        rotation.coordinates[15] = 1.0f;
        
        return rotation;
    }
    
}; // class columnMajorMatrix
 
// the class itself, stored in row-major form
class Matrix4
    { // Matrix4
    public:
    // the coordinates
    float coordinates[4][4];

    // constructor - default to the zero matrix
    Matrix4();
    
    // equality operator
    bool operator ==(const Matrix4 &other) const;

    // indexing - retrieves the beginning of a line
    // array indexing will then retrieve an element
    float * operator [](const int rowIndex);
    
    // similar routine for const pointers
    const float * operator [](const int rowIndex) const;

    // scalar operations
    // multiplication operator (no division operator)
    Matrix4 operator *(float factor) const;

    // vector operations on homogeneous coordinates
    // multiplication is the only operator we use
    Homogeneous4 operator *(const Homogeneous4 &vector) const;

    // and on Cartesian coordinates
    Cartesian3 operator *(const Cartesian3 &vector) const;

    // matrix operations
    // addition operator
    Matrix4 operator +(const Matrix4 &other) const;
    // subtraction operator
    Matrix4 operator -(const Matrix4 &other) const;
    // multiplication operator
    Matrix4 operator *(const Matrix4 &other) const; 
    
    // matrix transpose
    Matrix4 transpose() const;
    
    // returns a column-major array of 16 values
    // for use with OpenGL
    columnMajorMatrix columnMajor() const;

	// routine that returns a row vector as a Homogeneous4
	Homogeneous4 row(int rowNum);
	
	// and similar for a column
	Homogeneous4 column(int colNum);

    // methods that return particular matrices
    static Matrix4 Zero();

    // the identity matrix
    static Matrix4 Identity();
    static Matrix4 Translate(const Cartesian3 &vector);
    static Matrix4 Scale(const Cartesian3& vector);

    // rotations around main axes
	static Matrix4 RotateX(float degrees);
    static Matrix4 RotateY(float degrees);
    static Matrix4 RotateZ(float degrees);
    }; // Matrix4

// scalar operations
// additional scalar multiplication operator
Matrix4 operator *(float factor, const Matrix4 &matrix);

// stream input
std::istream & operator >> (std::istream &inStream, Matrix4 &value);

// stream output
std::ostream & operator << (std::ostream &outStream, const Matrix4 &value);
        
#endif
