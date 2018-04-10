#include "SQSDT.h"
SQSDT::SQSDT()
{

}

SQSDT::~SQSDT()
{
	
};
bool 
SQSDT::run()
{
	forward_pass();
	backward_pass();
	forward_pass();
	return true;
};


bool 
SQSDT::forward_pass()
{
	for(int j = 0 ; j< _latitude -1 ; j += 1)
	{
		for(int i = 0 ; i < _longitude * 1.5 ; i += 1)
		{
			int ii = i % _longitude;
			int idx = j * _longitude + ii;

			int di[4] = {1,-1,0,1};
			int dj[4] = {0,1,1,1};

			for(int k = 0 ; k < 4 ; k++)
			{
				int didx = (j+dj[k])* _longitude + ((ii+di[k]) %_longitude);
				Vector3d vec;
				vec.cross(_pos[idx], _pos[didx]);
				double angle = _pos[idx].angle(_pos[didx]);
				Quat4d dq;
				dq.set(AxisAngle4d(vec,angle));
				dq.mul(_dQ[idx]); //^q = dq hat
				if(dq.w > _dQ[didx].w)
				{
					_dQ[didx] = dq;
					_npos[didx] = _npos[idx];
				}
			}
		}
	}
	return true;
};

bool 
SQSDT::backward_pass()
{
	for(int j = _latitude -1; j > 0 ; j -= 1)
	{
		for(int i = (int)(_longitude*1.5) ; i >=0 ; i -= 1)
		{
			int ii = i % _longitude;
			int idx = j * _longitude + ii;
			int di[4] = {-1,1,0,-1};
			int dj[4] = {0,-1,-1,-1};

			for(int k = 0 ; k < 4 ; k++)
			{
				int didx = (j+dj[k])* _longitude + ((_longitude + ii+di[k]) %_longitude);
				Vector3d vec;
				vec.cross(_pos[idx], _pos[didx]);
				double angle = _pos[idx].angle(_pos[didx]);
				Quat4d dq;
				dq.set(AxisAngle4d(vec,angle));
				dq.mul(_dQ[idx]); //^q = dq hat
				if(dq.w > _dQ[didx].w)
				{
					_dQ[didx] = dq;
					_npos[didx] = _npos[idx];
				}

			}
		}
	}
	return true;
};
