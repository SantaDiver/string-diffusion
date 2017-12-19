#include "loadExchange.h"

void sendToTop(PieceOfWebFunc &piece, uint valueToExchange, uint myRank, uint comm_width) {
    MPI_Request request;
    uint width = piece.getWidth();
    
    pair< vector<vector<float> >, vector< vector<float> > > pair = 
        piece.cutPieceFromTop(valueToExchange);
    
    for(uint k=0; k<width; ++k) {
        MPI_Isend (&pair.first[k][0], valueToExchange, MPI_FLOAT,
            myRank - comm_width, k, MPI_COMM_WORLD, &request);
        MPI_Isend (&pair.second[k][0], valueToExchange, MPI_FLOAT,
            myRank - comm_width, k+width, MPI_COMM_WORLD, &request);
    }
}

void recieveFromBottom(PieceOfWebFunc &piece, uint valueToExchange, uint myRank, uint comm_width) {
    MPI_Request request;
    uint width = piece.getWidth();
            
    vector< vector<float> > pieceOfRes(width, vector<float>(valueToExchange));
    for(uint k=0; k<width; ++k) {
        MPI_Recv (&pieceOfRes[k][0], valueToExchange, MPI_FLOAT, myRank + comm_width,
            k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    vector< vector<float> > pieceOfPrev(width, vector<float>(valueToExchange));
    for(uint k=0; k<width; ++k) {
        MPI_Recv (&pieceOfPrev[k][0], valueToExchange, MPI_FLOAT, myRank + comm_width,
            k+width, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    piece.addPieceToBottom(pieceOfRes, pieceOfPrev);
}

void sendToLeft(PieceOfWebFunc &piece, uint valueToExchange, uint myRank) {
    MPI_Request request;
    uint length = piece.getLength();
    
    pair< vector<vector<float> >, vector< vector<float> > > pair = 
        piece.cutPieceFromLeft(valueToExchange);
    
    for(uint k=0; k<valueToExchange; ++k) {
        MPI_Isend (&pair.first[k][0], length, MPI_FLOAT,
            myRank - 1, k, MPI_COMM_WORLD, &request);
        MPI_Isend (&pair.second[k][0], length, MPI_FLOAT,
            myRank - 1, k+valueToExchange, MPI_COMM_WORLD, &request);
    }
}

void recieveFromRight(PieceOfWebFunc &piece, uint valueToExchange, uint myRank) {
    MPI_Request request;
    uint length = piece.getLength();
    
    vector< vector<float> > pieceOfRes(valueToExchange, vector<float>(length));
    for(uint k=0; k<valueToExchange; ++k) {
        MPI_Recv (&pieceOfRes[k][0], length, MPI_FLOAT, myRank + 1,
            k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    vector< vector<float> > pieceOfPrev(valueToExchange, vector<float>(length));
    for(uint k=0; k<valueToExchange; ++k) {
        MPI_Recv (&pieceOfPrev[k][0], length, MPI_FLOAT, myRank + 1,
            k+valueToExchange, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    piece.addPieceToRight(pieceOfRes, pieceOfPrev);
}

void sendToBottom(PieceOfWebFunc &piece, uint valueToExchange, uint myRank, uint comm_width) {
    MPI_Request request;
    uint width = piece.getWidth();
    
    pair< vector<vector<float> >, vector< vector<float> > > pair = 
        piece.cutPieceFromBottom(valueToExchange);
    
    for(uint k=0; k<width; ++k) {
        MPI_Isend (&pair.first[k][0], valueToExchange, MPI_FLOAT,
            myRank + comm_width, k, MPI_COMM_WORLD, &request);
        MPI_Isend (&pair.second[k][0], valueToExchange, MPI_FLOAT,
            myRank + comm_width, k+width, MPI_COMM_WORLD, &request);
    }
}

void recieveFromTop(PieceOfWebFunc &piece, uint valueToExchange, uint myRank, uint comm_width) {
    MPI_Request request;
    uint width = piece.getWidth();
            
    vector< vector<float> > pieceOfRes(width, vector<float>(valueToExchange));
    for(uint k=0; k<width; ++k) {
        MPI_Recv (&pieceOfRes[k][0], valueToExchange, MPI_FLOAT, myRank - comm_width,
            k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    vector< vector<float> > pieceOfPrev(width, vector<float>(valueToExchange));
    for(uint k=0; k<width; ++k) {
        MPI_Recv (&pieceOfPrev[k][0], valueToExchange, MPI_FLOAT, myRank - comm_width,
            k+width, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    piece.addPieceToTop(pieceOfRes, pieceOfPrev);
}

void sendToRight(PieceOfWebFunc &piece, uint valueToExchange, uint myRank) {
    MPI_Request request;
    uint length = piece.getLength();
    
    pair< vector<vector<float> >, vector< vector<float> > > pair = 
        piece.cutPieceFromRight(valueToExchange);
    
    for(uint k=0; k<valueToExchange; ++k) {
        MPI_Isend (&pair.first[k][0], length, MPI_FLOAT,
            myRank + 1, k, MPI_COMM_WORLD, &request);
        MPI_Isend (&pair.second[k][0], length, MPI_FLOAT,
            myRank + 1, k+valueToExchange, MPI_COMM_WORLD, &request);
    }
}

void recieveFromLeft(PieceOfWebFunc &piece, uint valueToExchange, uint myRank) {
    MPI_Request request;
    uint length = piece.getLength();
    
    vector< vector<float> > pieceOfRes(valueToExchange, vector<float>(length));
    for(uint k=0; k<valueToExchange; ++k) {
        MPI_Recv (&pieceOfRes[k][0], length, MPI_FLOAT, myRank - 1,
            k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    vector< vector<float> > pieceOfPrev(valueToExchange, vector<float>(length));
    for(uint k=0; k<valueToExchange; ++k) {
        MPI_Recv (&pieceOfPrev[k][0], length, MPI_FLOAT, myRank - 1,
            k+valueToExchange, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    piece.addPieceToLeft(pieceOfRes, pieceOfPrev);
}

float calcCoef(uint myRank, uint comm_width, uint comm_length) {
    return 1.0/5.0;
}

PieceOfWebFunc calcPiece(uint myRank, uint comm_width, uint comm_length,
    vector<uint> &widthFrags, vector<uint> &lengthFrags, float tau, float coef,
    float (*gen2D)(uint, uint), float (*genDir)(uint, uint) ) {
    
    uint x = myRank % comm_width;
    uint y = myRank / comm_width;
    
    uint pieceWidth = widthFrags[x];
    uint pieceLength = lengthFrags[y];
    
    vector< vector<float> > prev(pieceWidth, vector<float>(pieceLength));
    vector< vector<float> > prev_prev(pieceWidth, vector<float>(pieceLength));
    
    uint width_offset = 0;
    for(uint i=0; i<x; ++i) width_offset += widthFrags[i];
    uint length_offset = 0;
    for(uint i=0; i<y; ++i) length_offset += lengthFrags[i];
    
    for(uint i=0; i<pieceWidth; ++i) {
        for(uint j=0; j<pieceLength; ++j) {
            prev_prev[i][j] = gen2D(i+width_offset, j+length_offset);
            prev[i][j] = genDir(i+width_offset, j+length_offset) * tau + prev_prev[i][j];
        }
    }
    
    return PieceOfWebFunc(coef, prev_prev, prev, myRank, comm_width, comm_length);
}

uint getTopLoad(uint myRank, uint comm_width) {
    uint topLoad;
    MPI_Recv (&topLoad, 1 , MPI_INT, myRank-comm_width, 0, 
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
    return topLoad;
}

void sendLoadToTop(PieceOfWebFunc &piece, uint myRank, uint comm_width) {
    MPI_Request request;
    uint load = piece.getLength();
    MPI_Isend (&load, 1, MPI_INT, myRank-comm_width, 0, MPI_COMM_WORLD, &request);
}

uint getBottomLoad(uint myRank, uint comm_width) {
    uint bottomLoad;
    MPI_Recv (&bottomLoad, 1 , MPI_INT, myRank+comm_width, 0, 
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
    return bottomLoad;
}

void sendLoadToBottom(PieceOfWebFunc &piece, uint myRank, uint comm_width) {
    MPI_Request request;
    uint load = piece.getLength();
    MPI_Isend (&load, 1, MPI_INT, myRank+comm_width, 0, MPI_COMM_WORLD, &request);
}

uint getLeftLoad(uint myRank) {
    uint leftLoad;
    MPI_Recv (&leftLoad, 1 , MPI_INT, myRank-1, 0, 
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
    return leftLoad;
}

void sendLoadToLeft(PieceOfWebFunc &piece, uint myRank) {
    MPI_Request request;
    uint load = piece.getWidth();
    MPI_Isend (&load, 1, MPI_INT, myRank-1, 0, MPI_COMM_WORLD, &request);
}

uint getRightLoad(uint myRank) {
    uint rightLoad;
    MPI_Recv (&rightLoad, 1 , MPI_INT, myRank+1, 0, 
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
    return rightLoad;
}

void sendLoadToRight(PieceOfWebFunc &piece, uint myRank) {
    MPI_Request request;
    uint load = piece.getWidth();
    MPI_Isend (&load, 1, MPI_INT, myRank+1, 0, MPI_COMM_WORLD, &request);
}


