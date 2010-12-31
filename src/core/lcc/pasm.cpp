#include <QtGui>

#include "pasm.h"
#include "parser.h"

Cpasm::Cpasm() {
    nlabp =-1;
    nlabcnt = 0;

    labp = 0;
    codpos = 0;
    mcase = false;

    startadr = 0;
    symcnt = 0;
    opp = 0;
}

int Cpasm::mathparse(QByteArray s, int w) {
    parser op;
    int y = op.evaluate_expression(s.data());

    return y;
}

const QString Cpasm::opcode[] = {
    "LII","LIJ","LIA","LIB","IX",
    "DX","IY","DY","MVW","EXW",
    "MVB","EXB","ADN","SBN","ADW",
    "SBW","LIDP","LIDL","LIP","LIQ",
    "ADB","SBB","?022?","?023?","MVWD",
    "EXWD","MVBD","EXBD","SRW","SLW",
    "FILM","FILD","LDP","LDQ","LDR",
    "RA","IXL","DXL","IYS","DYS",
    "JRNZP","JRNZM","JRNCP","JRNCM","JRP",
    "JRM","?046?","LOOP","STP","STQ",
    "STR","?051?","PUSH","DATA","?054?",
    "RTN","JRZP","JRZM","JRCP","JRCM",
    "?060?","?061?","?062?","?063?","INCI",
    "DECI","INCA","DECA","ADM","SBM",
    "ANMA","ORMA","INCK","DECK","INCM",
    "DECM","INA","NOPW","WAIT","WAITI",
    "INCP","DECP","STD","MVDM","READM",
    "MVMD","READ","LDD","SWP","LDM",
    "SL","POP","?092?","OUTA","?094?",
    "OUTF","ANIM","ORIM","TSIM","CPIM",
    "ANIA","ORIA","TSIA","CPIA","?104?",
    "DTJ","?106?","TEST","?108?","?109?",
    "?110?","?111?","ADIM","SBIM","?114?",
    "?115?","ADIA","SBIA","?118?","?119?",
    "CALL","JP","PTJ","?123?","JPNZ",
    "JPNC","JPZ","JPC","LP00","LP01",
    "LP02","LP03","LP04","LP05","LP06",
    "LP07","LP08","LP09","LP10","LP11",
    "LP12","LP13","LP14","LP15","LP16",
    "LP17","LP18","LP19","LP20","LP21",
    "LP22","LP23","LP24","LP25","LP26",
    "LP27","LP28","LP29","LP30","LP31",
    "LP32","LP33","LP34","LP35","LP36",
    "LP37","LP38","LP39","LP40","LP41",
    "LP42","LP43","LP44","LP45","LP46",
    "LP47","LP48","LP49","LP50","LP51",
    "LP52","LP53","LP54","LP55","LP56",
    "LP57","LP58","LP59","LP60","LP61",
    "LP62","LP63","INCJ","DECJ","INCB",
    "DECB","ADCM","SBCM","TSMA","CPMA",
    "INCL","DECL","INCN","DECN","INB",
    "?205?","NOPT","?207?","SC","RC",
    "SR","?211?","ANID","ORID","TSID",
    "?215?","LEAVE","?217?","EXAB","EXAM",
    "?220?","OUTB","?222?","OUTC","CAL00",
    "CAL01","CAL02","CAL03","CAL04","CAL05",
    "CAL06","CAL07","CAL08","CAL09","CAL10",
    "CAL11","CAL12","CAL13","CAL14","CAL15",
    "CAL16","CAL17","CAL18","CAL19","CAL20",
    "CAL21","CAL22","CAL23","CAL24","CAL25",
    "CAL26","CAL27","CAL28","CAL29","CAL30",
    "CAL31"};

const unsigned char nbargu [] = {
    2,2,2,2,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,3,2,2,2,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    2,2,2,2,2,
    2,1,2,1,1,
    1,1,1,1,1,
    1,2,2,2,2,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,2,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,2,2,2,2,
    2,2,2,2,1,
    1,1,2,1,1,
    1,1,2,2,1,
    1,2,2,1,1,
    3,3,1,1,3,
    3,3,3,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,2,2,2,
    1,1,1,1,1,
    1,1,1,1,2,
    2,2,2,2,2,
    2,2,2,2,2,
    2,2,2,2,2,
    2,2,2,2,2,
    2,2,2,2,2,
    2,2,2,2,2,
    2};

QString Cpasm::replace_text(QString text, QString such, QString ers) {

    QString regex = "([^_0-9A-Za-z])("+such+")([^_0-9A-Za-z])";
    return text.replace(QRegExp(regex),"\\1"+ers+"\\3");
}

void Cpasm::abort(QString t) {
    writeln("ERROR",tr("Line %1: ").arg(cline+1) + t + " in file " + cf);
 // halt;
}

bool Cpasm::findnlabel(QString l) {
    nlabp = nlab.indexOf(l.toUpper());
    return (nlabp >=0);
}

void Cpasm::addnlabel(QString label) {
    label = label.toUpper();
    nlab.append(label);

    nlabpos.append(codpos);
    nlabasm.append((mcase?"#":"")+op + " " + params);

    nlabcnt++;
}

void Cpasm::delnlabel(int l) {
    nlab.removeAt(l);
    nlabpos.removeAt(l);
    nlabasm.removeAt(l);
    nlabcnt--;
}

bool Cpasm::findlabel(QString label) {
    labp = lab.indexOf(label.toUpper());
    return (labp >=0);
}

void Cpasm::addlabel(QString l) {
    int  tpos;
    bool bup;

    l = l.toUpper();

    if (findlabel(l)) abort("Label " + l + " already defined!");
    writeln("SYMBOL: " + l + tr(" - %1").arg(codpos + startadr));
    lab.append(l);
    labpos.append(codpos);
    labcnt++;
    while (findnlabel(l)) {
        tpos = codpos;
        bup = mcase;
        codpos = nlabpos[nlabp];
        tok = nlabasm[nlabp];
        if (tok[0] == '#') { mcase = true; tok.remove(0,1); }
        extractop(tok);
        doasm();
        codpos = tpos;
        delnlabel(nlabp);
        mcase = bup;
    }
}

bool Cpasm::findsymbol(QString l) {
 return sym.contains(l);
}

void Cpasm::addsymbol(QString s1, QString s2) {
    if (findsymbol(s1)) abort("Symbol " + s1 + " already defined!");
    sym.append(s1);
    symval.append(s2);
    writeln("SYMBOL: " + s1 + " - " + s2);
    symcnt++;
}

bool Cpasm::findop(QString l) {
    opp = opcode->indexOf(l);
    return (opp>=0);
}

void Cpasm::addcode(unsigned char b) {
    code.append(b);
    codpos++;
    if ((codpos + startadr) >= 65536) abort("Code exceeds maximum memory!");
}

void Cpasm::extractop(QString s) {
    int i;

    if (s.isEmpty()) return;
    s.append(" ");

    i = 1;
    //    while s[i] in [' ', #9] do inc(i);
    //    delete(s, 1, i - 1);
    s = s.trimmed();

    i = 0;
    while (! QString(" \t").contains(s[i])) i++;
    op = s.left(i).trimmed().toUpper();
    s.remove(0,i); s = s.trimmed();
    params = s;

    if (s.contains(",")) s[s.indexOf(",")] = " ";

    i = 1; s.append(" ");
    while ((i < s.size()) && !QString(" \t").contains(s[i])) i++;
    param = s.left(i-1).trimmed();
    s.remove(0,i); s = s.trimmed();

    if (!s.isEmpty()) param2 = s; else param2 = "";
}

int Cpasm::calcadr(void) {
    int i,result;
    QString s, s2;
    bool lf;

    result = 0;
    i = 0;
    lf = false;
    while (i < params.length()) {
        if ((params[i] == '0') &&
            ((i < length(params)-1) && (params[i+1].toUpper() == 'X'))) {
            i+=2;
            while (QString("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ").contains(params[i])) i++;
        }
        if ((params[i] == '0') &&
            ((i < length(params)-1) && (params[i+1].toUpper() == 'B'))) {
            i+=2;
            while (QString("01").contains(params[i])) i++;
        }
        if (QString("_ABCDEFGHIJKLMNOPQRSTUVWXYZ").contains(params[i].toUpper())) {
            s = "";
            while ((i < params.length()) &&
                   QString("_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ").contains(params[i].toUpper())) {
                s.append(params[i]);
                i++;
            }
            if (!findlabel(s)) {
                addnlabel(s);
                lf = true;
            }else {
                s2 = QString("%1").arg(startadr + labpos[labp]);
                params = replace_text(params, s, s2);
                param1 = s;
                param2 = "";
                i = i - s.length() + s2.length();
            }
        }
        i++;
    }
    if (lf) result = 0;
    else if (param2.isEmpty()) result = mathparse(param1, 16);
    else result = mathparse(param1, 8) * 256 + mathparse(param2, 8);

    return result;
}


#define NOP 77
#define JRNZP 40
#define JRNZM 41
#define JRNCP 42
#define JRNCM 43
#define JRP 44
#define JRM 45
#define LOOP 47
#define JRZP 56
#define JRZM 57
#define JRCP 58
#define JRCM 59
const QList<char> Cpasm::JRPLUS = QList<char> ()<<JRNZP<<JRNCP<<JRP<<JRZP<<JRCP;
const QList<char> Cpasm::JRMINUS = QList<char> ()<<JRNZM<<JRNCM<<JRM<<LOOP<<JRZM<<JRCM;
const QList<char> Cpasm::JR = QList<char> ()<<JRNZP<<JRNCP<<JRP<<JRZP<<JRCP<<JRNZM<<JRNCM<<JRM<<LOOP<<JRZM<<JRCM;


void Cpasm::doasm(void) {
    int adr;

        param1 = param1.trimmed();
        if (!param1.isEmpty())
           if (param1.right(1) == ",")
              param1.chop(1);
        param2 = param2.trimmed();

        QList<char> l1 = QList<char> () << 120<<121<<16<<124<<125<<126<<127;
        if (findop(op)) {
            if (l1.contains(opp)) {
                adr = calcadr;
                if (adr > 0) {
                     addcode(opp);
                     addcode(adr / 256);
                     addcode(adr % 256);
                 }
                else {
                     addcode(NOP);
                     addcode(NOP);
                     addcode(NOP);
                 }
            }
            else
                if (JR.contains(opp)) {              // relativ

              adr = calcadr;
              if (adr >= 8192) {
                   addcode(opp);
//                   if opp in JRPLUS then addcode(adr - codpos - startadr)
//                   else
                   addcode(abs(codpos + startadr - adr));
               }
              else
                  if (adr > 0) {
                   addcode(opp); addcode(adr);
               }
              else {
                   addcode(NOP); addcode(NOP);
               }
          }
            else {
              addcode(opp);
              if (NBARGU[opp] == 2) addcode(mathparse(param1, 8));
              else if (NBARGU[opp] == 3) {
                  if (param2.isEmpty()) {
                    addcode((mathparse(param1, 16) << 8) & 0xFF);
                    addcode((mathparse(param1, 16)) & 0xFF);
                }
                  else {
                    addcode(mathparse(param1, 8));
                    addcode(mathparse(param2, 8));
                }
              }
          }
        }
        else {

            if (op == "LP") {
           if (mathparse(param1, 8) > 63) abort("LP command exceeds range!");
           addcode(128 + mathparse(param1, 8));
       }
            else
        if (op == "RTN") addcode(55);
        else if (op == "SUBW") addcode(0x15);
        else if (op == "SUBC") addcode(0xC5);
        else if (op == "ADDW") addcode($14);
        else if (op == "ADDC") addcode($C4);
        else if (op == "ADD") {
           if ((param1 == "[P]") && (param2 == "A")) addcode(0x44);
           else if (param1 == "[P]") { addcode(0x70); addcode(mathparse(param2, 8)); }
           else if (param1 == "A") { addcode(0x74); addcode(mathparse(param2, 8)); }
       }
        else if (op = "ADDB") {
           if ((param1 = "[P]") && (param2 == "A")) addcode(0x0C);
           else if ((param1 == "[P]") && (param2 == "[Q]")) addcode(0x0E);
       }
        else if (op = "SUB") {
           if ((param1 = "[P]") && (param2 = "A")) addcode(0x45);
           else if (param1 == "[P]") { addcode(0x71); addcode(mathparse(param2, 8)); }
           else if (param1 = 'A') { addcode($74); addcode(mathparse(param2, 8)); }
       }
        else if (op == "SUBB") {
           if ((param1 == "[P]") && (param2 == "A")) addcode(0x0D);
           else if ((param1 == "[P]") && (param2 == "[Q]")) addcode(0x0F);
       }
        else if op = 'ROL' then
           addcode($5A)
        else if op = 'SLB' then
           addcode($1D)
        else if op = 'ROR' then
           addcode(210)
        else if op = 'SRB' then
           addcode(28)
        else if op = 'SWAP' then
           addcode(88)
        else if op = 'RC' then
           addcode(209)
        else if op = 'SC' then
           addcode(208)
        else if (op = 'OR') then
        begin
           if (param1 = '[P]') and (param2 = 'A') then addcode($47)
           else if (param1 = '[P]') then begin addcode($61); addcode(mathparse(param2, 8)); end
           else if (param1 = '[DP]') then begin addcode($D5); addcode(mathparse(param2, 8)); end
           else if (param1 = 'A') then begin addcode($65); addcode(mathparse(param2, 8)); end;
        end
        else if (op = 'AND') then
        begin
           if (param1 = '[P]') and (param2 = 'A') then addcode($46)
           else if (param1 = '[P]') then begin addcode($60); addcode(mathparse(param2, 8)); end
           else if (param1 = '[DP]') then begin addcode($D4); addcode(mathparse(param2, 8)); end
           else if (param1 = 'A') then begin addcode($64); addcode(mathparse(param2, 8)); end;
        end
        else if (op = 'OUT') then
        begin
           if (param1 = 'A') then addcode(93)
           else if (param1 = 'B') then addcode(221)
           else if (param1 = 'C') then addcode(223)
           else if (param1 = 'F') then addcode(95);
        end
        else if (op = 'IN') then
        begin
           if (param1 = 'A') then addcode(76)
           else if (param1 = 'B') then addcode(204);
        end
        else if (op = 'INC') then
        begin
           if (param1 = 'A') or (mathparse(param1, 8) = 2) then addcode(66)
           else if (param1 = 'B') or (mathparse(param1, 8) = 3) then addcode(194)
           else if (param1 = 'J') or (mathparse(param1, 8) = 1) then addcode(192)
           else if (param1 = 'K') or (mathparse(param1, 8) = 8) then addcode(72)
           else if (param1 = 'L') or (mathparse(param1, 8) = 9) then addcode(200)
           else if (param1 = 'M') or (mathparse(param1, 8) = 10) then addcode(74)
           else if (param1 = 'N') or (mathparse(param1, 8) = 11) then addcode(202)
           else if (param1 = 'P') then addcode(80)
           else if (param1 = 'X') or (mathparse(param1, 8) = 4) then addcode(4)
           else if (param1 = 'Y') or (mathparse(param1, 8) = 6) then addcode(6)
           else if (param1 = 'I') or (mathparse(param1, 8) = 0) then addcode(64);
        end
        else if (op = 'DEC') then
        begin
           if (param1 = 'A') or (mathparse(param1, 8) = 2) then addcode(67)
           else if (param1 = 'B') or (mathparse(param1, 8) = 3) then addcode(195)
           else if (param1 = 'J') or (mathparse(param1, 8) = 1) then addcode(193)
           else if (param1 = 'K') or (mathparse(param1, 8) = 8) then addcode(73)
           else if (param1 = 'L') or (mathparse(param1, 8) = 9) then addcode(201)
           else if (param1 = 'M') or (mathparse(param1, 8) = 10) then addcode(75)
           else if (param1 = 'N') or (mathparse(param1, 8) = 11) then addcode(203)
           else if (param1 = 'P') then addcode(81)
           else if (param1 = 'X') or (mathparse(param1, 8) = 4) then addcode(5)
           else if (param1 = 'Y') or (mathparse(param1, 8) = 6) then addcode(7)
           else if (param1 = 'I') or (mathparse(param1, 8) = 0) then addcode(65);
        end
        else if op = 'CALL' then
        begin
           adr := calcadr;
           if adr > 0 then
           begin
                   if adr < 8192 then begin addcode($E0 + adr div 256); addcode(adr mod 256); end
                   else begin addcode($78); addcode(adr div 256); addcode(adr mod 256); end;
           end else
           begin
                   addcode(NOP); addcode(NOP); addcode(NOP);
           end;
        end
        else if op = 'JMP' then
        begin
           adr := calcadr;
           if adr > 0 then
           begin
                   addcode(121); addcode(adr div 256); addcode(adr mod 256);
           end else
           begin
                   addcode(NOP); addcode(NOP); addcode(NOP);
           end;
        end
        else if op = 'JPLO' then
        begin
           adr := calcadr;
           if adr > 0 then
           begin
                   addcode(127); addcode(adr div 256); addcode(adr mod 256);
           end else
           begin
                   addcode(NOP); addcode(NOP); addcode(NOP);
           end;
        end
        else if op = 'JPSH' then
        begin
           adr := calcadr;
           if adr > 0 then
           begin
                   addcode(125); addcode(adr div 256); addcode(adr mod 256);
           end else
           begin
                   addcode(NOP); addcode(NOP); addcode(NOP);
           end;
        end
        else if op = 'JPNE' then
        begin
           adr := calcadr;
           if adr > 0 then
           begin
                   addcode(124); addcode(adr div 256); addcode(adr mod 256);
           end else
           begin
                   addcode(NOP); addcode(NOP); addcode(NOP);
           end;
        end
        else if op = 'JPEQ' then
        begin
           adr := calcadr;
           if adr > 0 then
           begin
                   addcode(126); addcode(adr div 256); addcode(adr mod 256);
           end else
           begin
                   addcode(NOP); addcode(NOP); addcode(NOP);
           end;
        end
        else if op = 'RJMP' then
        begin
           adr := calcadr;
           if adr >= 8192 then
           begin
               if abs(adr - startadr - codpos) <= 255 then
               begin
                   if adr > startadr + codpos then addcode(44)
                   else addcode(45);
                   addcode(abs(adr - startadr - codpos));
               end else
               begin
                   addcode(121); addcode(adr div 256); addcode(adr mod 256);   // Do absolute jump then
               end;
           end else
           if (adr >= 1) and (adr <= 255) then
           begin
                   addcode(44); addcode(adr);
           end else
           if (adr <= -1) and (adr >= -255) then
           begin
                   addcode(45); addcode(adr);
           end else
           begin
                   addcode(NOP); addcode(NOP);
           end;
        end
        else if op = 'BRLO' then
        begin
           adr := calcadr;
           if adr >= 8192 then
           begin
               if abs(adr - startadr - codpos) <= 255 then
               begin
                   if adr > startadr + codpos then addcode(58)
                   else addcode(59);
                   addcode(abs(adr - startadr - codpos));
               end else abort('Relative jump exceeds 255 bytes!');
           end else
           if (adr >= 1) and (adr <= 255) then
           begin
                   addcode(58); addcode(adr);
           end else
           if (adr <= -1) and (adr >= -255) then
           begin
                   addcode(59); addcode(adr);
           end else
           if (abs(adr) >= 256) and (abs(adr) < 8192) then
           begin
                   adr := startadr + codpos + adr;
                   addcode(127); addcode(adr div 256); addcode(adr mod 256);
           end else
           begin
                   addcode(NOP); addcode(NOP);
           end;
        end
        else if op = 'BRSH' then
        begin
           adr := calcadr;
           if adr >= 8192 then
           begin
               if abs(adr - startadr - codpos) <= 255 then
               begin
                   if adr > startadr + codpos then addcode(42)
                   else addcode(43);
                   addcode(abs(adr - startadr - codpos));
               end else abort('Relative jump exceeds 255 bytes!');
           end else
           if (adr >= 1) and (adr <= 255) then
           begin
                   addcode(42); addcode(adr);
           end else
           if (adr <= -1) and (adr >= -255) then
           begin
                   addcode(43); addcode(adr);
           end else
           if (abs(adr) >= 256) and (abs(adr) < 8192) then
           begin
                   adr := startadr + codpos + adr;
                   addcode(125); addcode(adr div 256); addcode(adr mod 256);
           end else
           begin
                   addcode(NOP); addcode(NOP);
           end;
        end
        else if op = 'BRNE' then
        begin
           adr := calcadr;
           if adr >= 8192 then
           begin
               if abs(adr - startadr - codpos) <= 255 then
               begin
                   if adr > startadr + codpos then addcode(40)
                   else addcode(40);
                   addcode(abs(adr - startadr - codpos));
               end else abort('Relative jump exceeds 255 bytes!');
           end else
           if (adr >= 1) and (adr <= 255) then
           begin
                   addcode(40); addcode(adr);
           end else
           if (adr <= -1) and (adr >= -255) then
           begin
                   addcode(41); addcode(adr);
           end else
           if (abs(adr) >= 256) and (abs(adr) < 8192) then
           begin
                   adr := startadr + codpos + adr;
                   addcode(124); addcode(adr div 256); addcode(adr mod 256);
           end else
           begin
                   addcode(NOP); addcode(NOP);
           end;
        end
        else if op = 'BREQ' then
        begin
           adr := calcadr;
           if adr >= 8192 then
           begin
               if abs(adr - startadr - codpos) <= 255 then
               begin
                   if adr > startadr + codpos then addcode(56)
                   else addcode(57);
                   addcode(abs(adr - startadr - codpos));
               end else abort('Relative jump exceeds 255 bytes!');
           end else
           if (adr >= 1) and (adr <= 255) then
           begin
                   addcode(56); addcode(adr);
           end else
           if (adr <= -1) and (adr >= -255) then
           begin
                   addcode(57); addcode(adr);
           end else
           if (abs(adr) >= 256) and (abs(adr) < 8192) then
           begin
                   adr := startadr + codpos + adr;
                   addcode(126); addcode(adr div 256); addcode(adr mod 256);
           end else
           begin
                   addcode(NOP); addcode(NOP);
           end;
        end
        else if (op = 'CASE') then
        begin
           addcode(122);
           mcase := true;
           ccase := codpos;
           casecnt := 0;
           addcode(NOP); addcode(NOP); addcode(NOP);
           addcode(105);
        end
        else if mcase then
        begin
           if (op = 'ENDCASE') then
           begin
              mcase := false;
              adr := codpos;
              codpos := ccase;
              addcode(casecnt);
              addcode((adr+startadr) div 256); addcode((adr+startadr) mod 256);
              codpos := adr;
           end else
           begin
              if op <> 'ELSE' then
                 begin addcode(mathparse(op, 8)); inc(casecnt); end;
              dec(codpos);
              adr := calcadr;
              inc(codpos);
              if adr > 0 then
                 begin addcode(adr div 256); addcode(adr mod 256); end
              else
                 begin addcode(NOP); addcode(NOP); end;
           end;
        end
        else if op = 'MOV' then
        begin
           if (param1 = 'A') and (param2 = '[+X]') then addcode(36)
           else if (param1 = 'A') and (param2 = '[-X]') then addcode(37)
           else if (param1 = '[+Y]') and (param2 = 'A') then addcode(38)
           else if (param1 = '[-Y]') and (param2 = 'A') then addcode(39)
           else if (param1 = 'A') and (param2 = 'R') then addcode(34)
           else if (param1 = 'R') and (param2 = 'A') then addcode(50)
           else if (param1 = 'A') and (param2 = 'Q') then addcode(33)
           else if (param1 = 'Q') and (param2 = 'A') then addcode(49)
           else if (param1 = 'A') and (param2 = 'P') then addcode(32)
           else if (param1 = 'P') and (param2 = 'A') then addcode(48)
           else if (param1 = '[P]') and (param2 = '[DP]') then addcode(85)
           else if (param1 = '[DP]') and (param2 = '[P]') then addcode(83)
           else if (param1 = 'A') and (param2 = '[DP]') then addcode(87)
           else if (param1 = '[DP]') and (param2 = 'A') then addcode(82)
           else if (param1 = 'A') and (param2 = '[P]') then addcode(89)
           else if (param1 = '[P]') and (param2 = 'A') then begin addcode(219); addcode(89); end
           else if (param1 = 'A') then begin addcode(2); addcode(mathparse(param2, 8)); end
           else if (param1 = 'B') then begin addcode(3); addcode(mathparse(param2, 8)); end
           else if (param1 = 'I') then begin addcode(0); addcode(mathparse(param2, 8)); end
           else if (param1 = 'J') then begin addcode(1); addcode(mathparse(param2, 8)); end
           else if (param1 = 'P') then begin addcode(18); addcode(mathparse(param2, 8)); end
           else if (param1 = 'Q') then begin addcode(19); addcode(mathparse(param2, 8)); end
           else if (param1 = 'DPL') then begin addcode(17); addcode(mathparse(param2, 8)); end
           else if (param1 = 'DP') then begin addcode(16); adr := mathparse(param2, 16); addcode(adr div 256); addcode(adr mod 256); end;
        end
        else if op = 'NOP' then
        begin
           if param1 = '' then addcode(NOP) else begin addcode(78); addcode(mathparse(param1, 8)); end;
        end
        else
           abort('Unknown OP-code: '+op);

        end;
    }


#if 0
program pasm;
{$APPTYPE CONSOLE}
{%File 'ModelSupport\default.txvpck'}

uses
  sysutils,
  calcunit;

var
      param1, param2: string;




    blcnt: integer;


        ccase: integer = 0;
        casecnt: integer;
         i: integer;

//	f: textfile;
//	f2: file of byte;
//        wr: integer;
        s, cf: string;

const
        NOP = 77;
        JRNZP = 40;
        JRNZM = 41;
        JRNCP = 42;
        JRNCM = 43;
        JRP = 44;
        JRM = 45;
        LOOP = 47;
        JRZP = 56;
        JRZM = 57;
        JRCP = 58;
        JRCM = 59;
        JRPLUS = [JRNZP,JRNCP,JRP,JRZP,JRCP];
        JRMINUS = [JRNZM,JRNCM,JRM,LOOP,JRZM,JRCM];
        JR = [JRNZP,JRNCP,JRP,JRZP,JRCP,JRNZM,JRNCM,JRM,LOOP,JRZM,JRCM];










procedure parsefile(fname: string);
var datei: textfile;
    lcnt: integer;

function readline: string;
var
    i: integer;
    c: boolean;
begin
    repeat
    repeat
        readln(datei, result);
                inc(lcnt);
                cline := lcnt;
                result := trim(result);
    until eof(datei) or (result <> '');
    if eof(datei) and (result = '') then exit;

    c := false;
    for i := 1 to length(result) do
    begin
        if (result[i] = '''') or (result[i] = '"') then c := not c;
        if (not c) and (result[i] in [';','#']) then
        begin
            delete(result, i, length(result));
            break;
        end;
    end;
        if result <> '' then
        begin
        i := pos(':', result);
        if i > 0 then
        begin
        addlabel(trim(copy(result, 1, i - 1)));
        delete(result, 1, i);
                result := trim(result);
            end;
        end;
        if result <> '' then
        begin
            if pos('ifdef', lowercase(result)) = 0 then
            begin
                if symcnt > 0 then for i := 0 to symcnt - 1 do
                        result := replace_text(result, sym[i], symval[i]);
                if labcnt > 0 then for i := 0 to labcnt - 1 do
                        result := replace_text(result, lab[i], inttostr(labpos[i] + startadr));
            end;
        end;
     until eof(datei) or (result <> '');
end;

begin
  if not fileexists(fname) then
  begin
    writeln('File '+fname+' not found!');
    halt;
  end;
    assignfile(datei, fname);
    reset(datei);
        cf := fname;
        lcnt := 0;

    tok := readline; // Überspringt Leerzeilen und entfernt Kommentare
    while (not eof(datei)) or (tok <> '') do
    begin
            if pos('.endif', tok) = 0 then
            begin

        extractop(tok);
        if op = '.ORG' then
                begin
                        startadr := mathparse(param1, 16);
                end
        else if op = '.EQU' then
                begin
                        addsymbol(param1, param2);
                end
                else if op = '.DB' then
                begin
                        while pos(',', params) > 0 do
                        begin
                                s := copy(params, 1, pos(',', params) - 1);
                                //if s[1] = '''' then addcode(ord(s[2])) else
                                addcode(mathparse(s, 8));
                                delete(params, 1, pos(',', params));
                        end;
                        params := trim(params);
                        //if params[1] = '''' then addcode(ord(params[2])) else
                        addcode(mathparse(params, 8));
                end
                else if op = '.DW' then
                begin
                        while pos(',', params) > 0 do
                        begin
                                s := copy(params, 1, pos(',', params) - 1);
                                addcode((mathparse(s, 16) shr 8) and $FF);
                                addcode(mathparse(s, 16) and $FF);
                                delete(params, 1, pos(',', params));
                        end;
                        addcode((mathparse(params, 16) shr 8) and $FF);
                        addcode(mathparse(params, 16) and $FF);
                end
                else if op = '.DS' then
                begin
                        delete(params, 1, pos('"', params));
                        while (params <> '') and (params[1] <> '"') do
                        begin
                                if params[1] = '\' then
                                begin
                                   if params[2] = '\' then addcode(ord('\'))
                                   else
                                   begin
                                      addcode(converthex(uppercase(copy(params, 3, 2))));
                                      delete(params, 1, 4);
                                   end;
                                end else
                                begin
                                   addcode(ord(params[1]));
                                   delete(params, 1, 1);
                                end;
                        end;
                end
                else if op = '.IFDEF' then
                begin
                        if not findsymbol(param1) then
                        while not eof(datei) and (op <> '.ENDIF') do
                        begin
                                tok := readline;
                                //extractop(tok);
                                if pos('.endif', lowercase(tok)) > 0 then
                                   op := '.ENDIF';
                        end;
                end
                else if op = '.INCLUDE' then
                begin
                        if fileexists(params) then
                        begin
                                parsefile(params);
                        end else abort('Include file ' + params + ' not found!');
                end
                else
                        doasm;
            end;
            tok := readline; // Überspringt Leerzeilen und entfernt Kommentare
    end;

    closefile(datei);
end;


procedure savefile(fname: string);
var f: textfile;
    f2: file of byte;
    i, wr: integer;
    s: string;
    b: byte;

begin
        if uppercase(paramstr(3)) = 'DEC' then
        begin
            assignfile(f, fname);
                rewrite(f);

                for wr := 0 to codpos - 1 do
                begin
                        b := code[wr];
                        writeln(f, inttostr(b));
                end;

            closefile(f);
        end else
        if uppercase(paramstr(3)) = 'BAS' then
        begin
            assignfile(f, paramstr(2));
                rewrite(f);
                blcnt := 10;
                s := '';
                i := 0;

                for wr := 0 to codpos - 1 do
                begin
                        b := code[wr];
                        s := s + ',' + inttostr(b);
                        if length(s) >= 60 then
                        begin
                                writeln(f, inttostr(blcnt) + ' POKE ' + inttostr(startadr) + s);
                                s := '';
                                blcnt := blcnt + 10;
                                startadr := startadr + i + 1;
                                i := -1;
                        end;
                        inc(i);
                end;
                if s <> '' then
                        writeln(f, inttostr(blcnt) + ' POKE ' + inttostr(startadr) + s);

            closefile(f);
        end else
        begin
            assignfile(f2, paramstr(2));
                rewrite(f2);

                for wr := 0 to codpos - 1 do
                begin
                        b := code[wr];
                        write(f2, b);
                end;

            closefile(f2);
        end;
end;



begin
        writeln('pasm v1.1 - Assembler for Pocket Computers with SC61860 CPU');
        writeln('(c) Simon Lehmayr 2004');
    if paramcount < 2 then
        begin
          writeln('Usage: pasm asmfile outputfile [mode]');
          writeln('       mode: bin = binary file output (default)');
          writeln('             dec = decimal file output');
          writeln('             bas = basic file output');
          exit;
        end;

        parsefile(paramstr(1));
        if nlabcnt > 0 then
        begin
          for i := 0 to nlabcnt - 1 do
            writeln('In line "' + nlabasm[i] + '": ' + nlab[i]);
          abort('Labels were not available!');
        end;

        writeln;
        writeln('Start address:'#9,startadr);
        writeln('End address:'#9,startadr+codpos-1);

        if codpos = 0 then
        begin
                addcode(55);
                writeln('An empty program was produced!');
                exit;
        end;

        savefile(paramstr(2));
end.


#endif
