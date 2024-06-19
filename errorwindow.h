#ifndef ERRORWINDOW_H
#define ERRORWINDOW_H

#include <QWidget>

namespace Ui {
class ErrorWindow;
}

class ErrorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorWindow(const QString& ErrorString_, QWidget *parent = nullptr);
    ~ErrorWindow();

signals:
    void closed();

private:
    Ui::ErrorWindow *ui;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // ERRORWINDOW_H
