#!/usr/bin/env python3
"""
Talk to AI Producer while it runs inside Live, and record feedback

Run this while Ableton is open with AI Producer activated. The first argument
is a command address; the rest are its arguments.

    python3 live_control.py /live/mixer
    python3 live_control.py /live/meters
    python3 live_control.py /live/track/set/volume_db "AI Bass" -3
    python3 live_control.py /live/track/set/pan 6 -0.2
    python3 live_control.py /live/track/set/send "AI Lead" 0 0.3
    python3 live_control.py /live/device/load master audio_effects/Limiter
    python3 live_control.py /live/device/params master 0
    python3 live_control.py /live/device/set master 0 1 0.5

A track is named by its index, its name, "master" or "return:0".

To record feedback, along with what Live was doing at that moment:

    python3 live_control.py feedback "the hi-hat is too bright" --source user
"""

import argparse
import datetime
import json
import pathlib
import socket
import struct
import sys

PORT = 9000
REPLY_ADDRESS = '/ai_producer/reply'
DEFAULT_LOG = 'feedback/log.jsonl'


def osc_string(text):
    """Encode a string the OSC way: null-terminated, padded to 4 bytes"""
    data = text.encode('utf-8') + b'\x00'
    return data + b'\x00' * (-len(data) % 4)


def encode(address, *args):
    """Build an OSC message from an address and int, float or string arguments"""
    tags, data = ',', b''
    for arg in args:
        if isinstance(arg, bool):
            raise TypeError("OSC booleans are not supported; send 0 or 1")
        if isinstance(arg, int):
            tags, data = tags + 'i', data + struct.pack('>i', arg)
        elif isinstance(arg, float):
            tags, data = tags + 'f', data + struct.pack('>f', arg)
        else:
            tags, data = tags + 's', data + osc_string(str(arg))
    return osc_string(address) + osc_string(tags) + data


def decode_reply(packet):
    """Read one reply message: its position, the total count and its text"""
    address_end = packet.index(b'\x00')
    if packet[:address_end].decode('utf-8') != REPLY_ADDRESS:
        return None

    position = (address_end + 4) & ~3          # past the address
    position = (packet.index(b'\x00', position) + 4) & ~3   # past the type tags
    index, count = struct.unpack('>ii', packet[position:position + 8])
    text_end = packet.index(b'\x00', position + 8)
    return index, count, packet[position + 8:text_end]


def request(address, *args, port=PORT, host='127.0.0.1', timeout=2.0):
    """Send a command and return the reply, or None if nothing answered"""
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.settimeout(timeout)
        sock.sendto(encode(address, *args), (host, port))

        chunks = {}
        try:
            while True:
                packet, _ = sock.recvfrom(65536)
                decoded = decode_reply(packet)
                if decoded is None:
                    continue
                index, count, text = decoded
                chunks[index] = text
                if len(chunks) == count:
                    return json.loads(b''.join(chunks[i] for i in range(count)).decode('utf-8'))
        except socket.timeout:
            return None


def live_context(port=PORT):
    """What Live is doing now: the mixer, the meters and every device's settings"""
    mixer = request('/live/mixer', port=port)
    if mixer is None:
        return None

    devices = []
    for track in mixer.get('tracks', []):
        for index, name in enumerate(track['devices']):
            params = request('/live/device/params', track['target'], index, port=port) or {}
            devices.append({'track': track['name'], 'device': name,
                            'params': {p[1]: p[5] for p in params.get('params', [])}})

    meters = request('/live/meters', port=port) or {}
    return {'mixer': mixer.get('tracks'), 'meters': meters.get('meters'), 'devices': devices}


def record_feedback(text, source='user', log_path=DEFAULT_LOG, port=PORT):
    """Append a piece of feedback, with Live's state, to the feedback log"""
    record = {
        'time': datetime.datetime.now().astimezone().isoformat(timespec='seconds'),
        'source': source,
        'text': text,
        'status': 'open',
        'live': live_context(port),
    }

    path = pathlib.Path(log_path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('a', encoding='utf-8') as log:
        log.write(json.dumps(record) + '\n')
    return record


def typed(text):
    """Turn a command-line word into an int, a float or leave it as a string"""
    for convert in (int, float):
        try:
            return convert(text)
        except ValueError:
            pass
    return text


def main(argv):
    if argv and argv[0] == 'feedback':
        parser = argparse.ArgumentParser(prog='live_control.py feedback')
        parser.add_argument('text')
        parser.add_argument('--source', default='user', help="who it came from: user, measurement or observation")
        parser.add_argument('--log', default=DEFAULT_LOG, help="feedback log, relative to the current directory")
        options = parser.parse_args(argv[1:])

        record = record_feedback(options.text, options.source, options.log)
        state = "with Live's state" if record['live'] else "without Live's state (AI Producer did not answer)"
        print(f"Recorded in {options.log}, {state}.")
        return 0

    if not argv or not argv[0].startswith('/'):
        print(__doc__)
        return 2

    reply = request(argv[0], *[typed(word) for word in argv[1:]])
    if reply is None:
        print("No reply. Either the command sends none, or Live is not running with")
        print("AIProducer chosen as a Control Surface (Settings -> Link, Tempo & MIDI).")
        return 1

    print(json.dumps(reply, indent=2))
    return 0 if reply.get('ok') else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
