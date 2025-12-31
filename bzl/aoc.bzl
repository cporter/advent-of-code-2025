
def aoc(day, linkopts = []):
    native.cc_binary(
        name = "day{}".format(day),
        srcs = ["day{}.cpp".format(day)],
        deps = ["//prelude:prelude", "@fmt//:fmt", "@spdlog//:spdlog"],
        linkopts = linkopts,
    )
