cc_library(
    name = "gotw008-test-lib",
    srcs = ["test-main.cpp"],
    deps = [
      "//third-party:catch"
    ],
)

cc_test(
    name = "integer-stack-test",
    srcs = ["int-stack-test.cpp"],
    deps = [
        "gotw008-test-lib",
        "//src:gotw008-libs",
    ],
    timeout = "short"
)
