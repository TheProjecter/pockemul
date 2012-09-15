#include "cf79107pj.h"


CCF79107PJ::CCF79107PJ(CpcXXXX *parent, int clk)
{
    pPC = parent;

}

CCF79107PJ::~CCF79107PJ()
{
}


bool CCF79107PJ::init()
{
}

bool CCF79107PJ::exit()
{
}

void CCF79107PJ::Reset()
{
}

bool CCF79107PJ::step()
{
}

bool CCF79107PJ::instruction(BYTE cmd)
{
}

void CCF79107PJ::Load_Internal(QXmlStreamReader *)
{
}

void CCF79107PJ::save_internal(QXmlStreamWriter *)
{
}

#if 0
/***************************************/
/* ??:comet.c ???                */
/* ??:comet?????               */
/* ??:????                      */
/* ??:1.0 ?                        */
/* ??:???                        */
/* ??:chai2010@2002.cug.edu.cn      */
/* ??:2005-5-3                      */
/***************************************/

#include "comet.h"

/* ??comet?? */

void
comet_load(void)
{
    off_t n, tmp[2];
    fseek(source, 0, SEEK_SET);
    n = fread(tmp, sizeof(off_t), 2, source);
    if(n != 2) {
        printf("?? %s ????\n", pgmName);
        exit(1);
    }
    n = fread(&cmt.mem[tmp[0]], sizeof(off_t), tmp[1], source);
    if(n != tmp[1]) {
        printf("?? %s ????\n", pgmName);
        exit(1);
    }
    cmt.gr[4] = (off_t)sp_start;
    cmt.pc = tmp[0];
}

/* ??comet????IO?? */

void
comet_io(void)
{
    int i;
    off_t addr;
    short count, type, fio;
    char *fmt;
    count = cmt.mem[IO_FLAG] & IO_MAX;
    if(count == 0) return;
    fio = cmt.mem[IO_FLAG] & IO_FIO;
    type = cmt.mem[IO_FLAG] & IO_TYPE;
    addr = cmt.mem[IO_ADDR];
    if(type == IO_CHR) fmt = "%c";
    else if(type == IO_OCT) fmt = "%o";
    else if(type == IO_DEC) fmt = "%d";
    else if(type == IO_HEX) fmt = "%x";
    else {
        cmt.mem[IO_FLAG] &= (!IO_MAX);
        cmt.mem[IO_FLAG] |= IO_ERROR;
        return;
    }
    for(i = 0; i < count; ++i) {
        if(fio == IO_IN)
            scanf(fmt, &cmt.mem[addr++]);
        else if(fio == IO_OUT)
            printf(fmt, cmt.mem[addr++]);
    }
    cmt.mem[IO_FLAG] &= (!IO_MAX);
}

/* ????comet??? */

int
comet_step(void)
{
    off_t temp;
    off_t adr, x1, x2;
    short op, gr, xr;

    op = (off_t)cmt.mem[cmt.pc] / 0x100;
    gr = (off_t)cmt.mem[cmt.pc] % 0x100 / 0x10;
    xr = (off_t)cmt.mem[cmt.pc] % 0x10;
    adr = (off_t)cmt.mem[cmt.pc + 1];

    if(gr < 0 || gr > 4) {
        temp = cmt.mem[cmt.pc];
        printf("????:mem[%x] = %x\n", cmt.pc, temp);
        return 0;
    }
    if(xr < 0 || xr > 4) {
        temp = cmt.mem[cmt.pc];
        printf("????:mem[%x] = %x\n", cmt.pc, temp);
        return 0;
    }
    if(xr != 0) adr += cmt.gr[xr];

    comet_io();

    switch(op) {
        case HALT:
            cmt.pc += 1;
            return 0;
        case LD:
            cmt.pc += 2;
            cmt.gr[gr] = cmt.mem[adr];
            return 1;
        case ST:
            cmt.pc += 2;
            cmt.mem[adr] = cmt.gr[gr];
            return 1;

        case LEA:
            cmt.pc += 2;
            cmt.gr[gr] = adr;
            cmt.fr = cmt.gr[gr];
            return 1;
        case ADD:
            cmt.pc += 2;
            cmt.gr[gr] += cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case SUB:
            cmt.pc += 2;
            cmt.gr[gr] -= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case MUL:
            cmt.pc += 2;
            cmt.gr[gr] *= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case DIV:
            cmt.pc += 2;
            cmt.gr[gr] /= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case MOD:
            cmt.pc += 2;
            cmt.gr[gr] %= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case AND:
            cmt.pc += 2;
            cmt.gr[gr] &= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case OR :
            cmt.pc += 2;
            cmt.gr[gr] |= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case EOR:
            cmt.pc += 2;
            cmt.gr[gr] ^= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;

        case SLA:
            cmt.pc += 2;
            cmt.gr[gr] <<= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case SRA:
            cmt.pc += 2;
            cmt.gr[gr] >>= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;

        case SLL:
            cmt.pc += 2;
            x1 = cmt.mem[gr];
            x1 <<= cmt.mem[adr];
            cmt.mem[gr] = cmt.fr = x1;
            return 1;
        case SRL:
            cmt.pc += 2;
            x1 = cmt.mem[gr];
            x1 >>= cmt.mem[adr];
            cmt.mem[gr] = cmt.fr = x1;
            return 1;

        case CPA:
            cmt.pc += 2;
            cmt.fr = cmt.gr[gr] - cmt.mem[adr];
            return 1;
        case CPL:
            cmt.pc += 2;
            x1 = cmt.gr[gr];
            x2 = cmt.mem[adr];
            cmt.fr = x1 - x2;
            return 1;

        case JMP:
            cmt.pc += 2;
            cmt.pc = adr;
            return 1;
        case JPZ:
            cmt.pc += 2;
            if(cmt.fr >= 0) cmt.pc = adr;
            return 1;
        case JMI:
            cmt.pc += 2;
            if(cmt.fr < 0) cmt.pc = adr;
            return 1;
        case JNZ:
            cmt.pc += 2;
            if(cmt.fr != 0) cmt.pc = adr;
            return 1;
        case JZE:
            cmt.pc += 2;
            if(cmt.fr == 0) cmt.pc = adr;
            return 1;

        case PUSH:
            cmt.pc += 2;
            x1 = --cmt.gr[4];
            cmt.mem[x1] = cmt.mem[adr];
            return 1;
        case POP:
            cmt.pc += 1;
            x1 = cmt.gr[4]++;
            cmt.gr[gr] = cmt.mem[x1];
            return 1;

        case CALL:
            cmt.pc += 2;
            x1 = --cmt.gr[4];
            cmt.mem[x1] = cmt.pc;
            cmt.pc = cmt.mem[adr];
            return 1;
        case RET:
            cmt.pc += 1;
            x1 = cmt.gr[4]++;
            cmt.pc = cmt.mem[x1];
            return 1;

        default :
            temp = cmt.mem[cmt.pc];
            printf("????:mem[%x] = %x\n", cmt.pc, temp);
            return 0;
    }
}

/* ????,???? */

void
writeIns(off_t pc, off_t n)
{
    off_t op, gr, adr, xr;
    off_t i;

    for(i = 0; i < n; ++i) {
        op = cmt.mem[pc] / 0x100;
        gr = cmt.mem[pc] % 0x100 / 0x10;
        xr = cmt.mem[pc] % 0x10;
        adr = cmt.mem[pc + 1];
        if(op > RET) {
            printf("mem[%-4x]: ??\n", pc);
            return;
        }
        if(gr < 0 || gr > 4) {
            printf("mem[%-4x]: ??\n", pc);
            return;
        }
        if(xr < 0 || xr > 4) {
            printf("mem[%-4x]: ??\n", pc);
            return;
        }
        printf("mem[%-4x]: %s\t", pc, opTab[op].str);
        if(op == HALT || op == RET) {
            printf("\n");
            pc += 1;
            continue;
        }else if(op == POP) {
            printf("GR%d\n", gr);
            pc += 1;
            continue;
        }if(op < CPL) {
            printf("GR%d, %x", gr, adr);
            pc += 2;
        }else {
            printf("%x", adr);
            pc += 2;
        }
        if(xr != 0) printf(", GR%d", xr);
        printf("\n");
    }
}

/* ??comet????? */

void
comet_debug(void)
{
    off_t stepcnt = 0;
    off_t pntflag = 0;
    off_t traflag = 0;

    char buf[32], s[32];
    off_t x1, x2;
    int i, n, cmd;

    printf("?? (???? help)...\n\n");

LOOP:

    do {
        fflush(stdin);
        printf ("????: ");
        fgets(buf, NELEMS(buf), stdin);
        n = sscanf(buf, "%s %x %x", s, &x1, &x2);
        for(i = 0, cmd = -1; i < NELEMS(dbTab); ++i) {
            if(!strcmp(dbTab[i].s1, s) ||
                !strcmp(dbTab[i].s2, s)) {
                cmd = dbTab[i].db;
                break;
            }
        }
    }while(n <= 0);

    switch(cmd) {
        case HELP:
            printf("????:\n");
            printf("  h)elp           ???????\n");
            printf("  g)o             ????????\n");
            printf("  s)tep  <n>      ?? n ??? (??? 1 )\n");
            printf("  j)ump  <b>      ??? b ?? (???????)\n");
            printf("  r)egs           ???????\n");
            printf("  i)Mem  <b <n>>  ??? b ?? n ?????\n");
            printf("  d)Mem  <b <n>>  ??? b ?? n ?????\n");
            printf("  a(lter <b <v>>  ?? b ???????? v ?\n");
            printf("  t)race          ????????\n");
            printf("  p)rint          ????????\n");
            printf("  c)lear          ???????\n");
            printf("  q)uit           ?????\n");
            break;

        case GO:
            stepcnt = 0;
            do { stepcnt++;
                if(traflag) writeIns(cmt.pc, 1);
            }while(comet_step());
            if(pntflag)
                printf("?????? = %d\n", stepcnt);
            break;

        case STEP:
            if(n >= 2) stepcnt = x1;
            else stepcnt = 1;
            for(i = 0; i < stepcnt; ++i) {
                if(traflag) writeIns(cmt.pc, 1);
                if(!comet_step()) break;
            }
            if(pntflag)
                printf("?????? = %d\n", i);
            break;

        case JUMP:
            if(n < 2) x1 = cmt.pc;
            cmt.pc = x1;
            printf("????? %x\n", x1);
            break;

        case REGS:
            printf("???????\n");
            printf("GR[0] = %4x\tPC = %4x\n",
                (off_t)cmt.gr[0], cmt.pc);
            printf("GR[1] = %4x\tSP = %4x\n",
                (off_t)cmt.gr[1], (off_t)cmt.gr[4]);
            printf("GR[2] = %4x\t", (off_t)cmt.gr[2]);
            if(cmt.fr > 0) printf("FR =   00\n");
            else if(cmt.fr < 0) printf("FR =   10\n");
            else printf("FR =   01\n");
            printf("GR[3] = %4x\n", (off_t)cmt.gr[3]);
            break;

        case IMEM:
            printf("??????\n");
            if(n < 2) x1 = cmt.pc;
            if(n < 3) x2 = 1;
            writeIns(x1, x2);
            break;


        case DMEM:
            printf("??????\n");
            if(n < 2) x1 = cmt.pc;
            if(n < 3) x2 = 1;
            if(x2 < 0) printf("????\n");
            for(i = 0; i < x2; ++i) {
                off_t temp;
                temp = cmt.mem[x1];
                printf("mem[%-4x] = %x\n", x1, temp);
                x1++;
            }
            break;

        case ALTER:
            printf("?????? ");
            if(n == 3) {
                printf(" mem[%x] = %x\n", x1, x2);
                cmt.mem[x1] = x2;
            }else printf("??!\n");
            break;

        case TRACE:
            traflag = !traflag;
            printf("??????");
            if(traflag) printf("??\n");
            else printf("??\n");
            break;

        case PRINT:
            pntflag = !pntflag;
            printf("??????");
            if(pntflag) printf("??\n");
            else printf("??\n");
            break;

        case CLEAR:
            printf("????????\n");
            comet_load();
            break;

        case QUIT:
            printf("????...\n");
            return;

        default :
            printf("???? %s\n", s);
            break;
    }
    goto LOOP;
}

/* ??????? */

void
init(int n, char*v[])
{
    int len;
    char *s;
    if(system("cls")) system("clear");
    printf("===============\n");
    printf("COMET?????\n");
    printf("===============\n\n");
    if(n != 2 && n != 3) {
        printf("??: %s [-d(ebug] <???>\n", v[0]);
        exit(1);
    }
    len = strlen(v[n-1]);
    if(len > 16) {
        printf("?????");
        exit(1);
    }
    strcpy(pgmName, v[n-1]);
    s = strchr(pgmName, '.');
    if(s != NULL) {
        if(strcmp(s, ".comet")) {
            printf("%s ??comet??\n", pgmName);
            exit(1);
        }
        s = '\0';
    }
    strcat(pgmName, ".comet");
    if(n == 3) {
        if(strcmp(v[n-2], "-d") &&
            strcmp(v[n-2], "-debug")) {
            printf("??: %s [-d(ebug] <???>\n", v[0]);
            exit(1);
        }
        debug = 1;
    }
    source = fopen(pgmName, "rb");
    if(source == NULL) {
        printf("%s ??????\n", pgmName);
        exit(1);
    }
    comet_load();
}

/* ??? */

int
main(int argc, char *argv[])
{
    init(argc, argv);
    if(debug) comet_debug();
    else while(comet_step());
    fclose(source);
    return 0;
}

#endif
