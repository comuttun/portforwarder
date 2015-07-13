/*
 * Copyright (C) 2003 Fujio Nobori (toh@fuji-climb.org)
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
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: FindFile.cpp,v 1.3 2006/03/08 13:46:43 ozawa Exp $
 */

#include <stdafx.h>

/* Shows open-file dialog and gets file name. */
BOOL FindFile(HWND owner,
              LPTSTR title,
              LPTSTR filter,
              LPTSTR dir,
              LPTSTR file,
              int length)
{
    OPENFILENAME fileInfo;

    memset((LPVOID)&fileInfo, 0, sizeof(OPENFILENAME));

#if WINVER > 0x0400  /* Win2000 or above */
    fileInfo.lStructSize = OPENFILENAME_SIZE_VERSION_400;
                  /* for backward compatibilities to Win9x/NT 4.0 */
#else  /* Win9x/NT 4.0 */
    fileInfo.lStructSize = sizeof(OPENFILENAME);
#endif

    fileInfo.hwndOwner = owner;
    fileInfo.lpstrFilter = filter;
    fileInfo.lpstrFile = file;
    fileInfo.nMaxFile = length;
    fileInfo.lpstrInitialDir = dir;
    fileInfo.lpstrTitle = title;
    fileInfo.Flags = OFN_HIDEREADONLY | OFN_LONGNAMES;

    return GetOpenFileName(&fileInfo);
}
