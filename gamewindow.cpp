#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "happyaccidentwindow.h"
#include <QShortcut>
#include "youlosewindow.h"
#include "youwinwindow.h"
#include "hideplayerstatswindow.h"
#include <string>

GameWindow::GameWindow(Game* _gameState, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameWindow)
{
    gameState=_gameState;
    ui->setupUi(this);
    setWindowTitle("Экономическая стратегия");

    QIcon economy_icon(":/source/economy_icon.png");
    setWindowIcon(economy_icon);


    bank=new Bank;
    for(int i =1;i<=_gameState->players;i++){
        bank->addPlayer(i);
    }
    displayBankStates();
    displayPlayerStates(bank->getPlayersMap()[1].getInfo());
    currentPlayerId=1;
    //bank->win(2);
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(shortcut, &QShortcut::activated, this, &GameWindow::on_nextTurnButton_clicked);

   // connect(ui->field_00, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

int roundNumber = 1;
int turnNumber = 1;

GameWindow::~GameWindow()
{
    delete ui;
}

void checkHappyAccident(){
    int happyNumber=rand()%10;
    if (happyNumber==0){
        HappyAccidentWindow* happyAccidentWindow = new HappyAccidentWindow();
        //happyAccidentWindow->generateRandomAccident();
        happyAccidentWindow->show();
    }
}

void GameWindow::on_nextTurnButton_clicked()
{

    bank->auctionBuyOffer(currentPlayerId,ui->buyMaterialsAmountChoice->text().toInt(),ui->buyMaterialsPriceChoice->text().toInt());
    bank->auctionSellOffer(currentPlayerId,ui->sellResourcesAmountChoice->text().toInt(),ui->sellProductPriceChoice->text().toInt());
    bank->makeMaterial(currentPlayerId, ui->spinBox->text().toInt());

    if(ui->checkBox->isChecked()){
        bank->upgradeFactory(currentPlayerId);
    }

    if(ui->lineEdit->text().length() != 0) {
        bank->grantCredit(currentPlayerId, ui->lineEdit->text().toInt());
    }

    if(ui->insuranceBox->isChecked()){
        bank->insurancePayment(currentPlayerId);
    }

    int maxId=-1;
    for (auto it:bank->getPlayersMap()){
        if(it.first>maxId) maxId=it.first;
    }
    if(currentPlayerId==maxId){

        bank->processAuctions();
        bank->processTurn();

        currentPlayerId=bank->getPlayersMap().begin()->first;
        roundNumber++;
        turnNumber=currentPlayerId;
        bank->saveToFile(gameState.login);
        displayBankStates();

    }
    else{
        bool next=false;
        for(auto it:bank->getPlayersMap()){
            if(next){
                currentPlayerId=it.first;
                break;
            }
            if(it.first==currentPlayerId) next = true;
        }
        turnNumber=currentPlayerId;
    }

    Chance happy = bank->handleRandomEvent(currentPlayerId);
    if(happy != Chance::No_Event) {
        HappyAccidentWindow* happyAccidentWindow = new HappyAccidentWindow();
        happyAccidentWindow->generateRandomAccident(happy, currentPlayerId);
        happyAccidentWindow->show();
    }
    if(happy == Chance::Skip) {
        int maxId=-1;
        for (auto it:bank->getPlayersMap()){
            if(it.first>maxId) maxId=it.first;
        }
        if(currentPlayerId==maxId){

            bank->processAuctions();
            bank->processTurn();

            currentPlayerId=bank->getPlayersMap().begin()->first;
            roundNumber++;
            turnNumber=currentPlayerId;

            displayBankStates();

        }
        else{
            bool next=false;
            for(auto it:bank->getPlayersMap()){
                if(next){
                    currentPlayerId=it.first;
                    break;
                }
                if(it.first==currentPlayerId) next = true;
            }
            turnNumber=currentPlayerId;
        }
    }

    ui->gamerNumber->setText("Игрок " + QString::number(currentPlayerId));
    ui->roundNumber->setText("Раунд " + QString::number(roundNumber));

    HidePlayerStatsWindow* hidePlayerStatsWindow = new HidePlayerStatsWindow();
    hidePlayerStatsWindow->show();

    displayPlayerStates(bank->getPlayersMap()[turnNumber].getInfo());

    ui->sellProductPriceChoice->setText("0");
    ui->sellResourcesAmountChoice->setValue(0);
    ui->buyMaterialsAmountChoice->setValue(0);
    ui->buyMaterialsPriceChoice->setText("0");

    ui->spinBox->setValue(0);


    ui->lineEdit->setText("0");
    ui->checkBox->setChecked(false);
    ui->insuranceBox->setChecked(false);

    if(bank->playerWon(currentPlayerId)){  //Проверка на победу надо добавить текст какой именно игрок победил + надо закрывать игру мб
        YouWinWindow* uwin = new YouWinWindow;
        uwin->show();
    }
    //Если остался 1 игрок то тоже вызывается окно победы
    // if(bank->getPlayersMap().size()==1){
    //     YouWinWindow* uwin = new YouWinWindow;
    //     uwin->show();
    // }
}

void GameWindow::on_giveUpButton_clicked()
{
    bank->LoseMoney(currentPlayerId);
    int maxId=-1;
    for (auto it:bank->getPlayersMap()){
        if(it.first>maxId) maxId=it.first;
    }
    if(currentPlayerId==maxId){

        bank->processAuctions();
        bank->processTurn();

        currentPlayerId=bank->getPlayersMap().begin()->first;
        roundNumber++;
        turnNumber=currentPlayerId;

        displayBankStates();

    }
    else{
        bool next=false;
        for(auto it:bank->getPlayersMap()){
            if(next){
                currentPlayerId=it.first;
                break;
            }
            if(it.first==currentPlayerId) next = true;
        }
        turnNumber=currentPlayerId;
    }

    Chance happy = bank->handleRandomEvent(currentPlayerId);
    if(happy != Chance::No_Event) {
        HappyAccidentWindow* happyAccidentWindow = new HappyAccidentWindow();
        happyAccidentWindow->generateRandomAccident(happy, currentPlayerId);
        happyAccidentWindow->show();
    }
    if(happy == Chance::Skip) {
        int maxId=-1;
        for (auto it:bank->getPlayersMap()){
            if(it.first>maxId) maxId=it.first;
        }
        if(currentPlayerId==maxId){

            bank->processAuctions();
            bank->processTurn();

            currentPlayerId=bank->getPlayersMap().begin()->first;
            roundNumber++;
            turnNumber=currentPlayerId;

            displayBankStates();

        }
        else{
            bool next=false;
            for(auto it:bank->getPlayersMap()){
                if(next){
                    currentPlayerId=it.first;
                    break;
                }
                if(it.first==currentPlayerId) next = true;
            }
            turnNumber=currentPlayerId;
        }
    }

    ui->gamerNumber->setText("Игрок " + QString::number(currentPlayerId));
    ui->roundNumber->setText("Раунд " + QString::number(roundNumber));

    HidePlayerStatsWindow* hidePlayerStatsWindow = new HidePlayerStatsWindow();
    hidePlayerStatsWindow->show();

    displayPlayerStates(bank->getPlayersMap()[turnNumber].getInfo());

    ui->sellProductPriceChoice->setText("0");
    ui->sellResourcesAmountChoice->setValue(0);
    ui->buyMaterialsAmountChoice->setValue(0);
    ui->buyMaterialsPriceChoice->setText("0");

    ui->spinBox->setValue(0);

    ui->lineEdit->setText("0");
    ui->checkBox->setChecked(false);
    ui->insuranceBox->setChecked(false);
    YouLoseWindow* youLoseWindow = new YouLoseWindow();
    youLoseWindow->show();
}

void GameWindow::displayBankStates(){
    bank->bankBuyOffer();
    bank->bankSellOffer();


    std::string bankStatesString="Продажа сырья: " + std::to_string(bank->getRaw_material_price())
                             + "\nКоличество сырья: " + std::to_string(bank->getRaw_materials_for_sale())
                             + "\nКупля ресурсов: " + std::to_string(bank->getProduct_price())
                             + "\nКоличество ресурсов: " + std::to_string(bank->getProducts_for_sale());

    QString bankStateDisplay = QString::fromStdString(bankStatesString);

    ui->bankStates->setText(bankStateDisplay);
}

void GameWindow::displayPlayerStates(std::string playerStateString){
    QString playerStateDisplay = QString::fromStdString(playerStateString);

    ui->playerStates->setText(playerStateDisplay);
}

GameWindow::GameWindow(Bank* _bank,Game _gameState):ui(new Ui::GameWindow){
    gameState = _gameState;
    bank = _bank;
    roundNumber = bank->getCurrent_month();
    ui->setupUi(this);
    setWindowTitle("Экономическая стратегия");

    QIcon economy_icon(":/source/economy_icon.png");
    setWindowIcon(economy_icon);

    displayBankStates();
    currentPlayerId=bank->getPlayersMap().begin()->first;
    displayPlayerStates(bank->getPlayersMap()[currentPlayerId].getInfo());

    ui->gamerNumber->setText("Игрок " + QString::number(currentPlayerId));
    ui->roundNumber->setText("Раунд " + QString::number(roundNumber));
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(shortcut, &QShortcut::activated, this, &GameWindow::on_nextTurnButton_clicked);
}
