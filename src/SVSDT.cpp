#include "SVSDT.h"

#define CYCLE 1.5

SVSDT::SVSDT()
{

};

SVSDT::~SVSDT()
{

};

bool 
SVSDT::run()
{
	cerr<<"Start computing"<<endl;
	forward_pass();
	backward_pass();
	if(CYCLE>1)forward_pass();
	return true;
};
bool 
SVSDT::forward_pass()
{
	for(int j = 0 ; j< _latitude -1 ; j += 1)
	{
		for(int i = 0 ; i < _longitude*CYCLE ; i += 1)
		{
			int ii = i % _longitude;
			int idx = j * _longitude + ii;

			int di[4] = {1,-1,0,1};
			int dj[4] = {0,1,1,1};

			for(int k = 0 ; k < 4 ; k++)
			{
				int didx = (j+dj[k])* _longitude + ((ii+di[k]) %_longitude);
							
				Vector3d vec;
				vec.sub(_pos[didx], _pos[idx]);
				vec.add(_dvec[idx]);
				if(vec.lengthSquared() < _dvec[didx].lengthSquared())
				{
					_dvec[didx] = vec;
					_npos[didx] = _npos[idx];
				}
			}
		}
	}
	return true;
};

bool 
SVSDT::backward_pass()
{
	for(int j = _latitude -1; j > 0 ; j -= 1)
	{
		for(int i = (int)(_longitude*CYCLE) ; i >=0 ; i -= 1)
		{
			int ii = i % _longitude;
			int idx = j * _longitude + ii;
			int di[4] = {-1,1,0,-1};
			int dj[4] = {0,-1,-1,-1};

			for(int k = 0 ; k < 4 ; k++)
			{
				int didx = (j+dj[k])* _longitude + ((_longitude + ii+di[k]) %_longitude);
				Vector3d vec;
				vec.sub(_pos[didx], _pos[idx]);
				vec.add(_dvec[idx]);
				if(vec.lengthSquared() < _dvec[didx].lengthSquared())
				{
					_dvec[didx] = vec;
					_npos[didx] = _npos[idx];
				}
			}
		}
	}
	return true;
};
