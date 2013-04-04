#include "gifwidget.h"
#include <QProgressDialog>
#include <QFileDialog>

GifWidget::GifWidget(
     QWidget* parent, 
     Qt::WindowFlags f): QWidget(parent,f), timerId(-1), currentFrame(-1)
{
     setupUi(this);
     createActions();
}

GifWidget::~GifWidget()
{

}

void GifWidget::addFrame(const QImage& f)
{
     QImage i(f);
     i = i.mirrored().convertToFormat(QImage::Format_RGB888);
     gif.resize(i.width(),i.height());
     gif.prepareFrame(&i, palette);
     prevFrames.append(i.mirrored());
     currentFrame = 0;
}

void GifWidget::createActions()
{
     QAction* a = new QAction(tr("Save gif..."), this);
     a->setIcon(QIcon(":/res/floppy.png"));
     saveButton->setDefaultAction(a);
     connect(a, SIGNAL(triggered()), this, SLOT(save()));

     a = new QAction(tr("Play"), this);
     a->setIcon(QIcon(":/res/start.png"));
     playButton->setDefaultAction(a);
     connect(a, SIGNAL(triggered()), this, SLOT(play()));
     
     a = new QAction(tr("Pause"), this);
     a->setIcon(QIcon(":/res/pause.png"));
     pauseButton->setDefaultAction(a);
     connect(a, SIGNAL(triggered()), this, SLOT(pause()));

}

void GifWidget::play()
{
     if(timerId == -1)
     {
	  timerId = startTimer(intervalBox->value());
	  skipped = 0;
	  currentFrame = 0;
     }
     else
     {
	  pause();
	  play();
     }
     intervalBox->setEnabled(false);
}

void GifWidget::pause()
{
     killTimer(timerId);
     timerId = -1;
     intervalBox->setEnabled(true);
}

void GifWidget::save()
{
     qDebug() << "saving gif...";

     QString filename = QFileDialog::getSaveFileName(
	  this, tr("Save GIF file"), qApp->applicationDirPath(),
	  "GIF files (*.gif);;All files (*.*)");

     if(filename.isEmpty())
	  return;

     if(!prevFrames.size())
	  return;
     pause();
     gif.setDuration((double)intervalBox->value()/1000);
     gif.save(filename.toStdString().c_str(),
	  saveEveryBox->isChecked() ? seBox->value() : 1);
}

void GifWidget::timerEvent(QTimerEvent*)
{
     preview->setImage(prevFrames.at(currentFrame));
     currentFrame += saveEveryBox->isChecked() ? seBox->value() : 1;

     if(currentFrame >= prevFrames.size())
	  currentFrame = skipped = 0;
}
