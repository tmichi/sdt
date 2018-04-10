#ifndef __VSDT__
#define __VSDT__ 1
#include "SDT.h"


class VSDT : public SDT
{
protected:	
	vector<Vector3d> _dvec; //�����x�N�g��
	vector<int>		 _npos; //�ŋߓ_
public:
	VSDT();
	~VSDT();

	virtual bool run ();
protected:
	virtual bool init_node();
	virtual bool finalize_distance();

};

#endif //