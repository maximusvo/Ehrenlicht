import struct
import numpy as np
import pyaudio
#from neopixel import *
import pygame, sys
from pygame.locals import *

class Simulation(object):

    WINDOWWIDTH = 400
    WINDOWHEIGHT = 400
    BLACK = (0, 0, 0)

    def __init__(self, audio, num_bars):
        pygame.init()
        self.mainClock = pygame.time.Clock()
        self.audio = audio
        self.windowSurface = pygame.display.set_mode((self.WINDOWWIDTH, self.WINDOWHEIGHT), 0, 32)
        self.num_bars = num_bars
        self.bar_size = round(self.WINDOWHEIGHT / self.num_bars)

    def events(self):
        for event in pygame.event.get():
            if event.type == QUIT:
                self.audio.stream.stop_stream()
                self.audio.stream.close()
                self.audio.audio.terminate()
                pygame.quit()
                sys.exit()

    def draw(self):
        self.windowSurface.fill(self.BLACK)
        for i in range(self.num_bars):
            y = self.audio.data
            s = np.sum(y[i*self.bar_size:(i + 1)*self.bar_size])
            if s > 50:
                pygame.draw.rect(self.windowSurface, (255,0,0), (0, 2*i*self.bar_size, self.WINDOWWIDTH, 2*self.bar_size))
        pygame.display.update()
        self.mainClock.tick(40)

class LedStrip(object):

    # LED strip configuration:
    LED_COUNT      = 144     # Number of LED pixels.
    LED_PIN        = 18      # GPIO pin connected to the pixels (must support PWM!).
    LED_FREQ_HZ    = 800000  # LED signal frequency in hertz (usually 800khz)
    LED_DMA        = 5       # DMA channel to use for generating signal (try 5)
    LED_BRIGHTNESS = 255     # Set to 0 for darkest and 255 for brightest
    # True to invert the signal (when using NPN transistor level shift)
    LED_INVERT     = False

    def __init__(self, audio, num_bars):
        self.audio = audio
        self.leds_per_bar = int(self.LED_COUNT / num_bars)
        self.num_bars = num_bars
        self.bar_size = round(self.audio.nFFT/self.num_bars)
        self.strip = Adafruit_NeoPixel(
            self.LED_COUNT,
            self.LED_PIN,
            self.LED_FREQ_HZ,
            self.LED_DMA,
            self.LED_INVERT,
            self.LED_BRIGHTNESS)
        # Intialize the library (must be called once before other functions).
        self.strip.begin()

    def update(self):
        for i in range(self.LED_COUNT):
            self.strip.setPixelColor(i, Color(0, 0, 0))

        for i in range(self.num_bars):
            y = self.audio.data
            s = np.sum(y[i*self.bar_size:(i + 1)*self.bar_size])

            if s > 50:
                for led_i in range(self.leds_per_bar):
                    self.strip.setPixelColor(i*int(self.leds_per_bar) + led_i, Color(i*50, 200, i*10))
        self.strip.show()


class AudioProcessor(object):
    nFFT = int(512)
    buffer_size = 4 * nFFT
    format = pyaudio.paInt16
    channels = 1
    rate = 44100

    def __init__(self):
        self.audio = pyaudio.PyAudio()
        # Used for normalizing signal. If use paFloat32, then it's already -1..1.
        # Because of saving wave, paInt16 will be easier.
        self.max_y = 2.0 ** (self.audio.get_sample_size(self.format) * 8 - 1)
        self.stream = self.audio.open(format=self.format,
                                      channels=self.channels,
                                      rate=self.rate,
                                      input=True,
                                      frames_per_buffer=self.buffer_size)

    def capture_audio(self):
        # Read n*nFFT frames from stream, n > 0
        num_frames = max(self.stream.get_read_available() / self.nFFT, 1) * self.nFFT
        data = self.stream.read(int(num_frames))

        # Unpack data, LRLRLR...
        y = np.array(struct.unpack("%dh" % (num_frames * self.channels), data)) / self.max_y
        y_L = y[::2]
        y_R = y[1::2]

        #y_R = y_R[::-1]
        Y_L = np.fft.fft(y_L, self.nFFT)
        Y_R = np.fft.fft(y_R, self.nFFT)

        # Sewing FFT of two channels together, DC part uses right channel's
        Y = abs(np.hstack((Y_L[-round(self.nFFT / 2):-1:-1], Y_R[:round(self.nFFT / 2)])))
        oY = Y[:round(len(Y)/2)] + Y[round(len(Y)/2):]

        self.data = oY

#num_bars = 10
#bar_size = round(WINDOWHEIGHT / num_bars)
#bar_sound_size = round(nFFT/2 / num_bars)
#num_leds = int(144.0 / num_bars)

class Runner(object):

    def __init__(self, num_bars, simulation=True, led_strip=True):

        self.num_bars = num_bars
        self.audio = AudioProcessor()
        self.simulation = simulation
        self.led_strip = led_strip

        if simulation:
            self.simulation = Simulation(self.audio, self.num_bars)

        if led_strip:
            self.led_strip = LedStrip(self.num_bars)


    def run(self):
        while True:
            self.audio.capture_audio()
            if self.simulation:
                self.simulation.draw()
            if self.led_strip:
                self.led_strip.update()

if __name__ == '__main__':
    runner = Runner(10, True, False)
    runner.run()
