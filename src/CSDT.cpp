#define _USE_MATH_DEFINES
#include "CSDT.h"
#include "dynpq.h"
#include <cmath>
using namespace std;
CSDT::CSDT()
{
	_latitude = 100;
	_longitude = 100;
	_seednum = 10;

};
CSDT::~CSDT()
{


};
bool 
CSDT::run()
{
	dynamic_priority_queue<double> pq(_dist);
	int count=0;
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

				double angle = _pos[idx].angle(_pos[didx]);
				if(angle + _dist[idx] < _dist[didx])
				{
					pq.changeKeyAt(didx, angle + _dist[idx]);
				}
			}
		}
	}
	return true;
};
