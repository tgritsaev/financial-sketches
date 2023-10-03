//
// Created by Tim Gritsaev on 28.02.2021.
//

#pragma once

#include <string>
#include <set>
#include "order.h"
#include "backtest.h"

typedef long double ld;
typedef long long ll;

class request {
private:
public:

    ll time;
    std::string s;
    ld volume, price;
    int id;

    int get_pos(ll time);

    int index(int id);

    bool operator<(const request &s) const;

    void market_order();

    void print_balance();

    void limit_order();

    std::pair<bool, order> exist_order();

    bool delete_order();

    bool read(ll now_time, std::set<request>& reqs);

    void process();

    void recalc_balance(int pos, int id, ld volume, ld price, bool market_order);
};
