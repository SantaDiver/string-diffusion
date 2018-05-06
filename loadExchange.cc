#include "loadExchange.h"

void sendToTop(PieceOfWebFunc &piece, int valueToExchange, int myRank, int comm_width) {
    MPI_Request request;
    int width = piece.getWidth();

    pair< vector<vector<float> >, vector< vector<float> > > pair =
        piece.cutPieceFromTop(valueToExchange);

    for(int k=0; k<width; ++k) {
        MPI_Isend (&pair.first[k][0], valueToExchange, MPI_FLOAT,
            myRank - comm_width, k, MPI_COMM_WORLD, &request);
        MPI_Isend (&pair.second[k][0], valueToExchange, MPI_FLOAT,
            myRank - comm_width, k+width, MPI_COMM_WORLD, &request);
    }
}

void recieveFromBottom(PieceOfWebFunc &piece, int valueToExchange, int myRank, int comm_width) {
    MPI_Request request;
    int width = piece.getWidth();

    vector< vector<float> > pieceOfRes(width, vector<float>(valueToExchange));
    for(int k=0; k<width; ++k) {
        MPI_Recv (&pieceOfRes[k][0], valueToExchange, MPI_FLOAT, myRank + comm_width,
            k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    vector< vector<float> > pieceOfPrev(width, vector<float>(valueToExchange));
    for(int k=0; k<width; ++k) {
        MPI_Recv (&pieceOfPrev[k][0], valueToExchange, MPI_FLOAT, myRank + comm_width,
            k+width, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    piece.addPieceToBottom(pieceOfRes, pieceOfPrev);
}

void sendToLeft(PieceOfWebFunc &piece, int valueToExchange, int myRank) {
    MPI_Request request;
    int length = piece.getLength();

    pair< vector<vector<float> >, vector< vector<float> > > pair =
        piece.cutPieceFromLeft(valueToExchange);

    for(int k=0; k<valueToExchange; ++k) {
        MPI_Isend (&pair.first[k][0], length, MPI_FLOAT,
            myRank - 1, k, MPI_COMM_WORLD, &request);
        MPI_Isend (&pair.second[k][0], length, MPI_FLOAT,
            myRank - 1, k+valueToExchange, MPI_COMM_WORLD, &request);
    }
}

void recieveFromRight(PieceOfWebFunc &piece, int valueToExchange, int myRank) {
    MPI_Request request;
    int length = piece.getLength();

    vector< vector<float> > pieceOfRes(valueToExchange, vector<float>(length));
    for(int k=0; k<valueToExchange; ++k) {
        MPI_Recv (&pieceOfRes[k][0], length, MPI_FLOAT, myRank + 1,
            k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    vector< vector<float> > pieceOfPrev(valueToExchange, vector<float>(length));
    for(int k=0; k<valueToExchange; ++k) {
        MPI_Recv (&pieceOfPrev[k][0], length, MPI_FLOAT, myRank + 1,
            k+valueToExchange, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    piece.addPieceToRight(pieceOfRes, pieceOfPrev);
}

void sendToBottom(PieceOfWebFunc &piece, int valueToExchange, int myRank, int comm_width) {
    MPI_Request request;
    int width = piece.getWidth();

    pair< vector<vector<float> >, vector< vector<float> > > pair =
        piece.cutPieceFromBottom(valueToExchange);

    for(int k=0; k<width; ++k) {
        MPI_Isend (&pair.first[k][0], valueToExchange, MPI_FLOAT,
            myRank + comm_width, k, MPI_COMM_WORLD, &request);
        MPI_Isend (&pair.second[k][0], valueToExchange, MPI_FLOAT,
            myRank + comm_width, k+width, MPI_COMM_WORLD, &request);
    }
}

void recieveFromTop(PieceOfWebFunc &piece, int valueToExchange, int myRank, int comm_width) {
    MPI_Request request;
    int width = piece.getWidth();

    vector< vector<float> > pieceOfRes(width, vector<float>(valueToExchange));
    for(int k=0; k<width; ++k) {
        MPI_Recv (&pieceOfRes[k][0], valueToExchange, MPI_FLOAT, myRank - comm_width,
            k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    vector< vector<float> > pieceOfPrev(width, vector<float>(valueToExchange));
    for(int k=0; k<width; ++k) {
        MPI_Recv (&pieceOfPrev[k][0], valueToExchange, MPI_FLOAT, myRank - comm_width,
            k+width, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    piece.addPieceToTop(pieceOfRes, pieceOfPrev);
}

void sendToRight(PieceOfWebFunc &piece, int valueToExchange, int myRank) {
    MPI_Request request;
    int length = piece.getLength();

    pair< vector<vector<float> >, vector< vector<float> > > pair =
        piece.cutPieceFromRight(valueToExchange);

    for(int k=0; k<valueToExchange; ++k) {
        MPI_Isend (&pair.first[k][0], length, MPI_FLOAT,
            myRank + 1, k, MPI_COMM_WORLD, &request);
        MPI_Isend (&pair.second[k][0], length, MPI_FLOAT,
            myRank + 1, k+valueToExchange, MPI_COMM_WORLD, &request);
    }
}

void recieveFromLeft(PieceOfWebFunc &piece, int valueToExchange, int myRank) {
    MPI_Request request;
    int length = piece.getLength();

    vector< vector<float> > pieceOfRes(valueToExchange, vector<float>(length));
    for(int k=0; k<valueToExchange; ++k) {
        MPI_Recv (&pieceOfRes[k][0], length, MPI_FLOAT, myRank - 1,
            k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    vector< vector<float> > pieceOfPrev(valueToExchange, vector<float>(length));
    for(int k=0; k<valueToExchange; ++k) {
        MPI_Recv (&pieceOfPrev[k][0], length, MPI_FLOAT, myRank - 1,
            k+valueToExchange, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    piece.addPieceToLeft(pieceOfRes, pieceOfPrev);
}

float calcCoef(int myRank, int comm_width, int comm_length) {
    return 1.0/5.0;
}

PieceOfWebFunc calcPiece(int myRank, int comm_width, int comm_length,
    vector<int> &widthFrags, vector<int> &lengthFrags, float tau, float coef,
    float (*gen2D)(int, int), float (*genDir)(int, int) ) {

    int x = myRank % comm_width;
    int y = myRank / comm_width;

    int pieceWidth = widthFrags[x];
    int pieceLength = lengthFrags[y];

    vector< vector<float> > prev(pieceWidth, vector<float>(pieceLength));
    vector< vector<float> > prev_prev(pieceWidth, vector<float>(pieceLength));

    int width_offset = 0;
    for(int i=0; i<x; ++i) width_offset += widthFrags[i];
    int length_offset = 0;
    for(int i=0; i<y; ++i) length_offset += lengthFrags[i];

    for(int i=0; i<pieceWidth; ++i) {
        for(int j=0; j<pieceLength; ++j) {
            prev_prev[i][j] = gen2D(i+width_offset, j+length_offset);
            prev[i][j] = genDir(i+width_offset, j+length_offset) * tau + prev_prev[i][j];
        }
    }

    return PieceOfWebFunc(coef, prev_prev, prev, myRank, comm_width, comm_length);
}

float getTopLoad(int myRank, int comm_width) {
    float topLoad;
    MPI_Recv (&topLoad, 1 , MPI_FLOAT, myRank-comm_width, 0,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return topLoad;
}

void sendLoadToTop(PieceOfWebFunc &piece, int myRank, int comm_width, float load) {
    MPI_Request request;
    // int load = piece.getLength();
    MPI_Isend (&load, 1, MPI_FLOAT, myRank-comm_width, 0, MPI_COMM_WORLD, &request);
}

float getBottomLoad(int myRank, int comm_width) {
    float bottomLoad;
    MPI_Recv (&bottomLoad, 1 , MPI_FLOAT, myRank+comm_width, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return bottomLoad;
}

void sendLoadToBottom(PieceOfWebFunc &piece, int myRank, int comm_width, float load) {
    MPI_Request request;
    // int load = piece.getLength();
    MPI_Isend (&load, 1, MPI_FLOAT, myRank+comm_width, 0, MPI_COMM_WORLD, &request);
}

float getLeftLoad(int myRank) {
    float leftLoad;
    MPI_Recv (&leftLoad, 1 , MPI_FLOAT, myRank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return leftLoad;
}

void sendLoadToLeft(PieceOfWebFunc &piece, int myRank, float load) {
    MPI_Request request;
    // int load = piece.getWidth();
    MPI_Isend (&load, 1, MPI_FLOAT, myRank-1, 0, MPI_COMM_WORLD, &request);
}

float getRightLoad(int myRank) {
    float rightLoad;
    MPI_Recv (&rightLoad, 1 , MPI_FLOAT, myRank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return rightLoad;
}

void sendLoadToRight(PieceOfWebFunc &piece, int myRank, float load) {
    MPI_Request request;
    // int load = piece.getWidth();
    MPI_Isend (&load, 1, MPI_FLOAT, myRank+1, 0, MPI_COMM_WORLD, &request);
}

double recalc_coef(double coef, double power) {
  return pow(coef, power);
}

double overheat(int rank, int i, int stepsToCount, PieceOfWebFunc &piece, double calcTime, double upTo) {
  double overheatStartTime = MPI_Wtime();
  // rank==4*8+4
  if (rank==0) {
    double startTime=MPI_Wtime();
    double timeToSleep;

    // if (i <= stepsToCount/3){
    //   timeToSleep = upTo*calcTime * 3*i/stepsToCount;
    // } else if (i <= 2*stepsToCount/3) {
    //   timeToSleep = upTo*calcTime;
    // } else {
    //   timeToSleep = upTo*calcTime * 3*(stepsToCount-i)/stepsToCount;
    // }
    //
    timeToSleep=upTo*calcTime;

    while (1)
      if (MPI_Wtime()-startTime > timeToSleep) break;
  }

  return MPI_Wtime() - overheatStartTime;
}
