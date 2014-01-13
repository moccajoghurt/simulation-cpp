from Tkinter import *
import glob
import os

map_list = ["null"]
os.chdir("./maps")
for files in glob.glob("*.png"):
    map_list.append(files)

os.chdir("../")

def disable_entries(delay,step_num):
	global checkbox_state
	if (checkbox_state == 0):
		checkbox_state = 1
		delay.configure(state="normal")
		step_num.configure(state="disabled")
	else:
		checkbox_state = 0
		step_num.configure(state="normal")
		delay.configure(state="disabled")


def start_sim():
	cmd = None
	if (checkbox_state == 1):
		cmd = "start sim " + e_node_size.get() + " " + e_width_size.get() + " " + e_height_size.get() + " " + delay.get() + " " + str(0)
	else:
		cmd = "start sim " + e_node_size.get() + " " + e_width_size.get() + " " + e_height_size.get() + " " + str(-1) + " " + step_num.get()
	
	if (var.get() == "null"):
		cmd += " null"
	else:
		cmd += " ./maps/" + var.get()
	
	
	os.system(cmd)

checkbox_state = 0
window = Tk()
window.wm_title("Simulation Augenfarbe")

node_size = Label(window, text = "node size").grid(row=0, sticky=W)
sim_width = Label(window, text = "width").grid(row=1, sticky=W)
sim_height = Label(window, text = "height").grid(row=2, sticky=W)
e_node_size = Entry(window)
e_node_size.grid(row=0, column=1)
e_node_size.insert(0, "5")
e_width_size = Entry(window)
e_width_size.grid(row=1, column=1)
e_width_size.insert(0, "500")
e_height_size = Entry(window)
e_height_size.grid(row=2, column=1)
e_height_size.insert(0, "500")
label_delay = Label(window, text = "delay").grid(row=4, sticky=W)
delay = Entry(window)
delay.grid(row=4, column = 1, sticky=W)
delay.insert(0, "5")
delay.configure(state="disabled")
step_num_label = Label(window, text = "step num").grid(row=5, sticky=W)
step_num = Entry(window)
step_num.grid(row=5, column = 1, sticky=W)
step_num.insert(0, "100")
graphics = Checkbutton(window, text="graphics", command=lambda d=delay, s=step_num : disable_entries(d, s))
graphics.grid(row=3, sticky=W)
preload_label = Label(window, text = "choose map").grid(row=6, sticky=W)
var = StringVar(window)
var.set("null")
opt = OptionMenu(window, var, *map_list)
opt.grid(row=6, column=1, sticky=W)
button = Button(window, command=start_sim, text = "start simulation!")
button.grid(row=7, column=1, sticky=W)


window.mainloop()
