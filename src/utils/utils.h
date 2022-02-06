//
// Created by nic on 21/05/2021.
//

#pragma once

#ifndef CUBICAD_UTILS_H
#define CUBICAD_UTILS_H

#include <vector>
#include <boost/bimap.hpp>
#include <unordered_set>


//static inline constexpr uint32_t hash(const std::string_view data) noexcept;
//static inline constexpr uint32_t operator"" _hash(const char* str, size_t len) noexcept;

class Utils {
  public:
    static std::vector<char> readFile(const std::string &filename);

    static uint_fast8_t getMSB(uint_fast64_t x);

    template<typename L, typename R>
    static boost::bimap<L, R> makeBimap(std::initializer_list<typename boost::bimap<L, R>::value_type> list);

    template<typename type>
    static void remove(std::vector<type> &v);
};

template<typename L, typename R>
boost::bimap<L, R> Utils::makeBimap(std::initializer_list<typename boost::bimap<L, R>::value_type> list) {
    return boost::bimap<L, R>(list.begin(), list.end());
}

template<typename type>
void Utils::remove(std::vector<type> &v) {
    std::unordered_set<type> s;
    auto end = std::copy_if(v.begin(), v.end(), v.begin(), [&s](type const &i) {
      return s.insert(i).second;
    });

    v.erase(end, v.end());
}

constexpr uint32_t hash(const std::string_view data) noexcept {
    uint32_t hash = 5381;

    for (const char *c = data.data(); c < data.data() + data.size(); ++c)
        hash = ((hash << 5) + hash) + (unsigned char) *c;

    return hash;
}

constexpr uint32_t operator "" _hash(const char *str, size_t len) noexcept {
    return hash(std::string_view{str, len});
}

#endif //CUBICAD_UTILS_H
