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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "nut.h"

//#include "util.h"
//#include "charmap.h"
//#include "mfrm.h"
//#include "nut.h"
//#include "asm.h"
//#include "disasm.h"
//#include "page.h"
//#include "module.h"
//#include "fat.h"
//#include "cardrdr.h"
//#include "display.h"
//#include "phineas.h"
//#include "printer.h"
//#include "wand.h"
//#include "hpil.h"
//#include "xio.h"
//#include "nsim.h"
//#include "focal.h"

#define DISASM2( inst, fmt, ... )
#define DISASM( fmt, ... ) DISASM2( {}, fmt, ## __VA_ARGS__ )


// map from high opcode bits to register index
int tmap[] = { 3, 4, 5, 10, 8, 6, 11, -1, 2, 9, 7, 13, 1, 12, 0, -1 };

// map from register index to high opcode bits
int itmap[] = { 0xE, 0xC, 0x8, 0x0, 0x1, 0x2, 0x5, 0xA, 0x4, 0x9, 0x3, 0x6, 0xD, 0xB };

char *ext_flagmap[] = { "?PF\t3", "?PF\t4", "?EDAV",  "?ORAV",
                               "?FRAV",  "?IFCR",  "?TFAIL", "#1EC",
                               "?WNDB",  "?FRNS",  "?SRQR",  "?SERV",
                               "?CRDR",  "?ALM",   "?PBSY",  "#3EC" };

char *fieldmap[] = { "PT", "X", "WPT", "ALL", "PQ", "XS", "M", "S" };

char regmap[] = "TZYXLMNOPQtabcde";

Cnut::Cnut::Cnut()
{

nut.regs = {
       { "a",         56, (void*)(&nut.a), },
       { "b",         56, (void*)(&nut.b), },
       { "c",         56, (void*)(&nut.c), },
       { "m",         56, (void*)(&nut.m), },
       { "n",         56, (void*)(&nut.n), },
       { "g",          8, (void*)(&nut.g), },
       { "p",          4, (void*)(&nut.p), },
       { "q",          4, (void*)(&nut.q), },
       { "pc",        16, (void*)(&nut.pc), },
       { "stk[0]",    16, (void*)(&nut.stack[0]), },
       { "stk[1]",    16, (void*)(&nut.stack[1]), },
       { "stk[2]",    16, (void*)(&nut.stack[2]), },
       { "stk[3]",    16, (void*)(&nut.stack[3]), },
       { "carry",      1, (void*)(&nut.carry), },
       { "stat",   SSIZE, (void*)(&nut.s), },
    };

}

int display_timer;

void (*op_fcn[ 1024 ])( int );

int  pf_addr;
bool pf_exists[ MAX_PFAD ];

void (*rd_n_fcn[ MAX_PFAD ])( int );
void (*wr_n_fcn[ MAX_PFAD ])( int );
void (*wr_fcn[ MAX_PFAD ])( void );

void (*save_fcn[ MAX_PFAD ])( FILE *f, char *prefix );
bool (*load_fcn[ MAX_PFAD ])( char *buf );

int    peraddr = -1;
void (*op_selper_fcn[ MAX_PER ])(int);

void (*op_selper_take_fcn[ MAX_PER ])( void );
void (*op_selper_give_fcn[ MAX_PER ])( void );
static bool (*op_selper_has_fcn[ MAX_PER ])( void );

#if 0
char decode_char( int char41, int *punct )
{
  if( punct )
    *punct = "\0.:,"[(char41 & 0xC0) >> 6];
  return lcd_charmap[(char41 & 0x3F) | (punct ? ((char41 & 0x100) >> 2) : char41 & 0x7F)];
}

short encode_char( int ascii, int punct )
{
  short char41;
  char *pu, *lcdpu;

  if( 0x40 == ( ascii & 0xE0 ) )
    char41 = ascii & 0x01F;
  else if( 0x20 == ( ascii & 0xE0 ) )
    char41 = ascii & 0x03F;
  else if( (0x60 <= ascii) && (0x7f >= ascii) )
    char41 = ((ascii & 0x1F) | 0x100);
  else
    char41 = 0;
  if( (lcd_charmap[ (char41 & 0x3F) | ((char41 & 0x100) >> 2)] != ascii) )
    {
    Ooops( "Bad character '%c' (%02X) -> encode %03X\n", ascii, ascii, punct );
    }
  if( !punct )
    char41 = (char41 & 0x3F) | ((char41 & 0x100) >> 2);
  else if( (pu = strchr( (lcdpu = ".:,"), punct )) )
    char41 |= (pu + 1 - lcdpu) << 6;
  return( char41 );
}
#endif

short get_code_with_bank( int a, int b )
{
  int   p = a/PAGE_SIZE;
  short w = 0;

  if( nsim.code[p][b] )
    w = nsim.code[p][b][a & 0xFFF];
  return( unmake_code41( w ) );
}

short Cnut::get_code( int a )
{
  return get_code_with_bank( a, nsim.activebank[a / PAGE_SIZE] );
}

void Cnut::put_code_with_bank( int a, int b, short w )
{
  int p = a/PAGE_SIZE;

  if( nsim.code[p][b] )
    {
    if( nsim.atpage[p][b] && (nsim.atpage[p][b]->hw->bits & HW_RAM) )
      nsim.code[p][b][a & 0xFFF] = make_code41( w );
    else
      Ooops( "%04X/%X : Code %03X in a ROM page... Not written\n", a, b, w );
    }
  else
    Ooops( "Putting code on an unexisting page %X[ %04X/%X %03X ]\n", p, a, b, w );
}

void Cnut::put_code( int a, short w )
{
  put_code_with_bank( a, nsim.activebank[a / PAGE_SIZE], w );
}

int Cnut::get_active_bank( int p )
{
  return nsim.activebank[p];
}

void Cnut::set_active_bank( int p, int b )
{
  nsim.activebank[p] = nsim.atpage[p][b] ? b : 0;
}

void Cnut::reset_bank( void )
{
  memset( nsim.activebank, 0, sizeof(nsim.activebank) );
}

void Cnut::select_bank( int a, int b )
{
  // change active bank only if requested bank actually exists
  int p = a / PAGE_SIZE;

  if( (nsim.atpage[p][0]->hw->bits & HW_WWRMBX) && (b != 3) )
    {
    // RamBox -> ENBANK1 : All pages in bank 0 selected
    //           ENBANK2 : All odd pages in bank 1 selected
    //           ENBANK3 : All even pages in bank 1 selected
    //           ENBANK4 : Do nothing ?
    int im;

    for( im = 0; im < MAX_PAGE; im++ )
      {
      if( nsim.atpage[im][0] && (nsim.atpage[im][0]->hw->bits & HW_WWRMBX) )
        {
        switch( b )
          {
          case 0: // ENBANK1
                  nsim.activebank[im] = 0;
                  break;
          case 1: // ENBANK2
          case 2: // ENBANK3
                  if( (im & ( b == 1)) || ((b == 2) && !(im & 1)) )
                    {
                    nsim.activebank[im] = nsim.atpage[im][1] ? 1 : 0;
                    }
                  break;
          }
        }
      }
    }
  else if( nsim.atpage[p & 0xE][b] || nsim.atpage[p | 0x1][b] )
    {
    nsim.activebank[p & 0xE] = nsim.atpage[p & 0xE][b] ? b : 0;
    nsim.activebank[p | 0x1] = nsim.atpage[p | 0x1][b] ? b : 0;
    }
  else if( nsim.atpage[5][1] && ((3 == p) || (5 == p)) )
    {
    // Specific to page 3 and 5 (CX). It seems that ENBANK1 in
    // page 3 or 5 change the active bank 5
    nsim.activebank[5] = b;
    }
  else
    Ooops( "No page %X bank %X <== %04x\n", p, b, a );
}

void Cnut::print_reg( reg r )
{
  int i;

  for( i = WSIZE - 1; i >= 0; i-- )
    printf( "%X", r[i] );
}


void Cnut::print_stat( void )
{
  int i;

  for( i = 0; i < SSIZE; i++)
    printf( nut.s[i] ? "%X" : ".", i);
}


void Cnut::show_trace( void )
{
  int is;
  int opc = nut.pc;

  printf( "CYCLE %5d  ", nut.cycle );
  printf( "%c=%X  ", (nut.pt == & nut.p) ? 'P' : 'p', nut.p );
  printf( "%c=%X  ", (nut.pt == & nut.q) ? 'Q' : 'q', nut.q );
  printf( "%s (%d)  ", nut.carry ?"CARRY" : "carry", nut.carry );
  printf( "STAT=" );
  print_stat();
  printf( "\n" );
  printf( "  A=" );
  print_reg( nut.a );
  printf( "  B=");
  print_reg( nut.b );
  printf( "  C=");
  print_reg( nut.c );
  printf( "\n");
  printf( "  M=" );
  print_reg( nut.m );
  printf( "  N=" );
  print_reg( nut.n );
  printf( "  G=%X%X", nut.g[0] & 0xF, nut.g[1] & 0xF );
  printf( "\n");
  printf( "  PC=%04X ", nut.pc );
  for( is = 0; is < STACK_DEPTH; is++ )
     printf( " STK[%X]=%04X", is, nut.stack[is] );
  printf( "\n" );

  disasm_inst( nut.pc, get_active_bank(nut.pc / PAGE_SIZE), -1 );
  if( opc != nut.pc )
    Ooops_FATAL(1,"PC CHANGED... PC=%04X OLD PC=%04X\n", nut.pc, opc);

  printf( "\n" );
  show_display();
}




void Cnut::reg_copy( reg dest, reg src )
{
  int i;

  for( i = 0; i < WSIZE; i++ )
    dest[i] = src[i];
}

void Cnut::reg_zero( reg dest )
{
  int i;

  for( i = 0; i < WSIZE; i++ )
    dest[i] = 0;
}

void Cnut::bad_op( int opcode )
{
  DISASM( "#%03X\t; Illegal opcode", opcode );
  Ooops( "%04X/%X ILLEGAL OPCODE %03X\n",
         disasm.is ? disasm.pc : nut.prev_pc,
         get_active_bank((disasm.is ? disasm.pc : nut.prev_pc) / PAGE_SIZE),
         opcode );
  dbg.run = dbg.step = dbg.next = 0;
}

digit Cnut::do_add( digit x, digit y )
{
  int res;

  res = x + y + nut.carry;
  if( res >= nut.arith_base )
    {
    res -= nut.arith_base;
    nut.carry = 1;
    }
  else
    nut.carry = 0;
  return( res );
}

digit Cnut::do_sub( digit x, digit y )
{
  int res;

  res = (x - y) - nut.carry;
  if( res < 0 )
    {
    res += nut.arith_base;
    nut.carry = 1;
    }
  else
    nut.carry = 0;
  return( res );
}

void Cnut::op_arith( int opcode )
{
  uchar op, field;
  int   first, last;
  int   temp;
  int   i;

  char *disasm_str[] =
  {
    "A=0",   "B=0",   "C=0",   "A<>B",  "B=A",   "A<>C",   "C=B",   "B<>C",
    "A=C",   "A=A+B", "A=A+C", "A=A+1", "A=A-B", "A=A-1",  "A=A-C", "C=C+C",
    "C=A+C", "C=C+1", "C=A-C", "C=C-1", "C=-C",  "C=-C-1", "?B#0",  "?C#0",
    "?A<C",  "?A<B",  "?A#0",  "?A#C",  "ASR",   "BSR",    "CSR",   "ASL",
  };

  op = opcode >> 5;
  field = (opcode >> 2) & 7;

  DISASM( "%s\t%s", disasm_str[op], fieldmap[field] );

  switch(field)
    {
    case 0:  // P
         first = (*nut.pt);
         last = (*nut.pt);
         break;
    case 1:  // X
         first = 0;
         last = 2;
         break;
    case 2:  // WP
         first = 0;
         last = (*nut.pt);
         break;
    case 3:  // w
         first = 0;
         last = WSIZE - 1;
         break;
    case 4:  // PQ
         first = nut.p;
         last = nut.q;
         if( first > last )
           last = WSIZE - 1;
         break;
    case 5:  // XS
         first = 2;
         last = 2;
         break;
    case 6:  // M
         first = 3;
         last = WSIZE - 2;
         break;
    case 7:  // S
         first = WSIZE - 1;
         last = WSIZE - 1;
         break;
    }

  switch(op)
    {
    case 0x00:  // A=0
      for( i = first; i <= last; i++ )
        nut.a[i] = 0;
      break;
    case 0x01:  // B=0
      for( i = first; i <= last; i++ )
        nut.b[i] = 0;
      break;
    case 0x02:  // C=0
      for( i = first; i <= last; i++ )
        nut.c[i] = 0;
      break;
    case 0x03:  // A<>B
      for( i = first; i <= last; i++ )
        { temp = nut.b[i]; nut.b[i] = nut.a[i]; nut.a[i] = temp; }
      break;
    case 0x04:  // B=A
      for( i = first; i <= last; i++ )
        nut.b[i] = nut.a[i];
      break;
    case 0x05:  // A<>C
      for( i = first; i <= last; i++ )
        { temp = nut.c[i]; nut.c[i] = nut.a[i]; nut.a[i] = temp; }
      break;
    case 0x06:  // C=B
      for( i = first; i <= last; i++ )
        nut.c[i] = nut.b[i];
      break;
    case 0x07:  // B<>C
      for( i = first; i <= last; i++ )
        { temp = nut.c[i]; nut.c[i] = nut.b[i]; nut.b[i] = temp; }
      break;
    case 0x08:  // A=C
      for( i = first; i <= last; i++ )
        nut.a[i] = nut.c[i];
      break;
    case 0x09:  // A=A+B
      for( i = first; i <= last; i++ )
        nut.a[i] = do_add( nut.a[i], nut.b[i] );
      break;
    case 0x0A:  // A=A+C
      for( i = first; i <= last; i++ )
        nut.a[i] = do_add( nut.a[i], nut.c[i] );
      break;
    case 0x0B:  // A=A+1
      nut.carry = 1;
      for( i = first; i <= last; i++ )
        nut.a[i] = do_add( nut.a[i], 0 );
      break;
    case 0x0C:  // A=A-B
      for( i = first; i <= last; i++ )
        nut.a[i] = do_sub( nut.a[i], nut.b[i] );
      break;
    case 0x0D:  // A=A-1
      nut.carry = 1;
      for( i = first; i <= last; i++ )
        nut.a[i] = do_sub( nut.a[i], 0 );
      break;
    case 0x0E:  // A=A-C
      for( i = first; i <= last; i++ )
        nut.a[i] = do_sub( nut.a[i], nut.c[i] );
      break;
    case 0x0F:  // C=C+C
      for( i = first; i <= last; i++ )
        nut.c[i] = do_add( nut.c[i], nut.c[i] );
      break;
    case 0x10:  // C=A+C
      for( i = first; i <= last; i++ )
        nut.c[i] = do_add( nut.a[i], nut.c[i] );
      break;
    case 0x11:  // C=C+1
      nut.carry = 1;
      for( i = first; i <= last; i++ )
        nut.c[i] = do_add( nut.c[i], 0 );
      break;
    case 0x12:  // C=A-C
      for( i = first; i <= last; i++ )
        nut.c[i] = do_sub( nut.a[i], nut.c[i] );
      break;
    case 0x13:  // C=C-1
      nut.carry = 1;
      for( i = first; i <= last; i++ )
        nut.c[i] = do_sub( nut.c[i], 0 );
      break;
    case 0x14:  // C=-C
      for( i = first; i <= last; i++ )
        nut.c[i] = do_sub( 0, nut.c[i] );
      break;
    case 0x15:  // C=-C-1
      nut.carry = 1;
      for( i = first; i <= last; i++ )
        nut.c[i] = do_sub( 0, nut.c[i] );
      break;
    case 0x16:  // ? B#0
      for( i = first; i <= last; i++ )
        nut.carry |= (nut.b[i] != 0);
      break;
    case 0x17:  // ? C#0
      for( i = first; i <= last; i++ )
        nut.carry |= (nut.c[i] != 0);
      break;
    case 0x18:  // ? A<C
      for( i = first; i <= last; i++ )
        do_sub( nut.a[i], nut.c[i] );
      break;
    case 0x19:  // ? A<B
      for( i = first; i <= last; i++ )
        do_sub( nut.a[i], nut.b[i] );
      break;
    case 0x1A:  // ? A#0
      for( i = first; i <= last; i++ )
        nut.carry |= (nut.a[i] != 0);
      break;
    case 0x1B:  // ? A#C
      for( i = first; i <= last; i++ )
        nut.carry |= (nut.a[i] != nut.c[i]);
      break;
    case 0x1C:  // ASR
      for( i = first; i <= last; i++ )
        nut.a[i] = (i == last) ? 0 : nut.a[i + 1];
      break;
    case 0x1D:  // BSR
      for( i = first; i <= last; i++ )
        nut.b[i] = (i == last) ? 0 : nut.b[i + 1];
      break;
    case 0x1E:  // CSR
      for( i = first; i <= last; i++ )
        nut.c[i] = (i == last) ? 0 : nut.c[i + 1];
      break;
    case 0x1F:  // ASL
      for( i = last; i >= first; i-- )
        nut.a[i] = (i == first) ? 0 : nut.a[i - 1];
      break;
    }
}

//
// stack operations


static int Cnut::pop( void )
{
  int i;
  int ret;

  ret = nut.stack[0];
  for( i = 0; i < STACK_DEPTH - 1; i++ )
    nut.stack[i] = nut.stack[i + 1];
  nut.stack[STACK_DEPTH - 1] = 0;
  return( ret );
}

void Cnut::push( int a )
{
  int i;

  for( i = STACK_DEPTH - 1; i > 0; i-- )
    nut.stack[i] = nut.stack[i - 1];
  nut.stack[0] = a;
}

void Cnut::op_return( int opcode )
{
  DISASM( "RTN" );

  if( -1 != nut.stack_j )
    {
    nut.pc = nut.stack_j;
    nut.stack_j = -1;
    dbg.run = dbg.step = dbg.next = 0;
    return;
    }
  nut.pc = pop();
}

void Cnut::op_return_if_carry( int opcode )
{
  DISASM( "CRTN" );

  if( nut.prev_carry)
    {
    if( -1 != nut.stack_j )
      {
      nut.pc = nut.stack_j;
      nut.stack_j = -1;
      dbg.run = dbg.step = dbg.next = 0;
      return;
      }
    nut.pc = pop();
    }
}

void Cnut::op_return_if_no_carry( int opcode )
{
  DISASM( "NCRTN" );

  if( ! nut.prev_carry)
    {
    if( -1 != nut.stack_j )
      {
      nut.pc = nut.stack_j;
      nut.stack_j = -1;
      dbg.run = dbg.step = dbg.next = 0;
      return;
      }
    nut.pc = pop();
    }
}

void Cnut::op_pop( int opcode )
{
  DISASM( "CLRRTN" );

  (void)pop();
}

void Cnut::op_pop_c( int opcode )
{
  DISASM( "C=STK" );

  int a;

  a = pop();
  nut.c[6] = a >> 12;
  nut.c[5] = (a >> 8) & 0x0F;
  nut.c[4] = (a >> 4) & 0x0F;
  nut.c[3] = a & 0x0F;
}

void Cnut::op_push_c( int opcode )
{
  DISASM( "STK=C" );

  push((nut.c[6] << 12) | (nut.c[5] << 8) | (nut.c[4] << 4) | nut.c[3]);
}

//
// branch operations


void Cnut::op_short_branch( int opcode )
{
  int offset;

  offset = (opcode >> 3) & 0x3F;
  if( opcode & 0x200)
    offset = offset - 64;

#if 0
  DISASM( "J%sC\t%04X",
          (opcode >> 2) & 1 ? "" : "N",
          disasm.pc + 1 + offset - 1 );
#endif
  if( disasm.is )
    {
    char  *symb;
    int target = disasm.pc + 1 + offset - 1;
    if( NULL == (symb = get_mfrm_name( (ushort)target )) )
      if( NULL == (symb = asm_get_name_in_page( (ushort)target, (disasm.pc >> 12), disasm.bank )) )
        symb = disasm_get_name( (ushort)target );
    if( symb )
      {
      DISASM( "J%sC\t%s%s\t; %04X",
              (opcode >> 2) & 1 ? "" : "N",
              symb, 8 <= strlen(symb) ? "" : "\t", target );
      }
    else
      {
      DISASM( "J%sC\t%04X",
              (opcode >> 2) & 1 ? "" : "N",
              disasm.target = target );
      }
    }

  if( ((opcode >> 2) & 1) == nut.prev_carry)
    nut.pc = nut.pc + offset - 1;
}

void Cnut::op_long_branch( int opcode )
{
  short  word2;
  int    target;
  int    quad = -1;
  char  *symb;
  char  *rel = NULL;

  word2 = ( disasm.is ?
              get_code_with_bank( ++disasm.pc, disasm.bank ) :
              get_code( nut.pc++ ) );
  target = (opcode >> 2) | ((word2 & 0x3FC) << 6);

/*
             Quad 0      Quad 1     Quad 2     Quad 3     Same Quad
NCXQ         [GOSUB0]    [GOSUB1]   [GOSUB2]   [GOSUB3]   [GOSUB]
(Address)    23D2        23DB       23E4       23ED       0FDE
NCXQ         [GOL0]      [GOL1]     [GOL2]     [GOL3]     [GOLONG]
(Address)    23D0        23D9       23E2       23EB       0FDA
*/
  if( disasm.is )
    {
    char *goxq;

    if( NULL == (symb = get_mfrm_name( (ushort)target )) )
      if( NULL == (symb = asm_get_name_in_page( (ushort)target, (disasm.pc >> 12), disasm.bank )) )
        symb = disasm_get_name( (ushort)target );
    switch( target )
      {
      case 0x23ED: quad++;
      case 0x23E4: quad++;
      case 0x23DB: quad++;
      case 0x23D2: quad++;
      case 0x0FDE: goxq = "XQ"; rel = "REL"; break;
      case 0x23EB: quad++;
      case 0x23E2: quad++;
      case 0x23D9: quad++;
      case 0x23D0: quad++;
      case 0x0FDA: goxq = "GO"; rel = "REL"; break;
      }
    //if( rel && !(word2 & 3))
    if( rel )
      {
      int offset = get_code_with_bank( ++disasm.pc, disasm.bank );
      target = (disasm.pc & 0xF000) |
               (( -1 == quad ? disasm.pc & 0x0C00 : quad << 10 ) +
                (offset & 0x3FF));
      if( (-1 != quad) && ((target & 0x0C00) == (disasm.pc & 0x0C00)) )
        rel = "REL!";
      if( NULL == (symb = get_mfrm_name( (ushort)target )) )
        if( NULL == (symb = asm_get_name_in_page( (ushort)target, (disasm.pc >> 12), disasm.bank )) )
          symb = disasm_get_name( (ushort)target );
      if( symb )
        {
        DISASM2( {disasm.is_messl = (!(word2 & 0x002) && (0x07EF == target));},
                 "%sC%s%s\t%s%s\t; %04X",
                 word2 & 0x001 ? "" : "N",
                 goxq, rel,
                 symb, 8 <= strlen(symb) ? "" : "\t", target );
        }
      else
        {
        DISASM2( {disasm.is_messl = (!(word2 & 0x002) && (0x07EF == target));},
                 "%sC%s%s\t%04X",
                 word2 & 0x001 ? "" : "N",
                 goxq, rel,
                 disasm.target = target );
        }
      }
    else if( symb )
      {
      DISASM2( {disasm.is_messl = (!(word2 & 0x002) && (0x07EF == target));},
               "%sC%s\t%s%s\t; %04X",
               word2 & 0x001 ? "" : "N",
               word2 & 0x002 ? "GO" : "XQ",
               symb, 8 <= strlen(symb) ? "" : "\t", target );
      }
    else
      {
      DISASM2( {disasm.is_messl = (!(word2 & 0x002) && (0x07EF == target));},
               "%sC%s\t%04X",
               word2 & 0x001 ? "" : "N",
               word2 & 0x002 ? "GO" : "XQ",
               disasm.target = target );
      }
    }

  if( (word2 & 0x001) == nut.prev_carry)
    {
    if( word2 & 0x002)
      nut.pc = target;
    else
      {
      if( dbg.next-- )
        {
        nut.stack_j = nut.pc;
        dbg.run = 1;
        }
      push (nut.pc);
      nut.pc = target;
      if( 0 == get_code(nut.pc) )
        {
        nut.pc = pop();
        nut.stack_j = -1;
        }
      }
    }
}

void Cnut::op_goto_c( int opcode )
{
  DISASM( "GTOC" );

  nut.pc = (nut.c[6] << 12) | (nut.c[5] << 8) | (nut.c[4] << 4) | nut.c[3];
}

void Cnut::op_romblk( int opcode )
{
  int dp, ps;

  DISASM( "ROMBLK");

  dp = nut.c[0]; // destination page is in c[0]
  ps = 8; // always page 8 or n[6] ???
  if( get_page( ps, get_active_bank(ps) ) == (HW_HEPAX | HW_ROM) &&
      (HW_NONE == ( get_page( dp, 0 ) |
                    get_page( dp, 1 ) |
                    get_page( dp, 2 ) |
                    get_page( dp, 3 ) )) )
    {
    move_page( nsim.atpage[ps][0], ps, dp );
    if( HW_NONE == get_page( dp, 0 ) )
      {
      // page move not performed!!
      bad_op( opcode );
      }
    if( nsim.atpage[SHADOW_PAGE][0] &&
        (HW_HEPAX | HW_RAM) == nsim.atpage[SHADOW_PAGE][0]->hw->bits &&
        8 == nsim.atpage[SHADOW_PAGE][0]->page[0].page &&
        !nsim.atpage[ps][0] )
      {
      // Case of the shadow page 8 by HEPAX ROM
      unshadow_page( nsim.atpage[SHADOW_PAGE][0], ps );
      }
    }
  else
    bad_op( opcode );
}

void Cnut::op_write_mldl( int opcode )
{
  int   addr;
  int   bank, page;
  short tyte;

  DISASM( "WMLDL" );

  page = nut.c[6];
  bank = get_active_bank( page );
  addr = (nut.c[6] << 12) | (nut.c[5] << 8) | (nut.c[4] << 4) | nut.c[3];
  tyte = (( nut.c[2] & 0x3 ) << 8)| ( nut.c[1] << 4)|( nut.c[0]);
  if( !( get_page( page, bank ) & (HW_RAM | HW_HEPAX) ) )
    {
    Ooops( "%04X : WMLDL( %04X/%X ) -> %03X: NO PAGE or ROM PAGE\n", nut.pc, addr, bank, tyte );
    return;
    }

  if( dbg.trace )
    printf( "%04X : WMLDL( %04X/%X ) -> %03X\n", nut.pc, addr, bank, tyte );

  put_code_with_bank( addr, bank, tyte );
  rescan_fat_and_bitmap( page, bank, addr, debuglevel );
}


//
// Bank selection used in 41CX, Advantage ROM, HEPAX, WWRAMBOX and perhaps others

void Cnut::op_enbank( int opcode )
{
  int bank = (opcode >> 6) & 3;

  // 00 -> 00, 01 -> 10, 10 -> 01, 11 -> 11
  bank = ((bank & 2) >> 1) | ((bank & 1) << 1);

  DISASM( "ENBANK%u", bank + 1 );

  select_bank( nut.prev_pc, bank );
}


//
// M operations

void Cnut::op_c_to_m( int opcode )
{
  int i;

  DISASM( "M=C" );

  for( i = 0; i < WSIZE; i++ )
    nut.m[i] = nut.c[i];
}

void Cnut::op_m_to_c( int opcode )
{
  int i;

  DISASM( "C=M" );

  for( i = 0; i < WSIZE; i++ )
    nut.c[i] = nut.m[i];
}

void Cnut::op_c_exch_m( int opcode )
{
  int i, t;

  DISASM( "C<>M" );

  for( i = 0; i < WSIZE; i++ )
    {
    t = nut.c[i];
    nut.c[i] = nut.m[i];
    nut.m[i] = t;
    }
}


//
// N operations

void Cnut::op_c_to_n( int opcode )
{
  int i;

  DISASM( "N=C" );

  for( i = 0; i < WSIZE; i++ )
    nut.n[i] = nut.c[i];
}

void Cnut::op_n_to_c( int opcode )
{
  int i;

  DISASM( "C=N" );

  for( i = 0; i < WSIZE; i++ )
    nut.c[i] = nut.n[i];
}

void Cnut::op_c_exch_n( int opcode )
{
  int i, t;

  DISASM( "C<>N" );

  for( i = 0; i < WSIZE; i++ )
    {
    t = nut.c[i];
    nut.c[i] = nut.n[i];
    nut.n[i] = t;
    }
}


//
// RAM and peripheral operations

void Cnut::op_c_to_dadd( int opcode )
{
  DISASM( "RAMSLCT" );

  ram_addr = ((nut.c[2] << 8) | (nut.c[1] << 4) | nut.c[0]) & 0x3FF;
}

void Cnut::op_c_to_pfad( int opcode )
{
  DISASM( "PERSLCT");

  pf_addr = (nut.c[1] << 4) | nut.c[0];
}

void Cnut::op_read_reg_n( int opcode )
{
  int i;
  int is_ram, is_pf;

  if( opcode >> 6 )
    DISASM( "C=REGN\t%u\t; '%c'", (opcode >> 6), regmap[opcode >> 6] );
  else
    DISASM( "RDATA" );

  if( (opcode >> 6) )
    ram_addr = (ram_addr & ~0x0F) | (opcode >> 6);
  is_ram = ram_exists[ram_addr];
  is_pf  = pf_exists[pf_addr];

  if( (is_ram && !(ram_addr <= 0x00F || ram_addr >= 0x020)) &&
      is_pf )
    {
    Ooops( "%04X: conflicting read RAM %03X PF %02X REG %01X\n",
           nut.pc, ram_addr, pf_addr, opcode >> 6);
    }
  if( is_ram )
    {
    for( i = 0; i < WSIZE; i++ )
      nut.c[i] = ram[ram_addr][i];
    }
  else if( is_pf )
    {
#if 0
    for( i = 0; i < WSIZE; i++ )
      nut.c[i] = 0;
#endif
    if( rd_n_fcn[pf_addr] )
      (*rd_n_fcn[pf_addr])(opcode >> 6);
    }
  else
    {
    Ooops( "%04X: warning: stray read RAM %03X PF %02X REG %01X\n",
           nut.pc, ram_addr, pf_addr, opcode >> 6);
    for( i = 0; i < WSIZE; i++ )
      nut.c[i] = 0;
    }
}

void Cnut::op_write_reg_n( int opcode )
{
  int i;
  int is_ram, is_pf;

  DISASM( "REGN=C\t%u\t; '%c'", (opcode >> 6), regmap[opcode >> 6] );

  ram_addr = (ram_addr & ~0x0F) | (opcode >> 6);
  is_ram = ram_exists[ram_addr];
  is_pf  = pf_exists[pf_addr];

  if( (is_ram && !(ram_addr <= 0x00F || ram_addr >= 0x020)) &&
      is_pf )
    {
    Ooops( "%04X: conflicting write RAM %03X PF %02X REG %01X\n",
           nut.pc,ram_addr, pf_addr, opcode >> 6);
    }
  else if( (!is_ram) && (!is_pf))
    {
#ifdef WARN_STRAY_WRITE
    Ooops( "%04X/%X: STRAY WRITE RAM %03X PF %02X REG %01X\n",
           nut.pc, get_active_bank(nut.pc / PAGE_SIZE), ram_addr, pf_addr, opcode >> 6);
#endif
    }
  if( is_ram )
    {
    for( i = 0; i < WSIZE; i++ )
      ram[ram_addr][i] = nut.c[i];
    if( focal.trace &&
        (FOCAL_TRACE_REG & focal.trace_opts) &&
        focal.reg_ok )
      focal.regmap[ram_addr / sizeof(int)] |= (1 << (ram_addr % sizeof(int)));
    }
  if( is_pf )
    {
    if( wr_n_fcn[pf_addr] )
      (*wr_n_fcn[pf_addr])(opcode >> 6);
    }
}

void Cnut::op_c_to_data( int opcode )
{
  int i;
  int is_ram, is_pf;

  DISASM( "WDATA" );

  is_ram = ram_exists[ram_addr];
  is_pf  = pf_exists[pf_addr];

  if( (is_ram && !(ram_addr <= 0x00F || ram_addr >= 0x020)) &&
      is_pf )
    {
    Ooops( "%04X/%X: CONFLICTING WRITE RAM %03x PF %02x\n",
           nut.pc, get_active_bank(nut.pc / PAGE_SIZE), ram_addr, pf_addr);
    }
  else if( (! is_ram) && (! is_pf))
    {
#ifdef WARN_STRAY_WRITE
    Ooops( "%04X/%X: STRAY WRITE RAM %03x PF %02x\n",
           nut.pc, get_active_bank(nut.pc / PAGE_SIZE), ram_addr, pf_addr);
#endif
    }
  if( is_ram)
    {
    for( i = 0; i < WSIZE; i++)
      ram[ram_addr][i] = nut.c[i];
    if( focal.trace &&
        (FOCAL_TRACE_REG & focal.trace_opts) &&
        focal.reg_ok )
      focal.regmap[ram_addr / sizeof(int)] |= (1 << (ram_addr % sizeof(int)));
    }
  if( is_pf )
    {
    if( wr_fcn[pf_addr] )
      (*wr_fcn[pf_addr])();
    }
}

void Cnut::op_test_ext_flag( int opcode )
{
  DISASM( "%s", ext_flagmap[opcode >> 6] );

  nut.carry = 0;

  if( (get_page( 6, 0 ) & HW_PRNTR) && (itmap[0] == (opcode >> 6)) )
    // PRINTER busy ?
    nut.carry = 0;
  else if( pf_exists[PF_CARDR] && (itmap[1] == (opcode >> 6)) )
    // interrupt flag set for CARD READER ?
    nut.carry = cardr_test_if();
  else if( pf_exists[PF_WAND] && (itmap[2] == (opcode >> 6)) )
    // data available in WAND buffer ?
    nut.carry = wand_test_data();
}


//
// S operations

void Cnut::op_set_s( int opcode )
{
  DISASM( "SF\t%u", tmap[opcode >> 6] );

  nut.s[tmap[opcode >> 6]] = 1;
}

void Cnut::op_clr_s( int opcode )
{
  DISASM( "CF\t%u", tmap[opcode >> 6] );

  nut.s[tmap[opcode >> 6]] = 0;
}

void Cnut::op_test_s( int opcode )
{
  DISASM( "?FS\t%u", tmap[opcode >> 6] );

  nut.carry = nut.s[tmap[opcode >> 6]];
}

static int Cnut::get_s_bits( int first, int count )
{
  int i, mask = 1, r = 0;

  for( i = first; i < first + count; i++ )
    {
    if( nut.s[i])
      r = r + mask;
    mask <<= 1;
    }
  return r;
}

void Cnut::set_s_bits( int first, int count, int v )
{
  int i, mask = 1;

  for( i = first; i < first + count; i++ )
    {
    nut.s[i] = (v & mask) != 0;
    mask <<= 1;
    }
}

void Cnut::op_clear_all_s( int opcode )
{
  DISASM( "ST=0" );

  set_s_bits( 0, 8, 0 );
}

void Cnut::op_c_to_s( int opcode )
{
  DISASM( "ST=C" );

  set_s_bits( 0, 4, nut.c[0] );
  set_s_bits( 4, 4, nut.c[1] );
}

void Cnut::op_s_to_c( int opcode )
{
  DISASM( "C=ST" );

  nut.c[0] = get_s_bits( 0, 4 );
  nut.c[1] = get_s_bits( 4, 4 );
}

void Cnut::op_c_exch_s( int opcode )
{
  int t;

  DISASM( "C<>ST" );

  t = get_s_bits( 0, 4 );
  set_s_bits( 0, 4, nut.c[0] );
  nut.c[0] = t;
  t = get_s_bits( 4, 4 );
  set_s_bits( 4, 4, nut.c[1] );
  nut.c[1] = t;
}

void Cnut::op_sb_to_f( int opcode )
{
  DISASM( "F=ST" );

  nut.fo = get_s_bits( 0, 8 );
}

void Cnut::op_f_to_sb( int opcode )
{
  DISASM( "ST=F" );

  set_s_bits( 0, 8, nut.fo );
}

void Cnut::op_f_exch_sb( int opcode )
{
  int t;

  DISASM( "ST<>F" );

  t = get_s_bits( 0, 8 );
  set_s_bits( 0, 8, nut.fo );
  nut.fo = t;
}


//
// POINTER operations

void Cnut::op_dec_pt( int opcode )
{
  DISASM( "-PT" );

  (*nut.pt)--;
  if( (*nut.pt) >= WSIZE ) // can't be negative because it is unsigned
    (*nut.pt) = WSIZE - 1;
}

void Cnut::op_inc_pt( int opcode )
{
  DISASM( "+PT" );

  (*nut.pt)++;
  if( (*nut.pt) >= WSIZE )
    (*nut.pt) = 0;
}

void Cnut::op_set_pt( int opcode )
{
  DISASM( "PT=\t%u", tmap[opcode >> 6] );

  (*nut.pt) = tmap[opcode >> 6];
}

void op_test_pt( int opcode )
{
  DISASM( "?PT=\t%u", tmap[opcode >> 6] );

  nut.carry = ((*nut.pt) == tmap[opcode >> 6]);
}

void Cnut::op_sel_p( int opcode )
{
  DISASM( "PT=P" );

  nut.pt = & nut.p;
}

void Cnut::op_sel_q( int opcode )
{
  DISASM( "PT=Q" );

  nut.pt = & nut.q;
}

void Cnut::op_test_pq( int opcode )
{
  DISASM( "?P=Q" );

  if( nut.p == nut.q )
    nut.carry = 1;
}

void Cnut::op_lc( int opcode )
{
  DISASM( "LC\t%X", opcode>>6 );

  nut.c[(*nut.pt)--] = opcode >> 6;
  if( (*nut.pt) >= WSIZE ) // unsigned, can't be negative
    (*nut.pt) = WSIZE - 1;
}

void Cnut::op_c_to_g( int opcode )
{
  DISASM( "G=C" );

  nut.g[0] = nut.c[(*nut.pt)];
  if( (*nut.pt) == (WSIZE - 1) )
    {
    nut.g[1] = 0;
#ifdef WARNING_G
    Ooops( "warning: c to g transfer with pt=13\n");
#endif
    }
  else
    nut.g[1] = nut.c[(*nut.pt) + 1];
}

void Cnut::op_g_to_c( int opcode )
{
  DISASM( "C=G" );

  nut.c[(*nut.pt)] = nut.g[0];
  if( (*nut.pt) == (WSIZE - 1) )
    {
    ;
#ifdef WARNING_G
    Ooops( "warning: g to c transfer with pt=13\n");
#endif
    }
  else
    nut.c[(*nut.pt) + 1] = nut.g[1];
}

void Cnut::op_c_exch_g( int opcode )
{
  int t;

  DISASM( "C<>G" );

  t = nut.g[0];
  nut.g[0] = nut.c[(*nut.pt)];
  nut.c[(*nut.pt)] = t;
  if( (*nut.pt) == (WSIZE - 1) )
    {
    nut.g[1] = 0;
#ifdef WARNING_G
    Ooops( "warning: c exchange g with pt=13\n");
#endif
    }
  else
    {
    t = nut.g[1];
    nut.g[1] = nut.c[(*nut.pt) + 1];
    nut.c[(*nut.pt) + 1] = t;
    }
}

//
// keyboard operations

//
// This table maps logical keycodes from the I/O system into the actual
// keycodes returned by the c=key microinstruction.  -1 entries are
// invalid key codes.

static int keymap[100] =
{
  /* col       0,    1,    2,    3,    4,    5,    6,    7,    8,    9 */
  /* row 0 */ -1, 0x18, 0xc6, 0xc5, 0xc4,   -1,   -1,   -1,   -1,   -1,
  /* row 1 */ -1, 0x10, 0x30, 0x70, 0x80, 0xc0,   -1,   -1,   -1,   -1,
  /* row 2 */ -1, 0x11, 0x31, 0x71, 0x81, 0xc1,   -1,   -1,   -1,   -1,
  /* row 3 */ -1, 0x12, 0x32, 0x72, 0x82, 0xc2,   -1,   -1,   -1,   -1,
  /* row 4 */ -1, 0x13,   -1, 0x73, 0x83, 0xc3,   -1,   -1,   -1,   -1,
  /* row 5 */ -1, 0x14, 0x34, 0x74, 0x84,   -1,   -1,   -1,   -1,   -1,
  /* row 6 */ -1, 0x15, 0x35, 0x75, 0x85,   -1,   -1,   -1,   -1,   -1,
  /* row 7 */ -1, 0x16, 0x36, 0x76, 0x86,   -1,   -1,   -1,   -1,   -1,
  /* row 8 */ -1, 0x17, 0x37, 0x77, 0x87,   -1,   -1,   -1,   -1,   -1,
  /* row 9 */ -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1
};


void Cnut::handle_keyboard( void )
{
  int key;

  key = check_keyboard();
  if( key == KC_EXIT )
    nsim_exit_and_save();
  if( key == KC_NONE )
    return;
  if( keymap[key] < 0 )
    return;
  if( (!nut.awake) && (!display_enable) && (0x18 != keymap[key]) )
    return;
  nut.key_flag = 1;
  nut.key_buf = keymap[key];
  nut.awake = 1;
  return;
}

void Cnut::op_keys_to_pc( int opcode )
{
  DISASM( "GTOKEY" );
  nut.pc = (nut.pc & 0xFF00) | nut.key_buf;
}

void Cnut::op_keys_to_c( int opcode )
{
  DISASM( "C=KEY" );

  nut.c[4] = nut.key_buf >> 4;
  nut.c[3] = nut.key_buf & 0x0F;
}

void Cnut::op_test_kb( int opcode )
{
  DISASM( "?KEY" );

  nut.carry = nut.key_flag;
}

void Cnut::op_reset_kb( int opcode )
{
  DISASM( "CLRKEY" );

  nut.key_flag = 0;
  handle_keyboard();  // see if key still down
}


//
// misc. operations

void Cnut::op_nop( int opcode )
{
  DISASM( "NOP" );

  (void)opcode;
}

void Cnut::op_set_hex( int opcode )
{
  DISASM( "SETHEX" );

  (void)opcode;
  nut.arith_base = 16;
}

void Cnut::op_set_dec( int opcode )
{
  DISASM( "SETDEC" );

  (void)opcode;
  nut.arith_base = 10;
}

void Cnut::op_rom_to_c( int opcode )
{
  int addr, tyte;

  DISASM( "RDROM" );

  (void)opcode;
  addr = (nut.c[6] << 12) | (nut.c[5] << 8) | (nut.c[4] << 4) | nut.c[3];
  tyte = get_code( addr );
  nut.c[2] = (tyte >> 8) & 0x03;
  nut.c[1] = (tyte >> 4) & 0x0F;
  nut.c[0] = tyte & 0x0F;
}

void Cnut::op_clear_abc( int opcode )
{
  int i;

  DISASM( "ABC=0" );

  for( i = 0; i < WSIZE; i++ )
    nut.a[i] = nut.b[i] = nut.c[i] = 0;
}

void Cnut::op_ldi( int opcode )
{
  int tyte = (disasm.is ?
              get_code_with_bank( ++disasm.pc, disasm.bank ) :
              get_code( nut.pc++ ));

  DISASM( "LDI\t%03X", tyte );

  nut.c[2] = (tyte >> 8) & 0x03;
  nut.c[1] = (tyte >> 4) & 0x0F;
  nut.c[0] = tyte & 0x0F;
}
 void Cnut::op_or( int opcode )
{
  int i;

  DISASM( "C=CORA" );

  for( i = 0; i < WSIZE; i++ )
    nut.c[i] = nut.c[i] | nut.a[i];
}

void Cnut::op_and( int opcode )
{
  int i;

  DISASM( "C=CANDA" );

  for( i = 0; i < WSIZE; i++ )
    nut.c[i] = nut.c[i] & nut.a[i];
}

void Cnut::op_rcr( int opcode )
{
  int count, i, j;
  reg t;

  count = tmap[opcode >> 6];

  DISASM( "RCR\t%u", count );

  for( i = 0; i < WSIZE; i++ )
    {
    j = (i + count) % WSIZE;
    t[i] = nut.c[j];
    }
  for( i = 0; i < WSIZE; i++ )
    nut.c[i] = t[i];
}

void Cnut::op_wtog( int opcode )
{
  DISASM( "?WTOG" );
}

void Cnut::op_lld( int opcode )
{
  DISASM( "?BAT" );

  nut.carry = 0;  // "batteries" are fine
}

void Cnut::op_powoff( int opcode )
{
  DISASM( "POWOFF" );

  nut.awake = 0;
  nut.pc = 0;

  if( display_enable)
    {
    // going to light sleep
#ifdef AUTO_POWER_OFF
    // start display timer if LCD chip is selected
    if( PF_DISPLAY == pf_addr )
      display_timer = DISPLAY_TIMEOUT;
#endif // AUTO_POWER_OFF
    }
  else
    {
    // going to deep sleep
    nut.carry = 1;
    // reset all banks to 0
    reset_bank();
    // reset page moved if ROMBLK called
    reset_moved_page();
    }
}

void Cnut::op_c_to_hpil( int opcode )
{
  int reg = (opcode >> 6) & 0x007;

  DISASM( "HPIL=C\t%X", reg );

  if( hpil.is )
    hpil_from_c( reg );
}

void Cnut::op_hpil_to_c( int opcode )
{
  int reg = (opcode >> 6) & 0x007;
  short  word2, word3;

  word2 = ( disasm.is ?
              get_code_with_bank( ++disasm.pc, disasm.bank ) :
              get_code( nut.pc++ ) );

  if( (word2 & 0x003) == 0x001 )
    {
    DISASM( "HPL=CH\t%X,%02X", reg, (word2 >> 2) & 0x0FF );
    }
  else
    {
    word3 = ( disasm.is ?
                get_code_with_bank( ++disasm.pc, disasm.bank ) :
                get_code( nut.pc++ ) );
    if( ((0x03A | (reg << 6)) != word2) || ((0x003 | (reg << 6)) != word3) )
      {
      Ooops( "%04X/%X C=HPIL\t%X is badly set! => %03X,%03X,%03X\n",
             disasm.is ? disasm.pc : nut.prev_pc,
             get_active_bank((disasm.is ? disasm.pc : nut.prev_pc) / PAGE_SIZE),
             reg, opcode, word2, word3 );
      DISASM2( {disasm.per = -1; disasm.pc -= 2; },
               "#%03X\t; Incorrect ? C=HPIL\t%X\n", opcode, reg );
      nut.pc -= 2;
      }
    DISASM( "C=HPIL\t%X", reg );
    }

  if( hpil.is )
    hpil_to_c( reg, word2, word3 );
}

void Cnut::op_selprf( int opcode )
{
  int per = (opcode >> 6);

  DISASM2( {disasm.per = per;}, "PERTCT\t%X", per );

  (*op_selper_take_fcn[per])();
  peraddr = per;
}

void Cnut::op_selper_nofcn( int opcode )
{
  int per = (opcode >> 6);

  DISASM2( {disasm.per = per;}, "#%03X\t; No SELP for PER %02X", opcode, disasm.per );
  if( opcode & 1 );
    peraddr = -1;
  return;
}
void Cnut::op_selper_no_give_fcn( void )
{
  return;
}
void Cnut::op_selper_no_take_fcn( void )
{
  return;
}
static bool Cnut::op_selper_no_has_fcn( void )
{
  return 0;
}

void Cnut::register_selper_ops( int    per,
                          void (*perfcn)( int ),
                          void (*take)( void ),
                          void (*give)( void ),
                          bool (*has)( void ) )
{
  op_selper_fcn[per]      = perfcn;
  op_selper_give_fcn[per] = give;
  op_selper_take_fcn[per] = take;
  op_selper_has_fcn[per]  = has;
}

void Cnut::unregister_selper_ops( int per )
{
  op_selper_fcn[per]      = op_selper_nofcn;
  op_selper_give_fcn[per] = op_selper_no_give_fcn;
  op_selper_take_fcn[per] = op_selper_no_take_fcn;
  op_selper_has_fcn[per]  = op_selper_no_has_fcn;
}

void Cnut::register_pf_ops( int    pf,
                      void (*read_pf)( int ),
                      void (*write_pf)( int ),
                      void (*write_data_pf)( void ),
                      bool (*load_pf)( char * ),
                      void (*save_pf)( FILE *, char* ) )
{
  if( ! pf_exists[pf] )
    {
    rd_n_fcn[pf]  = read_pf;
    wr_n_fcn[pf]  = write_pf;
    wr_fcn[pf]    = write_data_pf;
    load_fcn[pf]  = load_pf;
    save_fcn[pf]  = save_pf;
    pf_exists[pf] = 1;
    }
  else
    Ooops( "PF %02X ALREADY EXISTS.\n", pf);
}

void Cnut::unregister_pf_ops( int pf )
{
  if( pf_exists[pf] )
    {
    pf_exists[pf] = 0;
    rd_n_fcn[pf]  = NULL;
    wr_n_fcn[pf]  = NULL;
    wr_fcn[pf]    = NULL;
    load_fcn[pf]  = NULL;
    save_fcn[pf]  = NULL;
    }
}

void Cnut::init_pf( void )
{
  int i;

  for( i = 0; i < MAX_PFAD; i++ )
    pf_exists[i] = 0;
}

void Cnut::register_op( int opcode, void (*op)( int ) )
{
  if( bad_op == op_fcn[opcode] )
    op_fcn[opcode] = op;
}

void Cnut::unregister_op( int opcode )
{
  op_fcn[opcode] = bad_op;
}

//
// initialization


void Cnut::init_ops( void )
{
  int i;
  for( i = 0; i < 1024; i += 4) {
      op_fcn[i + 0] = bad_op;
      op_fcn[i + 1] = op_long_branch;
      op_fcn[i + 2] = op_arith;
      op_fcn[i + 3] = op_short_branch;
  }

  op_fcn[0x000] = op_nop;

  op_fcn[0x030] = op_romblk; // HEPAX page move
  op_fcn[0x040] = op_write_mldl;
  op_fcn[0x064] = op_nop;

  op_fcn[0x100] = op_enbank;
  op_fcn[0x180] = op_enbank;
  op_fcn[0x140] = op_enbank;
  op_fcn[0x1C0] = op_enbank;
  op_fcn[0x1F0] = op_wtog;   //WTOG

  for( i = 0; i < WSIZE; i++) {
      op_fcn[0x004 + (itmap[i] << 6)] = op_clr_s;
      op_fcn[0x008 + (itmap[i] << 6)] = op_set_s;
      op_fcn[0x00C + (itmap[i] << 6)] = op_test_s;
      op_fcn[0x014 + (itmap[i] << 6)] = op_test_pt;
      op_fcn[0x01C + (itmap[i] << 6)] = op_set_pt;
      op_fcn[0x02C + (itmap[i] << 6)] = op_test_ext_flag;
      op_fcn[0x03C + (itmap[i] << 6)] = op_rcr;
  }

  op_fcn[0x3C4] = op_clear_all_s;
  op_fcn[0x3C8] = op_reset_kb;
  op_fcn[0x3CC] = op_test_kb;
  op_fcn[0x3D4] = op_dec_pt;
  op_fcn[0x3Dc] = op_inc_pt;

  for( i = 0; i < 16; i++) {
      op_fcn[0x010 + (i << 6)] = op_lc;
      op_fcn[0x024 + (i << 6)] = op_selprf;
      op_fcn[0x028 + (i << 6)] = op_write_reg_n;
      op_fcn[0x038 + (i << 6)] = op_read_reg_n;
  }

  op_fcn[0x058] = op_c_to_g;
  op_fcn[0x098] = op_g_to_c;
  op_fcn[0x0D8] = op_c_exch_g;

  op_fcn[0x158] = op_c_to_m;
  op_fcn[0x198] = op_m_to_c;
  op_fcn[0x1D8] = op_c_exch_m;

  op_fcn[0x258] = op_sb_to_f;
  op_fcn[0x298] = op_f_to_sb;
  op_fcn[0x2D8] = op_f_exch_sb;

  op_fcn[0x358] = op_c_to_s;
  op_fcn[0x398] = op_s_to_c;
  op_fcn[0x3D8] = op_c_exch_s;

  op_fcn[0x020] = op_pop;
  op_fcn[0x060] = op_powoff;
  op_fcn[0x0A0] = op_sel_p;
  op_fcn[0x0E0] = op_sel_q;
  op_fcn[0x120] = op_test_pq;
  op_fcn[0x160] = op_lld;
  op_fcn[0x1A0] = op_clear_abc;
  op_fcn[0x1E0] = op_goto_c;
  op_fcn[0x220] = op_keys_to_c;
  op_fcn[0x260] = op_set_hex;
  op_fcn[0x2A0] = op_set_dec;
  op_fcn[0x360] = op_return_if_carry;
  op_fcn[0x3A0] = op_return_if_no_carry;
  op_fcn[0x3E0] = op_return;

  op_fcn[0x070] = op_c_to_n;
  op_fcn[0x0B0] = op_n_to_c;
  op_fcn[0x0F0] = op_c_exch_n;

  op_fcn[0x130] = op_ldi;
  op_fcn[0x170] = op_push_c;
  op_fcn[0x1B0] = op_pop_c;
  op_fcn[0x230] = op_keys_to_pc;
  op_fcn[0x270] = op_c_to_dadd;
  op_fcn[0x2F0] = op_c_to_data;
  op_fcn[0x330] = op_rom_to_c;
  op_fcn[0x370] = op_or;
  op_fcn[0x3B0] = op_and;
  op_fcn[0x3F0] = op_c_to_pfad;

  for( i = 0; i < 16; i++ ) {
      op_selper_fcn[i]      = op_selper_nofcn;
      op_selper_give_fcn[i] = op_selper_no_give_fcn;
      op_selper_take_fcn[i] = op_selper_no_take_fcn;
      op_selper_has_fcn[i]  = op_selper_no_has_fcn;
  }

  // HPIL : Not supported. For disassembler only
  for( i = 0; i < 8; i++ ) {
      op_fcn[0x200 | (i << 6)] = op_c_to_hpil;
      op_fcn[0x024 | (i << 6)] = op_hpil_to_c;
  }
}

void Cnut::nut_push( int a )
{
  push( a );
}

void Cnut::init_cpu_registers( void )
{
  int i;

  // wake from deep sleep
  nut.awake = 1;
  nut.pc = 0;
  nut.carry = 1;

  for( i = 0; i < WSIZE; i++)
    nut.a[i] = nut.b[i] = nut.c[i] = nut.m[i] = nut.n[i] = 0;
  nut.g[0] = 0;
  nut.g[1] = 0;
  nut.fo = 0;
  op_clear_all_s (0);

  nut.p = nut.q = 0;
  nut.pt = & nut.p;

  nut.arith_base = 16;

  for( i = 0; i < STACK_DEPTH; i++)
    nut.stack[i] = 0;
  nut.stack_j = -1;

  nut.cycle = 0;
}

void Cnut::nut_awake()
{
  nut.awake = 1;
}

void Cnut::nut_sim( void )
{
  int opcode;

  opcode = get_code(nut.pc);

  nut.prev_pc = nut.pc;
  nut.prev_carry = nut.carry;
  nut.carry = 0;
  nut.pc++;
  if( peraddr != -1 && (* op_selper_has_fcn[peraddr])() )
    {
    (*op_selper_fcn[peraddr])(opcode);
    if( opcode & 1 )
      {
      (* op_selper_give_fcn[peraddr])();
      peraddr = -1;
      }
    }
  else
    (*op_fcn[opcode])(opcode);
  nut.cycle++;
}

void Cnut::open_nut( void *pmod )
{
static bool xio = 0;
  extern void open_xio( void * );

  DEBUG( "(nut) open\n" );
if( !xio )
  open_xio( pmod );
//xio=1;
  open_display( pmod );
}

void Cnut::close_nut( void *pmod )
{
  extern void close_xio( void * );

  DEBUG( "(nut) close\n" );
  close_display( pmod );
  close_xio( pmod );
}

void Cnut::poll_nut( void *pmod )
{
  (void)pmod;

  handle_display();
  handle_keyboard();
}

void Cnut::notify_nut_poweron( void )
{
  nut.awake = 1;
  display_enable = 1;
  handle_display();
}

#endif
