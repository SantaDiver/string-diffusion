#ifndef LOAD_EXCH
#define LOAD_EXCH

#include "systemIncludes.h"
#include "pieceOfWebFunc.h"
using namespace std;

void sendToTop(PieceOfWebFunc &piece, int valueToExchange, int myRank, int comm_width);
void recieveFromBottom(PieceOfWebFunc &piece, int valueToExchange, int myRank, int comm_width);
void sendToLeft(PieceOfWebFunc &piece, int valueToExchange, int myRank);
void recieveFromRight(PieceOfWebFunc &piece, int valueToExchange, int myRank);
void sendToBottom(PieceOfWebFunc &piece, int valueToExchange, int myRank, int comm_width);
void recieveFromTop(PieceOfWebFunc &piece, int valueToExchange, int myRank, int comm_width);
void sendToRight(PieceOfWebFunc &piece, int valueToExchange, int myRank);
void recieveFromLeft(PieceOfWebFunc &piece, int valueToExchange, int myRank);

float calcCoef(int myRank, int comm_width, int comm_length);

PieceOfWebFunc calcPiece(int myRank, int comm_width, int comm_length,
    vector<int> &widthFrags, vector<int> &lengthFrags, float tau, float coef,
    float (*gen2D)(int, int), float (*genDir)(int, int) );
    
int getTopLoad(int myRank, int comm_width);
void sendLoadToTop(PieceOfWebFunc &piece, int myRank, int comm_width);
int getBottomLoad(int myRank, int comm_width);
void sendLoadToBottom(PieceOfWebFunc &piece, int myRank, int comm_width);
int getLeftLoad(int myRank);
void sendLoadToLeft(PieceOfWebFunc &piece, int myRank);
int getRightLoad(int myRank);
void sendLoadToRight(PieceOfWebFunc &piece, int myRank);

#endif
