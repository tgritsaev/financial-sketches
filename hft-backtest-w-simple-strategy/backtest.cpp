//
// Created by Tim Gritsaev on 01.03.2021.
//

#include <iostream>
#include "backtest.h"
#include "trade.h"

std::ifstream other_read("../input.txt");

int class_backtest::get_pos(ll time) {
    return (time - TIME_START) / TIME_FREQUENCY;
}

int class_backtest::index(int id) {
    return (id == -1 ? 0 : 1);
}

void class_backtest::read_data() {
    LAST_ADD = 0;

    std::ifstream file_trade("../trades_eth_test.txt");
    int n;
    file_trade >> n;
    for (int i = 0; i < n; ++i) {
        file_trade >> trades[i].time >> trades[i].price >> trades[i].volume >> trades[i].is_buyer_maker;
    }
    file_trade.close();

    std::ifstream file_order("../orders_eth_depth50_test.txt");
    int m;
    file_order >> m;
    for (int i = 0; i < m; ++i) {
        order_book[0][i].clear();
        order_book[1][i].clear();
        for (int j = 0; j < 50; ++j) {
            order ask;
            file_order >> ask.price >> ask.volume;
            order_book[1][i].push_back(ask);
        }
        for (int j = 0; j < 50; ++j) {
            order bid;
            file_order >> bid.price >> bid.volume;
            order_book[0][i].push_back(bid);
        }
    }
    file_order.close();
}

void class_backtest::read_var() {
    std::cout << "чтобы разобраться как работать с бэктестом - прочтите READ_ME.txt\n\n";

    std::cout << "задержка на лимитный ордер (кратная 100 мс):\n";
    other_read >> latency_limit;  // 100 for test
    std::cout << "задержка на маркет ордер (кратная 100 мс):\n";
    other_read >> latency_market;  // 100 for test
    std::cout << "задержка на отмену лимитного ордера (удаление с биржи) (кратная 100 мс):\n";
    other_read >> latency_cancellation_order;  // 100 for test
    std::cout << "частота вызова интерфейса пользователя бектеста (кратная 100 мс):\n";
    other_read >> call_frequency;  // 100 for test
    std::cout << "комиссия на лимитный ордер (процент):\n";
    other_read >> commission_limit;  // 0.001 for test
    std::cout << "комиссия на маркет-ордер (процент):\n";
    other_read >> commision_market;  // 0.001 for test
}

bool class_backtest::read_request(ll time, std::set<request> &reqs) {
    std::string s;
    std::cin >> s;
    if (s == "stop" || s == "5") return 0;

    request nw_req;
    nw_req.s = s;
    return nw_req.read(time, reqs);
}

void class_backtest::pass_through(ll time_start, ll time_end) {
    int pos = get_pos(time_start);
    for (ll time = time_start; time <= time_end; ++time) {
        while (TRADE_POS < CNT_TRADES && trades[TRADE_POS].time == time) {
            make_trade(TRADE_POS++, pos);
        }
    }

    bal[get_pos(time_end)] = bal[get_pos(time_start)];
    std::pair<int, order> my_limit_order = exist_order(time_start);
    if (my_limit_order.first) {
        add_limit_order(my_limit_order.first, time_end,
                        my_limit_order.second.my_volume, my_limit_order.second.price);
    }

}

void class_backtest::call_backtest(int STRATEGY__) {
    STRATEGY = STRATEGY__;
    read_data();
    read_var();

    if (STRATEGY == 2) {
        precalc_strategy_mean_reversion();
    }

    ll time = TIME_START;

    while (time < TIME_END) {
        clear_order_book(get_pos(time));
        if ((time - TIME_START) % call_frequency == 0) {
            *class_backtest::get_instance().OUT << "следует отправлять запросы:\n";
            if (STRATEGY == 0) {
                while (read_request(time, reqs));
            } else if (STRATEGY == 2) {
                mean_reversion_work(time);
            }
            while (reqs.size() && (*reqs.begin()).time <= time) {
                request now_req = *reqs.begin();
                reqs.erase(reqs.begin());
                now_req.process();
            }
        }

        pass_through(time, time + 100);
        time += 100;
    }
}
