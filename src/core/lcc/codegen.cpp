#include "lcc.h"



//{--------------------------------------------------------------}
//{ Inserts library code for used libs }
void Clcc::addlib(int libid) {
    if (libcnt == 0)
        libtext = libtext + "; LIB Code\r\n";
    libcnt++;

    if (!libins.contains(libid)) {
        libins.append(libid);
        libtext = libtext + ".include "+libname[libid]+".lib\r\n";
    }
}


void  Clcc::addasm(QByteArray s) {
    asmlist.append(s+"\r\n");
    asmcnt++;
}

//{ Generates init code for a var in xram }

void Clcc::varxram(int value, int adr, int size, QByteArray nm) {
    writln("asm","\tLIDP\t"+QString("%1").arg(adr)+"\t; Variable "+nm+" = "+QString("%1").arg(value));
    if (size == 1) {
        if (value == 0) writln("asm","\tRA");
        else  writln("asm",tr("\tLIA\t%1").arg(value));
        writln("asm","\tSTD");
    }
    else {
        if (value % 256 == 0)  writln("asm", "\tRA");
        else writln("asm", tr("\tLIA\tLB(%1)").arg(value));

        writln("asm", "\tSTD");
        if ( (adr/256) == ((adr+1)/256) ) writln("asm",tr("\tLIDL\tLB(%1)").arg(adr+1));
        else writln("asm",tr("\tLIDP\t%1").arg(adr+1));

        if ((value %256) != (value / 256)) {
            if ((value / 256) == 0) writln("asm", "\tRA");
            else writln("asm",tr("\tLIA\tHB(%1)").arg(value));
        }
        writln("asm", "\tSTD");

    }
    writln("asm","");
}
//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ Generates init code for an array in xram }

void Clcc::varxarr(QList<unsigned char> value,int adr, int size,QByteArray nm, QByteArray typ) {
    //var i: integer;
    //    v, c: integer;
    //    s: string;
    //begin

    int v,c;
    QByteArray s;

    if (size == 0) return;

    s = "";
    for (int i = 0 ; i< size;i++) {
        if (i < value.length()) {
            if (typ !="word") s.append(tr("%1").arg(value[i]));
            else s.append(tr("%1").arg(256*value[i*2]+value[i*2+1]));
        }
        else s.append("0");

        if (i < (size-1))s.append(", ");
    }

    if ((typ != "word") && (size <= 5)) {
        // Set up address and write 1st byte
        writln("asm", "\tLIDP\t"+tr("%1").arg(adr)+"\t; Variable "+nm+" = ("+s+")");
        v = value[0];
        if (v == 0) writln("asm","\tRA");
        else writln("asm",tr("\tLIA\t%1").arg(v));
        writln("asm","\tSTD");

        c = v;
        if (size > 1) {
            for (int i = 1 ;i< size;i++) {
                if (((adr+i-1) / 256) == ((adr+i) / 256)) writln("asm",tr("\tLIDL\tLB(%1)").arg(adr+i));
                else writln("asm",tr("\tLIDP\t%1").arg(adr+i));

                if (i < value.size()) v = value[i];
                else v = 0;

                if (v != c) {
                    if (v == 0) writln("asm","\tRA");
                    else writln("asm",tr("\tLIA\t%1").arg(v));
                    writln("asm","\tSTD");
                }
                else writln("asm","\tSTD");
                c = v;
            }
        }
    }
    else if (typ != "word") {
        writln("asm","\t; Variable "+nm+" = ("+s+")");
        load_x(nm+"-1");
        load_y(QByteArray::number(adr-1));
        writln("asm", tr("\tLII\t%1\t; Load I as counter").arg(size));
        writln("asm", "\tIXL");
        writln("asm", "\tIYS");
        writln("asm", "\tDECI");
        writln("asm", "\tJRNZM\t4");

        addasm(nm+":\t; Variable init data "+nm);
        s="\t.DB\t" + s;
        addasm(s);
        addasm("");
    }
    else {
        writln("asm", "\t; Variable "+nm+" = ("+s+")");
        load_x(nm+"-1");
        load_y(QByteArray::number(adr-1));
        writln("asm", tr("\tLII\t%1\t; Load I as counter").arg(size*2));
        writln("asm", "\tIXL");
        writln("asm", "\tIYS");
        writln("asm", "\tDECI");
        writln("asm", "\tJRNZM\t4");

        addasm(nm+":\t; Variable init data "+nm);
        s="\t.DW\t" + s;
        addasm(s);
        addasm("");
    }
    writln("asm","");
}
//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ Generates init code for an array variable in a register }

void Clcc::varrarr(QList<unsigned char> value,int adr, int size,QByteArray nm, QByteArray typ) {
    QByteArray s;

    if (size == 0) return;
    s="";
    for (int i=0 ; i< size; i++) {
        if (i <= value.size()) {
            if (typ !="word") s.append(tr("%1").arg(value[i]));
            else s.append(tr("%1").arg(256*value[i*2]+value[i*2+1]));
        }
        else s.append("0");

        if (i < size) s.append(", ");
    }

    if (typ !="word") writln("asm",tr("\tLII\t%1").arg(size-1)+"\t; Variable "+nm+" = ("+s+")");
    else writln("asm",tr("\tLII\t%1").arg(size*2-1)+"\t; Variable "+nm+" = ("+s+")");
    writln("asm","\tLIDP\t"+nm);
    if (adr <= 63) writln("asm",tr("\tLP\t%1").arg(adr));
    else writln("asm",tr("\tLIP\t%1").arg(adr));
    writln("asm","\tMVWD");

    addasm(nm+":\t; Variable init data "+nm);
    if (typ != "word") s="\t.DB\t" + s;
    else s="\t.DW\t" + s;
    addasm(s);
    addasm("");

    writln("asm","");
}
//{--------------------------------------------------------------}



void Clcc::load_x(QByteArray s) {
        writln("asm", "\tLP\t4\t; Load XL");
        writln("asm", "\tLIA\tLB("+s+")");
        writln("asm", "\tEXAM");
        writln("asm", "\tLP\t5\t; Load XH");
        writln("asm", "\tLIA\tHB("+s+")");
        writln("asm", "\tEXAM");
    }


void Clcc::load_y(QByteArray s) {
        writln("asm", "\tLP\t6\t; Load YL");
        writln("asm", "\tLIA\tLB("+s+")");
        writln("asm", "\tEXAM");
        writln("asm", "\tLP\t7\t; Load YH");
        writln("asm", "\tLIA\tHB("+s+")");
        writln("asm", "\tEXAM");
    }

//{--------------------------------------------------------------}
//{ Generates init code for a var in code space }

void Clcc::varcode(int value, int adr, int size,QByteArray nm) {
    if (value == -1) value = 0;
    addasm(nm+":\t; Variable "+nm+" = "+QByteArray::number(value));
    if (size == 1) addasm("\t.DB\t"+QByteArray::number(value));
    else addasm("\t.DW\t"+QByteArray::number(value));
    addasm("");
}
//{--------------------------------------------------------------}

//{--------------------------------------------------------------}
//{ Generates init code for an array in code space }

void Clcc::varcarr(QList<unsigned char> value, int adr, int size,QByteArray nm, QByteArray typ) {
    QByteArray s;
    if (size == 0) return;

    s ="";
    for (int i = 0; i< size;i++) {
        if (i < value.size()) {
            if (typ !="word") s.append(tr("%1").arg(value[i]));
            else s.append(tr("%1").arg(256*value[i*2]+value[i*2+1]));
        }
        else s.append("0");

        if (i < (size-1)) s.append(", ");
    }

    addasm(nm+":\t; Variable "+nm+" = ("+s+")");
    if (typ !="word") s = "\t.DB\t" + s;
    else s = "\t.DW\t" + s;
    addasm(s);
    addasm("");
}
//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ Generates init code for a variable in a register }

void Clcc::varreg(int value, int adr, int size, QByteArray nm) {
    //        { Check for named register }
    if (((adr == 0) || (adr == 1)) && (size == 1)) {
        if (adr == 0) writln("asm",tr("\tLII\t%1").arg(value)+"\t; I is "+nm+tr(" = %1").arg(value));
        else writln("asm",tr("\tLIJ\t%1").arg(value)+"\t; J is "+nm+tr(" = %1").arg(value));
        writln("asm","");
        return;
    }

    if (adr <= 63) writln("asm",tr("\tLP\t%1").arg(adr)+"\t; Variable "+nm+tr(" = %1").arg(value));
    else {
        writln("asm",tr("\tLIP\t%1").arg(adr)+"\t; Variable "+nm+tr(" = %1").arg(value));
    }
    if (size == 1) {
        writln("asm",tr("\tLIA\t%1").arg(value));
        writln("asm","\tEXAM");
    }
    else {
        if ((value % 256) == 0) writln("asm","\tRA\t; LB");
        else writln("asm",tr("\tLIA\tLB(%1)\t;LB").arg(value));
        writln("asm","\tEXAM");

        writln("asm","\tINCP");
        if ((value % 256) != (value / 256)) {
            if ((value / 256) == 0) writln("asm","\tRA\t; HB");
            else writln("asm",tr("\tLIA\tHB(%1)\t; HB").arg(value));
        }
        writln("asm","\tEXAM");
    }
    writln("asm","");
}

//{---------------------------------------------------------------}
//{ Comparison Equal }

void Clcc::CompEqual(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_CPE16\t; Compare for equal");
        addlib(CPE16);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tLP\t3");
        writln("asm","\tCPMA\t\t; Compare for equal");
        writln("asm","\tRA");
        writln("asm","\tJRNZP\t2");
        writln("asm","\tDECA");
    }
    writln("asm","");
}


//{---------------------------------------------------------------}
//{ Comparison Not Equal }

void Clcc::CompNotEqual(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_CPNE16\t; Compare not equal");
        addlib(CPNE16);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tLP\t3");
          writln("asm","\tCPMA\t\t; Compare for not equal");
          writln("asm","\tRA");
          writln("asm","\tJRZP\t2");
        writln("asm","\tDECA");
    }
    writln("asm","");
}


//{---------------------------------------------------------------}
//{ Comparison Greater or Equal }

void Clcc::CompGrOrEq(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_CPGE16\t; Compare for Greater or Equal");
        addlib(CPGE16);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tLP\t3");
        writln("asm","\tCPMA\t\t; Compare for Greater or Equal");
        writln("asm","\tRA");
        writln("asm","\tJRCP\t2");
        writln("asm","\tDECA");
    }
    writln("asm","");
}


//{---------------------------------------------------------------}
//{ Comparison smaller or equal }

void Clcc::CompSmOrEq(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_CPSE16\t; Compare for smaller or equal");
        addlib(CPSE16);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tLP\t3");
        writln("asm","\tCPMA\t\t; Compare for smaller or equal");
        writln("asm","\tRA");
        writln("asm","\tJRCP\t2");
        writln("asm","\tDECA");
    }
    writln("asm","");
}


//{---------------------------------------------------------------}
//{ Comparison Greater }

void Clcc::CompGreater(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_CPG16\t; Compare for greater");
        addlib(CPG16);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tLP\t3");
          writln("asm","\tCPMA\t\t; Compare for greater");
          writln("asm","\tRA");
          writln("asm","\tJRNCP\t2");
        writln("asm","\tDECA");
    }
    writln("asm","");
}


//{---------------------------------------------------------------}
//{ Comparison smaller }

void Clcc::CompSmaller(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_CPS16\t; Compare for smaller");
        addlib(CPS16);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tLP\t3");
        writln("asm","\tCPMA\t\t; Compare for smaller");
        writln("asm","\tRA");
        writln("asm","\tJRNCP\t2");
        writln("asm","\tDECA");
    }
    writln("asm","");
}


//{---------------------------------------------------------------}
//{ Branch Unconditional  }

void Clcc::Branch(QByteArray L){
    writln("asm","\tRJMP\t"+L);
}

//{---------------------------------------------------------------}
//{ Branch False }

void Clcc::BranchFalse(QByteArray L) {
    writln("asm","\tTSIA\t255\t; Branch if false");
    writln("asm","\tJRZP\t"+L);
    writln("asm","");
}

//{--------------------------------------------------------------}
//{ Bitwise Not Primary }

void Clcc::NotIt(void) {
    if (isword) {
        writln("asm","\tLP\t1");
        writln("asm","\tORIM\t255");
        writln("asm","\tLP\t0");
        writln("asm","\tORIM\t255");
        writln("asm","\tSBB\t\t; Negate");
        writln("asm","\tLP\t1");
        writln("asm","\tLDM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t0");
        writln("asm","\tLDM");
    }
    else {
        writln("asm","\tLIB\t0");
        writln("asm","\tLP\t3");
        writln("asm","\tSBM\t\t; Negate");
        writln("asm","\tEXAB");
    }
}


//{--------------------------------------------------------------}
//{ Negate Primary }

void Clcc::Negate(void) {
    if (isword) {
        writln("asm","\tLP\t1");
        writln("asm","\tORIM\t255");
        writln("asm","\tLP\t0");
        writln("asm","\tORIM\t255");
        writln("asm","\tSBB\t\t; Negate");
        writln("asm","\tLP\t1");
        writln("asm","\tLDM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t0");
        writln("asm","\tLDM");
    }
    else {
        writln("asm","\tLIB\t0");
        writln("asm","\tLP\t3");
        writln("asm","\tSBM\t\t; Negate");
        writln("asm","\tEXAB");
    }
}

//{--------------------------------------------------------------}
//{ Or TOS with Primary }

void Clcc::PopOr(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tORMA\t\t; OR HB");
        writln("asm","\tLP\t0");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tORMA\t\t; OR LB");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tLP\t3");
        writln("asm","\tORMA\t\t; OR");
        writln("asm","\tEXAB");
    }
}


//{--------------------------------------------------------------}
//{ Exclusive-Or TOS with Primary }

void Clcc::PopXor(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_XOR16\t; XOR");
        addlib(XOR16);
        addlib(XOR8);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_XOR8\t; XOR");
        addlib(XOR8);
    }
}

//{--------------------------------------------------------------}
//{ Shift left TOS with Primary }

void Clcc::PopSl(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_SL16\t; SL");
        addlib(SL16);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_SL8\t; Shift left");
        addlib(SL8);
    }
}


//{--------------------------------------------------------------}
//{ Shift right TOS with Primary }

void Clcc::PopSr(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_SR16\t; SR");
        addlib(SR16);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_SR8\t; Shift right");
        addlib(SR8);
    }
}

//{--------------------------------------------------------------}
//{ Modulo TOS with Primary }
void Clcc::PopMod(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_MOD16\t; Modulo");
        addlib(MOD16);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_DIV8\t; Modulo");
        writln("asm","\tEXAB");
        addlib(DIVMOD8);
    }
}
//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ And Primary with TOS }

void Clcc::PopAnd(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tANMA\t\t; AND HB");
        writln("asm","\tLP\t0");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tANMA\t\t; AND LB");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tLP\t3");
        writln("asm","\tANMA\t\t; AND");
        writln("asm","\tEXAB");
    }
}
//{--------------------------------------------------------------}


///{--------------------------------------------------------------}
//{ Push Primary to Stack }

void Clcc::Push(void) {
    if (isword) {
        writln("asm","\tPUSH\t\t; Push A then B"); pushcnt++;
        writln("asm","\tEXAB");
        writln("asm","\tPUSH"); pushcnt++;
    }
    else {
        writln("asm","\tPUSH"); pushcnt++;
    }
}


//{--------------------------------------------------------------}
//{ Add TOS to Primary }

void Clcc::PopAdd(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
//      writln("asm","\tEXAB");
        writln("asm","\tLP\t0");
        writln("asm","\tADB\t\t; Addition");
        writln("asm","\tLP\t1");
        writln("asm","\tLDM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t0");
        writln("asm","\tLDM");
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tLP\t3");
        writln("asm","\tADM\t\t; Addition");
        writln("asm","\tEXAB");
    }
}


//{--------------------------------------------------------------}
//{ Subtract TOS from Primary }

void Clcc::PopSub(void) {
    if (isword) {
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAM");
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAM");
        writln("asm","\tSBB\t\t; Subtraction");
        writln("asm","\tLP\t1");
        writln("asm","\tLDM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t0");
        writln("asm","\tLDM");
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tLP\t3");
        writln("asm","\tEXAB");
        writln("asm","\tSBM\t\t; Subtraction");
        writln("asm","\tEXAB");
    }
}
//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ Multiply TOS by Primary }

void Clcc::PopMul(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_MUL16\t; Multiplication");
        addlib(MUL16);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_MUL8\t; Multiplication");
        addlib(MUL8);
    }
}

//{--------------------------------------------------------------}
//{ Divide Primary by TOS }

void Clcc::PopDiv(void) {
    if (isword) {
        writln("asm","\tLP\t0");
        writln("asm","\tEXAM");
        writln("asm","\tEXAB");
        writln("asm","\tLP\t1");
        writln("asm","\tEXAM");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_DIV16\t; Division");
        addlib(DIV16);
    }
    else {
        writln("asm","\tEXAB");
        writln("asm","\tPOP"); pushcnt--;
        writln("asm","\tCALL\tLIB_DIV8\t; Division");
        addlib(DIVMOD8);
    }
}
//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ Load the Primary Register with a Constant }

void Clcc::LoadConstant(QByteArray n) {
    if (isword) {
        writln("asm","\tLIA\tLB("+n+")\t; Load constant LB "+n);
        writln("asm","\tLIB\tHB("+n+")\t; Load constant HB "+n);
    }
    else {

        if (n.toInt() == 0) writln("asm", "\tRA\t\t; Load 0");
        else writln("asm","\tLIA\t"+n+"\t; Load constant "+n);
    }
}
