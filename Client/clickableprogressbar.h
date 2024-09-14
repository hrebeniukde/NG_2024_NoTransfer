#ifndef CLICKABLEPROGRESSBAR_H
#define CLICKABLEPROGRESSBAR_H

#include <QProgressBar>
#include <QMouseEvent>

class ClickableProgressBar : public QProgressBar {
    Q_OBJECT
public:
    explicit ClickableProgressBar(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CLICKABLEPROGRESSBAR_H
