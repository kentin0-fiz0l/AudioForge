# Clip Control for AI Producer
# Describes a clip, switches its warping, moves the start and end of what it
# plays, copies it to the Arrangement and clears a track's Arrangement,
# driven by OSC commands that send a reply
#
# A clip is named by its track and a slot: a number for a Session slot, or
# "@<beat>" for the Arrangement clip that is playing at that beat

import math


class ClipControl:
    """Reads and changes clips in the Session view and on the Arrangement timeline"""

    # Live keeps an unwarped clip's positions on whole samples, so a marker
    # or a copy's start comes back a fraction of a unit from what was asked for
    MARKER_TOLERANCE = 0.001

    def __init__(self, find_track, log_function):
        self.find_track = find_track  # Target (index or name) -> track
        self.log = log_function

        self.commands = {
            '/live/clip/info': self.get_info,
            '/live/clip/arrangement': self.list_arrangement,
            '/live/clip/duplicate': self.duplicate_to_arrangement,
            '/live/clip/set/warping': self.set_warping,
            '/live/clip/set/markers': self.set_markers,
            '/live/clip/clear_arrangement': self.clear_arrangement,
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

    def _clip(self, target, slot):
        track = self.find_track(target)

        if str(slot).startswith('@'):
            return self._arrangement_clip(track, float(str(slot)[1:]))

        # Return tracks have no slots at all
        slots = getattr(track, 'clip_slots', None)
        if slots is None:
            raise ValueError(f"'{track.name}' has no clip slots")

        slot_index = int(slot)
        if not 0 <= slot_index < len(slots):
            raise ValueError(f"No slot {slot_index}; '{track.name}' has {len(slots)}")

        clip = slots[slot_index].clip
        if clip is None:
            raise ValueError(f"Slot {slot_index} of '{track.name}' has no clip")
        return clip

    @classmethod
    def _arrangement_clip(cls, track, beat):
        """The clip on the track's timeline that is playing at a beat.

        A clip that starts a hair after the beat counts too: Live puts a copy
        on a whole sample, so it can start just after the beat it was asked
        for, and the beat is then still inside the clip that was cut off there.
        """
        playing = [clip for clip in getattr(track, 'arrangement_clips', [])
                   if clip.start_time - cls.MARKER_TOLERANCE <= beat < clip.end_time]
        if not playing:
            raise ValueError(f"No clip at beat {beat:g} on the Arrangement of '{track.name}'")
        return max(playing, key=lambda clip: clip.start_time)

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

        # Where the clip sits on the timeline, in beats, for an Arrangement clip
        if getattr(clip, 'is_arrangement_clip', hasattr(clip, 'end_time')):
            description['start_time'] = clip.start_time
            description['end_time'] = clip.end_time

        # How long the file itself is, whatever part of it the clip plays
        samples = getattr(clip, 'sample_length', None)
        rate = getattr(clip, 'sample_rate', None)
        if audio and samples and rate:
            description['file_seconds'] = samples / float(rate)

        return description

    def get_info(self, target, slot):
        return self._describe(self._clip(target, slot))

    def list_arrangement(self, target):
        """Describe every clip a track has on the Arrangement timeline, in order"""
        track = self.find_track(target)
        clips = sorted(getattr(track, 'arrangement_clips', []), key=lambda clip: clip.start_time)
        return {'track': track.name, 'clips': [self._describe(clip) for clip in clips]}

    def duplicate_to_arrangement(self, target, slot, beat):
        """Copy a clip onto the track's timeline, starting at a beat.

        Live cuts whatever was already there under the copy. The copy of an
        unwarped audio clip spans the whole file, whatever the clip's own
        markers say; set/markers on the copy trims it.
        """
        track = self.find_track(target)
        clip = self._clip(target, slot)
        beat = float(beat)
        if not (math.isfinite(beat) and beat >= 0.0):
            raise ValueError("The beat has to be a number, at or after the start")

        track.duplicate_clip_to_arrangement(clip, beat)
        self.log(f"Copied '{clip.name}' to beat {beat:g} of '{track.name}'")
        return self._describe(self._arrangement_clip(track, beat))

    def _audio_clip(self, target, slot):
        clip = self._clip(target, slot)
        if not getattr(clip, 'is_audio_clip', False):
            raise ValueError(f"'{clip.name}' is not an audio clip")
        return clip

    def set_markers(self, target, slot, start, end):
        """Move the start and end of what an audio clip plays.

        The values are in the clip's own units, as info reports them: beats
        for a warped clip, seconds for an unwarped one.
        """
        clip = self._audio_clip(target, slot)
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

    def clear_arrangement(self, target):
        """Delete every clip a track has on the Arrangement timeline"""
        track = self.find_track(target)
        clips = list(getattr(track, 'arrangement_clips', []))

        for clip in clips:
            track.delete_clip(clip)

        self.log(f"Cleared {len(clips)} clip(s) from the Arrangement of '{track.name}'")
        return {'track': track.name, 'deleted': len(clips)}

    @staticmethod
    def _set_pair(clip, start_name, end_name, start, end):
        """Set a start and an end that Live will not let cross each other"""
        if start >= getattr(clip, end_name):
            setattr(clip, end_name, end)
            setattr(clip, start_name, start)
        else:
            setattr(clip, start_name, start)
            setattr(clip, end_name, end)

    def set_warping(self, target, slot, on):
        """Switch a clip's warping on or off.

        Live warps a long file to its own guess at the tempo when the file is
        imported. Stems of one song each get a different guess, and then no
        longer line up. With warping off a clip plays as recorded.
        """
        clip = self._audio_clip(target, slot)
        clip.warping = bool(int(on))
        self.log(f"Warping {'on' if clip.warping else 'off'} for '{clip.name}'")
        return self._describe(clip)
