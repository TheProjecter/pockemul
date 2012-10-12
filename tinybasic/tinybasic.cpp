#include <QDebug>

#include "tinybasic.h"

enum {
    MEM,
    LIST
};
#define KW_LIST     0x01
#define KW_PRINT    0x02
#define KW_CLS      0x03
#define KW_RUN      0x04
#define KW_PI       0x05
#define KW_FOR      0x06
#define KW_TO       0x07

#define TOKENCHAR(c)        QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789").contains(c)
#define TOKENSTARTCHAR(c)   QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ").contains(c)
#define STRINGCHAR(c)       QString("\"'").contains(c)
#define NUMERIC(c)          QString("0123456789").contains(c)

CTinyBasic::CTinyBasic(QObject *parent)
{
    CommandMap["LIST"] = CKeyword("LIST",KW_LIST);
    CommandMap["PRINT"] = CKeyword("PRINT",KW_PRINT);
    CommandMap["CLS"] = CKeyword("CLS",KW_CLS);
    CommandMap["RUN"] = CKeyword("RUN",KW_RUN);
    CommandMap["PI"] = CKeyword("RUN",KW_PI);
    CommandMap["FOR"] = CKeyword("FOR",KW_FOR);
    CommandMap["TO"] = CKeyword("TO",KW_TO);

    mode = COMMAND;
}

void CTinyBasic::test() {
    inputCommand("LIST");
    Interpret(commandBuffer);
    inputCommand("100APRI NTB C  (13*4):\" CLS\":LIST");
    Interpret(commandBuffer);
    inputCommand("   50   FOR I = 1 TO 4");
    Interpret(commandBuffer);
    inputCommand("   10CLS");
    Interpret(commandBuffer);
    inputCommand("LIST");
    Interpret(commandBuffer);
    inputCommand("RUN");
    Interpret(commandBuffer);

}

void CTinyBasic::saveBasicLine() {
    // Search line number
    int pos= 0;
    while (NUMERIC(commandBuffer.at(pos))) pos++;
    int lineNumber = commandBuffer.left(pos).toInt();
    basicLines.insert(lineNumber,commandBuffer.mid(pos));

}

void CTinyBasic::executeCommand(QByteArray code) {
    switch (code.at(0)) {
    case KW_LIST :  go_LIST(code.mid(1)); break;
    case KW_RUN:    go_RUN(code.mid(1)); break;
    }
}

void CTinyBasic::go_RUN(QByteArray code) {
    qWarning()<< "RUN BASIC PROGRAMME";
    // Check parameters

    mode = RUN;
    QMapIterator<int,QByteArray> i(basicLines);
     while (i.hasNext()) {
         i.next();
         Interpret( i.value());
     }
}

void CTinyBasic::go_LIST(QByteArray code) {

    // Check parameters

    qWarning() << "LIST File:";
    QMapIterator<int,QByteArray> i(basicLines);
     while (i.hasNext()) {
         i.next();
         qWarning() << i.key() << ": " << i.value();
     }
}

void CTinyBasic::Interpret(QByteArray ops,int pos)
{

    if (mode == RUN) {
        qWarning()<<"Interpret : "<< ops.mid(pos);
        executeCommand(ops.mid(pos));
    }
    if (mode == COMMAND) {
        // Check if buffer start by number
        if (NUMERIC(ops.at(pos))) {
            // record BASIC line

            qWarning()<< "Record BASIC LINE:"<<ops.mid(pos);
            saveBasicLine();
        }
        else
        {
            executeCommand(ops.mid(pos));
        }
    }


}

void CTinyBasic::inputCommand(QByteArray command)
{
    qWarning()<< command;
    commandBuffer = command;
    Parse();
}



void CTinyBasic::Parse()
{
    commandBuffer.append('\r');
    int startToken = 0;
    int lenght=0;
    bool foundToken = false;
    for (int i = 0 ; i<commandBuffer.size();i++) {
        char c = commandBuffer.at(i);
//        qWarning()<< c;

        if (STRINGCHAR(c)) {        // STRING found continue until end of string
            i++;
            while ( (i<commandBuffer.size()) && !STRINGCHAR(commandBuffer.at(i))) i++;
            c = commandBuffer.at(i);
        }

        // Remove all spaces
        if (c==' ') {
            commandBuffer.remove(i,1);
//            qWarning()<<"remove space:"<<commandBuffer;
            i--;
            continue;
        }

        if ( !foundToken && TOKENSTARTCHAR(c)) {
//            qWarning()<<"Start Token";
            foundToken = true;
            startToken = i;
        }

        if (foundToken && !TOKENCHAR(c) ) {
//            qWarning()<<"End Token";

            lenght = i - startToken;

            QByteArray currentToken = commandBuffer.mid(startToken,lenght);
//            qWarning() << "currentToken:" << currentToken;

            for (int k = currentToken.size();k>0;k--) {
//                qWarning()<< "search:"<<currentToken.mid(0,k);
                if (CommandMap.contains(currentToken.mid(0,k))) {
//                    qWarning()<<"found";
                    // Replace by code in the buffer
                    commandBuffer.replace(startToken,k,QByteArray(1,CommandMap[currentToken.mid(0,k)].Code));
//                    startToken++;
                    i=startToken-1;
                    foundToken = false;

                    break;
                }
            }
            foundToken = false;
            i = startToken;


        }

    }
//    qWarning()<< commandBuffer;



}


CKeyword::CKeyword(QString name, quint8 code)
{
    Name = name;
    Code = code;
}
