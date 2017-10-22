#include "third-party/catch.hpp"
#include "src/gotw008.hpp"

TEST_CASE("Stack Operation Tests", "[integer stack]") {
  auto roundTo2ToTheN = [](int N) {
    int result = 2;
    while (result < N) {
      result <<= 1;
    }
    return result;
  };

  // Default Setup code for the following small tests
  Stack<int> stack;
  REQUIRE(stack.Count() == 0);
  REQUIRE(stack.Capacity() == 0);

  SECTION("Push() and Pop() 1 integer") {
    stack.Push(3);
    REQUIRE(stack.Count() == 1);
    REQUIRE(stack.Capacity() == 1);
    REQUIRE(stack.Pop() == 3);
    REQUIRE(stack.Count() == 0);
    REQUIRE(stack.Capacity() == 1);
  }

  SECTION("Push() and Pop() 100 integer") {
    constexpr int N = 100;
    const auto expected_capacity = roundTo2ToTheN(N);
    for (int i = 0; i < N; ++i) {
      stack.Push(i);
      REQUIRE(stack.Count() == i + 1);
    }
    REQUIRE(stack.Capacity() == expected_capacity);
    REQUIRE(stack.Count() == N);
    for (int i = 0; i < N; ++i) {
      REQUIRE(stack.Pop() == N - 1 - i);
    }
    REQUIRE(stack.Capacity() == expected_capacity);
    REQUIRE(stack.Count() == 0);
  }

  SECTION("Copy constructor") {
    constexpr int N = 10;
    for (int i = 0; i < N; ++i)
      stack.Push(i);
    const auto expected_capacity = roundTo2ToTheN(N);
    REQUIRE(stack.Capacity() == expected_capacity);
    REQUIRE(stack.Count() == N);

    Stack<int> stack2(stack);
    REQUIRE(stack2.Capacity() == expected_capacity);
    REQUIRE(stack2.Count() == N);
    for (int i = N - 1; i >= 0; --i)
      REQUIRE(stack2.Pop() == i);
    REQUIRE(stack2.Capacity() == expected_capacity);
    REQUIRE(stack2.Count() == 0);
  }

  SECTION("Copy assignment") {
    constexpr int N = 10;
    for (int i = 0; i < N; ++i)
      stack.Push(i);

    SECTION("Self assignment has no effect") {
      stack = stack;
      REQUIRE(stack.Capacity() == roundTo2ToTheN(N));
      REQUIRE(stack.Count() == N);
      for (int i = N - 1; i >= 0; --i)
        REQUIRE(stack.Pop() == i);
    }

    SECTION("Assign a bigger stack to a smaller one") {
      // Construct stack2 and assign it back to stack
      Stack<int> stack2 = stack;
      constexpr int M = 500;
      for (int i = 0; i < M; ++i)
        stack2.Push(i);

      REQUIRE(stack2.Capacity() == roundTo2ToTheN(N + M));
      REQUIRE(stack2.Count() == M + N);

      stack = stack2;
      REQUIRE(stack.Capacity() == roundTo2ToTheN(N + M));
      REQUIRE(stack.Count() == M + N);

      for (int i = M - 1; i >= 0; --i)
        REQUIRE(stack2.Pop() == i);
      for (int i = N - 1; i != 0; --i)
        REQUIRE(stack2.Pop() == i);
    }
  }
}
