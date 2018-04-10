#include "QSDT.h"
#include "dynpq.h"
QSDT::QSDT()
{


}

QSDT::~QSDT()
{


}

bool 
QSDT::init_node()
{
	int size = _latitude * _longitude ;
	_dQ.assign(size, Quat4d(1,0,0,0));
	_dist.assign(size, 10000);
	_npos.assign(size, -1);

	for(int i = 0 ; i < size ; i++)
	{
		if(!_binary[i])
		{
			_dQ[i] = Quat4d(0,0,0,1);
			_npos[i] = i;
			_dist[i] = -1;
		}
	}
	return true;
}

bool QSDT::run()
{
	dynamic_priority_queue<double> pq(_dist);
	while(pq.empty() == false)
	{
		int idx = (int)pq.topIndex() ; 
		double key = pq.topKey();
		pq.pop();
		int lat  = (int) (idx / _longitude);
		int lon  = (int) (idx % _longitude); 

		for(int j = -1 ; j <= 1 ; j++)
		{
			if(lat == 0 && j == -1 ) continue;
			if(lat == _latitude-1   && j == 1 ) continue;

			for(int i = -1 ; i <= 1 ; i++)
			{
				if(j == 0 && i == 0) continue;
				int di = (lon + i + _longitude) % _longitude;
				int dj = lat + j;
				int didx = dj * _longitude + di;  


				Vector3d vec;
				vec.cross(_pos[idx], _pos[didx]);
				double angle = _pos[idx].angle(_pos[didx]);
				Quat4d dq;
				dq.set(AxisAngle4d(vec,angle));
				dq.mul(_dQ[idx]); //^q = dq hat


				if(dq.w > _dQ[didx].w)
				{
					_dQ[didx] = dq;
					pq.changeKeyAt(didx, -dq.w);
				}
			}
		}
	}
	return true;
}



bool 
QSDT::finalize_distance()
{
	int size = _latitude * _longitude ;
	for(int i  = 0 ; i  < size ; i++)
	{
		if(fabs(_dQ[i].w - 1.0) > 1.0e-10)
		{
			_dQ[i].normalize();
		}
		_dist[i] = 2*acos(_dQ[i].w);
	}
	return true;
};