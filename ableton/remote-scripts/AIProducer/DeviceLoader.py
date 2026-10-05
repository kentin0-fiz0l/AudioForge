# Device Loader for AI Producer
# Puts an instrument on each track the script generates


class DeviceLoader:
    """Loads an instrument from Live's browser onto a track"""

    # What to try for each kind of track, in order. An AudioForge plugin
    # comes first. After it comes something every edition of Live has,
    # Intro included, so nothing here needs Suite. For drums that is a kit:
    # an empty Drum Rack makes no sound.
    CHOICES = {
        'drums': [('plugins', 'DrumSynth'), ('drums', '909 Core Kit'), ('drums', '808 Core Kit')],
        'bass': [('plugins', 'BasicSynth'), ('instruments', 'Drift')],
        'chords': [('plugins', 'ElectricPiano'), ('instruments', 'Drift')],
        'lead': [('plugins', 'Polysynth'), ('instruments', 'Drift')],
    }

    # The factory patch to choose after loading: its position, and how many
    # the plugin has. A JUCE plugin offers its patches to the host as one
    # parameter; whether Live lists it has not been checked in Live.
    PATCHES = {
        ('bass', 'BasicSynth'): (1, 4),  # Init, Bass, Pad, Lead
    }
    PATCH_PARAMETER = 'Program'

    # How deep to look under Plug-Ins: format, maker, plugin
    PLUGIN_DEPTH = 3

    def __init__(self, app, song, log_function):
        self.app = app
        self.song = song
        self.log = log_function

    def load_instrument(self, track, instrument_type):
        """Load an instrument for this kind of track. Returns its name, or None.

        Never raises: a track with no instrument still gets its clip.
        """
        try:
            return self._load(track, instrument_type)
        except Exception as e:
            self.log(f"Could not load an instrument on '{track.name}': {str(e)}")
            return None

    def _load(self, track, instrument_type):
        if len(track.devices) > 0:
            self.log(f"'{track.name}' already has '{track.devices[0].name}'; leaving it")
            return track.devices[0].name

        for root, name in self.CHOICES.get(instrument_type, []):
            item = self._find(root, name)
            if item is None:
                continue

            # The browser loads onto whichever track is selected
            self.song.view.selected_track = track
            self.app.browser.load_item(item)

            if len(track.devices) == 0:
                self.log(f"Asked Live to load '{item.name}' on '{track.name}'; it is not there yet")
                return item.name

            self.log(f"Loaded '{item.name}' on '{track.name}'")
            self._choose_patch(track.devices[0], instrument_type, name)
            return item.name

        wanted = ' or '.join(name for _, name in self.CHOICES.get(instrument_type, []))
        self.log(f"No instrument could be loaded on '{track.name}'. "
                 f"Put one on it to hear its clip: {wanted}.")
        return None

    def _find(self, root, name):
        """A loadable browser item with this name, or None"""
        top = getattr(self.app.browser, root, None)
        if top is None:
            return None

        return self._search(top, name, self.PLUGIN_DEPTH if root == 'plugins' else 1)

    def _search(self, folder, name, depth):
        children = list(folder.children)

        for child in children:
            if child.is_loadable and self._matches(child.name, name):
                return child

        if depth > 1:
            # VST3 before Audio Units, so the same format is used every time
            for child in sorted(children, key=lambda c: c.name != 'VST3'):
                found = self._search(child, name, depth - 1)
                if found is not None:
                    return found

        return None

    @staticmethod
    def _matches(item_name, wanted):
        """Ignores case, a preset's file extension, and the 'AudioForge - '
        that some of the plugins were named with"""
        item_name, wanted = item_name.lower(), wanted.lower()
        if item_name.rsplit('.', 1)[0] == wanted:
            return True
        return item_name in (wanted, 'audioforge - ' + wanted)

    def _choose_patch(self, device, instrument_type, name):
        patch = self.PATCHES.get((instrument_type, name))
        if patch is None:
            return

        index, count = patch

        for param in device.parameters:
            if param.name == self.PATCH_PARAMETER:
                param.value = param.min + (param.max - param.min) * index / (count - 1)
                self.log(f"Chose patch {index + 1} of {count} on '{device.name}'")
                return

        self.log(f"'{device.name}' does not show its patches to Live; "
                 f"choose patch {index + 1} in its window")
