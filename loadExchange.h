#ifndef LOAD_EXCH
#define LOAD_EXCH

#include "systemIncludes.h"
#include "pieceOfWebFunc.h"
using namespace std;

void sendToTop(PieceOfWebFunc &piece, uint valueToExchange, uint myRank, uint comm_width);
void recieveFromBottom(PieceOfWebFunc &piece, uint valueToExchange, uint myRank, uint comm_width);
void sendToLeft(PieceOfWebFunc &piece, uint valueToExchange, uint myRank);
void recieveFromRight(PieceOfWebFunc &piece, uint valueToExchange, uint myRank);
void sendToBottom(PieceOfWebFunc &piece, uint valueToExchange, uint myRank, uint comm_width);
void recieveFromTop(PieceOfWebFunc &piece, uint valueToExchange, uint myRank, uint comm_width);
void sendToRight(PieceOfWebFunc &piece, uint valueToExchange, uint myRank);
void recieveFromLeft(PieceOfWebFunc &piece, uint valueToExchange, uint myRank);

float calcCoef(uint myRank, uint comm_width, uint comm_length);

PieceOfWebFunc calcPiece(uint myRank, uint comm_width, uint comm_length,
    vector<uint> &widthFrags, vector<uint> &lengthFrags, float tau, float coef,
    float (*gen2D)(uint, uint), float (*genDir)(uint, uint) );
    
uint getTopLoad(uint myRank, uint comm_width);
void sendLoadToTop(PieceOfWebFunc &piece, uint myRank, uint comm_width);
uint getBottomLoad(uint myRank, uint comm_width);
void sendLoadToBottom(PieceOfWebFunc &piece, uint myRank, uint comm_width);
uint getLeftLoad(uint myRank);
void sendLoadToLeft(PieceOfWebFunc &piece, uint myRank);
uint getRightLoad(uint myRank);
void sendLoadToRight(PieceOfWebFunc &piece, uint myRank);

#endif
