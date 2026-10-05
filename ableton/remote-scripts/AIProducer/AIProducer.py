# AI Producer - Main Remote Script
# Autonomous track generation for Ableton Live

import Live
from .MusicTheory import TrackGenerator
from .MIDIGenerator import MIDIClipGenerator
from .OSCServer import OSCServer
from .DeviceLoader import DeviceLoader
from .EffectChains import EffectChains
from .MixerControl import MixerControl

class AIProducer:
    """AI Producer Remote Script - Generates complete tracks automatically"""

    TRIGGER_NOTE = 60  # C3
    MAX_MESSAGES_PER_UPDATE = 16

    def __init__(self, c_instance):
        self.c_instance = c_instance
        self.app = Live.Application.get_application()
        self.song = self.app.get_document()

        # Initialize device loader for automatic instrument loading
        self.device_loader = DeviceLoader(self.app, self.log)

        # Initialize effect chains
        self.effect_chains = EffectChains(self.log)

        # Mixer, device and meter access for the chat interface
        self.mixer_control = MixerControl(self.song, self.app, self.log)

        self.log("AI Producer initialized!")
        self.log("=" * 50)
        self.log("READY! Waiting for trigger...")
        self.log("Trigger options:")
        self.log("  1. Press MIDI note C3 (middle C)")
        self.log("  2. Uncomment auto-generate below (for testing)")
        self.log("=" * 50)

        # AUTO-GENERATE FOR TESTING (uncomment to enable)
        # Uncomment the line below to auto-generate on script load:
        # self.generate_track()  # ❌ DISABLED - Only generate via chat commands

        # Start OSC server for chat interface
        self.osc_server = OSCServer(port=9000)
        self.log("OSC server started on port 9000")
        self.log("Chat interface can now send commands!")

        # Listen for key commands
        self.song.add_current_song_time_listener(self._on_time_changed)

    def log(self, message):
        """Log message to Ableton's Log.txt"""
        self.c_instance.log_message(f"[AI Producer] {message}")

    def load_device_simple(self, track, device_type):
        """
        Simplified device loading - tells user which instrument to use
        Full browser API automation is complex, so we guide the user instead
        """
        device_recommendations = {
            'drums': 'Drum Rack (Browser → Drums → Drum Rack)',
            'bass': 'Operator or Wavetable (Browser → Instruments)',
            'chords': 'Analog or Wavetable (Browser → Instruments)',
            'lead': 'Operator or Wavetable (Browser → Instruments)'
        }

        if device_type in device_recommendations:
            self.log(f"💡 Recommended instrument: {device_recommendations[device_type]}")
            self.log(f"   Drag it onto the '{track.name}' track to hear sound!")
            return True
        return False

    def disconnect(self):
        """Cleanup on script unload"""
        self.song.remove_current_song_time_listener(self._on_time_changed)
        if self.osc_server:
            self.osc_server.close()
        self.log("AI Producer disconnected")

    def update_display(self):
        """Called every 100ms by Live - required method"""
        # Check for OSC messages. Take more than one per update, so that
        # several senders at once do not queue up behind each other.
        if self.osc_server:
            for _ in range(self.MAX_MESSAGES_PER_UPDATE):
                message = self.osc_server.receive()
                if not message:
                    break
                self._handle_osc_message(message)

        self.mixer_control.update_meters()

    def refresh_state(self):
        """Called when Live refreshes - required method"""
        pass

    def connect_script_instances(self, instanciated_scripts):
        """Called once all control surface scripts are loaded - required method"""
        pass

    def can_lock_to_devices(self):
        """Called by Live to ask whether this script follows a device - required method"""
        return False

    def _on_time_changed(self):
        """Called periodically - we use this to check for triggers"""
        pass

    def _handle_osc_message(self, message):
        """Handle incoming OSC command from chat interface"""
        address = message.get('address', '')
        args = message.get('args', [])

        self.log(f"OSC: {address} {args}")

        try:
            if self.mixer_control.handles(address):
                reply = self.mixer_control.handle(address, args)
                self.osc_server.send_reply(message.get('sender'), reply)

            elif address == '/ai_producer/generate':
                # Extract genre from args (default to 'house')
                genre = 'house'
                if args and len(args) > 0:
                    # Args come as string from OSC
                    genre = str(args[0]).lower()
                self.log(f"Generating {genre} track...")
                self.generate_track(genre)

            elif address == '/live/song/set/tempo':
                if args:
                    self.song.tempo = float(args[0])
                    self.log(f"Tempo set to {args[0]} BPM")

            elif address == '/live/song/start_playing':
                self.song.start_playing()
                self.log("Playback started")

            elif address == '/live/song/stop_playing':
                self.song.stop_playing()
                self.log("Playback stopped")

            elif address == '/live/song/create_midi_track':
                track_index = len(self.song.tracks)
                self.song.create_midi_track(track_index)
                self.log("Created MIDI track")

        except Exception as e:
            self.log(f"OSC Error: {str(e)}")

    def generate_track(self, genre='house'):
        """Generate a complete track with all instruments and MIDI"""
        self.log("=" * 50)
        self.log(f"GENERATING {genre.upper()} TRACK NOW!")
        self.log("=" * 50)

        try:
            # Check if we're in a template with pre-existing tracks
            existing_tracks = self._find_template_tracks()

            if existing_tracks:
                self.log("✅ Template detected! Using existing tracks with instruments.")
                self._use_template_tracks(existing_tracks, genre)
            else:
                self.log("Creating new tracks...")
                self._create_new_tracks(genre)

        except Exception as e:
            self.log("=" * 50)
            self.log(f"FATAL ERROR: {str(e)}")
            self.log("=" * 50)
            import traceback
            self.log(traceback.format_exc())

    def _find_template_tracks(self):
        """Check if template tracks already exist"""
        template_track_names = ['AI Drums', 'AI Bass', 'AI Chords', 'AI Lead']
        existing = {}

        for track in self.song.tracks:
            if track.name in template_track_names:
                existing[track.name] = track

        # Return existing tracks only if we found all 4
        if len(existing) == 4:
            return existing
        return None

    def _use_template_tracks(self, tracks, genre='house'):
        """Add MIDI to existing template tracks (they already have instruments!)"""
        structure = TrackGenerator.generate_track(genre)
        bpm = structure['bpm']
        total_bars = structure['total_bars']
        self.log(f"Genre: {structure['genre']}, BPM: {bpm}, Key: {structure['scale']}")

        # Set tempo
        self.song.tempo = bpm
        self.log(f"Set tempo to {bpm} BPM")

        # Add MIDI to existing tracks
        try:
            track = tracks['AI Drums']
            clip_slot = track.clip_slots[0]
            if clip_slot.has_clip:
                clip_slot.delete_clip()
            clip_slot.create_clip(total_bars * 4.0)
            MIDIClipGenerator.create_drum_clip(clip_slot.clip, structure)
            self.log("✓ Drums MIDI added")
        except Exception as e:
            self.log(f"✗ Drums failed: {str(e)}")

        try:
            track = tracks['AI Bass']
            clip_slot = track.clip_slots[0]
            if clip_slot.has_clip:
                clip_slot.delete_clip()
            clip_slot.create_clip(total_bars * 4.0)
            MIDIClipGenerator.create_bass_clip(clip_slot.clip, structure)
            self.log("✓ Bass MIDI added")
        except Exception as e:
            self.log(f"✗ Bass failed: {str(e)}")

        try:
            track = tracks['AI Chords']
            clip_slot = track.clip_slots[0]
            if clip_slot.has_clip:
                clip_slot.delete_clip()
            clip_slot.create_clip(total_bars * 4.0)
            MIDIClipGenerator.create_chord_clip(clip_slot.clip, structure)
            self.log("✓ Chords MIDI added")
        except Exception as e:
            self.log(f"✗ Chords failed: {str(e)}")

        try:
            track = tracks['AI Lead']
            clip_slot = track.clip_slots[0]
            if clip_slot.has_clip:
                clip_slot.delete_clip()
            clip_slot.create_clip(total_bars * 4.0)
            MIDIClipGenerator.create_lead_clip(clip_slot.clip, structure)
            self.log("✓ Lead MIDI added")
        except Exception as e:
            self.log(f"✗ Lead failed: {str(e)}")

        # Apply effect chains to make it sound professional!
        self.log("")
        self.log("🎛️  Applying Effect Chains...")
        self.log("=" * 50)

        try:
            self.effect_chains.apply_chain(tracks['AI Drums'], 'drums')
        except:
            pass

        try:
            self.effect_chains.apply_chain(tracks['AI Bass'], 'bass')
        except:
            pass

        try:
            self.effect_chains.apply_chain(tracks['AI Chords'], 'chords')
        except:
            pass

        try:
            self.effect_chains.apply_chain(tracks['AI Lead'], 'lead')
        except:
            pass

        self.log("=" * 50)
        self.log(f"COMPLETE! {total_bars} bars, {bpm} BPM")
        self.log("Template tracks ready - press SPACE to play!")
        self.log("Check the log above for effect chain recommendations!")
        self.log("=" * 50)

    def _create_new_tracks(self, genre='house'):
        """Create new tracks from scratch (original behavior)"""
        structure = TrackGenerator.generate_track(genre)
        bpm = structure['bpm']
        total_bars = structure['total_bars']

        # Set tempo
        self.song.tempo = bpm
        self.log(f"Genre: {structure['genre']}, BPM: {bpm}, Key: {structure['scale']}")
        self.log(f"Set tempo to {bpm} BPM")

        # Create tracks
        try:
            self._create_drum_track(structure)
            self.log("✓ Drums created")
        except Exception as e:
            self.log(f"✗ Drums failed: {str(e)}")

        try:
            self._create_bass_track(structure)
            self.log("✓ Bass created")
        except Exception as e:
            self.log(f"✗ Bass failed: {str(e)}")

        try:
            self._create_chord_track(structure)
            self.log("✓ Chords created")
        except Exception as e:
            self.log(f"✗ Chords failed: {str(e)}")

        try:
            self._create_lead_track(structure)
            self.log("✓ Lead created")
        except Exception as e:
            self.log(f"✗ Lead failed: {str(e)}")

        # Apply effect chain recommendations
        self.log("")
        self.log("🎛️  Effect Chain Recommendations...")
        self.log("=" * 50)
        self.log("Once you add instruments, apply these effects:")

        self.effect_chains.apply_chain(None, 'drums')
        self.effect_chains.apply_chain(None, 'bass')
        self.effect_chains.apply_chain(None, 'chords')
        self.effect_chains.apply_chain(None, 'lead')

        self.log("=" * 50)
        self.log(f"COMPLETE! {total_bars} bars, {bpm} BPM")
        self.log("Add instruments, then apply effects for pro sound!")
        self.log("Press SPACE to play!")
        self.log("=" * 50)

    def _create_drum_track(self, structure):
        """Create drums track with Drum Rack and MIDI"""
        self.log("Creating drums track...")

        # Create MIDI track
        track_index = len(self.song.tracks)
        self.song.create_midi_track(track_index)
        track = self.song.tracks[track_index]
        track.name = "AI Drums"

        # Load Drum Rack automatically!
        self.log("Loading Drum Rack...")
        self.device_loader.load_instrument(track, 'drums')

        # Create MIDI clip
        clip_slot = track.clip_slots[0]
        clip_slot.create_clip(structure['total_bars'] * 4.0)
        clip = clip_slot.clip

        # Generate drums
        MIDIClipGenerator.create_drum_clip(clip, structure)

        self.log("Drums track created")

    def _create_bass_track(self, structure):
        """Create bass track with synth and MIDI"""
        self.log("Creating bass track...")

        track_index = len(self.song.tracks)
        self.song.create_midi_track(track_index)
        track = self.song.tracks[track_index]
        track.name = "AI Bass"

        # Load Operator for bass
        self.log("Loading Operator for bass...")
        self.device_loader.load_instrument(track, 'bass')

        # Create clip
        clip_slot = track.clip_slots[0]
        clip_slot.create_clip(structure['total_bars'] * 4.0)
        clip = clip_slot.clip

        # Generate bass
        MIDIClipGenerator.create_bass_clip(clip, structure)

        self.log("Bass track created")

    def _create_chord_track(self, structure):
        """Create chord track with pad and MIDI"""
        self.log("Creating chord track...")

        track_index = len(self.song.tracks)
        self.song.create_midi_track(track_index)
        track = self.song.tracks[track_index]
        track.name = "AI Chords"

        # Load Analog for chords
        self.log("Loading Analog for chords...")
        self.device_loader.load_instrument(track, 'chords')

        # Create clip
        clip_slot = track.clip_slots[0]
        clip_slot.create_clip(structure['total_bars'] * 4.0)
        clip = clip_slot.clip

        # Generate chords
        MIDIClipGenerator.create_chord_clip(clip, structure)

        self.log("Chord track created")

    def _create_lead_track(self, structure):
        """Create lead track with synth and MIDI"""
        self.log("Creating lead track...")

        track_index = len(self.song.tracks)
        self.song.create_midi_track(track_index)
        track = self.song.tracks[track_index]
        track.name = "AI Lead"

        # Load Wavetable for lead
        self.log("Loading Wavetable for lead...")
        self.device_loader.load_instrument(track, 'lead')

        # Create clip
        clip_slot = track.clip_slots[0]
        clip_slot.create_clip(structure['total_bars'] * 4.0)
        clip = clip_slot.clip

        # Generate lead
        MIDIClipGenerator.create_lead_clip(clip, structure)

        self.log("Lead track created")

    # MIDI CC or Key binding handlers
    def build_midi_map(self, midi_map_handle):
        """Build MIDI mappings - called by Live"""
        # Live only passes a note to receive_midi if the script asks for it
        script_handle = self.c_instance.handle()
        for channel in range(16):
            Live.MidiMap.forward_midi_note(script_handle, midi_map_handle, channel, self.TRIGGER_NOTE)

    def receive_midi(self, midi_bytes):
        """Handle incoming MIDI - can trigger generation"""
        # Note C3 (MIDI 60) triggers generation
        if len(midi_bytes) == 3:
            status, note, velocity = midi_bytes
            is_note_on = (status & 0xF0) == 0x90  # Note On, any channel
            if is_note_on and note == self.TRIGGER_NOTE and velocity > 0:
                self.log("Trigger received! Generating track...")
                self.generate_track()
