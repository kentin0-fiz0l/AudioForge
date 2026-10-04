#!/usr/bin/env python3
"""
Simple trigger for AI Producer
Run this while Ableton is open with AI Producer activated

    python3 trigger_generate.py            # house
    python3 trigger_generate.py techno     # house, techno, dnb, hiphop or ambient
"""

import socket
import sys

PORT = 9000


def osc_string(text):
    """Encode a string the OSC way: null-terminated, padded to 4 bytes"""
    data = text.encode('utf-8') + b'\x00'
    return data + b'\x00' * (-len(data) % 4)


def send_generate(genre='house', port=PORT, host='127.0.0.1'):
    """Send the OSC message that makes AI Producer generate a track"""
    message = osc_string('/ai_producer/generate') + osc_string(',s') + osc_string(genre)

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.sendto(message, (host, port))


if __name__ == "__main__":
    genre = sys.argv[1] if len(sys.argv) > 1 else 'house'
    send_generate(genre)
    print(f"Sent a generate request for '{genre}'.")
    print("Nothing happens unless Live is open with AIProducer chosen as a")
    print("Control Surface (Settings -> Link, Tempo & MIDI). Live's Log.txt")
    print("shows what the script did.")
