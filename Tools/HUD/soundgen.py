'''
Created on 4 Aug 2014

@author: matt
'''
import pyaudio
import numpy
from math import *
import time
import Queue
import sys
import threading

CHUNK = 1024
TABLE_LENGTH = 1024
RATE = 11025
CHANNELS = 2

print ("system platform = " + sys.platform)
if sys.platform == 'darwin':
    CHANNELS = 1



def sine(frequency, time, rate):
    length = int(time * rate)
    factor = float(frequency) * (pi * 2) / rate
    return numpy.sin(numpy.arange(length) * factor)


def cb(in_data, frame_count, time_info, status):
    flags = status
#    print("in_data[0], in_data_len, frame count, status", in_data[0], len(in_data), frame_count, status)
    if flags != 0:
        if flags & pyaudio.paInputOverflow: print("Input Overflow")
        if flags & pyaudio.paInputUnderflow: print("Input Underflow")
        if flags & pyaudio.paOutputOverflow: print("Output Overflow")
        if flags & pyaudio.paOutputUnderflow: print("Output Underflow")
        if flags & pyaudio.paPrimingOutput: print("Priming Output")
    if(my_sgen != None):
        return my_sgen.callback(in_data, frame_count, time_info, status)
    else:
        return (None, pyaudio.paComplete)
class soundgen(object):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        self.wave = sine(1.0, 1.0, RATE)
        self.phase = 0.0
        self.frequency = 300.0
        
        self.attack = 0.01
        self.decay = 0.05
        self.hold = 0.05
        self.rate = 4
        
        self.gen_buffer = numpy.zeros(CHUNK * CHANNELS, numpy.float32)
        self.chunks = Queue.Queue(3)
        self.amplitude = 0.25

        self.stream = None
        
        self.quietchunk = (self.gen_buffer.astype(numpy.float32).tostring())
        
        self.stop_flag = threading.Event()
        
        self.sgen_thread = threading.Thread(target=self.sgen_app)
        self.sgen_thread.daemon = True
        self.sgen_thread.start()
    
    
    def sgen_app(self):
        self.open_stream()
        self.run()
        self.close_stream()
        
    def app_running(self):
        return self.sgen_thread.isAlive()
        
    def open_stream(self):
        self.stream = p.open(format=pyaudio.paFloat32,
                                  channels=CHANNELS, rate=RATE, output=True, stream_callback=cb) #self.callback
        
    def stop(self):
        self.stop_flag.set()        
            
    def callback(self, in_data, frame_count, time_info, status):
#        print("in_data[0], in_data_len, frame count, status", in_data[0], len(in_data), frame_count, status)
        try:
            self.outchunk = self.chunks.get_nowait()
            self.chunks.task_done()
            return (self.outchunk, pyaudio.paContinue)
        except:
            print("no chunks available, playing quiet chunk")
            return ( self.quietchunk, pyaudio.paContinue)


    def gen_sound(self):
        phase_delta = self.frequency * (float(TABLE_LENGTH) / float(RATE) )
        for i in xrange(0, CHUNK-1):
            self.phase += phase_delta
            if(self.phase >= TABLE_LENGTH):
                self.phase -= TABLE_LENGTH
            if(CHANNELS == 2):
                self.gen_buffer[i*2] = self.wave[int(self.phase)] * self.amplitude
                self.gen_buffer[(i*2)+1] = self.wave[int(self.phase)] * self.amplitude
            else:
                self.gen_buffer[i] = self.wave[int(self.phase)] * self.amplitude
            time.sleep(0)   #yield to callback that needs to run quickly

        
        chunk = (self.gen_buffer.astype(numpy.float32).tostring())
        try:
            self.outchunk = chunk
            self.chunks.put_nowait(chunk)
        except:
            print("not enough space in chunk queue")
            
    def run(self):
        if(self.stream == None): return
        
        endtime = time.time() + 10
        
        while time.time() < endtime:
            if(not self.chunks.full()):
                self.gen_sound()
                self.frequency += 1
            else:
                if(self.stream.is_active() == False):
                    self.stream.start_stream()
                time.sleep(0.01)
    
                
        print("ended soundgen run")
#            if(self.first_chunk == True):
#                self.first_chunk = False
               
#        self.stream.stop_stream()
#        self.stream.close()
#        time.sleep(0.1)
        
        #self.stream.write(self.buffer.astype(numpy.float32).tostring()
        
    def close_stream(self):
        self.stream.stop_stream()
        while(self.stream.is_active()):
            time.sleep(0.1)
        self.stream.close()
        print("closed soundgen stream")
#        self.sgen_thread.join()
        
if __name__ == '__main__':
    
    p = pyaudio.PyAudio()
    my_sgen = soundgen()
    #raw_input("Press key to exit...")
    #myvario.stop()
    while(my_sgen.app_running()):
        time.sleep(0.5)
    p.terminate()
    
    print("finished soundgen main")