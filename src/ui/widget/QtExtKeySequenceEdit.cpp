#include "include/ui/widget/QtExtKeySequenceEdit.h"

#include <QKeyEvent>

QtExtKeySequenceEdit::QtExtKeySequenceEdit(QWidget *parent)
    : QKeySequenceEdit(parent) {
}

QtExtKeySequenceEdit::~QtExtKeySequenceEdit() {
}

void QtExtKeySequenceEdit::keyPressEvent(QKeyEvent *pEvent) {
    QKeySequenceEdit::keyPressEvent(pEvent);

    QKeySequence keySeq = keySequence();
    if (keySeq.count() <= 0) {
        return;
    }

    // allow single Delete key or combination of modifiers + Delete / Backspace key to be set, use single Backspace key to unset.
    auto key = keySeq[0].key();
    if (key == Qt::Key_Backspace && pEvent->modifiers() == Qt::NoModifier) {
        key = static_cast<Qt::Key>(0);
        setKeySequence(key);
    }
}
