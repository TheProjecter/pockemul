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

CTinyBasic::CTinyBasic(QObject *parent)
{
    CommandMap["LIST"] = CKeyword("LIST",KW_LIST);
    CommandMap["PRINT"] = CKeyword("PRINT",KW_PRINT);
    CommandMap["CLS"] = CKeyword("CLS",KW_CLS);
    CommandMap["RUN"] = CKeyword("RUN",KW_RUN);
    CommandMap["PI"] = CKeyword("RUN",KW_PI);

}



void CTinyBasic::Interpret(QByteArray ops,int pos)
{

//        switch (ops[i]) {
//        case MEM:
//            // return MEM
//            break;
//        case LIST: // LIST
//            break;

//        }
        // get next token


}

void CTinyBasic::Command(QByteArray command)
{
    commandBuffer = command;
    Parse();
}

#define TOKENCHAR(c)        QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789").contains(c)
#define TOKENSTARTCHAR(c)   QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ").contains(c)
#define STRINGCHAR(c)       QString("\"'").contains(c)
#define NUMERIC(c)          QString("0123456789").contains(c)

void CTinyBasic::Parse()
{
    commandBuffer.append(' ');
    int startToken = 0;
    int lenght=0;
    bool foundToken = false;
    for (int i = 0 ; i<commandBuffer.size();i++) {
        char c = commandBuffer.at(i);
        qWarning()<< c;
        if (STRINGCHAR(c)) {
            i++;
            while ( (i<commandBuffer.size()) && !STRINGCHAR(commandBuffer.at(i))) i++;
            c = commandBuffer.at(i);
        }

        if (c==' ') { commandBuffer.remove(i,1);
            c = commandBuffer.at(i);
        }
        if ( !foundToken && TOKENSTARTCHAR(c)) {
            foundToken = true;
            startToken = i;
        }
        if (foundToken && !TOKENCHAR(c) ) {
            lenght = i - startToken;

            QByteArray currentToken = commandBuffer.mid(startToken,lenght);
            qWarning() << "currentToken:" << currentToken;

            for (int k = currentToken.size();k>0;k--) {
                qWarning()<< "search:"<<currentToken.mid(0,k);
                if (CommandMap.contains(currentToken.mid(0,k))) {
                    qWarning()<<"found";
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
    qWarning()<< commandBuffer;



}


CKeyword::CKeyword(QString name, quint8 code)
{
    Name = name;
    Code = code;
}
