#include "browser.h"
#include <opcuaclient/UaBrowser.h>
namespace EleSyOpcUaClient
{

    QUaBrowser::QUaBrowser()
        : QObject(nullptr)
    {
        connect(qlw, &QListWidget::itemClicked, this, &QUaBrowser::currentDirChanged);
    }

QUaBrowser::QUaBrowser(UaConnection::Ptr conn)
        : QObject(nullptr)
    {
        connect(qlw, &QListWidget::itemClicked, this, &QUaBrowser::currentDirChanged);
        setConnection(conn);
    }

    QListWidget *QUaBrowser::widget()
    {
        return qlw;
    }

    void QUaBrowser::setConnection(UaConnection::Ptr conn)
    {
        connection = conn;
        if(!isConnected())
        {
            status = uaBad;
        }
        else if(browserInit())
        {
            status = browser->changeBrowsePosition(NODEID_OBJECTS);
            readCurrentDir();
        }
    }

    bool QUaBrowser::readCurrentDir(BrowseDirection direction, ReferenceCategory refCategory)
    {
        std::vector<std::string> tmp;
        BrowseResults result;
        status = browser->browse(direction, refCategory, tmp, result);
        if(status == uaGood)
        {
            for(const auto &str : tmp)
            {
                qlw->addItem(QString::fromStdString(str));
            }
            UaBytes contPoint, contPointNext;
            do
            {
                status = browser->browseNext(contPoint, tmp, result, contPointNext);
                contPoint = contPointNext;
                for(const auto &str : tmp)
                {
                    qlw->addItem(QString::fromStdString(str));
                }
            } while(!tmp.empty() && status == uaGood);
            browser->releasePoint(contPoint);
        }
        return status == uaGood;
    }

    bool QUaBrowser::isConnected() const
    {
        return connection->isConnected();
    }

    bool QUaBrowser::browserInit()
    {
        status = EleSyOpcUaClient::createBrowser(connection, browser);
        return status == uaGood;
    }

    void QUaBrowser::currentDirChanged(QListWidgetItem *item)
    {
        if(item)
        {
            auto dirname = item->text();
            status = browser->changeBrowsePosition(dirname.toStdString());
            if(status == uaGood)
            {
                qlw->clear();
                readCurrentDir();
                emit directoryChanged(dirname);
            }
        }
    }

}
