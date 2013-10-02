#ifndef NUT_H
#define NUT_H

#if 0
//        _______________________________
//       +-------------------------------+
//       |  N S I M II  E M U L A T O R  |
//       |             01234             |
//       +-------------------------------+
//       | =ON==|=USER       PRGM=|ALPHA |
//       +--------------___--------------+
// =============================================================================
// nsimII is a simulator for the processor used in the HP-41 (Nut) and in the
// HP Series 10 (Voyager) calculators.
//
// Copyright 1995, 2003 Eric Smith <eric@brouhaha.com>
// Copyright 2005, 2012 Christophe Gottheimer <cgh75015@gmail.com>
//
// nsimII is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License version 2 as published by the Free
// Software Foundation.  Note that I am not granting permission to redistribute
// or modify NSIM under the terms of any later version of the General Public
// License.
//
// This program is distributed in the hope that it will be useful (or at least
// amusing), but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program (in the file "COPYING"); if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// =============================================================================
//       | [ / ]   [ 0 ]   [ . ]   [R/S] |
//       |_______________________________|
//       +_HP-41_NUT_EMULATOR_____NSIMII_+
//

#include "cpu.h"

extern char ascii_charmap[];

#define PAGE_SIZE 4096
#define MAX_ROM 65536


typedef UINT32 uint;       // 32-bit unsigned

#define WSIZE 14
typedef UINT8 digit;
typedef digit reg [WSIZE];

#define P 0
#define Q 1

typedef struct
{
  reg    a, b, c, m, n;

  digit  p, q, *pt;

#define SSIZE 14
  bool   s[SSIZE];
  digit  g[2];

  uchar  fo;

  uchar  arith_base;  // 10 or 16
  bool   carry, prev_carry;

  bool   awake;

  bool   key_flag;
  uchar  key_buf;

  int    pc, prev_pc;

#define STACK_DEPTH 4
  int          stack[STACK_DEPTH];
  int    stack_j;

  uint   cycle;

  struct {
         char    *str;
         int      bits;
         void    *addr;
  }      regs[15];

} Cnut_state;

class Cnut : public CCPU
{
public:

 Cnut_state *nut;

#ifdef AUTO_POWER_OFF
#define DISPLAY_TIMEOUT 66000 // nominal 11 minutes
extern int display_timer;  // display time out in 1/100 of seconds
#endif // AUTO_POWER_OFF

#define MAX_PFAD 256

int    pf_addr;
bool   pf_exists[ MAX_PFAD ];
void (*save_fcn[ MAX_PFAD ])( FILE *f, char *prefix );
bool (*load_fcn[ MAX_PFAD ])( char *buf );

void (*op_fcn[ 1024 ])( int );

#define MAX_PER     16

int    peraddr;
void (*op_selper_fcn[ MAX_PER ])(int);

bool parse_hex( char **buf, int *v, int d );
bool parse_reg( char *buf, digit *r, int d );
void write_reg( FILE *f, digit *r, int d );

void reg_copy( reg dest, reg src );
void reg_zero( reg dest );

#define make_code41(w)   ( ((w & 0xFF) << 8) | ((w >> 8) &  0x3) )
#define unmake_code41(w) ( ((w &  0x3) << 8) | ((w >> 8) & 0xFF) )

#if 0
extern short encode_char( int ascii, int punct );
extern char decode_char( int char41, int *punct );
#endif
short get_code_with_bank( int address, int bank );
short get_code( int address );
void put_code( int a, short w );
void put_code_with_bank( int a, int b, short w );
int get_active_bank( int p );
void set_active_bank( int p, int b );
void reset_bank( void );
void select_bank (int a, int b);
void show_trace( void );
void handle_keyboard( void );
void register_selper_ops( int    per,
                                 void (*perfcn)( int ),
                                 void (*take)( void ),
                                 void (*give)( void ),
                                 bool (*has)( void ) );
void unregister_selper_ops( int per );
void register_pf_ops( int    pf,
                             void (*read_pf)( int ),
                             void (*write_pf)( int ),
                             void (*write_data_pf)( void ),
                             bool (*load_pf)( char * ),
                             void (*save_pf)( FILE *, char* ) );
void unregister_pf_ops( int pf );
void init_pf( void );

void bad_op( int opcode );
void register_op( int opcode, void (*op)( int ) );
void unregister_op( int opcode );

void init_ops( void );

void nut_push( int addr );

void init_cpu_registers( void );

void nut_awake( void );
void nut_sim( void );

void open_nut( void *pmod );
void close_nut( void *pmod );
void poll_nut( void *pmod );
void notify_nut_poweron( void );


#endif

#endif // NUT_H
