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
    writln(outf,"\tLIDP\t"+QString("%1").arg(adr)+"\t; Variable "+nm+" = "+QString("%1").arg(value));
    if (size == 1) {
        if (value == 0) writln(outf,"\tRA");
        else  writln(outf,tr("\tLIA\t%1").arg(value));
        writln(outf,"\tSTD");
    }
    else {
        if (value % 256 == 0)  writln(outf, "\tRA");
        else writln(outf, tr("\tLIA\tLB(%1)").arg(value));

        writln(outf, "\tSTD");
        if ( (adr/256) == ((adr+1)/256) ) writln(outf,tr("\tLIDL\tLB(%1)").arg(adr+1));
        else writln(outf,tr("\tLIDP\t%1").arg(adr+1));

        if ((value %256) != (value / 256)) {
            if ((value / 256) == 0) writln(outf, "\tRA");
            else writln(outf,tr("\tLIA\tHB(%1)").arg(value));
        }
        writln(outf, "\tSTD");

    }
    writln(outf,"");
}
//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ Generates init code for an array in xram }


void Clcc::varxarr(QByteArray value,int adr, int size,QByteArray nm, QByteArray typ) {

    int v,c;
    QByteArray s;

    if (size == 0) return;

    s = "";
    for (int i = 0 ; i< size;i++) {
        if (i < value.length()) {
            if (typ !="word") s.append(tr("%1").arg((unsigned char)value.at(i)));
            else s.append(tr("%1").arg(256*value[i*2]+value[i*2+1]));
        }
        else s.append("0");

        if (i < (size-1))s.append(", ");
    }

    if ((typ != "word") && (size <= 5)) {
        // Set up address and write 1st byte
        writln(outf, "\tLIDP\t"+tr("%1").arg(adr)+"\t; Variable "+nm+" = ("+s+")");
        v = value[0];
        if (v == 0) writln(outf,"\tRA");
        else writln(outf,tr("\tLIA\t%1").arg(v));
        writln(outf,"\tSTD");

        c = v;
        if (size > 1) {
            for (int i = 1 ;i< size;i++) {
                if (((adr+i-1) / 256) == ((adr+i) / 256)) writln(outf,tr("\tLIDL\tLB(%1)").arg(adr+i));
                else writln(outf,tr("\tLIDP\t%1").arg(adr+i));

                if (i < value.size()) v = value[i];
                else v = 0;

                if (v != c) {
                    if (v == 0) writln(outf,"\tRA");
                    else writln(outf,tr("\tLIA\t%1").arg(v));
                    writln(outf,"\tSTD");
                }
                else writln(outf,"\tSTD");
                c = v;
            }
        }
    }
    else if (typ != "word") {
        writln(outf,"\t; Variable "+nm+" = ("+s+")");
        load_x(nm+"-1");
        load_y(QByteArray::number(adr-1));
        writln(outf, tr("\tLII\t%1\t; Load I as counter").arg(size));
        writln(outf, "\tIXL");
        writln(outf, "\tIYS");
        writln(outf, "\tDECI");
        writln(outf, "\tJRNZM\t4");

        addasm(nm+":\t; Variable init data "+nm);
        s="\t.DB\t" + s;
        addasm(s);
        addasm("");
    }
    else {
        writln(outf, "\t; Variable "+nm+" = ("+s+")");
        load_x(nm+"-1");
        load_y(QByteArray::number(adr-1));
        writln(outf, tr("\tLII\t%1\t; Load I as counter").arg(size*2));
        writln(outf, "\tIXL");
        writln(outf, "\tIYS");
        writln(outf, "\tDECI");
        writln(outf, "\tJRNZM\t4");

        addasm(nm+":\t; Variable init data "+nm);
        s="\t.DW\t" + s;
        addasm(s);
        addasm("");
    }
    writln(outf,"");
}
//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ Generates init code for an array variable in a register }

//void Clcc::varrarr(QList<unsigned char> value,int adr, int size,QByteArray nm, QByteArray typ) {
void Clcc::varrarr(QByteArray value,int adr, int size,QByteArray nm, QByteArray typ) {
    QByteArray s;

    if (size == 0) return;
    s="";
    // TODO : convert \n \t \r to corresponding char value
    value.replace("\\n","\n").replace("\\r","\r");
    for (int i=0 ; i< size; i++) {
        if (i <= value.size()) {
            if (typ !="word") s.append(tr("%1").arg((unsigned char)value.at(i)));
            else s.append(tr("%1").arg(256*value[i*2]+value[i*2+1]));
        }
        else s.append("0");

        if (i < (size-1)) s.append(", ");
    }

    if (typ !="word") writln(outf,tr("\tLII\t%1").arg(size-1)+"\t; Variable "+nm+" = ("+s+")");
    else writln(outf,tr("\tLII\t%1").arg(size*2-1)+"\t; Variable "+nm+" = ("+s+")");
    writln(outf,"\tLIDP\t"+nm);
    if (adr <= 63) writln(outf,tr("\tLP\t%1").arg(adr));
    else writln(outf,tr("\tLIP\t%1").arg(adr));
    writln(outf,"\tMVWD");

    addasm(nm+":\t; Variable init data "+nm);
    if (typ != "word") s="\t.DB\t" + s;
    else s="\t.DW\t" + s;
    addasm(s);
    addasm("");

    writln(outf,"");
}
//{--------------------------------------------------------------}



void Clcc::load_x(QByteArray s) {
        writln(outf, "\tLP\t4\t; Load XL");
        writln(outf, "\tLIA\tLB("+s+")");
        writln(outf, "\tEXAM");
        writln(outf, "\tLP\t5\t; Load XH");
        writln(outf, "\tLIA\tHB("+s+")");
        writln(outf, "\tEXAM");
    }


void Clcc::load_y(QByteArray s) {
        writln(outf, "\tLP\t6\t; Load YL");
        writln(outf, "\tLIA\tLB("+s+")");
        writln(outf, "\tEXAM");
        writln(outf, "\tLP\t7\t; Load YH");
        writln(outf, "\tLIA\tHB("+s+")");
        writln(outf, "\tEXAM");
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
//void Clcc::varcarr(QList<unsigned char> value, int adr, int size,QByteArray nm, QByteArray typ) {
void Clcc::varcarr(QByteArray value, int adr, int size,QByteArray nm, QByteArray typ) {
    QByteArray s;
    if (size == 0) return;

    s ="";
    for (int i = 0; i< size;i++) {
        if (i < value.size()) {
            if (typ !="word") s.append(tr("%1").arg((unsigned char)value.at(i)));
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
        if (adr == 0) writln(outf,tr("\tLII\t%1").arg(value)+"\t; I is "+nm+tr(" = %1").arg(value));
        else writln(outf,tr("\tLIJ\t%1").arg(value)+"\t; J is "+nm+tr(" = %1").arg(value));
        writln(outf,"");
        return;
    }

    if (adr <= 63) writln(outf,tr("\tLP\t%1").arg(adr)+"\t; Variable "+nm+tr(" = %1").arg(value));
    else {
        writln(outf,tr("\tLIP\t%1").arg(adr)+"\t; Variable "+nm+tr(" = %1").arg(value));
    }
    if (size == 1) {
        writln(outf,tr("\tLIA\t%1").arg(value));
        writln(outf,"\tEXAM");
    }
    else {
        if ((value % 256) == 0) writln(outf,"\tRA\t; LB");
        else writln(outf,tr("\tLIA\tLB(%1)\t;LB").arg(value));
        writln(outf,"\tEXAM");

        writln(outf,"\tINCP");
        if ((value % 256) != (value / 256)) {
            if ((value / 256) == 0) writln(outf,"\tRA\t; HB");
            else writln(outf,tr("\tLIA\tHB(%1)\t; HB").arg(value));
        }
        writln(outf,"\tEXAM");
    }
    writln(outf,"");
}

//{---------------------------------------------------------------}
//{ Comparison Equal }

void Clcc::CompEqual(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_CPE16\t; Compare for equal");
        addlib(CPE16);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tLP\t3");
        writln(outf,"\tCPMA\t\t; Compare for equal");
        writln(outf,"\tRA");
        writln(outf,"\tJRNZP\t2");
        writln(outf,"\tDECA");
    }
    writln(outf,"");
}


//{---------------------------------------------------------------}
//{ Comparison Not Equal }

void Clcc::CompNotEqual(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_CPNE16\t; Compare not equal");
        addlib(CPNE16);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tLP\t3");
          writln(outf,"\tCPMA\t\t; Compare for not equal");
          writln(outf,"\tRA");
          writln(outf,"\tJRZP\t2");
        writln(outf,"\tDECA");
    }
    writln(outf,"");
}


//{---------------------------------------------------------------}
//{ Comparison Greater or Equal }

void Clcc::CompGrOrEq(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_CPGE16\t; Compare for Greater or Equal");
        addlib(CPGE16);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t3");
        writln(outf,"\tCPMA\t\t; Compare for Greater or Equal");
        writln(outf,"\tRA");
        writln(outf,"\tJRCP\t2");
        writln(outf,"\tDECA");
    }
    writln(outf,"");
}


//{---------------------------------------------------------------}
//{ Comparison smaller or equal }

void Clcc::CompSmOrEq(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_CPSE16\t; Compare for smaller or equal");
        addlib(CPSE16);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tLP\t3");
        writln(outf,"\tCPMA\t\t; Compare for smaller or equal");
        writln(outf,"\tRA");
        writln(outf,"\tJRCP\t2");
        writln(outf,"\tDECA");
    }
    writln(outf,"");
}


//{---------------------------------------------------------------}
//{ Comparison Greater }

void Clcc::CompGreater(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_CPG16\t; Compare for greater");
        addlib(CPG16);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tLP\t3");
          writln(outf,"\tCPMA\t\t; Compare for greater");
          writln(outf,"\tRA");
          writln(outf,"\tJRNCP\t2");
        writln(outf,"\tDECA");
    }
    writln(outf,"");
}


//{---------------------------------------------------------------}
//{ Comparison smaller }

void Clcc::CompSmaller(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_CPS16\t; Compare for smaller");
        addlib(CPS16);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t3");
        writln(outf,"\tCPMA\t\t; Compare for smaller");
        writln(outf,"\tRA");
        writln(outf,"\tJRNCP\t2");
        writln(outf,"\tDECA");
    }
    writln(outf,"");
}


//{---------------------------------------------------------------}
//{ Branch Unconditional  }

void Clcc::Branch(QByteArray L){
    writln(outf,"\tRJMP\t"+L);
}

//{---------------------------------------------------------------}
//{ Branch False }

void Clcc::BranchFalse(QByteArray L) {
    writln(outf,"\tTSIA\t255\t; Branch if false");
//    writln(outf,"\tJRZP\t"+L);
    writln(outf,"\tJPZ\t"+L);
    writln(outf,"");
}

//{--------------------------------------------------------------}
//{ Bitwise Not Primary }

void Clcc::NotIt(void) {
    if (isword) {
        writln(outf,"\tLP\t1");
        writln(outf,"\tORIM\t255");
        writln(outf,"\tLP\t0");
        writln(outf,"\tORIM\t255");
        writln(outf,"\tSBB\t\t; Negate");
        writln(outf,"\tLP\t1");
        writln(outf,"\tLDM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t0");
        writln(outf,"\tLDM");
    }
    else {
        writln(outf,"\tLIB\t0");
        writln(outf,"\tLP\t3");
        writln(outf,"\tSBM\t\t; Negate");
        writln(outf,"\tEXAB");
    }
}


//{--------------------------------------------------------------}
//{ Negate Primary }

void Clcc::Negate(void) {
    if (isword) {
        writln(outf,"\tLP\t1");
        writln(outf,"\tORIM\t255");
        writln(outf,"\tLP\t0");
        writln(outf,"\tORIM\t255");
        writln(outf,"\tSBB\t\t; Negate");
        writln(outf,"\tLP\t1");
        writln(outf,"\tLDM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t0");
        writln(outf,"\tLDM");
    }
    else {
        writln(outf,"\tLIB\t0");
        writln(outf,"\tLP\t3");
        writln(outf,"\tSBM\t\t; Negate");
        writln(outf,"\tEXAB");
    }
}

//{--------------------------------------------------------------}
//{ Or TOS with Primary }

void Clcc::PopOr(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tORMA\t\t; OR HB");
        writln(outf,"\tLP\t0");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tORMA\t\t; OR LB");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tLP\t3");
        writln(outf,"\tORMA\t\t; OR");
        writln(outf,"\tEXAB");
    }
}


//{--------------------------------------------------------------}
//{ Exclusive-Or TOS with Primary }

void Clcc::PopXor(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_XOR16\t; XOR");
        addlib(XOR16);
        addlib(XOR8);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_XOR8\t; XOR");
        addlib(XOR8);
    }
}

//{--------------------------------------------------------------}
//{ Shift left TOS with Primary }

void Clcc::PopSl(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_SL16\t; SL");
        addlib(SL16);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_SL8\t; Shift left");
        addlib(SL8);
    }
}


//{--------------------------------------------------------------}
//{ Shift right TOS with Primary }

void Clcc::PopSr(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_SR16\t; SR");
        addlib(SR16);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_SR8\t; Shift right");
        addlib(SR8);
    }
}

//{--------------------------------------------------------------}
//{ Modulo TOS with Primary }
void Clcc::PopMod(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_MOD16\t; Modulo");
        addlib(DIVMOD16);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_MOD8\t; Modulo");
        writln(outf,"\tEXAB");
        addlib(DIVMOD8);
    }
}
//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ And Primary with TOS }

void Clcc::PopAnd(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tANMA\t\t; AND HB");
        writln(outf,"\tLP\t0");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tANMA\t\t; AND LB");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tLP\t3");
        writln(outf,"\tANMA\t\t; AND");
        writln(outf,"\tEXAB");
    }
}
//{--------------------------------------------------------------}


///{--------------------------------------------------------------}
//{ Push Primary to Stack }

void Clcc::Push(void) {
    if (isword) {
        writln(outf,"\tPUSH\t\t; Push A then B"); pushcnt++;
        writln(outf,"\tEXAB");
        writln(outf,"\tPUSH"); pushcnt++;
    }
    else {
        writln(outf,"\tPUSH"); pushcnt++;
    }
}


//{--------------------------------------------------------------}
//{ Add TOS to Primary }

void Clcc::PopAdd(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
//        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t0");
        writln(outf,"\tADB\t\t; Addition");
        writln(outf,"\tLP\t1");
        writln(outf,"\tLDM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t0");
        writln(outf,"\tLDM");
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tLP\t3");
        writln(outf,"\tADM\t\t; Addition");
        writln(outf,"\tEXAB");
    }
}


//{--------------------------------------------------------------}
//{ Subtract TOS from Primary }

void Clcc::PopSub(void) {
    if (isword) {
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAM");
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAM");
        writln(outf,"\tSBB\t\t; Subtraction");
        writln(outf,"\tLP\t1");
        writln(outf,"\tLDM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t0");
        writln(outf,"\tLDM");
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tLP\t3");
        writln(outf,"\tEXAB");
        writln(outf,"\tSBM\t\t; Subtraction");
        writln(outf,"\tEXAB");
    }
}

//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ Multiply TOS by Primary }

void Clcc::PopMul(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_MUL16\t; Multiplication");
        addlib(MUL16);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_MUL8\t; Multiplication");
        addlib(MUL8);
    }
}

//{--------------------------------------------------------------}
//{ Divide Primary by TOS }

void Clcc::PopDiv(void) {
    if (isword) {
        writln(outf,"\tLP\t0");
        writln(outf,"\tEXAM");
        writln(outf,"\tEXAB");
        writln(outf,"\tLP\t1");
        writln(outf,"\tEXAM");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_DIV16\t; Division");
        addlib(DIVMOD16);
    }
    else {
        writln(outf,"\tEXAB");
        writln(outf,"\tPOP"); pushcnt--;
        writln(outf,"\tCALL\tLIB_DIV8\t; Division");
        addlib(DIVMOD8);
    }
}
//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ Load the Primary Register with a Constant }

void Clcc::LoadConstant(QByteArray n) {
    if (isword) {
        writln(outf,"\tLIA\tLB("+n+")\t; Load constant LB "+n);
        writln(outf,"\tLIB\tHB("+n+")\t; Load constant HB "+n);
    }
    else {

        if (n.toInt() == 0) writln(outf, "\tRA\t\t; Load 0");
        else writln(outf,"\tLIA\t"+n+"\t; Load constant "+n);
    }
}
