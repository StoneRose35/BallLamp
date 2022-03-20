import scipy.io
import scipy.fft
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal
from numpy import uint, ushort


class TransferFunction:
    def __init__(self,xa,xb,ya,yb):
        self.xa=xa
        self.xb=xb
        self.ya=ya
        self.yb=yb

        mat = np.array([[xa * xa * xa, xa * xa, xa, 1], [xb * xb * xb, xb * xb, xb, 1],
                        [3 * xb * xb, 2 * xb, 1, 0], [3 * xa * xa, 2 * xa, 1, 0]])
        vectr = np.array([ya, yb, (1 - yb) / (1 - xb), ya / xa])
        self.coeffs = np.linalg.solve(mat, vectr)

    def compute(self,x):

        xi=np.abs(x)
        if xi < self.xa:
            oi = xi * self.ya / self.xa
        elif xi >= self.xb:
            oi = (1 - self.yb) / (1 - self.xb) * xi + (self.yb - self.xb) / (1 - self.xb)
        else:
            oi = xi * xi * xi * self.coeffs[0] + xi * xi * self.coeffs[1] + xi * self.coeffs[2] + self.coeffs[3]
        return np.sign(x)*oi

def polyfct(x, coeffs):
    return x*x*x*coeffs[0] + x*x*coeffs[1] + x*coeffs[2] + coeffs[3]

def totalfct(xa,ya,xb,yb,coeffs,x):
    if x < xa:
        return x*ya/xa
    elif x >= xb:
        return (1-yb)/(1-xb)*x + (yb-xb)/(1-xb)
    else:
        return polyfct(x,coeffs)

def der_polyfct(x,coeffs):
    return 3*x*x*coeffs[0] + 2*x*coeffs[1] + coeffs[2]

def calculate_transfer_fct(n_points=64,scalingfact=1.0,xa=0.5,ya=0.85,xb=0.6,yb=0.97):

    mat = np.array([[xa*xa*xa, xa*xa, xa, 1], [xb*xb*xb, xb*xb, xb, 1],
           [3*xb*xb, 2*xb, 1, 0], [3*xa*xa, 2*xa, 1, 0]])
    vectr = np.array([ya, yb, (1-yb)/(1-xb), ya/xa])
    coeffs = np.linalg.solve(mat, vectr)
    print("f(xa): {}".format(polyfct(xa, coeffs)))
    print("f(xb): {}".format(polyfct(xb, coeffs)))
    print("f'(xa): {}".format(der_polyfct(xa, coeffs)))
    print("f'(xb): {}".format(der_polyfct(xb, coeffs)))

    xxes = np.arange(0.,1.,1./512.)
    yyes = []
    i=0
    while i < 512:
        yyes.append(totalfct(xa,ya,xb,yb,coeffs,xxes[i]))
        i += 1

    plt.plot(xxes,yyes)
    plt.show()

    xxes = np.arange(0.,1., 1./n_points)
    yyes = []
    i=0
    while (i< n_points):
        yyes.append(totalfct(xa,ya,xb,yb,coeffs,xxes[i])*scalingfact)
        print("{:02x}".format(yyes[-1]))

    return yyes

if __name__ == "__main__":
    fs = 48000.
    sine_freq = 1200.
    sine_periods = 64

    transferCurve = TransferFunction(0.5,0.55,0.97,1.0)
    xxes = np.linspace(-1,1,128)
    yyes = []
    for x in xxes:
        yyes.append(transferCurve.compute(x)*32767)
        print("0x{:02x}, ".format(ushort(yyes[-1])), end="")
    plt.plot(xxes*32767,yyes)
    plt.show()

"""
    oversampling = 2
    distortionCurve = TransferFunction(0.5, 0.6, 0.85, 0.97)

    n_samples = int(fs/sine_freq*sine_periods/2)*2*oversampling
    non_upsampled = []
    t_vals = np.linspace(0.,2.*np.pi*sine_periods,n_samples)
    windows_fct = np.concatenate((np.linspace(0,1,int(n_samples/2)), np.linspace(1,0,int(n_samples/2))))
    sine_vals = np.sin(t_vals)*windows_fct
    sine_vals_distorted = [distortionCurve.compute(x) for x in sine_vals]
    spec_orig = scipy.fft.fft(sine_vals)
    spec_distorted = scipy.fft.fft(sine_vals_distorted)
    faxis_orig = scipy.fft.fftfreq(n_samples,1./fs/oversampling)
    upsampled = np.zeros(n_samples)
    upsampled[0::oversampling]=sine_vals[0::oversampling]
    b,a = scipy.signal.butter(2,1/oversampling)
    upsampled = scipy.signal.lfilter(b,a,upsampled)*oversampling
    spec_upsampled = scipy.fft.fft(upsampled)

    distorted_upsampled = [distortionCurve.compute(x) for x in upsampled]
    distorted_upsampled = scipy.signal.lfilter(b, a, distorted_upsampled) * oversampling
    spec_distorted_upsampled = scipy.fft.fft(distorted_upsampled)
    faxis_upsampled = scipy.fft.fftfreq(n_samples, 1./fs/oversampling)
    plt.subplot(4,1,1)
    plt.plot(upsampled)
    plt.subplot(4,1,2)
    plt.plot(t_vals,sine_vals,"-k")
    plt.plot(t_vals,sine_vals_distorted,"--r")
    plt.subplot(4,1,3)
    log_spec_orig = 20*np.log10(abs(spec_orig[:int(n_samples/2)]))
    log_spec_distorted = 20*np.log10(abs(spec_distorted[:int(n_samples/2)]))
    log_spec_upsampled = 20*np.log10(abs(spec_upsampled[:int(len(spec_upsampled)/2.)]))
    log_spec_distorted_upsampled = 20*np.log10(abs(spec_distorted_upsampled[:int(len(spec_distorted_upsampled)/2.)]))
    plt.plot(faxis_orig[:int(n_samples/2)], log_spec_orig - max(log_spec_orig), "g")
    plt.plot(faxis_upsampled[:int(faxis_upsampled.size/2)], log_spec_upsampled - max(log_spec_upsampled), "+k")
    plt.ylim([-80,0])
    plt.subplot(4, 1, 4)
    plt.plot(faxis_orig[:int(n_samples/2)], log_spec_distorted - max(log_spec_distorted), "r")
    plt.plot(faxis_upsampled[:int(faxis_upsampled.size/2)], log_spec_distorted_upsampled - max(log_spec_distorted_upsampled), "+k")
    plt.ylim([-80,0])

    plt.show()



    pass
    """
