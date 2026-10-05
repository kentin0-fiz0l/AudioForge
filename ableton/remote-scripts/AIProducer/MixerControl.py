# Mixer Control for AI Producer
# Faders, pans, sends, devices and level meters for any track, including
# the master and return tracks, driven by OSC commands that send a reply

import math


class MixerControl:
    """Reads and changes the mixer and device chains of the Live set"""

    BROWSER_ROOTS = ('audio_effects', 'instruments', 'drums', 'sounds', 'midi_effects',
                     'plugins', 'packs', 'user_library', 'samples', 'max_for_live', 'clips')

    def __init__(self, song, app, log_function):
        self.song = song
        self.app = app
        self.log = log_function
        self.metering = False
        self.meter_peaks = {}

        self.commands = {
            '/live/mixer': self.get_mixer,
            '/live/meters': self.get_meters,
            '/live/track/set/volume': self.set_volume,
            '/live/track/set/volume_db': self.set_volume_db,
            '/live/track/set/pan': self.set_pan,
            '/live/track/set/send': self.set_send,
            '/live/device/load': self.load_device,
            '/live/device/params': self.get_device_params,
            '/live/device/set': self.set_device_param,
            '/live/device/delete': self.delete_device,
            '/live/song/clear_locators': self.clear_locators,
        }

    def handles(self, address):
        return address in self.commands

    def handle(self, address, args):
        """Run a command and return its reply as a dictionary"""
        try:
            reply = self.commands[address](*args)
            reply['ok'] = True
            return reply
        except Exception as e:
            self.log(f"Mixer error for {address}: {str(e)}")
            return {'ok': False, 'error': str(e)}

    # ------------------------------------------------------------------
    # Finding tracks

    def _all_tracks(self):
        """Every track with its target name: an index, 'return:N' or 'master'"""
        tracks = list(enumerate(self.song.tracks))
        tracks += [(f'return:{i}', t) for i, t in enumerate(self.song.return_tracks)]
        tracks.append(('master', self.song.master_track))
        return tracks

    def find_track(self, target):
        """Find a track by index, by 'master', by 'return:N' or by its name"""
        return self._track(target)

    def _track(self, target):
        """Find a track by index, by 'master', by 'return:N' or by its name"""
        text = str(target).strip()

        if text.lstrip('-').isdigit():
            return self._at(self.song.tracks, text, 'track', 'the set')

        # 'master' and 'return:N' win over a track that happens to share the name
        for name, track in self._all_tracks():
            if name == text.lower():
                return track

        for _, track in self._all_tracks():
            if track.name == text:
                return track

        raise ValueError(f"No track called '{text}'")

    @staticmethod
    def _at(items, index, noun, owner):
        """The item at an index, or an error saying how many there are"""
        index = int(index)
        if not 0 <= index < len(items):
            raise ValueError(f"No {noun} {index}; {owner} has {len(items)}")
        return items[index]

    # ------------------------------------------------------------------
    # Reading the mixer

    @staticmethod
    def _describe_parameter(param):
        return {'value': param.value, 'display': param.str_for_value(param.value)}

    def get_mixer(self):
        """Names, levels, pans, sends and devices of every track"""
        tracks = []
        for target, track in self._all_tracks():
            mixer = track.mixer_device
            tracks.append({
                'target': target,
                'name': track.name,
                'volume': self._describe_parameter(mixer.volume),
                'pan': mixer.panning.value,
                'sends': [send.value for send in mixer.sends],
                'devices': [device.name for device in track.devices],
            })
        return {'tracks': tracks}

    def update_meters(self):
        """Remember the loudest meter reading per track; called on every display update"""
        if not self.metering:
            return

        # This runs inside Live's display loop, so it must never raise
        try:
            for target, track in self._all_tracks():
                if track.has_audio_output:
                    level = max(track.output_meter_left, track.output_meter_right)
                    self.meter_peaks[target] = max(self.meter_peaks.get(target, 0.0), level)
        except Exception as e:
            self.metering = False
            self.log(f"Meters switched off: {str(e)}")

    def get_meters(self):
        """Meter readings now, and the highest seen since the last request.

        Readings are Live's meter values from 0 to 1, not decibels.
        """
        self.metering = True

        meters = []
        for target, track in self._all_tracks():
            if track.has_audio_output:
                left, right = track.output_meter_left, track.output_meter_right
                meters.append({
                    'target': target,
                    'name': track.name,
                    'left': left,
                    'right': right,
                    'peak': max(self.meter_peaks.get(target, 0.0), left, right),
                })

        self.meter_peaks = {}
        return {'meters': meters}

    # ------------------------------------------------------------------
    # Changing the mixer

    @staticmethod
    def _set(param, value):
        value = float(value)
        if math.isnan(value):
            raise ValueError("Not a number")
        param.value = max(param.min, min(param.max, value))
        return MixerControl._describe_parameter(param)

    @staticmethod
    def _shown_db(param, value):
        text = param.str_for_value(value)
        if 'inf' in text or '\u221e' in text:  # Silence, shown as -inf or with the infinity sign
            return float('-inf')
        return float(text.split()[0])

    def set_volume(self, target, value):
        return self._set(self._track(target).mixer_device.volume, value)

    def set_volume_db(self, target, db):
        """Set a fader to the position Live displays as the given level"""
        param = self._track(target).mixer_device.volume
        db = float(db)
        if math.isnan(db):
            raise ValueError("Not a number")
        if db == float('-inf'):
            return self._set(param, param.min)

        # Live does not say how fader positions map to decibels, so search
        # for the positions it displays as this level. The display is
        # rounded, so a small range of positions matches: use its middle.
        # A level outside the fader's range ends up at the nearest end.
        start = self._first_position(param, lambda shown: shown >= db)
        end = self._first_position(param, lambda shown: shown > db)
        return self._set(param, (start + end) / 2.0)

    def _first_position(self, param, reached):
        """The lowest fader position whose displayed level satisfies the test"""
        low, high = param.min, param.max
        for _ in range(40):
            middle = (low + high) / 2.0
            if reached(self._shown_db(param, middle)):
                high = middle
            else:
                low = middle
        return high

    def set_pan(self, target, value):
        return self._set(self._track(target).mixer_device.panning, value)

    def set_send(self, target, send_index, value):
        sends = self._track(target).mixer_device.sends
        return self._set(self._at(sends, send_index, 'send', 'the track'), value)

    # ------------------------------------------------------------------
    # Locators

    def clear_locators(self):
        """Delete every locator in the Arrangement.

        Live only toggles a locator at the playhead, so the playhead visits
        each one and is then put back.
        """
        was_at = self.song.current_song_time
        times = [cue.time for cue in self.song.cue_points]

        for time in times:
            self.song.current_song_time = time
            self.song.set_or_delete_cue()

        self.song.current_song_time = was_at
        self.log(f"Deleted {len(times)} locator(s)")
        return {'deleted': len(times)}

    # ------------------------------------------------------------------
    # Devices

    def _device(self, target, device_index):
        return self._at(self._track(target).devices, device_index, 'device', 'the track')

    @staticmethod
    def _same_name(item_name, wanted):
        """Compare browser names ignoring case and a preset's file extension"""
        item_name, wanted = item_name.lower(), wanted.lower()
        return item_name == wanted or item_name.rsplit('.', 1)[0] == wanted

    def _browser_item(self, path):
        """Walk Live's browser, e.g. 'audio_effects/Limiter'"""
        parts = [part for part in str(path).split('/') if part]
        if not parts or parts[0].lower() not in self.BROWSER_ROOTS:
            raise ValueError(f"Path must start with one of: {', '.join(self.BROWSER_ROOTS)}")

        item = getattr(self.app.browser, parts[0].lower())
        for name in parts[1:]:
            match = [child for child in item.children if self._same_name(child.name, name)]
            if not match:
                raise ValueError(f"'{name}' not found in the browser under '{item.name}'")
            item = match[0]

        if not item.is_loadable:
            raise ValueError(f"'{item.name}' is a folder, not something that can be loaded")
        return item

    def load_device(self, target, path):
        """Load a device or preset from the browser onto a track"""
        track = self._track(target)
        item = self._browser_item(path)

        # The browser loads onto whichever track is selected
        self.song.view.selected_track = track
        self.app.browser.load_item(item)
        self.log(f"Loaded '{item.name}' on '{track.name}'")
        return {'loaded': item.name, 'track': track.name}

    def get_device_params(self, target, device_index):
        """Every parameter of a device: index, name, value, range and displayed value"""
        device = self._device(target, device_index)
        params = [[i, p.name, p.value, p.min, p.max, p.str_for_value(p.value)]
                  for i, p in enumerate(device.parameters)]
        return {'device': device.name, 'params': params}

    def set_device_param(self, target, device_index, param_index, value):
        params = self._device(target, device_index).parameters
        param = self._at(params, param_index, 'parameter', 'the device')
        reply = self._set(param, value)
        reply['name'] = param.name
        return reply

    def delete_device(self, target, device_index):
        track = self._track(target)
        name = self._at(track.devices, device_index, 'device', 'the track').name
        track.delete_device(int(device_index))
        self.log(f"Deleted '{name}' from '{track.name}'")
        return {'deleted': name, 'track': track.name}
