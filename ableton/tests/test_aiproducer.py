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


class FakeTrack:
    def __init__(self, name="MIDI"):
        self.name = name
        self.clip_slots = [FakeClipSlot() for _ in range(8)]
        self.mixer_device = types.SimpleNamespace(
            volume=FakeParameter("Volume", 0.85, display=fader_display),
            panning=FakeParameter("Pan", 0.0, -1.0, 1.0),
            sends=[FakeParameter("A", 0.0), FakeParameter("B", 0.0)])
        self.devices = []
        self.has_audio_output = True
        self.output_meter_left = 0.0
        self.output_meter_right = 0.0

    def delete_device(self, index):
        del self.devices[index]


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
    application = types.SimpleNamespace(get_document=lambda: song, browser=FakeBrowser(song))
    live.Application = types.SimpleNamespace(get_application=lambda: application)
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
