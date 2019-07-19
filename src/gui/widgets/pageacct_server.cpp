#ifndef __STABLE_HPP__
#include <core/stable.hpp>
#endif

#include <gui/widgets/pageacct_server.hpp>
#include <ui_pageacct_server.h>

#include <gui/widgets/dlgchooser.hpp>
#include <gui/widgets/detailedit.hpp>

#include <core/moneychanger.hpp>

#include <opentxs/opentxs.hpp>

MTPageAcct_Server::MTPageAcct_Server(QWidget *parent) :
    QWizardPage(parent),
    m_bFirstRun(true),
    ui(new Ui::MTPageAcct_Server)
{
    ui->setupUi(this);

    QString style_sheet = "QPushButton{border: none; border-style: outset; text-align:left; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #dadbde, stop: 1 #f6f7fa);}"
            "QPushButton:pressed {border: 1px solid black; text-align:left; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #dadbde, stop: 1 #f6f7fa); }"
            "QPushButton:hover {border: 1px solid black; text-align:left; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #dadbde, stop: 1 #f6f7fa); }";

    ui->pushButtonSelect->setStyleSheet(style_sheet);

    ui->lineEditID->setStyleSheet("QLineEdit { background-color: lightgray }");

    this->registerField("NotaryID*",  ui->lineEditID);
    this->registerField("ServerName", ui->pushButtonSelect, "text");
    // -----------------------------------------------
    connect(this, SIGNAL(SetDefaultServer(QString, QString)), Moneychanger::It(), SLOT(setDefaultServer(QString,QString)));
}



void MTPageAcct_Server::on_pushButtonSelect_clicked()
{
    const auto & ot = Moneychanger::It()->OT();
    const auto reason = ot.Factory().PasswordPrompt(__FUNCTION__);

    QString qstr_default_id = Moneychanger::It()->get_default_notary_id();
    // -------------------------------------------
    QString qstr_current_id = field("NotaryID").toString();
    // -------------------------------------------
    if (qstr_current_id.isEmpty())
        qstr_current_id = qstr_default_id;
    // -------------------------------------------
    const auto servers = ot.Wallet().ServerList();

    if (qstr_current_id.isEmpty() && (servers.size() > 0))
        qstr_current_id = QString::fromStdString(servers.front().first);

    const auto notaryId = ot.Factory().ServerID(qstr_current_id.toStdString());
    // -------------------------------------------
    // Select from Servers in local wallet.
    //
    DlgChooser theChooser(this);
    // -----------------------------------------------
    mapIDName & the_map = theChooser.m_map;

    bool bFoundDefault = false;
    // -----------------------------------------------
//    const int32_t the_count = ot.Exec().GetServerCount();
    // -----------------------------------------------
    for (const auto & [id, name] : servers)
    {
        QString OT_id = QString::fromStdString(id);
        QString OT_name = QString::fromStdString(name);
        // -----------------------------------------------
        if (!OT_id.isEmpty())
        {
            if (!qstr_current_id.isEmpty() && (0 == qstr_current_id.compare(OT_id)))
                bFoundDefault = true;
            // -----------------------------------------------
            the_map.insert(OT_id, OT_name);
        }
     }
    // -----------------------------------------------
    if (bFoundDefault)
        theChooser.SetPreSelected(qstr_current_id);
    // -----------------------------------------------
    theChooser.setWindowTitle(tr("Select the Server"));
    // -----------------------------------------------
    if (theChooser.exec() == QDialog::Accepted)
    {
        if (!theChooser.m_qstrCurrentID  .isEmpty() &&
            !theChooser.m_qstrCurrentName.isEmpty())
        {
            setField("NotaryID",   theChooser.m_qstrCurrentID);
            setField("ServerName", theChooser.m_qstrCurrentName);
            // -----------------------------------------
            ui->lineEditID->home(false);
            // -----------------------------------------
            if (qstr_default_id.isEmpty())
                emit SetDefaultServer(theChooser.m_qstrCurrentID, theChooser.m_qstrCurrentName);
        }
    }
}

void MTPageAcct_Server::initializePage() //virtual
{
    const auto & ot = Moneychanger::It()->OT();
    const auto reason = ot.Factory().PasswordPrompt(__FUNCTION__);
    const auto servers = ot.Wallet().ServerList();

    if (!Moneychanger::It()->expertMode())
    {
        ui->pushButtonManage->setVisible(false);
    }
    // -------------------------------------------
    std::string str_name;
    QString     qstr_id;
    // -------------------------------------------
    QString qstr_default_id = Moneychanger::It()->get_default_notary_id();
    // -------------------------------------------
    QString qstr_current_id = field("NotaryID").toString();
    // -------------------------------------------
    qstr_id = qstr_current_id.isEmpty() ? qstr_default_id : qstr_current_id;
    // -------------------------------------------
    if (qstr_id.isEmpty() && (servers.size() > 0))
        qstr_id = QString::fromStdString(servers.front().first);
    // -------------------------------------------
    if (!qstr_id.isEmpty())
    {
        const auto notaryId = ot.Factory().ServerID(qstr_id.toStdString());
        const auto notary = ot.Wallet().Server(notaryId, reason);
        str_name = notary->Alias();
    }
    // -------------------------------------------
    if (str_name.empty() || qstr_id.isEmpty())
        SetFieldsBlank();
    else
    {
        QString qstrName = QString::fromStdString(str_name);
        // ---------------------------
        setField("NotaryID",   qstr_id);
        setField("ServerName", qstrName);
        // ---------------------------
        ui->lineEditID->home(false);
        // ---------------------------
        if (qstr_default_id.isEmpty())
            emit SetDefaultServer(qstr_id, qstrName);
    }
}


void MTPageAcct_Server::on_pushButtonManage_clicked()
{
    const auto & ot = Moneychanger::It()->OT();
    const auto reason = ot.Factory().PasswordPrompt(__FUNCTION__);
    const auto servers = ot.Wallet().ServerList();

    MTDetailEdit * pWindow = new MTDetailEdit(this);

    pWindow->setAttribute(Qt::WA_DeleteOnClose);
    // -------------------------------------
    mapIDName & the_map = pWindow->m_map;
    // -------------------------------------
    the_map.clear();
    // -------------------------------------
    QString qstrPreSelected   = field("NotaryID").toString();
    bool    bFoundPreselected = false;
    // -------------------------------------
    int32_t the_count = servers.size();
    bool    bStartingWithNone = (the_count < 1);

    for (const auto& [id, name] : servers)
    {
        QString OT_id   = QString::fromStdString(id);
        QString OT_name = QString::fromStdString(name);

        the_map.insert(OT_id, OT_name);

        if (!qstrPreSelected.isEmpty() && (0 == qstrPreSelected.compare(OT_id)))
            bFoundPreselected = true;
    } // for
    // -------------------------------------
    pWindow->setWindowTitle(tr("Manage Servers"));
    // -------------------------------------
    if (bFoundPreselected)
        pWindow->SetPreSelected(qstrPreSelected);
    // -------------------------------------
    pWindow->dialog(MTDetailEdit::DetailEditTypeServer, true);
    // -------------------------------------
    if (bStartingWithNone && (servers.size() > 0))
    {
        std::string str_id = servers.front().first;

        if (!str_id.empty())
        {
            const auto notaryId = ot.Factory().ServerID(str_id);
            const auto notary = ot.Wallet().Server(notaryId, reason);
            std::string str_name = notary->Alias();

            if (str_name.empty())
                str_name = str_id;
            // --------------------------------
            setField("NotaryID",   QString::fromStdString(str_id));
            setField("ServerName", QString::fromStdString(str_name));
            // --------------------------------
            ui->lineEditID->home(false);
        }
    }
    // -------------------------------------
    else if (servers.size() < 1)
        SetFieldsBlank();
}

void MTPageAcct_Server::SetFieldsBlank()
{
    setField("NotaryID",   "");
    setField("ServerName", QString("<%1>").arg(tr("Click to choose Server")));
}

MTPageAcct_Server::~MTPageAcct_Server()
{
    delete ui;
}
