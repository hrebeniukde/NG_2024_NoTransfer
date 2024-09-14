#include "ClickableProgressBar.h"
#include <QDebug>

ClickableProgressBar::ClickableProgressBar(QWidget *parent)
    : QProgressBar(parent) {}

void ClickableProgressBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QProgressBar::mousePressEvent(event);
}
