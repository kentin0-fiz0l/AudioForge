"""
Tests for the AI Producer Remote Script.

Live runs Remote Scripts inside its own Python, with a `Live` module that
exists nowhere else. These tests stand in a small fake for the parts of that
API the script uses, so the script can be imported and driven without Live.
They cannot show that Live accepts the calls, only that the script loads,
makes the calls it should, and makes them with sensible arguments.

Run from the repository root:

    python3 -m unittest discover -s ableton/tests -v
"""

import importlib
import json
import math
import pathlib
import sys
import tempfile
import threading
import time
import types
import unittest

SCRIPTS_DIR = pathlib.Path(__file__).resolve().parents[1] / "remote-scripts"


# --- A stand-in for the parts of Live's API the script touches ---------------

class FakeClip:
    def __init__(self, length):
        self.length = length
        self.name = ""
        self.loop_end = length
        self.notes = []

    def remove_notes_extended(self, from_pitch, pitch_span, from_time, time_span):
        # Live's order is pitch, pitch span, time, time span. A span of zero
        # removes nothing, which is how swapped arguments show up.
        if pitch_span <= 0 or time_span <= 0:
            raise ValueError("remove_notes_extended called with an empty span")
        self.notes = [n for n in self.notes
                      if not (from_pitch <= n[0] < from_pitch + pitch_span
                              and from_time <= n[1] < from_time + time_span)]

    def _check(self, notes):
        for pitch, start, duration, velocity, muted in notes:
            if not (0 <= pitch <= 127 and start >= 0 and duration > 0 and 1 <= velocity <= 127):
                raise ValueError(f"bad note {(pitch, start, duration, velocity, muted)}")

    def set_notes(self, notes):
        # Deprecated since Live 11, and still there
        FakeClip.deprecated_calls += 1
        self._check(notes)
        self.notes.extend(notes)

    def add_new_notes(self, specifications):
        # Live 11 and later: takes note specifications, not tuples
        notes = []
        for spec in specifications:
            if not isinstance(spec, FakeNoteSpecification):
                raise TypeError("add_new_notes takes MidiNoteSpecification objects")
            notes.append((spec.pitch, spec.start_time, spec.duration, spec.velocity, spec.mute))
        self._check(notes)
        self.notes.extend(notes)

    deprecated_calls = 0


class FakeNoteSpecification:
    """Live.Clip.MidiNoteSpecification, which takes keyword arguments only"""

    def __init__(self, *, pitch, start_time, duration, velocity=100, mute=False):
        self.pitch = pitch
        self.start_time = start_time
        self.duration = duration
        self.velocity = velocity
        self.mute = mute


class FakeAudioClip:
    """An audio clip as Live makes one from a long file: warped, on Live's own guess"""

    def __init__(self, name):
        self.name = name
        self.is_audio_clip = True
        self.length = 356.1
        self.warping = True
        self.warp_mode = 0

        # Live also moves the start of the clip to where it thinks the
        # first beat is
        self.loop_start = 1.15   # With looping off, these are the clip's own start and end
        self.loop_end = 145.38
        self.start_marker = 1.15
        self.end_marker = 145.38
        self.looping = False
        self.sample_length = 6411200
        self.sample_rate = 44100.0

    def __setattr__(self, name, value):
        # As in Live: the start cannot be put after the end, nor the end before the start
        if name == 'loop_start' and hasattr(self, 'loop_end') and value >= self.loop_end:
            raise RuntimeError("loop start must be before the loop end")
        if name == 'loop_end' and hasattr(self, 'loop_start') and value <= self.loop_start:
            raise RuntimeError("loop end must be after the loop start")

        # Seen in Live 12.4.6: with looping off, a start marker placed
        # before the clip's own start is ignored, with no error
        if (name == 'start_marker' and hasattr(self, 'loop_start')
                and not getattr(self, 'looping', False) and value < self.loop_start):
            return

        if name == 'start_marker' and hasattr(self, 'end_marker') and value >= self.end_marker:
            raise RuntimeError("start marker must be before the end marker")
        if name == 'end_marker' and hasattr(self, 'start_marker') and value <= self.start_marker:
            raise RuntimeError("end marker must be after the start marker")
        object.__setattr__(self, name, value)


class FakeOldClip(FakeClip):
    """A clip from before Live 11, which has no add_new_notes"""
    add_new_notes = None


class FakeClipSlot:
    def __init__(self):
        self.clip = None

    @property
    def has_clip(self):
        return self.clip is not None

    def create_clip(self, length):
        if self.clip is not None:
            raise RuntimeError("clip slot already has a clip")
        self.clip = FakeClip(length)

    def delete_clip(self):
        self.clip = None


class FakeParameter:
    def __init__(self, name, value, minimum=0.0, maximum=1.0, display=None):
        self.name = name
        self.value = value
        self.min = minimum
        self.max = maximum
        self._display = display or (lambda v: f"{v:.2f}")

    def str_for_value(self, value):
        return self._display(value)


def fader_display(value):
    # Like Live's faders: 0.85 is 0 dB, the bottom is silence, and the scale
    # in between is not something a script is told
    return "-inf dB" if value <= 0.0 else f"{(value - 0.85) * 40.0:.1f} dB"


class FakeDevice:
    def __init__(self, name, num_params=3):
        self.name = name
        self.parameters = [FakeParameter("Device On", 1.0)]
        self.parameters += [FakeParameter(f"{name} Param {i}", 0.5) for i in range(1, num_params)]

        # A plugin with factory patches shows them to Live as a parameter
        if name == "BasicSynth":
            self.parameters.append(FakeParameter("Program", 0.0))


class FakeTrack:
    def __init__(self, name="MIDI"):
        self.name = name
        self.clip_slots = [FakeClipSlot() for _ in range(8)]
        self.mixer_device = types.SimpleNamespace(
            volume=FakeParameter("Volume", 0.85, display=fader_display),
            panning=FakeParameter("Pan", 0.0, -1.0, 1.0),
            sends=[FakeParameter("A", 0.0), FakeParameter("B", 0.0)])
        self.devices = []
        self.arrangement_clips = []
        self.has_audio_output = True
        self.output_meter_left = 0.0
        self.output_meter_right = 0.0

    def delete_device(self, index):
        del self.devices[index]

    def delete_clip(self, clip):
        # Live's Track.delete_clip takes the clip itself, Session or Arrangement
        self.arrangement_clips.remove(clip)

    def duplicate_clip_to_arrangement(self, clip, destination_time):
        # Seen in Live 12.4.6: the copy of an unwarped audio clip spans the
        # whole file, whatever the clip's own markers say
        # Live puts an unwarped clip on a whole sample, so the copy can start
        # a few billionths of a beat from where it was asked to
        samples_per_beat = 44100.0 * 60.0 / 120.0
        copy = FakeAudioClip(clip.name)
        copy.start_time = math.ceil(destination_time * samples_per_beat) / samples_per_beat
        copy.end_time = copy.start_time + copy.length
        self.arrangement_clips.append(copy)
        self.arrangement_clips.sort(key=lambda c: c.start_time)
        return copy


class FakeBrowserItem:
    def __init__(self, name, children=(), loadable=None):
        self.name = name
        self.children = list(children)
        self.is_loadable = (not self.children) if loadable is None else loadable


class FakeBrowser:
    def __init__(self, song):
        self.song = song
        self.audio_effects = FakeBrowserItem("Audio Effects", [
            FakeBrowserItem("Limiter"),
            FakeBrowserItem("Utility"),
            FakeBrowserItem("Compressor", [FakeBrowserItem("Glue.adv")], loadable=True),
            FakeBrowserItem("Presets Only", [FakeBrowserItem("Wide.adg")]),
        ])

        # What Live Intro has, with the AudioForge plugins installed. Four
        # of them were named with a prefix, and one may still be.
        self.instruments = FakeBrowserItem("Instruments", [
            FakeBrowserItem("Drift"), FakeBrowserItem("Simpler"), FakeBrowserItem("Instrument Rack")])
        self.drums = FakeBrowserItem("Drums", [
            FakeBrowserItem("Drum Hits", [FakeBrowserItem("Kick 909.aif")]),
            FakeBrowserItem("Drum Rack"), FakeBrowserItem("808 Core Kit.adg"), FakeBrowserItem("909 Core Kit.adg")])
        self.plugins = FakeBrowserItem("Plug-Ins", [
            FakeBrowserItem("Audio Units", [FakeBrowserItem("AudioForge", [FakeBrowserItem("DrumSynth")])]),
            FakeBrowserItem("VST3", [FakeBrowserItem("AudioForge", [
                FakeBrowserItem("AudioForge - ElectricPiano"), FakeBrowserItem("BasicSynth"),
                FakeBrowserItem("DrumSynth"), FakeBrowserItem("Polysynth"), FakeBrowserItem("SimpleGain")])]),
        ])

    def load_item(self, item):
        self.song.view.selected_track.devices.append(FakeDevice(item.name.rsplit('.', 1)[0]))


class FakeSong:
    def __init__(self):
        self.tempo = 120.0
        self.tracks = []
        self.return_tracks = [FakeTrack("A-Reverb"), FakeTrack("B-Delay")]
        self.master_track = FakeTrack("Master")
        self.view = types.SimpleNamespace(selected_track=None)
        self.is_playing = False
        self.time_listeners = []
        self.insert_marker = 136.0
        self._cue_points = []
        self._time = 0.0
        self._pending_time = None

    # Seen in Live 12.4.6: a new song time is applied a moment after it is
    # set, so a toggle in the same breath lands at the old position. Here
    # the move lands when time passes, which is when the song is next looked at.
    @property
    def current_song_time(self):
        return self._time

    @current_song_time.setter
    def current_song_time(self, value):
        self._pending_time = value

    def _settle(self):
        if self._pending_time is not None:
            self._time, self._pending_time = self._pending_time, None

    @property
    def cue_points(self):
        self._settle()
        return self._cue_points

    @cue_points.setter
    def cue_points(self, value):
        self._cue_points = value

    def set_or_delete_cue(self):
        # As in Live: toggles a locator at the current song time
        for cue in self._cue_points:
            if abs(cue.time - self._time) < 1e-3:
                self._cue_points.remove(cue)
                self._settle()
                return
        self._cue_points.append(types.SimpleNamespace(time=self._time, name=""))
        self._settle()

    def create_midi_track(self, index):
        self.tracks.insert(index, FakeTrack())

    def start_playing(self):
        # As in Live: play starts from the insert marker, wherever that was last clicked
        self._time, self._pending_time = self.insert_marker, None
        self.is_playing = True

    def continue_playing(self):
        self._settle()
        self.is_playing = True

    def stop_playing(self):
        self.is_playing = False

    def add_current_song_time_listener(self, listener):
        self.time_listeners.append(listener)

    def remove_current_song_time_listener(self, listener):
        self.time_listeners.remove(listener)


class FakeControlSurfaceInstance:
    """What Live hands to create_instance"""

    def __init__(self):
        self.messages = []

    def log_message(self, message):
        self.messages.append(message)

    def handle(self):
        return 4242


def install_fake_live(song):
    forwarded = []

    live = types.ModuleType("Live")
    application = types.SimpleNamespace(get_document=lambda: song, browser=FakeBrowser(song))
    live.Application = types.SimpleNamespace(get_application=lambda: application)
    live.Clip = types.SimpleNamespace(MidiNoteSpecification=FakeNoteSpecification)
    live.MidiMap = types.SimpleNamespace(
        forward_midi_note=lambda script, midi_map, channel, note: forwarded.append(
            (script, midi_map, channel, note)) or True)

    sys.modules["Live"] = live
    return forwarded


# --- Tests -------------------------------------------------------------------

TRACK_NAMES = ["AI Drums", "AI Bass", "AI Chords", "AI Lead"]


class AIProducerTestCase(unittest.TestCase):
    def setUp(self):
        self.song = FakeSong()
        self.forwarded = install_fake_live(self.song)
        FakeClip.deprecated_calls = 0

        if str(SCRIPTS_DIR) not in sys.path:
            sys.path.insert(0, str(SCRIPTS_DIR))
        for name in [m for m in sys.modules if m == "AIProducer" or m.startswith("AIProducer.")]:
            del sys.modules[name]

        self.package = importlib.import_module("AIProducer")
        self.c_instance = FakeControlSurfaceInstance()
        self.script = self.package.create_instance(self.c_instance)
        self.addCleanup(self.script.disconnect)

    def log_text(self):
        return "\n".join(self.c_instance.messages)

    def assert_no_failures_logged(self):
        for marker in ("failed", "FATAL", "Error"):
            self.assertNotIn(marker, self.log_text())

    def clips_by_track(self):
        return {track.name: track.clip_slots[0].clip for track in self.song.tracks}

    def kick_steps_in_first_bar(self):
        drums = self.clips_by_track()["AI Drums"]
        return sorted(round(start / 0.25) for pitch, start, *_ in drums.notes
                      if pitch == 36 and start < 4.0)


class LoadingTests(AIProducerTestCase):
    def test_script_loads(self):
        self.assertIn("AI Producer initialized!", self.log_text())

    def test_has_every_method_live_calls(self):
        # Live's log showed AttributeError for the last two on every load
        for name in ("disconnect", "update_display", "refresh_state", "build_midi_map",
                     "receive_midi", "connect_script_instances", "can_lock_to_devices"):
            self.assertTrue(callable(getattr(self.script, name, None)), name)

        self.script.connect_script_instances([self.script])
        self.assertFalse(self.script.can_lock_to_devices())
        self.script.refresh_state()
        self.script.update_display()

    def test_disconnect_removes_its_listener(self):
        self.assertEqual(len(self.song.time_listeners), 1)
        self.script.disconnect()
        self.assertEqual(self.song.time_listeners, [])
        # The cleanup registered in setUp calls disconnect again
        self.song.add_current_song_time_listener(self.script._on_time_changed)


class GenerationTests(AIProducerTestCase):
    GENRES = {"house": 128, "techno": 135, "dnb": 174, "hiphop": 90, "ambient": 65}

    def test_every_genre_builds_four_tracks_with_clips(self):
        for genre, bpm in self.GENRES.items():
            with self.subTest(genre=genre):
                self.song.tracks.clear()
                self.c_instance.messages.clear()

                self.script.generate_track(genre)

                self.assert_no_failures_logged()
                self.assertEqual([t.name for t in self.song.tracks], TRACK_NAMES)
                self.assertEqual(self.song.tempo, bpm)

                clips = self.clips_by_track()
                for name in ("AI Bass", "AI Chords"):
                    self.assertTrue(clips[name].notes, f"{name} clip is empty")
                if genre != "ambient":  # Ambient has no drums by design
                    self.assertTrue(clips["AI Drums"].notes, "AI Drums clip is empty")

    def test_each_genre_gets_its_own_drums(self):
        expected_kicks = {
            "house": [0, 4, 8, 12],
            "techno": [0, 6, 8, 14],
            "dnb": [0, 10],
            "hiphop": [0, 6, 10],
            "ambient": [],
        }

        for genre, kicks in expected_kicks.items():
            with self.subTest(genre=genre):
                self.song.tracks.clear()
                self.script.generate_track(genre)
                self.assertEqual(self.kick_steps_in_first_bar(), kicks)

    def test_house_has_a_clap_on_two_and_four(self):
        self.script.generate_track("house")
        drums = self.clips_by_track()["AI Drums"]

        def steps(pitch):
            return sorted(round(start / 0.25) for note_pitch, start, *_ in drums.notes
                          if note_pitch == pitch and start < 4.0)

        self.assertEqual(steps(39), [4, 12], "D#1 is the clap")
        self.assertEqual(steps(38), [], "The clap takes the snare's place in house")

    def test_other_genres_keep_their_snare(self):
        self.script.generate_track("techno")
        drums = self.clips_by_track()["AI Drums"]

        self.assertTrue(any(pitch == 38 for pitch, *_ in drums.notes))
        self.assertFalse(any(pitch == 39 for pitch, *_ in drums.notes))

    def test_unknown_genre_falls_back_to_house(self):
        self.script.generate_track("polka")
        self.assert_no_failures_logged()
        self.assertEqual(self.song.tempo, 128)

    def test_existing_template_tracks_are_reused(self):
        self.song.tracks = [FakeTrack(name) for name in TRACK_NAMES]
        stale = self.song.tracks[0].clip_slots[0]
        stale.create_clip(4.0)
        stale.clip.set_notes(((36, 0.0, 0.1, 100, False),))

        self.script.generate_track("techno")

        self.assert_no_failures_logged()
        self.assertEqual([t.name for t in self.song.tracks], TRACK_NAMES)
        self.assertEqual(self.song.tempo, 135)
        self.assertEqual(self.kick_steps_in_first_bar(), [0, 6, 8, 14])
        for name, clip in self.clips_by_track().items():
            self.assertTrue(clip.notes, f"{name} clip is empty")


class InstrumentTests(AIProducerTestCase):
    def browser(self):
        return sys.modules["Live"].Application.get_application().browser

    def instruments_by_track(self):
        return {track.name: [device.name for device in track.devices] for track in self.song.tracks}

    def test_new_tracks_get_audioforge_instruments(self):
        self.script.generate_track("house")

        self.assert_no_failures_logged()
        self.assertEqual(self.instruments_by_track(), {
            "AI Drums": ["DrumSynth"],
            "AI Bass": ["BasicSynth"],
            "AI Chords": ["AudioForge - ElectricPiano"],
            "AI Lead": ["Polysynth"],
        })

    def test_the_bass_is_set_to_its_bass_patch(self):
        self.script.generate_track("house")

        bass = self.song.tracks[1].devices[0]
        program = next(p for p in bass.parameters if p.name == "Program")

        # Four patches spread over the parameter's range: Init, Bass, Pad, Lead
        self.assertAlmostEqual(program.value, 1.0 / 3.0, places=3)

    def test_without_the_plugins_it_uses_what_every_live_has(self):
        self.browser().plugins = FakeBrowserItem("Plug-Ins", [FakeBrowserItem("VST3", [])])

        self.script.generate_track("house")

        self.assert_no_failures_logged()
        self.assertEqual(self.instruments_by_track(), {
            "AI Drums": ["909 Core Kit"],
            "AI Bass": ["Drift"],
            "AI Chords": ["Drift"],
            "AI Lead": ["Drift"],
        })

    def test_with_nothing_to_load_it_says_what_to_add(self):
        browser = self.browser()
        browser.plugins = FakeBrowserItem("Plug-Ins", [])
        browser.instruments = FakeBrowserItem("Instruments", [])
        browser.drums = FakeBrowserItem("Drums", [])

        self.script.generate_track("house")

        self.assert_no_failures_logged()
        self.assertEqual(self.instruments_by_track(), {name: [] for name in TRACK_NAMES})
        self.assertIn("No instrument could be loaded on 'AI Bass'", self.log_text())
        for name, clip in self.clips_by_track().items():
            self.assertTrue(clip.notes, f"{name} clip is empty")

    def test_a_failed_load_still_leaves_every_track_its_clip(self):
        def refuse(item):
            raise RuntimeError("the browser is busy")

        self.browser().load_item = refuse

        self.script.generate_track("house")

        self.assertEqual([t.name for t in self.song.tracks], TRACK_NAMES)
        self.assertIn("Could not load an instrument on 'AI Drums': the browser is busy", self.log_text())
        for name, clip in self.clips_by_track().items():
            self.assertTrue(clip.notes, f"{name} clip is empty")

    def test_a_load_that_has_not_landed_is_not_called_loaded(self):
        self.browser().load_item = lambda item: None

        self.script.generate_track("house")

        self.assert_no_failures_logged()
        self.assertNotIn("Loaded '", self.log_text())
        self.assertIn("it is not there yet", self.log_text())

    def test_the_patch_is_placed_within_whatever_range_live_gives(self):
        loader = self.script.device_loader
        device = FakeDevice("BasicSynth")
        program = next(p for p in device.parameters if p.name == "Program")
        program.min, program.max = 0.0, 3.0

        loader._choose_patch(device, "bass", "BasicSynth")

        self.assertEqual(program.value, 1.0)

    def test_it_never_recommends_what_live_intro_lacks(self):
        self.script.generate_track("house")

        for missing in ("Operator", "Analog", "Wavetable", "EQ Eight"):
            self.assertNotIn(missing, self.log_text())

    def test_template_tracks_keep_their_instruments_and_empty_ones_get_one(self):
        self.song.tracks = [FakeTrack(name) for name in TRACK_NAMES]
        self.song.tracks[0].devices.append(FakeDevice("My Own Kit"))

        self.script.generate_track("house")

        self.assert_no_failures_logged()
        self.assertEqual(self.instruments_by_track()["AI Drums"], ["My Own Kit"])
        self.assertEqual(self.instruments_by_track()["AI Bass"], ["BasicSynth"])


class NoteWritingTests(AIProducerTestCase):
    def test_notes_are_written_with_the_current_call(self):
        self.script.generate_track("house")

        self.assert_no_failures_logged()
        self.assertEqual(FakeClip.deprecated_calls, 0)
        for name, clip in self.clips_by_track().items():
            self.assertTrue(clip.notes, f"{name} clip is empty")

    def test_an_older_live_still_gets_its_notes(self):
        original = FakeClipSlot.create_clip

        def create_old_clip(slot, length):
            slot.clip = FakeOldClip(length)

        FakeClipSlot.create_clip = create_old_clip
        self.addCleanup(setattr, FakeClipSlot, "create_clip", original)

        self.script.generate_track("house")

        self.assert_no_failures_logged()
        for name, clip in self.clips_by_track().items():
            self.assertTrue(clip.notes, f"{name} clip is empty")


class MidiTriggerTests(AIProducerTestCase):
    def test_asks_live_to_forward_middle_c(self):
        self.script.build_midi_map(77)

        self.assertEqual(len(self.forwarded), 16)
        for channel, (script, midi_map, forwarded_channel, note) in enumerate(self.forwarded):
            self.assertEqual((script, midi_map, forwarded_channel, note), (4242, 77, channel, 60))

    def test_middle_c_generates_on_any_channel(self):
        for status in (0x90, 0x93):
            with self.subTest(status=hex(status)):
                self.song.tracks.clear()
                self.script.receive_midi((status, 60, 100))
                self.assertEqual([t.name for t in self.song.tracks], TRACK_NAMES)

    def test_other_midi_is_ignored(self):
        for message in ((0x90, 61, 100),   # another note
                        (0x80, 60, 0),     # note off
                        (0x90, 60, 0),     # note on with zero velocity, which is a note off
                        (0xB0, 60, 127)):  # a controller
            self.script.receive_midi(message)

        self.assertEqual(self.song.tracks, [])


class OscTests(AIProducerTestCase):
    def setUp(self):
        super().setUp()
        self.trigger = importlib.import_module("AIProducer.trigger_generate")
        self.osc_module = importlib.import_module("AIProducer.OSCServer")

    def parse(self, packet):
        return self.script.osc_server._parse_osc(packet)

    def test_parses_string_int_and_float_arguments(self):
        s = self.trigger.osc_string
        self.assertEqual(self.parse(s("/ai_producer/generate") + s(",s") + s("techno")),
                         {"address": "/ai_producer/generate", "args": ["techno"]})
        self.assertEqual(self.parse(s("/live/track/set/volume") + s(",if") + b"\x00\x00\x00\x02" + b"\x3f\x00\x00\x00"),
                         {"address": "/live/track/set/volume", "args": [2, 0.5]})
        # A string whose length is a multiple of four still gets a full pad
        self.assertEqual(self.parse(s("/x") + s(",si") + s("abcd") + b"\x00\x00\x00\x07")["args"], ["abcd", 7])
        self.assertEqual(self.parse(s("/live/song/start_playing")),
                         {"address": "/live/song/start_playing", "args": []})

    def test_trigger_script_reaches_the_running_script(self):
        # Listen on a port the system picks, so the test cannot collide with Live
        self.script.osc_server.close()
        server = self.osc_module.OSCServer(port=0)
        self.assertTrue(server.running)
        self.script.osc_server = server
        port = server.sock.getsockname()[1]

        self.trigger.send_generate("dnb", port=port)

        deadline = time.time() + 2.0
        while not self.song.tracks and time.time() < deadline:
            self.script.update_display()
            time.sleep(0.01)

        self.assert_no_failures_logged()
        self.assertEqual([t.name for t in self.song.tracks], TRACK_NAMES)
        self.assertEqual(self.song.tempo, 174)

    def test_transport_and_mixer_commands(self):
        s = self.trigger.osc_string
        self.song.tracks = [FakeTrack("A"), FakeTrack("B")]

        self.script._handle_osc_message(self.parse(s("/live/song/set/tempo") + s(",f") + b"\x42\xf0\x00\x00"))
        self.assertEqual(self.song.tempo, 120.0)

        self.script._handle_osc_message(self.parse(s("/live/song/start_playing")))
        self.assertTrue(self.song.is_playing)
        self.script._handle_osc_message(self.parse(s("/live/song/stop_playing")))
        self.assertFalse(self.song.is_playing)

        self.script._handle_osc_message(self.parse(s("/live/track/set/volume") + s(",if") + b"\x00\x00\x00\x01" + b"\x3f\x00\x00\x00"))
        self.assertEqual(self.song.tracks[1].mixer_device.volume.value, 0.5)
        self.assert_no_failures_logged()


class LiveControlTestCase(AIProducerTestCase):
    """Drives the script through live_control over a real UDP socket"""

    def setUp(self):
        super().setUp()
        self.control = importlib.import_module("AIProducer.live_control")
        osc_module = importlib.import_module("AIProducer.OSCServer")

        self.song.tracks = [FakeTrack(name) for name in TRACK_NAMES]

        # Listen on a port the system picks, so the test cannot collide with Live
        self.script.osc_server.close()
        self.script.osc_server = osc_module.OSCServer(port=0)
        self.port = self.script.osc_server.sock.getsockname()[1]

        # Live calls update_display about ten times a second; stand in for that
        self.pumping = True
        self.pump = threading.Thread(target=self._pump, daemon=True)
        self.pump.start()
        self.addCleanup(self._stop_pump)

    def _pump(self):
        while self.pumping:
            self.script.update_display()
            time.sleep(0.002)

    def _stop_pump(self):
        self.pumping = False
        self.pump.join()

    def ask(self, address, *args):
        reply = self.control.request(address, *args, port=self.port)
        self.assertIsNotNone(reply, f"no reply to {address}")
        return reply


class MixerTests(LiveControlTestCase):
    def test_mixer_lists_every_track_with_its_level(self):
        self.song.tracks[1].devices.append(FakeDevice("BasicSynth"))

        reply = self.ask("/live/mixer")

        self.assertTrue(reply["ok"])
        self.assertEqual([t["target"] for t in reply["tracks"]], [0, 1, 2, 3, "return:0", "return:1", "master"])
        self.assertEqual(reply["tracks"][1]["name"], "AI Bass")
        self.assertEqual(reply["tracks"][1]["devices"], ["BasicSynth"])
        self.assertEqual(reply["tracks"][1]["volume"]["display"], "0.0 dB")
        self.assertEqual(reply["tracks"][1]["sends"], [0.0, 0.0])

    def test_tracks_are_found_by_index_name_master_and_return(self):
        for target, track in ((2, self.song.tracks[2]),
                              ("AI Lead", self.song.tracks[3]),
                              ("master", self.song.master_track),
                              ("return:1", self.song.return_tracks[1])):
            with self.subTest(target=target):
                self.ask("/live/track/set/pan", target, 0.25)
                self.assertEqual(track.mixer_device.panning.value, 0.25)

    def test_master_means_the_master_even_if_a_track_shares_the_name(self):
        self.song.tracks[0].name = "master"
        self.song.tracks[1].name = "return:0"

        self.ask("/live/track/set/pan", "master", 0.5)
        self.ask("/live/track/set/pan", "return:0", -0.5)

        self.assertEqual(self.song.master_track.mixer_device.panning.value, 0.5)
        self.assertEqual(self.song.return_tracks[0].mixer_device.panning.value, -0.5)
        self.assertEqual(self.song.tracks[0].mixer_device.panning.value, 0.0)
        self.assertEqual(self.song.tracks[1].mixer_device.panning.value, 0.0)

    def test_fader_requests_outside_its_range_end_up_at_the_nearest_end(self):
        self.assertEqual(self.ask("/live/track/set/volume_db", 0, 40)["value"], 1.0)
        self.assertEqual(self.ask("/live/track/set/volume_db", 0, "-inf")["display"], "-inf dB")

    def test_not_a_number_is_refused(self):
        for address, args in (("/live/track/set/pan", (0, "nan")),
                              ("/live/track/set/volume_db", (0, "nan"))):
            with self.subTest(address=address):
                self.assertFalse(self.ask(address, *args)["ok"])
        self.assertEqual(self.song.tracks[0].mixer_device.panning.value, 0.0)
        self.assertEqual(self.song.tracks[0].mixer_device.volume.value, 0.85)

    def test_fader_is_set_to_the_level_live_displays(self):
        reply = self.ask("/live/track/set/volume_db", "AI Bass", -6)

        self.assertTrue(reply["ok"])
        self.assertEqual(reply["display"], "-6.0 dB")
        self.assertAlmostEqual(self.song.tracks[1].mixer_device.volume.value, 0.70, places=3)

    def test_values_are_kept_inside_the_parameter_range(self):
        self.assertEqual(self.ask("/live/track/set/pan", 0, 5.0)["value"], 1.0)
        self.assertEqual(self.ask("/live/track/set/send", 0, 1, -3.0)["value"], 0.0)

    def test_mistakes_come_back_as_errors(self):
        for address, args in (("/live/track/set/pan", ("No Such Track", 0.0)),
                              ("/live/track/set/pan", (99, 0.0)),
                              ("/live/track/set/send", (0, 7, 0.5)),
                              ("/live/device/params", ("master", 0)),
                              ("/live/device/load", ("master", "audio_effects/Nope")),
                              ("/live/device/load", ("master", "audio_effects/Presets Only")),
                              ("/live/device/load", ("master", "somewhere/Limiter"))):
            with self.subTest(address=address, args=args):
                reply = self.ask(address, *args)
                self.assertFalse(reply["ok"])
                self.assertTrue(reply["error"])


class DeviceTests(LiveControlTestCase):
    def test_a_device_can_be_loaded_on_the_master(self):
        reply = self.ask("/live/device/load", "master", "audio_effects/Limiter")

        self.assertEqual(reply["loaded"], "Limiter")
        self.assertIs(self.song.view.selected_track, self.song.master_track)
        self.assertEqual([d.name for d in self.song.master_track.devices], ["Limiter"])

    def test_presets_load_by_name_without_their_extension(self):
        self.ask("/live/device/load", "AI Drums", "audio_effects/Compressor/Glue")
        self.assertEqual([d.name for d in self.song.tracks[0].devices], ["Glue"])

    def test_parameters_can_be_read_and_set(self):
        self.song.master_track.devices.append(FakeDevice("Limiter"))

        listing = self.ask("/live/device/params", "master", 0)
        self.assertEqual(listing["device"], "Limiter")
        self.assertEqual(listing["params"][1], [1, "Limiter Param 1", 0.5, 0.0, 1.0, "0.50"])

        reply = self.ask("/live/device/set", "master", 0, 1, 0.25)
        self.assertEqual((reply["name"], reply["value"], reply["display"]), ("Limiter Param 1", 0.25, "0.25"))
        self.assertEqual(self.song.master_track.devices[0].parameters[1].value, 0.25)

    def test_a_long_reply_arrives_whole(self):
        # More text than fits in one message, so it has to be split and rejoined
        self.song.tracks[0].devices.append(FakeDevice("Big Plugin", num_params=400))

        listing = self.ask("/live/device/params", 0, 0)

        self.assertEqual(len(listing["params"]), 400)
        self.assertEqual(listing["params"][399][1], "Big Plugin Param 399")

    def test_a_device_can_be_deleted(self):
        self.song.tracks[2].devices += [FakeDevice("SimpleEQ"), FakeDevice("SimpleGain")]

        reply = self.ask("/live/device/delete", "AI Chords", 0)

        self.assertEqual(reply["deleted"], "SimpleEQ")
        self.assertEqual([d.name for d in self.song.tracks[2].devices], ["SimpleGain"])


class ClipTests(LiveControlTestCase):
    def setUp(self):
        super().setUp()
        self.vocals = FakeAudioClip("vocals")
        self.song.tracks[1].clip_slots[0].clip = self.vocals
        self.song.tracks[0].clip_slots[0].create_clip(4.0)  # A MIDI clip

    def test_a_clip_can_be_described(self):
        reply = self.ask("/live/clip/info", "AI Bass", 0)

        self.assertEqual(reply["name"], "vocals")
        self.assertTrue(reply["audio"])
        self.assertTrue(reply["warping"])
        self.assertAlmostEqual(reply["length"], 356.1)

    def test_warping_can_be_switched_off_and_on(self):
        reply = self.ask("/live/clip/set/warping", 1, 0, 0)
        self.assertFalse(self.vocals.warping)
        self.assertFalse(reply["warping"])

        self.ask("/live/clip/set/warping", "AI Bass", 0, 1)
        self.assertTrue(self.vocals.warping)

    def test_a_clip_reports_where_it_starts_and_how_long_its_file_is(self):
        reply = self.ask("/live/clip/info", "AI Bass", 0)

        self.assertAlmostEqual(reply["start_marker"], 1.15)
        self.assertAlmostEqual(reply["end_marker"], 145.38)
        self.assertAlmostEqual(reply["file_seconds"], 6411200 / 44100.0)

    def test_the_start_and_end_of_a_clip_can_be_moved(self):
        reply = self.ask("/live/clip/set/markers", "AI Bass", 0, 0.0, 145.0)

        self.assertEqual(self.vocals.start_marker, 0.0, "The start should move even to before where the clip began")
        self.assertEqual(self.vocals.end_marker, 145.0)
        self.assertEqual((self.vocals.loop_start, self.vocals.loop_end), (0.0, 145.0))
        self.assertEqual(reply["start_marker"], 0.0)
        self.assertEqual(reply["loop_start"], 0.0)

        # Moving both past the old end must not trip over the order they are set in
        self.ask("/live/clip/set/markers", "AI Bass", 0, 150.0, 160.0)
        self.assertEqual((self.vocals.start_marker, self.vocals.end_marker), (150.0, 160.0))

        backwards = self.ask("/live/clip/set/markers", "AI Bass", 0, 10.0, 5.0)
        self.assertFalse(backwards["ok"])
        self.assertEqual((self.vocals.start_marker, self.vocals.end_marker), (150.0, 160.0))

    def test_a_marker_live_rounds_to_the_nearest_sample_still_counts(self):
        class SampleExactClip(FakeAudioClip):
            """Keeps positions on whole samples, as Live does for an unwarped clip"""

            def __setattr__(self, name, value):
                if name in ('start_marker', 'end_marker', 'loop_start', 'loop_end'):
                    value = round(value * 44100.0) / 44100.0
                super().__setattr__(name, value)

        self.song.tracks[2].clip_slots[0].clip = SampleExactClip("exact")

        reply = self.ask("/live/clip/set/markers", "AI Chords", 0, 0.0, 145.378685)

        self.assertTrue(reply["ok"], reply.get("error"))
        self.assertAlmostEqual(reply["end_marker"], 145.378685, places=4)

    def test_the_timeline_of_a_track_can_be_cleared(self):
        track = self.song.tracks[1]
        track.arrangement_clips = [FakeAudioClip("vocals"), FakeAudioClip("vocals")]

        reply = self.ask("/live/clip/clear_arrangement", "AI Bass")

        self.assertEqual(reply["deleted"], 2)
        self.assertEqual(track.arrangement_clips, [])
        self.assertEqual(self.ask("/live/clip/clear_arrangement", "AI Bass")["deleted"], 0,
                         "Clearing an empty timeline is fine")

    def test_a_session_clip_can_be_copied_to_the_timeline(self):
        reply = self.ask("/live/clip/duplicate", "AI Bass", 0, 312.0)

        track = self.song.tracks[1]
        self.assertEqual(len(track.arrangement_clips), 1)
        self.assertEqual(track.arrangement_clips[0].start_time, 312.0)
        self.assertEqual(reply["start_time"], 312.0)
        self.assertEqual(reply["name"], "vocals")

    def test_timeline_clips_are_listed_and_addressed_by_beat(self):
        self.ask("/live/clip/duplicate", "AI Bass", 0, 8.0)
        self.ask("/live/clip/duplicate", "AI Bass", 0, 500.0)

        listing = self.ask("/live/clip/arrangement", "AI Bass")
        self.assertEqual([c["start_time"] for c in listing["clips"]], [8.0, 500.0])
        self.assertEqual(listing["clips"][0]["end_time"], 8.0 + 356.1)

        # Any beat inside a clip finds it
        self.assertEqual(self.ask("/live/clip/info", "AI Bass", "@520.5")["start_time"], 500.0)

        reply = self.ask("/live/clip/set/markers", "AI Bass", "@8", 2.0, 10.0)
        self.assertTrue(reply["ok"], reply.get("error"))
        self.assertEqual((reply["start_marker"], reply["end_marker"]), (2.0, 10.0))
        self.assertEqual(self.vocals.start_marker, 1.15, "The Session clip is left alone")

        nothing = self.ask("/live/clip/info", "AI Bass", "@400")
        self.assertFalse(nothing["ok"])
        self.assertIn("No clip at beat 400", nothing["error"])

    def test_a_copy_that_lands_just_after_the_beat_asked_for_is_still_found(self):
        self.ask("/live/clip/duplicate", "AI Bass", 0, 8.0)
        reply = self.ask("/live/clip/duplicate", "AI Bass", 0, 86.384333)

        self.assertTrue(reply["ok"], reply.get("error"))
        self.assertGreater(reply["start_time"], 86.384333, "Live put the copy on the next whole sample")

        # The beat asked for names the copy, not the clip that was cut off there
        trimmed = self.ask("/live/clip/set/markers", "AI Bass", "@86.384333", 0.0, 91.465813)
        self.assertTrue(trimmed["ok"], trimmed.get("error"))
        self.assertEqual(trimmed["start_time"], reply["start_time"])
        self.assertEqual(self.ask("/live/clip/info", "AI Bass", "@8")["start_marker"], 1.15,
                         "The copy at 8 is left alone")

    def test_a_looping_clip_keeps_its_loop(self):
        self.vocals.looping = True
        self.ask("/live/clip/set/markers", "AI Bass", 0, 2.0, 100.0)

        self.assertEqual((self.vocals.start_marker, self.vocals.end_marker), (2.0, 100.0))
        self.assertEqual((self.vocals.loop_start, self.vocals.loop_end), (1.15, 145.38),
                         "With looping on, those are the loop, and are left alone")

    def test_a_marker_live_does_not_accept_is_reported_and_undone(self):
        class StubbornClip(FakeAudioClip):
            """Takes a new end but quietly keeps its old start, as Live can"""

            def __setattr__(self, name, value):
                if name == 'start_marker' and hasattr(self, 'start_marker'):
                    return
                super().__setattr__(name, value)

        stubborn = StubbornClip("stubborn")
        self.song.tracks[2].clip_slots[0].clip = stubborn

        reply = self.ask("/live/clip/set/markers", "AI Chords", 0, 0.0, 100.0)

        self.assertFalse(reply["ok"])
        self.assertIn("did not take", reply["error"])
        self.assertEqual(stubborn.end_marker, 145.38, "The end should be put back where it was")
        self.assertEqual((stubborn.loop_start, stubborn.loop_end), (1.15, 145.38))

    def test_mistakes_come_back_as_errors(self):
        empty = self.ask("/live/clip/info", "AI Bass", 3)
        self.assertFalse(empty["ok"])
        self.assertIn("no clip", empty["error"])

        midi = self.ask("/live/clip/set/warping", "AI Drums", 0, 0)
        self.assertFalse(midi["ok"])
        self.assertIn("not an audio clip", midi["error"])

        no_slot = self.ask("/live/clip/info", "AI Bass", 99)
        self.assertFalse(no_slot["ok"])
        self.assertIn("No slot 99", no_slot["error"])

        midi_markers = self.ask("/live/clip/set/markers", "AI Drums", 0, 0.0, 2.0)
        self.assertFalse(midi_markers["ok"])
        self.assertIn("not an audio clip", midi_markers["error"])

        endless = self.ask("/live/clip/set/markers", "AI Bass", 0, 0.0, "inf")
        self.assertFalse(endless["ok"])

        for beat in (-4.0, "nan"):
            refused = self.ask("/live/clip/duplicate", "AI Bass", 0, beat)
            self.assertFalse(refused["ok"])
            self.assertEqual(self.song.tracks[1].arrangement_clips, [], "Nothing should be copied")

        # The master's slots launch scenes; they never hold a clip
        master = self.ask("/live/clip/info", "master", 0)
        self.assertFalse(master["ok"])


class TransportTests(LiveControlTestCase):
    def test_play_from_starts_at_the_beat_asked_for(self):
        reply = self.ask("/live/song/play_from", 72.0)

        self.assertTrue(self.song.is_playing)
        self.assertEqual(self.song.current_song_time, 72.0, "Not from the insert marker")
        self.assertEqual(reply["from"], 72.0)

        bad = self.ask("/live/song/play_from", -1.0)
        self.assertFalse(bad["ok"])


class LocatorTests(LiveControlTestCase):
    def place_cues(self, *times):
        for time in times:
            self.song.current_song_time = time
            self.song.cue_points        # time passes
            self.song.set_or_delete_cue()

    def run_until_done(self, address, *args, limit=40):
        for _ in range(limit):
            reply = self.ask(address, *args)
            self.assertTrue(reply["ok"], reply.get("error"))
            if reply["done"]:
                return reply
        self.fail(f"{address} never finished")

    def test_all_locators_can_be_cleared_a_step_at_a_time(self):
        self.place_cues(8.0, 24.0, 360.0)
        self.song.current_song_time = 100.0
        self.song.cue_points

        first = self.ask("/live/song/clear_locators")
        self.assertFalse(first["done"], "The playhead has only been sent to the first locator")
        self.assertEqual(len(self.song.cue_points), 3, "Nothing is toggled until Live has moved the playhead")

        reply = self.run_until_done("/live/song/clear_locators")

        self.assertEqual(reply["deleted"], 3)
        self.assertEqual(self.song.cue_points, [])
        self.song.cue_points
        self.assertEqual(self.song.current_song_time, 100.0, "The playhead goes back where it was")
        self.assertTrue(self.ask("/live/song/clear_locators")["done"])

    def test_a_locator_is_set_where_asked_and_named(self):
        self.song.current_song_time = 100.0
        self.song.cue_points

        reply = self.run_until_done("/live/song/set_locator", 312.0, "Breakdown")

        self.assertEqual([(c.time, c.name) for c in self.song.cue_points], [(312.0, "Breakdown")])
        self.assertEqual(reply["time"], 312.0)

        # Asking again renames rather than toggling it away
        self.run_until_done("/live/song/set_locator", 312.0, "Breakdown A")
        self.assertEqual([(c.time, c.name) for c in self.song.cue_points], [(312.0, "Breakdown A")])


class MeterTests(LiveControlTestCase):
    def test_meters_report_now_and_the_peak_since_last_asked(self):
        drums = self.song.tracks[0]
        self.ask("/live/meters")  # The first request switches metering on

        drums.output_meter_left = 0.8
        time.sleep(0.05)
        drums.output_meter_left, drums.output_meter_right = 0.1, 0.2
        time.sleep(0.05)

        first = self.ask("/live/meters")["meters"][0]
        self.assertEqual((first["name"], first["left"], first["right"]), ("AI Drums", 0.1, 0.2))
        self.assertEqual(first["peak"], 0.8)

        # The peak starts again after each request
        time.sleep(0.05)
        self.assertEqual(self.ask("/live/meters")["meters"][0]["peak"], 0.2)

    def test_a_failing_meter_switches_metering_off_without_breaking_the_display_loop(self):
        class BrokenTrack(FakeTrack):
            @property
            def output_meter_left(self):
                raise RuntimeError("meter unavailable")

            @output_meter_left.setter
            def output_meter_left(self, value):
                pass

        self._stop_pump()
        self.script.mixer_control.metering = True
        self.song.tracks.append(BrokenTrack("Broken"))

        self.script.update_display()  # Must not raise
        self.script.update_display()

        self.assertFalse(self.script.mixer_control.metering)
        self.assertEqual(self.log_text().count("Meters switched off"), 1)

    def test_tracks_without_audio_are_left_out(self):
        self.song.tracks[3].has_audio_output = False
        names = [m["name"] for m in self.ask("/live/meters")["meters"]]
        self.assertNotIn("AI Lead", names)
        self.assertIn("Master", names)


class FeedbackTests(LiveControlTestCase):
    def test_feedback_is_recorded_with_what_live_was_doing(self):
        self.song.tracks[0].devices.append(FakeDevice("DrumSynth"))
        self.song.tracks[0].output_meter_left = 0.6

        with tempfile.TemporaryDirectory() as folder:
            log = pathlib.Path(folder) / "feedback" / "log.jsonl"
            self.control.record_feedback("the hi-hat is too bright", "user", log, port=self.port)
            self.control.record_feedback("kick is fine", "user", log, port=self.port)
            records = [json.loads(line) for line in log.read_text().splitlines()]

        self.assertEqual([r["text"] for r in records], ["the hi-hat is too bright", "kick is fine"])
        first = records[0]
        self.assertEqual((first["source"], first["status"]), ("user", "open"))
        self.assertEqual(first["live"]["mixer"][0]["name"], "AI Drums")
        self.assertEqual(first["live"]["devices"][0]["device"], "DrumSynth")
        self.assertEqual(first["live"]["devices"][0]["params"]["DrumSynth Param 1"], "0.50")
        self.assertEqual(first["live"]["meters"][0]["left"], 0.6)

    def test_feedback_is_still_recorded_when_live_does_not_answer(self):
        self._stop_pump()
        self.script.osc_server.close()

        with tempfile.TemporaryDirectory() as folder:
            log = pathlib.Path(folder) / "log.jsonl"
            # request waits out its timeout when nothing answers; keep that short
            original = self.control.request
            self.control.request = lambda *a, **k: original(*a, **{**k, "timeout": 0.1})
            try:
                record = self.control.record_feedback("no sound at all", "user", log, port=self.port)
            finally:
                self.control.request = original
            self.assertEqual(len(log.read_text().splitlines()), 1)

        self.assertIsNone(record["live"])
        self.assertEqual(record["text"], "no sound at all")


if __name__ == "__main__":
    unittest.main()
