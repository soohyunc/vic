/*
 * FILE:    mbus.h
 * AUTHORS: Colin Perkins
 * Modified by Dimitrios Miras
 *
 * Copyright (c) 1997,1998 University College London
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, is permitted, for non-commercial use only, provided
 * that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Computer Science
 *      Department at University College London
 * 4. Neither the name of the University nor of the Department may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 * Use of this software for commercial purposes is explicitly forbidden
 * unless prior written permission is obtained from the authors.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef mbus_handler_h
#define mbus_handler_h

#include "config.h"

#include "iohandler.h"
#include "timer.h"

#include "mbus_engine.h"

#include "memory.h"
#include "mbus.h"
#include "mbus_config.h"


#ifdef WIN32
extern "C" int gettimeofday(struct timeval *, struct timezone *);
#endif

class Source;

class MBusHandler : public IOHandler, public Timer {
    
private:
	struct mbus *mbusp_;

public:
	MBusHandler(void (*cmd_handler)(char *src, char *cmd, char *arg, void *dat),
		void (*err_handler)(int seqnum, int reason) );
	~MBusHandler();
	inline struct mbus* m() const { return (mbusp_); }
	//inline int mbus_fd() { return mbusp_->s.fd; }
	virtual void dispatch(int mask);
	void timeout();
	void listen_audio();

	char *mbus_audio_addr;
	char name[128];
};

#endif
