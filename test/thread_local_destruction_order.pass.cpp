//===-------------- thread_local_destruction_order.pass.cpp ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03

#include <cassert>
#include <thread>

int seq = 0;

class OrderChecker {
public:
  explicit OrderChecker(int n) : n_{n} { }

  ~OrderChecker() {
    assert(seq++ == n_);
  }

private:
  int n_;
};

template <int ID>
class CreatesThreadLocalInDestructor {
public:
  ~CreatesThreadLocalInDestructor() {
    thread_local OrderChecker checker{ID};
  }
};

OrderChecker global{7};

void thread_fn() {
  static OrderChecker fn_static{5};
  thread_local CreatesThreadLocalInDestructor<2> creates_tl2;
  thread_local OrderChecker fn_thread_local{1};
  thread_local CreatesThreadLocalInDestructor<0> creates_tl0;
}

int main() {
  static OrderChecker fn_static{6};

  std::thread{thread_fn}.join();
  assert(seq == 3);

  thread_local OrderChecker fn_thread_local{4};
  thread_local CreatesThreadLocalInDestructor<3> creates_tl;

  return 0;
}
