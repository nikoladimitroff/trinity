#include "ImplicitSurface.h"
#include <iostream>

using namespace std;

bool ImplicitSurface::intersect(Ray ray, IntersectionData& data)
{
    Vector currentPos = ray.start;
    double currentDistance = this->implicitFunction(ray.start);
    double step = 0.05;

    double tempDistance = this->implicitFunction(ray.start + ray.dir * 1e-3);

    while (abs(currentDistance) > 1e-8)
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
//
//    if (fabs(currentDistance) > 1e-3)
//     {
//        currentPos += -step * ray.dir;
//        Vector savedPosition = currentPos;
//        double maxDistance = max(currentDistance, tempDistance);
//        double minDistance = min(currentDistance, tempDistance);
//        double lambda = 0.5;
//        while (fabs(currentDistance) > 1e-3)
//        {
//            double newStep = step * lambda;
//            double midDistance = minDistance + fabs(maxDistance - minDistance) / 2;
//            currentPos = savedPosition + newStep * ray.dir;
//            tempDistance = this->implicitFunction(currentPos);
//            if (tempDistance > 0)
//            {
//                maxDistance = midDistance;
//                lambda -= lambda / 2;
//            }
//            else if (tempDistance < 0)
//            {
//                minDistance = midDistance;
//                lambda += lambda / 2;
//            }
//            currentDistance = tempDistance;
//        }
//     }

    double distance = (currentPos - ray.start).length();
    if (distance > data.dist)
    {
        return false;
    }
    cout << currentPos.x << " " << currentPos.y << " " << currentPos.z << endl;
    data.p = currentPos;
    data.dist = distance;
    data.normal = this->getNormal(currentPos);
    data.dNdx = Vector(data.normal.x, 0, 0);
    data.dNdy = Vector(0, 0, data.normal.z);
    data.u = data.p.x;
    data.v = data.p.z;
    data.g = this;

    return true;
}
