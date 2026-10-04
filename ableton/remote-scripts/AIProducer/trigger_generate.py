#!/usr/bin/env python3
"""
Simple trigger for AI Producer
Run this while Ableton is open with AI Producer activated
"""

import socket
import sys

def send_osc_trigger():
    """Send OSC message to trigger generation"""
    # This is a simple approach - in reality, the Remote Script
    # needs to be triggered via MIDI or internal mechanisms
    print("To trigger AI Producer:")
    print("1. Make sure Ableton is open")
    print("2. Preferences → Link, Tempo & MIDI")
    print("3. Control Surface: AIProducer")
    print("4. Press MIDI note C3 (middle C) on your keyboard")
    print("")
    print("Or edit AIProducer.py to auto-generate on load!")

if __name__ == "__main__":
    send_osc_trigger()
