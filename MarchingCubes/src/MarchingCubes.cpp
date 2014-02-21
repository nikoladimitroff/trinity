#include "MarchingCubes.h"
#include "lookUpTables.h"
#include <iostream>

using namespace std;

Mesh* MarchingCubes::cubesIntersect(float minX, float maxX, float minY, float maxY, float minZ, float maxZ,
        int nCellsX, int nCellsY, int nCellsZ, float minValue)
{
    //Here we need to calculate the space that will be devided
    //because now it is explicitly typed
    float * values = new float[(nCellsX+1)*(nCellsY+1)*(nCellsZ+1)];
    Vector * cubesDataPoints = new Vector[(nCellsX+1)*(nCellsY+1)*(nCellsZ+1)];
    Vector stepSize((maxX-minX)/nCellsX, (maxY-minY)/nCellsY, (maxZ-minZ)/nCellsZ);

    int yTimesZ = (nCellsY+1)*(nCellsZ+1);         //Optimisation
    for(int i = 0; i<nCellsX+1; i++)
    {
        int ni = i*yTimesZ;             //Little optimisation
        float vertX = minX + i*stepSize.x;
        for(int j = 0; j<nCellsY+1; j++)
        {
            int nj = j*(nCellsZ+1);
            float vertY = minY + j*stepSize.y;
            for(int k=0; k<nCellsZ+1; k++)
            {
                Vector vert(vertX, vertY, minZ + k * stepSize.z);
                cubesDataPoints[ni + nj + k] = vert;
                values[ni + nj + k] = this->implicitFunction(vert);
            }
        }
    }

    return createTriangles(nCellsX, nCellsY, nCellsZ, minValue, cubesDataPoints, values);
}

Mesh* MarchingCubes::createTriangles(int nCellsX, int nCellsY, int nCellsZ, float minValue, Vector* points, float* values)
{
    mTriangle* triangles = new mTriangle[3 * nCellsX*nCellsY*nCellsZ];
    int numTriangles = 0;
    int yTimeZ = (nCellsY+1)*(nCellsZ+1);
    int ni,nj;

    //Go through all points
    for(int i = 0; i < nCellsX; i++)
    {
        ni = i*yTimeZ;
        for(int j =0; j<nCellsY; j++)
        {
            nj = j*(nCellsZ+1);
            for(int k = 0; k<nCellsZ; k++)
            {
                Vector verts[8];
                float vals[8];
                int ind = ni+nj+k;

                //Populate all vertexes of the current cube and their values
                verts[0] = points[ind];
                vals[0] = values[ind];
                verts[1] = points[ind + yTimeZ];
                vals[1] = values[ind + yTimeZ];
                verts[2] = points[ind + yTimeZ + 1];
                vals[2] = values[ind + yTimeZ + 1];
                verts[3] = points[ind + 1];
                vals[3] = values[ind + 1];
                verts[4] = points[ind + (nCellsZ+1)];
                vals[4] = values[ind + (nCellsZ+1)];
                verts[5] = points[ind + yTimeZ + (nCellsZ+1)];
                vals[5] = values[ind + yTimeZ + (nCellsZ+1)];
                verts[6] = points[ind + yTimeZ + (nCellsZ+1) + 1];
                vals[6] = values[ind + yTimeZ + (nCellsZ+1) + 1];
                verts[7] = points[ind + (nCellsZ+1) + 1];
                vals[7] = values[ind + (nCellsZ+1) + 1];

                int cubeIndex = 0;
                //Calculates cubeIndex
                for(int n=0; n <8; n++)
                    if(vals[n] <= minValue) cubeIndex |= (1 << n);

                //If cube is completely outside or inside
                if(!edgeTable[cubeIndex]) continue;

                //Interpolate between vertices on edges
                Vector intVect[12];

                if(edgeTable[cubeIndex] & 1) intVect[0] = linearInterpolation(verts[0], verts[1],vals[0],vals[1],minValue);
                if(edgeTable[cubeIndex] & 2) intVect[1] = linearInterpolation(verts[1], verts[2],vals[1],vals[2],minValue);
                if(edgeTable[cubeIndex] & 4) intVect[2] = linearInterpolation(verts[2], verts[3],vals[2],vals[3],minValue);
                if(edgeTable[cubeIndex] & 8) intVect[3] = linearInterpolation(verts[3], verts[0],vals[3],vals[0],minValue);
                if(edgeTable[cubeIndex] & 16) intVect[4] = linearInterpolation(verts[4], verts[5],vals[4],vals[5],minValue);
                if(edgeTable[cubeIndex] & 32) intVect[5] = linearInterpolation(verts[5], verts[6],vals[5],vals[6],minValue);
                if(edgeTable[cubeIndex] & 64) intVect[6] = linearInterpolation(verts[6], verts[7],vals[6],vals[7],minValue);
                if(edgeTable[cubeIndex] & 128) intVect[7] = linearInterpolation(verts[7], verts[4],vals[7],vals[4],minValue);
                if(edgeTable[cubeIndex] & 256) intVect[8] = linearInterpolation(verts[0], verts[4],vals[0],vals[4],minValue);
                if(edgeTable[cubeIndex] & 512) intVect[9] = linearInterpolation(verts[1], verts[5],vals[1],vals[5],minValue);
                if(edgeTable[cubeIndex] & 1024) intVect[10] = linearInterpolation(verts[2], verts[6],vals[2],vals[6],minValue);
                if(edgeTable[cubeIndex] & 2048) intVect[11] = linearInterpolation(verts[3], verts[7],vals[3],vals[7],minValue);

                //Build the triangles
                for(int n=0; triTable[cubeIndex][n]!=-1; n+=3)
                {
                    triangles[numTriangles].p[0] = intVect[triTable[cubeIndex][n+2]];
                    triangles[numTriangles].p[1] = intVect[triTable[cubeIndex][n+1]];
                    triangles[numTriangles].p[2] = intVect[triTable[cubeIndex][n]];
                    //Computre cross product of the edges to get the normal
                    triangles[numTriangles].norm = normalize(cross(triangles[numTriangles].p[1] - triangles[numTriangles].p[0], triangles[numTriangles].p[2] -
                                                   triangles[numTriangles].p[0]));
                    numTriangles++;
                }
            }
        }
    }

    mTriangle* resultTriangles = new mTriangle[numTriangles];
    for(int i=0; i<numTriangles; i++)
        resultTriangles[i] = triangles[i];

    delete[] triangles;

    return convertTriangles(resultTriangles, numTriangles);

}

Mesh* MarchingCubes::convertTriangles(mTriangle* triangles, int length)
{
    Mesh* mesh = new Mesh();
    vector<Vector> vertices;
    vector<Vector> normals;
    vector<Triangle> tTriangles;

    for(int i=0; i < length; i++)
    {
        vertices.push_back(triangles[i].p[0]);
        vertices.push_back(triangles[i].p[1]);
        vertices.push_back(triangles[i].p[2]);

        normals.push_back(triangles[i].norm);

        Triangle temp;
        temp.v[0] = vertices.size()-3;
        temp.v[1] = vertices.size()-2;
        temp.v[2] = vertices.size()-1;
//        std::cout<<temp.v[0]<<endl;
        temp.n[0] = temp.n[1] = temp.n[2] = normals.size()-1;

        temp.t[0] = temp.t[1] = temp.t[2] = 0;

        temp.gnormal = triangles[i].norm;

        tTriangles.push_back(temp);
    }
//    std::cout<<tTriangles.size()<<endl;

    mesh->fillData(vertices, normals, tTriangles);

    return mesh;
}

//Index are hacked - many vertices more than once
//Only geometric normals
//Scene.parse is changed
//Changed Color and Lighs modifiers
//Mesh.initMesh made public
//Mesh.kdTree made true by default


