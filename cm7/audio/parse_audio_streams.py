#! /usr/bin/env python3
'''
Copyright 2019, 2021-2022 NXP.
This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.
'''

import argparse
import serial
import os
import sys
import shutil
import wave
from scipy.io import wavfile
import threading
import signal

# Configuration variables. Adjust to your needs
CONVERT_RAW_TO_WAV = True
KEEP_RAW_FILES = False

# Enable to monitor detected wake words.
WW_SPIKES_ENABLED = False

# Global variables
STOP_PROCESS = False
SER = None
MICS_CNT = '3'

def signal_handler(sig, frame):
    global STOP_PROCESS

    print('--> CTRL + C pressed')
    STOP_PROCESS = True


def wait_cancel_from_user():
    global STOP_PROCESS

    while True:
        try:
            cmd = input("Type 'q' and press Enter to cancel the process >> ")
            if cmd == 'q':
                STOP_PROCESS = True
        except:
            STOP_PROCESS = True
            break

        if STOP_PROCESS:
            break

def print_offset(total_samples, ww_start_offset_samples):
    if WW_SPIKES_ENABLED:
        end_ms       = total_samples / 16
        end_mseconds = end_ms % 1000
        end_seconds  = (end_ms / 1000) % 60
        end_minutes  = (end_ms / (1000 * 60)) % 60
        end_hours    = (end_ms / (1000 * 60 * 60)) % 24

        start_ms       = (total_samples - ww_start_offset_samples) / 16
        start_mseconds = start_ms % 1000
        start_seconds  = (start_ms / 1000) % 60
        start_minutes  = (start_ms / (1000 * 60)) % 60
        start_hours    = (start_ms / (1000 * 60 * 60)) % 24

        ww_duration_ms = end_ms - start_ms

        print ("WW start %02d:%02d:%02d:%03d   ---   WW end %02d:%02d:%02d:%03d   ---   WW duration %d [ms]" % (start_hours, start_minutes, start_seconds, start_mseconds, end_hours, end_minutes, end_seconds, end_mseconds, ww_duration_ms))

def capture_g_afe():
    global STOP_PROCESS

    if WW_SPIKES_ENABLED:
        amp_file_size = 0
        ww_start_spike_size = 320
        ww_start_spike_buff = bytes([50] * ww_start_spike_size)
        ww_detected_spike_size = 320
        ww_detected_spike_buff = bytes([100] * ww_detected_spike_size)

    while(1):
        if STOP_PROCESS:
            print("--> Recording Stopped")
            break

        ser_data = SER.read(320)
        mic1_file.write(ser_data)

        ser_data = SER.read(320)
        mic2_file.write(ser_data)

        if MICS_CNT == '3':
            ser_data = SER.read(320)
            mic3_file.write(ser_data)

        ser_data = SER.read(320)
        amp_file.write(ser_data)

        if WW_SPIKES_ENABLED:
            # Add Wake Word spikes
            amp_file_size += 320
            check1 = int(ser_data[0] | ser_data[1] << 8)
            check2 = int(ser_data[2] | ser_data[3] << 8)
            ww_len = int(ser_data[4] | ser_data[5] << 8)
            if check1 == 13 and check2 == 14 and ww_len != 0:
                # Add Wake Word start spike
                amp_file.seek(amp_file_size - ww_len * 2)
                amp_file.write(ww_start_spike_buff)
                amp_file.seek(amp_file_size)

                # Add Wake Word detected spike
                amp_file.seek(amp_file_size - ww_detected_spike_size)
                amp_file.write(ww_detected_spike_buff)
                amp_file.seek(amp_file_size)
                print_offset(amp_file_size / 2, ww_len)

        ser_data = SER.read(320)
        clean_audio_file.write(ser_data)

        if not ser_data:
            break

def convert_raw_to_wav(file_path):
    new_file_path = file_path[:-3] + "wav"

    with open(file_path, "rb") as inp_f:
        data = inp_f.read()
        with wave.open(new_file_path, "wb") as out_f:
            out_f.setnchannels(1)
            out_f.setsampwidth(2) # number of bytes
            out_f.setframerate(16000)
            out_f.writeframesraw(data)

    if not KEEP_RAW_FILES:
        os.remove(file_path)


""" Parse the provided parameters """
parser = argparse.ArgumentParser()
parser.add_argument('-m', '--microphones', type=int, required=True, choices=[2, 3], help="Set the number of microphones.")
parser.add_argument('-p', '--port', type=str, required=True, help="Set the port the device is connected. Example for Window COM3, for Linux /dev/ttyACM3.")
parser.add_argument('-f', '--folder', type=str, required=True, help="Folder where to save audio files. Note: overwrite existing folder.")
args = parser.parse_args()

# Get the Number of microphones
MICS_CNT = str(args.microphones)

# Get the Test name
test_name = args.folder

# Delete the Test with the same name (if exists)
try:
    shutil.rmtree(test_name)
except:
    pass

try:
    os.mkdir(test_name)
except OSError:
    print("--> ERROR: Creation of the directory %s failed" % test_name)
    sys.exit(1)

MIC1_STREAM_PATH                = test_name + "/" + test_name + "_mic1.raw"
MIC2_STREAM_PATH                = test_name + "/" + test_name + "_mic2.raw"
MIC3_STREAM_PATH                = test_name + "/" + test_name + "_mic3.raw"
AMP_STREAM_PATH                 = test_name + "/" + test_name + "_amp.raw"
CLEAN_STREAM_PATH               = test_name + "/" + test_name + "_clean_processed_audio.raw"
AMP_PRE_DOWNSAMPLED_STREAM_PATH = test_name + "/" + test_name + "_amp_pre_downsampled.raw"


mic1_file        = open(MIC1_STREAM_PATH,  "wb")
mic2_file        = open(MIC2_STREAM_PATH,  "wb")
amp_file         = open(AMP_STREAM_PATH,   "w+b")
clean_audio_file = open(CLEAN_STREAM_PATH, "wb")

if MICS_CNT == '3':
    mic3_file = open(MIC3_STREAM_PATH, "wb")


# Get the Serial Port to the device
port_num = args.port

# Configure the serial connections (the parameters differs on the device you are connecting to)
try:
    SER = serial.Serial(
        port=port_num,
        baudrate=2048000,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        inter_byte_timeout=2,
        write_timeout=1,
    )
except:
    print("--> ERROR: Failed to connect to " + port_num + " serial port")
    sys.exit(1)

signal.signal(signal.SIGINT, signal_handler)

try:
    SER.write(b'c')
except:
    print("--> WARNING: Start write timeout. Probably audio_dump was already started...")


# Start recording. User has to type "stop" to cancel the process
stop_thread = threading.Thread(target=wait_cancel_from_user)
stop_thread.start()
capture_g_afe()
stop_thread.join()

# Close the files
mic1_file.close()
mic2_file.close()
if MICS_CNT == '3':
    mic3_file.close()

amp_file.close()
clean_audio_file.close()

# Convert RAW files to WAV files
if CONVERT_RAW_TO_WAV:
    print("--> Converting RAW to WAV")
    convert_raw_to_wav(MIC1_STREAM_PATH)
    convert_raw_to_wav(MIC2_STREAM_PATH)
    if MICS_CNT == '3':
        convert_raw_to_wav(MIC3_STREAM_PATH)

    convert_raw_to_wav(AMP_STREAM_PATH)
    convert_raw_to_wav(CLEAN_STREAM_PATH)
