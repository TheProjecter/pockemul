#include "QString"
#include <QMessageBox>

#include "lcpp.h"

#define ENABLE_COMPILATION 1

#if ENABLE_COMPILATION


void Clcpp::abort(QString t) {
    QMessageBox::about((QWidget*)mainwindow,"ERROR","Line " + QString("%1").arg(cline+1) + ": " + t + " in file " + inpf);
}


QString Clcpp::replace_text(QString text, QString such, QString ers) {

    QString regex = "([^_0-9A-Za-z])("+such+")([^_0-9A-Za-z])";
    return text.replace(QRegExp(regex),"\\1"+ers+"\\3");
}


bool Clcpp::findsymbol(QString l) {
#if 1
    return sym.contains(l);
#else
    for (int i = 0 ; i< sym.size();i++) {
        if (sym[i] == l) return true;
    }

    return false;
#endif
}


void Clcpp::addsymbol(QString s1, QString s2) {
    if (findsymbol(s1)) {
        abort("Symbol " + s1 + " already defined!");
    }
    sym.append(s1);
    symval.append(s2);
        writeln("output","SYMBOL: " + s1 + " - " + s2);
        symcnt++;
    }



QString Clcpp::extractparam(QString s,int p) {

#if 1
    QString ss = s.simplified();
    QStringList sl = ss.split(" ");
    if (p<sl.size()) return sl.at(p);
    return "";

#else
    int i, c;
    QString result;
    i = 0;
    for (c = 1; c <= p; c++) {
        s = s.trimmed();
        if (s.indexOf(' ') > 0) i = s.indexOf(' ');
        if ((s.indexOf(0x09) > 0) && (s.indexOf(0x09) < i)) i = s.indexOf(0x09);
        s.remove(0,i);
        if (s.indexOf(' ') > 0) i = s.indexOf(' ');
        if ((s.indexOf(0x09) > 0) && (s.indexOf(0x09) < i)) i = s.indexOf(0x09);
        if (i != 0) result = s.mid(0,i-1).trimmed();
        else result = s.trimmed();
    }
    return result;
#endif
}


QString Clcpp::readline(QStringListIterator *linesIter) {

    char c;
    QString result;

    do {
        do {
            if (linesIter->hasNext())
                result = linesIter->next();
            lcnt++;
            cline = lcnt;
            result = result.trimmed();
            if (lcom) {
                if (result.indexOf("*/")==-1) result = "";
                else {
                    result.remove(0,result.indexOf("*/")+2);
                    lcom = false;
                }
            }
        }
        while (linesIter->hasNext() && result.isEmpty());
        c = ' ';
        for (int i = 0 ;i <result.length();i++) {
            if (QString("'\"").contains(result[i])) {
                if (c == ' ') c = result[i].toAscii();
                else c = ' ';
            }
            if ((c == ' ') && result.mid(i).startsWith("//")) {
                result = result.left(i);
                break;
            }
            if ((c == ' ') && result.mid(i).startsWith("/*")) {
                if (result.indexOf("*/") > i)
                    result.remove(i, result.indexOf("*/") - i + 2);
                else {
                    result=result.left(i);
                    lcom = true;
                }
                break;
            }
        }
        if (!result.isEmpty()) {
            if (!result.toLower().startsWith("#ifdef")) {
                if (sym.size() > 0) {
                    for (int i = 0 ; i< sym.size();i++) {
                        result = replace_text(result, sym[i], symval[i]);
                    }
                }
            }
        }
    }
    //until eof(datei) or (result <> '');
    while (linesIter->hasNext() && result.isEmpty());
    result = result.trimmed();

    return result;
}

QString Clcpp::parsefile(QString srcName,QString source) {
//var datei: textfile;
//    lcnt: integer;
//    op, tok: string;
//    lcom: boolean
    QString op,tok;



    lcnt=0;
    QStringList lines = source.split("\n");
    QStringListIterator linesIter(lines);

    tok = readline(&linesIter);
    while ((linesIter.hasNext()) || !tok.isEmpty()) {
        //            if pos('#endif', tok) = 0 then
        {

            if (tok.startsWith("#define")) addsymbol(extractparam(tok, 1), extractparam(tok, 2));
            else if (tok.startsWith("#org") ||
                     tok.startsWith("#asm") ||
                     tok.startsWith("#endasm") ||
                     tok.startsWith("#save") ||
                     tok.startsWith("#restore") ||
                     tok.startsWith("#nosave")) {
                writeln( srcName,tok + ";");
            }
            else if (tok.startsWith("#ifdef")) {
                if (! findsymbol(extractparam(tok, 1))) {
                    while (linesIter.hasNext() && (op !="#endif"))
                    {
                        tok = readline(&linesIter);
                        if (tok.indexOf("#endif") > 0) op = "#endif";
                    }
                }
            }
            else if (tok.startsWith("#ifndef")) {
                if ( findsymbol(extractparam(tok, 1))) {
                    while (linesIter.hasNext() && (op !="#endif"))
                    {
                        tok = readline(&linesIter);
                        if (tok.indexOf("#endif") > 0) op = "#endif";
                    }
                }
            }
            else if (tok.startsWith("#include")) {
                op = extractparam(tok, 1);
                // Two case : "filename", look in standard sources set
                //             <filename>, look in library
                if (op.startsWith('"')) {
                    op.remove('"');
                    if (sources->contains(op)) {
                            parsefile(srcName,sources->value(op));
                        }
                    else
                        abort("Include file " + op + " not found!");
                }
                if (op.startsWith('<')) {
                    op.remove('<').remove('>');

                    if (pStdLibs->libmap.contains(op)) {
                        parsefile(srcName,pStdLibs->libmap.value(op));
                    }
                }
            }
            else if (tok.indexOf("#endif")<0) writeln(srcName,tok);
        }
        tok = readline(&linesIter);
    }

    return "ok";
}


void Clcpp::run() {

    // Fetch all sources and precomp
    QMapIterator<QString, QByteArray> i(*sources);
    while (i.hasNext()) {
        i.next();
        inpf = i.key();
        parsefile(inpf,i.value());
    }

}

void Clcpp::writeln(QString srcName,QString s) {
    QByteArray locs = out->value(srcName);
    out->insert(srcName,locs+"\r"+s.toAscii());
}

Clcpp::Clcpp(QMap<QString,QByteArray> *sources,QMap<QString,QByteArray> *out,QString model) {
    this->sources = sources;
    this->out = out;
    this->model = model;
    symcnt = 0;
    pStdLibs = new Cstdlib();
    pStdLibs->setModel(model);
    lcom=false;
}

#endif
