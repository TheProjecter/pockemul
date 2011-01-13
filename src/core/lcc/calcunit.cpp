#include "parser/parser.h"
#include "lcc.h"


#define ENABLE_COMPILATION 1

#if ENABLE_COMPILATION





int Clcc::mathparse(QByteArray s, int w) {
//var i, p: integer;
//    c, s2: string;
//    lf: boolean;
    int i,p;
    QString c;
    bool lf;
    QString s2;
    int result=0;


    Parser op(s.data());
    int y = op.Evaluate();

    return y;

    if (s.contains("'")) {
        i = 0;
        while (i < s.length()) {
            if (s[i] == '\'') {
                int val = s[i+1];
                c = QString(val);
                s.remove(i,3);//delete(s, i, 3);
                s.insert(i,c);//insert(c, s, i);
                i = i - 3 + c.length();
            }
            i++;
        }
    }

        i = 0;
        lf = false;
        while (i < s.length()) {
            //if upcase(s[i]) in ['_','A'..'Z'] then
            if (QString("_ABCDEFGHIJKLMNOPQRSTUVWXYZ").contains(toupper(s[i]))) {
                if ((i > 0) && (s[i - 1] == '0')) {
                    i++;
                    if (toupper(s[i-1]) == 'X') i=skiphex(s, i);
                    else if (s[i-1] == 'B') i=skipbin(s, i);
                    i--;
                }
                else {
                    s2 = "";
                    while ((i < s.length()) &&
                           QString("_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ").contains(toupper(s[i]))) {
                        s2 = s2 + s[i];
                        i++;
                    }
                    if ( (s2.toUpper() != "LB") &&
                         (s2.toUpper() != "HB") &&
                         (s2.toUpper() != "NOT") &&
                         (s2.toUpper() != "SIN") &&
                         (s2.toUpper() != "TAN") &&
                         (s2.toUpper() != "COS") &&
                         (s2.toUpper() != "FAK") &&
                         (s2.toUpper() != "ABS") &&
                         (s2.toUpper() != "SQRT") &&
                         (s2.toUpper() != "SQR") &&
                         (s2.toUpper() != "LN") &&
                         (s2.toUpper() != "LOG") &&
                         (s2.toUpper() != "EXP") &&
                         (s2.toUpper() != "ARCTAN") &&
                         (s2.toUpper() != "PI") &&
                         (s2.toUpper() != "E") ) {
                        lf = true;
                    }
                }
            }
            i++;
        }
        if (lf) {
            return 0;
        }

    //Evaluate(s, w, i, p);
    result = i;
    if (p != 0) {
        QMessageBox::about(mainwindow,"ERROR","Erroneous formula: " + s);
        return (0);
    }
    return result;
}

#endif

#if 0
int Clcc::converthex(QString s) {
    bool ok;
    return s.toInt(&ok,16);
}

int Clcc::convertbin(QString s) {
    bool ok;
    return s.toInt(&ok,2);
}



int Clcc::Evaluate(QString Formula,    // {Fomula to evaluate}
                   int wid,              // { Place for x in formula }
                   int *ErrPosf) //  { Posfition of error }
{
//    const
//  HexNumbers: set of Char = ['0'..'9','A'..'F'];
//  BinNumbers: set of Char = ['0','1'];
//  Numbers: set of Char = ['0'..'9'];
//  EofLine  = ^M;

//var
//  Posf, i, c: Integer;    { Current Posfition in formula                     }
//  Ch: Char;           { Current character being scanned                 }
  QString dummy;
  int sumr, suml,c,Value;



  //{--first make the formula a little easier --}
  if (Formula.isEmpty()) {
      *ErrPosf = -1;
      return 0;
  }

    dummy = Formula.remove(' ');

  sumr=0;  suml=0;
  dummy.replace(',','.');
  sumr = dummy.count('(');
  suml = dummy.count(')');
  dummy = dummy.toUpper();


  if (dummy[0]=='.') dummy.insert(0,'0');
  if (dummy[0]=='+') dummy.remove(0,1);

  if (dummy.isEmpty()) {
      *ErrPosf=-1;
      return 0;
  }
  if (sumr != suml) {
      *ErrPosf=-2;
      return 0;
  }

  dummy.replace("<<","<");
  dummy.replace(">>",">");
  //if (Pos('<<',dummy) > 0) then delete(dummy, Pos('<<',dummy), 1);
  //if (Pos('>>',dummy) > 0) then delete(dummy, Pos('>>',dummy), 1);
  Formula=dummy;
  Posf=0;
  NextCh(Formula);
  c = 0;
  for (int i = 0; i <wid; i++) c =c | (1 << i);
  Value = trunc(Expression()) & c;
  if (Ch==0x0d) *ErrPosf=0; else *ErrPosf=Posf;
  return Value;
}

void Clcc::NextCh(QString Formula) {
    do {
        Posf =Posf+1;
        if (Posf<=Formula.length()) Ch =Formula[Posf].toAscii();
        else Ch=0x0d;
    }
    while (Ch ==' ');
}

#define ISNUMBERS(a) (((a)>='0') && ((a)<='9'))

float Factor(void) {

//StandardFunctionList = array[StandardFunction] of string[7];

//const
//StandardFunctionNames: StandardFunctionList =('ABS','SQRT','SQR','SIN','COS',
//                                          'ARCTAN','LN','LOG','EXP','FAK','PI','E','NOT','LB','HB');
//var
    enum StandardFunction {fabs,fsqrt,fsqr,fsin,fcos,farctan,fln,flog,fexp,ffact,fPi,fE,fnot,flb,fhb};


int L;  //       { intermidiate variables }
bool hex,bin,Found;
float F;
StandardFunction Sf;
int Start;
QString s;

{
    F=0;
    if (ISNUMBERS(Ch)) {
        Start=Posf;
        hex=false; bin=false;
        do { NextCh(); } while (ISNUMBERS(Ch));
        if (Ch=='.') do { NextCh(); } while (ISNUMBERS(toupper(Ch)));
        if (Ch=='E') {
                  NextCh();
                    repeat NextCh until not (Ch in Numbers);
            end;
            if Ch='X' then
begin
NextCh;
repeat NextCh until not (Ch in HexNumbers);
hex:=true;
end;
if Ch='B' then
begin
NextCh;
repeat NextCh until not (Ch in BinNumbers);
bin:=true;
end;

s:=Copy(Formula,Start,Posf-Start);
if hex then F:=converthex(copy(s,3,255))
else if bin then F:=convertbin(copy(s,3,255))
else Val(s,F,ErrPosf);
{              if upcase(Ch)='H' then
begin
F:=converthex(s)
end else
begin
if pos('B',s) > 0 then F:=convertbin(s)
else Val(s,F,ErrPosf);
end;}
end else
if Ch='(' then
begin
NextCh;
F:=Expression;
if Ch=')' then NextCh else ErrPosf:=Posf;
end else
begin
found:=false;
for sf:=fabs to fhb do
if not found then
begin
l:=Length(StandardFunctionNames[sf]);
if copy(Formula,Posf,l)=StandardFunctionNames[sf] then
begin
  Posf:=Posf+l-1; NextCh;
  F:=Factor;
//                  writeln(StandardFunctionNames[sf]);
  case sf of
    fabs:     f:=abs(f);
    fsqrt:    f:=sqrt(f);
    fsqr:     f:=sqr(f);
    fsin:     f:=sin(f);
    fcos:     f:=cos(f);
    farctan:  f:=arctan(f);
    fln :     f:=ln(f);
    flog:     f:=ln(f)/ln(10);
    fexp:     f:=exp(f);
    ffact:    f:=fakult(trunc(f));
    fPi:      f:=Pi;
    fE:       f:=Exp(1);
//                    fX:       f:=X;
    fnot:     f:=notbin(trunc(f));
    flb:      f:=trunc(f) and $FF;
    fhb:      f:=(trunc(f) shr 8) and $FF;
  end;
  Found:=true;
end;
end;
if not Found then ErrPosf:=Posf;
end;
result:=F;
end { function Factor};



float Clcc::SimpleExpression(void) {
//var
//  S: Real;
//  Opr: Char;

    float Term(void) {
//  var
//    T: Real;

        float SignedFactor(void) {


    begin { SignedFactor }
     if Ch='-' then
      begin
        NextCh; result:=-Factor;
      end
      else if Ch='~' then
      begin
        NextCh; result:=notbin(trunc(Factor));
      end else result:=Factor;
    end { SignedFactor };

  begin { Term }
    T:=SignedFactor;
    while Ch='^' do
    begin
      NextCh; t:=exp(ln(t)*SignedFactor);
    end;
    result:=t;
  end { Term };

begin { SimpleExpression }
  s:=term;
  while Ch in ['*','/','<','>','%'] do
  begin
    Opr:=Ch;  NextCh;
    case Opr of
      '*': s:=s*term;
      '/': s:=s/term;
      '<': s:=trunc(s) shl trunc(term);
      '>': s:=trunc(s) shr trunc(term);
      '%': s:=trunc(s) mod trunc(term);
    end;
  end;
  result:=s;
end { SimpleExpression };

#endif

#if 0

//{$MODE DELPHI}
unit calcunit;
// global

//uses
//  Classes;
//TYPE  Real=Extended;

interface
  procedure Evaluate(var Formula: String; wid: integer; var Value: integer;var ErrPosf: Integer);   { Posfition of error }
  function converthex(s: string): integer;


implementation



procedure Evaluate(var Formula: String;    { Fomula to evaluate}
                   wid      : integer;      { Place for x in formula }
                   var Value: integer;      { Result of formula }
                   var ErrPosf: Integer);   { Posfition of error }
const
  HexNumbers: set of Char = ['0'..'9','A'..'F'];
  BinNumbers: set of Char = ['0','1'];
  Numbers: set of Char = ['0'..'9'];
  EofLine  = ^M;

var
  Posf, i, c: Integer;    { Current Posfition in formula                     }
  Ch: Char;           { Current character being scanned                 }
  dummy : String;
  SumR, SumL  : Integer;

{ Procedure NextCh returns the next character in the formula         }
{ The variable Posf contains the Posfition ann Ch the character        }

  procedure NextCh;
  begin
    repeat
      Posf:=Posf+1;
      if Posf<=Length(Formula) then
      Ch:=Formula[Posf] else Ch:=eofline;
    until Ch<>' ';
  end  { NextCh };


  function Fakult(I: Integer): Real;  { Fakultaet }
  var  dummy   : double;
  begin
    IF i=1 THEN BEGIN  result:=1;  Exit;  END;
    dummy:=1;
    FOR i:=1 TO I DO Dummy:=dummy*i;
    result:=dummy;
  end  { Fact };

  function notbin(s: integer): integer;
  var i, c: integer;
  begin
    result := 0;
    c := 1;
//    writeln(s);

    for i := 0 to wid-1 do
    begin
      if (s and c) = 0 then result := result or c;
      c := c shl 1;
    end;
//    notbin := result;
  end;

  function Expression: Real;
  var
    E: Real;
    Opr: Char;

    function SimpleExpression: Real;
    var
      S: Real;
      Opr: Char;

      function Term: Real;
      var
        T: Real;

        function SignedFactor: Real;

          function Factor: Real;
          type
            StandardFunction = (fabs,fsqrt,fsqr,fsin,fcos,
            farctan,fln,flog,fexp,ffact,fPi,fE,fnot,flb,fhb);
            StandardFunctionList = array[StandardFunction] of string[7];

          const
            StandardFunctionNames: StandardFunctionList =('ABS','SQRT','SQR','SIN','COS',
                                                          'ARCTAN','LN','LOG','EXP','FAK','PI','E','NOT','LB','HB');
          var
            L:  Integer;       { intermidiate variables }
            hex,bin,Found:Boolean;
            F: Real;
            Sf:StandardFunction;
            Start:Integer;
            s:string;

          begin { Function Factor }
            F:=0;
            if Ch in Numbers then
            begin
              Start:=Posf; hex:=false; bin:=false;
              repeat NextCh until not (Ch in Numbers);
              if Ch='.' then repeat NextCh until not (upcase(Ch) in Numbers);
              if Ch='E' then
              begin
                NextCh;
                repeat NextCh until not (Ch in Numbers);
              end;
              if Ch='X' then
              begin
                NextCh;
                repeat NextCh until not (Ch in HexNumbers);
                hex:=true;
              end;
              if Ch='B' then
              begin
                NextCh;
                repeat NextCh until not (Ch in BinNumbers);
                bin:=true;
              end;

              s:=Copy(Formula,Start,Posf-Start);
              if hex then F:=converthex(copy(s,3,255))
              else if bin then F:=convertbin(copy(s,3,255))
              else Val(s,F,ErrPosf);
{              if upcase(Ch)='H' then
              begin
                F:=converthex(s)
              end else
              begin
                if pos('B',s) > 0 then F:=convertbin(s)
                else Val(s,F,ErrPosf);
              end;}
            end else
            if Ch='(' then
            begin
              NextCh;
              F:=Expression;
              if Ch=')' then NextCh else ErrPosf:=Posf;
            end else
            begin
              found:=false;
              for sf:=fabs to fhb do
              if not found then
              begin
                l:=Length(StandardFunctionNames[sf]);
                if copy(Formula,Posf,l)=StandardFunctionNames[sf] then
                begin
                  Posf:=Posf+l-1; NextCh;
                  F:=Factor;
//                  writeln(StandardFunctionNames[sf]);
                  case sf of
                    fabs:     f:=abs(f);
                    fsqrt:    f:=sqrt(f);
                    fsqr:     f:=sqr(f);
                    fsin:     f:=sin(f);
                    fcos:     f:=cos(f);
                    farctan:  f:=arctan(f);
                    fln :     f:=ln(f);
                    flog:     f:=ln(f)/ln(10);
                    fexp:     f:=exp(f);
                    ffact:    f:=fakult(trunc(f));
                    fPi:      f:=Pi;
                    fE:       f:=Exp(1);
//                    fX:       f:=X;
                    fnot:     f:=notbin(trunc(f));
                    flb:      f:=trunc(f) and $FF;
                    fhb:      f:=(trunc(f) shr 8) and $FF;
                  end;
                  Found:=true;
                end;
              end;
              if not Found then ErrPosf:=Posf;
            end;
            result:=F;
          end { function Factor};

        begin { SignedFactor }
         if Ch='-' then
          begin
            NextCh; result:=-Factor;
          end
          else if Ch='~' then
          begin
            NextCh; result:=notbin(trunc(Factor));
          end else result:=Factor;
        end { SignedFactor };

      begin { Term }
        T:=SignedFactor;
        while Ch='^' do
        begin
          NextCh; t:=exp(ln(t)*SignedFactor);
        end;
        result:=t;
      end { Term };

    begin { SimpleExpression }
      s:=term;
      while Ch in ['*','/','<','>','%'] do
      begin
        Opr:=Ch;  NextCh;
        case Opr of
          '*': s:=s*term;
          '/': s:=s/term;
          '<': s:=trunc(s) shl trunc(term);
          '>': s:=trunc(s) shr trunc(term);
          '%': s:=trunc(s) mod trunc(term);
        end;
      end;
      result:=s;
    end { SimpleExpression };

  begin { Expression }
    E:=SimpleExpression;
    while Ch in ['+','-','|','&'] do
    begin
      Opr:=Ch; NextCh;
      case Opr of
        '+': e:=e+SimpleExpression;
        '-': e:=e-SimpleExpression;
        '|': e:=trunc(e) or trunc(SimpleExpression);
        '&': e:=trunc(e) and trunc(SimpleExpression);
      end;
    end;
    result:=E;
  end { Expression };





begin
end.



#endif
