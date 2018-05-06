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

int main(int argc, char** argv)
{
    if (argc<6) throw invalid_argument("too few parameters");

    int myRank = 0, comm_size;

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

        vector<int> widthFrags(comm_width, 30);
        vector<int> lengthFrags(comm_length, 30);

        // ------------------ Here may add some changes to frags --------------
        // widthFrags[comm_width-1] = 500;
        // lengthFrags[comm_length-1] = 500;
        // --------------------------------------------------------------------

        PieceOfWebFunc piece = calcPiece(myRank, comm_width, comm_length,
            widthFrags, lengthFrags, tau, coef, generateZeroes, generateXY);

        int fullWidth = 0;
        int fullLength = 0;
        for(int i=0; i<comm_width; ++i) fullWidth += widthFrags[i];
        for(int i=0; i<comm_length; ++i) fullLength += lengthFrags[i];

        vector< vector<float> > asideLeft(stepsToCount, vector<float>(fullLength));
        vector< vector<float> > asideRight(stepsToCount, vector<float>(fullLength));
        vector< vector<float> > asideTop(stepsToCount, vector<float>(fullWidth));
        vector< vector<float> > asideBottom(stepsToCount, vector<float>(fullWidth));
        piece.setAsideVectors(asideLeft, asideRight, asideTop, asideBottom);

        int color = myRank / comm_width;
        MPI_Comm row_comm;
        MPI_Comm_split(MPI_COMM_WORLD, color, myRank, &row_comm);

        color = myRank % comm_width;
        MPI_Comm col_comm;
        MPI_Comm_split(MPI_COMM_WORLD, color, myRank, &col_comm);

        double startTime = MPI_Wtime();
        float diffCoef = calcCoef(myRank, comm_width, comm_length);
        int balanceOnceAt = atoi(argv[3]);
        bool balanceUsingTime = true;
        double upTo = atof(argv[4]);
        double diffPower = atof(argv[5]);

        double lastBalanceTime = MPI_Wtime();
        double calcTime=0;
        double s=0;

        for(int i=1; i<=stepsToCount; ++i) {
            s = piece.calcRes();
            calcTime += s;
            calcTime += overheat(myRank, i, stepsToCount, piece, s, upTo);

            if (i%balanceOnceAt != 0) continue;
            // calcTime = MPI_Wtime()-lastBalanceTime;

            // for (int jj=0; jj<comm_size; ++jj) {
            //   if (myRank==jj) cerr << piece.getLength() << " " << piece.getWidth() << "\t";
            //   if (myRank==jj && jj%comm_width==1) cerr << "\n";
            //   MPI_Barrier(MPI_COMM_WORLD);
            // }
            // if (myRank==0) cerr << "\n";

            float lengthLoad = piece.getLength();
            float widthLoad = piece.getWidth();
            if (balanceUsingTime) {
              double rowTime;
              MPI_Reduce(&calcTime, &rowTime, 1, MPI_DOUBLE, MPI_MAX, 0, row_comm);
              MPI_Bcast(&rowTime, 1, MPI_DOUBLE, 0, row_comm);
              lengthLoad = rowTime;

              double colTime;
              MPI_Reduce(&calcTime, &colTime, 1, MPI_DOUBLE, MPI_MAX, 0, col_comm);
              MPI_Bcast(&colTime, 1, MPI_DOUBLE, 0, col_comm);
              widthLoad = colTime;

              // for (int jj=0;jj<comm_size;++jj) {
              //   if (myRank==jj) cerr << jj <<"  " << calcTime << " " << rowTime << " " << colTime << "\n";
              //   MPI_Barrier(MPI_COMM_WORLD);
              // }
            }

            if (myRank / comm_width != 0 && comm_length > 1)
                sendLoadToTop(piece, myRank, comm_width, lengthLoad);
            if (myRank / comm_width != comm_length-1 && comm_length > 1)
                sendLoadToBottom(piece, myRank, comm_width, lengthLoad);

            if (myRank % comm_width != 0 && comm_width > 1)
                sendLoadToLeft(piece, myRank, widthLoad);
            if (myRank % comm_width != comm_width-1 && comm_width > 1)
                sendLoadToRight(piece, myRank, widthLoad);

            float topLoad;
            if (myRank / comm_width != 0 && comm_length > 1)
                topLoad = getTopLoad(myRank, comm_width);

            float bottomLoad;
            if (myRank / comm_width != comm_length-1 && comm_length > 1)
                bottomLoad = getBottomLoad(myRank, comm_width);

            float leftLoad;
            if (myRank % comm_width != 0 && comm_width > 1)
                leftLoad = getLeftLoad(myRank);

            float rightLoad;
            if (myRank % comm_width != comm_width-1 && comm_width > 1)
                rightLoad = getRightLoad(myRank);

            int valueToExchange;
            // Vertical
            if (myRank / comm_width != 0 && comm_length > 1) {
                valueToExchange = static_cast<int>(diffCoef*fabs(lengthLoad-topLoad));

                if (balanceUsingTime) {
                  double val =  diffCoef*fabs(lengthLoad-topLoad) / max(lengthLoad, topLoad);
                  val = recalc_coef(val, diffPower);

                  float myvalue = piece.getLength();
                  sendLoadToTop(piece, myRank, comm_width, myvalue);
                  float partnervalue = getTopLoad(myRank, comm_width);

                  if (lengthLoad > topLoad) valueToExchange = static_cast<int>(val*myvalue);
                  else valueToExchange = static_cast<int>(val*partnervalue);
                }

                if (valueToExchange > 0) {
                    if (lengthLoad > topLoad) sendToTop(piece, valueToExchange, myRank, comm_width);
                    else recieveFromTop(piece, valueToExchange, myRank, comm_width);
                }
            }
            if (myRank / comm_width != comm_length-1 && comm_length > 1) {
                valueToExchange = static_cast<int>(diffCoef*fabs(lengthLoad-bottomLoad));

                if (balanceUsingTime) {
                  double val = diffCoef*fabs(lengthLoad-bottomLoad) / max(lengthLoad, bottomLoad);
                  val = recalc_coef(val, diffPower);

                  float myvalue = piece.getLength();
                  sendLoadToBottom(piece, myRank, comm_width, myvalue);
                  float partnervalue = getBottomLoad(myRank, comm_width);

                  if (lengthLoad > bottomLoad) valueToExchange = static_cast<int>(val*myvalue);
                  else valueToExchange = static_cast<int>(val*partnervalue);
                }

                if (valueToExchange > 0) {
                    if (lengthLoad > bottomLoad) sendToBottom(piece, valueToExchange, myRank, comm_width);
                    else recieveFromBottom(piece, valueToExchange, myRank,comm_width);
                }
            }

            // Horizontal
            if (myRank % comm_width != 0 && comm_width > 1) {
                int valueToExchange = static_cast<int>(diffCoef*fabs(leftLoad-widthLoad));

                if (balanceUsingTime) {
                  double val = diffCoef*fabs(leftLoad-widthLoad) / max(widthLoad, leftLoad);
                  val = recalc_coef(val, diffPower);

                  float myvalue = piece.getWidth();
                  sendLoadToLeft(piece, myRank, myvalue);
                  float partnervalue = getLeftLoad(myRank);

                  if (widthLoad > leftLoad) valueToExchange = static_cast<int>(val*myvalue);
                  else valueToExchange = static_cast<int>(val*partnervalue);
                }

                if (valueToExchange > 0) {
                    if (widthLoad > leftLoad) sendToLeft(piece, valueToExchange, myRank);
                    else recieveFromLeft(piece, valueToExchange, myRank);
                }
            }
            if (myRank % comm_width != comm_width-1 && comm_width > 1) {
                int valueToExchange = static_cast<int>(diffCoef*fabs(rightLoad-widthLoad));

                if (balanceUsingTime) {
                  double val = diffCoef*fabs(rightLoad-widthLoad) / max(widthLoad, rightLoad);
                  val = recalc_coef(val, diffPower);

                  float myvalue = piece.getWidth();
                  sendLoadToRight(piece, myRank, myvalue);
                  float partnervalue = getRightLoad(myRank);

                  if (widthLoad > rightLoad) valueToExchange = static_cast<int>(val*myvalue);
                  else valueToExchange = static_cast<int>(val*partnervalue);
                }

                if (valueToExchange > 0) {
                    if (widthLoad > rightLoad) sendToRight(piece, valueToExchange, myRank);
                    else recieveFromRight(piece, valueToExchange, myRank);
                }
            }

            lastBalanceTime = MPI_Wtime();
            calcTime = 0;
            // MPI_Barrier(MPI_COMM_WORLD);
        }

        double workTime = MPI_Wtime()-startTime;

        double maxtime;
        MPI_Reduce(&workTime, &maxtime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        if (myRank==0) cout << balanceOnceAt << "\t" << upTo << "\t" << diffPower << "\t" << maxtime << endl;

    } catch (char const *error) {
        cerr<<"Error: " << error << " at rank " << myRank << endl;
    }

    MPI_Finalize();
    return 0;
}
