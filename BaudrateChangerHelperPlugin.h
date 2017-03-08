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

#ifndef BAUDRATECHANGERDIALOG_H
#define BAUDRATECHANGERDIALOG_H

#include "HelperPluginInterface.h"
#include <QShowEvent>
#include <QSettings>
#include <QTranslator>

#include "UrgDevice.h"

using namespace qrk;

namespace Ui {
class BaudrateChangerHelperPlugin;
}

class BaudrateChangerHelperPlugin : public HelperPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(HelperPluginInterface)
    Q_PLUGIN_METADATA(IID "org.kristou.UrgBenri.BaudrateChangerHelperPlugin")
    PLUGIN_FACTORY(BaudrateChangerHelperPlugin)

public:
    explicit BaudrateChangerHelperPlugin(QWidget *parent = 0);
    virtual ~BaudrateChangerHelperPlugin();

    QString pluginName() const{return tr("Baudrate Changer");}
    QIcon pluginIcon() const {return QIcon(":/BaudrateChangerHelperPlugin/tabIcon");}
    QString pluginDescription() const {return tr("Baudrate Changer");}
    PluginVersion pluginVersion() const {return PluginVersion(1, 0, 0);}
    QString pluginAuthorName() const {return "Mehrez Kristou";}
    QString pluginAuthorContact() const {return "mehrez@kristou.com";}
    int pluginLoadOrder() const {return 0;}
    bool pluginIsExperimental() const { return false; }

    QString pluginLicense() const { return "GPL"; }
    QString pluginLicenseDescription() const { return "GPL"; }

    void saveState(QSettings &settings);
    void restoreState(QSettings &settings);

    void loadTranslator(const QString &locale);

signals:
    void error(const QString &header, const QString &message);
    void warning(const QString &header, const QString &message);
    void information(const QString &header, const QString &message);
    void baudrateChanged(long baud);

public slots:
    void onLoad(PluginManagerInterface *manager);
    void onUnload();

protected:
    void closeEvent(QCloseEvent *);
    void showEvent(QShowEvent *event);
    void changeEvent(QEvent *e);

private slots:
    void updateButtonClicked();

private:
    Ui::BaudrateChangerHelperPlugin *ui;
    UrgDevice *m_sensor;
    long m_lastBaudrate;
    QTranslator m_translator;

    void selectBauderate(long baud);

    UrgDevice *sensor() const;
    void setSensor(UrgDevice *sensor);
    long lastBaudrate(){ return m_lastBaudrate;}

    //Functions interface
    QVariant setDeviceMethod(const QVariantList &vars);
    QVariant lastBaudrateMethod(const QVariantList &vars);
};

#endif // BAUDRATECHANGERDIALOG_H

