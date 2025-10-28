import os
toremove = ["main.py","version.py","sensors.py","files.py","icons.py","prefs.py","data.py"]
for i in toremove:
    try:
        os.remove(i)
        print(i, "removed")
    except:
        print(i, "doesn't exist")

print("Files left", os.listdir())