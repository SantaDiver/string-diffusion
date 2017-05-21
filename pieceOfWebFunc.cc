#include "pieceOfWebFunc.h"

PieceOfWebFunc::PieceOfWebFunc(float coef,
    vector<float> &prev_prev, vector<float> &prev, uint rank, uint comm_size) :

    _coef(coef), _prev(prev_prev), _res(prev),
    _rank(rank), _comm_size(comm_size), _stepsCounter(0)

{
    if (prev.size() != prev_prev.size())
        throw string("Wrong size of initial pieces!");

    _length = prev.size();

    _prev_prev = vector<float>( _length, 0);
}

void PieceOfWebFunc::calcRes()
{
    if(_rank != 0) {
        MPI_Request request;
        MPI_Isend
        (
            &_res[0],
            1,
            MPI_FLOAT,
            _rank-1,
            0,
            MPI_COMM_WORLD,
            &request
        );
    }

    if(_rank != _comm_size-1) {
        MPI_Request request;
        MPI_Isend
        (
            &_res[_length-1],
            1,
            MPI_FLOAT,
            _rank+1,
            0,
            MPI_COMM_WORLD,
            &request
        );
    }

    _prev.swap(_prev_prev);
    _res.swap(_prev);
    _res.resize(_prev.size());

    float leftPoint;
    if(_rank != 0) {
        MPI_Recv
        (
            &leftPoint,
            1,
            MPI_FLOAT,
            _rank-1,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }

    float rightPoint;
    if(_rank != _comm_size-1)
    {
        MPI_Recv
        (
            &rightPoint,
            1,
            MPI_FLOAT,
            _rank+1,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }

    if (_rank != 0)
        _res[0] = 2*_prev[0] - _prev_prev[0] + _coef*(leftPoint - 2*_prev[0] + _prev[1]);
    else
        _res[0] = _asideLeft[_stepsCounter];

    for (uint j=1; j<_length-1; ++j)
    {
        _res[j] = 2*_prev[j] - _prev_prev[j] + _coef*(_prev[j-1] - 2*_prev[j] + _prev[j+1]);
    }

    if (_rank != _comm_size-1)
        _res[_length-1] = 2*_prev[_length-1] - _prev_prev[_length-1] +
            _coef*(_prev[_length-2] - 2*_prev[_length-1] + rightPoint);
    else
        _res[_length-1] = _asideRight[_stepsCounter];

    ++_stepsCounter;
}

void PieceOfWebFunc::setAsideLeftVector(vector<float> &asideLeft)
{
    _asideLeft = asideLeft;
}

void PieceOfWebFunc::setAsideRightVector(vector<float> &asideRight)
{
    _asideRight = asideRight;
}

pair< vector<float>, vector<float> > PieceOfWebFunc::cutPieceFromLeft(uint l)
{
    vector<float> vecResToReturn(_res.begin(), _res.begin()+l);
    _res.erase(_res.begin(), _res.begin()+l);

    vector<float> vecPrevToReturn(_prev.begin(), _prev.begin()+l);
    _prev.erase(_prev.begin(), _prev.begin()+l);

    _length = _res.size();

    return pair< vector<float>, vector<float> > (vecResToReturn, vecPrevToReturn);
}

pair< vector<float>, vector<float> > PieceOfWebFunc::cutPieceFromRight(uint l)
{
    vector<float> vecResToReturn(_res.end()-l, _res.end());
    _res.erase(_res.end()-l, _res.end());

    vector<float> vecPrevToReturn( _prev.end()-l, _prev.end());
    _prev.erase( _prev.end()-l, _prev.end());

    _length = _res.size();

    return pair< vector<float>, vector<float> > (vecResToReturn, vecPrevToReturn);
}

void PieceOfWebFunc::addPieceToLeft(vector<float> pieceOfRes,
    vector<float> pieceOfPrev)
{
    _res.insert(_res.begin(), pieceOfRes.begin(), pieceOfRes.end());
    _prev.insert(_prev.begin(), pieceOfPrev.begin(), pieceOfPrev.end());

    _length = _res.size();
}

void PieceOfWebFunc::addPieceToRight(vector<float> pieceOfRes,
    vector<float> pieceOfPrev)
{
    _res.insert(_res.end(), pieceOfRes.begin(), pieceOfRes.end());
    _prev.insert(_prev.end(), pieceOfPrev.begin(), pieceOfPrev.end());

    _length = _res.size();
}

vector<float> PieceOfWebFunc::getRes()
{
    return _res;
}

uint PieceOfWebFunc::getLength()
{
    return _length;
}
