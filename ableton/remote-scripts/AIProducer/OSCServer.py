# OSC Server for AI Producer Remote Script
# Receives commands from the chat interface

import socket
import struct

class OSCServer:
    """Simple OSC server for receiving commands"""

    def __init__(self, port=9000):
        self.port = port
        self.sock = None
        self.running = False

        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.sock.bind(('127.0.0.1', port))
            self.sock.setblocking(False)
            self.running = True
        except Exception as e:
            print(f"[AI Producer] Could not start OSC server: {e}")

    def receive(self):
        """Check for incoming OSC messages (non-blocking)"""
        if not self.running:
            return None

        try:
            data, addr = self.sock.recvfrom(1024)
            return self._parse_osc(data)
        except socket.error:
            # No data available (non-blocking)
            return None
        except Exception as e:
            return None

    def _parse_osc(self, data):
        """Parse OSC message"""
        try:
            # Simple OSC parsing (address pattern + type tags + arguments)
            # Format: /address\x00,args\x00...

            # Find null terminator for address
            null_idx = data.find(b'\x00')
            if null_idx == -1:
                return None

            address = data[:null_idx].decode('utf-8')

            # Find type tags (starts with comma)
            type_start = null_idx + 1
            while type_start < len(data) and data[type_start:type_start+1] != b',':
                type_start += 1

            if type_start >= len(data):
                # No arguments
                return {'address': address, 'args': []}

            # Skip the comma and get type tags
            type_start += 1
            type_end = data.find(b'\x00', type_start)
            if type_end == -1:
                return {'address': address, 'args': []}

            type_tags = data[type_start:type_end].decode('utf-8')

            # Parse arguments based on type tags
            args = []
            arg_start = type_end + 1
            # Align to 4-byte boundary
            while arg_start % 4 != 0:
                arg_start += 1

            for tag in type_tags:
                if tag == 'i':  # int32
                    if arg_start + 4 <= len(data):
                        value = struct.unpack('>i', data[arg_start:arg_start+4])[0]
                        args.append(value)
                        arg_start += 4
                elif tag == 'f':  # float32
                    if arg_start + 4 <= len(data):
                        value = struct.unpack('>f', data[arg_start:arg_start+4])[0]
                        args.append(value)
                        arg_start += 4
                elif tag == 's':  # null-terminated string, padded to 4 bytes
                    string_end = data.find(b'\x00', arg_start)
                    if string_end != -1:
                        args.append(data[arg_start:string_end].decode('utf-8'))
                        arg_start = (string_end + 4) & ~3  # skip the null, round up to a multiple of 4

            return {'address': address, 'args': args}

        except Exception as e:
            return None

    def close(self):
        """Close the OSC server"""
        if self.sock:
            self.sock.close()
        self.running = False
