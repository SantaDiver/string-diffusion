#ifndef PIECE_OF_WEB_FUNC
#define PIECE_OF_WEB_FUNC

#include "systemIncludes.h"

using namespace std;

class PieceOfWebFunc
{
private:
  // Coefs of string equation
  float _coef;

  // Net of results
  vector<float> _res;
  vector<float> _prev;
  vector<float> _prev_prev;

  // aside vectors (will be empty, if needed)
  vector<float> _asideLeft;
  vector<float> _asideRight;

  // Length of piece
  uint _length;

  uint _rank;
  uint _comm_size;

  uint _stepsCounter;
public:
    PieceOfWebFunc(
        float coef, vector<float> &prev_prev, vector<float> &prev,
        uint rank, uint comm_size
    );

    void calcRes();

    void setAsideLeftVector(vector<float> &asideLeft);
    void setAsideRightVector(vector<float> &asideRight);

    pair< vector<float>, vector<float> > cutPieceFromLeft(uint l);
    pair< vector<float>, vector<float> > cutPieceFromRight(uint l);

    void addPieceToLeft(vector<float> pieceOfRes, vector<float> pieceOfPrev);
    void addPieceToRight(vector<float> pieceOfRes, vector<float> pieceOfPrev);

    vector<float> getRes();
    uint getLength();

};

#endif
