#include "VSDT.h"
#include "dynpq.h"
VSDT::VSDT()
{

};
VSDT::~VSDT()
{

};

bool 
VSDT::init_node()
{
	int size = _latitude * _longitude ;
	_dvec.assign(size, Vector3d(10000,0,0));
	_dist.assign(size, 10000);
	_npos.assign(size, -1);

	for(int i = 0 ; i < size ; i++)
	{
		if(!_binary[i])
		{
			_dvec[i] = Vector3d(0,0,0);
			_npos[i] = i;
			_dist[i] = 0;
		}
	}
	return true;
};

bool 
VSDT::run ()
{
	dynamic_priority_queue<double> pq(_dist);
	int count=0;
	while(pq.empty() == false)
	{
		int idx = (int)pq.topIndex() ; 
		double key =   pq.topKey();
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
				vec.sub(_pos[didx], _pos[idx]);
				vec.add(_dvec[idx]);
				double length = vec.lengthSquared();
				if(length < _dist[didx])
				{
					_npos[didx] = _npos[idx];
					_dvec[didx] = vec;
					pq.changeKeyAt(didx, length);
				}
			}
		}
	}
	return true;
};


bool 
VSDT::finalize_distance()
{
	int size = _latitude * _longitude ;
	for(int i  = 0 ; i  < size ; i++)
	{
		_dist[i] =acos( 1 - 0.5*_dvec[i].lengthSquared()  );
	}
	return true;
};