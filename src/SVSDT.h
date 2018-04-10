#ifndef __SVSDT__
#define __SVSDT__ 1
#include "VSDT.h"


class SVSDT : public VSDT
{
protected:	
public:
	SVSDT();
	~SVSDT();

	virtual bool run ();
protected:
	bool forward_pass();
	bool backward_pass();
};

#endif //