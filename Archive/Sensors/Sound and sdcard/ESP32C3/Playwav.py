from machine import I2S, Pin
import math
import time
import machine
import sdcard
import os
import ustruct


'''
A1 - This is used for audio data, or DAT.
A2 - This is used for wordselect clock, or LR.
A3 - This is used for bitclock, or BCLK.
'''
'''
Pin 2
Pin 8
Pin 9
Pin 10
Pin 3
Pin 4
Pin 5
 



'''


# I2S Setup Function with Variable Rate
def setup_i2s(sample_rate, bit_depth,BUFFER_SIZE = 1024):
    i2s = I2S(
        0,
        sck=Pin(5),   # Serial Clock
        ws=Pin(4),    # Word Select / LR Clock
        sd=Pin(3),    # Serial Data
        mode=I2S.TX,
        bits=bit_depth,
        format=I2S.MONO,
        rate=sample_rate,
        ibuf=BUFFER_SIZE
    )
    return i2s

# Setup SPI and SD card
spi = machine.SPI(1, baudrate=1000000, polarity=0, phase=0,
                  sck=machine.Pin(8), mosi=machine.Pin(10), miso=machine.Pin(9))
cs = machine.Pin(2, machine.Pin.OUT)
sd = sdcard.SDCard(spi, cs)

# Mount the SD card
vfs = os.VfsFat(sd)
os.mount(vfs, "/sd")



# Helper function to parse the .wav header and extract audio parameters
def parse_wav_header(file):
    file.seek(0)  # Ensure we're at the start of the file
    header = file.read(44)  # Standard .wav header size
    sample_rate = ustruct.unpack('<I', header[24:28])[0]
    bit_depth = ustruct.unpack('<H', header[34:36])[0]
    return sample_rate, bit_depth



def play_sound(path):
        # Open the .wav file from the SD card
    with open("/sd/"+ path, "rb") as wav_file:
        sample_rate, bit_depth = parse_wav_header(wav_file)
        print(sample_rate, bit_depth)
        i2s = setup_i2s(sample_rate, bit_depth)

        # Skip the header and begin reading audio data
        wav_file.seek(44)  # Start of PCM data after 44-byte header

        # Buffer to hold PCM data read from the file
        buffer = bytearray(1024)
        
        while True:
            try:
                num_read = wav_file.readinto(buffer)
                if num_read == 0:
                    break  # End of file
                i2s.write(buffer[:num_read])  # Write to I2S in chunks
            except KeyboardInterrupt:
                print("CTRL C pressed")



try:
    play_sound("sound1.wav")

    print('played the sound')
    time.sleep(5)
    print('sleeping')
    machine.deepsleep(5000)
    
except Exception as e:
    print("An error occurred:", e)
finally:
    # Ensure unmounting even if there was an error
    try:
        os.umount("/sd")
    except:
        pass
