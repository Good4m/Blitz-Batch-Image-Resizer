#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());
    QDate today;
    ui->lineEdit_WatermarkText->setText(today.currentDate().toString());
}


void MainWindow::fillList()
{
    ui->listWidget_files->clear();
    for (int i=0; i<directory.entryList().count(); i++) {
        if (directory.entryList().at(i) != "."
            && directory.entryList().at(i) != ".."
            && directory.entryList().at(i).contains(".")
            && (directory.entryList().at(i).contains(".jpg")
                || directory.entryList().at(i).contains(".png")
                || directory.entryList().at(i).contains(".bmp"))
           ) {
           ui->listWidget_files->addItem(directory.entryList().at(i));
        }
    }
}


void MainWindow::on_listWidget_files_clicked(const QModelIndex &index)
{
    // Get all listview selections
    QList<QListWidgetItem *> currentSelections;
    currentSelections = ui->listWidget_files->selectedItems();

    // Load preview image
    QString currentitem = directory.path() + "/" + ui->listWidget_files->currentItem()->text();
    if (previewimage.load(currentitem)) {
        previewimage = previewimage.scaled(QSize(ui->lineEdit_Width->text().toInt(),ui->lineEdit_Height->text().toInt()));

        if (ui->checkBox_watermark->isChecked()) {
            QPainter p(&previewimage);
            p.setFont(QFont("Times", 10, QFont::Bold));
            if (ui->checkBox_watermarkshadow->isChecked()) {
                p.setPen(QPen(Qt::black));
                p.drawText(previewimage.width() - (ui->lineEdit_WatermarkText->text().length() * 6.4), previewimage.height() - 4, this->ui->lineEdit_WatermarkText->text());
            }
            p.setPen(QPen(Qt::white));
            p.drawText(previewimage.width() - (ui->lineEdit_WatermarkText->text().length() * 6.5), previewimage.height() - 5, this->ui->lineEdit_WatermarkText->text());
        }

        // Set image to preview pane
        ui->label_previewimage->setPixmap(QPixmap::fromImage(previewimage));
    } else {
        messageBox("Error loading image..");
    }
}


void MainWindow::on_pushButton_Browse_clicked()
{
    QString path = QFileDialog::getExistingDirectory (this, tr("Directory"), directory.path());
    if (!path.isNull()) {
        directory.setPath(path);
        fillList();
    }
}


void MainWindow::on_pushButton_Start_clicked()
{
    // Check if directory exists, if it doesn't... create it
    QDir dirr;
    if (!dirr.exists(directory.path() + "/" + ui->lineEdit_NewFolderName->text())
        && !dirr.mkdir(directory.path() + "/" + ui->lineEdit_NewFolderName->text())) {
         messageBox("Error creating output directory.");
    } else {

        // Get all listview selections
        QList<QListWidgetItem *> currentSelections;
        currentSelections = ui->listWidget_files->selectedItems();

        if (currentSelections.count() == 0) {
            messageBox("Please select the images you want to resize.");
            return;
        }

        foreach (QListWidgetItem* item, currentSelections) {
            // Load image
            if (image.load(directory.path() + "/" + item->text())) {
                if (ui->checkBox_aspectRatio->isChecked()) {
                    image = image.scaled(QSize(ui->lineEdit_Width->text().toInt(),ui->lineEdit_Height->text().toInt()), Qt::KeepAspectRatio);
                } else {
                    image = image.scaled(QSize(ui->lineEdit_Width->text().toInt(),ui->lineEdit_Height->text().toInt()), Qt::IgnoreAspectRatio);
                }
                // Watermark
                if (ui->checkBox_watermark->isChecked()) {
                    QPainter p(&image);
                    p.setFont(QFont("Times", 10, QFont::Bold));
                    if (ui->checkBox_watermarkshadow->isChecked()) {
                        p.setPen(QPen(Qt::black));
                        p.drawText(image.width() - (ui->lineEdit_WatermarkText->text().length() * 6.4), image.height() - 4, this->ui->lineEdit_WatermarkText->text());
                    }
                    p.setPen(QPen(Qt::white));
                    p.drawText(image.width() - (ui->lineEdit_WatermarkText->text().length() * 6.5), image.height() - 5, this->ui->lineEdit_WatermarkText->text());
                }

                // Save image
                QStringList strippedFilename = item->text().split(".");
                QString filename = directory.path() + "/" + ui->lineEdit_NewFolderName->text() + "/" + strippedFilename[0];

                QBuffer buffer(&bytes);
                buffer.open(QIODevice::WriteOnly);

                if (ui->radioButton_JPG->isChecked()) {
                    filename.append(".jpg");
                    QImageWriter imageWriter(filename, "jpg");
                    imageWriter.setQuality(ui->slider_Quality->value());
                    imageWriter.write(image);
                }

                if (ui->radioButton_PNG->isChecked()) {
                    filename.append(".png");
                    image.save(&buffer, "PNG");
                    QFile file(filename);
                    if(file.open(QIODevice::WriteOnly)) {
                        file.write(bytes);
                        file.close();
                    } else {
                        QString errormsg = "  Error saving file: ";
                        errormsg.append(filename);
                    }
                }


            }

        }
        ui->listWidget_files->clearSelection();
        messageBox("Complete");
    }
}


void MainWindow::on_checkBox_aspectRatio_clicked()
{
    if (ui->lineEdit_Height->isEnabled()) {
        ui->lineEdit_Height->setDisabled(true);
    } else {
        ui->lineEdit_Height->setEnabled(true);
    }
}


void MainWindow::on_slider_Quality_valueChanged()
{
    ui->label_percent->setText(QString::number(ui->slider_Quality->value()) + "%");
}


void MainWindow::on_slider_Quality_sliderMoved()
{
    ui->label_percent->setText(QString::number(ui->slider_Quality->value()) + "%");
}


void MainWindow::on_radioButton_JPG_clicked()
{
    ui->slider_Quality->setEnabled(true);
}


void MainWindow::on_radioButton_PNG_clicked()
{
    ui->slider_Quality->setValue(100);
    ui->slider_Quality->setEnabled(false);
}


void MainWindow::messageBox(QString str) {
    QMessageBox msgBox;
    msgBox.setText(str);
    msgBox.setWindowTitle("Blitz Batch Image Resizer");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}


MainWindow::~MainWindow()
{
    delete ui;
}
