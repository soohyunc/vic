/*
 * Copyright (c) 1994-1995 Regents of the University of California.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "sys-time.h"
#include "source.h"
#include "inet.h"
#include "config.h"
#include "ntp-time.h"

#include <fcntl.h>
/* This bit for playout buffer as was in playout.cc */

char *MtuAlloc::blk_list_;

MtuAlloc::MtuAlloc()
{
}

/* When does this get called? */
MtuAlloc::~MtuAlloc()
{
        char **p;

        while ((p = (char**)blk_list_) != NULL) {
                blk_list_ = *p;
                delete p;
        }
}
PacketData::PacketData(u_char *pkt, struct rtphdr* rh, u_char *vh,
                       int len, u_int playout)
        : pkt_(pkt), rh_(rh), vh_(vh), len_(len), playout_(playout), next_(0)
{
}

PacketData::~PacketData()
{
        /* RTPv1 */
        if ((u_char*)rh_ != pkt_) {
#ifdef not_done
                printf("Delete rh!\n");
                delete rh_;
#endif
        }
//        rh_ = 0;

        if (pkt_) {
		//pkt_ -= 4;
                delete_blk((char*)pkt_);
                pkt_ = 0;
        }
}

static int server_delay = 100;

/* gray out src if no ctrl msgs for this many consecutive update intervals */
#define CTRL_IDLE 8.

#define SHASH(a) ((int)((((a) >> 20) ^ ((a) >> 10) ^ (a)) & (SOURCE_HASH-1)))

/*XXX*/
PacketHandler::~PacketHandler() { }

Source::Source(u_int32_t srcid, u_int32_t ssrc, Address &addr)
	: TclObject(0),
	  next_(0),
	  hlink_(0),
	  handler_(0),
	  srcid_(srcid),
	  ssrc_(ssrc),
	  addr_(*(addr.copy())),
	  sts_data_(0),
	  sts_ctrl_(0),
	  map_rtp_time_(0), map_ntp_time_(0),	
	  fs_(0),
	  cs_(0),
	  np_(0),
	  nf_(0),
	  nb_(0),
	  nm_(0),
	  snp_(0),
	  sns_(0),
	  ndup_(0),
	  nrunt_(0),
	  badsesslen_(0),
	  badsessver_(0),
	  badsessopt_(0),
	  badsdes_(0),
	  badbye_(0),
	  format_(-1),
	  mute_(0),
	  lost_(0),
	  busy_(0),
	  ismixer_(0),
	  sync_(0), rtp2ntp_(0),
	  skew_(0), delta_(0), delay_(0), dvar_(80. * 90.), pdelay_(0),  
	  adapt_init_(0), count_(0), late_(0), apdelay_(0), pending_(0), 
	  head_(0), tail_(0), free_(0), 
	  now_(0), dtskew_(0), elastic_(1),
	  mbus_(0)
{
	lts_data_.tv_sec = 0;
	lts_data_.tv_usec = 0;
	lts_ctrl_.tv_sec = 0;
	lts_ctrl_.tv_usec = 0;
	lts_done_.tv_sec = 0;
	lts_done_.tv_usec = 0;
	/*
	 * Put an invalid seqno in each slot to guarantee that
	 * we don't count any initial dups by mistake.
	 */
	int i;
	for (i = 0; i < SOURCE_NSEQ; ++i)
		seqno_[i] = i + 1;

	for (i = 0; i <= RTCP_SDES_MAX; ++i)
		sdes_[i] = 0;
	//DMIRAS: was in ElasticBuffer constructor, ignore it for the moment...
/*        char    *p;

        if (p = getenv("SDEL"))
                server_delay = atoi(p);
*/

	Tcl::instance().evalf("register %s", TclObject::name());
}


Source::~Source()
{
	if (&addr_) delete &addr_;
	if (handler_ != 0)
		deactivate();
	Tcl::instance().evalf("unregister %s", TclObject::name());
	int i;
	for (i = 0; i <= RTCP_SDES_MAX; ++i)
		delete sdes_[i];
	// Destructor from ElasticBuffer, releases alloc-ed mem
	register PacketData *pkt;

        while (head_) {
                pkt = head_;
                head_ = head_->next_;
                delete pkt;
        }

        while (free_) {
                pkt = free_;
                free_ = free_->next_;
                delete pkt;
        }

        cancel();
}

void Source::set_busy()
{
	busy_ = 1;
	Tcl::instance().evalf("set_busy %s", TclObject::name());
}

PacketHandler* Source::activate(int format)
{
	format_ = format;
	Tcl::instance().evalf("activate %s", TclObject::name());
	if (handler_ == 0)
		/* tcl should install a handler */
		abort();
	return (handler_);
}

PacketHandler* Source::change_format(int format)
{
	format_ = format;
	Tcl::instance().evalf("change_format %s", TclObject::name());
	return (handler_);
}

void Source::deactivate()
{
	Tcl::instance().evalf("deactivate %s", TclObject::name());
	/*XXX tcl deletes it */
	handler_ = 0;
}

void Source::sdes(int t, const char* s)
{
	char** p = &sdes_[t];
	if (*p != 0 && strcmp(*p, s) == 0)
		/* no change */
		return;

	delete *p;
	int n = strlen(s);
	if (n > 254)
		n = 254;
	*p = new char[n + 1];
	strncpy(*p, s, n + 1);
	/*XXX*/
	Tcl::instance().evalf("update_source_info %s", TclObject::name());
}

char* onestat(char* cp, const char* name, u_long v)
{
	sprintf(cp, "%s %lu ", name, v);
	return (cp + strlen(cp));
}

char* Source::stats(char* cp) const
{
	cp = onestat(cp, "Kilobits", nb() >> (10-3));
	cp = onestat(cp, "Frames", nf());
	cp = onestat(cp, "Packets", np());
	int missing = ns() - np();
	if (missing < 0)
		missing = 0;
	cp = onestat(cp, "Missing", missing);
	cp = onestat(cp, "Misordered", nm());
	cp = onestat(cp, "Runts", runt());
	cp = onestat(cp, "Dups", dups());
	cp = onestat(cp, "Bad-S-Len", badsesslen());
	cp = onestat(cp, "Bad-S-Ver", badsessver());
	cp = onestat(cp, "Bad-S-Opt", badsessopt());
	cp = onestat(cp, "Bad-Sdes", badsdes());
	cp = onestat(cp, "Bad-Bye", badbye());
	*--cp = 0;
	return (cp);
}

static int sdes_atoi(const char* s)
{
	if (strcasecmp(s, "cname") == 0)
		return (RTCP_SDES_CNAME);
	if (strcasecmp(s, "name") == 0)
		return (RTCP_SDES_NAME);
	if (strcasecmp(s, "email") == 0)
		return (RTCP_SDES_EMAIL);
	if (strcasecmp(s, "phone") == 0)
		return (RTCP_SDES_PHONE);
	if (strcasecmp(s, "loc") == 0 || strcasecmp(s, "location") == 0)
		return (RTCP_SDES_LOC);
	if (strcasecmp(s, "tool") == 0)
		return (RTCP_SDES_TOOL);
	if (strcasecmp(s, "note") == 0)
		return (RTCP_SDES_NOTE);
	return (-1);
}

int Source::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();
	char* wrk = tcl.buffer();

	if (argc == 2) {
		if (strcmp(argv[1], "stats") == 0) {
			char* bp = tcl.buffer();
			tcl.result(bp);
			stats(bp);
			return (0);
		}
		if (strcmp(argv[1], "deactivate") == 0) {
			deactivate();
			return (TCL_OK);
		}
		if (strcmp(argv[1], "ns") == 0) {
			sprintf(wrk, "%lu", (u_long)ns());
			tcl.result(wrk);
			return (TCL_OK);
		}
#ifdef notdef
		if (strcmp(argv[1], "dvar") == 0) {
			double ms = dvar() * (1000. / 65536.);
			sprintf(wrk, "%g", ms);
			tcl.result(wrk);
			return (TCL_OK);
		}
#endif
		if (strcmp(argv[1], "np") == 0) {
			sprintf(wrk, "%lu", (u_long)np());
			tcl.result(wrk);
			return (TCL_OK);
		}
		if (strcmp(argv[1], "nb") == 0) {
			sprintf(wrk, "%lu", (u_long)(8 * nb()));
			tcl.result(wrk);
			return (TCL_OK);
		}
		if (strcmp(argv[1], "nf") == 0) {
			sprintf(wrk, "%lu", (u_long)nf());
			tcl.result(wrk);
			return (TCL_OK);
		}
		if (strcmp(argv[1], "addr") == 0) {
			strcpy(wrk, (const char *)addr_);
		//	tcl.result(InetNtoa(addr_), TCL_DYNAMIC);
			return (TCL_OK);
		}
		if (strcmp(argv[1], "srcid") == 0) {
			sprintf(wrk, "%lu", (u_long)ntohl(srcid_));
			tcl.result(wrk);
			return (TCL_OK);
		}
		if (strcmp(argv[1], "ssrc") == 0) {
			sprintf(wrk, "%lu", (u_long)ntohl(ssrc_));
			tcl.result(wrk);
			return (TCL_OK);
		}
		if (strcmp(argv[1], "format") == 0) {
			sprintf(wrk, "%d", format_);
			tcl.result(wrk);
			return (TCL_OK);
		}
		if (strcmp(argv[1], "lastdata") == 0) {
			if (lts_data_.tv_sec != 0) {
				time_t now = lts_data_.tv_sec;
				char* cp = tcl.buffer();
				tcl.result(cp);
				strcpy(cp, ctime(&now));
				cp = strrchr(cp, '\n');
				if (cp != 0)
					*cp = 0;
			}
			return (TCL_OK);
		}
		if (strcmp(argv[1], "lastctrl") == 0) {
			if (lts_ctrl_.tv_sec != 0) {
				time_t now = lts_ctrl_.tv_sec;
				char* cp = tcl.buffer();
				tcl.result(cp);
				strcpy(cp, ctime(&now));
				cp = strrchr(cp, '\n');
				if (cp != 0)
					*cp = 0;
			}
			return (TCL_OK);
		}
		if (strcmp(argv[1], "lost") == 0) {
			tcl.result(lost_ ? "1" : "0");
			return (TCL_OK);
		}
		if (strcmp(argv[1], "handler") == 0) {
			if (handler_ != 0)
				tcl.result(handler_->name());
			return (TCL_OK);
		}
		if (strcmp(argv[1], "clear-busy") == 0) {
			busy_ = 0;
			return (TCL_OK);
		}
		if (strcmp(argv[1], "last-data") == 0) {
			sprintf(tcl.buffer(), "%u", ntptime(lts_data_));
			tcl.result(tcl.buffer());
			return (TCL_OK);
		}
		if (strcmp(argv[1], "mute") == 0) {
			sprintf(tcl.buffer(), "%u", mute_);
			tcl.result(tcl.buffer());
			return (TCL_OK);
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "sdes") == 0) {
			int item = sdes_atoi(argv[2]);
			if (item >= 0 && sdes_[item] != 0) {
				strcpy(wrk, sdes_[item]);
				tcl.result(wrk);
			}
			return (TCL_OK);
		}			
		if (strcmp(argv[1], "mute") == 0) {
			int v = atoi(argv[2]);
			mute(v);
			return (TCL_OK);
		}			
		if (strcmp(argv[1], "handler") == 0) {
			const char* o = argv[2];
			if (*o == 0)
				handler_ = 0;
			else
				handler_ = (PacketHandler*)TclObject::lookup(o);
			return (TCL_OK);
		}			
	} else if (argc == 4) {
		if (strcmp(argv[1], "sdes") == 0) {
			int item = sdes_atoi(argv[2]);
			if (item >= 0) {
				/*XXX error?*/
				sdes(item, argv[3]);
			}
			return (TCL_OK);
		}			
	}
	return (TclObject::command(argc, argv));
}
	
void Source::lost(int v)
{
	if (lost_ != v) {
		Tcl& tcl = Tcl::instance();
		lost_ = v;
		if (lost_)
			tcl.evalf("grayout %s", TclObject::name());
		else
			tcl.evalf("embolden %s", TclObject::name());
	}
}

void Source::clear_counters()
{
	np_ = 0;
	nf_ = 0;
	nb_ = 0;
	nm_ = 0;
	snp_ = 0;
	sns_ = 0;
	ndup_ = 0;
	nrunt_ = 0;

	lts_data_.tv_sec = 0;
	lts_data_.tv_usec = 0;
	lts_ctrl_.tv_sec = 0;
	lts_ctrl_.tv_usec = 0;
	lts_done_.tv_sec = 0;
	lts_done_.tv_usec = 0;
}




/* DMIRAS: Extra functions to implement the buffer */

void Source::adapt(u_int32_t arr_ts, u_int32_t ts, int flag)
{
	char arg[500];
		
	int d = delay_ = arr_ts - ts; 

	int delta = delta_;
	delta_ = d;

#ifdef WIN32
#define FUDGE int(300 * 65.536) /* XXX platform specific delay, need to work out on this [dm] */
#else
#define FUDGE int(100 * 65.536)
#endif
        if (adapt_init_ == 0) {
                if (flag) {
                        adapt_init_ = 1;
                        pdelay_ = FUDGE +  delay_ + int(3. * dvar_);
                }
                return;
        }

        d -= delta;
        if (d < 0)
                d = -d;
#define DGAIN (1./16.)
        dvar_ += DGAIN * (double(d) - dvar_);

        if (flag) {
		pdelay_ = FUDGE + delay_ + int(3. * dvar_); 
		if (pending_) {
			/* prepare mbus msg and send it to mbus, if we have to ... */
			sprintf(arg, "%s %d", mbus_->mbus_encode_str(sdes_[RTCP_SDES_CNAME]), pdelay_); 	
			mbus_->mbus_send(mbus_->mbus_audio_addr, "rtp.source.playout", arg, 0);
			pending_ = 0;
			/* set to the max between the audio & video playout delays */
			if (pdelay_ < apdelay_)
#ifndef WIN32 
				pdelay_ = apdelay_;
#else
				pdelay_ = apdelay_;
#endif
			/*printf("act del= %d\n", pdelay_);*/
		}
	}
}


void 
Source::add(u_char *pkt, struct rtphdr* rh, u_char *vh,
                        int len, u_int playout)
{
        PacketData *packet;
        if (free_) {
                packet = free_;
                free_ = free_->next_;
                packet->construct(pkt, rh, vh, len, playout);
        } else {
                packet = new PacketData(pkt, rh, vh, len, playout);
        }

        if (tail_) {
                tail_->next_ = packet;
                tail_ = packet;
        } else {
                head_ = tail_ = packet;
                /* No need to check for RTP_M */
                /* No need to update now_ as it was done by source... */
                schedule();
        }
}


void 
Source::schedule()
{
        if (head_ == 0)
                return;

        u_int playout = head_->playout();

        if (playout <= now_ || elastic_ == 0) {
                timeout();
        } else {
                msched(int((playout - now_) / 65.536));
        }
}


void 
Source::timeout()
{
        u_int start = ntptime();
        /* Process all packets with this playout point */
        u_int playout = head_->playout();
	u_int32_t ts = head_->rtp_hdr()->rh_ts;
        
	do {
                PacketData *pkt = head_;
                head_ = head_->next_;
                if (head_ == 0)
                        tail_ = 0;
		process((struct rtphdr*)pkt->rtp_hdr(), (u_char*)pkt->video_data(), (int)pkt->data_length());
		
                pkt->~PacketData();
                pkt->next_ = free_;
                free_ = pkt;
        } while (head_ && head_->playout() <= playout); // && head_->rtp_hdr()->rh_ts == ts);
        now_ = ntptime();    /* Needed for schedule */

        /* Calculate running average of render time */
        int dt = now_ - start;
        dtskew_ += (dt - int(dtskew_)) >> 6;
	//dtskew_ = .998002*dtskew_ + .75*(dt-dtskew_);
        if (head_)
                schedule();
}


void 
Source::process(struct rtphdr* rh, u_char *bp, int len)
{
        int flags = ntohs(rh->rh_flags);
        int fmt = flags & 0x7f;
        
	if (handler_ == 0)
                handler_ = activate(fmt);
        else if (format_ != fmt)
                handler_ = change_format(fmt);

        int hlen = handler_->hdrlen();
        int cc = len - hlen;
        if (cc < 0) {
                runt(1);
                return;
        }
        if (!mute()) {
                handler_->recv(rh, bp + hlen, cc);
	}
}


u_int32_t 
Source::convert_time(u_int32_t ts)
{
	u_int32_t t = ts;
	t += (ts >> 2) + (ts >> 3) - (ts >> 9) + (ts >> 12);
	return (t);
}


void
Source::recv(u_char *pkt, struct rtphdr* rh, u_char *vh, int cc)
{
	u_int32_t sendtime, when;
        int flags = ntohs(rh->rh_flags);

        u_int32_t ts = rh->rh_ts;
        NTOHL(ts);
	// should get a sr first; we need the rtp<->ntp mapping 
	if (rtp2ntp_ == 0) { 
		delete pkt;
                return;
	}

	now_ = ntptime();
	/* calculate the real send time */ 
	if (ts >= rtp_ctrl_) 
		sendtime = sts_ctrl_ + (((ts - rtp_ctrl_) << 12) / 5625);  
	else
		sendtime = sts_ctrl_ - (((rtp_ctrl_ - ts) << 12) / 5625);
        adapt(now_, sendtime, flags & RTP_M);
        if (adapt_init_ == 0) {
                /* XXX */
                delete pkt;
                return ;
        }

//#define DISP_TIME (server_delay * 65536 / 1000) 

	when = sendtime + pdelay_ - int(dtskew_);	
        if (when < now_) 
              late(1);
	
	/* add the packet to the playout buffer */
        add(pkt, rh, vh, cc, when);
}


SourceManager SourceManager::instance_;

SourceManager::SourceManager() :
	TclObject("srctab"),
	nsources_(0),
	sources_(0),
	clock_(0),
	keep_sites_(0),
	site_drop_time_(0),
	localsrc_(0),
	generator_(0)
{
	memset((char*)hashtab_, 0, sizeof(hashtab_));
}

int SourceManager::command(int argc, const char *const*argv)
{
	Tcl& tcl = Tcl::instance();
	if (argc == 2) {
		if (strcmp(argv[1], "local") == 0) {
			tcl.result(localsrc_->name());
			return (TCL_OK);
		}
		if (strcmp(argv[1], "gen-init") == 0) {
			generator_ = sources_;
			return (TCL_OK);
		}
		if (strcmp(argv[1], "gen-next") == 0) {
			Source* s = generator_;
			if (s != 0) {
				tcl.result(s->name());
				generator_ = s->next_;
			}
			return (TCL_OK);
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "keep-sites") == 0) {
			keep_sites_ = atoi(argv[2]);
			return (TCL_OK);
		}
		if (strcmp(argv[1], "site-drop-time") == 0) {
			site_drop_time_ = atoi(argv[2]);
			return (TCL_OK);
		}
		if (strcmp(argv[1], "delete") == 0) {
			Source* s = (Source*)TclObject::lookup(argv[2]);
			if (s != 0)
				remove(s);
			return (TCL_OK);
		}
	} else if (argc == 4) {
		if (strcmp(argv[1], "create-local") == 0) {
			u_int32_t srcid = strtoul(argv[2], 0, 0);
			Address * local;
			local = Address::alloc(argv[3]);
			srcid = htonl(srcid);
			if (local) init(srcid, *local);
			tcl.result(localsrc_->name());
			return (TCL_OK);
		}
	}
	return (TclObject::command(argc, argv));
}

void SourceManager::remove_from_hashtable(Source* s)
{
	/* delete the source from hash table */
	u_int32_t srcid = s->srcid();
	int h = SHASH(srcid);
	Source** p = &hashtab_[h];
	while (*p != s)
		p = &(*p)->hlink_;
	*p = (*p)->hlink_;
}

void SourceManager::init(u_int32_t localid, Address & localaddr)
{
	/*
	 * create the local object.  remove it from the hash
	 * table since collision resolution changes local id
	 * (we special case detection of our own loopbed back packets)
	 */
	localsrc_ = new Source(localid, localid, localaddr);
	enter(localsrc_);
	remove_from_hashtable(localsrc_);
	/*
	 * hack to prevent local source from turning gray at startup.
	 * we don't need to do a similar thing for external sources,
	 * because they are only created when a packet arrives.
	 */
	localsrc_->lts_ctrl(unixtime());
}

Source* SourceManager::enter(Source* s)
{
	s->next_ = sources_;
	sources_ = s;

	int h = SHASH(s->srcid());
	s->hlink_ = hashtab_[h];
	hashtab_[h] = s;

	++nsources_;

	return (s);
}

Source* SourceManager::consult(u_int32_t srcid)
{
	int h = SHASH(srcid);
	for (Source* s = hashtab_[h]; s != 0; s = s->hlink_) {
		/*XXX pulling these values into variable seems
		  to work around a DEC c++ bug */
		u_int32_t id = s->srcid();
		if (id == srcid)
			return (s);
	}
	return (0);
}


/*
 * Lookup the sources list for the source with the address 'addr'
 * IPV6Q
 */
Source* SourceManager::lookup(u_int32_t addr)
{
	Source *s;
	for (s = sources_; s != 0; s = s->next_) {
		if (s && s!=localsrc_)  //proswrino  (addr == s->addr())
			return s;
	}
	return (0);
}


Source* SourceManager::lookup(u_int32_t srcid, u_int32_t ssrc, Address &addr)
{
	Source* s = consult(srcid);
	if (s == 0) {
		if (srcid == ssrc)
			/*
			 * Heuristic: handle application re-starts 
			 * gracefully.  We have a new source that's
			 * not via a mixer.  Try to find an existing
			 * entry from the same host.
			 */
			s = lookup_duplicate(srcid, addr);

		if (s == 0) {
			s = new Source(srcid, ssrc, *(addr.copy()));
			enter(s);
		}
	}
	return (s);
}

/*
 * Demux data packet to its source table entry.  (We don't want an extra
 * SSRC arg here because CSRC's via a mixer don't have their own data
 * packets.)  If we haven't seen this source yet, allocate it but
 * wait until we see two in-order packets accepting the flow.
 */
Source* SourceManager::demux(u_int32_t srcid, Address &addr, u_int16_t seq)
{
	Source* s = consult(srcid);
	if (s == 0) {
		s = lookup_duplicate(srcid, addr);
		if (s == 0) {
			/* CSRC=SSRC for data stream */
			s = new Source(srcid, srcid, *(addr.copy()));
			enter(s);
		}
		/* it takes two in-seq packets to activate source */
		s->fs(seq);
		s->cs(seq);
		return (0);
	} else {
		/*
		 * check for a srcid conflict or loop:
		 *  - believe the new guy if the old guy said he's done.
		 *  - otherwise, don't believe the new guy if we've heard
		 *    from the old guy 'recently'.
		 */
		if (!(s->addr() == addr)) {
			u_int32_t t = s->lts_done().tv_sec;
			if (t == 0) {
				t = s->lts_data().tv_sec;
				u_int32_t now = unixtime().tv_sec;
				if (t && int(now - t) <= 2)
					return (0);
				t = s->lts_ctrl().tv_sec;
				if (t && int(now - t) <= 30)
					return (0);
			}
			s->addr(addr);
			s->clear_counters();
			s->lost(0);
		}
		if (s->np() == 0 && s->nb() == 0) {
			/*
			 * make sure we get 2 in-seq packets before
			 * accepting source.
			 */
			if ((u_int32_t)((u_int32_t)seq - s->cs() + 31) > 63) {
				s->fs(seq);
				s->cs(seq);
				return (0);
			}
		}
	}
	return (s);
}

/*
 * Try to find an entry in the source table with the same network
 * address (i.e.,  a "duplicate entry") but possibly different srcid.
 * As a side effect, refile the source under the new srcid.
 *
 * The idea here is to gracefully handle sites that restart (with
 * a new srcid).  If we assume that it takes a couple seconds to
 * restart
 *
 */
Source* SourceManager::lookup_duplicate(u_int32_t srcid, Address &addr)
{
	/*XXX - should eventually be conditioned on cname not ipaddr */
	/*
	 * could use hashing here, but this is rarely called.
	 */
	register Source* s;
	for (s = sources_; s != 0; s = s->next_) {
		/*
		 * if addresses match, take old entry if:
		 *  - it sent a 'done', or
		 *  - it hasn't sent any data for 2 seconds and
		 *    and any control for 30 seconds.
		 */
		if (s->addr() == addr) {
			if (s->lts_done().tv_sec != 0)
				break;
			u_int32_t now = unixtime().tv_sec;
			u_int32_t t = s->lts_data().tv_sec;
			if (t == 0 || int(now - t) > 2) {
				t = s->lts_ctrl().tv_sec;
				if (t == 0 || int(now - t) > 30)
					break;
			}
		}
	}
	if (s) {
		remove_from_hashtable(s);
		s->srcid(srcid);
		s->ssrc(srcid);
		int h = SHASH(srcid);
		s->hlink_ = hashtab_[h];
		hashtab_[h] = s;
		s->clear_counters();
		s->lost(0);
	}
	return (s);
}

void SourceManager::remove(Source* s)
{
	--nsources_;

	remove_from_hashtable(s);

	/* delete the source from list */
	Source** p = &sources_;
	while (*p != s)
		p = &(*p)->next_;
	*p = (*p)->next_;

	delete s;

	if (s == generator_)
		generator_ = 0;
}

/*
 * Go through all the sources and see if any should be "grayed out"
 * or removed altogether.  'msgint' is the current "report interval"
 * in ms.
 */
void SourceManager::CheckActiveSources(double msgint)
{
	u_int32_t now = unixtime().tv_sec;
	u_int max_idle = u_int(msgint * (CTRL_IDLE / 1000.));
	if (max_idle == 0)
		max_idle = 1;

	Source* n;
	for (Source* s = sources_; s != 0; s = n) {
		n = s->next_;
		u_int32_t t = s->lts_done().tv_sec;
		if (t != 0) {
			if (keep_sites_)
				s->lost(1);
			else
				remove(s);
			continue;
		}
		t = s->lts_ctrl().tv_sec;
		if (t == 0)
			/*
			 * No session packets?  Probably ivs or nv sender.
			 * Revert to the data time stamp.
			 */
			t = s->lts_data().tv_sec;

		if (u_int(now - t) > max_idle) {
			if (keep_sites_ || site_drop_time_ == 0 ||
			    u_int(now - t) < site_drop_time_)
				s->lost(1);
			else
				remove(s);
		} else
			s->lost(0);
	}
}

int SourceManager::compare(const void* v0, const void* v1)
{
	const Source* x = *(Source**)v0;
	const Source* y = *(Source**)v1;

	const char* yn = y->sdes(RTCP_SDES_NAME);
	if (yn == 0) {
		yn = y->sdes(RTCP_SDES_CNAME);
		if (yn == 0)
			return (-1);
	}
	const char* xn = x->sdes(RTCP_SDES_NAME);
	if (xn == 0) {
		xn = x->sdes(RTCP_SDES_CNAME);
		if (xn == 0)
			return (1);
	}
	return (strcmp(xn, yn));
}

/*
 * Sort the sources by name and format a corresponding list
 * of ascii srcid's in the input buffer.
 */
void SourceManager::sortactive(char* cp) const
{
	static int ntab;
	static Source** srctab;
	if (srctab == 0) {
		ntab = 2 * nsources_;
		if (ntab < 32)
			ntab = 32;
		srctab = new Source*[ntab];
	} else if (ntab < nsources_) {
		ntab = 2 * nsources_;
		delete srctab;
		srctab = new Source*[ntab];
	}
	int n = 0;
	for (Source* s = sources_; s != 0; s = s->next_)
		if (s->handler() != 0)
			/* source is active if it has a PacketHandler */
			srctab[n++] = s;

	if (n == 0) {
		*cp = 0;
		return;
	}
	qsort(srctab, n, sizeof(*srctab), compare);
	for (int i = 0; i < n; ++i) {
		strcpy(cp, srctab[i]->TclObject::name());
		cp += strlen(cp);
		*cp++ = ' ';
	}
	/* nuke trailing space */
	cp[-1] = 0;
}


