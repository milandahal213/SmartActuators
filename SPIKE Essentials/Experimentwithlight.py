#playing with essentials light matrix

import hub

class ColorMatrix():
    def __init__(self, portName):
        self.light=portName.device


    def _hue_to_id(self, h):

        h = h % 360

        # Color.RED
        if h < 15 or h > 330:
            return 9
        # Color.ORANGE
        if 15 <= h <= 45:
            return 8
        # Color.YELLOW
        if 45 <= h <= 90:
            return 7
        # Color.GREEN
        if 90 <= h <= 135:
            return 6
        # Turquoise?
        if 135 <= h <= 165:
            return 5
        # Color.CYAN
        if 165 <= h <= 210:
            return 4
        # Color.BLUE
        if 210 <= h <= 255:
            return 3
        # Color.VIOLET
        if 255 <= h <= 285:
            return 2
        # Color.MAGENTA
        if 285 <= h <= 330:
            return 1

    def _color_to_id(self, color):

        brightness = color.v // 10

        if color.s < 30:
            return 10 if color.v <= 10 else 10 + 16 * brightness
        else:
            return self._hue_to_id(color.h) + 16 * brightness

    def off(self):
        self.light.write(2, [0]*9)

    def on(self, colors):
        self.light.write(2, [self._color_to_id(c) for c in colors])


color_matrix = ColorMatrix(hub.port.B)

# Static single color example
color_matrix.on(Color.ORANGE)
wait(1000)

# Static multi color example
color_matrix.on([Color.RED, Color.WHITE, Color.BLUE] * 3)
wait(1000)

# Fade example
for i in range(3):
    for brightness in tuple(range(0, 110, 10)) + tuple(range(100, -10, -10)):
        color_matrix.on(Color.BLUE * (brightness / 100) )
        wait(50)

# Randomness
for i in range(1000):
    color_matrix.on([Color(randint(0, 360), v=40) for i in range(9)])
    wait(200)