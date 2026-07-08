import time
import math
import turtle
def xt(t):
    return 16 * math.sin(t) ** 3
def yt(t):
    return 13 * math.cos(t) - 5 * \
        math.cos(2 * t) - 2 * \
        math.cos(3 * t) - \
        math.cos(4 * t)
t = turtle.Turtle()
turtle.Screen().screensize(1040, 1080)
t.speed(2500)
turtle.colormode(255)
turtle.Screen().bgcolor(0, 0, 0)
time.sleep(1)
for i in range(300):
    t.goto((xt(i) * 20, yt(i) * 20))
    t.pencolor("red")
    t.goto(0, 0)
t.hideturtle()
turtle.update()
turtle.mainloop()
