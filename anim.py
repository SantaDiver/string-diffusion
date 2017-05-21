import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

file = open("./reportdata")
file.readline()
x1 = float(file.readline())
dx = float(file.readline())
x_pts = int(file.readline())
t_pts = int(file.readline())

solution = []

for line in file:
	line = line.strip()
	if len(line) > 0:
  		solution.append(map(float, line.split(' ')))

# print solution

def show():
	def _graph_animate(t):
		line.set_ydata(solution[t])
		if (t >= t_pts-1):
			print ("THE END OF ANIMATION")
		return line
	xs = [x1 + i*dx for i in range(x_pts)]
	fig, ax = plt.subplots()
	line, = ax.plot(xs, solution[0])
	anm = animation.FuncAnimation(fig, _graph_animate, frames=t_pts, interval=5000.0/t_pts, repeat=False)
	plt.show()

show()
