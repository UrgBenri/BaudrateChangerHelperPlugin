/*
	This file is part of the UrgBenri application.

	Copyright (c) 2016 Mehrez Kristou.
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Please contact kristou@hokuyo-aut.jp for more details.

*/

#include "BaudrateChangerHelperPlugin.h"
#include "ui_BaudrateChangerHelperPlugin.h"

#include <QSettings>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDebug>

#define URG_DEFAULT_BAUDERATE 115200

BaudrateChangerHelperPlugin::BaudrateChangerHelperPlugin(QWidget *parent)
    : HelperPluginInterface(parent)
    , ui(new Ui::BaudrateChangerHelperPlugin)
    , m_sensor(NULL)
{
    REGISTER_FUNCTION(setDeviceMethod);
    REGISTER_FUNCTION(lastBaudrateMethod);

    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    setWindowTitle(pluginName());
    setWindowIcon(pluginIcon());

    ui->targetBaudrate->addItem("19.2Kbps", 19200);
    ui->targetBaudrate->addItem("38.4Kbps", 38400);
    ui->targetBaudrate->addItem("57.6Kbps", 57600);
    ui->targetBaudrate->addItem("115.2Kbps", 115200);
    ui->targetBaudrate->addItem("230.4Kbps", 230400);
    ui->targetBaudrate->addItem("250Kbps", 250000);
    ui->targetBaudrate->addItem("460.8Kbps", 460800);
    ui->targetBaudrate->addItem("500Kbps", 500000);
    ui->targetBaudrate->addItem("614.4Kbps", 614400);
    ui->targetBaudrate->addItem("750Kbps", 750000);
    ui->targetBaudrate->addItem("781.25Kbps", 781250);
    ui->targetBaudrate->addItem("921.6Kbps", 921600);
    selectBauderate(URG_DEFAULT_BAUDERATE);

    ui->targetBaudrate->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    connect(ui->updateButton, &QAbstractButton::clicked,
            this, &BaudrateChangerHelperPlugin::updateButtonClicked);
    connect(ui->closeButton, &QAbstractButton::clicked,
            this, [&](){
               QDialog *parentWidget = qobject_cast<QDialog *>(this->parent());
               if(parentWidget) parentWidget->close();
    });
}

void BaudrateChangerHelperPlugin::selectBauderate(long baud)
{
    for(int i = 0; i < ui->targetBaudrate->count(); ++i){
        if(ui->targetBaudrate->itemData(i).toDouble() == baud){
            ui->targetBaudrate->setCurrentIndex(i);
            break;
        }
    }
}

BaudrateChangerHelperPlugin::~BaudrateChangerHelperPlugin()
{
    delete ui;
}

UrgDevice *BaudrateChangerHelperPlugin::sensor() const
{
    return m_sensor;
}

void BaudrateChangerHelperPlugin::setSensor(UrgDevice *sensor)
{
    m_sensor = sensor;
}

QVariant BaudrateChangerHelperPlugin::setDeviceMethod(const QVariantList &vars)
{
    if(vars.size() == 1){
        setSensor((UrgDevice*) vars[0].value<void *>());
    }
    return QVariant();
}

QVariant BaudrateChangerHelperPlugin::lastBaudrateMethod(const QVariantList &vars)
{
    return (qlonglong)lastBaudrate();
}

void BaudrateChangerHelperPlugin::onLoad(PluginManagerInterface *manager)
{

}

void BaudrateChangerHelperPlugin::onUnload()
{

}

void BaudrateChangerHelperPlugin::saveState(QSettings &settings)
{
    settings.setValue("widgetGeometry", saveGeometry());
}

void BaudrateChangerHelperPlugin::restoreState(QSettings &settings)
{
    restoreGeometry(settings.value("widgetGeometry", saveGeometry()).toByteArray());
}

void BaudrateChangerHelperPlugin::loadTranslator(const QString &locale)
{
    qApp->removeTranslator(&m_translator);
    if (m_translator.load(QString("plugin.%1").arg(locale), ":/BaudrateChangerHelperPlugin")) {
        qApp->installTranslator(&m_translator);
    }
}

void BaudrateChangerHelperPlugin::closeEvent(QCloseEvent *)
{

}

void BaudrateChangerHelperPlugin::showEvent(QShowEvent *event)
{
    if(!event->spontaneous()){
        if(m_sensor && m_sensor->isConnected()){
            selectBauderate(m_sensor->connection()->baudrate());
            ui->currentBaudrate->setText(ui->targetBaudrate->itemText(ui->targetBaudrate->currentIndex()));
            m_lastBaudrate = m_sensor->connection()->baudrate();
            ui->updateButton->setEnabled(true);
        }
    }
}

void BaudrateChangerHelperPlugin::updateButtonClicked()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(tr("You are going to change the connection speed of the connected device.\n"
                      "The new speed will be set to \"%1\".").arg(ui->targetBaudrate->itemText(ui->targetBaudrate->currentIndex())));
    msgBox.setInformativeText(tr("Do you want to carry on this change?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    switch (msgBox.exec()) {
    case QMessageBox::No:
        return;
    }

    if(m_sensor && m_sensor->isConnected()){
        long currentBaude = ui->targetBaudrate->itemData(ui->targetBaudrate->currentIndex()).toDouble();
        if(m_sensor->changeBaurate(currentBaude) ||
                m_sensor->changeBaurate(currentBaude)){
            m_sensor->disconnect();
            emit information(tr("Baudrate changer"),
                             tr("Sensor communication speed changed successfully."));
            emit baudrateChanged(currentBaude);
            ui->currentBaudrate->setText(ui->targetBaudrate->itemText(ui->targetBaudrate->currentIndex()));
            QMessageBox::information(this, tr("Baudrate changer"),
                                     tr("The connection speed is set to %1").arg(ui->targetBaudrate->itemText(ui->targetBaudrate->currentIndex())));
            m_lastBaudrate = currentBaude;

            QDialog *parentWidget = qobject_cast<QDialog *>(parent());
            if(parentWidget) parentWidget->close();
        }else{
            emit error(tr("Baudrate changer"),
                       tr("Sensor communication speed could not be changed"));
            QMessageBox::information(this, tr("Baudrate changer"),
                                     tr("Sensor communication speed could not be changed"));
        }
    }
}

void BaudrateChangerHelperPlugin::changeEvent(QEvent *e)
{
    HelperPluginInterface::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange: {
        if(ui) ui->retranslateUi(this);
        setWindowTitle(pluginName());
    }break;
    default: {
    }
    }
}

