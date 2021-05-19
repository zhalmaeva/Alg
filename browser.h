#pragma once
#include <QListWidget>
#include <opcuaclient/UaBrowser.h>

#include "util.h"

namespace EleSyOpcUaClient
{

    class QUaBrowser : public QObject
    {
    Q_OBJECT
    public:
        using Ptr = std::shared_ptr<QUaBrowser>;
        QUaBrowser();
        QUaBrowser(EleSyOpcUaClient::UaConnection::Ptr conn);
        ~QUaBrowser()
        {
            delete qlw;
        }
        QListWidget* widget();

        void setConnection(UaConnection::Ptr conn);

        bool readCurrentDir(BrowseDirection direction = BrowseDirection::both, ReferenceCategory refCategory = ReferenceCategory::hierarchical);

        UaStatusCode lastError() const { return status; }
        bool isValid() const { return status == uaGood; }
        inline bool isConnected() const;
        void checkConnected();
        bool browserInit();

    private:
        UaConnection::Ptr connection;
        UaBrowser::Ptr    browser;
        UaStatusCode      status;
        QListWidget* qlw { new QListWidget };
    signals:
        void directoryChanged(const QString &dirname);
    public slots:
        void currentDirChanged(QListWidgetItem *item);
    };

}

