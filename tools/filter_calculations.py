import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import math

class AudioFilter:
    def __init__(self,a,b,sample_size):
        self.a=a
        self.b=b
        self.sample_size = sample_size
        self.bfrIn = np.zeros(len(b))
        #self.bfrOut = np.zeros(len(a))

    def apply(self, sample):
        out = 0
        for cnt in range(len(self.bfrIn)-1,0,-1):
            self.bfrIn[cnt] = self.bfrIn[cnt-1]
        self.bfrIn[0] = sample

        for c in range(1, len(self.bfrIn)):
            self.bfrIn[0] -= self.a[c]*self.bfrIn[c]/(2**(self.sample_size))
        for c in range(len(self.bfrIn)):
            out = out + self.b[c]*self.bfrIn[c]/(2**(self.sample_size)) # - self.a[c]*self.bfrOut[c]/(2**(self.sample_size-1))
        out = int(out)
        #for cnt in range(len(self.bfrOut)-1,0,-1):
        #    self.bfrOut[cnt] = self.bfrOut[cnt-1]
        #self.bfrOut[0] = out
        return out

def get_phase_increments():
    notes = range(128)
    fs=48000
    bitres = 32
    phaseincs=[]
    for note in notes:
        notefreq = math.pow(2.,(note-64)/12.)*440.
        phaseinc = notefreq/fs*((1 << bitres))
        f_synth = int(phaseinc)*fs/((1 << bitres))
        if (phaseinc > (1 << (bitres-1))):
            print("note {} unavailable".format(note))
        else:
            phaseincs.append(int(phaseinc))
            #print("{},{}, {}, {}".format(note, notefreq, int(phaseinc),f_synth))
            #print("{},".format(int(phaseinc)))
    return phaseincs

def get_sine_table():
    bitres = 16
    nvals = 256
    sinetable=[]
    for c in range(nvals):
        print("{},".format(int(math.cos(c/nvals*math.pi*2)*((1 << (bitres-1)) - 1))))
        sinetable.append(int(math.cos(c/nvals*math.pi*2)*((1 << (bitres-1)) - 1)))
    return sinetable

def design_and_plot_oversampling_lowpass_cheby(do_plot=False,to_integer=True,oversampling=2):
    ftype = 'lowpass'
    fs=48000
    fc = 20000
    order = 4
    sample_size = 16
    rs=20
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

def design_and_plot_oversampling_lowpass_butter(do_plot=False,to_integer=True,oversampling=2):
    ftype = 'lowpass'
    fs=48000
    fc = 20000
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
        return ad,bd

def get_square(current_phase):
    if ((current_phase & 0x80000000) != 0):
        waveformval = (1 << (sample_size - 1))
    else:
        waveformval = -(1 << (sample_size - 1))
    return waveformval

def get_saw(current_phase):
    waveformval = ((current_phase >> 16) & 0xFFFF) - 0x7FFF
    return waveformval

if __name__ == "__main__":
    notenr=126
    sample_size=16
    nplots = 512
    oversampling=2
    phaseincs=get_phase_increments()
    #a_vals, b_vals = design_and_plot_oversampling_lowpass_cheby(True,True,oversampling)
    #a_vals=np.zeros(len(b_vals)) # [0,0]
    firsize = 16
    b_vals = np.array(signal.firwin(firsize,1/oversampling,0.01)*32767).astype(int)
    a_vals = np.zeros(firsize)

    oversampling_filter = AudioFilter(a_vals, b_vals, sample_size)
    waveform_raw = []
    waveform_filt = []
    current_phase = 0
    for c in range(nplots):
        current_phase += (phaseincs[notenr-1] & 0xFFFFFFFF)
        waveformval = get_saw(current_phase)
        waveform_raw.append(waveformval)
        waveform_filt.append(oversampling_filter.apply(waveformval))
        for i in range(oversampling-1):
            waveform_raw.append(0)
        for i in range(oversampling-1):
            waveform_filt.append(0)

    downsampled_raw=[]
    downsampled_filtered=[]
    for q in range(int(len(waveform_raw)/oversampling)):
        downsampled_raw.append(np.average(waveform_raw[q*oversampling:(q+1)*oversampling]))
        downsampled_filtered.append(np.average(waveform_filt[q*oversampling:(q+1)*oversampling]))
    #downsampled_raw = waveform_raw[::oversampling]
    #downsampled_filtered = waveform_filt[::oversampling]
    plt.subplot(2,1,1)
    plt.plot(waveform_raw,"r")
    plt.plot(waveform_filt,"b")
    plt.subplot(2,1,2)
    plt.plot(downsampled_raw,"r")
    plt.plot(downsampled_filtered,"b")
    plt.show()




