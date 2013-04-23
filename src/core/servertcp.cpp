

#include "common.h"
#include "servertcp.h"
#include "mainwindowpockemul.h"
#include "pobject.h"

extern MainWindowPockemul *mainwindow;
extern QList<CPObject *> listpPObject;

ServeurTcp :: ServeurTcp (QObject *parent)
{
    listen(QHostAddress::LocalHost,4000);
    QObject:: connect(this, SIGNAL(newConnection()),
    this, SLOT(demande_connexion()));
}

// si un client demande une connexion
void ServeurTcp :: demande_connexion()
 {
    emit vers_IHM_connexion(); // on envoie un signal à l'IHM
    // on crée une nouvelle socket pour ce client
    clientConnection = nextPendingConnection();
    // si on reçoit des données, le slot lecture() est appelé
    QObject:: connect(clientConnection, SIGNAL(readyRead()),
    this, SLOT(lecture()));
    clientConnection->setTextModeEnabled(true);
    send_greeting(new QTextStream(clientConnection));
}
void ServeurTcp ::lecture()
{
    QString ligne;
    while(clientConnection->canReadLine())    // tant qu'on peut lire sur la socket
    {
        ligne = clientConnection->readLine(); // on lit une ligne
        ligne.remove('\n');
        qWarning()<<ligne;
        emit vers_IHM_texte(ligne);           // on l'envoie à l'IHM
        process_command(ligne);
    }

}

void ServeurTcp::send_greeting(QTextStream *sock)
{
    *sock << endl << "Welcome to the PockEmul v1.0 TELNET interface." << endl;
    *sock << "Use decimal, C hex (0x2a) or Asm hex (14h) for input." << endl;
    *sock << "Type 'help' for a list of supported commands." << endl;

}

void ServeurTcp::cmd_help(QTextStream *sock,QString subcmd)
{
    if (subcmd=="") {
    *sock << "Help" << endl << "====" << endl;
    *sock << "  bye" << endl;
    *sock << "  list        List all running model" << endl;
    *sock << "  start model_name" << endl;
    *sock << "  key(k) [enter cr f1 esc ctrl+c shift+code+a \"Text\" ...]" << endl;
}
    else if(subcmd=="start") {
        *sock << "Help start" << endl << "====" << endl;
        *sock << "  start ModelName" << endl;
        QHashIterator<QString, Models> i(mainwindow->objtable);
        while (i.hasNext()) {
            i.next();
            *sock << i.key() << " , " ;
        }
        *sock << endl;
    }
}
void ServeurTcp::process_command(QString ligne)
{

    // Separate out the command word from any arguments
    QStringList args =  splitArgs(ligne);//ligne.split(' ',QString::SkipEmptyParts);
    QString cmd = args.first();


    QTextStream texte(clientConnection);      // création d'un flux pour écrire dans la socket
//    texte << \"message reçu\n" << endl;          // message à envoyer au client

    qWarning()<<"cmd:'"<<cmd<<"'";
    if (cmd == "help")				// Check for help
        cmd_help(&texte,(args.size()>1)?args.at(1):"");
    else if (cmd=="start") {
        qWarning()<<args[1];
        mainwindow->LoadPocket((args.size()>1)?args.at(1):"");
    }
    else if (cmd == "list") {
        for (int i=0;i<listpPObject.size();i++)
        {
            CPObject *po = listpPObject.at(i);
            texte << i << " : "<< po->getName()<<endl;
        }
    }
    else if (cmd=="select") {
        if (args.size()<2) { // Erreur
        }
        else if (args.at(1).toInt() < listpPObject.size()) {
            CPObject *pc = listpPObject.at(args.at(1).toInt());
            pc->raise();
            pc->setFocus();
        }
    }
}

QStringList ServeurTcp::splitArgs(QString cmd) {
    bool inside = (cmd.at(0) == '\"'); //true if the first character is "
    QStringList tmpList = cmd.split(QRegExp("\""), QString::SkipEmptyParts); // Split by " and make sure you don't have an empty string at the beginning
    QStringList csvlist;
    foreach (QString s, tmpList) {
        if (inside) { // If 's' is inside quotes ...
            csvlist.append(s); // ... get the whole string
        } else { // If 's' is outside quotes ...
            csvlist.append(s.split(" ", QString::SkipEmptyParts)); // ... get the splitted string
        }
        inside = !inside;
    }

    return csvlist;
}
