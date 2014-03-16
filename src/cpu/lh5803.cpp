/**********************************************************/
/* LH5801 CPU emulation                                  */
/**********************************************************/
#include <stdlib.h>
//#include <string.h>

//#include "common.h"
//#include "pcxxxx.h"
#include "lh5803.h"
//#include "Keyb.h"
//#include "Connect.h"

//#include "Debug.h"
//#include "Log.h"
/*
 *TODO:
  LH-5803 is an 8-bit C-MOS CPU, which is an upper versiofi of LH-5801. Thereiore, LH—5803 supports
~most ail LH-5801 machine language instructions, except that the SDP, RDP and OFF instructions of
L H-5801 aperate as a NOP instruction in LH-5803.
*/


void CLH5803::step()
{
    CLH5801::step();
    lh5801.bf=1;
    lh5801.dp=0;
}
