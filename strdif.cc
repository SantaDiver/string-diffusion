/*

Made by SNM

*/

// System includes
#include "systemIncludes.h"

// Includes of my files
#include "pieceOfWebFunc.h"
#include "generatingFuncs.h"

using namespace std;

int myRank = 0, comm_size;

bool debug = false;
bool diffusion = true;

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    float c = 1;
    float tau = 0.4;
    float h = 2;

    float coef = pow(tau, 2) * pow (c, 2) / pow(h, 2);

    uint stepsToCount = 30000;
    vector<float> asideLeft;
    vector<float> asideRight;

    uint fullLength = 5000;
    vector<int> lengthForProc(comm_size-1, 10);

    if(myRank==0) {
        vector<float> initial = makeParab(fullLength);

        asideLeft = vector<float>(stepsToCount, 0);
        asideRight = vector<float>(stepsToCount, 0);
        vector<float> initialV(initial.size(), 0);

        vector<float> step2(initial.size());
        for (uint i=1; i<step2.size()-1; ++i)
        {
            step2[i] = initial[i] + initialV[i]*tau + coef/2 *
                (initial[i+1] - 2*initial[i] + initial[i-1]);
        }
        step2[0] = asideLeft[0];
        step2[step2.size()-1] = asideRight[0];

        uint restLength = fullLength;
        uint alreadySent = 0;

        for(uint proc=0; proc<comm_size-1; ++proc) {
            MPI_Request request;
            MPI_Isend (&lengthForProc[proc], 1, MPI_INT, proc,
                0, MPI_COMM_WORLD, &request);

            vector<float> initialPartToSend(
                initial.begin()+alreadySent,
                initial.begin()+alreadySent+lengthForProc[proc]
            );
            MPI_Isend (initialPartToSend.data(), lengthForProc[proc], MPI_FLOAT, proc,
                1, MPI_COMM_WORLD, &request);

            vector<float> step2PartToSend(
                step2.begin()+alreadySent,
                step2.begin()+alreadySent+lengthForProc[proc]
            );
            MPI_Isend (step2PartToSend.data(), lengthForProc[proc], MPI_FLOAT, proc,
                2, MPI_COMM_WORLD, &request);

            restLength -= lengthForProc[proc];
            if (restLength <= 0) throw string("Bag vector of lengths!");

            alreadySent += lengthForProc[proc];
        }

        MPI_Request request;
        MPI_Isend (&restLength, 1, MPI_INT, comm_size-1,
            0, MPI_COMM_WORLD, &request);

        vector<float> initialPartToSend(
                initial.end()-restLength,
                initial.end()
        );
        MPI_Isend (initialPartToSend.data(), restLength, MPI_FLOAT, comm_size-1,
            1, MPI_COMM_WORLD, &request);


        vector<float> step2PartToSend(
                step2.end()-restLength,
                step2.end()
        );
        MPI_Isend (step2PartToSend.data(), restLength, MPI_FLOAT, comm_size-1,
            2, MPI_COMM_WORLD, &request);
    }

    if(myRank == comm_size-1) {
        asideRight = vector<float>(30000, 0);
    }

    int lengthOfPart;
    MPI_Recv (&lengthOfPart, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    vector<float> prev_prev(lengthOfPart);
    vector<float> prev(lengthOfPart);

    MPI_Recv (prev_prev.data(), lengthOfPart, MPI_FLOAT, 0, 1,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Recv (prev.data(), lengthOfPart, MPI_FLOAT, 0, 2,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    PieceOfWebFunc piece(coef, prev_prev, prev, myRank, comm_size);
    if (myRank==0) piece.setAsideLeftVector(asideLeft);
    if (myRank==comm_size-1) piece.setAsideRightVector(asideRight);

    uint n = 100;
    uint resultLayersNum = (stepsToCount+1)/n;
    if (myRank == 0 && debug) {
        cout << 0 << endl;
        cout << h << endl;
        cout << fullLength << endl;
        cout << resultLayersNum << endl;
    }

    double startTime = MPI_Wtime();

    for(uint i=1; i<=stepsToCount; ++i)
    {
        // if (myRank==1) cerr << lengthOfPart << endl;
        piece.calcRes();

        if (i%n==0 && debug) {
            vector<float> res=piece.getRes();

            MPI_Request request;
            MPI_Isend (&lengthOfPart, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
            MPI_Isend (res.data(), lengthOfPart, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &request);

            if(myRank == 0) {
                vector<float> buff(fullLength);
                int buffLength;

                for(uint proc=0; proc<comm_size; ++proc) {
                    MPI_Recv (&buffLength, 1 , MPI_INT,
                        proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv (buff.data(), buffLength, MPI_FLOAT,
                        proc, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    for(uint j=0; j<buffLength; ++j) {
                        cout << buff[j] << " ";
                    }
                }
                cout << endl;
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }

        if (diffusion) {
            int lengthOfLeftPartner;
            int lengthOfRightPartner;
            MPI_Request request;
            if(myRank != 0)
                MPI_Isend (&lengthOfPart, 1, MPI_INT, myRank-1, 0, MPI_COMM_WORLD, &request);
            if(myRank != comm_size-1)
                MPI_Isend (&lengthOfPart, 1, MPI_INT, myRank+1, 0, MPI_COMM_WORLD, &request);

            if(myRank != 0)
                MPI_Recv (&lengthOfLeftPartner, 1 , MPI_INT, myRank-1,
                    0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(myRank != comm_size-1)
                MPI_Recv (&lengthOfRightPartner, 1 , MPI_INT, myRank+1,
                    0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            float diffCoef = 1.0/3.0;
            if(myRank != 0) {
                uint valueToExchange = diffCoef * abs(lengthOfPart-lengthOfLeftPartner);
                if(lengthOfPart < lengthOfLeftPartner) {
                    vector<float> res(valueToExchange);
                    MPI_Recv (res.data(), valueToExchange , MPI_FLOAT, myRank-1,
                        0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    vector<float> prev(valueToExchange);
                    MPI_Recv (prev.data(), valueToExchange , MPI_FLOAT, myRank-1,
                        1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    piece.addPieceToLeft(res, prev);
                }
                else if(lengthOfPart > lengthOfLeftPartner) {
                    pair< vector<float>, vector<float> > pair =
                        piece.cutPieceFromLeft(valueToExchange);

                    MPI_Isend (pair.first.data(), valueToExchange, MPI_FLOAT,
                        myRank-1, 0, MPI_COMM_WORLD, &request);
                    MPI_Isend (pair.second.data(), valueToExchange, MPI_FLOAT,
                        myRank-1, 1, MPI_COMM_WORLD, &request);
                }
            }
            if(myRank != comm_size-1) {
                uint valueToExchange = diffCoef * abs(lengthOfPart-lengthOfRightPartner);

                if(lengthOfPart < lengthOfRightPartner) {
                    vector<float> res(valueToExchange);
                    MPI_Recv (res.data(), valueToExchange , MPI_FLOAT, myRank+1,
                        0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    vector<float> prev(valueToExchange);
                    MPI_Recv (prev.data(), valueToExchange , MPI_FLOAT, myRank+1,
                        1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    piece.addPieceToRight(res, prev);

                }
                else if(lengthOfPart > lengthOfRightPartner) {
                    pair< vector<float>, vector<float> > pair =
                        piece.cutPieceFromRight(valueToExchange);
                    MPI_Isend (pair.first.data(), valueToExchange, MPI_FLOAT,
                        myRank+1, 0, MPI_COMM_WORLD, &request);
                    MPI_Isend (pair.second.data(), valueToExchange, MPI_FLOAT,
                        myRank+1, 1, MPI_COMM_WORLD, &request);
                }
            }

            lengthOfPart = piece.getLength();
        }
    }

    double workTime = MPI_Wtime()-startTime;

    double maxtime;
    MPI_Reduce(&workTime, &maxtime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (myRank==0 && !debug) cout << maxtime << endl;

    MPI_Finalize();
    return 0;
}
