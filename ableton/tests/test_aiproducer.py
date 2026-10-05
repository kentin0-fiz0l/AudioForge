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
import pathlib
import sys
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

    def set_notes(self, notes):
        for pitch, start, duration, velocity, muted in notes:
            if not (0 <= pitch <= 127 and start >= 0 and duration > 0 and 1 <= velocity <= 127):
                raise ValueError(f"bad note {(pitch, start, duration, velocity, muted)}")
        self.notes.extend(notes)


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


class FakeTrack:
    def __init__(self, name="MIDI"):
        self.name = name
        self.clip_slots = [FakeClipSlot() for _ in range(8)]
        self.mixer_device = types.SimpleNamespace(volume=types.SimpleNamespace(value=0.85))


class FakeSong:
    def __init__(self):
        self.tempo = 120.0
        self.tracks = []
        self.is_playing = False
        self.time_listeners = []

    def create_midi_track(self, index):
        self.tracks.insert(index, FakeTrack())

    def start_playing(self):
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
    live.Application = types.SimpleNamespace(
        get_application=lambda: types.SimpleNamespace(get_document=lambda: song))
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


if __name__ == "__main__":
    unittest.main()
