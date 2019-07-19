#ifndef __STABLE_HPP__
#include <core/stable.hpp>
#endif

#include <gui/widgets/pageoffer_assets.hpp>
#include <ui_pageoffer_assets.h>

#include <core/moneychanger.hpp>

#include <gui/widgets/dlgchooser.hpp>
#include <gui/widgets/detailedit.hpp>

#include <opentxs/opentxs.hpp>

PageOffer_Assets::PageOffer_Assets(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::PageOffer_Assets)
{
    ui->setupUi(this);

    QString style_sheet = "QPushButton{border: none; border-style: outset; text-align:left; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #dadbde, stop: 1 #f6f7fa);}"
            "QPushButton:pressed {border: 1px solid black; text-align:left; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #dadbde, stop: 1 #f6f7fa); }"
            "QPushButton:hover {border: 1px solid black; text-align:left; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #dadbde, stop: 1 #f6f7fa); }";

    ui->pushButtonSelectAsset   ->setStyleSheet(style_sheet);
    ui->pushButtonSelectCurrency->setStyleSheet(style_sheet);

    ui->labelInstrumentDefinitionID   ->setStyleSheet("QLabel { color : gray; }");
    ui->labelCurrencyID->setStyleSheet("QLabel { color : gray; }");

    ui->lineEditInstrumentDefinitionID   ->setStyleSheet("QLineEdit { background-color: lightgray }");
    ui->lineEditCurrencyID->setStyleSheet("QLineEdit { background-color: lightgray }");

    this->registerField("InstrumentDefinitionID*",    ui->lineEditInstrumentDefinitionID);
    this->registerField("CurrencyID*", ui->lineEditCurrencyID);

    this->registerField("AssetName",    ui->pushButtonSelectAsset,    "text");
    this->registerField("CurrencyName", ui->pushButtonSelectCurrency, "text");
}


void PageOffer_Assets::on_pushButtonManageAsset_clicked()
{
    const auto & ot = Moneychanger::It()->OT();
    const auto reason = ot.Factory().PasswordPrompt(__FUNCTION__);

    MTDetailEdit * pWindow = new MTDetailEdit(this);

    pWindow->setAttribute(Qt::WA_DeleteOnClose);
    // -------------------------------------
    mapIDName & the_map = pWindow->m_map;
    // -------------------------------------
    the_map.clear();
    // -------------------------------------
    QString qstrPreselected   = field("InstrumentDefinitionID").toString();
    bool    bFoundPreselected = false;
    // -------------------------------------
    const auto units = ot.Wallet().UnitDefinitionList();

    int32_t the_count = units.size();
    bool    bStartingWithNone = (the_count < 1);

    for (const auto & [id, name] : units)
    {
        QString OT_id   = QString::fromStdString(id);
        QString OT_name = QString::fromStdString(name);

        the_map.insert(OT_id, OT_name);

        if (!qstrPreselected.isEmpty() && (0 == qstrPreselected.compare(OT_id)))
            bFoundPreselected = true;
    } // for
    // -------------------------------------
    if (bFoundPreselected)
        pWindow->SetPreSelected(qstrPreselected);
    // -------------------------------------
    pWindow->setWindowTitle(tr("Manage Asset Types"));
    // -------------------------------------
    pWindow->dialog(MTDetailEdit::DetailEditTypeAsset, true);
    // -------------------------------------
    if (bStartingWithNone && (units.size() > 0))
    {
        std::string str_id = units.front().first;

        if (!str_id.empty())
        {
            std::string str_name = units.front().second;

            if (str_name.empty())
                str_name = str_id;
            // --------------------------------
            setField("AssetName", QString::fromStdString(str_name));
            setField("InstrumentDefinitionID",   QString::fromStdString(str_id));
            // --------------------------------
            ui->lineEditInstrumentDefinitionID->home(false);
        }
    }
    // -------------------------------------
    else if (units.size() < 1)
        SetAssetBlank();
}

void PageOffer_Assets::on_pushButtonManageCurrency_clicked()
{
    const auto & ot = Moneychanger::It()->OT();
    const auto reason = ot.Factory().PasswordPrompt(__FUNCTION__);

    MTDetailEdit * pWindow = new MTDetailEdit(this);

    pWindow->setAttribute(Qt::WA_DeleteOnClose);
    // -------------------------------------
    mapIDName & the_map = pWindow->m_map;
    // -------------------------------------
    the_map.clear();
    // -------------------------------------
    QString qstrPreselected   = field("CurrencyID").toString();
    bool    bFoundPreselected = false;
    // -------------------------------------
    const auto units = ot.Wallet().UnitDefinitionList();
    int32_t the_count = units.size();
    bool    bStartingWithNone = (the_count < 1);

    for (const auto& [id, name] : units)
    {
        QString OT_id   = QString::fromStdString(id);
        QString OT_name = QString::fromStdString(name);

        the_map.insert(OT_id, OT_name);

        if (!qstrPreselected.isEmpty() && (0 == qstrPreselected.compare(OT_id)))
            bFoundPreselected = true;
    } // for
    // -------------------------------------
    if (bFoundPreselected)
        pWindow->SetPreSelected(qstrPreselected);
    // -------------------------------------
    pWindow->setWindowTitle(tr("Manage Currency Types"));
    // -------------------------------------
    pWindow->dialog(MTDetailEdit::DetailEditTypeAsset, true);
    // -------------------------------------
    if (bStartingWithNone && units.size() > 0)
    {
        std::string str_id = units.front().first;

        if (!str_id.empty())
        {
            std::string str_name = units.front().second;

            if (str_name.empty())
                str_name = str_id;
            // --------------------------------
            setField("CurrencyName", QString::fromStdString(str_name));
            setField("CurrencyID",   QString::fromStdString(str_id));
            // --------------------------------
            ui->lineEditCurrencyID->home(false);
        }
    }
    // -------------------------------------
    else if (units.size() < 1)
        SetCurrencyBlank();
}

void PageOffer_Assets::on_pushButtonSelectAsset_clicked()
{
    const auto & ot = Moneychanger::It()->OT();
    const auto reason = ot.Factory().PasswordPrompt(__FUNCTION__);
    const auto units = ot.Wallet().UnitDefinitionList();
    // -------------------------------------------
    QString qstr_current_id = field("InstrumentDefinitionID").toString();
    // -------------------------------------------
    if (qstr_current_id.isEmpty() && (units.size() > 0))
        qstr_current_id = QString::fromStdString(units.front().first);
    // -------------------------------------------
    // Select from Asset Types in local wallet.
    //
    DlgChooser theChooser(this);
    // -----------------------------------------------
    mapIDName & the_map = theChooser.m_map;

    bool bFoundDefault = false;
    // -----------------------------------------------
    const int32_t the_count = units.size();
    // -----------------------------------------------
    for (const auto [id, name] : units)
    {
        QString OT_id = QString::fromStdString(id);
        QString OT_name = QString::fromStdString(name);
        const auto unitId = ot.Factory().UnitID(id);
        const auto unit = ot.Wallet().UnitDefinition(unitId, reason);
        // -----------------------------------------------
        if (!OT_id.isEmpty())
        {
            if (!qstr_current_id.isEmpty() && (0 == qstr_current_id.compare(OT_id)))
                bFoundDefault = true;
            // -----------------------------------------------
            OT_name = QString::fromStdString(unit->Alias());
            // -----------------------------------------------
            the_map.insert(OT_id, OT_name);
        }
     }
    // -----------------------------------------------
    if (bFoundDefault)
        theChooser.SetPreSelected(qstr_current_id);
    // -----------------------------------------------
    theChooser.setWindowTitle(tr("Select the Asset Type"));
    // -----------------------------------------------
    if (theChooser.exec() == QDialog::Accepted)
    {
        if (!theChooser.m_qstrCurrentID  .isEmpty() &&
            !theChooser.m_qstrCurrentName.isEmpty())
        {
            setField("AssetName", theChooser.m_qstrCurrentName);
            setField("InstrumentDefinitionID",   theChooser.m_qstrCurrentID);
            // -----------------------------------------
            ui->lineEditInstrumentDefinitionID->home(false);
            // -----------------------------------------
        }
    }
}

void PageOffer_Assets::on_pushButtonSelectCurrency_clicked()
{
    const auto & ot = Moneychanger::It()->OT();
    const auto reason = ot.Factory().PasswordPrompt(__FUNCTION__);
    const auto units = ot.Wallet().UnitDefinitionList();
    // -------------------------------------------
    QString qstr_current_id = field("CurrencyID").toString();
    // -------------------------------------------
    if (qstr_current_id.isEmpty() && (units.size() > 0))
        qstr_current_id = QString::fromStdString(units.front().first);
    // -------------------------------------------
    // Select from Asset Types in local wallet.
    //
    DlgChooser theChooser(this);
    // -----------------------------------------------
    mapIDName & the_map = theChooser.m_map;

    bool bFoundDefault = false;
    // -----------------------------------------------
    const int32_t the_count = units.size();
    // -----------------------------------------------
    for (const auto & [id, name] : units)
    {
        QString OT_id = QString::fromStdString(id);
        QString OT_name = QString::fromStdString(name);
        const auto unitId = ot.Factory().UnitID(id);
        const auto unit = ot.Wallet().UnitDefinition(unitId, reason);
        // -----------------------------------------------
        if (!OT_id.isEmpty())
        {
            if (!qstr_current_id.isEmpty() && (0 == qstr_current_id.compare(OT_id)))
                bFoundDefault = true;
            // -----------------------------------------------
            OT_name = QString::fromStdString(unit->Alias());
            // -----------------------------------------------
            the_map.insert(OT_id, OT_name);
        }
     }
    // -----------------------------------------------
    if (bFoundDefault)
        theChooser.SetPreSelected(qstr_current_id);
    // -----------------------------------------------
    theChooser.setWindowTitle(tr("Select the Currency"));
    // -----------------------------------------------
    if (theChooser.exec() == QDialog::Accepted)
    {
        if (!theChooser.m_qstrCurrentID  .isEmpty() &&
            !theChooser.m_qstrCurrentName.isEmpty())
        {
            setField("CurrencyName", theChooser.m_qstrCurrentName);
            setField("CurrencyID",   theChooser.m_qstrCurrentID);
            // -----------------------------------------
            ui->lineEditCurrencyID->home(false);
        }
    }
}


void PageOffer_Assets::SetAssetBlank()
{
    setField("AssetName", QString("<%1>").arg(tr("Click to choose Asset Type")));
    setField("InstrumentDefinitionID",   "");
}

void PageOffer_Assets::SetCurrencyBlank()
{
    setField("CurrencyName", QString("<%1>").arg(tr("Click to choose Currency")));
    setField("CurrencyID",   "");
}


void PageOffer_Assets::initializePage()
{
    if (!Moneychanger::It()->expertMode())
    {
        ui->pushButtonManageAsset->setVisible(false);
        ui->pushButtonManageCurrency->setVisible(false);
    }
    // -------------------------------------------
    const bool bIsBid = field("bid").toBool();

    if (bIsBid)
    {
        ui->labelAsset->   setText(QString("%1:").arg(tr("Buying Asset")));
        ui->labelCurrency->setText(QString("%1:").arg(tr("With Currency")));
    }
    else
    {
        ui->labelAsset->   setText(QString("%1:").arg(tr("Selling Asset")));
        ui->labelCurrency->setText(QString("%1:").arg(tr("For Currency")));
    }
}

PageOffer_Assets::~PageOffer_Assets()
{
    delete ui;
}
