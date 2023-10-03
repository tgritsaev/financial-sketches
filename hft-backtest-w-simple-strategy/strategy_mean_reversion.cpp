//
// Created by Tim Gritsaev on 01.03.2021.
//

#include "backtest.h"
#include <string>

ld class_backtest::calc_mean_price() {
    ld mean_price = 0;
    for (int i = 0; i < N; ++i) {
        mean_price += (order_book[0][i][0].price + order_book[1][i][0].price) / 2;
    }
    return mean_price / N;
}

void class_backtest::precalc_strategy_mean_reversion() {
    MEAN_PRICE = calc_mean_price();
}

int class_backtest::mean_reversion_prediction(ll time) {
    int pos = get_pos(time);
    ld nw_price = (order_book[0][pos][0].price + order_book[1][pos][0].price) / 2;
    if (nw_price >= MEAN_PRICE + DELTA_STRATEGY && time - LAST_ADD >= DONT_TOUCH * 100) {
        LAST_ADD = time;
        return 1;
    } else if (nw_price <= MEAN_PRICE - DELTA_STRATEGY && time - LAST_ADD >= DONT_TOUCH * 100){
        LAST_ADD = time;
        return -1;
    }
    return 0;
}

void class_backtest::mean_reversion_work(ll time) {

    int prediction = mean_reversion_prediction(time);
    if (prediction != 0) {
        int pos = get_pos(time);
        int other = (prediction == 1 ? 0 : 1);
        reqs.clear();
        reqs.insert(
        request({time + latency_limit, "delete" })
        );
        request order = request({time + std::max(latency_cancellation_order, latency_limit),
                                 (prediction == 1 ? "ask" : "bid"),
                                 VOLUME_STRATEGY,
                                 order_book[other][pos][0].price});
        reqs.insert(order);
        reqs.insert(request({time + std::max(latency_cancellation_order, latency_limit), "balance"}));
    }
}