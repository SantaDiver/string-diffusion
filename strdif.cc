/*

Made by SNM

*/

// System includes
#include "systemIncludes.h"

// Includes of my files
#include "pieceOfWebFunc.h"
#include "generatingFuncs.h"
#include "loadExchange.h"

using namespace std;

int myRank = 0, comm_size;

bool debug = false;
bool horizontal_diffusion = true;
bool vertical_diffusion = true;

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    try {
        MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

        float c = 1;
        float tau = 0.4;
        float h = 2; // Menas step in x AND y (where delta x = delta y)

        float coef = pow(tau, 2) * pow (c, 2) / pow(h, 2);

        int stepsToCount = atoi(argv[1]);

        int comm_width = atoi(argv[2]);
        int comm_length = comm_size / comm_width;

        if(comm_width*comm_length != comm_size)
            throw "Wrong comm width!";

        vector<int> widthFrags(comm_width, 100);
        vector<int> lengthFrags(comm_length, 100);

        // ------------------ Here may add some changes to frags --------------
        widthFrags[comm_width-1] = 5;
        lengthFrags[comm_length-1] = 5;
        // --------------------------------------------------------------------

        PieceOfWebFunc piece = calcPiece(myRank, comm_width, comm_length,
            widthFrags, lengthFrags, tau, coef, generateZeroes, generateZeroes);

        int fullWidth = 0;
        int fullLength = 0;
        for(int i=0; i<comm_width; ++i) fullWidth += widthFrags[i];
        for(int i=0; i<comm_length; ++i) fullLength += lengthFrags[i];

        vector< vector<float> > asideLeft(stepsToCount, vector<float>(fullLength));
        vector< vector<float> > asideRight(stepsToCount, vector<float>(fullLength));
        vector< vector<float> > asideTop(stepsToCount, vector<float>(fullWidth));
        vector< vector<float> > asideBottom(stepsToCount, vector<float>(fullWidth));
        piece.setAsideVectors(asideLeft, asideRight, asideTop, asideBottom);

        double startTime = MPI_Wtime();

        float diffCoef = calcCoef(myRank, comm_width, comm_length);
        
        int balanceOnceAt = atoi(argv[3]);
        for(int i=0; i<stepsToCount; ++i) {
            piece.calcRes();

            MPI_Barrier(MPI_COMM_WORLD);
            if (i%balanceOnceAt != 0) continue;
            
            int lengthLoad = piece.getLength();
            if (myRank / comm_width != 0 && comm_length > 1)
                sendLoadToTop(piece, myRank, comm_width);
            if (myRank / comm_width != comm_length-1 && comm_length > 1)
                sendLoadToBottom(piece, myRank, comm_width);

            int widthLoad = piece.getWidth();
            if (myRank % comm_width != 0 && comm_width > 1)
                sendLoadToLeft(piece, myRank);
            if (myRank % comm_width != comm_width-1 && comm_width > 1)
                sendLoadToRight(piece, myRank);

            int topLoad;
            if (myRank / comm_width != 0 && comm_length > 1)
                topLoad = getTopLoad(myRank, comm_width);

            int bottomLoad;
            if (myRank / comm_width != comm_length-1 && comm_length > 1)
                bottomLoad = getBottomLoad(myRank, comm_width);

            int leftLoad;
            if (myRank % comm_width != 0 && comm_width > 1)
                leftLoad = getLeftLoad(myRank);

            int rightLoad;
            if (myRank % comm_width != comm_width-1 && comm_width > 1)
                rightLoad = getRightLoad(myRank);


            MPI_Barrier(MPI_COMM_WORLD);

            int valueToExchange;
            if (myRank / comm_width != 0 && comm_length > 1) {
                valueToExchange = diffCoef*abs(lengthLoad-topLoad);
                if (valueToExchange > 0) {
                    if (lengthLoad > topLoad) sendToTop(piece, valueToExchange, myRank, comm_width);
                    else recieveFromTop(piece, valueToExchange, myRank, comm_width);
                }
            }
            if (myRank / comm_width != comm_length-1 && comm_length > 1) {
                valueToExchange = diffCoef*abs(lengthLoad-bottomLoad);
                if (valueToExchange > 0) {
                    if (lengthLoad > bottomLoad) sendToBottom(piece, valueToExchange, myRank, comm_width);
                    else recieveFromBottom(piece, valueToExchange, myRank,comm_width);
                }
            }

            MPI_Barrier(MPI_COMM_WORLD);

            if (myRank % comm_width != 0 && comm_width > 1) {
                int valueToExchange = diffCoef*abs(leftLoad-widthLoad);
                if (valueToExchange > 0) {
                    if (widthLoad > leftLoad) sendToLeft(piece, valueToExchange, myRank);
                    else recieveFromLeft(piece, valueToExchange, myRank);
                }
            }
            if (myRank % comm_width != comm_width-1 && comm_width > 1) {
                int valueToExchange = diffCoef*abs(rightLoad-widthLoad);
                if (valueToExchange > 0) {
                    if (widthLoad > rightLoad) sendToRight(piece, valueToExchange, myRank);
                    else recieveFromRight(piece, valueToExchange, myRank);
                }
            }

            MPI_Barrier(MPI_COMM_WORLD);
        }

        double workTime = MPI_Wtime()-startTime;

        double maxtime;
        MPI_Reduce(&workTime, &maxtime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        if (myRank==0 && !debug) cout << balanceOnceAt << " " << maxtime << endl;

    } catch (char const *error) {
        cerr<<"Error: " << error << " at rank " << myRank << endl;
    }

    MPI_Finalize();
    return 0;
}
