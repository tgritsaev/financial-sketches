//
// Created by Tim Gritsaev on 26.02.2021.
//

#pragma once

#include "struct_request.h"

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <fstream>

typedef long double ld;
typedef long long ll;

#include "balance.h"
#include "order.h"
#include "trade.h"

class request;

class class_backtest {
private:
public:

    static class_backtest& get_instance() {
        static class_backtest instance{};
        return instance;
    }

    static const long long TIME_START = 1603659600000, TIME_END = 1603663200000, TIME_FREQUENCY = 100;
    static const int CNT_TRADES = 16135;
    static const int N = (-TIME_START + TIME_END) / TIME_FREQUENCY + 1;

    balance bal[N];
    std::vector<order> order_book[2][N]; // 0 for bid    1 for ask

    trade trades[CNT_TRADES];
    int TRADE_POS = 0;

    int STRATEGY = 0;
    ld VOLUME_STRATEGY, DELTA_STRATEGY;
    ll LAST_ADD = 0, DONT_TOUCH = 100;

    std::ostream* OUT = &std::cout;

    ld MEAN_PRICE = 0;

    std::set<request> reqs;

    static int get_pos(ll time);

    static int index(int id);

    ll latency_limit, latency_market, latency_cancellation_order,
            call_frequency,
            commission_limit, commision_market;


    std::pair<int, order> exist_order(ll time);

    void destruct();

    void add_limit_order(int id, ll time, ld volume, ld price);

    void recalc_balance(int pos, int id, ld volume, ld price, bool market_order);

    void make_trade_while_clear(int pos, order& bid, order& ask);

    void clear_zero_orders(int pos_order_book);

    void clear_order_book(int pos_order_book);

    void make_trade(order& now_order, trade& now_trade, int pos, int id);

    void make_trade(int now_trade, int pos_order_book);

    void pass_through(ll time_start, ll time_end);

    void read_data();
    void read_var();

    bool read_request(ll time, std::set<request> &reqs);

    void call_backtest(int STRATEGY__);

    ld calc_mean_price();

    void mean_reversion_work(ll time);

    int mean_reversion_prediction(ll time);

    void precalc_strategy_mean_reversion();
};

