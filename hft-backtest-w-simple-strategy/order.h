//
// Created by Tim Gritsaev on 01.03.2021.
//

#ifndef A_CPP_ORDER_H
#define A_CPP_ORDER_H

typedef long double ld;

struct order {
    ld price, volume;
    ld my_volume = 0;
    ld prior_volume = 0;
};


#endif //A_CPP_ORDER_H
