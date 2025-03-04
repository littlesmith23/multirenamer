//
// Created by stefan on 04.03.25.
//

#pragma once
namespace littlesmith {
    template <typename Int>
    Int shift_right(Int x, unsigned int n) { return x >> n; }
    template <typename Int>
    Int shift_left(Int x, unsigned int n) { return x << n; }
    template <typename Int>
    Int rotate_right(Int x, unsigned int n) { return shift_right(x, n) | (x << ((sizeof(x) << 3) - n)); }
    template <typename Int>
    Int rotate_left(Int x, unsigned int n) { return shift_left(x, n) | (x >> ((sizeof(x) << 3) - n)); }
    template <typename Int>
    Int choose(Int x, Int y, Int z) { return (x & y) ^ (~x  & z); }
    template <typename Int>
    Int majority(Int x, Int y, Int z) { return (x & y) ^ (x & z) ^ (y & z); }
}