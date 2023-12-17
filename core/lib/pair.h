//
// Created by rplaz on 2023-12-16.
//

#ifndef PAIR_H
#define PAIR_H

namespace NycaTech {

#ifndef INLINE_LIB
#define INLINE_LIB inline
#endif

template <typename T1, typename T2>
struct Pair {
  T1 first;
  T2 second;
};

}

#endif //PAIR_H
