# Clip Control for AI Producer
# Describes a Session clip, switches its warping, and moves the start and
# end of what it plays, driven by OSC commands that send a reply

import math


class ClipControl:
    """Reads and changes clips in the Session view"""

    # Live keeps an unwarped clip's positions on whole samples, so a marker
    # comes back a fraction of a millisecond from what was asked for
    MARKER_TOLERANCE = 0.001

    def __init__(self, find_track, log_function):
        self.find_track = find_track  # Target (index or name) -> track
        self.log = log_function

        self.commands = {
            '/live/clip/info': self.get_info,
            '/live/clip/set/warping': self.set_warping,
            '/live/clip/set/markers': self.set_markers,
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
            self.log(f"Clip error for {address}: {e}")
            return {'ok': False, 'error': str(e)}

    def _clip(self, target, slot_index):
        track = self.find_track(target)

        # Return tracks have no slots at all
        slots = getattr(track, 'clip_slots', None)
        if slots is None:
            raise ValueError(f"'{track.name}' has no clip slots")

        slot_index = int(slot_index)
        if not 0 <= slot_index < len(slots):
            raise ValueError(f"No slot {slot_index}; '{track.name}' has {len(slots)}")

        clip = slots[slot_index].clip
        if clip is None:
            raise ValueError(f"Slot {slot_index} of '{track.name}' has no clip")
        return clip

    @staticmethod
    def _describe(clip):
        audio = bool(getattr(clip, 'is_audio_clip', False))
        description = {
            'name': clip.name,
            'audio': audio,
            'length': clip.length,
            'warping': bool(clip.warping) if audio else None,
            'start_marker': getattr(clip, 'start_marker', None),
            'end_marker': getattr(clip, 'end_marker', None),
            'loop_start': getattr(clip, 'loop_start', None),
            'loop_end': getattr(clip, 'loop_end', None),
            'looping': bool(getattr(clip, 'looping', False)),
        }

        # How long the file itself is, whatever part of it the clip plays
        samples = getattr(clip, 'sample_length', None)
        rate = getattr(clip, 'sample_rate', None)
        if audio and samples and rate:
            description['file_seconds'] = samples / float(rate)

        return description

    def get_info(self, target, slot_index):
        return self._describe(self._clip(target, slot_index))

    def _audio_clip(self, target, slot_index):
        clip = self._clip(target, slot_index)
        if not getattr(clip, 'is_audio_clip', False):
            raise ValueError(f"'{clip.name}' is not an audio clip")
        return clip

    def set_markers(self, target, slot_index, start, end):
        """Move the start and end of what an audio clip plays.

        The values are in the clip's own units, as info reports them: beats
        for a warped clip, seconds for an unwarped one.
        """
        clip = self._audio_clip(target, slot_index)
        start, end = float(start), float(end)
        if not (math.isfinite(start) and math.isfinite(end) and start < end):
            raise ValueError("The start has to come before the end, and both have to be numbers")

        # With looping off, the loop positions are the clip's own start and
        # end. The markers cannot go outside them: Live ignores a start
        # marker placed before the clip's start, without an error. So move
        # the clip's own bounds first.
        bounds = [('start_marker', 'end_marker')]
        if not getattr(clip, 'looping', False):
            bounds.insert(0, ('loop_start', 'loop_end'))

        before = {name: getattr(clip, name) for pair in bounds for name in pair}

        try:
            for start_name, end_name in bounds:
                self._set_pair(clip, start_name, end_name, start, end)

            # Live can drop a value without saying so; a reply of "ok" has
            # to mean the markers are where they were asked to be
            if (abs(clip.start_marker - start) > self.MARKER_TOLERANCE
                    or abs(clip.end_marker - end) > self.MARKER_TOLERANCE):
                raise ValueError(f"The markers did not take: Live has them at "
                                 f"{clip.start_marker} and {clip.end_marker}")
        except Exception:
            # Leave the clip as it was found
            for start_name, end_name in reversed(bounds):
                self._set_pair(clip, start_name, end_name, before[start_name], before[end_name])
            raise

        return self._describe(clip)

    @staticmethod
    def _set_pair(clip, start_name, end_name, start, end):
        """Set a start and an end that Live will not let cross each other"""
        if start >= getattr(clip, end_name):
            setattr(clip, end_name, end)
            setattr(clip, start_name, start)
        else:
            setattr(clip, start_name, start)
            setattr(clip, end_name, end)

    def set_warping(self, target, slot_index, on):
        """Switch a clip's warping on or off.

        Live warps a long file to its own guess at the tempo when the file is
        imported. Stems of one song each get a different guess, and then no
        longer line up. With warping off a clip plays as recorded.
        """
        clip = self._audio_clip(target, slot_index)
        clip.warping = bool(int(on))
        self.log(f"Warping {'on' if clip.warping else 'off'} for '{clip.name}'")
        return self._describe(clip)
