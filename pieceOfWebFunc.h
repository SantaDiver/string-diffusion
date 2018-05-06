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
  int _width; // Means horizontal
  int _length; // Means vertical

  int _rank;
  int _comm_width; // Means horizontal
  int _comm_length; // Means vertical

  int _stepsCounter;
public:
    PieceOfWebFunc(
        float coef, vector< vector<float> > &t0, vector< vector<float> > &t1,
        int rank, int comm_width, int comm_length
    );

    double calcRes();

    void setAsideVectors(vector<vector< float> > &asideLeft,
        vector< vector<float> > &asideRight, vector< vector<float> > &asideTop,
        vector< vector<float> > &asideBottom);

    pair< vector<vector<float> >, vector< vector<float> > > cutPieceFromLeft(int l);
    pair< vector<vector<float> >, vector< vector<float> > > cutPieceFromRight(int l);
    pair< vector<vector<float> >, vector< vector<float> > > cutPieceFromTop(int l);
    pair< vector<vector<float> >, vector< vector<float> > > cutPieceFromBottom(int l);

    void addPieceToLeft(vector< vector<float> >pieceOfRes, vector< vector<float> > pieceOfPrev);
    void addPieceToRight(vector< vector<float> > pieceOfRes, vector< vector<float> > pieceOfPrev);
    void addPieceToTop(vector< vector<float> > pieceOfRes, vector< vector<float> > pieceOfPrev);
    void addPieceToBottom(vector< vector<float> > pieceOfRes, vector< vector<float> > pieceOfPrev);

    vector< vector<float> > getRes();
    int getWidth(); // Means horizontal
    int getLength(); // Means vertical

};

#endif
