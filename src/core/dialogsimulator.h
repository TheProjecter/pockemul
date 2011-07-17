#ifndef DIALOGSIMULATOR_H
#define DIALOGSIMULATOR_H

#include "ui_simulatorconsole.h"


class DialogSimulator : public QDialog, public Ui::DialogSimulator
{
Q_OBJECT
public:
    DialogSimulator( QWidget * parent = 0, Qt::WFlags f = 0 );

    void refresh( void);

private slots:


protected:
    void closeEvent(QCloseEvent *event);
    int currentIndex;
};

#endif // DIALOGSIMULATOR_H
