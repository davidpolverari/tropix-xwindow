/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86pciBus.h,v 3.12 2005/06/03 03:18:31 tsi Exp $ */

/*
 * Copyright (c) 1999-2003 by The XFree86 Project, Inc.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *   1.  Redistributions of source code must retain the above copyright
 *       notice, this list of conditions, and the following disclaimer.
 *
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer
 *       in the documentation and/or other materials provided with the
 *       distribution, and in the same place and form as other copyright,
 *       license and disclaimer information.
 *
 *   3.  The end-user documentation included with the redistribution,
 *       if any, must include the following acknowledgment: "This product
 *       includes software developed by The XFree86 Project, Inc
 *       (http://www.xfree86.org/) and its contributors", in the same
 *       place and form as other third-party acknowledgments.  Alternately,
 *       this acknowledgment may appear in the software itself, in the
 *       same form and location as other such third-party acknowledgments.
 *
 *   4.  Except as contained in this notice, the name of The XFree86
 *       Project, Inc shall not be used in advertising or otherwise to
 *       promote the sale, use or other dealings in this Software without
 *       prior written authorization from The XFree86 Project, Inc.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE XFREE86 PROJECT, INC OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _XF86_PCI_BUS_H
#define _XF86_PCI_BUS_H

#define PCITAG_SPECIAL pciTag(0xFF,0xFF,0xFF)

typedef struct {
    CARD32 command;
    CARD32 base[6];
    CARD32 biosBase;
} pciSave, *pciSavePtr;

typedef struct {
    PCITAG tag;
    CARD32 ctrl;
} pciArg;

typedef struct {
    int busnum;
    int devnum;
    int funcnum;
    pciArg arg;
    xf86AccessRec ioAccess;
    xf86AccessRec io_memAccess;
    xf86AccessRec memAccess;
    pciSave save;
    pciSave restore;
    Bool ctrl;
} pciAccRec, *pciAccPtr;

typedef union {
    CARD16 control;
} pciBridgesSave, *pciBridgesSavePtr;

typedef struct pciBusRec {
    int domain;			/* bridge & bus domain number */
    int brbus, brdev, brfunc;	/* ID of the bridge to this bus */
    /*
     * If primary == -1, then redundant host bridge, else
     * if secondary == -1, then pre-PCI bus, else
     * if primary == secondary, then PCI root bus segment, else
     * PCI non-root bus segment.
     */
    int primary, secondary, subordinate;
    int subclass;		/* bridge type */
    int interface;
    int brcontrol;		/* bridge_control word */
    resPtr preferred_io;	/* I/O range */
    resPtr preferred_mem;	/* non-prefetchable memory range */
    resPtr preferred_pmem;	/* prefetchable memory range */
    resPtr io;			/* for subtractive PCI-PCI bridges */
    resPtr mem;
    resPtr pmem;
    struct pciBusRec *next;
} PciBusRec, *PciBusPtr;

void xf86PciProbe(void);
void ValidatePci(void);
resList GetImplicitPciResources(int entityIndex);
void initPciState(void);
void initPciBusState(void);
void DisablePciAccess(void);
void DisablePciBusAccess(void);
void PciStateEnter(void);
void PciBusStateEnter(void);
void PciStateLeave(void);
void PciBusStateLeave(void);
resPtr ResourceBrokerInitPci(resPtr *osRes);
void pciConvertRange2Host(int entityIndex, resRange *pRange);
void isaConvertRange2Host(resRange *pRange);

extern pciAccPtr * xf86PciAccInfo;

#endif /* _XF86_PCI_BUS_H */
