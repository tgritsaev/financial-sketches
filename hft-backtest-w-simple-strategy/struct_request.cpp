//
// Created by Tim Gritsaev on 01.03.2021.
//

//
// Created by Tim Gritsaev on 28.02.2021.
//

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <fstream>

typedef long double ld;
typedef long long ll;
#include "struct_request.h"

int request::get_pos(ll time) {
    return (time - class_backtest::get_instance().TIME_START) / class_backtest::get_instance().TIME_FREQUENCY;
}

int request::index(int id) {
    return (id == -1 ? 0 : 1);
}

int get_priority(const std::string& s) {
    if (s == "delete")
        return 1;
    if (s == "ask" || s == "sell" || s == "buy" || s == "bid")
        return 2;
    if (s == "order_ask")
        return 3;
    if (s == "balance")
        return 4;
    return 5;
}

bool request::operator<(const request &s) const {
    if (time == s.time) {
        return get_priority(this->s) < get_priority(s.s);
    }
    return time < s.time;
}

void request::market_order() {
    int pos = get_pos(time);
    ld was_stock = class_backtest::get_instance().bal[pos].stock, was_money = class_backtest::get_instance().bal[pos].money;
    for (const order& a : class_backtest::get_instance().order_book[index(id)][pos]) {
        if (volume == 0) break;
        ld now_bought = std::min(volume, a.volume);
        recalc_balance(pos, id, now_bought, a.price, 1);

        volume -= now_bought;
    }
    if (volume > 0) {
        *class_backtest::get_instance().OUT << "Маркет ордер слишком большой и его нельзя закрыть мгновенно\n";
    }
    if (id == 1) {
        *class_backtest::get_instance().OUT << "Было куплено бумаг обьемом " << abs(was_stock - class_backtest::get_instance().bal[pos].stock)
                  << " и потрачено денег " << abs(was_money - class_backtest::get_instance().bal[pos].money) << '\n';
    } else {
        *class_backtest::get_instance().OUT << "Было продано бумаг обьемом " << abs(was_stock - class_backtest::get_instance().bal[pos].stock)
                  << " и получено денег " << abs(was_money - class_backtest::get_instance().bal[pos].money) << '\n';
    }
}

void request::limit_order() {
    int pos = get_pos(time);
    class_backtest::get_instance().add_limit_order(id, time, volume, price);

    if (id == 1) {
        *class_backtest::get_instance().OUT << "Зарегистрирован ask обьемом " << volume << " с установленной ценой " << price << '\n';
    } else {
        *class_backtest::get_instance().OUT << "Зарегистрирован bid обьемом " << volume << " с установленной ценой " << price << '\n';
    }
    class_backtest::get_instance().get_instance().clear_order_book(pos);
}

std::pair<bool, order> request::exist_order() {
    int pos = get_pos(time);
    for (int ind = 0; ind <= 1; ++ind) {
        for (int i = 0; i < class_backtest::get_instance().order_book[ind][pos].size(); ++i) {
            if (class_backtest::get_instance().order_book[ind][pos][i].my_volume > 0) {
                *class_backtest::get_instance().OUT
                << "Найден " << (ind == 0 ? "bid" : "ask") << " ордер обьемом " <<
                class_backtest::get_instance().order_book[ind][pos][i].my_volume << " и ценой " <<
                class_backtest::get_instance().order_book[ind][pos][i].price << '\n';
                return {1, class_backtest::get_instance().order_book[ind][pos][i]};
            }
        }
    }
    *class_backtest::get_instance().OUT << "В данный момент лимит-ордеров не найдено\n";
    return {0, {}};
}

bool request::delete_order() {
    int pos = get_pos(time);
    for (int ind = 0; ind <= 1; ++ind) {
        for (int i = 0; i < class_backtest::get_instance().order_book[ind][pos].size(); ++i) {
            if (class_backtest::get_instance().order_book[ind][pos][i].my_volume > 0) {
                *class_backtest::get_instance().OUT << "Найден " << (ind == 0 ? "bid" : "ask") << " ордер обьемом " <<
                class_backtest::get_instance().order_book[ind][pos][i].my_volume << " и ценой " <<
                class_backtest::get_instance().order_book[ind][pos][i].price <<
                          ". Ордер удален.";

                class_backtest::get_instance().order_book[ind][pos][i].volume -=
                        class_backtest::get_instance().order_book[ind][pos][i].my_volume;
                class_backtest::get_instance().order_book[ind][pos][i].prior_volume = 0;
                class_backtest::get_instance().order_book[ind][pos][i].my_volume = 0;

                return 1;
            }
        }
    }
    *class_backtest::get_instance().OUT << "Лимит-ордер не был найден\n";
    return 0;
}

bool request::read(ll now_time, std::set<request> &reqs) {
    if (s == "stop") return 0;

    if (s == "buy" || s == "1/0") {
        std::cin >> volume;
        time = now_time + class_backtest::get_instance().latency_market;
        id = -1;
    } else if (s == "sell" || s == "1/1") {
        std::cin >> volume;
        time = now_time + class_backtest::get_instance().latency_market;
        id = 1;
    } else if (s == "ask" || s == "2/1") {
        std::cin >> price >> volume;
        time = now_time + class_backtest::get_instance().latency_limit;
        id = 1;
    } else if (s == "bid" || s == "2/0") {
        std::cin >> price >> volume;
        time = now_time + class_backtest::get_instance().latency_limit;
        id = -1;
    } else if (s == "exist_order" || s == "3") {
        std::cin >> time;
    } else if (s == "delete" || s == "4") {
        time = now_time + class_backtest::get_instance().latency_cancellation_order;
    }
    reqs.insert(*this);
    return 1;
}

void request::print_balance() {
    int pos = get_pos(time);

    ld PRICE__ =
            (class_backtest::get_instance().order_book[0][pos][0].price
            +
            class_backtest::get_instance().order_book[1][pos][0].price) / 2;
    ld VAL_1 = (class_backtest::get_instance().bal[pos].money + class_backtest::get_instance().bal[pos].stock
                                                                     * PRICE__);
    ld VAL_2 = (class_backtest::get_instance().bal[pos].money + class_backtest::get_instance().bal[pos].stock
                                                                * class_backtest::get_instance().MEAN_PRICE);
    *class_backtest::get_instance().OUT << "Денег: " << class_backtest::get_instance().bal[pos].money << ". Стоков: " <<
              class_backtest::get_instance().bal[pos].stock <<
              ". Если продать все стоки по нынешней средне-спредовой цене: " <<
              VAL_1 << " и если по средней цене за весь час: " <<
              VAL_2 << '\n';

}

void request::process() {
    if (s == "buy" || s == "1/0") {
        id = -1;
        market_order();
    } else if (s == "sell" || s == "1/1") {
        id = 1;
        market_order();
    } else if (s == "ask" || s == "2/1") {
        id = 1;
        limit_order();
    } else if (s == "bid" || s == "2/0") {
        id = -1;
        limit_order();
    } else if (s == "exist_order" || s == "3") {
        exist_order();
    } else if (s == "delete" || s == "4") {
        delete_order();
    } else if (s == "balance" || s == "5") {
        print_balance();
    }
}

void request::recalc_balance(int pos, int id, ld volume, ld price, bool market_order) {
    if (id == 1) { //  sell or ask
        class_backtest::get_instance().bal[pos].money += volume * price * ((ld) 1 -
                                            (market_order ?
                                            class_backtest::get_instance().commision_market :
                                            class_backtest::get_instance().commission_limit));
        class_backtest::get_instance().bal[pos].stock -= volume;
    } else {
        class_backtest::get_instance().bal[pos].money -= volume * price * ((ld) 1 +
                                            (market_order ?
                                             class_backtest::get_instance().commision_market :
                                             class_backtest::get_instance().commission_limit));
        class_backtest::get_instance().bal[pos].stock += volume;
    }
}