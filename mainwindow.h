#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBuffer>
#include <QImageWriter>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <QtDebug>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void fillList();
    QDir directory;
    QImage previewimage;
    QImage image;
    QByteArray bytes;
    void messageBox(QString str);

private slots:
    void on_listWidget_files_clicked(const QModelIndex &index);
    void on_pushButton_Browse_clicked();
    void on_pushButton_Start_clicked();
    void on_checkBox_aspectRatio_clicked();
    void on_slider_Quality_valueChanged();
    void on_slider_Quality_sliderMoved();
    void on_radioButton_JPG_clicked();
    void on_radioButton_PNG_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
