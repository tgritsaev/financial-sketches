//
// Created by Tim Gritsaev on 01.03.2021.
//

#ifndef A_CPP_TRADE_H
#define A_CPP_TRADE_H

typedef long double ld;
typedef long long ll;

struct trade {
    ld price, volume;
    ll time;
    bool is_buyer_maker;
};

#endif //A_CPP_TRADE_H
