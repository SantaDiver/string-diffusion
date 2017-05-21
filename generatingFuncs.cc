#include "generatingFuncs.h"

vector<float> makeHauss(uint nodes, float u, float s)
{
    //cerr << "LOOK: " << u << " " << s << endl;

    float step = 10.0 / (3*nodes);

    float x=-5;
    vector<float> result(nodes*3);
    for (uint i=0; i<result.size(); ++i)
    {
        if (i < nodes) result[i] = 0;
        else if (i > 2*nodes) result[i] = 0;
        else result[i] = exp(-pow(x-u, 2)/2*pow(s, 2)) / (s* pow(2*M_PI, 0.5));
        x += step;
    }

    return result;
}

vector<float> makeParab(uint nodes)
{
    int mid = nodes/2;
    int height = mid*mid;

    vector<float> result(nodes);
    for (int i=-mid; i<mid; ++i)
    {
        result[i+mid] = (-pow(i, 2) + height)/1000;
    }

    // for (int i=0; i<1000; ++i ) result.push_back(0);

    // for (int i=-mid; i<mid; ++i)
    // {
    //     result.push_back((-pow(i, 2) + height)/1000);
    // }

    // for (int i=0; i<500; ++i ) result.push_back(0);

    return result;
}

vector<float> makePi(uint nodes, uint offset, uint width, float height)
{
    if (offset+width > nodes) throw "Too big offset!\n";

    vector<float> result;
    for (uint i=0; i<nodes; ++i)
    {
        if (i < offset || i > offset+width)
            result.push_back(0);
        else
            result.push_back(height);
    }

    return result;
}
