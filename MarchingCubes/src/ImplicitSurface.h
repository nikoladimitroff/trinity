#ifndef IMPLICITSURFACE_H
#define IMPLICITSURFACE_H

#include "geometry.h"


static double defaultPlane(const Vector& point)
{
    return point.x * point.x + (2*point.y * point.y) + point.z * point.z - 100;
}

static Vector defaultGetNormal(const Vector& point)
{
    Vector gradient(2 * point.x, 2 * point.y, 2 * point.z);
    gradient.normalize();
    return gradient;
}

class ImplicitSurface : public Geometry
{
    private:
        double (* const implicitFunction) (const Vector&);
        Vector (* const computeGradient) (const Vector&);
    public:
        ImplicitSurface(
                        double (* const fp) (const Vector&) = &defaultPlane,
                        Vector (* const normalFunctionPointer) (const Vector&) = &defaultGetNormal
                        ): implicitFunction(fp), computeGradient(normalFunctionPointer) { }

        void fillProperties(ParsedBlock& pb)
        {

        }

        bool intersect(Ray ray, IntersectionData& data);
        const char* getName() { return "ImplicitSurface"; }
        bool isInside(const Vector& p) const { return this->implicitFunction(p) < 0; }
};

#endif // IMPLICITSURFACE_H
