#ifndef __CSDT__
#define __CSDT__ 1
#include "SDT.h"
class CSDT : public SDT
{
public:
	CSDT();
	~CSDT();
	virtual bool run();
};

#endif//