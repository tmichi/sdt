#ifndef __SQSDT__
#define __SQSDT__ 1
#include "QSDT.h"


class SQSDT : public QSDT
{
public:
	SQSDT();
	virtual ~SQSDT();

	virtual bool run ();
	
protected:
	bool forward_pass();
	bool backward_pass();
};

#endif //