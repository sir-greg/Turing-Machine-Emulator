#include "errorwindow.h"
#include "ui_errorwindow.h"

ErrorWindow::ErrorWindow(const QString& ErrorString_, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ErrorWindow)
{
    ui->setupUi(this);
    ui->ErrorLabel->setText(ErrorString_);
}

ErrorWindow::~ErrorWindow()
{
    delete ui;
}

void ErrorWindow::closeEvent(QCloseEvent *event)
{
    emit closed();
    QWidget::closeEvent(event);
}
