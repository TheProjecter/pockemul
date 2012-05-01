#include "common.h"
#include "pcxxxx.h"
#include "cextension.h"
#include "mainwindowpockemul.h"

extern MainWindowPockemul* mainwindow;
#define MAXEXT  40
bool CpcXXXX::CheckUpdateExtension(CExtension *ext)
{
	return true;
}

void CpcXXXX::updateExtension(CExtensionArray *array,QAction *action)
{
        for (int ind = 0 ; ind < MAXEXT; ind++){
			if (array->ExtArray[ind]->Action == action)
			{
				if (CheckUpdateExtension(array->ExtArray[ind]))
					array->setChecked(ind,true);
			}
		}
}

void CpcXXXX::updateExtension(QAction *action)
{
	for (int ind = 0; ind < 5; ind++){
		if (extensionArray[ind]) updateExtension(extensionArray[ind],action);
	}
    TurnOFF();
	InitDisplay();

}

void CpcXXXX::updateMenuFromExtension(void)
{
	QAction *action;
	bool checked;

	for (int indArray = 0; indArray < 5; indArray++){
		if (extensionArray[indArray]) 
        for (int indExt = 0; indExt < MAXEXT ; indExt++) {
			checked = extensionArray[indArray]->ExtArray[indExt]->IsChecked;
			action = extensionArray[indArray]->ExtArray[indExt]->Action;
			if (action) action->setChecked(false);
			if (action && checked &&  !action->isChecked()) action->setChecked(true);
		}
	}
}


CExtension * CpcXXXX::findExtension(CExtensionArray *array,QAction *action)
{
    for (int ind = 0 ; ind < MAXEXT; ind++){
		if (array->ExtArray[ind]->Action == action)
		{
			return(array->ExtArray[ind]);
		}
	}
	return(0);
}


void CpcXXXX::addExtMenu(CExtensionArray *ext)
{
    if (! ext) {
//        MSG_ERROR("ERREUR");
        return;
    }
	
	delete ext->Menu;
	
	ext->Menu = new QMenu(ext->Id);

	ext->emptyAction = ext->Menu->addAction("Empty");
	connect(ext->emptyAction, SIGNAL(triggered()), this, SLOT(manageEmptyExtensions()));
	
	ext->Menu->addSeparator();
	ext->actionGroup = new QActionGroup(this);
	connect(ext->actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(manageExtensions(QAction *)));
    for (int ind = 0 ; ind < MAXEXT; ind++){
		if (ext->ExtArray[ind]->IsAvailable){
			ext->ExtArray[ind]->Action = ext->Menu->addAction(ext->ExtArray[ind]->Id + " ("+ext->ExtArray[ind]->Description+")");
			ext->ExtArray[ind]->Action->setToolTip(ext->ExtArray[ind]->Description);
			ext->ExtArray[ind]->Action->setIcon(QIcon(ext->ExtArray[ind]->fname));
			ext->ExtArray[ind]->Action->setCheckable(true);
			ext->ExtArray[ind]->Action->setChecked(ext->ExtArray[ind]->IsChecked);
			ext->actionGroup->addAction(ext->ExtArray[ind]->Action);
		}
	}
    // check if power saved extensions exist
    ext->Menu->addSeparator();
    ext->loadAction = ext->Menu->addAction("Load...");
    ext->saveAction = ext->Menu->addAction("Save...");
    connect(ext->loadAction, SIGNAL(triggered()), this, SLOT(manageLoadExtensions()));
    connect(ext->saveAction, SIGNAL(triggered()), this, SLOT(manageSaveExtensions()));


}

void CpcXXXX::emptyExtensionArray(QAction *action)
{
	for (int ind = 0; ind < 5; ind++){
		if (extensionArray[ind])
		{
			if (extensionArray[ind]->emptyAction == action)
			{
				// is this array empty or not ?
//				bool isempty=true;
                for (int indExt = 0;indExt<MAXEXT; indExt++)
				{
					if (extensionArray[ind]->ExtArray[indExt]->IsChecked)
						{
							extensionArray[ind]->ExtArray[indExt]->IsChecked = false;
							TurnOFF();
							InitDisplay();
						} 
				}
			}
		}
	}

}
void CpcXXXX::loadExtensionArray(QAction *action)
{

}
void CpcXXXX::saveExtensionArray(QAction *action)
{

}
