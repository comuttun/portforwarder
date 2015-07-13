#pragma once

/*
 * odsf.h -- a replication of dprintf.h
 *  http://vision.kuee.kyoto-u.ac.jp/~nob/doc/win32/win32.html
 *  odsf = printf-like OutputDebugString
 */

#include <windows.h>
#include <tchar.h>
#include <stdarg.h>
#include <stdio.h>

#define DPRINTF_MES_LENGTH 256

static void odsf( const _TCHAR * fmt, ... )
{
  _TCHAR buf[DPRINTF_MES_LENGTH];
  va_list ap;
  va_start(ap, fmt);
  _vsntprintf(buf, DPRINTF_MES_LENGTH, fmt, ap);
  va_end(ap);
  OutputDebugString(buf);
}
