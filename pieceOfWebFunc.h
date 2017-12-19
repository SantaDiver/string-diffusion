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
  vector< vector<float> > _res;
  vector< vector<float> > _prev;
  vector< vector<float> > _prev_prev;

  // aside vectors (will be empty, if needed)
  vector< vector<float> > _asideLeft;
  vector< vector<float> > _asideRight;
  vector< vector<float> > _asideTop;
  vector< vector<float> > _asideBottom;

  // Length of piece
  uint _width; // Means horizontal
  uint _length; // Means vertical

  uint _rank;
  uint _comm_width; // Means horizontal
  uint _comm_length; // Means vertical

  uint _stepsCounter;
public:
    PieceOfWebFunc(
        float coef, vector< vector<float> > &t0, vector< vector<float> > &t1,
        uint rank, uint comm_width, uint comm_length
    );

    void calcRes();

    void setAsideVectors(vector<vector< float> > &asideLeft, 
        vector< vector<float> > &asideRight, vector< vector<float> > &asideTop,
        vector< vector<float> > &asideBottom);

    pair< vector<vector<float> >, vector< vector<float> > > cutPieceFromLeft(uint l);
    pair< vector<vector<float> >, vector< vector<float> > > cutPieceFromRight(uint l);
    pair< vector<vector<float> >, vector< vector<float> > > cutPieceFromTop(uint l);
    pair< vector<vector<float> >, vector< vector<float> > > cutPieceFromBottom(uint l);

    void addPieceToLeft(vector< vector<float> >pieceOfRes, vector< vector<float> > pieceOfPrev);
    void addPieceToRight(vector< vector<float> > pieceOfRes, vector< vector<float> > pieceOfPrev);
    void addPieceToTop(vector< vector<float> > pieceOfRes, vector< vector<float> > pieceOfPrev);
    void addPieceToBottom(vector< vector<float> > pieceOfRes, vector< vector<float> > pieceOfPrev);

    vector< vector<float> > getRes();
    uint getWidth(); // Means horizontal
    uint getLength(); // Means vertical

};

#endif
