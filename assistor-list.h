#ifndef _ASSISTOR_LIST_H
#define _ASSISTOR_LIST_H
/*
 */

#ifndef lint
/*
static char rcsid[] =
    "@(#) $Header$ (LBL)";
 */
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "Tcl.h"

class AssistorList : public TclObject {
	struct	wassistor {
		char	*name;
		char	*formats;
	} *wassistors;
	int	nrofwassistors;
public:
	AssistorList() : TclObject("assistorlist") {
	}
	int command(int argc,const char *const*argv);
	void register_assistor(char *name,char *formats);
};
#endif _ASSISTOR_LIST_H
