#include <cctype>
#include <QString>

#define ENABLE_COMPILATION 1

#if ENABLE_COMPILATION

#include "lcc.h"

const QByteArray Alpha="_ABCDEFGHIJKLMNOPQRSTUVWXYZ";

Clcc::Clcc(QMap<QString,QByteArray> *sources,QMap<QString,QByteArray> *out) {
    this->sources = sources;
    this->out = out;
    proccount = 0;
    VarCount = 0;
    VarPos = 8;
    level = 0;

    outfile = true;
    LCount = 0;

    asmcnt = 0;

    libcnt = 0;

    libname[MUL8] = "mul8";
    libname[DIVMOD8] = "divmod8";
    libname[SR8] = "sr8";
    libname[SL8] = "sl8";
    libname[XOR8] = "xor8";

    libname[MUL16] = "mul16";
    libname[DIV16] = "div16";
    libname[MOD16] = "mod16";
    libname[AND16] = "and16";
    libname[OR16] = "or16";
    libname[XOR16] = "xor16";
    libname[SR16] = "sr16";
    libname[SL16] = "sl16";
    libname[NOT16] = "not16";
    libname[NEG16] = "neg16";

    libname[CPE16] = "cpe16";
    libname[CPNE16] = "cpne16";
    libname[CPS16] = "cps16";
    libname[CPG16] = "cpg16";
    libname[CPSE16] = "cpse16";
    libname[CPGE16] = "cpge16";

//    cg = new Clcg("outputasm");
}


void Clcc::writln(QString fname,QString s) {
    if (outfile) asmtext.append(s + "\r\n");
    else addasm(s.toAscii());

}



void Clcc::writeln(QString fname,QString s){
    write(fname,s+"\r\n");
}
void Clcc::write(QString fname,QString s){
    // emit signal ?
    // or fill an arraybuffer ?
    emit outputSignal(fname,s);
}

//{--------------------------------------------------------------}
//{ Write error Message and Halt }

void Clcc::Error(QString s) {
  if (!Tok.isEmpty()) s.append("\r\nToken: "+Tok);
  if (!dummy.isEmpty())s.append("\r\nCode: "+dummy);
  QMessageBox::about(mainwindow,"ERROR",s);

}

//{--------------------------------------------------------------}
//{ Write "<something> Expected" }

void Clcc::Expected(QString s) {
    Error(s + " Expected");
}


const int MODEKEYB = 0;
const int MODEFILE = 1;
const int MODESTR = 2;


QList<char> SpacesList = QList<char> () << ' ' << 0x09 << 0x0A << 0x0B << 0x0C << 0x0D << 0x0E;
QList<char> OpsList = QList<char> () << '<' << '>' << '+' << '-' << '*' << '/' << '~' << '&' << '|' << '!' << '%' << '^';
QList<char> Ops2List = QList<char> () << '<' << '>' << '+' << '-' << '*' << '/' << '~' << '&' << '|' << '!' << '%' << '^' << '=';
QList<char> HexList = QList<char> () << 'A' << 'B' << 'C' << 'D' << 'E' << 'F';
QList<char> NumList = QList<char> () << '0' << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9';


bool Clcc::FindProc(QByteArray t) {
    bool result = false;
    for (int i = 0; i < proclist.size(); i++) {
        if (proclist[i].ProcName.toLower() == t.toLower())
        {
            result = true;
            ProcFound = i;
            return result;
        }
    }
    return result;
}

bool Clcc::FindVar(QByteArray t) {
    bool result = false;
    for (int i = 0 ; i< varlist.size(); i++) {
        if (varlist.at(i).varname.toLower()== t.toLower()) {

            result = true;
            VarFound = i;
            return result;
        }
    }
    return result;
}

/*{--------------------------------------------------------------}
{ Test if variable is at address }
*/

bool Clcc::IsVarAtAdr(int adr,int size) {
    VarFound = -1;
    for (int i = 0; i< varlist.size(); i++) {
        if (varlist.at(i).address == adr) {
            VarFound=i;
            return true;
        }
    }
    return false;
}

void Clcc::printvarlist(void) {
    int  initn, adr, size, lproc;
    QByteArray s, name, typ;
    QList<unsigned char> inits;
    bool xr, arr, loc;

    writeln("output","");
    writeln("output","VARIABLES DECLARED:");
    writeln("output","");
    for (int i=0; i< varlist.size();i++) {
        Cvar v = varlist.at(i);
        name = v.varname;
        typ = v.typ;
        xr = v.xram;
        arr = v.arr;
        adr = v.address;
        size = v.size;
        initn = v.initn;
        inits = v.inits;
        if (v.pointer) s="*"; else s="";
        loc = v.local;
        lproc = v.locproc;

        write("output",tr("VAR %1: ").arg(i+1));
        write("output",s+name+", TYP: "+typ+", ADR: "+QString("%1, XRAM: ").arg(adr));
        if (xr) write("output","yes, LOCAL: "); else write("output","no, LOCAL: ");
        if (loc) s = proclist[lproc].ProcName;
        if (loc) write("output","yes, FUNC: "+s+", SIZE: "); else write("output","no, SIZE: ");
        write("output",QString("%1").arg(size));
        if (initn != -1) {
            write("output",", INIT=");
            if (arr) {
                if (typ == "char") {
                    QString s = "";
                    for (int i =0;i<inits.size();i++) s.append(inits[i]);
                    writeln("output",'"'+s+'"');
                }
                else if (typ == "byte") {
                    for (int c = 0; c<size;c++) {
                        write("output",QString("%1").arg(inits[c]));
                        if (c < (size-1))
                            write("output",", ");
                        else
                            writeln("output","");
                    }
                }
                else if (typ == "word")
                    for (int c = 0 ;c < size;c++) {
                    write("output",QString("%1").arg(256*inits[2*c]+inits[2*c+1]));

                    //write(QString("%1").arg(256*inits[c*2-1].toAscii()+inits[c*2].toAscii()));
                    if (c < (size-1))
                        write("output",", ");
                    else
                        writeln("output","");
                }
            }
            else
                if (typ == "char")
                    writeln("output",QString((char)initn));
            else
                writeln("output",QString("%1").arg(initn));
        }
        else
            writeln("output","");
    }
}
/*{--------------------------------------------------------------}
{ Print Proc Table }
*/
void Clcc::printproclist(void) {

    writeln("output","");
    writeln("output","PROCEDURES DECLARED:");
    writeln("output","");
    //QMapIterator<QString, Cproc> i(proclist);
    for (int i=0; i<proclist.size();i++) {
        write("output","PROC "+proclist.at(i).ProcName+": ");
        write("output",proclist.at(i).ProcName);//,', CODE: ',proclist[i].proccode);
        if (proclist.at(i).hasreturn) {
            write("output",", RETURNS: ");
            if (proclist.at(i).ReturnIsWord) write("output","word");
            else write("output","byte");
        }
        if (proclist.at(i).ParCnt > 0) writeln("output",", PARAMS: "+proclist.at(i).Params);
        else writeln("output","");
    }
    writeln("output","");
}

QByteArray Clcc::vardecl(void) {

//var Name, Typ, t: string;
//    xr, p, l: boolean;
    QByteArray t;
    QByteArray Typ;
    QByteArray result="";
    bool p,xr,l;
    QByteArray name;


    name = ExtrWord(&Tok);
    Typ = name;
    Tok.append(',');
    xr = false;
    do {
        t = ExtrList(&Tok);
        name=ExtrWord(&t).trimmed();
        p = false;
        if (name.startsWith('*')) {
            p = true;
            name.remove(0,1);//delete(name, 1, 1);
            name=name.trimmed();
        }
        if (name.startsWith("xram")) {
            xr = true;
            name=ExtrWord(&t);
        }
        if (t.size()>0) name.append(" " + t);
        l = (level==0 ? false : true);
        //result.append(name);
        AddVar(name, Typ, xr, p, l);  // Global var definition
    }
    while (Tok.size() >0);

    return name;
}

void Clcc::AddProc(QByteArray t, QByteArray c, QByteArray par, int pc, bool hr, bool wd) {

    QByteArray s = ExtrWord(&t);
    if (! FindProc(s)) {
        Cproc p;
        p.ProcName = s;
        p.ProcCode = c;
        p.Params = par;
        p.ParCnt = pc;
        p.hasreturn = hr;
        p.ReturnIsWord = wd;
        proclist.append(p);
        proccount++;
        //                writeln('Proc add: NAME: ' + s);
    }
    else
        QMessageBox::about(mainwindow,"ERROR","Procedure already declared: " + s);
}

//{ Add Variable Declaration }

void Clcc::AddVar(QByteArray t,QByteArray typ, bool xr, bool pnt, bool loc) {

    QByteArray litem;
    QByteArray s = ExtrWord(&t);
    // Test if var still exist
    if (! FindVar(s)) {
        Cvar v;
        v.varname = s;
        v.pointer = pnt;
        v.xram = xr;
        v.local = loc;
        v.typ = typ;
        if (pnt) {
            v.pnttyp = typ;
            v.typ = (xr ? "word" : "byte");
        }

        if ((typ == "byte") || (typ == "char")) v.size = 1;
        else if (typ == "word") v.size = 2;

        v.arr = false;
        if (t.startsWith('[')) {

            QByteArray s = ExtrCust(&t, ']');
            //if s[1] <> '[' then Expected('[size]");
            s.remove(0,1);
            int arsize = mathparse(s, 16);
            //val(s, arsize, temp);
            //                                if arsize >= 256 then Error('Array too big!");
            v.size = arsize;
            v.arr = true;
        }
        v.at = false;

        //s := ExtrWord(t);
        if (!loc) {
            if (t.startsWith("at")) {
                if (loc) {
                    QMessageBox::about(mainwindow,"ERROR","Local vars can't have 'at' assignments!");
                }
                s = ExtrWord(&t);
                if (t.indexOf('=') > 0) {
                    s = ExtrCust(&t, '=');
                    if (t.startsWith('(')) t = " " + t;
                    t = "=" + t;
                }
                else s = t;
                //val(s, temp, c);
                int temp = mathparse(s, 16);
                if (temp > 95) xr = true;
                v.xram = xr;
                v.at = true;
                v.address = temp;
                if (pnt) {
                    if (xr) {
                        v.typ = "word";
                        v.size = 2;
                    }
                    else {
                        v.typ = "byte";
                        v.size = 1;
                    }
                }
                AllocVar(xr, true, v.size, temp);
                    //s := ExtrWord(t);
            }
            else {
                if (typ == "word")
                    v.address = AllocVar(xr, v.at, v.size * 2, -1);
                else
                    v.address = AllocVar(xr, v.at, v.size, -1);
            }
            if (t.startsWith('=')) {
                if (loc) QMessageBox::about(mainwindow,"ERROR","Local vars cant have init values!");
                if (pnt) QMessageBox::about(mainwindow,"ERROR","pointers cant have init values!");
                t.remove(0,1);
                if (v.arr && (typ == "char") ) {
                    t.remove(0,1);
                    t.remove(t.length()-1,1);//delete(t, length(t), 1);
                    //VarList[VarCount].inits := stringparse(t, size);
                    for (int i=0;i<t.length();i++) v.inits.append(t[i]);//+chr(0);
                    v.initn = 0;
                }
                else if (v.arr && (typ == "byte") ) {
                    t.remove(0,2);
                    t.remove(t.length()-1,1);//delete(t, length(t), 1);
                    t = t + ',';
                    while ((litem = ExtrList(&t)) != "") {
                        v.inits.append((char)mathparse(litem, 8));
                        //val(litem, temp, c);
                        //VarList[VarCount].inits := VarList[VarCount].inits + chr(temp);
                    }
                    v.initn = 0;
                }
                else if (v.arr && (typ == "word")) {
                    t.remove(0,2);
                    t.remove(t.length()-1,1);//delete(t, length(t), 1);
                    t = t + ',';
                    while ((litem = ExtrList(&t)) != "") {
                        v.inits.append((char)mathparse(litem+"/256", 8));
                        v.inits.append((char)mathparse(litem+"%256", 8));
                    //val(litem, temp, c);
                    //VarList[VarCount].inits := VarList[VarCount].inits + chr(temp div 256) + chr(temp mod 256);
                    }
                    v.initn = 0;
                }
                else
                                //val(t, VarList[VarCount].initn, temp);
                    v.initn = mathparse(t, 16);
            }
            else
                v.initn = -1;

        }
        else {
            v.initn = -1;
            if (! procd) {
                proclist[currproc].LocName.append(v.varname);
                proclist[currproc].LocTyp.append(v.typ);
                proclist[currproc].LocCnt++;

            }
        }

//            {                        write('Var add: NAME: ' + VarList[VarCount].VarName + ', XRAM: ");
//                if (v.xram) write("yes, ADR: ");
//                else write("no, ADR: ");
//                writeln(v.address);
//            }
        VarCount++;

        varlist.append(v);
    }
    else {
        QMessageBox::about(mainwindow,"ERROR","Variable already declared: " + s);
    }
}


//{ Split String in Words }

QByteArray Clcc::ExtrWord(QByteArray *word) {

    char c;

    QByteArray result = "";
    if (word->isEmpty()) return "";

    c = ' ';
    while ( (!word->isEmpty() && ! SpacesList.contains(word->at(0))) || (c != ' ')) {
        if (word->startsWith('"') || word->startsWith("'")) {
            if (word->startsWith(c)) c = ' ';
            else c = word->at(0);
        }
        result = result + word->at(0);
        word->remove(0,1);//delete(word, 1, 1);
    }
    if (!word->isEmpty())
        if (SpacesList.contains(word->at(0))) word->remove(0,1);// delete(word, 1, 1);
    result = result.trimmed();
    //        if word[1] in ['[', '('] then
    //                word := word + ExtrWord(word);

    return result;
}

//{ Split String in Words }

QByteArray Clcc::ExtrCust(QByteArray *word,char c) {
    char sc;
    QByteArray result = "";
    if (word->isEmpty()) return "";

    sc = ' ';
    while ( (!word->isEmpty() && !word->startsWith(c)) || (sc != ' ') ) { //and not (word[1] in ['[', '(']) do
        if (word->startsWith('"') || word->startsWith("'")) {
            if (word->startsWith(sc)) sc = ' ';
            else sc = word->at(0);
        }
        result = result + word->at(0);
        word->remove(0,1);//delete(word, 1, 1);
    }
    if (!word->isEmpty()) {
        if (word->startsWith(c)) word->remove(0,1);//delete(word, 1, 1);
        if (!word->isEmpty())
            if (SpacesList.contains(word->at(0))) word->remove(0,1);// delete(word, 1, 1);
        result = result.trimmed();
    }
    result = result.trimmed();
    //        if word[1] in ['[', '('] then
    //                word := word + ExtrWord(word);
    return result;
}

//{ Split List }

QByteArray Clcc::ExtrList(QByteArray *list) {

    QByteArray result = "";
    int l = 0;

    if (list->isEmpty()) return "";
    char c = ' ';
    while ( ((list->size()>0) && !(list->at(0) == ',')) || (l > 0) || (c != ' ') ) {
        if (list->startsWith('(')) l++;
        if (list->startsWith(')')) l--;
        if (list->startsWith('"') || list->startsWith("'")) {
            if (list->startsWith(c)) c = ' ';
            else c = list->at(0);
        }
        result.append(list->at(0));
        list->remove(0,1);//delete(list, 1, 1);
    }
    list->remove(0,1);//delete(list, 1, 1);
    result = result.trimmed();

    return result;
}

//{ Allocate Variable Declaration }

int Clcc::AllocVar(bool xr,bool at,int  size, int adr) {
    QByteArray s;
    int result;
    if (!xr) {
        if (at) {
            result = adr;
            if (IsVarAtAdr(result, size)) {
                if (varlist[VarFound].at) {
                    s= QByteArray::number(varlist[VarFound].address);
                    QMessageBox::about(mainwindow,"ERROR","Previous var "+varlist[VarFound].varname+" at "+s+" already declared!");
                }
                varlist[VarFound].address = VarPos;
                VarPos += varlist[VarFound].size;
                if (size == 2) {
                    if (IsVarAtAdr(result+1, 1)) {
                        if (varlist[VarFound].at) {
                            s= QByteArray::number(varlist[VarFound].address);
                            QMessageBox::about(mainwindow,"ERROR","Overlap with "+varlist[VarFound].varname+" at "+s+"!");
                        }
                        varlist[VarFound].address = VarPos;
                        VarPos += varlist[VarFound].size;
                    }
                }
                result = adr;
            }
        }
        else {
            while (IsVarAtAdr(VarPos, size)) {
                VarPos += varlist[VarFound].size;
            }
            result = VarPos;
            VarPos += size;
        }
        //for i:=AllocVar to AllocVar+size-1 do memimg[i] := VarCount;
    }
    else result = -1;

    return result;
}

/*{--------------------------------------------------------------}
{ Do First Scan }
*/
void Clcc::FirstScan(QByteArray filen) {
    QByteArray name, name2, t, temp, s;
    int i;
    bool hasret;
    QByteArray src= filen;

    GetToken(MODESTR, &filen);
    int z= Tok.length();
    src = filen.mid(z);
    while (Tok != "")
    {
        name = Tok.mid(0,4);//copy(Tok,1,4); //ExtrWord(Tok);
        if ((level == 0) && ((name == "byte") || (name == "char") || (name == "word")))
        {
            Tok.remove(0,4); Tok = Tok.trimmed();
            Tok = name + " " + Tok;
            vardecl();
        }
        else if (Tok.startsWith("#org ")) {
            ExtrWord(&Tok);
            org = QByteArray::number(mathparse(Tok.trimmed(),16));
        }
        else if (Tok.startsWith("#pc ")) { ExtrWord(&Tok); pc = Tok.trimmed(); }
        else if (Tok.startsWith("#nosave")) nosave = true;
        else if (Tok.contains('('))
        {
            name2 = ExtrWord(&Tok);
            isword = false;
            hasret = false;
            if (name == "word")
            {
                isword = true;
                name = ExtrWord(&Tok);
                hasret = true;
            } else
                if ((name == "char") || (name == "byte"))
                {
                name = ExtrWord(&Tok);
                hasret = true;
            }
            else {
                name = name2;
            }
            Tok = Tok.trimmed(); Tok.remove(0,1); Tok = Tok.trimmed();
            i = 0;
            //                        p := 0;
            temp = "";
            if (!Tok.startsWith(")"))
            {
                Tok.remove(Tok.size()-1,1);//delete(Tok, length(Tok), 1);
                temp = Tok;
                s = Tok + ',';
                currproc = proccount;
                while (!s.isEmpty()) {
                        Tok = ExtrList(&s);
                        proclist[proccount].partyp.append("");
                        proclist[proccount].partyp[i] = Tok.left(4);//copy(Tok, 1, 4);
                        procd = true;
                        proclist[proccount].parname.append("");
                        proclist[proccount].parname[i] = vardecl();
                        procd = false;
                        i++;
                    }
            }
            t = "";
            do {
                //QByteArray src = "dummy";
                rd(&Look, &filen);
                t = t + Look;
            } while ( level != 0);
            t.remove(t.size()-1,1);//delete(t, length(t), 1);
            AddProc(name, t.trimmed(), temp, i, hasret, isword);
        }
        GetToken(MODESTR, &filen);
        src = src.mid(src.indexOf(Tok)+Tok.size());
    }
    //closefile(f);

    if (!FindProc("main")) {
        QMessageBox::about(mainwindow,"ERROR","main not found!");
    }
    printvarlist();
    printproclist();
}

char Clcc::Read(void) {

}

void Clcc::rd(char *c, QByteArray *s) {
    if (md == 0) *c=Read();
    else
    if (md == 1) {
        *c = Read();
        //if (eof(f))  *c = chr(27);
    }
    else if (md == 2) {
        if (!s->isEmpty()) *c = s->at(0);
        else *c = 27;
        s->remove(0,1);
    }
    if (l == ' ')
    {
        if (*c == '}') level--;
        if (*c == '{') level++;
    }
    if (*c == '\'')
    {
        if (l == ' ') l = '\'';
        else if (l == '\'') l = ' ';
    }
    if (*c == '"')
    {
        if (l == ' ') l = '"';
        else if (l == '"') l = ' ';
    }

}

void Clcc::GetToken(int mode, QByteArray *s) {

    Tok="";
    l = ' ';
    md = mode;

    do {
        rd(&Look, s);
        if (Look == 27) return;
    }
    while (SpacesList.contains(Look) || QByteArray("{}").contains(Look));

    //for  ( ; (l != ' ') || !QString(";{}").contains(Look) ; )
    while((l != ' ') || !QByteArray(";{}").contains(Look))
    {
        if (l == ' ')
        {
            if ( !SpacesList.contains(Look) && !QByteArray("{}").contains(Look))
            {
                Tok = Tok + Look;
                rd(&Look,s);
            } else
            {
                Tok = Tok + ' ';
                while (SpacesList.contains(Look)) {
                    rd(&Look, s);
                }
            }
        } else
        {
            Tok = Tok + Look;
            rd(&Look, s);
        }
    }

    //        if (Tok != "")  if (Tok[0] == ' ') Tok.remove(0,1);
    //        if (Tok.right(1) == ' ' ) delete(Tok, length(Tok), 1);
    Tok = Tok.trimmed();

    //i = 2;
    l = ' ';
    for (int i = 1 ; i< Tok.size(); )
    {
        if (Tok[i] == '\'') {
            if (l == ' ')  l = '\'';
            else if (l == '\'') l = ' ';
        }
        if (Tok[i] == '"') {
            if (l == ' ') l = '"';
            else if (l == '"') l = ' ';

        }
        if (l == ' ')
        {
            if ((Tok[i-1] != ' ') && (QByteArray("[(=").contains(Tok[i])))
            {
                Tok.insert(i,' ');
                i++;
            }
            if (QByteArray("])").contains(Tok[i]) && (Tok[i+1] != ' '))
            {
                Tok.insert(i+1,' ');
                i++;
            }
            if (Ops2List.contains(Tok[i]) && (Tok[i+1] == ' '))
            {
                Tok.remove(i+1, 1);
                i--;
            }
            if ((Tok[i-1] == ' ') && OpsList.contains(Tok[i]))
            {
                Tok.remove(i-1, 1);
                i--;
            }
        }
        i++;
    }

}


//{--------------------------------------------------------------}
//{ Get an Identifier }

QByteArray Clcc::GetName(void) {
    QByteArray n;

    n = "";
    if (!Alpha.contains(toupper(Look))) Expected("Name");
    while (Alpha.contains(toupper(Look)) || NumList.contains(toupper(Look))) {
        n.append(Look);
        rd(&Look, &Tok);
        Tok = Tok.trimmed();
    }
    return (n);
}

int Clcc::skiphex(QByteArray hs,int hi) {
    while ( (hi < hs.length()) && QByteArray("0123456789ABCDEFabcdef").contains(hs[hi])) {
        hi++;
    }
    return hi;
}
int Clcc::skipbin(QByteArray hs,int hi) {
    while ( (hi < hs.length()) && QByteArray("0123456789").contains(hs[hi])) {
        hi++;
    }
    return hi;
}

//{--------------------------------------------------------------}

int Clcc::find_text(QByteArray such, QByteArray text) {
    int i;
    char c;
    int result = 0;
    if (such.isEmpty() || text.isEmpty()) return 0;
    text = " "  + text + " ";
    i = 0;
    c = ' ';
    while (i < (text.length()-1)) {
        if (QByteArray("'\"").contains(text[i])) {
            if (c == ' ') c = text[i];
            else c = ' ';
        }
        if ((c == ' ') &&
            text.mid(i).startsWith(such) &&
            !  ( QByteArray("_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ").contains(toupper(text[i-1])) ||
                 QByteArray("_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ").contains(text[i+such.length()]) ) ) {
            result = i - 1;
            break;
        }
        i++;
    }
}

//{--------------------------------------------------------------}
//{ Recognize an Addition Operator }
bool Clcc::IsAddop(char c) {
    return (QByteArray("+-|~$§").contains(c));
}

//{--------------------------------------------------------------}
void Clcc::Expression(void) {
    char c;
    int i;
    Tok = Look + Tok;
    for (int i= 0 ; i<varlist.size(); i++) {
        if (find_text(varlist[i].varname, Tok) > 0) {
            if (varlist[i].typ == "word") isword = true;
            else if (varlist[i].pointer && (varlist[i].pnttyp == "word")) isword = true;
        }
    }
    i = 1;

    // replace << by $
    // replace >> by §
    // replace ++ by PP
    // replace -- by MM
    Tok.replace("<<","$").replace(">>","§");
    Tok.replace("++",QByteArray(1,PP));
    Tok.replace("--",QByteArray(1,MM));
// A REVOIR !!!!!!!!!!!!!!!!!!
//    while (i < (Tok.length()-1)) {
//        if (Tok.at(i) == '\'') { c = copy(Tok, i+1,1)[1]; delete(Tok,i,3); insert(inttostr(ord(c)),Tok,i); i--; }
//        else i++;
//    }
    rd(&Look, &Tok);

    SignedTerm();
    while (IsAddop(Look)) {
        switch (Look) {
        case '+': Add(); break;
        case '-': Subtract(); break;
        case '|': _Or(); break;
        case '~': _Xor(); break;
        case '§': ShiftR(); break;
        case '$': ShiftL(); break;
        }
    }
}

//{--------------------------------------------------------------}
//{ Recognize a Numeric Character }

bool Clcc::IsDigit(char c) {
    return ( (c >='0') && (c<='9') );
}

//{--------------------------------------------------------------}
//{ Get a Number }

QByteArray Clcc::GetNumber(void) {
QByteArray n;
int p;
bool isbin, ishex, ischr;

    n = "";
        p = 0;
        ishex = false;
        isbin = false;
        ischr = false;

    if (!IsDigit(Look) && (Look != '\'')) Expected("Integer");

    while (IsDigit(Look) ||
                ( (p=0) && (Look=='\'') ) ||
                ( (p=1) && ((toupper(Look)=='B') || (toupper(Look)=='X')) ) ||
                ( ishex && HexList.contains(toupper(Look)) ) ||
                ( isbin && NumList.contains(toupper(Look)) ) ||
                ischr) {
        if (p == 1) {
            if (toupper(Look)=='X') ishex = true;
            else if (toupper(Look)=='B') isbin = true;
        }
        if ((p == 0) && (Look == '\'')) ischr = true;
        if ((p == 2) && ischr) ischr = false;
        n.append(Look);
        p++;
        rd(&Look, &Tok);
        if (! ischr) Tok = Tok.trimmed();
    }
//       if ishex then GetNumber := inttostr(converthex(n))
//        else if isbin then GetNumber := inttostr(convertbin(n))
//        else if n[1]='''' then GetNumber := inttostr(ord(n[2]))
//        else
    return (n);
}


//{--------------------------------------------------------------}
//{ Load a Variable to the Primary Register }

void Clcc::LoadVariable(QByteArray name) {
    QByteArray typ;
    bool xr,arr,loc;
    int adr;

    if (!FindVar(name)) Error("Variable not defined: "+name);
    typ = varlist[VarFound].typ;
    adr = varlist[VarFound].address;
    loc = varlist[VarFound].local;
    arr = varlist[VarFound].arr;
    xr = varlist[VarFound].xram;

    if (! arr) {
        if ((typ=="char") || (typ=="byte")) {
            if (!xr) {
                if (!loc) {
                    if (adr < 64) writln("asm",tr("\tLP\t%1").arg(adr)+"\t; Load variable "+name);
                    else writln("asm",tr("\tLIP\t%1").arg(adr)+"\t; Load variable "+name);
                    writln("asm","\tLDM");
                }
                else {// Local char
                    writln("asm","LDR");
                    writln("asm",tr("ADIA\t%1").arg(adr+2+pushcnt));
                    writln("asm","STP");
                    writln("asm","LDM\t\t; Load variable "+name);
                }
            }
            else {
                if (adr !=-1) writln("asm",tr("\tLIDP\t%1").arg(adr)+"\t; Load variable "+name);
                else writln("asm","\tLIDP\t"+name+"\t; Load variable "+name);
                writln("asm","\tLDD");
            }
            if (isword) writln("asm","\tLIB\t0");
        }
        else {
            if (!xr) {
                if (!loc) {
                    if (adr < 64) writln("asm",tr("\tLP\t%1").arg(adr+1)+"\t; Load 16bit variable "+name);
                    else writln("asm",tr("\tLIP\t%1").arg(adr+1)+"\t; Load 16bit variable "+name);
                    writln("asm","\tLDM\t\t; HB");
                    writln("asm","\tEXAB");
                    writln("asm","\tDECP\t\t; LB");
                    writln("asm","\tLDM");
                }
                else {// Local word
                    writln("asm","\tLDR");
                    writln("asm",tr("\tADIA\t%1").arg(adr+1+pushcnt));
                    writln("asm","\tSTP");
                    writln("asm","\tLDM\t; HB - Load variable "+name);
                    writln("asm","\tEXAB");
                    writln("asm","\tINCP");
                    writln("asm","\tLDM\t; LB");
                }
            }
            else {
                if (adr != -1) writln("asm",tr("\tLIDP\t%1").arg(adr+1)+"\t; Load 16bit variable "+name);
                else writln("asm","\tLIDP\t"+name+"+1\t; Load 16bit variable "+name);
                writln("asm","\tLDD\t\t; HB");
                writln("asm","\tEXAB");
                if ((adr != -1) && ((adr + 1) / 256 == (adr / 256)))
                    writln("asm",tr("\tLIDL\tLB(%1)").arg(adr));
                else if (adr !=-1) writln("asm",tr("\tLIDP\t%1").arg(adr));
                else writln("asm","\tLIDP\t"+name);
                writln("asm","\tLDD\t\t; LB");
            }
        }
    }
    else {
        if ((typ=="char") || (typ=="byte")) {
            if (!xr) {
                writln("asm",tr("\tLIB\t%1").arg(adr)+"\t; Load array element from "+name);
                writln("asm","\tLP\t3");
                writln("asm","\tADM");
                writln("asm","\tEXAB");
                writln("asm","\tSTP");
                writln("asm","\tLDM");
            }
            else {
                writln("asm","\tPUSH\t\t; Load array element from "+name); pushcnt++;
                writln("asm","\tLP\t5\t; HB of address");
                if (adr !=-1) {
                    writln("asm",tr("\tLIA\tHB(%1-1)").arg(adr));
                    writln("asm","\tEXAM");
                    writln("asm","\tLP4\t; LB");
                    writln("asm",tr("\tLIA\tLB(%1-1)").arg(adr));
                }
                else {
                    writln("asm","\tLIA\tHB("+name+"-1)");
                    writln("asm","\tEXAM");
                    writln("asm","\tLP\t4\t; LB");
                    writln("asm","\tLIA\tLB("+name+"-1)");
                }
                writln("asm","\tEXAM");
                writln("asm","\tPOP"); pushcnt--;
                writln("asm","\tLIB\t0");
                writln("asm","\tADB");
                writln("asm","\tIXL");
            }
        }
        else {
            if (!xr) {
                writln("asm","\tRC");
                writln("asm","\tSL");
                writln("asm",tr("\tLII\t%1").arg(adr)+"\t; Store array element from "+name);
                writln("asm","\tLP\t0");
                writln("asm","\tADM");
                writln("asm","\tEXAM");
                writln("asm","\tSTP");
                writln("asm","\tLDM");
                writln("asm","\tEXAB");
                writln("asm","\tINCP");
                writln("asm","\tLDM");
                writln("asm","\tEXAB");
            }
            else {
                writln("asm","\tRC");
                writln("asm","\tSL");
                writln("asm","\tPUSH\t\t; Load array element from "+name); pushcnt++;
                writln("asm","\tLP\t5\t; HB of address");
                if (adr !=-1) {
                    writln("asm",tr("\tLIA\tHB(%1-1)").arg(adr));
                    writln("asm","\tEXAM");
                    writln("asm","\tLP\t4\t; LB");
                    writln("asm",tr("\tLIA\tLB(%1-1)").arg(adr));
                }
                else {
                    writln("asm","\tLIA\tHB("+name+"-1)");
                    writln("asm","\tEXAM");
                    writln("asm","\tLP\t4\t; LB");
                    writln("asm","\tLIA\tLB("+name+"-1)");
                }
                writln("asm","\tEXAM");
                writln("asm","\tPOP"); pushcnt--;
                writln("asm","\tLIB\t0");
                writln("asm","\tADB");
                writln("asm","\tIXL");
                writln("asm","\tEXAB");
                writln("asm","\tIXL");
                writln("asm","\tEXAB");
            }
        }
    }
}


/*
{--------------------------------------------------------------}
{ Store the Primary Register to a Variable }
*/
void Clcc::StoreVariable(QByteArray name) {

    Cvar var;

    if (! FindVar(name)) { QMessageBox::about(mainwindow,"ERROR","Variable not defined: "+name); }
    var = varlist.at(VarFound);

    if (!var.arr) {
        if ( (var.typ=="char") || (var.typ=="byte") ) {
//                if isword then
//                        writln(#9'EXAB'#9#9'; Store only HB in byte var!');
            if (! var.xram) {
                if (!var.local) {
                    if (var.address <= 63) {
                        writln("asm","\tLP\t"+QByteArray::number(var.address)+"\t; Store result in "+var.varname);
                    }
                    else {
                        writln("asm","\tLIP\t"+QByteArray::number(var.address)+"\t; Store result in "+var.varname);
                    }
                    writln("asm","\tEXAM");
                }
                else {
                    // Local char
                    writln("asm","\tEXAB");
                    writln("asm","\tLDR");
                    writln("asm","\tADIA\t"+QByteArray::number(var.address+2+pushcnt));
                    writln("asm","\tSTP");
                    writln("asm","\tEXAB");
                    writln("asm","\tEXAM\t\t; Store result in "+var.varname);
                }
            }
            else {
                writln("asm","\tLIDP\t"+var.getLabel()+"\t; Store result in "+var.varname);
                writln("asm","\tSTD");
            }
        }
        else {
            if (!var.xram) {
                if (!var.local) {
                    if (var.address < 64) {
                        writln("asm","\tLP\t"+QByteArray::number(var.address)+"\t; Store 16bit variable "+var.varname);
                    }
                    else {
                        writln("asm","\tLIP\t"+QByteArray::number(var.address)+"\t; Store 16bit variable "+var.varname);
                    }
                    writln("asm","\tEXAM\t\t; LB");
                    writln("asm","\tEXAB");
                    writln("asm","\tINCP\t\t; HB");
                    writln("asm","\tEXAM");
                }
                else {
                // Local word
                    writln("asm","\tPUSH"); pushcnt++;
                    writln("asm","\tLDR");
                    writln("asm","\tADIA\t"+QByteArray::number(var.address+1+pushcnt));
                    writln("asm","\tSTP");
                    writln("asm","\tPOP"); pushcnt--;
                    writln("asm","\tEXAM\t; LB - Store result in "+var.varname);
                    writln("asm","\tEXAB");
                    writln("asm","\tDECP");
                    writln("asm","\tEXAM\t; HB");
                }
            }
            else {
                writln("asm","\tLIDP\t"+var.getLabel()+"\t; Store 16bit variable "+var.varname);
                writln("asm","\tSTD\t\t; LB");
                writln("asm","\tEXAB");
                if ( var.address &&
                     ( ((var.address + 1) / 256) == (var.address / 256) ) ) {
                     writln("asm","\tLIDL\tLB("+QByteArray::number(var.address)+"+1)");
                }
                else  {
                    writln("asm","\tLIDP\t"+ var.getLabel()+"+1");
                    writln("asm","\tSTD\t\t; HB");
                }
            }
        }
    }
    else {
        if ((var.typ=="char") || (var.typ=="byte"))
        {
            if (!var.xram)
            {
                writln("asm","\tLIB\t"+QByteArray::number(var.address)+"\t; Store array element from "+var.varname);
                writln("asm","\tLP\t3");
                writln("asm","\tADM");
                writln("asm","\tEXAB");
                writln("asm","\tSTP");
                writln("asm","\tPOP"); pushcnt--;
                writln("asm","\tEXAM");
            }
            else
            {
                writln("asm","\tPUSH\t\t; Store array element from "+var.varname); pushcnt++;
                writln("asm","\tLP\t7\t; HB of address");

                writln("asm","\tLIA\tHB("+var.getLabel()+"-1)");
                writln("asm","\tEXAM");
                writln("asm","\tLP\t6\t; LB");
                writln("asm","\tLIA\tLB("+var.getLabel()+"-1)");

                writln("asm","\tEXAM");
                writln("asm","\tPOP"); pushcnt--;
                writln("asm","\tLIB\t0");
                writln("asm","\tADB");
                writln("asm","\tPOP"); pushcnt;
                writln("asm","\tIYS");
            }
        }
        else
        {
            if (!var.xram)
            {
                writln("asm","\tRC");
                writln("asm","\tSL");
                writln("asm","\tLII\t"+QByteArray::number(var.address)+"\t; Store array element from "+var.varname);
                writln("asm","\tLP\t0");
                writln("asm","\tADM");
                writln("asm","\tEXAM");
                writln("asm","\tSTP");
                writln("asm","\tINCP");
                writln("asm","\tPOP"); pushcnt--;
                writln("asm","\tEXAM");
                writln("asm","\tDECP");
                writln("asm","\tPOP"); pushcnt--;
                writln("asm","\tEXAM");
            }
            else
            {
                writln("asm","\tRC");
                writln("asm","\tSL");
                writln("asm","\tPUSH\t _t; Store array element from "+var.varname); pushcnt++;
                writln("asm","\tLP\t7\t; HB of address");

                writln("asm","\tLIA\tHB("+var.getLabel()+"-1)");
                writln("asm","\tEXAM");
                writln("asm","\tLP\t6\t; LB");
                writln("asm","\tLIA\tLB("+var.getLabel()+"-1)");

                writln("asm","\tEXAM");
                writln("asm","\tPOP"); pushcnt--;
                writln("asm","\tLIB\t0");
                writln("asm","\tADB");
                writln("asm","\tPOP"); pushcnt--;
                writln("asm","\tEXAB");
                writln("asm","\tPOP"); pushcnt--;
                writln("asm","\tIYS");
                writln("asm","\tEXAB");
                writln("asm","\tIYS");
            }
        }
    }
}


//{--------------------------------------------------------------}
//{ Parse and Translate a Factor }

void Clcc::Factor(void) {
    //var s{, temp}: string;
    QByteArray s;

    if (Look =='(') {
        rd(&Look, &Tok);
        Tok = Tok.trimmed();
        Expression();
        rd(&Look, &Tok);
        Tok = Tok.trimmed();
    }
    else if (IsDigit(Look)) {
        LoadConstant(GetNumber());
    }
    else if (Alpha.contains(toupper(Look))) {
        s = GetName();
        if (Look == '[') {
            rd(&Look, &Tok);
           Tok = Tok.trimmed();
            Expression();
            //Push;
        }
        if (FindVar(s)) {
            if (pointer == ptrREF) {
                LoadVariable(s);
                if (! varlist[VarFound].pointer)
                    Error("This var ("+s+") is not a pointer!");
                if (varlist[VarFound].xram) {
                    writln("asm","\tLP\t4\t; XL");
                    writln("asm","\tEXAM");
                    writln("asm","\tLP\t5\t; XH");
                    writln("asm","\tEXAB");
                    writln("asm","\tEXAM");
                    writln("asm","\tDX");
                    if (varlist[VarFound].pnttyp != "word") {
                        writln("asm","\tIXL\t\t; Load content *"+s);
                    }
                    else {
                        writln("asm","\tIXL\t\t; Load content LB *"+s);
                        writln("asm","\tEXAB");
                        writln("asm","\tIXL\t\t; Load content HB *"+s);
                        writln("asm","\tEXAB");
                    }
                }
                else {
                    // LIP
                    writln("asm","\tSTP\t\t; Set P");
                    if (varlist[VarFound].pnttyp != "word") {
                        writln("asm","\tLDM\t\t; Load content *"+s);
                    }
                    else {
                        writln("asm","\tLDM\t\t; Load content LB *"+s);
                        writln("asm","\tEXAB");
                        writln("asm","\tINCP");
                        writln("asm","\tLDM\t\t; Load content HB *"+s);
                        writln("asm","\tEXAB");
                    }
                }
            }
            else
                if (pointer == ptrADR) {
                if (varlist[VarFound].xram) {
                    if (varlist[VarFound].address == -1) {
                        writln("asm","\tLIA\tLB('+s+')\t; &"+s);
                        writln("asm","\tLIB\tHB('+s+')\t; &"+s);
                    }
                    else {
                        writln("asm",tr("\tLIA\tLB(%1)\t; &").arg(varlist[VarFound].address)+s);
                        writln("asm",tr("\tLIB\tHB(%1)\t; &").arg(varlist[VarFound].address)+s);
                    }
                }
                else {
                    writln("asm",tr("\tLIA\t%1\t; &").arg(varlist[VarFound].address)+s);
                }
            }
            else LoadVariable(s);
        }
        else if (FindProc(s)) {
            Tok = s + " (" + Tok.trimmed();
            ProcCall();
            Tok = Tok.trimmed();
        }
    }
    else Error("Unrecognized character " + Look);
}
//{--------------------------------------------------------------}



//{--------------------------------------------------------------}
//{ Parse and Translate a Factor with Optional "Not" }
void Clcc::NotFactor(void) {
    if (Look == '!') {
        rd(&Look, &Tok);
        Tok = Tok.trimmed();
        Factor();
        NotIt();
    }
    else if (Look == '*') {
        rd(&Look, &Tok);
        Tok = Tok.trimmed();
        pointer = ptrREF;
        Factor();
    }
    else if (Look == '&') {
        rd(&Look, &Tok);
        Tok = Tok.trimmed();
        pointer = ptrADR;
        Factor();
    }
    else {
        pointer = 0;
        Factor();
    }
}
//{--------------------------------------------------------------}


//{--------------------------------------------------------------}
//{ Parse and Translate a Term }
void Clcc::Term(void) {
    NotFactor();
    while (QByteArray("*/&%").contains(Look)) {
        switch (Look) {
        case '*': Multiply();break;
        case '/': Divide();break;
        case '&': _And();break;
        case '%': _Mod();break;
        }
    }
}

//{--------------------------------------------------------------}
//{ Parse and Translate a Factor with Optional Sign }
void Clcc::SignedTerm(void) {
    char Sign;
    Sign = Look;
    if (IsAddop(Look)) {
        rd(&Look, &Tok);
        Tok = Tok.trimmed();
    }
    Term();
    if (Sign == '-') Negate();
}

//{--------------------------------------------------------------}
//{ Parse and Translate an Addition Operation }

void Clcc::Add() {
    rd(&Look, &Tok);
    Tok = Tok.trimmed();
    Push();
    Term();
    PopAdd();
}

//{--------------------------------------------------------------}
//{ Parse and Translate a Subtraction Operation }

void Clcc::Subtract(void) {
    rd(&Look, &Tok);
    Tok = Tok.trimmed();
    Push();
    Term();
    PopSub();
}


//{--------------------------------------------------------------}
//{ Parse and Translate an Multiply Operation }

void Clcc::Multiply(void) {
    rd(&Look, &Tok);
    Tok = Tok.trimmed();
    Push();
    Term();
    PopMul();
}


//{--------------------------------------------------------------}
//{ Parse and Translate a Divide Operation }

void Clcc::Divide(void) {
    rd(&Look, &Tok);
    Tok = Tok.trimmed();
    Push();
    Term();
    PopDiv();
}


//{ Parse and Translate a Subtraction Operation }

void Clcc::_Or(void) {
    rd(&Look, &Tok);
    Tok = Tok.trimmed();
    Push();
    Term();
    PopOr();
}

//{ Parse and Translate a Shift Operation }

void Clcc::ShiftR(void) {
    rd(&Look, &Tok);
    Tok = Tok.trimmed();
    Push();
    Term();
    PopSr();
}

//{ Parse and Translate a Shift Operation }

void Clcc::ShiftL(void) {
    rd(&Look, &Tok);
    Tok = Tok.trimmed();
    Push();
    Term();
    PopSl();
}

//{ Parse and Translate a Subtraction Operation }

void Clcc::_Xor(void) {
    rd(&Look, &Tok);
    Tok = Tok.trimmed();
    Push();
    Term();
    PopXor();
}


//{ Parse and Translate a Boolean And Operation }

void Clcc::_And(void) {
    rd(&Look, &Tok);
    Tok = Tok.trimmed();
    Push();
    NotFactor();
    PopAnd();
}


//{ Parse and Translate a Modulo Operation }

void Clcc::_Mod(void) {
    rd(&Look, &Tok);
    Tok = Tok.trimmed();
    Push();
    Term();
    PopMod();
}

//{--------------------------------------------------------------}

//{-------------------------------------------------------------}
//{ Procedure Call }

void Clcc::ProcCall() {
    int i, c, a;
    QByteArray temp;

    if (Tok.isEmpty()) return;

    temp = ExtrWord(&Tok);
    //        s := Tok;          // Nur den Parameterblock der ersten Funktion extrahieren!!!
    a = 0;
    rd(&Look, &Tok);
    Tok = Tok.trimmed();
    if (FindProc(temp)) {
        i = proclist[ProcFound].ParCnt;
        if (i > 0) {
            //delete(s, 1, 1); s := trim(s);
            c = 0;
            do {
                rd(&Look, &Tok);
                Tok = Tok.trimmed();
                if (proclist[ProcFound].partyp[c] != "word") {
                    isword = false;
                    a++;
                }
                else {
                    isword = true;
                    a+=2;
                }
                Expression();
                Push();
                c++;
                if (c > proclist[ProcFound].ParCnt) Error("Too many parameters for "+proclist[ProcFound].ProcName);
                //until (Look <> ',");
            }
            while (Look == ',');
            if (c != proclist[ProcFound].ParCnt) Error("Wrong number of parameters for "+proclist[ProcFound].ProcName);
        }
        i = proclist[ProcFound].LocCnt;
        if (i > 0) {
            for (c = 0; c < i;i++) {
                if (proclist[ProcFound].LocTyp[c] != "word") {
                    isword = false;
                    a++;
                }
                else {
                    isword = true;
                    a+=2;
                }
                Push();
            }
        }
        writln("asm","\tCALL\t"+temp+"\t; Call procedure "+temp);
        if (a > 0) {
            if (proclist[ProcFound].ReturnIsWord) {
                writln("asm","\tLP\t0");
                writln("asm","\tEXAM");
                writln("asm","\tLDR");
                writln("asm",tr("\tADIA\t%1").arg(a)); pushcnt -= a;
                writln("asm","\tSTR");
                writln("asm","\tEXAM");
            }
            else
                if (proclist[ProcFound].hasreturn) {
                writln("asm","\tEXAB");
                writln("asm","\tLDR");
                writln("asm",tr("\tADIA\t%1").arg(a)); pushcnt-=a;
                writln("asm","\tSTR");
                writln("asm","\tEXAB");
            }
            else {
                if (a < 4)
                    for (int i = 0 ; i<a; i++)
                    {
                    writln("asm","\tPOP"); pushcnt--;
                }
                else
                {
                    writln("asm","\tLDR");
                    writln("asm",tr("\tADIA\t%1").arg(a)); pushcnt-=a;
                    writln("asm","\tSTR");
                }
            }
        }
        //                Tok := s;
        rd(&Look, &Tok);
    }
    else
        Expected("procedure call");
}
//{-------------------------------------------------------------}



void Clcc::repadr(void) {
    int i, lc, pc, m, a;
    QByteArray name;

    lc = proclist[currproc].LocCnt;
    pc = proclist[currproc].ParCnt;
    if ((lc == 0) && (pc == 0)) return;
    if (lc > 0) name = proclist[currproc].LocName[lc - 1];
    else name = proclist[currproc].parname[pc - 1];
    if (! FindVar(name)) Error("Var "+name+" not declared!");
    if (! varlist[VarFound].local) Error("Var "+name+" not local!");
    m = 0;
    if (pc > 0)
        for (int i = 0; i < pc; i++)
            if (proclist[currproc].partyp[i] == "word") m+=2; else m++;
    if (lc > 0)
        for (int i=0; i< lc; i++)
            if (proclist[currproc].LocTyp[i] == "word") m+=2; else m++;
    a = 1;
    if (pc > 0)
        for (int i= 0; i< pc;i++) {
        name = proclist[currproc].parname[i];
        if (! FindVar(name)) Error("Var "+name+" not declared!");
        if (! varlist[VarFound].local) Error("Var "+name+" not local!");
        varlist[VarFound].address = m - a;
        a++;
        if (proclist[currproc].partyp[i] == "word") a++;
    }
    if (lc > 0)
        for (int i = 0; i< lc; i++) {
        name = proclist[currproc].LocName[i];
        if (! FindVar(name)) Error("Var "+name+" not declared!");
        if (! varlist[VarFound].local) Error("Var "+name+" not local!");
        varlist[VarFound].address = m - a;
        a++;
        if (proclist[currproc].LocTyp[i] == "word") a++;
    }
}



//{--------------------------------------------------------------}
//{ Parse and Translate an Assignment Statement }

void Clcc::Assignment(void) {
    QByteArray name,temp,s;
    bool fv;
    QByteArray forml;
    int i, p;

    isword = false;
    p = 0;
    if (Tok[0] == '*') {
        p = ptrREF;
        Tok.remove(0,1);
        Tok = Tok.trimmed();
    }
    rd(&Look, &Tok); Tok = Tok.trimmed();
    name = GetName();
    if (FindVar(name)) {
        if (p == ptrREF) {
            if (! varlist[VarFound].pointer && (varlist[VarFound].typ == "word")) isword = true;
            if (varlist[VarFound].pointer && (varlist[VarFound].pnttyp == "word")) isword = true;
        }
        else if (varlist[VarFound].typ == "word") isword = true;
    }
    else Error("Var "+name+" not declared!");
    s = "";
    if (Look == '[') {
        s = Tok;
        Tok.remove(0,Tok.indexOf("=")); Tok = Tok.trimmed();
    }
    if (QByteArray("+-*/%&|><").contains(Look)) {
        temp = name + Look;
        rd(&Look, &Tok); Tok = Tok.trimmed();
        if ((Look =='<') || (Look=='>')) {
            temp = temp + Look;
            //Rd(Look, Tok); Tok := trim(Tok);
        }
        else if ((Look =='+') || (Look='-')) {
            if (FindVar(name) && !(varlist[VarFound].typ == "word")) {
                if (Look == '+') writln("asm","\t; "+name+"++");
                else writln("asm","\t; "+name+"--");
                i = varlist[VarFound].address;
                if (i == 0) {
                    if (Look=='+') writln("asm","\tINCI");
                    else writln("asm","\tDECI");
                }
                else if (i == 1) {
                    if (Look=='+') writln("asm","\tINCJ");
                    else writln("asm","\tDECJ");
                }
                else if (i == 2) {
                    if (Look=='+') writln("asm","\tINCA");
                    else writln("asm","\tDECA");
                }
                else if (i == 3) {
                    if (Look=='+') writln("asm","\tINCB");
                    else writln("asm","\tDECB");
                }
                else if (i = 8) {
                    if (Look=='+') writln("asm","\tINCK");
                    else writln("asm","\tDECK");
                }
                else if (i == 9) {
                    if (Look=='+') writln("asm","\tINCL");
                    else writln("asm","\tDECL");
                }
                else if (i == 10) {
                    if (Look=='+') writln("asm","\tINCM");
                    else writln("asm","\tDECM");
                }
                else if (i == 11) {
                    if (Look=='+') writln("asm","\tINCN");
                    else writln("asm","\tDECN");
                }
                else {
                    LoadVariable(name);
                    if (Look == '+') writln("asm","\tINCA");
                    else writln("asm","\tDECA");
                    StoreVariable(name);
                }
                return;
            }
            else {
                temp = name + Look + '1';
                Tok = " ";
            }
        }
        else if (Look == '=') Tok = " "+ Tok;
        Tok.remove(0,1);
        Tok = temp + Tok;
    }
    Tok = Tok.trimmed();
    forml = Tok;
    if (forml[0] == '=') forml.remove(0,1);
    rd(&Look, &Tok); Tok = Tok.trimmed();

    fv = false;
    for (int i = 0; i< VarCount;i++) {
        if (find_text(varlist[i].varname, forml) > 0) {
            if (! varlist[i].pointer && (varlist[i].typ == "word")) isword = true;
            if (varlist[i].pointer && (varlist[i].pnttyp == "word")) isword = true;
            fv = true;
        }
    }
    if (!fv) for (int i=0;i< proccount;i++)
        if (find_text(proclist[i].ProcName, forml) > 0) fv = true;

    if (fv) Expression();
    else LoadConstant(forml);
    if (!s.isEmpty()) {
        Push();
        Tok = s;
        rd(&Look, &Tok); Tok = Tok.trimmed();
        Expression();
    }
    if (p == 0) StoreVariable(name);
    else if (p == ptrREF) {
        if (FindVar(name)) {
            if (varlist[VarFound].pnttyp != "word") {
                writln("asm","\tPUSH"); pushcnt++;
            }
            else {
                writln("asm","\tPUSH"); pushcnt++;
                writln("asm","\tEXAB");
                writln("asm","\tPUSH"); pushcnt++;
            }

            LoadVariable(name);
            if (! varlist[VarFound].pointer) Error("This var ("+name+") is not a pointer!");
            if (varlist[VarFound].xram) {
                writln("asm","\tLP\t6\t; YL");
                writln("asm","\tEXAM");
                writln("asm","\tLP\t7\t; YH");
                writln("asm","\tEXAB");
                writln("asm","\tEXAM");
                writln("asm","\tDY");
                if (varlist[VarFound].pnttyp != "word") {
                    writln("asm","\tPOP"); pushcnt--;
                    writln("asm","\tIYS\t\t; Store content *"+s);
                }
                else {
                    writln("asm","\tPOP"); pushcnt--;
                    writln("asm","\tEXAB");
                    writln("asm","\tPOP"); pushcnt--;
                    writln("asm","\tIYS\t\t; Store content LB *"+s);
                    writln("asm","\tEXAB");
                    writln("asm","\tIYS\t\t; Store content HB *"+s);
                }
            }
            else {
                // LIP
                writln("asm","\tSTP\t\t; Set P");
                if (varlist[VarFound].pnttyp != "word") {
                    writln("asm","\tPOP"); pushcnt--;
                    writln("asm","\tEXAM\t\t; Store content *"+s);
                }
                else {
                    writln("asm","\tPOP"); pushcnt--;
                    writln("asm","\tEXAB");
                    writln("asm","\tPOP"); pushcnt--;
                    writln("asm","\tEXAM\t\t; Store content LB *"+s);
                    writln("asm","\tEXAB");
                    writln("asm","\tEXAM\t\t; Store content HB *"+s);
                }
            }
        }
    }
}
//{--------------------------------------------------------------}


//{---------------------------------------------------------------}
//{ Parse and Translate a Boolean Expression }

void Clcc::BoolExpression(void) {
    int t,i;
    i = 0;
    while (i < Tok.length()) {
        if (Tok.mid(i,2) == "||") {
            Tok.remove(i,1); Tok[i]=BO;
        }
        else if (Tok.mid(i,2) == "&&") {
            Tok.remove(i,1); Tok[i]=BA;
        }
        else if (Tok.mid(i,3) == "= =") {
            Tok.remove(i,2); Tok[i]=EQ; i--;
        }
        else if (Tok.mid(i,3) == "> =" ) {
            Tok.remove(i,2); Tok[i]=GE; i--;
        }
        else if (Tok.mid(i,3) == "< =") {
            Tok.remove(i,2); Tok[i]=SE; i--;
        }
        else if (Tok.mid(i,3) == "! =" ) {
            Tok.remove(i,2); Tok[i]=NE; i--;
        }
        else
            i++;
    }
    rd(&Look, &Tok); Tok = Tok.trimmed();
    NotCompTerm();
    while ((Look==BO) || (Look==BA)) {
        Push();
        if (Look==BO) BoolOr();
        if (Look==BA) BoolAnd();
    }
}


//{--------------------------------------------------------------}
//{ Recognize and Translate a Boolean OR }

void Clcc::BoolOr(void) {
   rd(&Look, &Tok); Tok = Tok.trimmed();
   NotCompTerm();
   PopOr();
}

//{--------------------------------------------------------------}
//{ Recognize and Translate a Boolean OR }

void Clcc::BoolAnd(void) {
   rd(&Look, &Tok); Tok=Tok.trimmed();
   NotCompTerm();
   PopAnd();
}

void Clcc::NotCompTerm(void) {
char Sign;

    Sign = Look;
    if (Look == '!') {
                rd(&Look, &Tok); Tok = Tok.trimmed();
            }
    CompTerm();
    if (Sign == '!') NotIt();
}


void Clcc::CompTerm(void) {
    char compOp;
    if (Look =='(') {
        //Rd(Look, tok); tok := trim(tok);
        BoolExpression();
        rd(&Look, &Tok); Tok = Tok.trimmed();
    }
    else {
        Expression();
        Push();
        compOp = Look;
        rd(&Look, &Tok); Tok = Tok.trimmed();
        Expression();
        switch (compOp) {
        case '>': CompGreater();break;
        case '<': CompSmaller();break;
        case EQ: CompEqual();break;
        case GE: CompGrOrEq();break;
        case SE: CompSmOrEq();break;
        case NE: CompNotEqual();break;
        }
    }
}

//{--------------------------------------------------------------}
//{ Generate a Unique Label }

QByteArray Clcc::NewLabel(void) {
   LCount++;
   return "LB" + QByteArray::number(LCount-1);
}

//{--------------------------------------------------------------}
//{ Post a Label To Output }

void Clcc::PostLabel(QByteArray L) {
   writln("asm","  "+L+":");
}


//{-------------------------------------------------------------}
//{ Switch Statement }

void Clcc::DoSwitch(void) {
    QByteArray L1, temp;
    bool iselse;

    Tok.remove(0,7); Tok = Tok.trimmed();
    writln("asm","\t; Switch");
    rd(&Look, &Tok); Tok = Tok.trimmed();
    Expression();
    writln("asm","\tCASE");
    iselse = false;
    do {
        GetToken(MODESTR, &dummy);
        temp = ExtrCust(&Tok, ':'); Tok = Tok.trimmed();
        if (!Tok.isEmpty()) {
            dummy = Tok + ";}" + dummy;
            level++;
        }
        Tok = temp;

        if (Tok.contains("else")) iselse = true;
        L1 = NewLabel();
        writln("asm",'\t' + Tok + '\t' + L1);
        outfile = false;
        writln("asm","  " + L1 + ':');
        Block();
        writln("asm","\tRTN");
        writln("asm","");
        outfile = true;
    }
    while (dummy.trimmed().at(0) != '}');
    //until trim(dummy)[1] = '}';
    if (!iselse) writln("asm","\tELSE:\tEOP");
    writln("asm","\tENDCASE");
    writln("asm","\t; End switch");
}




//{-------------------------------------------------------------}
//{ If Statement }
void Clcc::DoIf(void) {
    QByteArray L1, L2;

    writln("asm","\t; If block: Boolean expression");
    writln("asm","");
    Tok.remove(0,4);//delete(Tok, 1, 4);
    BoolExpression();
    if (!Tok.isEmpty()) {
        dummy = Tok + ";}" + dummy;
        level++;
    }
    L1 = NewLabel(); L2 = L1;
    BranchFalse(L1);
    writln("asm","");
    writln("asm","\t; If expression = true");
    Block();

    if (dummy.startsWith("else")) {
        GetToken(MODESTR, &dummy);
        Tok.remove(0,4); Tok = Tok.trimmed(); //delete(Tok, 1, 4); Tok:=trim(Tok);
        if (!Tok.isEmpty()) {
            dummy = Tok + ";}" + dummy;
            level++;
        }
        L2 = NewLabel();
        Branch(L2);
        PostLabel(L1);
        writln("asm","\t; If expression = false");

        Block();
    }
    writln("asm","\t; End of if");
    PostLabel(L2);
}
//{-------------------------------------------------------------}



//{-------------------------------------------------------------}
//{ Goto Statement }

void Clcc::DoGoto(void) {
    Tok.remove(0,5); Tok = Tok.trimmed();
    writln("asm", "\tRJMP\t"+Tok+"\t; Goto");
    writln("asm","");
}


//{-------------------------------------------------------------}
//{ Label Statement }

void Clcc::DoLabel(void) {
    Tok.remove(0,6); Tok = Tok.trimmed();//delete(tok, 1, 6); tok := trim(tok);
    if (FindVar(Tok) || FindProc(Tok)) Error(Tok+": This label name is already used!");
    writln("asm","");
    writln("asm","  "+Tok+":\t; User label");
}


//{-------------------------------------------------------------}
//{ Break Statement }

void Clcc::DoBreak(void) {
    if (InnerLoop == "loop") writln("asm","\tLEAVE\t\t; Break");
    else writln("asm","\tRJMP\t"+ExitLabel+"\t; Break");
    writln("asm","");
}


//{-------------------------------------------------------------}
//{ Exit Statement }

void Clcc::DoReturn(void) {
    Tok.remove(0,6); Tok = Tok.trimmed();
    if (!Tok.isEmpty()) {
        rd(&Look, &Tok); Tok = Tok.trimmed();
        isword = proclist[currproc].ReturnIsWord;
        Expression();
    }
    writln("asm","\tRTN\t\t; Return");
    writln("asm","");
}


//{-------------------------------------------------------------}
//{ Loop Statement }

void Clcc::DoLoop(void) {
    QByteArray L1,L2;

    InnerLoop = "loop";
    L1 = NewLabel();
    L2 = NewLabel();
    ExitLabel = L2;
    writln("asm","\t; Loop");
    writln("asm","");
    Tok.remove(0,6); Tok = Tok.trimmed();
    rd(&Look, &Tok); Tok = Tok.trimmed();
    Expression();
    isword = false;
    Push();pushcnt--;
    PostLabel(L1);
    if (!Tok.isEmpty()) {
        dummy = Tok + ";}" + dummy;
        level++;
    }
    Block();
    writln("asm","\tLOOP\t"+L1);
    PostLabel(L2);
    writln("asm","\t; End of loop");
}


//{-------------------------------------------------------------}
//{ While Statement }

void Clcc::DoWhile(void) {
    QByteArray L1,L2;

    InnerLoop = "while";
    L1 = NewLabel();
    L2 = NewLabel();
    ExitLabel = L2;
    writln("asm","\t; While");
    writln("asm","");
    PostLabel(L1);
    Tok.remove(0,7);
    BoolExpression();
    BranchFalse(L2);
    writln("asm","\t; While expression = true");
    if (!Tok.isEmpty()) {
        dummy = Tok + ";}" + dummy;
        level++;
    }
    Block();
    writln("asm","\tRJMP\t"+L1);
    PostLabel(L2);
    writln("asm","\t; End of while");
}


//{-------------------------------------------------------------}
//{ For Statement }

void Clcc::DoFor(void) {
    QByteArray L1, L2, temp;

    InnerLoop = "for";
    Tok.remove(0,5);
    writln("asm","\t; For loop");
    Assignment();
    L1 = NewLabel();
    L2 = NewLabel();
    ExitLabel = L2;
    PostLabel(L1);
    GetToken(MODESTR, &dummy);
    BoolExpression();
    BranchFalse(L2);
    if (dummy.trimmed().at(0) != ')') {
        GetToken(MODESTR, &dummy);
        if (Tok.endsWith(')')) {
            Tok.remove(Tok.length()-1,1);//delete(tok, length(tok), 1);
            Tok = Tok.trimmed();
            if (!Tok.isEmpty()) dummy = Tok +';'+ dummy;
        }
        else {
            temp = ExtrCust(&Tok, ')');
            dummy = temp + ';' + Tok + ";}" + dummy;
            level++;
        }
    }
    else {
        GetToken(MODESTR, &dummy);
        if (Tok != ")") {
            Tok.remove(0,1);Tok = Tok.trimmed();
            dummy = Tok + ";}" + dummy;
            level++;
        }
    }

    Block();
    writln("asm","\tRJMP\t"+L1);
    PostLabel(L2);
    writln("asm","\t; End of for");
}


void Clcc::DoLoad(void) {
    Tok.remove(0,5);
    rd(&Look, &Tok);
    Expression();
}


void Clcc::DoSave(void) {
    QByteArray name;

    Tok.remove(0,5);
    name = GetName();
    StoreVariable(name);
}

//{-------------------------------------------------------------}
//{ Do..While Statement }

void Clcc::DoDoWhile(void) {
    QByteArray L1, L2;

    InnerLoop = "do";
    L1 = NewLabel();
    L2 = NewLabel();
    ExitLabel = L2;
    writln("asm","\t; Do..while");
    writln("asm","");
    PostLabel(L1);
    //        GetToken(MODESTR, dummy);
    ExtrWord(&Tok); Tok = Tok.trimmed();
    if (!Tok.isEmpty()) {
        dummy = Tok + ";}" + dummy;
        level++;
    }
    Block();
    GetToken(MODESTR, &dummy);
    Tok.remove(0,7);//delete(Tok, 1, 7);
    Tok.remove(Tok.length()-1,1);//delete(Tok, length(Tok), 1);
    BoolExpression();
    BranchFalse(L2);
    writln("asm","\t; While expression = true");
    writln("asm","\tRJMP\t"+L1);
    PostLabel(L2);
    writln("asm","\t; End of do..while");
}




//{--------------------------------------------------------------}
//{ Parse and Translate a Block }

void Clcc::Block(void) {
    QByteArray name;

    do {
        GetToken(MODESTR, &dummy);
        Tok = Tok.trimmed();
        name = Tok.mid(0,5);
        if ((name == "byte ") || (name == "char ") || (name == "word "))    // Local var definition
                vardecl();
        else if (firstp) {
            firstp = false;
            repadr();
        }
        if (!Tok.isEmpty()) {
          if (Tok.startsWith("if ")) DoIf();
          else if (Tok.startsWith("load "))     DoLoad();
          else if (Tok.startsWith("save "))     DoSave();
          else if (Tok.startsWith("switch "))   DoSwitch();
          else if (Tok.startsWith("loop "))     DoLoop();
          else if (Tok.startsWith("while "))    DoWhile();
          else if (Tok.startsWith("for "))      DoFor();
          else if (Tok.startsWith("goto "))     DoGoto();
          else if (Tok.startsWith("label "))    DoLabel();
          else if (Tok.contains(":"))           DoLabel();
          else if ((Tok.mid(0,2) == "do") &&
                  !Alpha.contains(toupper(Tok[3])))    DoDoWhile();
          else if (Tok == "break")              DoBreak();
          else if (Tok.startsWith("return"))    DoReturn();
          else if (Tok.startsWith("#asm")) {
                Tok = ExtrCust(&dummy, 0x0d);
                while (!Tok.startsWith("#endasm")) {
                        writln("asm","\t"+Tok);
                        Tok = ExtrCust(&dummy, 0x0D );
                    }
            }
          else if (Tok.contains("=")) Assignment();
          else if (Tok.contains("(")) ProcCall();
          else Assignment();
          writln("asm","");
      }
        dummy = dummy.trimmed();
    }
    //until (trim(dummy) = '') or (trim(dummy)[1] = '}');
    while (! dummy.trimmed().isEmpty() && !dummy.trimmed().startsWith("}"));
    if (!dummy.isEmpty()) rd(&Look, &dummy);
    dummy = dummy.trimmed();
}
//{--------------------------------------------------------------}



void Clcc::removelocvars(QByteArray pn) {
    int i, c;

    i = 0;
    if (! FindProc(pn)) Error("function "+pn+" unknown!");
    if (proclist[ProcFound].ParCnt == 0) return;
    while (i < VarCount) {
        if (varlist[i].local && (varlist[i].locproc == ProcFound)) {
            for (c = i ; c<VarCount - 1;c++)
                varlist[c] = varlist[c+1];
            VarCount--;
            i--;
        }
        i++;
    }
}




void Clcc::SecondScan(QByteArray filen) {
QByteArray name, typ, s, s2, s3;
bool    at, arr;
int    i, adr, size, value;
//    f2: textfile;
//begin
    //md = MODESTR;
    //        assign(f,'temp.asm");
    //        rewrite(f);
    QString f = "asm";

    //{ Write Intro }
    writln(f, "; pasm file - assemble with pasm!");
    writln(f, "; Compiled with lcc v1.0");
    writln(f,"");
    writln(f,".ORG\t"+org);
    writln(f,"");
    if (!nosave) {
        writln(f,"\tLP\t0");
        writln(f,"\tLIDP\tSREG");
        writln(f,"\tLII\t11");
        writln(f,"\tEXWD");
    }

//        { Write Header & Var inits }
//        writln( 'JMP\tMAIN");
//        writln('");

    for (int i=0; i<varlist.size();i++) {
        Cvar v = varlist[i];
        adr = v.address;
        size = v.size;
        name = v.varname;
        value = v.initn;
        typ = v.typ;
        at = v.at;
        arr = v.arr;

        if (v.xram) {
            if (at) {
                if (value !=-1) {
                    if (!arr) varxram(value, adr, size, name);
                    else varxarr(varlist[i].inits, adr, size, name, typ);
                }
            }
            else {
                if (!arr) varcode(value, adr, size, name);
                else varcarr(varlist[i].inits, adr, size, name, typ);
            }
        }
        else {
            if (value != -1) {
                if (!arr)  varreg(value, adr, size, name);
                else varrarr(v.inits, adr, size, name, typ);
            }
        }
    }

//        { Process procedures }
    for (int i = 0; i< proclist.size();i++) {
        if (proclist[i].ProcName == "main") {
            writln(f,"");
            //writln("asm","\tCALL\tMAIN");
            if (!nosave) {
                Tok = "main ()";
                ProcCall();
                writln(f,"\tLP\t0");
                writln(f,"\tLIDP\tSREG");
                writln(f,"\tLII\t11");
                writln(f,"\tMVWD");
                writln(f,"\tRTN");
                writln(f,"");
                writln(f,"SREG:\t.DW 0, 0, 0, 0, 0, 0");
            }
            writln(f,"");
            writln(f,"MAIN:");
            dummy = proclist[i].ProcCode;
            currproc = i;
            level = 1;
            pushcnt = 0; firstp = true;
            Block();
            if (pushcnt != 0) writeln(f,proclist[i].ProcName+": Possible Stack corruption!");
            removelocvars("main");
            writln(f, " EOP:\tRTN");
            writln(f,"");
            break;
        }
    }
    for (int i = 0; i<proclist.size();i++) {
        if (proclist[i].ProcName != "main") {
            writln(f,"");
            writln( f,proclist[i].ProcName+":\t; Procedure");
            dummy = proclist[i].ProcCode;
            level = 1;
            currproc = i;
            pushcnt = 0; firstp = true;
            Block();
            if (pushcnt != 0) writeln(f,proclist[i].ProcName+": Possible Stack corruption!");
            removelocvars(proclist[i].ProcName);
            writln(f,"\tRTN");
            writln(f,"");
        }
    }

        if (asmcnt > 0)
                for (int i = 0;i<asmcnt;i++) write(f, asmlist[i]);


        //{ Second pass: Optimize Code }

        s2 = "\tINCA";
        s3 = "\tDECA";
// A REVOIR !!!!!!!!!!!!
//        for (int i = 1 ;i <= 6; i++) {
//            s = "\tLIA\t"+QString("%1").arg(i)+"\t; Load constant "+QString("%1").arg(i)+"\r\n\tEXAB\r\n\tLP\t3\r\n\tSBM\t\t; Subtraction\r\n\tEXAB";
//            while (asmtext.contains(s)) {
//                asmtext.remove(asmtext.indexOf(s), s.length());
//                insert(s2, asmtext, pos(s, asmtext));
//                s2 = s2 + "\r\n\tDECA";
//            }
//         s := #9'LIA'#9+inttostr(i)+#9'; Load constant '+inttostr(i)+#13#10#9'LP\t3'#13#10#9'ADM\t\t; Addition'#13#10#9'EXAB';
//         while pos(s, asmtext) > 0 do {
//             delete(asmtext, pos(s, asmtext), length(s));
//             insert(s3, asmtext, pos(s, asmtext));
//             s3 := s3 + #13#10#9'INCA';
//         }
//     }
        s = "\tEXAB\r\n\tEXAB\r\n";
        while (asmtext.contains(s)) asmtext.remove(asmtext.indexOf(s), s.length());

//        assignfile(f, 'temp.asm");
//        rewrite(f);
        writeln(f, asmtext);
        writeln(f, libtext);
//        closefile(f);

#if 0
        {
        // Replace LIA... EXAB to LIB
        assignfile(f, 'temp.asm');
        reset(f);
        assignfile(f2, 'temp2.asm');
        rewrite(f2);

        while not eof(f) do
        begin
                readln(f, s);
                if copy(trim(s),1,3) = 'LIA' then
                begin
                        name := s;
                        readln(f, s);
                        if copy(trim(s),1,4) = 'EXAB' then
                        begin
                                s := #9'LIB' + copy(name,5,255);
                                writeln(f2, s);
                        end else
                        begin
                                writeln(f2, name);
                                writeln(f2, s);
                        end;
                end else
                        writeln(f2, s);
        end;
        closefile(f2);
        closefile(f);

        // Replace PUSH LIB... POP to LIB...
        assignfile(f, 'temp2.asm');
        reset(f);
        assignfile(f2, 'temp.asm');
        rewrite(f2);

        while not eof(f) do
        begin
                readln(f, s);
                if copy(trim(s),1,4) = 'PUSH' then
                begin
                        name := s;
                        readln(f, s);
                        if copy(trim(s),1,3) = 'LIB' then
                        begin
                                typ := s;
                                readln(f, s);
                                if copy(trim(s),1,3) = 'POP' then
                                begin
                                        writeln(f2, typ);
                                end else
                                begin
                                        writeln(f2, name);
                                        writeln(f2, typ);
                                        writeln(f2, s);
                                end;
                        end else
                        begin
                                writeln(f2, name);
                                writeln(f2, s);
                        end;
                end else
                begin
                        writeln(f2, s);
                end;
        end;
        closefile(f2);
        closefile(f);

        // Remove double EXAB
        assignfile(f, 'temp.asm');
        reset(f);
        assignfile(f2, 'temp2.asm');
        rewrite(f2);

        while not eof(f) do
        begin
                readln(f, s);
                if copy(trim(s),1,3) = 'EXAB' then
                begin
                        name := s;
                        readln(f, s);
                        if copy(trim(s),1,4) <> 'EXAB' then
                        begin
                                writeln(f2, name);
                                writeln(f2, s);
                        end;
                end else
                        writeln(f2, s);
        end;
        closefile(f2);
        closefile(f);

        // Replace n++ code to INCA
        assignfile(f, 'temp2.asm');
        reset(f);
        assignfile(f2, 'temp.asm');
        rewrite(f2);

        while not eof(f) do
        begin
                readln(f, s);
                if (copy(trim(s),1,6) = 'LIB\t1'#9) or (s = 'LIB\t1') then
                begin
                        name := s;
                        readln(f, s);
                        if copy(trim(s),1,4) = 'LP\t3' then
                        begin
                                typ := s;
                                readln(f, s);
                                if copy(trim(s),1,3) = 'ADM' then
                                begin
                                        s2 := s;
                                        readln(f, s);
                                        if copy(trim(s),1,4) = 'EXAB' then
                                        begin
                                                writeln(f2, #9'INCA');
                                        end else
                                        begin
                                                writeln(f2, name);
                                                writeln(f2, typ);
                                                writeln(f2, s2);
                                                writeln(f2, s);
                                        end;
                                end else
                                begin
                                        writeln(f2, name);
                                        writeln(f2, typ);
                                        writeln(f2, s);
                                end;
                        end else
                        begin
                                writeln(f2, name);
                                writeln(f2, s);
                        end;
                end else
                begin
                        writeln(f2, s);
                end;
        end;
        closefile(f2);
        closefile(f);

        // Replace n-- code to DECA
        assignfile(f, 'temp.asm');
        reset(f);
        assignfile(f2, filen);
        rewrite(f2);
        i := 0;

        while not eof(f) do
        begin
                readln(f, s);
                if (copy(trim(s),1,6) = 'LIB\t1'#9) or (s = 'LIB\t1') then
                begin
                        name := s;
                        readln(f, s);
                        if copy(trim(s),1,4) = 'LP\t3' then
                        begin
                                typ := s;
                                readln(f, s);
                                if copy(trim(s),1,4) = 'EXAB' then
                                begin
                                        s2 := s;
                                        readln(f, s);
                                        if copy(trim(s),1,3) = 'SBM' then
                                        begin
                                                s3 := s;
                                                readln(f, s);
                                                if copy(trim(s),1,4) = 'EXAB' then
                                                begin
                                                        writeln(f2, #9'DECA'); i++;
                                                end else
                                                begin
                                                        writeln(f2, name); if name<> '' then inc(i);
                                                        writeln(f2, typ); if typ<> '' then inc(i);
                                                        writeln(f2, s2); if s2<> '' then inc(i);
                                                        writeln(f2, s3); if s3<> '' then inc(i);
                                                        writeln(f2, s); if s<> '' then inc(i);
                                                end;
                                        end else
                                        begin
                                                writeln(f2, name); if name<> '' then inc(i);
                                                writeln(f2, typ); if typ<> '' then inc(i);
                                                writeln(f2, s2); if s2<> '' then inc(i);
                                                writeln(f2, s); if s<> '' then inc(i);
                                        end;
                                end else
                                begin
                                        writeln(f2, name); if name<> '' then inc(i);
                                        writeln(f2, typ); if typ<> '' then inc(i);
                                        writeln(f2, s); if s<> '' then inc(i);
                                end;
                        end else
                        begin
                                writeln(f2, name); if name<> '' then inc(i);
                                writeln(f2, s); if s<> '' then inc(i);
                        end;
                end else
                begin
                        writeln(f2, s);  if s<> '' then inc(i);
                end;
        end;
        closefile(f2);
        closefile(f);

        deletefile('temp.asm');
        deletefile('temp2.asm');
    }
#endif
        writeln("output",tr("Complete: %1 assembler lines were produced!").arg(i));
    }
//{--------------------------------------------------------------}




#endif
