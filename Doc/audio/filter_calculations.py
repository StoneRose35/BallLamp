import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec
import math


def int32(x):
    xint=x & 0xFFFFFFFF
    if xint > (1 << 31):
        xint = xint - (1 << 32)
    return xint


class AudioFilter:
    def __init__(self,a,b):
        self.a=a
        self.b=b
        self.w=[int(0),int(0),int(0)]

    def apply(self, sample):
        out = 0
        self.w[0] = int32(int(sample) - (int32(self.a[0]*self.w[1]) >> 15) - (int32(self.a[1]*self.w[2]) >> 15))
        out = int32(int32((int32(self.b[0]*self.w[0])>> 15)) + int32((int32(self.b[1]*self.w[1]) >> 15)) + int32((int32(self.b[2]*self.w[2]) >> 15) ))
        self.w[2]=self.w[1]
        self.w[1]=self.w[0]
        return out

def compute_td_energy_fraction(bd_orig,ad_orig,sample_size=16,scaling=1.0):
    bd = bd_orig * scaling
    ad = ad_orig * scaling
    bvals = np.array(bd * ((1 << (sample_size - 1)) - 1)).astype(int)
    avals = np.array(ad[1:] * ((1 << (sample_size - 1)) - 1)).astype(int)
    testFilter = AudioFilter(avals, bvals)
    tdOutput = []
    max_ampl = (1 << (sample_size-1)) -1
    for c in range(1024):
        if c == 0:
            inputVal = max_ampl
        else:
            inputVal = 0
        tdOutput.append(testFilter.apply(inputVal))
    energy = np.sqrt(np.sum(np.square(tdOutput))) * scaling
    if scaling > 0.000001:
        return energy/max_ampl/scaling
    else:
        return 0.

def get_phase_increments(f_sampling=48000,oversampling=2,bitres=32):
    notes = range(128)
    fs=f_sampling*oversampling
    phaseincs=[]
    f_synths = []
    for note in notes:
        notefreq = math.pow(2.,(note-64)/12.)*440.
        phaseinc = notefreq/fs*((1 << bitres))
        f_synth = int(phaseinc)*fs/((1 << bitres))
        f_synths.append(f_synth)
        if (phaseinc > (1 << (bitres-1))):
            print("note {} unavailable".format(note))
        else:
            phaseincs.append(int(phaseinc))
            #print("{},{}, {}, {}".format(note, notefreq, int(phaseinc),f_synth))
            #print("{},".format(int(phaseinc)))
    return phaseincs, f_synths

def get_sine_table():
    bitres = 16
    nvals = 256
    sinetable=[]
    for c in range(nvals):
        print("{},".format(int(math.cos(c/nvals*math.pi*2)*((1 << (bitres-1)) - 1))))
        sinetable.append(int(math.cos(c/nvals*math.pi*2)*((1 << (bitres-1)) - 1)))
    return sinetable


def design_and_plot_iir_filter(do_plot=True,rs=20,fc=None,fs=48000,scaling=None,type="cheby2",ftype="lowpass"):
    if fc is None:
        fc = fs/2
    order = 2
    sample_size = 16
    auto_scaling = scaling is None
    if type == "cheby2":
        sos = signal.cheby2(order,rs, fc, analog=False, btype=ftype, output='sos', fs=fs)
    elif type == "cheby1":
        sos = signal.cheby1(order, rs, fc, analog=False, btype=ftype, output='sos', fs=fs)
    elif type == "butter":
        sos = signal.butter(order, fc, analog=False, btype=ftype, output='sos', fs=fs)
    else:
        sos= [[1.,0.,0.], [1.,0.,0.]]

    cnt=0
    sos_returned = []
    for second_order_filter in sos:
        bd_orig = second_order_filter[:3]
        ad_orig = second_order_filter[3:]

        if auto_scaling is True:
            is_done = False
            scaling = 1.
            scaling_low = 0.
            scaling_eps=0.0001
            while is_done is False:
                energy_fraction = compute_td_energy_fraction(bd_orig,ad_orig,sample_size,scaling)
                energy_fraction_l = compute_td_energy_fraction(bd_orig,ad_orig,sample_size,scaling_low)
                print("upper scaling: {:.4f}, energy fraction: {:.5f}, lower scaling factor: {:.4f}, energy fraction: {:.5f}".format(scaling,energy_fraction,scaling_low,energy_fraction_l))
                if (energy_fraction > 1.0 and energy_fraction_l < 1.0):
                    scaling = scaling +0.5*(scaling_low - scaling)
                elif energy_fraction < 1.0 and energy_fraction_l < 1.0:
                    scaling_old =scaling
                    scaling = scaling + (scaling-scaling_low)
                    scaling_low = scaling_old
                    if scaling > 1.0:
                        scaling=1.0
                        is_done = True
                elif energy_fraction < 1.0 and energy_fraction_l > 1.0:
                    scaling = scaling + 0.5 * (scaling - scaling_low)
                elif energy_fraction > 1.0 and energy_fraction_l > 1.0:
                    scaling_old = scaling_low
                    scaling_low = scaling_low + (scaling - scaling_low)
                    scaling = scaling_old
                    if scaling_low < 0:
                        scaling_low = 0
                if (scaling - scaling_low) < scaling_eps:
                    is_done = True
                    bd = bd_orig * scaling_low
                    ad = ad_orig * scaling_low
                    bvals = np.array(bd * ((1 << (sample_size - 1)) - 1)).astype(int)
                    avals = np.array(ad[1:] * ((1 << (sample_size - 1)) - 1)).astype(int)
                    scaling = scaling_low
        else:
            bd = bd_orig * scaling
            ad = ad_orig * scaling
            bvals = np.array(bd * ((1 << (sample_size - 1)) - 1)).astype(int)
            avals = np.array(ad[1:] * ((1 << (sample_size - 1)) - 1)).astype(int)
        if do_plot is True:
            print("B coefficients: {}".format(bvals))
            print("A coefficients: {}".format(avals))
            print("post gain factor: {:.6f}".format(1./scaling))

            wz, hz = signal.freqz(bd, ad)
            fig = plt.figure(figsize=[7.2,7.2],constrained_layout=True)
            gs = GridSpec(3,2,figure=fig)

            axs0 = fig.add_subplot(gs[0,0])
            freqs = wz*fs/2./np.pi
            axs0.plot(freqs,20.*np.log10(abs(hz)),linewidth=2,color="blue")
            axs0.grid(color="gray",linestyle="--",which="major",axis="both")
            axs0.grid(color="gray", linestyle="--", which="minor", axis="x")
            axs0.axis([1,fs/2,-60,10])
            axs0.set_xscale("log")
            axs0.set_title("Frequency Response")
            axs0.set_ylabel("Gain [dB]")
            axs0.set_xlabel("Frequency [Hz]")

            axs1 = fig.add_subplot(gs[1, 0])
            axs1.plot(freqs,np.unwrap(np.arctan2(np.real(hz),np.imag(hz)))*180./np.pi,linewidth=2,color="blue")
            axs1.grid(color="gray",linestyle="--",which="major",axis="both")
            axs1.grid(color="gray", linestyle="--", which="minor", axis="x")
            axs1.set_xlim([1,fs/2])
            axs1.set_xscale("log")
            axs1.set_title("Phase response")
            axs1.set_ylabel("Phase [deg]")
            axs1.set_xlabel("Frequency [Hz]")

            # time-domain test
            x_time_domain = np.linspace(0,1./fs*1000,1024)
            testFilter = AudioFilter(avals,bvals)
            tdOutput=[]
            for c in range(1024):
                if c==0:
                    inputVal=32767
                else:
                    inputVal = 0
                tdOutput.append(testFilter.apply(inputVal))
            axs2=fig.add_subplot(gs[2,0])
            axs2.plot(x_time_domain,tdOutput, ".-r", label="positive pulse")
            testFilter = AudioFilter(avals,bvals)
            tdOutput=[]
            for c in range(1024):
                if c==0:
                    inputVal=-32767
                else:
                    inputVal = 0
                tdOutput.append(testFilter.apply(inputVal))
            axs2.plot(x_time_domain,tdOutput, ".-g", label="negative pulse")
            axs2.set_ylim([-33000, 33000])
            axs2.set_yticks(np.arange(start=-32768,stop=32769,step=8192))
            axs2.set_title("Time Domain response")
            axs2.set_xlabel("Time [ms]")
            axs2.set_ylabel("Ampl. [Int16]")
            axs2.grid(color="gray", linestyle="--", which="both", axis="y")
            axs2.legend(loc="upper right")

            axs3 = fig.add_subplot(gs[:,1])
            if type == "cheby1":
                filter_type_displ = "Chebychev Type I, "
            elif type == "cheby2":
                filter_type_displ = "Chebychev Type II, "
            elif type == "butter":
                filter_type_displ = "Butterworth, "
            else:
                filter_type_displ = "Unknown, "
            filter_type_displ += ftype
            cnt+=1
            fsize=0.08
            axs3.text(0.02,0.97-0*fsize,"Second-Order Section {}/{}".format(cnt,len(sos)))
            axs3.text(0.02,0.97-1*fsize,"Filter Type: \n    " + filter_type_displ)
            freq_atten_displ = "Filter Cutoff Frequencies and Attenuation: \n    {}Hz".format(fc)
            if type!="butter":
                freq_atten_displ += ", Attenuation: {}dB".format(rs)
            axs3.text(0.02, 0.97 - 2 * fsize, freq_atten_displ)
            axs3.text(0.02,0.97-3*fsize,"B coefficients: \n    {}".format(bvals))
            axs3.text(0.02,0.97-4*fsize,"A coefficients: \n    {}".format(avals))
            axs3.text(0.02, 0.97-5*fsize, "post gain factor: \n    {:.6f}".format(1./scaling))
            axs3.tick_params(bottom=False, labelbottom=False, labelleft=False, left=False)
            axs3.set_ylim([0,1])
            plt.show()
        sos_returned.append({"b": bvals, "a": avals, "gain_coeff": 1./scaling})
    return sos_returned

def design_and_plot_oversampling_lowpass_cheby(do_plot=False,to_integer=True,rs=20,oversampling=2,fc=None):
    ftype = 'lowpass'
    fs = 48000
    if fc is None:
        fc = fs/2
    order = 2
    sample_size = 16
    f0 = fc/(fs*oversampling)*2.
    bd, ad = signal.cheby2(order,rs, f0, analog=False, btype=ftype, output='ba')

    bvals = np.array(bd * ((1 << (sample_size-1)) - 1)).astype(int)
    avals = np.array(ad*((1 << (sample_size-1)) -1)).astype(int)
    if do_plot is True:
        print("B coefficients: {}".format(bvals))
        print("A coefficients: {}".format(avals))

        wz, hz = signal.freqz(bd, ad)

        freqs = wz*(fs*oversampling)/2./np.pi
        plt.subplot(2,1,1)
        plt.plot(freqs,20.*np.log10(abs(hz)))
        #plt.axis([20,(fs*oversampling)/2,-60,0])
        #plt.xscale("log")
        plt.subplot(2,1,2)
        plt.plot(freqs,np.unwrap(np.arctan2(np.real(hz),np.imag(hz)))*180./np.pi)
        #plt.xscale("log")
        plt.show()
    if to_integer is True:
        return avals,bvals
    else:
        return ad,bd

def design_and_plot_oversampling_lowpass_butter(do_plot=False,to_integer=True,oversampling=2,fc=None):
    ftype = 'lowpass'
    fs = 48000
    if fc is None:
        fc = fs/2
    order = 2
    sample_size = 16

    f0 = fc/(fs*oversampling)*2.
    bd, ad = signal.butter(order, f0, analog=False, btype=ftype, output='ba')

    bvals = np.array(bd * ((1 << (sample_size-1)) - 1)).astype(int)
    avals = np.array(ad*((1 << (sample_size-1)) -1)).astype(int)
    if do_plot is True:
        print("B coefficients: {}".format(bvals))
        print("A coefficients: {}".format(avals))

        wz, hz = signal.freqz(bd, ad)

        freqs = wz*(fs*oversampling)/2./np.pi
        plt.subplot(2,1,1)
        plt.plot(freqs,20.*np.log10(abs(hz)))
        plt.axis([20,(fs*oversampling)/2,-60,0])
        #plt.xscale("log")
        plt.subplot(2,1,2)
        plt.plot(freqs,np.unwrap(np.arctan2(np.real(hz),np.imag(hz)))*180./np.pi)
        #plt.xscale("log")
        plt.show()
    if to_integer is True:
        return avals,bvals
    else:
        return ad, bd

def design_and_plot_oversampling_lowpass_fir(do_plot=False, to_integer=True, oversampling=2,fc=None):
    firsize = 4
    firwidth = 0.1
    fs=48000
    sample_size = 16
    bd = signal.firwin(firsize, fc / oversampling, firwidth)
    ad = np.zeros(firsize)
    bvals = np.array(bd*((1 << (sample_size))-1)).astype(int)
    avals = np.zeros(firsize)

    if do_plot is True:
        print("B coefficients: {}".format(bvals))
        print("A coefficients: {}".format(avals))

        wz, hz = signal.freqz(bd)

        freqs = wz*(fs*oversampling)/2./np.pi
        plt.subplot(2,1,1)
        plt.plot(freqs,20.*np.log10(abs(hz)))
        plt.axis([20,(fs*oversampling)/2,-60,0])
        #plt.xscale("log")
        plt.subplot(2,1,2)
        plt.plot(freqs,np.unwrap(np.arctan2(np.real(hz),np.imag(hz)))*180./np.pi)
        #plt.xscale("log")
        plt.show()
    if to_integer is True:
        return avals,bvals
    else:
        return ad,bd

def get_square(current_phase,sample_size=16):
    if ((current_phase & 0x80000000) != 0):
        waveformval = (1 << (sample_size - 1))
    else:
        waveformval = -(1 << (sample_size - 1))
    return waveformval

def get_saw(current_phase):
    waveformval = ((current_phase >> 16) & 0xFFFF) - 0x7FFF
    return waveformval


def design_oversampling_comb_lp_filter(rs=3,oversampling=4,fs=48000):
    for c in range(oversampling-1):
        f_cutoff = (np.tan((fs/2*(c+1))/((oversampling*fs)/2.))*oversampling*fs/2.)/np.sqrt(2)
        print("Chebychev filter @{}Hz, attenuation:{}dB".format(int(f_cutoff),rs))
        design_and_plot_oversampling_lowpass_cheby(True, oversampling=oversampling, rs=rs, fc=f_cutoff)
    print("Butterworth filter @12000Hz")
    design_and_plot_oversampling_lowpass_butter(True, oversampling=oversampling, fc=12000)


if __name__ == "__main__":
    notenr = 128
    sample_rate=48000
    oversampling = 4
    phaseincr_bitsize=32
    rs=3
    design_and_plot_iir_filter(True,rs=10,fc=170,type="cheby2",ftype="highpass")
    #design_and_plot_iir_filter(True, to_integer=True, rs=10, fc=6000, scaling=1.0, type="butter", ftype="lowpass")
    #design_and_plot_iir_filter(True, to_integer=True, rs=10, fc=[400, 1700], scaling=0.15, type="cheby2", ftype="bandstop")
    #design_oversampling_comb_lp_filter(rs,oversampling,sample_rate)
    #design_and_plot_oversampling_lowpass_cheby(True,oversampling=4,rs=1,fc=17333.18)  #first antialiasing filter
    #design_and_plot_oversampling_lowpass_cheby(True, oversampling=4, rs=3, fc=37084.24) #second antialiasing filter
    #design_and_plot_oversampling_lowpass_cheby(True, oversampling=4, rs=3, fc=63238) # third antialiasing filter
    #design_and_plot_oversampling_lowpass_butter(True,oversampling=1,fc=6000)



