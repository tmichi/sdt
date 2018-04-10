#ifndef __VSDT__
#define __VSDT__ 1
#include "SDT.h"


class VSDT : public SDT
{
protected:	
	vector<Vector3d> _dvec; //距離ベクトル
	vector<int>		 _npos; //最近点
public:
	VSDT();
	~VSDT();

	virtual bool run ();
protected:
	virtual bool init_node();
	virtual bool finalize_distance();

};

#endif //