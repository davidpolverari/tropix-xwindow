/*
 * Copyright (c) 2005 ASPEED Technology Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the authors not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The authors makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THE AUTHORS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/aspeed/ast_2dtool.c,v 1.1 2006/04/14 00:50:38 dawes Exp $ */

#include "xf86.h"
#include "xf86_ansic.h"
#include "compiler.h"

/* Driver specific headers */
#include "ast.h"

#ifdef	Accel_2D

static Bool
bInitCMDQInfo(ScrnInfoPtr pScrn, ASTRecPtr pAST)
{
    pAST->CMDQInfo.pjCmdQBasePort    = pAST->MMIOVirtualAddr+ 0x8044;
    pAST->CMDQInfo.pjWritePort       = pAST->MMIOVirtualAddr+ 0x8048;
    pAST->CMDQInfo.pjReadPort        = pAST->MMIOVirtualAddr+ 0x804C;
    pAST->CMDQInfo.pjEngStatePort    = pAST->MMIOVirtualAddr+ 0x804C;

    /* CMDQ mode Init */
    if (!pAST->MMIO2D) {
	ScreenPtr pScreen = screenInfo.screens[pScrn->scrnIndex];

	pAST->CMDQInfo.ulCMDQType = VM_CMD_QUEUE;

	do {
	    pAST->pCMDQPtr = xf86AllocateOffscreenLinear (pScreen, pAST->CMDQInfo.ulCMDQSize, 8, NULL, NULL, NULL);
	    if (pAST->pCMDQPtr)
		break;
	    pAST->CMDQInfo.ulCMDQSize >>= 1;
	} while (pAST->CMDQInfo.ulCMDQSize >= MIN_CMDQ_SIZE);

	if (pAST->pCMDQPtr)
	{
	    pAST->CMDQInfo.ulCMDQOffsetAddr  = pAST->pCMDQPtr->offset*((pScrn->bitsPerPixel + 1) / 8);
	    pAST->CMDQInfo.pjCMDQVirtualAddr = pAST->FBVirtualAddr + pAST->CMDQInfo.ulCMDQOffsetAddr;

	    pAST->CMDQInfo.ulCurCMDQueueLen = pAST->CMDQInfo.ulCMDQSize - CMD_QUEUE_GUARD_BAND;
	    pAST->CMDQInfo.ulCMDQMask = pAST->CMDQInfo.ulCMDQSize - 1 ;
	}
	else
	{
	    xf86DrvMsg(pScrn->scrnIndex, X_ERROR,"Allocate CMDQ failed \n");
	    pAST->MMIO2D = TRUE;		/* set to MMIO mode if CMDQ allocate failed */
	}
    }

    /* MMIO mode init */
    if (pAST->MMIO2D) {
	pAST->CMDQInfo.ulCMDQType = VM_CMD_MMIO;
    }

    return (TRUE);
}

static Bool
bEnableCMDQ(ScrnInfoPtr pScrn, ASTRecPtr pAST)
{
    ULONG ulVMCmdQBasePort = 0;

    vASTWaitEngIdle(pScrn, pAST);

    /* set DBG Select Info */
    if (pAST->DBGSelect)
    {
	*(ULONG *) (pAST->MMIOVirtualAddr + 0x804C) = (ULONG) (pAST->DBGSelect);
    }

    /* set CMDQ base */
    switch (pAST->CMDQInfo.ulCMDQType)
    {
    case VM_CMD_QUEUE:
	ulVMCmdQBasePort  = (pAST->CMDQInfo.ulCMDQOffsetAddr - 0) >> 3;

	/* set CMDQ Threshold */
	ulVMCmdQBasePort |= 0xF0000000;

	/* set CMDQ Size */
	switch (pAST->CMDQInfo.ulCMDQSize)
	{
	case CMD_QUEUE_SIZE_256K:
	    ulVMCmdQBasePort |= 0x00000000;
	    break;

	case CMD_QUEUE_SIZE_512K:
	    ulVMCmdQBasePort |= 0x04000000;
	    break;

	case CMD_QUEUE_SIZE_1M:
	    ulVMCmdQBasePort |= 0x08000000;
	    break;

	case CMD_QUEUE_SIZE_2M:
	    ulVMCmdQBasePort |= 0x0C000000;
	    break;

	default:
	    return(FALSE);
	    break;
	}

	*(ULONG *) (pAST->CMDQInfo.pjCmdQBasePort) = ulVMCmdQBasePort;
	pAST->CMDQInfo.ulWritePointer = *(ULONG *) (pAST->CMDQInfo.pjWritePort);
	break;

    case VM_CMD_MMIO:
	/* set CMDQ Threshold */
	ulVMCmdQBasePort |= 0xF0000000;

	ulVMCmdQBasePort |= 0x02000000;			/* MMIO mode */
	*(ULONG *) (pAST->CMDQInfo.pjCmdQBasePort) = ulVMCmdQBasePort;
	break;

    default:
	return (FALSE);
	break;
    }

    return (TRUE);
}

Bool
bASTEnable2D(ScrnInfoPtr pScrn, ASTRecPtr pAST)
{
    SetIndexRegMask(CRTC_PORT, 0xA4, 0xFE, 0x01);		/* enable 2D */

    if (!bInitCMDQInfo(pScrn, pAST))
    {
	vASTDisable2D(pScrn, pAST);
	return (FALSE);
    }

    if (!bEnableCMDQ(pScrn, pAST))
    {
	vASTDisable2D(pScrn, pAST);
	return (FALSE);
    }

    return (TRUE);
}

void
vASTDisable2D(ScrnInfoPtr pScrn, ASTRecPtr pAST)
{

    vASTWaitEngIdle(pScrn, pAST);
    vASTWaitEngIdle(pScrn, pAST);

    SetIndexRegMask(CRTC_PORT, 0xA4, 0xFE, 0x00);

}


void
vASTWaitEngIdle(ScrnInfoPtr pScrn, ASTRecPtr pAST)
{
    ULONG ulEngState, ulEngState2;
    UCHAR jReg;
    ULONG ulEngCheckSetting;

    if (pAST->MMIO2D)
	ulEngCheckSetting = 0x10000000;
    else
	ulEngCheckSetting = 0x80000000;

    /* 2D disable if 0xA4 D[0] = 1 */
    GetIndexRegMask(CRTC_PORT, 0xA4, 0x01, jReg);
    if (!jReg) goto Exit_vWaitEngIdle;

    /* 2D not work if in std. mode */
    GetIndexRegMask(CRTC_PORT, 0xA3, 0x0F, jReg);
    if (!jReg) goto Exit_vWaitEngIdle;

    do
    {
	ulEngState = (*(volatile ULONG *)(pAST->CMDQInfo.pjEngStatePort)) & 0xFFFC0000;
	ulEngState2 = (*(volatile ULONG *)(pAST->CMDQInfo.pjEngStatePort)) & 0xFFFC0000;
	ulEngState2 = (*(volatile ULONG *)(pAST->CMDQInfo.pjEngStatePort)) & 0xFFFC0000;
	ulEngState2 = (*(volatile ULONG *)(pAST->CMDQInfo.pjEngStatePort)) & 0xFFFC0000;
	ulEngState2 = (*(volatile ULONG *)(pAST->CMDQInfo.pjEngStatePort)) & 0xFFFC0000;
	ulEngState2 = (*(volatile ULONG *)(pAST->CMDQInfo.pjEngStatePort)) & 0xFFFC0000;

    } while ((ulEngState & ulEngCheckSetting) || (ulEngState != ulEngState2));

Exit_vWaitEngIdle:
    ;
}

/* ULONG ulGetCMDQLength() */
static __inline ULONG ulGetCMDQLength(ASTRecPtr pAST, ULONG ulWritePointer, ULONG ulCMDQMask)
{
    ULONG ulReadPointer, ulReadPointer2;

    do {
	ulReadPointer  = *((volatile ULONG *)(pAST->CMDQInfo.pjReadPort)) & 0x0003FFFF;
	ulReadPointer2 = *((volatile ULONG *)(pAST->CMDQInfo.pjReadPort)) & 0x0003FFFF;
	ulReadPointer2 = *((volatile ULONG *)(pAST->CMDQInfo.pjReadPort)) & 0x0003FFFF;
	ulReadPointer2 = *((volatile ULONG *)(pAST->CMDQInfo.pjReadPort)) & 0x0003FFFF;
	ulReadPointer2 = *((volatile ULONG *)(pAST->CMDQInfo.pjReadPort)) & 0x0003FFFF;
	ulReadPointer2 = *((volatile ULONG *)(pAST->CMDQInfo.pjReadPort)) & 0x0003FFFF;
     } while (ulReadPointer != ulReadPointer2);

    return ((ulReadPointer << 3) - ulWritePointer - CMD_QUEUE_GUARD_BAND) & ulCMDQMask;
}

UCHAR *pjASTRequestCMDQ(
ASTRecPtr pAST, ULONG   ulDataLen)
{
    UCHAR   *pjBuffer;
    ULONG   i, ulWritePointer, ulCMDQMask, ulCurCMDQLen, ulContinueCMDQLen;

    ulWritePointer = pAST->CMDQInfo.ulWritePointer;
    ulContinueCMDQLen = pAST->CMDQInfo.ulCMDQSize - ulWritePointer;
    ulCMDQMask = pAST->CMDQInfo.ulCMDQMask;

    if (ulContinueCMDQLen >= ulDataLen)
    {
	/* Get CMDQ Buffer */
	if (pAST->CMDQInfo.ulCurCMDQueueLen >= ulDataLen)
	{
		;
	}
	else
	{

	    do
	    {
		ulCurCMDQLen = ulGetCMDQLength(pAST, ulWritePointer, ulCMDQMask);
	    } while (ulCurCMDQLen < ulDataLen);

	    pAST->CMDQInfo.ulCurCMDQueueLen = ulCurCMDQLen;

	}

	pjBuffer = pAST->CMDQInfo.pjCMDQVirtualAddr + ulWritePointer;
	pAST->CMDQInfo.ulCurCMDQueueLen -= ulDataLen;
	pAST->CMDQInfo.ulWritePointer = (ulWritePointer + ulDataLen) & ulCMDQMask;
	return pjBuffer;
    }
    else
    {

	/* Fill NULL CMD to the last of the CMDQ */
	if (pAST->CMDQInfo.ulCurCMDQueueLen >= ulContinueCMDQLen)
	{
		;
	}
	else
	{

	    do
	    {
		ulCurCMDQLen = ulGetCMDQLength(pAST, ulWritePointer, ulCMDQMask);
	    } while (ulCurCMDQLen < ulContinueCMDQLen);

	    pAST->CMDQInfo.ulCurCMDQueueLen = ulCurCMDQLen;

	}

	pjBuffer = pAST->CMDQInfo.pjCMDQVirtualAddr + ulWritePointer;
	for (i = 0; i<ulContinueCMDQLen/8; i++, pjBuffer+=8)
	{
	    *(ULONG *)pjBuffer = (ULONG) PKT_NULL_CMD;
	    *(ULONG *) (pjBuffer+4) = 0;

	}
	pAST->CMDQInfo.ulCurCMDQueueLen -= ulContinueCMDQLen;
	pAST->CMDQInfo.ulWritePointer = ulWritePointer = 0;

	/* Get CMDQ Buffer */
	if (pAST->CMDQInfo.ulCurCMDQueueLen >= ulDataLen)
	{
		;
	}
	else
	{

	    do
	    {
		ulCurCMDQLen = ulGetCMDQLength(pAST, ulWritePointer, ulCMDQMask);
	    } while (ulCurCMDQLen < ulDataLen);

	    pAST->CMDQInfo.ulCurCMDQueueLen = ulCurCMDQLen;

	}

	pAST->CMDQInfo.ulCurCMDQueueLen -= ulDataLen;
	pjBuffer = pAST->CMDQInfo.pjCMDQVirtualAddr + ulWritePointer;
	pAST->CMDQInfo.ulWritePointer = (ulWritePointer + ulDataLen) & ulCMDQMask;
	return pjBuffer;

    }

} /* end of pjRequestCmdQ() */

Bool bASTGetLineTerm(_LINEInfo *LineInfo, LINEPARAM *dsLineParam)
{
    LONG GAbsX, GAbsY, GXInc, GYInc, GXMajor;
    LONG MM, mm, Error0, K1, K2;

    /* Init */
#ifdef LONG64
    GAbsX = abs (LineInfo->X1 - LineInfo->X2);
    GAbsY = abs (LineInfo->Y1 - LineInfo->Y2);
#else
    GAbsX = labs (LineInfo->X1 - LineInfo->X2);
    GAbsY = labs (LineInfo->Y1 - LineInfo->Y2);
#endif

    GXInc = (LineInfo->X1 < LineInfo->X2) ? 1:0;
    GYInc = (LineInfo->Y1 < LineInfo->Y2) ? 1:0;
    GXMajor = (GAbsX >= GAbsY) ? 1:0;

    /* Calculate */
    if (GXMajor)
    {
        MM = GAbsX;
        mm = GAbsY;	        	
    }
    else
    {
        MM = GAbsY;
        mm = GAbsX;	        	    	
    }

    Error0 = (signed) (2*mm - MM);

    K1 = 2* mm;
    K2 = (signed) (2*mm - 2*MM);

    /*save the Param to dsLineParam */
    dsLineParam->dsLineX = (USHORT) LineInfo->X1;
    dsLineParam->dsLineY = (USHORT) LineInfo->Y1;
    dsLineParam->dsLineWidth = (USHORT) MM;
    dsLineParam->dwErrorTerm = (ULONG) Error0;
    dsLineParam->dwK1Term = K1;
    dsLineParam->dwK2Term = K2;

    dsLineParam->dwLineAttributes = 0;
    if (GXMajor) dsLineParam->dwLineAttributes |= LINEPARAM_XM;
    if (!GXInc) dsLineParam->dwLineAttributes |= LINEPARAM_X_DEC;
    if (!GYInc) dsLineParam->dwLineAttributes |= LINEPARAM_Y_DEC;

    return(TRUE);

}

#endif	/* end of Accel_2D */
