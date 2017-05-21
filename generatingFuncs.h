#ifndef GEN_FUNCS
#define GEN_FUNCS

#include "systemIncludes.h"
using namespace std;

vector<float> makeHauss(uint nodes, float u, float s);
vector<float> makeParab(uint nodes);
vector<float> makePi(uint nodes, uint offset, uint width, float height);

#endif
