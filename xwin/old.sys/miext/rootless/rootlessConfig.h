/*
 * Platform specific rootless configuration
 */
/*
 * Copyright (c) 2003 Torrey T. Lyons. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE ABOVE LISTED COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name(s) of the above copyright
 * holders shall not be used in advertising or otherwise to promote the sale,
 * use or other dealings in this Software without prior written authorization.
 */
/* $XFree86: xc/programs/Xserver/miext/rootless/rootlessConfig.h,v 1.3 2005/10/12 19:54:27 tsi Exp $ */

#ifndef _ROOTLESSCONFIG_H
#define _ROOTLESSCONFIG_H

#ifdef __DARWIN__

# define ROOTLESS_ACCEL TRUE
# define ROOTLESS_GLOBAL_COORDS TRUE
# define ROOTLESS_PROTECT_ALPHA TRUE
# define ROOTLESS_REDISPLAY_DELAY 10
# define ROOTLESS_RESIZE_GRAVITY TRUE
# undef  ROOTLESS_TRACK_DAMAGE

#endif /* __DARWIN__ */

/* Bit mask for alpha channel with a particular number of bits per
   pixel. Note that we only care for 32bpp data. Mac OS X uses planar
   alpha for 16bpp. */
#define RootlessAlphaMask(bpp) ((bpp) == 32 ? 0xFF000000 : 0)

#endif /* _ROOTLESSCONFIG_H */
