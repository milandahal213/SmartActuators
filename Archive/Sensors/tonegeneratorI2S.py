from machine import I2S, Pin
import math
import time

# I2S configuration
SAMPLE_RATE = 44100  # Standard audio sample rate
TONE_FREQUENCY = 440  # Frequency of tone in Hz (A4 note)
BUFFER_SIZE = 1024  # Size of audio buffer


# I2S Setup Function with Variable Rate
def setup_i2s(sample_rate, bit_depth =16,BUFFER_SIZE = 1024):
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

i2s= setup_i2s(SAMPLE_RATE)
# Generate a buffer with a single tone
def generate_tone_buffer(frequency, sample_rate, duration=1):
    ramp=.1
    num_samples = sample_rate * duration
    buffer = bytearray(num_samples * 2)  # 16-bit samples
    amplitude = 32767
    for i in range(num_samples):
        if(i< int(sample_rate*ramp)):
            amplitude = 32767*i/(sample_rate*ramp)
        elif(i> int(sample_rate*(1-ramp))):
            amplitude = 32767*(1-(i-sample_rate*(1-ramp))/(sample_rate*ramp))
        else:
             amplitude = 32767
        sample = int(amplitude * math.sin(2 * math.pi * frequency * i / sample_rate))
        buffer[2*i] = sample & 0xff  # Lower byte
        buffer[2*i + 1] = (sample >> 8) & 0xff  # Upper byte
    return buffer

# Generate tone and play
tone_buffer = generate_tone_buffer(TONE_FREQUENCY, SAMPLE_RATE)
print(len(tone_buffer))
print("playing")
while True:
    i2s.write(tone_buffer)
    time.sleep(0.5)  # Play tone continuously with a delay
