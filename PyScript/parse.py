import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


def createMathModelPlot(mathResFile, maxNumOfStations):
    mathModelPlot = []
    for i in range(maxNumOfStations):
        str = mathResFile.readline()
        n_str, s_str = str.split("\t")
        n, s = int(n_str), float(s_str)
        mathModelPlot.append(s)
    return mathModelPlot


def printMistakes(ax, args, plotMin, plotMax, mathModelPlot):
    x, y = [], []
    for i in range(maxNumOfStations):
        if (plotMin[i] > mathModelPlot[i] or plotMax[i] < mathModelPlot[i]):
            x.append(args[i])
            y.append(mathModelPlot[i])
    ax.plot(x, y, 'ro', color='blue', label='mistakes')

def printPlots(simResFile, mathResFile, maxNumOfStations, repeats):
    # args = [i for i in range(1, maxNumOfStations + 1)]
    args = []
    plotMin, plotMax, plotAvg = [], [], []

    min, max, sum = 0, 0, 0
    for i in range(maxNumOfStations):
        nn = 0
        for j in range(repeats):
            str = simResFile.readline()
            n_str, s_str = str.split("\t")
            n, s = int(n_str), float(s_str)
            nn = n
            if (j == 0):
                min, max = s, s
            if (min > s):
                min = s
            if (max < s):
                max = s
            sum += s
        args.append(nn)
        plotMin.append(min)
        plotMax.append(max)
        plotAvg.append(sum / repeats)
        sum = 0

    mathModelPlot = createMathModelPlot(mathResFile, maxNumOfStations)

    fig, ax = plt.subplots()

    printMistakes(ax, args, plotMin, plotMax, mathModelPlot)

    ax.plot(args, plotAvg,
            linestyle='-', 
            linewidth=1, 
            color='green',
            label='Simulation model (avg)')

    ax.plot(args, plotMin,
            linestyle='--', 
            linewidth=1, 
            color='red',
            label='Simulation model (min)')

    ax.plot(args, plotMax,
            linestyle='--', 
            linewidth=1, 
            color='red',
            label='Simulation model (max)')

    ax.plot(args, mathModelPlot,
            linestyle='-', 
            linewidth=1, 
            color='darkmagenta',
            label='Math model')


    plt.xlabel('Number of stations', fontsize=12, color='blue')
    plt.ylabel('Throughput', fontsize=12, color='blue')

    ax.xaxis.set_major_locator(ticker.MultipleLocator(10))
    ax.xaxis.set_minor_locator(ticker.MultipleLocator(5))
    ax.yaxis.set_major_locator(ticker.MultipleLocator(1))

    ax.grid(which='major',
            color = 'k')

    ax.minorticks_on()

    ax.grid(which='minor',
            color = 'gray',
            linestyle = ':')

    fig.set_figwidth(16)
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
