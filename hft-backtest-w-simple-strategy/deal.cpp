//
// Created by Tim Gritsaev on 01.03.2021.
//

#include "backtest.h"

std::pair<int, order> class_backtest::exist_order(ll time) {
    int pos = get_pos(time);
    for (int id = 0; id <= 1; ++id) {
        for (int i = 0; i < order_book[index(id)][pos].size(); ++i) {
            if (order_book[index(id)][pos][i].my_volume > 0) {
                return {1, order_book[index(id)][pos][i]};
            }
        }
    }
    return {0, {}};
}

void class_backtest::add_limit_order(int id, ll time, ld volume, ld price) {
    int ind = index(id);
    int pos = get_pos(time);
    bool used = false;
    for (int i = 0; i < order_book[ind][pos].size(); ++i) {
        if (used) break;
        if (order_book[ind][pos][i].price == price) {
            order_book[ind][pos][i].prior_volume = order_book[ind][pos][i].volume;
            order_book[ind][pos][i].my_volume += volume;
            order_book[ind][pos][i].volume += volume;
            used = true;
        } else if (order_book[ind][pos][i].price * id > price * id) {
            order_book[ind][pos].insert(order_book[ind][pos].begin() + i,
                                        {price, volume, volume, 0});
            used = true;
        }  // ???
    }
    if (!used) {
        order_book[ind][pos].push_back({price, volume, volume, 0});
    }
}

void class_backtest::recalc_balance(int pos, int id, ld volume, ld price, bool market_order) {
    if (id == 1) { //  sell or ask
        bal[pos].money += volume * price * ((ld) 1 -
                                            (market_order ? commision_market : commission_limit));
        bal[pos].stock -= volume;
    } else {
        bal[pos].money -= volume * price * ((ld) 1 +
                                            (market_order ? commision_market : commission_limit));
        bal[pos].stock += volume;
    }
}

void class_backtest::make_trade_while_clear(int pos, order& ask, order& bid) {
    ld volume = std::min(bid.volume, ask.volume);
    ld now_price = 0;
    if (ask.my_volume != 0) {
        now_price = ask.price;

        ld min_my_volume = std::min(volume, ask.my_volume);
        recalc_balance(pos, 1, min_my_volume, now_price, 0);

        volume -= min_my_volume;
        ask.my_volume -= min_my_volume;
        ask.volume -= min_my_volume;
        bid.volume -= min_my_volume;
    } else if (bid.my_volume != 0) {
        now_price = bid.price;

        ld min_my_volume = std::min(volume, bid.my_volume);
        recalc_balance(pos, -1, min_my_volume, now_price, 0);

        volume -= min_my_volume;
        bid.my_volume -= min_my_volume;
        bid.volume -= min_my_volume;
        ask.volume -= min_my_volume;
    }

    bid.volume -= volume;
    ask.volume -= volume;
}

void class_backtest::clear_zero_orders(int pos_order_book) {
    for (int id = 0; id < 2; ++id) {
        while (order_book[id][pos_order_book].size() && order_book[id][pos_order_book][0].volume == 0) {
            order_book[id][pos_order_book].erase(order_book[id][pos_order_book].begin());
        }
    }
}

void class_backtest::clear_order_book(int pos_order_book) {
    clear_zero_orders(pos_order_book);
    bool del = true;
    while (del) {
        del = false;
        if (order_book[0][pos_order_book].size() == 0 || order_book[1][pos_order_book].size() == 0) break;
        if (order_book[1][pos_order_book][0].price <= order_book[0][pos_order_book][0].price) {
            make_trade_while_clear(pos_order_book,
                                   order_book[1][pos_order_book][0],
                                   order_book[0][pos_order_book][0]);
            del = true;
        }
        clear_zero_orders(pos_order_book);
    }
    clear_zero_orders(pos_order_book);
}

void class_backtest::make_trade(order& now_order, trade& now_trade, int pos, int id) {
    ld volume = std::min(now_trade.volume, now_order.volume);
    if (now_order.my_volume != 0) {
        ld min_my_volume = std::min(volume, now_order.my_volume);
        recalc_balance(pos, id, min_my_volume, now_trade.price, 0);

        volume -= min_my_volume;
        now_order.my_volume -= min_my_volume;
        now_order.volume -= min_my_volume;
        now_trade.volume -= min_my_volume;
    }

    now_order.volume -= volume;
    now_trade.volume -= volume;
}

void class_backtest::make_trade(int now_trade, int pos_order_book) {
    int id = (trades[now_trade].is_buyer_maker ? -1 : 1);
    for (int i = 0; i < order_book[index(id)][pos_order_book].size(); ++i) {
        if (trades[now_trade].volume == 0) break;

        make_trade(order_book[index(id)][pos_order_book][i], trades[now_trade], pos_order_book, id);
    }
    clear_order_book(pos_order_book);
}
