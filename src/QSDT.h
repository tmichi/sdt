#ifndef __QSDT__
#define __QSDT__ 1
#include "SDT.h"


class QSDT : public SDT
{
protected:	
	vector<Quat4d>   _dQ;
	vector<int>		 _npos; //�ŋߓ_
public:
	QSDT();
	virtual ~QSDT();

	virtual bool run ();
protected:
	virtual bool init_node();
	virtual bool finalize_distance();
};

#endif //