/**
 * This source code is a part of Metathrone game project. 
 * (c) Perfect Play 2003.
 */

#define _WIN32_WINDOWS 0x0410
#define WINVER 0x0400

#pragma warning(disable:4786)
#include <cassert>
#include <cstdio>
#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <list>
#include <string>
#include <vector>
#include <cstdarg>
#include <windows.h>
#include <windef.h>
#include <winuser.h>
#include <windowsx.h>

// renderware includes
#include "rwcore.h"
#include "rwplcore.h"
#include "rwplcore.h"
#include "rpworld.h"
#include "rplogo.h"
#include "rtpitexd.h"
#include "rtcharse.h"
#include "rpcollis.h"
#include "rpmatfx.h"
#include "rpanisot.h"
#include "rpusrdat.h"
#include "rt2d.h"
#include "rtbmp.h"
#include "rttiff.h"
#include "rtpng.h"
#include "rtanim.h"
#include "rphanim.h"
#include "rpskin.h"
#include "rtpick.h"
#include "rtintsec.h"
#include "rtquat.h"
#include "rttoc.h"
#include "rprandom.h"
#include "rppvs.h"
#include "rpuvanim.h"
#include "rpptank.h"
#include "rpprtstd.h"
#include "rpprtadv.h"
#include "rpspline.h"
#include "rtray.h"
#include "rpnormmap.h"
#include "rtnormmap.h"
#include "rpltmap.h"
#include "rtltmap.h"
#include "rpmipkl.h"
