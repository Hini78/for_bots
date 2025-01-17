﻿#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>

#include "happyaccidentwindow.h"
using namespace std;


class Player {
public:

    string name;

    bool lost;
    int factories;
    int automated_factories;
    int factory_upgrade_month;

    int id;

    int raw_material;
    int products;
    int products_processing;

    int money;
    int credit;
    int insurance_months;
    int repayment;
    int turns_before_credit_end;
    Player(int id) : id(id), raw_material(2), products(2), money(10000), credit(0), insurance_months(0),lost(false),
        repayment(0), turns_before_credit_end(0), automated_factories(0), factories(2), products_processing(0), factory_upgrade_month(-1)
    {    }
    Player(){}
    std::string getInfo() const {
        std::string info = "Информация об игроке " + std::to_string(id) + ":\n";
        info += "Сырье: " + std::to_string(raw_material) + "\n";
        info += "Обычные фабрики: " + std::to_string(factories) + "\n";
        info += "Автоматизированные фабрики: " + std::to_string(automated_factories) + "\n";
        info += "Готовая продукция: " + std::to_string(products) + "\n";
        info += "Валюта: " + std::to_string(money) + "\n";
        info += "Кредит: " + std::to_string(credit) + "\n";
        info += "Месяцы до завершения \nулучшения фабрики: " + (factory_upgrade_month >= 0 ? std::to_string(factory_upgrade_month) : "не улучшается") + "\n";
        return info;
    }
};




class Bank {
private:
    int current_month;
    int raw_materials_for_sale;
    int products_for_sale;
    int raw_material_price;
    int product_price;
    int priority_player_id;


    std::map<int,Player> players;


    std::map<int, std::pair<int, int>> auction_buy_offers; // <player_id, <quantity, price>>
    std::map<int, std::pair<int, int>> auction_sell_offers; // <player_id, <quantity, price>>

public:
    Bank() :current_month(0), raw_materials_for_sale(0), products_for_sale(0), raw_material_price(0), product_price(0), priority_player_id(0) {
        bankSellOffer();
        bankBuyOffer();
    }
    void win(int id){
        players[id].money=100000;
    }
    void LoseMoney(int id){
        players[id].money=-100000;
    }
    int getRaw_material_price(){return raw_material_price;}
    int getProduct_price(){return product_price;}
    int getProducts_for_sale(){return products_for_sale;}

    int getRaw_materials_for_sale(){
        return raw_materials_for_sale;
    }

    int getCurrent_month() {
        return current_month;
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << current_month << "\n";
            file << raw_materials_for_sale << "\n";
            file << products_for_sale << "\n";
            file << raw_material_price << "\n";
            file << product_price << "\n";
            file << priority_player_id << "\n";

            file << players.size() << "\n";
            for (const auto& pair : players) {
                file << pair.first << "\n";
                const Player& player = pair.second;
                file << player.name << "\n";
                file << player.lost << "\n";
                file << player.factories << "\n";
                file << player.automated_factories << "\n";
                file << player.factory_upgrade_month << "\n";
                file << player.id << "\n";
                file << player.raw_material << "\n";
                file << player.products << "\n";
                file << player.products_processing << "\n";
                file << player.money << "\n";
                file << player.credit << "\n";
                file << player.insurance_months << "\n";
                file << player.repayment << "\n";
                file << player.turns_before_credit_end << "\n";
            }

            file << auction_buy_offers.size() << "\n";
            for (const auto& pair : auction_buy_offers) {
                file << pair.first << " " << pair.second.first << " " << pair.second.second << "\n";
            }

            file << auction_sell_offers.size() << "\n";
            for (const auto& pair : auction_sell_offers) {
                file << pair.first << " " << pair.second.first << " " << pair.second.second << "\n";
            }

            file.close();
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (file.is_open()) {
            file >> current_month;
            file >> raw_materials_for_sale;
            file >> products_for_sale;
            file >> raw_material_price;
            file >> product_price;
            file >> priority_player_id;

            size_t players_size;
            file >> players_size;
            players.clear();
            for (size_t i = 0; i < players_size; ++i) {
                int key;
                file >> key;

                Player player;
                file.ignore(); // Ignore newline character
                std::getline(file, player.name);
                file >> player.lost;
                file >> player.factories;
                file >> player.automated_factories;
                file >> player.factory_upgrade_month;
                file >> player.id;
                file >> player.raw_material;
                file >> player.products;
                file >> player.products_processing;
                file >> player.money;
                file >> player.credit;
                file >> player.insurance_months;
                file >> player.repayment;
                file >> player.turns_before_credit_end;

                players[key] = player;
            }

            size_t auction_buy_offers_size;
            file >> auction_buy_offers_size;
            auction_buy_offers.clear();
            for (size_t i = 0; i < auction_buy_offers_size; ++i) {
                int key, quantity, price;
                file >> key >> quantity >> price;
                auction_buy_offers[key] = std::make_pair(quantity, price);
            }

            size_t auction_sell_offers_size;
            file >> auction_sell_offers_size;
            auction_sell_offers.clear();
            for (size_t i = 0; i < auction_sell_offers_size; ++i) {
                int key, quantity, price;
                file >> key >> quantity >> price;
                auction_sell_offers[key] = std::make_pair(quantity, price);
            }

            file.close();
        }
    }
    std::map<int,Player> getPlayersMap(){
        return players;
    }
    void getPlayers() {
        for (auto player=players.begin();player!=players.end();player++) {
            std::cout << player->second.getInfo();
            std::cout << endl;
            std::cout << endl;
        }
    }

    void addPlayer(int id) {
        players[id]= Player(id);
    }

    void auctionBuyOffer(int player_id, int quantity, int price) {//Игрок делает запросы на покупку
        if (players[player_id].money > quantity * price) {
            auction_buy_offers[player_id] = std::make_pair(quantity, price);
        }
        else std::cout << "У игрока номер " << player_id << " недостаточно денег\n";

    }

    void auctionSellOffer(int player_id, int quantity, int price) {//Игрок делает запросы на продажу
        if (players[player_id].products >= quantity) {
            auction_sell_offers[player_id] = std::make_pair(quantity, price);
        }
        else {
            auction_sell_offers[player_id] = std::make_pair(players[player_id].products, price);
        }
    }


    void bankSellOffer() {
        raw_materials_for_sale = rand() % 10 + 1; // Случайное количество сырья от 1 до 10
        raw_material_price = rand() % 150 + 100; // Случайная цена от 100 до 249
        std::cout << "Банк предлагает " << raw_materials_for_sale << " шт. сырья по цене " << raw_material_price << " валюты за шт.\n";
    }

    void bankBuyOffer() {
        products_for_sale = rand() % 10 + 1; // Случайное количество продукции от 1 до 10
        product_price = rand() % 300 + 100; // Случайная цена от 100 до 399
        std::cout << "Банк готов купить " << products_for_sale << " шт. готовой продукции по цене " << product_price << " валюты за шт.\n";
    }

    bool playerLost(int player_id) {
        if (players[player_id].money < 0) return true;
        return false;
    }

    bool playerWon(int player_id) {
        if (players[player_id].money > 30000) return true;
        return false;
    }

    void processAuctions() {
        // Process buy offers
        while (raw_materials_for_sale > 0 && !auction_buy_offers.empty()) {

            auto best_buy_offer = std::min_element(auction_buy_offers.begin(), auction_buy_offers.end(),
                [this](const auto& a, const auto& b) {
                    if (a.second.second == b.second.second) {
                        if (a.first == priority_player_id) return true;
                        if (b.first == priority_player_id) return false;
                        return (a.first - priority_player_id) % players.size() < (b.first - priority_player_id) % players.size(); // Closest to priority player
                    }
                    return a.second.second > b.second.second; // Higher price is better
                });

            int player_id = best_buy_offer->first;
            int quantity = best_buy_offer->second.first;
            int price = best_buy_offer->second.second;

            if (quantity <= raw_materials_for_sale) {
                players[player_id].raw_material += quantity;
                players[player_id].money -= price * quantity;
                raw_materials_for_sale -= quantity;
                std::cout << "Player " << player_id << " bought " << quantity << " raw materials for " << price * quantity << " currency." << std::endl;
            }
            else if (raw_materials_for_sale>0){
                players[player_id].raw_material += raw_materials_for_sale;
                players[player_id].money -= price * raw_materials_for_sale;
                raw_materials_for_sale = 0;
                std::cout << "Player " << player_id << " bought " << raw_materials_for_sale << " raw materials for " << price * raw_materials_for_sale << " currency." << std::endl;
            }

            auction_buy_offers.erase(best_buy_offer->first);
        }

        // Process sell offers
        while (products_for_sale > 0 && !auction_sell_offers.empty()) {

        auto best_sell_offer = std::min_element(auction_sell_offers.begin(), auction_sell_offers.end(),
    [this](const auto& a, const auto& b) {
    if (a.second.second == b.second.second) {
    if (a.first == priority_player_id) return true;
    if (b.first == priority_player_id) return false;
    return (a.first - priority_player_id) % players.size() < (b.first - priority_player_id) % players.size(); // Closest to priority player
     }
     return a.second.second < b.second.second; // Lower price is better
            });

            int player_id = best_sell_offer->first;
            int quantity = best_sell_offer->second.first;
            int price = best_sell_offer->second.second;

            if (quantity <= products_for_sale) {
                players[player_id].products -= quantity;
                players[player_id].money += price * quantity;
                products_for_sale -= quantity;
                std::cout << "Player " << player_id << " sold " << quantity << " products for " << price * quantity << " currency." << std::endl;
            }
            else if (products_for_sale>0){
                players[player_id].products -= products_for_sale;
                players[player_id].money += price * products_for_sale;
                products_for_sale -= products_for_sale;
                std::cout << "Player " << player_id << " sold " << products_for_sale << " products for " << price * products_for_sale << " currency." << std::endl;
            }

            auction_sell_offers.erase(best_sell_offer);
        }

        auction_buy_offers.clear();
        auction_sell_offers.clear();
    }


    void makeMaterial(int player_id, int material) {
        if(material > 0) {
            if (material > players[player_id].raw_material) {
                material=players[player_id].raw_material;
            }

            int auto_processing = 0, default_processing = 0;
            int automated_factories_temp = players[player_id].automated_factories;

            while (automated_factories_temp > 0 && material > 0) {
                automated_factories_temp--;
                material -= 4;
                auto_processing += 4;
            }
            if (material < 0) {
                auto_processing += material;
            }
            players[player_id].money -= auto_processing * 20;




            int factories_temp = players[player_id].factories;

            while (factories_temp > 0 && material > 0) {
                factories_temp--;
                material -= 2;
                default_processing += 2;
            }
            if (material < 0) {
                default_processing += material;
            }
            players[player_id].money -= default_processing * 50;


            players[player_id].products_processing = default_processing + auto_processing;
            players[player_id].raw_material -= players[player_id].products_processing;
        }
    }

    void makeProducts(int player_id) {
        players[player_id].products += players[player_id].products_processing;
        players[player_id].products_processing = 0;
    }

    bool gameEnd() {
        if (players.size() < 2) return true;
        for (auto player : players) {
            if (playerWon(player.second.id)) return true;
        }
        return false;
    }


    void processTurn() {
        current_month++;

        //bool isRandom = true;
        bool start = false;
        for (auto player = players.begin(); player != players.end(); ) {
            if (start) {
                player = players.begin();
                start = false;
            }
            //qDebug()<<player->first<<'\n';
            // if (isRandom) {
            //     Chance happy = handleRandomEvent(player->second.id);

            //     if(happy != Chance::No_Event) {
            //         isRandom = false;

            //         HappyAccidentWindow* happyAccidentWindow = new HappyAccidentWindow();
            //         happyAccidentWindow->generateRandomAccident(happy);
            //         happyAccidentWindow->show();
            //     }

            //     if(happy == Chance::Skip) {
            //         continue;
            //     }
            // }

            if (player->second.turns_before_credit_end > 0) {
                //player.money -= player.repayment;
                //player.credit -= player.repayment;
                player->second.money -= player->second.credit / 12;
                player->second.turns_before_credit_end--;
                std::cout << "Player " << player->second.id << " repaid " << player->second.repayment << " currency of their credit. Remaining credit: " << player->second.credit << std::endl;
            }
            else if (player->second.turns_before_credit_end == 0) {
                player->second.credit = 0;
            }

            if (player->second.insurance_months > 0) {
                player->second.insurance_months--;
                std::cout << "Player " << player->second.id << " has " << player->second.insurance_months << " months of insurance left." << std::endl;
            }

            if (player->second.factory_upgrade_month == 0) {
                player->second.factory_upgrade_month=-1;
                player->second.money -= 1500;
                player->second.factories--;
                player->second.automated_factories++;
                std::cout << "Player " << player->second.id << " upgraded a factory to automated." << std::endl;
            }
            else if (player->second.factory_upgrade_month > 0) {
                player->second.factory_upgrade_month--;
            }

            chargeRent(player->second.id);

            makeProducts(player->first);

            if (playerWon(player->second.id)) {
                cout << "Player " << player->second.id << "won\n";
            }

            if (playerLost(player->second.id)) {
                cout << "Player " << player->second.id << " lost\n";
                player=players.erase(player);
            }
            else player++;
            //if (gameEnd())exit(0);


        }

        // Rotate priority player
        priority_player_id = (priority_player_id + 1) % players.size();
    }

    void grantCredit(int player_id, int amount) {
        if (players[player_id].credit > 0) {
            cout << "Player " << player_id << " has credit\n";

        }
        else if (players[player_id].money >= amount) {
            players[player_id].credit += amount * 1.1;
            players[player_id].money += amount;
            players[player_id].repayment = players[player_id].credit / 12;
            players[player_id].turns_before_credit_end = 12;
            std::cout << "Player " << player_id << " received a credit of " << amount << " currency. Total credit to repay: " << players[player_id].credit << std::endl;
        }
    }

    void insurancePayment(int player_id) {
        players[player_id].money -= 200;
        players[player_id].insurance_months++;
        std::cout << "Player " << player_id << " paid for insurance. Total insurance months: " << players[player_id].insurance_months << std::endl;
        
    }

    void upgradeFactory(int player_id) {
        if (players[player_id].money >= 1500 && players[player_id].factories > 0) {
            players[player_id].money -= 1500;
            players[player_id].factory_upgrade_month = 9;
            std::cout << "Player " << player_id << " started upgrading a factory. Completion in 9 months." << std::endl;
        }
    }

    void chargeRent(int player_id) {
        int rent = (players[player_id].raw_material * 20) + (players[player_id].products * 40);
        players[player_id].money -= rent;
        std::cout << "Player " << player_id << " paid rent of " << rent << " currency." << std::endl;
        if (players[player_id].money < 0) cout << "Player "<<player_id<<" was unable to pay rent\n";
    }

    Chance handleRandomEvent(int player_id) {
        srand(time(0));
        int event_chance = rand() % 4;
        if (event_chance == 0) {
            //cout << '\n\n\n' << "RANDOM" << '\n\n\n';
            int event_type = rand() % 5;
            int inheritance = 1000 + rand() % 9000;
            switch (event_type) {
            case 0:
                //std::cout << "Player " << player_id << " has a birthday. Other players must gift them." << std::endl;

                for (auto& player : players) {
                    if (player.second.id != player_id) {
                        if(player.second.insurance_months == 0) {
                            player.second.money -= 100;
                        }
                        players[player_id].money += 100;
                    }
                }

                return Chance::BirthDay;

                break;
            case 1:
                if (players[player_id].insurance_months == 0) {
                    //std::cout << "A factory burned down for player " << player_id << "." << std::endl;

                    if (players[player_id].factories > 0) {
                        players[player_id].factories--;
                        players[player_id].products_processing -= 2;
                        if(players[player_id].products_processing < 0) players[player_id].products_processing=0;
                    }
                    else if (players[player_id].automated_factories > 0) {
                        players[player_id].automated_factories--;
                        players[player_id].products_processing -= 4;
                        if(players[player_id].products_processing < 0) players[player_id].products_processing=0;
                    }
                    else {
                        players[player_id].money -= 15000;
                    }

                    return Chance::Fabric_burn;
                }
                else {
                    return Chance::Fabric_burn_not;
                }
                break;
            case 2:
                players[player_id].money += inheritance;

                //std::cout << "Player " << player_id << " received an inheritance of " << inheritance << " currency." << std::endl;

                return Chance::Inheritance;
                break;
            case 3:
                //std::cout << "Player " << player_id << " skip the move." << std::endl;

                return Chance::Skip;

                // Implement discount logic
                break;
            case 4:
                //std::cout << "Bank crisis! Player " << player_id << " loses 500 currency." << std::endl;
                //players[player_id].money -= 500;

                if (players[player_id].insurance_months == 0) {
                    for(auto& player : players) {
                        player.second.money -= 500;
                    }
                }
                else {
                    for(auto& player : players) {
                        if(player.first != player_id) {
                            player.second.money -= 500;
                        }
                    }
                }
                return Chance::Crisis;
                break;
            }
        }
        else {
            return Chance::No_Event;
        }
    }
};
