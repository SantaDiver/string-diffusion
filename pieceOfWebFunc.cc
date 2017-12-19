#include "pieceOfWebFunc.h"

PieceOfWebFunc::PieceOfWebFunc(float coef, vector< vector<float> > &t0, 
    vector< vector<float> > &t1, uint rank, uint comm_width, uint comm_length) :

    _coef(coef), _prev(t0), _res(t1), _rank(rank), _comm_width(comm_width), 
    _comm_length(comm_length), _stepsCounter(0)

{
    if (t0.size() != t1.size() || t0.size() < 1)
        throw "Wrong size of initial pieces!";

    _width = t0.size();
    _length = t0[0].size();

    _prev_prev = vector< vector<float> >(_width, vector<float>(_length, 0));
}

void PieceOfWebFunc::calcRes()
{
    if(_rank % _comm_width != 0) {
        MPI_Request request;
        MPI_Isend
        (
            &_res[0][0],
            _length,
            MPI_FLOAT,
            _rank-1,
            0,
            MPI_COMM_WORLD,
            &request
        );
    }

    if(_rank % _comm_width != _comm_width-1) {
        MPI_Request request;
        MPI_Isend
        (
            &_res[_width-1][0],
            _length,
            MPI_FLOAT,
            _rank+1,
            0,
            MPI_COMM_WORLD,
            &request
        );
    }
    
    if(_rank / _comm_width != 0) {
        vector<float> topPoints(_width);
        for (uint i=0; i<_width; ++i)
            topPoints[i] = _res[i][0];
        MPI_Request request;
        MPI_Isend
        (
            &topPoints[0],
            _width,
            MPI_FLOAT,
            _rank - _comm_width,
            0,
            MPI_COMM_WORLD,
            &request
        );
    }
    
    if(_rank / _comm_width != _comm_length-1) {
        vector<float> bottomPoints(_width);
        for (uint i=0; i<_width; ++i)
            bottomPoints[i] = _res[i][_length-1];
        MPI_Request request;
        MPI_Isend
        (
            &bottomPoints[0],
            _width,
            MPI_FLOAT,
            _rank + _comm_width,
            0,
            MPI_COMM_WORLD,
            &request
        );
    }

    _prev.swap(_prev_prev);
    _res.swap(_prev);
    _res.resize(_width);
    for(uint i=0; i<_res.size(); ++i)
        _res[i].resize(_length);
    
    vector<float> leftPoints(_length);
    if(_rank % _comm_width != 0) {
        MPI_Recv
        (
            &leftPoints[0],
            _length,
            MPI_FLOAT,
            _rank-1,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }
    else {
        leftPoints = _asideLeft[_stepsCounter];
    }
    
    vector<float> rightPoints(_length);
    if(_rank % _comm_width != _comm_width-1) {
        MPI_Recv
        (
            &rightPoints[0],
            _length,
            MPI_FLOAT,
            _rank+1,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }
    else {
        rightPoints = _asideRight[_stepsCounter];
    }
    
    vector<float> topPoints(_width);
    if(_rank / _comm_width != 0) {
        MPI_Recv
        (
            &topPoints[0],
            _width,
            MPI_FLOAT,
            _rank - _comm_width,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }
    else {
        topPoints = _asideTop[_stepsCounter];
    }
    
    vector<float> bottomPoints(_width);
    if(_rank / _comm_width != _comm_length-1) {
        MPI_Recv
        (
            &bottomPoints[0],
            _width,
            MPI_FLOAT,
            _rank + _comm_width,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }
    else {
        bottomPoints = _asideBottom[_stepsCounter];
    }
    
    _res[0][0] = 2*_prev[0][0] - _prev_prev[0][0] + 
        _coef*(leftPoints[0] - 2*_prev[0][0] + _prev[1][0] +
                topPoints[0] - 2*_prev[0][0] + _prev[0][1]);
    
    for(uint i=1; i<_width-1; ++i) {
        _res[i][0] = 2*_prev[i][0] - _prev_prev[i][0] +
            _coef*(_prev[i-1][0] - 2*_prev[i][0] + _prev[i+1][0] +
                topPoints[i] - 2*_prev[i][0] + _prev[i][1]);
    }
    
    for(uint j=1; j<_length-1; ++j) {
        _res[0][j] = 2*_prev[0][j] - _prev_prev[0][j] +
            _coef*(leftPoints[j] - 2*_prev[0][j] + _prev[1][j] +
                _prev[0][j-1] - 2*_prev[0][j] + _prev[0][j+1]);
    }

    for(uint i=1; i<_width-1; ++i) {
        for(uint j=1; j<_length-1; ++j) {
            _res[i][j] = 2*_prev[i][j] - _prev_prev[i][j] +
                _coef*(_prev[i-1][j] - 2*_prev[i][j] + _prev[i+1][j] +
                + _prev[i][j-1] - 2*_prev[i][j] + _prev[i][j+1]);
        }
    }
    
    _res[0][_length-1] = 2*_prev[0][_length-1] - _prev_prev[0][_length-1] +
        _coef*(leftPoints[_length-1] - 2*_prev[0][_length-1] + _prev[1][_length-1] +
            _prev[0][_length-2] - 2*_prev[0][_length-1] + bottomPoints[0]);
    
    for(uint i=1; i<_width-1; ++i) {
        _res[i][_length-1] = 2*_prev[i][_length-1] - _prev_prev[i][_length-1] +
            _coef*(_prev[i-1][_length-1] - 2*_prev[i][_length-1] + _prev[i+1][_length-1] +
                _prev[i][_length-2] - 2*_prev[i][_length-1] + topPoints[i]);
    }
    
    _res[_width-1][0] = 2*_prev[_width-1][0] - _prev_prev[_width-1][0] +
        _coef*(_prev[_width-2][0] - 2*_prev[_width-1][0] + rightPoints[0] +
            topPoints[_width-1] - 2*_prev[_width-1][0] + _prev[_width-1][1]);
    
    for(uint j=1; j<_length-1; ++j) {
        _res[_width-1][j] = 2*_prev[_width-1][j] - _prev_prev[_width-1][j] +
            _coef*(_prev[_width-2][j] - 2*_prev[_width-1][j] + rightPoints[j] +
                _prev[_width-1][j-1] - 2*_prev[_width-1][j] + _prev[_width-1][j+1]);
    }
    
    _res[_width-1][_length-1] = 2*_prev[_width-1][_length-1] - _prev_prev[_width-1][_length-1] +
        _coef*(_prev[_width-2][_length-1] - 2*_prev[_width-1][_length-1] + rightPoints[_length-1] +
            _prev[_width-1][_length-2] - 2*_prev[_width-1][_length-1] + bottomPoints[_width-1]);

    ++_stepsCounter;
}
void PieceOfWebFunc::setAsideVectors(vector< vector<float> > &asideLeft, 
        vector< vector<float> > &asideRight, vector< vector<float> > &asideTop,
        vector< vector<float> > &asideBottom)

{
    _asideLeft = asideLeft;
    _asideRight = asideRight;
    _asideTop = asideTop;
    _asideBottom = asideBottom;
}

pair< vector< vector<float> >, vector< vector<float> > > PieceOfWebFunc::cutPieceFromLeft(uint l)
{
     vector< vector<float> > vecResToReturn(_res.begin(), _res.begin()+l);
    _res.erase(_res.begin(), _res.begin()+l);

     vector< vector<float> > vecPrevToReturn(_prev.begin(), _prev.begin()+l);
    _prev.erase(_prev.begin(), _prev.begin()+l);

    _width = _res.size();

    return pair< vector< vector<float> >, vector< vector<float> > > (vecResToReturn, vecPrevToReturn);
}

pair< vector< vector<float> >, vector< vector<float> > > PieceOfWebFunc::cutPieceFromRight(uint l)
{
     vector< vector<float> > vecResToReturn(_res.end()-l, _res.end());
    _res.erase(_res.end()-l, _res.end());

     vector< vector<float> > vecPrevToReturn( _prev.end()-l, _prev.end());
    _prev.erase( _prev.end()-l, _prev.end());
    
    _width = _res.size();

    return pair< vector< vector<float> >, vector< vector<float> > > (vecResToReturn, vecPrevToReturn);
}

pair< vector< vector<float> >, vector< vector<float> > > PieceOfWebFunc::cutPieceFromTop(uint l)
{
    vector< vector<float> > vecResToReturn(_width, vector<float>(l));
    for(uint j=0; j<_width; ++j) {
        vecResToReturn[j] = vector<float>(_res[j].begin(), _res[j].begin()+l);
        _res[j].erase(_res[j].begin(), _res[j].begin()+l);
    }
    
    vector< vector<float> > vecPrevToReturn(_width, vector<float>(l));
    for(uint j=0; j<_width; ++j) {
        vecPrevToReturn[j] = vector<float>(_prev[j].begin(), _prev[j].begin()+l);
        _prev[j].erase(_prev[j].begin(), _prev[j].begin()+l);
    }
    
    _length = _res[0].size();
    
    return pair< vector< vector<float> >, vector< vector<float> > > (vecResToReturn, vecPrevToReturn);
}

pair< vector< vector<float> >, vector< vector<float> > > PieceOfWebFunc::cutPieceFromBottom(uint l)
{
    vector< vector<float> > vecResToReturn(_width, vector<float>(l));
    for(uint j=0; j<_width; ++j) {
        vecResToReturn[j] = vector<float>(_res[j].end()-l, _res[j].end());
        _res[j].erase(_res[j].end()-l, _res[j].end());
    }
    
    vector< vector<float> > vecPrevToReturn(_width, vector<float>(l));
    for(uint j=0; j<_width; ++j) {
        vecPrevToReturn[j] = vector<float>(_prev[j].end()-l, _prev[j].end());
        _prev[j].erase(_prev[j].end()-l, _prev[j].end());
    }
    
    _length = _res[0].size();
    
    return pair< vector< vector<float> >, vector< vector<float> > > (vecResToReturn, vecPrevToReturn);
}

void PieceOfWebFunc::addPieceToLeft(vector< vector<float> > pieceOfRes, 
    vector< vector<float> > pieceOfPrev)
{
    _res.insert(_res.begin(), pieceOfRes.begin(), pieceOfRes.end());
    _prev.insert(_prev.begin(), pieceOfPrev.begin(), pieceOfPrev.end());

    _width = _res.size();
}

void PieceOfWebFunc::addPieceToRight(vector< vector<float> > pieceOfRes, 
    vector< vector<float> > pieceOfPrev)
{
    _res.insert(_res.end(), pieceOfRes.begin(), pieceOfRes.end());
    _prev.insert(_prev.end(), pieceOfPrev.begin(), pieceOfPrev.end());

    _width = _res.size();
}

void PieceOfWebFunc::addPieceToTop(vector< vector<float> > pieceOfRes, 
    vector< vector<float> > pieceOfPrev)
{
    for(uint j=0; j<_width; ++j) {
        _res[j].insert(_res[j].begin(), pieceOfRes[j].begin(), pieceOfRes[j].end());
        _prev[j].insert(_prev[j].begin(), pieceOfPrev[j].begin(), pieceOfPrev[j].end());    
    }

    _length = _res[0].size();
}

void PieceOfWebFunc::addPieceToBottom(vector< vector<float> > pieceOfRes, 
    vector< vector<float> > pieceOfPrev)
{
    for(uint j=0; j<_width; ++j) {
        _res[j].insert(_res[j].begin(), pieceOfRes[j].begin(), pieceOfRes[j].end());
        _prev[j].insert(_prev[j].begin(), pieceOfPrev[j].begin(), pieceOfPrev[j].end());    
    }

    _length = _res[0].size();
}

vector< vector<float> > PieceOfWebFunc::getRes()
{
    return _res;
}

uint PieceOfWebFunc::getWidth()
{
    return _width;
}

uint PieceOfWebFunc::getLength()
{
    return _length;
}
