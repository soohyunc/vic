/*-
 * Copyright (c) 1993-1994 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and the Network Research Group at
 *      Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
static const char rcsid[] =
    "@(#) $Header$ (LBL)";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inet.h"
#include "rtp.h"
#include "decoder.h"
#include "bsd-endian.h"
#include "Tcl.h"
#include "renderer.h"
#include "decoder-jpeg.h"

class RawDecoder : public Decoder {
public:
	RawDecoder();
	virtual ~RawDecoder();
	int colorhist(u_int* hist) const;
	virtual void stats(char* wrk);
protected:
	virtual void recv(const struct rtphdr*, const u_char* data, int len);
	virtual void redraw();

	int inq_;		/* input quantization */
	int type_;		/* JPEG/RTP parameters type code */

	JpegReassembler reasm_;
};

static class RawDecoderMatcher : public Matcher {
public:
	RawDecoderMatcher() : Matcher("decoder") {}
	TclObject* match(const char* id) {
		if (strcasecmp(id, "raw") == 0)
			return (new RawDecoder());
		else
			return (0);
	}
} swd_raw_;

RawDecoder::RawDecoder()
	: Decoder(sizeof(jpeghdr))
{

	inw_ = 352;
	inh_ = 288;
	resize(inw_, inh_);
	inq_ = 0;
	/* guess type 0 */
	type_ = 0;
	decimation_ = 422;

	stat_[STAT_BADOFF].name = "Bad-Offset";
	stat_[STAT_HUGEFRM].name = "Huge-Frame";
	nstat_ = 2;
}

RawDecoder::~RawDecoder()
{
}

void RawDecoder::stats(char* wrk)
{
	/* pull stats out of reassembler */
	setstat(STAT_BADOFF, reasm_.badoff());
	setstat(STAT_HUGEFRM, reasm_.hugefrm());
	Decoder::stats(wrk);
}

int RawDecoder::colorhist(u_int* hist) const
{
	return 1;
}

void RawDecoder::recv(const rtphdr* rh, const u_char* bp, int cc)
{	
	/* using jpeg header */
	const jpeghdr* p = (const jpeghdr*)(rh + 1);

	int inw = p->width << 3;
	int inh = p->height << 3;

	if (inw_ !=  inw || inh_ != inh) {
		resize(inw, inh);
	}

	/* use jpeg reassembler to reassemble packets */
	bp = reasm_.reassemble(rh, bp, cc);

	/* update whole frame now */
	memset(rvts_, now_, nblk_);
	
	if (bp != 0) {
		render_frame(bp);
	}
}

void RawDecoder::redraw()
{
}
