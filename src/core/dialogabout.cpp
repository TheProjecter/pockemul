#include "dialogabout.h"

DialogAbout::DialogAbout( QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);
	
	textBrowser_2->setSource(QUrl("qrc:/POCKEMUL/pockemul/release_notes.html"));
}

