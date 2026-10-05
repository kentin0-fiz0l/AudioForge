# MIDI Clip Generator for Ableton Live
import Live
from .MusicTheory import MusicTheory, TrackGenerator


def write_notes(clip, notes):
    """Add notes to a clip. Each is (pitch, start, duration, velocity, muted)."""
    if not notes:
        return

    # Live 11 and later take note specifications. The older call is still
    # there but deprecated, and is all a clip has before Live 11.
    if getattr(clip, 'add_new_notes', None) is not None:
        clip.add_new_notes(tuple(
            Live.Clip.MidiNoteSpecification(pitch=pitch, start_time=start, duration=duration,
                                            velocity=velocity, mute=muted)
            for pitch, start, duration, velocity, muted in notes))
    else:
        clip.set_notes(tuple(notes))

class MIDIClipGenerator:
    """Generates MIDI clips for Ableton Live tracks"""

    @staticmethod
    def create_drum_clip(clip, structure):
        """Generate drums in a clip"""
        clip.name = f"AI Drums - {structure.get('genre', 'House')}"

        # Get genre-specific drum pattern
        pattern = MusicTheory.get_drum_pattern(structure.get('style', 'house'))
        total_bars = structure['total_bars']

        # Clear existing notes
        clip.remove_notes_extended(0, 128, 0, total_bars * 4)

        notes = []

        for bar in range(total_bars):
            bar_start = bar * 4.0  # 4 beats per bar

            # Kick drum
            for step in pattern['kick']:
                time = bar_start + (step * 0.25)  # 16th notes
                notes.append((MusicTheory.KICK, time, 0.1, 100, False))

            # Snare
            for step in pattern['snare']:
                time = bar_start + (step * 0.25)
                notes.append((MusicTheory.SNARE, time, 0.1, 90, False))

            # Clap, in the styles that have one
            for step in pattern.get('clap', []):
                time = bar_start + (step * 0.25)
                notes.append((MusicTheory.CLAP, time, 0.1, 95, False))

            # Closed hi-hat
            for step in pattern['hat_closed']:
                time = bar_start + (step * 0.25)
                notes.append((MusicTheory.CLOSED_HAT, time, 0.1, 70, False))

            # Open hi-hat
            for step in pattern['hat_open']:
                time = bar_start + (step * 0.25)
                notes.append((MusicTheory.OPEN_HAT, time, 0.1, 80, False))

        write_notes(clip, notes)

        # Set loop length
        clip.loop_end = total_bars * 4.0

    @staticmethod
    def create_bass_clip(clip, structure):
        """Generate bass in a clip"""
        clip.name = "AI Bass"

        root = structure['key']
        total_bars = structure['total_bars']

        clip.remove_notes_extended(0, 128, 0, total_bars * 4)

        notes = []
        bass_note = root - 12  # One octave lower

        # Simple: bass on every beat
        for bar in range(total_bars):
            for beat in range(4):
                time = bar * 4.0 + beat
                notes.append((bass_note, time, 0.9, 80, False))

        write_notes(clip, notes)

        clip.loop_end = total_bars * 4.0

    @staticmethod
    def create_chord_clip(clip, structure):
        """Generate chords in a clip"""
        clip.name = "AI Chords"

        root = structure['key']
        scale = structure['scale']
        progression_pattern = structure['progression']
        total_bars = structure['total_bars']

        clip.remove_notes_extended(0, 128, 0, total_bars * 4)

        # Get chord progression
        chords = MusicTheory.parse_progression(root, scale, progression_pattern)

        notes = []

        # 4-bar chord progression, repeated
        for bar in range(total_bars):
            chord_index = bar % len(chords)
            chord = chords[chord_index]

            time = bar * 4.0
            duration = 4.0  # Whole note

            # Add each note in the chord (up an octave for pads)
            for note in chord:
                notes.append((note + 12, time, duration, 60, False))

        write_notes(clip, notes)

        clip.loop_end = total_bars * 4.0

    @staticmethod
    def create_lead_clip(clip, structure):
        """Generate lead melody in a clip"""
        clip.name = "AI Lead"

        root = structure['key']
        scale = structure['scale']
        total_bars = structure['total_bars']

        clip.remove_notes_extended(0, 128, 0, total_bars * 4)

        # Get scale notes for melody
        scale_notes = MusicTheory.get_scale_notes(root + 24, scale)  # 2 octaves up

        notes = []

        # Generate melody only in "drop" sections
        for section in structure['sections']:
            if section['name'] == 'drop':
                start_bar = section['start']
                end_bar = start_bar + section['length']

                # Simple pentatonic melody (8th notes)
                for bar in range(start_bar, end_bar, 2):  # Every 2 bars
                    for i in range(8):
                        time = bar * 4.0 + i * 0.5
                        note = scale_notes[i % len(scale_notes)]
                        notes.append((note, time, 0.4, 90, False))

        write_notes(clip, notes)

        clip.loop_end = total_bars * 4.0
