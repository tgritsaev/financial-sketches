#include <iostream>
#include <fstream>
#include "backtest.h"
#include "test.h"
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <fstream>

typedef long double ld;
typedef long long ll;

const bool TESTING_MY_OWN = false;
const int TEST_STRATEGY = 0;

int main() {

    if (TESTING_MY_OWN) {
        freopen("test_value.txt", "r", stdin);
    }

    if (TESTING_MY_OWN) {
        fclose(stdin);
    }

    if (TESTING_MY_OWN) {
        test();
    } else if (TEST_STRATEGY == 2) {
        for (ld VOLUME = 12.5; VOLUME >= 7.5; VOLUME -= 2.5) {
            for (ld DELTA = 0.1; DELTA <= 0.16; DELTA += 0.025) {
                std::string file_name = "../output_dir/output_delta=" + std::to_string(DELTA) +
                                        "_volume=" + std::to_string(VOLUME) + ".txt";

                std::ofstream out(file_name.c_str());
                class_backtest::get_instance().OUT = &out;
                class_backtest::get_instance().VOLUME_STRATEGY = VOLUME,
                class_backtest::get_instance().DELTA_STRATEGY = DELTA;
                class_backtest::get_instance().call_backtest(TEST_STRATEGY);

                request x;
                x.time = class_backtest::get_instance().TIME_END;
                x.s = "balance";
                x.print_balance();

                std::cout << "работа с обьемом " << VOLUME << " и дельтой " << DELTA << " сделана\n";
            }
        }
    } else {
        class_backtest::get_instance().call_backtest(TEST_STRATEGY);
    }
    return 0;
}
