#include "include/ui/droptextedit.h"

void DropTextEdit::dropEvent(QDropEvent* event){
    const QMimeData* mimeData = event->mimeData();
    // check for our needed mime type, here a file or a list of files
    if (mimeData->hasUrls())
    {
      QList<QUrl> urlList = mimeData->urls();
      if(urlList.size() > 0){
          QFile file(urlList.at(0).toLocalFile());
          file.open(QIODevice::ReadOnly);
          QString s;
          QTextStream s1(&file);
          s.append(s1.readAll());
          file.close();
          this->setText(s);
      }
    }
    return;
}

DropTextEdit::DropTextEdit(QWidget *parent):QTextEdit(parent){

}

DropTextEdit::DropTextEdit(){

}
