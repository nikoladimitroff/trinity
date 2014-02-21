#ifndef IMPLICITSURFACE_H
#define IMPLICITSURFACE_H

#include <queue>
#include <string>
#include <iostream>


#include "geometry.h"
#include "FormulaParser.h"

class ImplicitSurface : public Geometry
{
    private:
        std::queue<std::string> formulaExpression;
    public:
        ImplicitSurface() { }

        void fillProperties(ParsedBlock& pb);
        double implicitFunction(const Vector& point) const
        {
           return FormulaParser::RPNParse(point.x, point.y, point.z, this->formulaExpression);
        }

        bool intersect(Ray ray, IntersectionData& data);
        const char* getName() { return "ImplicitSurface"; }
        bool isInside(const Vector& p) const { return this->implicitFunction(p) < 0; }
};

#endif // IMPLICITSURFACE_H
