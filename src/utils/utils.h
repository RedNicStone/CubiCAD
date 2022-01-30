//
// Created by nic on 21/05/2021.
//

#pragma once

#ifndef CUBICAD_UTILS_H
#define CUBICAD_UTILS_H

#include <vector>
#include <boost/bimap.hpp>
#include <unordered_set>


class Utils {
  public:
    static std::vector<char> readFile(const std::string &filename);

    static uint_fast8_t getMSB(uint_fast64_t x);

    template <typename L, typename R>
    static boost::bimap<L, R> makeBimap(std::initializer_list<typename boost::bimap<L, R>::value_type> list);

    template<typename type>
    static void remove(std::vector<type> &v);
};

#endif //CUBICAD_UTILS_H
