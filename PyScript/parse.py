import numpy as np
import matplotlib.pyplot as plt

def createMathModelPlot(mathResFile, maxNumOfStations):
    mathModelPlot = []
    for i in range(maxNumOfStations):
        str = mathResFile.readline()
        n_str, s_str = str.split("\t")
        n, s = int(n_str), float(s_str)
        mathModelPlot.append(s)
    return mathModelPlot

def printPlots(simResFile, mathResFile, maxNumOfStations, repeats):
    args = [i for i in range(1, maxNumOfStations + 1)]
    plotMin, plotMax, plotAvg = [], [], []

    min, max, sum = 0, 0, 0
    for i in range(maxNumOfStations):
        for j in range(repeats):
            str = simResFile.readline()
            n_str, s_str = str.split("\t")
            n, s = int(n_str), float(s_str)
            if (j == 0):
                min, max = s, s
            if (min > s):
                min = s
            if (max < s):
                max = s
            sum += s
        plotMin.append(min)
        plotMax.append(max)
        plotAvg.append(sum / repeats)
        sum = 0

    mathModelPlot = createMathModelPlot(mathResFile, maxNumOfStations)

    fig, ax = plt.subplots()

    ax.plot(args, plotAvg,
            linestyle='-', 
            linewidth=1, 
            color='green')

    ax.plot(args, plotMin,
            linestyle='--', 
            linewidth=1, 
            color='red')

    ax.plot(args, plotMax,
            linestyle='--', 
            linewidth=1, 
            color='red')

    ax.plot(args, mathModelPlot,
            linestyle='-', 
            linewidth=1, 
            color='darkmagenta')

    fig.set_figwidth(20)
    fig.set_figheight(10)
    fig.set_facecolor('linen')
    ax.set_facecolor('ivory')
    plt.show()


maxNumOfStations = 100
repeats = 5

simResFile = open("../sim_output.txt", "r")
mathResFile = open("../output.txt", "r")

printPlots(simResFile, mathResFile, maxNumOfStations, repeats)

simResFile.close()
mathResFile.close()
