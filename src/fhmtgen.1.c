/*====================================================================*
 -  Copyright (C) 2001 Leptonica.  All rights reserved.
 -  This software is distributed in the hope that it will be
 -  useful, but with NO WARRANTY OF ANY KIND.
 -  No author or distributor accepts responsibility to anyone for the
 -  consequences of using this software, or for whether it serves any
 -  particular purpose or works at all, unless he or she says so in
 -  writing.  Everyone is granted permission to copy, modify and
 -  redistribute this source code, for commercial or non-commercial
 -  purposes, with the following restrictions: (1) the origin of this
 -  source code must not be misrepresented; (2) modified versions must
 -  be plainly marked as such; and (3) this notice may not be removed
 -  or altered from any source or modified source distribution.
 *====================================================================*/

/*
 *      Top-level fast hit-miss transform with auto-generated sels
 *
 *            PIX      *pixFHMTGen_*()
 */

#include <stdio.h>
#include "allheaders.h"

static l_int32   NUM_SELS_GENERATED = 6;

static char  *SEL_NAMES[] = {
                             "sel_3hm",
                             "sel_3de",
                             "sel_3ue",
                             "sel_3re",
                             "sel_3le",
                             "sel_sl1"};

/*
 *  pixFHMTGen_*()
 *
 *     Input:  pixd (usual 3 choices: null, == pixs, != pixs)
 *             pixs 
 *             sel name
 *     Return: pixd
 *
 *     Action: hit-miss transform on pixs by the sel
 *     N.B.: the sel must have at least one hit, and it
 *           can have any number of misses.
 */
PIX *
pixFHMTGen_1(PIX    *pixd,
             PIX    *pixs,
             char   *selname)
{
l_int32    i, index, found, w, h, wpls, wpld;
l_uint32  *datad, *datas, *datat;
PIX       *pixt;

    PROCNAME("pixFHMTGen_*");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, pixd);
    if (pixGetDepth(pixs) != 1)
        return (PIX *)ERROR_PTR("pixs must be 1 bpp", procName, pixd);

    found = FALSE;
    for (i = 0; i < NUM_SELS_GENERATED; i++) {
        if (strcmp(selname, SEL_NAMES[i]) == 0) {
            found = TRUE;
            index = i;
            break;
        }
    }
    if (found == FALSE)
        return (PIX *)ERROR_PTR("sel index not found", procName, pixd);

    if (pixd) {
        if (!pixSizesEqual(pixs, pixd))
            return (PIX *)ERROR_PTR("sizes not equal", procName, pixd);
    }
    else {
        if ((pixd = pixCreateTemplate(pixs)) == NULL)
            return (PIX *)ERROR_PTR("pixd not made", procName, NULL);
    }

    wpls = pixGetWpl(pixs);
    wpld = pixGetWpl(pixd);

        /*  The images must be surrounded with ADDED_BORDER white pixels,
         *  that we'll read from.  We fabricate a "proper"
         *  image as the subimage within the border, having the 
         *  following parameters:  */
    w = pixGetWidth(pixs) - 2 * ADDED_BORDER;
    h = pixGetHeight(pixs) - 2 * ADDED_BORDER;
    datas = pixGetData(pixs) + ADDED_BORDER * wpls + ADDED_BORDER / 32;
    datad = pixGetData(pixd) + ADDED_BORDER * wpld + ADDED_BORDER / 32;

    if (pixd == pixs) {  /* need temp image if in-place */
        if ((pixt = pixCopy(NULL, pixs)) == NULL)
            return (PIX *)ERROR_PTR("pixt not made", procName, pixd);
        datat = pixGetData(pixt) + ADDED_BORDER * wpls + ADDED_BORDER / 32;
        fhmtgen_low_1(datad, w, h, wpld, datat, wpls, index);
        pixDestroy(&pixt);
    }
    else {  /* simple and not in-place */
        fhmtgen_low_1(datad, w, h, wpld, datas, wpls, index);
    }

    return pixd;
}

