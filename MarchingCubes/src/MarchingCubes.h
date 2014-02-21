#ifndef MARCHINGCUBES_H_INCLUDED
#define MARCHINGCUBES_H_INCLUDED

#include "geometry.h"
#include "mesh.h"
#include "ImplicitSurface.h"

static Vector linearInterpolation(Vector p1, Vector p2, float val1, float val2, float minValue)
{
    Vector p;
    if(val1 != val2)
        p = p1 + (p2 - p1)/(val2 - val1)*(minValue - val1);
    else
        p = p1;

    return p;
}

struct mTriangle {
    Vector p[3];
    Vector norm;
};

class MarchingCubes
{
private:
    double (* const implicitFunction) (const Vector&);
    Vector (* const computeGradient) (const Vector&);
    Mesh* convertTriangles(mTriangle* triangles, int length);

public:
    MarchingCubes(
        double (* const fp) (const Vector&) = &defaultPlane,
        Vector (* const normalFunctionPointer) (const Vector&) = &defaultGetNormal
    ): implicitFunction(fp), computeGradient(normalFunctionPointer) { }

    Mesh* createTriangles(int nCellsX, int nCellsY, int nCellsZ, float minValue, Vector* points, float* values);

    Mesh* cubesIntersect(float minX = -70, float maxX = 100, float minY = -40, float maxY = 100, float minZ = -100, float maxZ = 100,
                        int nCellsX = 60, int nCellsY = 60, int nCellsZ = 60, float minValue = 1e-3);

//    bool intersectTriangle(const Ray& ray, IntersectionData& data, mTriangle& T);

    bool intersect(Ray ray, IntersectionData& data);
    const char* getName()
    {
        return "Marching Cubes";
    }
    bool isInside(const Vector& p) const
    {
        return this->implicitFunction(p) < 0;
    }

};

#endif // MARCHINGCUBES_H_INCLUDED
