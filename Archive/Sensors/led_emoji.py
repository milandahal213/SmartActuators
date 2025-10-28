#This tool from Grove can used to draw emoji https://files.seeedstudio.com/wiki/Grove-RGB_LED_Matrix_w-Driver/res/docs.zip 
import random


"""
Colors
"""
c = {
    'red': 0x00,              # #FF0000 (color-0)
    'orange': 0x18,           # #FF8000 (color-24)
    'yellow': 0x2a,           # #FFFF00 (color-42)
    'chartreuse': 0x40,       # #80FF00 (color-64)
    'green': 0x55,            # #00FF00 (color-85)
    'spring green': 0x60,     # #00FF80 (color-96)
    'cyan': 0x80,             # #00FFFF (color-128)
    'azure': 0x94,            # #0080FF (color-148)
    'blue': 0xaa,             # #0000FF (color-170)
    'violet': 0xbf,           # #8000FF (color-191)
    'magenta': 0xd4,          # #FF00FF (color-212)
    'rose': 0xe7,              # #FF0080 (color-231)
    'white': 0xfe,
    'black': 0xff
}

"""
Custom Emoji
"""

ladybug_icon = [
    [c['cyan'], c['cyan'], c['red'],   c['red'],   c['red'],   c['red'],   c['cyan'], c['cyan']],
    [c['cyan'], c['red'],   c['white'], c['red'],   c['red'],   c['white'], c['red'],   c['cyan']],
    [c['red'],   c['red'],   c['red'],   c['black'], c['black'], c['red'],   c['red'],   c['red']],
    [c['red'],   c['black'], c['black'], c['red'],   c['red'],   c['black'], c['black'], c['red']],
    [c['red'],   c['black'], c['red'],   c['red'],   c['red'],   c['red'],   c['black'], c['red']],
    [c['red'],   c['red'],   c['black'], c['black'], c['black'], c['black'], c['red'],   c['red']],
    [c['cyan'], c['red'],   c['black'], c['black'], c['black'], c['black'], c['red'],   c['cyan']],
    [c['cyan'], c['cyan'], c['red'],   c['red'],   c['red'],   c['red'],   c['cyan'], c['cyan']],
] #need to flatten

baseball_icon = [
    [c['black'], c['black'], c['black'], c['white'], c['white'], c['black'], c['black'], c['black']],
    [c['black'], c['red'], c['white'],   c['white'], c['white'], c['white'],   c['red'], c['black']],
    [c['black'], c['white'],   c['red'], c['white'], c['white'], c['red'], c['white'],   c['black']],
    [c['white'], c['white'], c['red'], c['white'], c['white'], c['red'], c['white'], c['white']],
    [c['white'], c['white'], c['red'], c['white'], c['white'], c['red'], c['white'], c['white']],
    [c['black'], c['white'],   c['red'], c['white'], c['white'], c['red'], c['white'],   c['black']],
    [c['black'], c['red'], c['white'],   c['white'], c['white'], c['white'],   c['red'], c['black']],
    [c['black'], c['black'], c['black'], c['white'], c['white'], c['black'], c['black'], c['black']],
] #need to flatten

flower_icon = [
    [c['black'], c['black'], c['magenta'],   c['magenta'],   c['magenta'],   c['magenta'],   c['black'], c['black']],
    [c['black'], c['black'],   c['rose'],   c['rose'],   c['rose'],   c['rose'],   c['black'],   c['black']],
    [c['magenta'],   c['rose'],   c['black'],   c['red'], c['red'], c['black'],   c['rose'],   c['magenta']],
    [c['magenta'],   c['rose'],   c['red'], c['yellow'], c['yellow'], c['red'], c['rose'],   c['magenta']],
    [c['magenta'],   c['rose'],   c['red'],   c['yellow'], c['yellow'], c['red'],   c['rose'],   c['magenta']],
    [c['magenta'], c['rose'],   c['black'],   c['red'],   c['red'],   c['black'],   c['rose'],   c['magenta']],
    [c['black'], c['black'], c['rose'],   c['rose'],   c['rose'],   c['rose'],   c['black'], c['black']],
    [c['black'], c['black'], c['magenta'],   c['magenta'],   c['magenta'],   c['magenta'],   c['black'], c['black']],
] #need to flatten

# rainbow1 (initial rainbow)
rainbow1 = [
    [c['red']]*8,
    [c['orange']]*8,
    [c['yellow']]*8,
    [c['chartreuse']]*8,
    [c['green']]*8,
    [c['spring green']]*8,
    [c['cyan']]*8,
    [c['azure']]*8
] #need to flatten

# rainbow2 - remove red, add blue
rainbow2 = [
    [c['orange']]*8,
    [c['yellow']]*8,
    [c['chartreuse']]*8,
    [c['green']]*8,
    [c['spring green']]*8,
    [c['cyan']]*8,
    [c['azure']]*8,
    [c['blue']]*8
] #need to flatten

# rainbow3 - remove orange, add violet
rainbow3 = [
    [c['yellow']]*8,
    [c['chartreuse']]*8,
    [c['green']]*8,
    [c['spring green']]*8,
    [c['cyan']]*8,
    [c['azure']]*8,
    [c['blue']]*8,
    [c['violet']]*8
] #need to flatten

# rainbow4 - remove yellow, add magenta
rainbow4 = [
    [c['chartreuse']]*8,
    [c['green']]*8,
    [c['spring green']]*8,
    [c['cyan']]*8,
    [c['azure']]*8,
    [c['blue']]*8,
    [c['violet']]*8,
    [c['magenta']]*8
] #need to flatten

# rainbow5 - remove chartreuse, add rose
rainbow5 = [
    [c['green']]*8,
    [c['spring green']]*8,
    [c['cyan']]*8,
    [c['azure']]*8,
    [c['blue']]*8,
    [c['violet']]*8,
    [c['magenta']]*8,
    [c['rose']]*8
] #need to flatten

# rainbow6 - remove green, add red
rainbow6 = [
    [c['spring green']]*8,
    [c['cyan']]*8,
    [c['azure']]*8,
    [c['blue']]*8,
    [c['violet']]*8,
    [c['magenta']]*8,
    [c['rose']]*8,
    [c['red']]*8
] #need to flatten

# rainbow7 - remove spring green, add orange
rainbow7 = [
    [c['cyan']]*8,
    [c['azure']]*8,
    [c['blue']]*8,
    [c['violet']]*8,
    [c['magenta']]*8,
    [c['rose']]*8,
    [c['red']]*8,
    [c['orange']]*8
] #need to flatten

# rainbow8 - remove cyan, add yellow
rainbow8 = [
    [c['azure']]*8,
    [c['blue']]*8,
    [c['violet']]*8,
    [c['magenta']]*8,
    [c['rose']]*8,
    [c['red']]*8,
    [c['orange']]*8,
    [c['yellow']]*8
] #need to flatten

# rainbow9 - remove azure, add chartreuse
rainbow9 = [
    [c['blue']]*8,
    [c['violet']]*8,
    [c['magenta']]*8,
    [c['rose']]*8,
    [c['red']]*8,
    [c['orange']]*8,
    [c['yellow']]*8,
    [c['chartreuse']]*8
] #need to flatten

# rainbow10 - remove blue, add green
rainbow10 = [
    [c['violet']]*8,
    [c['magenta']]*8,
    [c['rose']]*8,
    [c['red']]*8,
    [c['orange']]*8,
    [c['yellow']]*8,
    [c['chartreuse']]*8,
    [c['green']]*8
] #need to flatten

# rainbow11 - remove violet, add spring green
rainbow11 = [
    [c['magenta']]*8,
    [c['rose']]*8,
    [c['red']]*8,
    [c['orange']]*8,
    [c['yellow']]*8,
    [c['chartreuse']]*8,
    [c['green']]*8,
    [c['spring green']]*8
] #need to flatten

# rainbow12 - remove magenta, add cyan
rainbow12 = [
    [c['rose']]*8,
    [c['red']]*8,
    [c['orange']]*8,
    [c['yellow']]*8,
    [c['chartreuse']]*8,
    [c['green']]*8,
    [c['spring green']]*8,
    [c['cyan']]*8
] #need to flatten

# red die face value 1
die_1 = [
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0x02,0x02,0xff,0xff,0xff, 
    0xff,0xff,0xff,0x02,0x02,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
] #flattened

die_2 = [
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0x13,0x13,0xff, 
    0xff,0xff,0xff,0xff,0xff,0x13,0x13,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0x13,0x13,0xff,0xff,0xff,0xff,0xff, 
    0xff,0x13,0x13,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
]  #flattened

die_3 = [
    0xff,0xff,0xff,0xff,0xff,0xff,0x2b,0x2b, 
    0xff,0xff,0xff,0xff,0xff,0xff,0x2b,0x2b, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0x2b,0x2b,0xff,0xff,0xff, 
    0xff,0xff,0xff,0x2b,0x2b,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0x2b,0x2b,0xff,0xff,0xff,0xff,0xff,0xff, 
    0x2b,0x2b,0xff,0xff,0xff,0xff,0xff,0xff
]  #flattened
 
die_4 = [
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0x55,0x55,0xff,0xff,0x55,0x55,0xff, 
    0xff,0x55,0x55,0xff,0xff,0x55,0x55,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0x55,0x55,0xff,0xff,0x55,0x55,0xff, 
    0xff,0x55,0x55,0xff,0xff,0x55,0x55,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
]  #flattened

die_5 = [
    0x8e,0x8e,0xff,0xff,0xff,0xff,0x8e,0x8e, 
    0x8e,0x8e,0xff,0xff,0xff,0xff,0x8e,0x8e, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xff,0xff,0xff,0x8e,0x8e,0xff,0xff,0xff, 
    0xff,0xff,0xff,0x8e,0x8e,0xff,0xff,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0x8e,0x8e,0xff,0xff,0xff,0xff,0x8e,0x8e, 
    0x8e,0x8e,0xff,0xff,0xff,0xff,0x8e,0x8e
]  #flattened
 
die_6 = [
    0xce,0xce,0xff,0xff,0xff,0xff,0xce,0xce, 
    0xce,0xce,0xff,0xff,0xff,0xff,0xce,0xce, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xce,0xce,0xff,0xff,0xff,0xff,0xce,0xce, 
    0xce,0xce,0xff,0xff,0xff,0xff,0xce,0xce, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0xce,0xce,0xff,0xff,0xff,0xff,0xce,0xce, 
    0xce,0xce,0xff,0xff,0xff,0xff,0xce,0xce
]  #flattened
    
 
def roll_die():
    roll = random.randint(1, 6)
    if roll == 1:
        return die_1
    elif roll == 2:
        return die_2
    elif roll == 3:
        return die_3
    elif roll == 4:
        return die_4
    elif roll == 5:
        return die_5
    else:
        return die_6
        
 

def fill_color(color):
    return [c[color]]*64

def flatten_icon(icon_2dlist):
    return [pixel for row in icon_2dlist for pixel in row]