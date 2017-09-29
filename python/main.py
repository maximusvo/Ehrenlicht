import struct
import numpy as np
import pyaudio
import pygame, sys, time, random
from pygame.locals import *

# Audio settings

nFFT = int(512)
buffer_size = 4 * nFFT
format = pyaudio.paInt16
channels = 1
rate = 44100

pygame.init()
mainClock = pygame.time.Clock()

# Video settings
WINDOWWIDTH = 400
WINDOWHEIGHT = 400
BLACK = (0, 0, 0)

num_bars = 10
bar_size = round(WINDOWHEIGHT / num_bars)
bar_sound_size = round(nFFT/2 / num_bars)

windowSurface = pygame.display.set_mode((WINDOWWIDTH, WINDOWHEIGHT), 0, 32)

def capture_audio(stream, MAX_y):
  # Read n*nFFT frames from stream, n > 0
  num_frames = max(stream.get_read_available() / nFFT, 1) * nFFT
  data = stream.read(int(num_frames))

  # Unpack data, LRLRLR...
  y = np.array(struct.unpack("%dh" % (num_frames * channels), data)) / MAX_y
  y_L = y[::2]
  y_R = y[1::2]
  #y_R = y_R[::-1]

  Y_L = np.fft.fft(y_L, nFFT)
  Y_R = np.fft.fft(y_R, nFFT)

  # Sewing FFT of two channels together, DC part uses right channel's
  Y = abs(np.hstack((Y_L[-round(nFFT / 2):-1:-1], Y_R[:round(nFFT / 2)])))

  oY = Y[:round(len(Y)/2)] + Y[round(len(Y)/2):]

  print(len(oY))
  return oY

p = pyaudio.PyAudio()
# Used for normalizing signal. If use paFloat32, then it's already -1..1.
# Because of saving wave, paInt16 will be easier.
MAX_y = 2.0 ** (p.get_sample_size(format) * 8 - 1)
stream = p.open(format=format,
                channels=channels,
                rate=rate,
                input=True,
                frames_per_buffer=buffer_size)

while True:
    # check for the QUIT event
    for event in pygame.event.get():
        if event.type == QUIT:
            stream.stop_stream()
            stream.close()
            p.terminate()
            pygame.quit()
            sys.exit()

    y = capture_audio(stream, MAX_y)

    windowSurface.fill(BLACK)
    for i in range(num_bars):
        s = np.sum(y[i*bar_sound_size:(i + 1)*bar_sound_size])
        if s > 10:
            pygame.draw.rect(windowSurface, (255,0,0), (0, 2*i*bar_size, WINDOWWIDTH, 2*bar_size))

    pygame.display.update()
    mainClock.tick(40)
