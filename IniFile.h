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
 * $Id: IniFile.h,v 1.3 2004/01/17 17:13:07 toh Exp $
 */

#ifndef INIFILE_H
#define INIFILE_H

#define STRING_TRUE TEXT("true")
#define STRING_FALSE TEXT("false")

#define HELP_LINE_HOST \
        TEXT("# RecentHost\t\t\thost\n")
#define HELP_LINE_CONFIGFOLDER \
        TEXT("# ConfigFolder\t\t\tpath_of_your_config_folder\n")
#define HELP_LINE_CONFIGFILE \
        TEXT("# ConfigFile\t\t\tname_of_your_config_file\n")
#define HELP_LINE_AUTOMATICALLYHIDE \
        TEXT("# AutomaticallyHide\t\ttrue/false\n")

#define CONF_RECENTHOST        TEXT("RecentHost")
#define CONF_CONFIGFOLDER      TEXT("ConfigFolder")
#define CONF_CONFIGFILE        TEXT("ConfigFile")
#define CONF_AUTOMATICALLYHIDE TEXT("AutomaticallyHide")

#define DEFAULT_CONFIGFOLDER TEXT("")
#define DEFAULT_CONFIG TEXT("config")
#define DEFAULT_AUTOMATICALLYHIDE FALSE

#endif /* INIFILE_H */
