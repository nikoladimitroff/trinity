#include "ImplicitSurface.h"
#include "FormulaParser.h"
#include <iostream>

using namespace std;

void ImplicitSurface::fillProperties(ParsedBlock& block)
{
    char formulaText[100];
    block.getStringProp("formula", formulaText);
    string formula = formulaText;

    this->formulaExpression = FormulaParser::GenerateTree(formulaText);

    bool shouldTriangulate;
    if (block.getBoolProp("triangulate", &shouldTriangulate) && shouldTriangulate)
    {
        MarchingCubes cubes(this->formulaExpression);
        block.getIntProp("triangulationQuality", &this->meshQuality);

        this->mesh = cubes.generateMesh(this->meshQuality);
    }

    this->goFast = block.getBoolProp("gofast", &this->goFast) && goFast;
}

bool ImplicitSurface::intersect(Ray ray, IntersectionData& data)
{
    if (this->mesh != nullptr)
        return this->mesh->intersect(ray, data);


    Vector currentPos = ray.start;
    double currentDistance = this->implicitFunction(ray.start);
    double step = 1;

    double tempDistance = this->implicitFunction(ray.start + ray.dir * 1e-3);

    while (abs(currentDistance) > 1e-6)
    {
        currentDistance = tempDistance;
        currentPos += step * ray.dir;
        tempDistance = this->implicitFunction(currentPos);

        if (tempDistance * currentDistance < 0)
        {
            break;
        }
        double diff = fabs(currentDistance) - fabs(tempDistance);
        if (diff <= 0)
        {
            return false;
        }
    }

    // Do binary search

    double maxStep = step;
    double minStep = 0;
    double leftSign = fabs(currentDistance) / currentDistance;
    Vector savedPos = currentPos - step * ray.dir;
    while (fabs(currentDistance) >= 1e-6)
    {
        double mid = minStep + (maxStep - minStep) / 2;
        currentPos = savedPos + mid * ray.dir;
        currentDistance = this->implicitFunction(currentPos);
        if (currentDistance * leftSign > 0)
        {
            minStep = mid;
        }
        else if (currentDistance * leftSign < 0)
        {
            maxStep = mid;
        }
    }

    double distance = (currentPos - ray.start).length();
    if (distance > data.dist)
    {
        return false;
    }

    double epsilon = 1e-6;
    double value = this->implicitFunction(currentPos);
    Vector epsilonVector(
                         this->implicitFunction(currentPos + Vector(epsilon, 0, 0)) - value,
                         this->implicitFunction(currentPos + Vector(0, epsilon, 0)) - value,
                         this->implicitFunction(currentPos + Vector(0, 0, epsilon)) - value);
    epsilonVector.normalize();
    //cout << epsilonVector.x << " " << epsilonVector.y << " " << epsilonVector.z << endl;
    data.p = currentPos;
    data.dist = distance;
    data.normal = epsilonVector;
    data.dNdx = Vector(data.normal.x, 0, 0);
    data.dNdy = Vector(0, 0, data.normal.z);
    data.u = data.p.x;
    data.v = data.p.z;
    data.g = this;

    return true;
}
