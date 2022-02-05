import matplotlib.pyplot as plt
import csv

if __name__ == "__main__":
    temps=[]
    heater=[]
    integral_factor=[]
    with open("./piregulator.txt","rt") as csvfile:
        rdr = csv.reader(csvfile,delimiter=",")
        for e in rdr:
            temps.append(float(e[0]))
            heater.append(int(e[1]))
            integral_factor.append(int(e[2]))
    plt.plot(temps)
    plt.show()
