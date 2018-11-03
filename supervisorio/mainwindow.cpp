#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QProcess>
#include <QJsonObject>
#include "logger.h"
#include "camerasettingmessage.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Logger::Init();

    connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(processReadyReadStandardOutput()));
    connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(processReadyReadStandardError()));
    connect(&process, SIGNAL(started()), this, SLOT(processStarted()));
    connect(Logger::Object(), SIGNAL(addToLogger(QString)), this, SLOT(addToLogger(QString)));

    qRegisterMetaType<ImageMessage>("ImageMessage");
    qRegisterMetaType<Message>("Message");

    tcpPort = 9572;

    connect(&tcpClient, SIGNAL(updateImage(ImageMessage)), this, SLOT(updateImage(ImageMessage)));
    connect(&tcpClient, SIGNAL(addImageType(QString)), this, SLOT(addImageType(QString)));
    connect(&tcpClient, SIGNAL(cameraSetting(int, int)), this, SLOT(cameraSetting(int, int)));
    connect(&tcpClient, SIGNAL(messageReceived(Message)), this, SLOT(messageReceived(Message)));

    currentGitRepo = "https://github.com/AlexanderSilvaB/Mari.git";
    selectedRobot = "127.0.0.1";
    findToolchains();
    load();
    loadModules();
    tcpClient.setPort(tcpPort);

    robotManager.setWidget(ui->treeRobots);
    robotManager.startSearch();

}

MainWindow::~MainWindow()
{
    tcpClient.disconnectFromHost();
    robotManager.stopSearch();
    delete ui;
}

void MainWindow::executeProcess(QString program, QStringList arguments)
{
    process.start(program,arguments);
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Finished: " << exitCode;
}

void MainWindow::processReadyReadStandardError()
{
    qDebug() << "ReadyError";
    QProcess *p = (QProcess *)sender();
    QByteArray buf = p->readAllStandardOutput();
    QString text = QString::fromUtf8(buf.data());

    qDebug() << text;
    Logger::log(text, LEVEL_ERROR);
}

void MainWindow::processReadyReadStandardOutput()
{
    QProcess *p = (QProcess *)sender();
    QByteArray buf = p->readAllStandardOutput();
    QString text = QString::fromUtf8(buf.data());

    qDebug() << text;

    Logger::log(text, LEVEL_DEBUG);
}

void MainWindow::processStarted()
{
    qDebug() << "Proc Started";
}

void MainWindow::loadModules()
{
    QFile readfile(codeReleasePath + "/root/home/nao/data/config.json");
    if (!readfile.open(QIODevice::ReadOnly))
    {
        Logger::log("Could not load the modules list");
        return;
    }
    QByteArray saveData = readfile.readAll();
    readfile.close();


    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    QJsonObject json = loadDoc.object();

    if(!json.contains("Modules"))
        json.insert("Modules", QJsonObject());

    QJsonObject modules = json["Modules"].toObject();

    loadModule(modules, "Motion", ui->chkMotion);
    loadModule(modules, "Perception", ui->chkPerception);
    loadModule(modules, "Strategy", ui->chkStrategy);
    loadModule(modules, "Remote", ui->chkRemoteControl);
    loadModule(modules, "Network", ui->chkNetwork);
}

void MainWindow::loadModule(QJsonObject &modules, QString moduleName, QCheckBox *checkbox)
{
    if(modules.contains(moduleName))
    {
        QJsonObject module = modules[moduleName].toObject();
        if(module.contains("Enabled"))
            checkbox->setChecked(module["Enabled"].toBool());
        else
            checkbox->setChecked(true);
    }
    else
        checkbox->setChecked(true);
}

void MainWindow::saveModules()
{
    QFile readfile(codeReleasePath + "/root/home/nao/data/config.json");
    if (!readfile.open(QIODevice::ReadOnly))
    {
        Logger::log("Could not load the modules list");
        return;
    }
    QByteArray saveData = readfile.readAll();
    readfile.close();


    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    QJsonObject json = loadDoc.object();

    if(!json.contains("Modules"))
        json.insert("Modules", QJsonObject());

    QJsonObject modules = json["Modules"].toObject();

    // Motion
    saveModule(modules, "Motion", ui->chkMotion);
    saveModule(modules, "Perception", ui->chkPerception);
    saveModule(modules, "Strategy", ui->chkStrategy);
    saveModule(modules, "Remote", ui->chkRemoteControl);
    saveModule(modules, "Network", ui->chkNetwork);

    json["Modules"] = modules;

    QFile writefile(codeReleasePath + "/root/home/nao/data/config.json");
    if (!writefile.open(QIODevice::WriteOnly))
    {
        Logger::log("Could not save the modules list");
        return;
    }
    QJsonDocument saveDoc(json);
    writefile.write(saveDoc.toJson());
    writefile.close();
}

void MainWindow::saveModule(QJsonObject &modules, QString moduleName, QCheckBox *checkbox)
{
    if(!modules.contains(moduleName))
        modules.insert(moduleName, QJsonObject());
    QJsonObject module = modules[moduleName].toObject();
    module["Enabled"] = checkbox->isChecked();
    modules[moduleName] = module;
}

void MainWindow::on_btnInstall_clicked()
{
    saveModules();
    QString program = "bash";
    QStringList arguments;
    arguments << "-c" << "cd "+ codeReleasePath + "; ./sync.sh " + selectedRobot + " " + selectedToolchain + (ui->chkAll->isChecked() ? " -all" : "");
    executeProcess(program, arguments);
}

void MainWindow::on_btnUninstall_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Uninstall");
    msgBox.setInformativeText("Do you really want to uninstall on " + selectedRobot + "?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Yes)
    {
        QString program = "bash";
        QStringList arguments;
        arguments << "-c" << "cd "+ codeReleasePath + "; ./sync.sh " + selectedRobot + " " + selectedToolchain + " -uninstall";
        executeProcess(program, arguments);
    }
}

void MainWindow::on_treeRobots_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(current != nullptr)
        selectedRobot = current->text(0);
    else
        selectedRobot = "127.0.0.1";
    Logger::log("Current robot: " + selectedRobot);
    tcpClient.connectToHost(selectedRobot);
}

void MainWindow::findToolchains()
{
    #ifdef Q_OS_WIN32
    ui->comboToolchain->addItem("Cannot find a toolchain");
    ui->btnCompile->setEnabled(false);
    ui->btnInstall->setEnabled(false);
    ui->btnConfigure->setEnabled(false);
    ui->btnUninstall->setEnabled(false);
    ui->btnClear->setEnabled(false);
    ui->comboToolchain->setEnabled(false);
    ui->tabActions->setEnabled(false);
    #else
    ui->lbWindows->deleteLater();
    ui->comboToolchain->clear();

    QProcess process;
    process.start("qitoolchain", QStringList() << "list");
    process.waitForFinished();
    QString output(process.readAllStandardOutput());
#ifdef Q_OS_WIN32
    QStringList lines = output.split("\r\n", QString::SplitBehavior::SkipEmptyParts);
#else
    QStringList lines = output.split("\n", QString::SplitBehavior::SkipEmptyParts);
#endif
    for (int i = 0; i < lines.size(); i++)
    {
        QString line = lines[i].trimmed();
        if(line.at(0) == '*')
        {
            QString item = line.mid(line.indexOf('*')+1).trimmed();
            ui->comboToolchain->addItem(item);
        }
    }
    if(ui->comboToolchain->count() == 0)
    {
        ui->comboToolchain->addItem("Cannot find a toolchain");
        ui->btnCompile->setEnabled(false);
        ui->btnInstall->setEnabled(false);
        ui->btnConfigure->setEnabled(false);
        ui->btnClear->setEnabled(false);
    }
    ui->comboToolchain->setCurrentIndex(0);
    selectedToolchain = ui->comboToolchain->currentText();
    if(selectedToolchain.contains(' '))
        selectedToolchain = "any";
    #endif
}

void MainWindow::on_comboToolchain_activated(const QString &arg1)
{
    selectedToolchain = arg1;
    if(selectedToolchain.contains(' '))
        selectedToolchain = "any";
    Logger::log("Current toolchain: " + selectedToolchain);
}

void MainWindow::load()
{
    QFile readfile("settings.json");
    if (!readfile.open(QIODevice::ReadOnly))
    {
        return;
    }
    QByteArray saveData = readfile.readAll();
    readfile.close();


    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    QJsonObject json = loadDoc.object();

    codeReleasePath = loadSetting(json, "codeReleasePath", "");
    selectedToolchain = loadSetting(json, "toolchain", "any");
    tcpPort = loadSetting(json, "tcpPort", "9572").toInt();
    ui->comboToolchain->setCurrentText(selectedToolchain);
    ui->teCodeRelease->setText(codeReleasePath);

    installed = loadSetting(json, "installed", "false") == "true";
    if(!installed)
    {
        QMessageBox msgBox;
        msgBox.setText("Install");
        msgBox.setInformativeText("You need some tools installed to use this software. You can install it running the 'requirements.sh' bundled with this software.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        installed = true;
    }
}

QString MainWindow::loadSetting(QJsonObject &json, QString name, QString defaltValue)
{
    if(json.contains(name))
        return json[name].toString();
    return defaltValue;
}

void MainWindow::save()
{
    QJsonObject json;
    QFile readfile("settings.json");
    if (readfile.open(QIODevice::ReadOnly))
    {
        QByteArray saveData = readfile.readAll();
        readfile.close();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        json = loadDoc.object();
    }



    saveSetting(json, "codeReleasePath", codeReleasePath);
    saveSetting(json, "toolchain", selectedToolchain);
    saveSetting(json, "tcpPort", QString::number(tcpPort));
    saveSetting(json, "installed", installed ? "true" : "false");

    QFile writefile("settings.json");
    if (!writefile.open(QIODevice::WriteOnly))
    {
        return;
    }
    QJsonDocument saveDoc(json);
    writefile.write(saveDoc.toJson());
    writefile.close();
}

void MainWindow::saveSetting(QJsonObject &json, QString name, QString value)
{
    if(!json.contains(name))
        json.insert(name, value);
    else
        json[name] = value;
}

void MainWindow::closeEvent(QCloseEvent *bar)
{
    save();
}

void MainWindow::on_btnAddRobot_clicked()
{
    QString ip = ui->edRobotIp->text();
    if(!robotManager.addRobot(ip))
    {

    }
}

void MainWindow::on_btnSelectCodeRelease_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "Select path", "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(path != NULL && path.length() > 0){
        ui->teCodeRelease->setText(path);
        codeReleasePath = path;
        loadModules();
    }
}

void MainWindow::on_btnCompile_clicked()
{
    QString program = "bash";
    QStringList arguments;
    arguments << "-c" << "cd "+ codeReleasePath + "; ./build.sh " + selectedToolchain;
    executeProcess(program, arguments);
}

void MainWindow::addToLogger(QString text)
{
    ui->txtLog->append(text);
    ui->txtLog->moveCursor(QTextCursor::End);
}
void MainWindow::messageReceived(Message msg)
{
    if(ui->chkTCP->isChecked()){
        bool add = false;
        QString base = "";
        switch (msg.getLevel())
        {
            case LEVEL_INFO:
                base = "<font color='#219e27'>I [%1] <b>%2</b></font>";
                add |= ui->chkInfo->isChecked();
                break;
            case LEVEL_DEBUG:
                base = "<font color='#162a99'>D [%1] <b>%2</b></font>";
                add |= ui->chkDebug->isChecked();
                break;
            case LEVEL_WARNING:
                base = "<font color='#e0ac00'>W [%1] <b>%2</b></font>";
                add |= ui->chkWarning->isChecked();
                break;
            case LEVEL_ERROR:
                base = "<font color='#c90000'>E [%1] <b>%2</b></font>";
                add |= ui->chkError->isChecked();
                break;
            default:
                return;
        }
        QString dateTime = msg.getDateTime().toString("hh:mm:ss dd/MM/yyyy");
        QString text = base.arg(dateTime, msg.toString());
        if(add)
        {
            ui->txtMensagem->append(text);
            ui->txtMensagem->moveCursor(QTextCursor::End);
        }
    }
}

void MainWindow::on_btnDownloadGithub_clicked()
{
    QString program = "bash";
    QStringList arguments;
    arguments << "-c" << "mkdir -p ~/RinobotCodeRelease; cd ~/RinobotCodeRelease; /usr/bin/git clone " + currentGitRepo;
    executeProcess(program, arguments);
    codeReleasePath = "~/RinobotCodeRelease/Mari";
}

void MainWindow::addImageType(QString name)
{
    if(ui->comboCameraImage->findText(name) < 0)
        ui->comboCameraImage->addItem(name);
    if(ui->comboCameraImage->count() == 1)
    {
        tcpClient.setImageType(name);
        ui->comboCameraImage->setCurrentIndex(0);
    }
}

void MainWindow::updateImage(ImageMessage imageMessage)
{
    if(ui->mainTabs->currentWidget() != ui->tabCamera)
        return;

    QImage::Format format = QImage::Format_RGB888;
    switch (imageMessage.getImageType())
    {
        case IMAGE_TYPE_GRAY:
            format = QImage::Format_Grayscale8;
            break;
        case IMAGE_TYPE_YUV:
            format = QImage::Format_RGB444;
            break;
        default:
            break;
    }
    QImage image(imageMessage.getData(), imageMessage.getWidth(), imageMessage.getHeight(), imageMessage.getStep(), format);
    QPixmap pixMap = QPixmap::fromImage(image);
    //pixMap = pixMap.scaled(cols, rows, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->lblCameraImage->setPixmap(pixMap);
}

void MainWindow::on_comboCameraImage_activated(const QString &arg1)
{
    tcpClient.setImageType(arg1);
}

void MainWindow::on_sliderCameraBrightness_valueChanged(int value)
{
    CameraSettingMessage message(SETTING_BRIGHTNESS, value);
    tcpClient.send(&message);
}

void MainWindow::on_sliderCameraSaturation_valueChanged(int value)
{
    CameraSettingMessage message(SETTING_SATURATION, value);
    tcpClient.send(&message);
}

void MainWindow::on_sliderCameraContrast_valueChanged(int value)
{
    CameraSettingMessage message(SETTING_CONTRAST, value);
    tcpClient.send(&message);
}

void MainWindow::on_sliderCameraSharpness_valueChanged(int value)
{
    CameraSettingMessage message(SETTING_SHARPNESS, value);
    tcpClient.send(&message);
}

void MainWindow::on_edRobotIp_returnPressed()
{
    QString ip = ui->edRobotIp->text();
    if(!robotManager.addRobot(ip))
    {

    }
}

void MainWindow::cameraSetting(int setting, int value)
{
    switch (setting)
    {
        case SETTING_BRIGHTNESS:
            ui->sliderCameraBrightness->setValue(value);
            break;
        case SETTING_CONTRAST:
            ui->sliderCameraContrast->setValue(value);
            break;
        case SETTING_SATURATION:
            ui->sliderCameraSaturation->setValue(value);
            break;
        case SETTING_SHARPNESS:
            ui->sliderCameraSharpness->setValue(value);
            break;
        default:
            break;
    }
}

void MainWindow::on_btnBoxCamera_clicked(QAbstractButton *button)
{
    if(button->text() == "Save")
    {
        CameraSettingMessage message(SETTING_SAVE, 1);
        tcpClient.send(&message);
    }
    else if(button->text() == "Discard")
    {
        CameraSettingMessage message(SETTING_DISCARD, 1);
        tcpClient.send(&message);
    }
}

void MainWindow::on_btnConfigure_clicked()
{
    QString program = "bash";
    QStringList arguments;
    arguments << "-c" << "cd "+ codeReleasePath + "; ./build.sh " + selectedToolchain + " -configure";
    executeProcess(program, arguments);
}

void MainWindow::on_btnClear_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Clear");
    msgBox.setInformativeText("Do you really want to clear the build for " + selectedToolchain + "?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Yes)
    {
        QString program = "bash";
        QStringList arguments;
        arguments << "-c" << "cd "+ codeReleasePath + "; ./sync.sh " + selectedRobot + " " + selectedToolchain + " -clear";
        executeProcess(program, arguments);
    }
}


void MainWindow::on_comboCamera_activated(const QString &arg1)
{
    int value;
    value = (arg1 == "Top")? 1:0;
    CameraSettingMessage message(SETTING_NUMBER, value);
    tcpClient.send(&message);
}
