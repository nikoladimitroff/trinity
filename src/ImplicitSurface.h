#ifndef IMPLICITSURFACE_H
#define IMPLICITSURFACE_H

#include <queue>
#include <string>
#include <iostream>


#include "geometry.h"
#include "FormulaParser.h"
#include "Mesh.h"
#include "MarchingCubes.h"

class ImplicitSurface : public Geometry
{
    private:
        bool goFast;
        std::queue<std::string> formulaExpression;
        int meshQuality;
        Mesh* mesh;
    public:
        ImplicitSurface(): mesh(nullptr), meshQuality(60) { }

        ~ImplicitSurface()
        {
            if (this->mesh != nullptr)
                delete this->mesh;
        }

        /** Copy Constructor */
        ImplicitSurface(const ImplicitSurface& other):
            formulaExpression(other.formulaExpression)
        {
            MarchingCubes cubes(this->formulaExpression);
            this->mesh = cubes.generateMesh(this->meshQuality);
        }

        /** Copy Assignment Operator */
        ImplicitSurface& operator= (ImplicitSurface other)
        {
            this->formulaExpression = other.formulaExpression;

            MarchingCubes cubes(this->formulaExpression);
            this->mesh = cubes.generateMesh(this->meshQuality);

            return *this;
        }

        void fillProperties(ParsedBlock& pb);
        double implicitFunction(const Vector& point) const
        {
            if (this->goFast)
                return point.x * point.x +  point.y * point.y + point.z * point.z - 100;

            return FormulaParser::RPNParse(point.x, point.y, point.z, this->formulaExpression);
        }

        bool intersect(Ray ray, IntersectionData& data);
        const char* getName() { return "ImplicitSurface"; }
        bool isInside(const Vector& p) const
        {
            if (this->mesh != nullptr)
                return this->mesh->isInside(p);

            return this->implicitFunction(p) < 0;
        }
};

#endif // IMPLICITSURFACE_H
