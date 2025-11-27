
def aoc(day):
    native.cc_binary(
        name = "day{}".format(day),
        srcs = ["day{}.cpp".format(day)],
        deps = ["//util:util"],
    )
