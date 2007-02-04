#include <QtGui>
#include "paperwidget.h"
#include "Log.h"

void CpaperWidget::setOffset(QPoint val)
{
	Offset = val;
}

QPoint CpaperWidget::getOffset()
{
	return Offset;
}

void CpaperWidget::contextMenuEvent ( QContextMenuEvent * event )
{
	QMenu menu(this);
	menu.addAction(tr("Copy"),this,SLOT(paperCopy()));
	menu.addAction(tr("Cut"),this,SLOT(paperCut()));
	menu.addAction(tr("Save Image ..."),this,SLOT(paperSaveImage()));
	menu.addAction(tr("Save Text ..."),this,SLOT(paperSaveText()));
	menu.exec(event->globalPos ());
}

void CpaperWidget::paperCopy()
{
	
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setImage(*bufferImage);
}

void CpaperWidget::paperCut()
{
	paperCopy();
	// Erase paper
	// Initialise position
	((Cprinter*)pPC)->clearPaper ();
	
	update();
}
void CpaperWidget::paperSaveImage()
{

	QString s = QFileDialog::getSaveFileName(
                    this,
                    tr("Choose a filename to save under"),
                    ".",
                    "Images (*.png)");
                    
	bufferImage->save(s,"PNG",100);
	
}
void CpaperWidget::paperSaveText()
{
	// Call the printer SaveAsText Virtual function
	((Cprinter*)pPC)->SaveAsText();
	
}

void CpaperWidget::paintEvent(QPaintEvent *event)
{
	float ratio = ( (float) width() ) / ( bufferImage->width() - Offset.x() );
	
	QRect source = QRect( QPoint(Offset.x() , Offset.y()  - height() / ratio ) , QPoint( bufferImage->width() , Offset.y() ) );
	
	QPainter painter;
	
	painter.begin(this);
	
	painter.drawImage(rect(),bufferImage->copy(source).scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation ));

	painter.end();
}
