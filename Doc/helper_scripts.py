import math


def oscillator_freq_calc():
        # computed the possible division factors for the pll
    f_xosc = 12000000.

    f_ws2818 = 800000.

    mult_pio = 10
    pio0_fact = 1

    feedbck = 1
    postdiv1 = 1
    postdiv2 = 1

    f_sys = f_xosc
    params_array = []
    bestparams = None
    while f_sys < 133000000:
        f_sys = f_ws2818 * mult_pio * pio0_fact
        if f_sys > f_xosc:
            postdiv1 = 1
            postdiv2 = 1
            while postdiv1 < 8:
                postdiv2 = 1
                while postdiv2 < 8:
                    if f_sys*postdiv1*postdiv2 > 400000000 and f_sys*postdiv1*postdiv2 < 1600000000:
                        f_vco = f_sys*postdiv1*postdiv2
                        feedbk_f = f_vco / f_xosc
                        feedbk = math.floor(feedbk_f)
                        f_vco_appr = feedbk * f_xosc
                        f_sys_approx = f_vco_appr/postdiv1/postdiv2
                        f_ws2812_approx = f_sys_approx/mult_pio/pio0_fact
                        f_ws2812_err = math.fabs(f_sys - f_ws2812_approx)
                        params = {"feedbk": feedbk, "pio_clock": pio0_fact, "postdiv1": postdiv1,
                                  "postdiv2": postdiv2, "f_vco": f_vco_appr,"f_sys": f_vco_appr/postdiv1/postdiv2,
                                  "f_ws2812": f_ws2812_approx,
                                  "f_ws_2812_err": f_ws2812_err}
                        print("Feedback Multiplier: {}".format(params["feedbk"]))
                        print("PIO0 Clock Divider: {}".format(params["pio_clock"]))
                        print("Post Divider 1: {}".format(params["postdiv1"]))
                        print("Post Divider 2: {}".format(params["postdiv2"]))
                        print("VCO Frequency: {}".format(params["f_vco"]))
                        print("System Frequency: {}".format(params["f_sys"]))
                        print("ws2812 Frequency: {}".format(params["f_ws2812"]))
                        print("")
                        if bestparams is None:
                            bestparams = params
                        elif bestparams["f_ws_2812_err"] > params["f_ws_2812_err"]:
                            bestparams = params
                        params_array.append(params)
                    postdiv2 += 1
                postdiv1 += 1
        pio0_fact += 1
    print("\n\n\nBest Parameter Set")
    print("Feedback Multiplier: {}".format(bestparams["feedbk"]))
    print("PIO0 Clock Divider: {}".format(bestparams["pio_clock"]))
    print("Post Divider 1: {}".format(bestparams["postdiv1"]))
    print("Post Divider 2: {}".format(bestparams["postdiv2"]))
    print("VCO Frequency: {}".format(bestparams["f_vco"]))
    print("System Frequency: {}".format(bestparams["f_sys"]))
    print("ws2812 Frequency: {}".format(bestparams["f_ws2812"]))


def st7735_encode_color(r,g,b):
    msb = (r & 0xF8) | ((g >> 5) & 0x7)
    lsb = ((g & 0x7) << 5) | ((b & 0xF8) >> 3)
    print("encoded color, msb: {}, lsb: {}".format(hex(msb),hex(lsb)))


if __name__ == "__main__":
    st7735_encode_color(254, 255, 179)