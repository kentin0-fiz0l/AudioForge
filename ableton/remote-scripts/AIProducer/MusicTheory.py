# Music Theory Engine for AI Producer
# Scales, chords, progressions, and drum patterns

class MusicTheory:
    """Music theory utilities for generating musical content"""

    # MIDI note numbers for drums (General MIDI standard)
    KICK = 36
    SNARE = 38
    CLOSED_HAT = 42
    OPEN_HAT = 46
    CRASH = 49

    # Musical scales (semitone intervals from root)
    SCALES = {
        'Major': [0, 2, 4, 5, 7, 9, 11],
        'Minor': [0, 2, 3, 5, 7, 8, 10],
        'Dorian': [0, 2, 3, 5, 7, 9, 10],
        'Phrygian': [0, 1, 3, 5, 7, 8, 10],
        'Mixolydian': [0, 2, 4, 5, 7, 9, 10],
        'Pentatonic': [0, 2, 4, 7, 9],
        'Blues': [0, 3, 5, 6, 7, 10],
    }

    # Common chord progressions (Roman numerals)
    PROGRESSIONS = {
        'house_classic': 'i-VI-III-VII',  # Minor house
        'pop_classic': 'I-V-vi-IV',       # Pop classic
        'emotional': 'vi-IV-I-V',         # Emotional
        'simple': 'i-i-i-i',              # Loop
    }

    @staticmethod
    def get_scale_notes(root, scale_name):
        """Get MIDI notes for a scale starting from root"""
        intervals = MusicTheory.SCALES.get(scale_name, MusicTheory.SCALES['Minor'])
        return [root + interval for interval in intervals]

    @staticmethod
    def parse_progression(root, scale_name, progression_pattern):
        """Parse Roman numeral progression into MIDI notes"""
        scale_notes = MusicTheory.get_scale_notes(root, scale_name)
        chords = []

        # Simple progression parser
        for numeral in progression_pattern.split('-'):
            # Map to scale degree (simplified)
            if 'VII' in numeral or 'vii' in numeral:
                degree = 6
            elif 'VI' in numeral or 'vi' in numeral:
                degree = 5
            elif 'V' in numeral or 'v' in numeral:
                degree = 4
            elif 'IV' in numeral or 'iv' in numeral:
                degree = 3
            elif 'III' in numeral or 'iii' in numeral:
                degree = 2
            elif 'II' in numeral or 'ii' in numeral:
                degree = 1
            else:  # I or i
                degree = 0

            root_note = scale_notes[degree % len(scale_notes)]

            # Build triad (root + third + fifth)
            third = scale_notes[(degree + 2) % len(scale_notes)]
            fifth = scale_notes[(degree + 4) % len(scale_notes)]

            chords.append([root_note, third, fifth])

        return chords

    @staticmethod
    def get_drum_pattern(style='house'):
        """Get drum pattern for a style (16th note grid, 0-15)"""
        patterns = {
            'house': {
                'kick': [0, 4, 8, 12],              # Four on floor
                'snare': [4, 12],                    # 2 and 4
                'hat_closed': [0, 2, 4, 6, 8, 10, 12, 14],  # 8ths
                'hat_open': [6, 14],                 # Accents
            },
            'techno': {
                'kick': [0, 6, 8, 14],              # Harder, syncopated
                'snare': [4, 8, 12],                 # More snares
                'hat_closed': list(range(16)),       # 16ths (driving)
                'hat_open': [7, 15],                 # Sparse opens
            },
            'dnb': {  # Drum & Bass
                'kick': [0, 10],                     # Sparse kicks
                'snare': [4, 13],                    # Syncopated snare
                'hat_closed': [0, 2, 3, 4, 6, 8, 10, 11, 12, 14],  # Fast hats
                'hat_open': [],                      # No opens (clean)
            },
            'hiphop': {
                'kick': [0, 6, 10],                  # Boom bap
                'snare': [4, 12],                    # 2 and 4 (classic)
                'hat_closed': [0, 2, 4, 6, 8, 10, 12, 14],  # 8th notes
                'hat_open': [3, 7, 11, 15],          # Swung opens
            },
            'ambient': {
                'kick': [],                          # No drums!
                'snare': [],
                'hat_closed': [],
                'hat_open': [],
            },
        }

        return patterns.get(style, patterns['house'])


class TrackGenerator:
    """Generates complete track arrangements"""

    @staticmethod
    def generate_track(genre='house'):
        """Generate a track structure for the specified genre"""
        generators = {
            'house': TrackGenerator.generate_house_track,
            'techno': TrackGenerator.generate_techno_track,
            'dnb': TrackGenerator.generate_dnb_track,
            'hiphop': TrackGenerator.generate_hiphop_track,
            'ambient': TrackGenerator.generate_ambient_track,
        }

        generator = generators.get(genre.lower(), TrackGenerator.generate_house_track)
        return generator()

    @staticmethod
    def generate_house_track():
        """Generate a progressive house track structure"""
        return {
            'genre': 'House',
            'bpm': 128,
            'key': 57,  # A (MIDI note)
            'scale': 'Minor',
            'progression': 'i-VI-III-VII',
            'total_bars': 72,
            'sections': [
                {'name': 'intro', 'start': 0, 'length': 8, 'energy': 3},
                {'name': 'build', 'start': 8, 'length': 8, 'energy': 6},
                {'name': 'drop', 'start': 16, 'length': 16, 'energy': 10},
                {'name': 'breakdown', 'start': 32, 'length': 8, 'energy': 4},
                {'name': 'build', 'start': 40, 'length': 8, 'energy': 7},
                {'name': 'drop', 'start': 48, 'length': 16, 'energy': 10},
                {'name': 'outro', 'start': 64, 'length': 8, 'energy': 2},
            ]
        }

    @staticmethod
    def generate_techno_track():
        """Generate a techno track - harder, faster, relentless"""
        return {
            'genre': 'Techno',
            'bpm': 135,
            'key': 50,  # D (darker)
            'scale': 'Phrygian',  # Dark scale
            'progression': 'i-i-i-i',  # Minimal progression
            'total_bars': 64,
            'sections': [
                {'name': 'intro', 'start': 0, 'length': 16, 'energy': 7},
                {'name': 'drop', 'start': 16, 'length': 32, 'energy': 10},
                {'name': 'breakdown', 'start': 48, 'length': 8, 'energy': 5},
                {'name': 'drop', 'start': 56, 'length': 8, 'energy': 10},
            ]
        }

    @staticmethod
    def generate_dnb_track():
        """Generate a Drum & Bass track - fast breaks, heavy bass"""
        return {
            'genre': 'Drum & Bass',
            'bpm': 174,
            'key': 55,  # G
            'scale': 'Minor',
            'progression': 'i-VI-III-VII',
            'total_bars': 64,
            'sections': [
                {'name': 'intro', 'start': 0, 'length': 8, 'energy': 4},
                {'name': 'build', 'start': 8, 'length': 8, 'energy': 7},
                {'name': 'drop', 'start': 16, 'length': 16, 'energy': 10},
                {'name': 'breakdown', 'start': 32, 'length': 8, 'energy': 3},
                {'name': 'drop', 'start': 40, 'length': 16, 'energy': 10},
                {'name': 'outro', 'start': 56, 'length': 8, 'energy': 2},
            ]
        }

    @staticmethod
    def generate_hiphop_track():
        """Generate a Hip-Hop track - boom bap, laid back"""
        return {
            'genre': 'Hip-Hop',
            'bpm': 90,
            'key': 60,  # C
            'scale': 'Minor',
            'progression': 'i-VII-VI-V',
            'total_bars': 32,
            'sections': [
                {'name': 'intro', 'start': 0, 'length': 4, 'energy': 5},
                {'name': 'verse', 'start': 4, 'length': 12, 'energy': 7},
                {'name': 'hook', 'start': 16, 'length': 8, 'energy': 9},
                {'name': 'verse', 'start': 24, 'length': 8, 'energy': 7},
            ]
        }

    @staticmethod
    def generate_ambient_track():
        """Generate an Ambient track - no drums, evolving pads"""
        return {
            'genre': 'Ambient',
            'bpm': 65,
            'key': 62,  # D
            'scale': 'Dorian',
            'progression': 'I-V-vi-IV',  # Spacious
            'total_bars': 64,
            'sections': [
                {'name': 'intro', 'start': 0, 'length': 16, 'energy': 2},
                {'name': 'swell', 'start': 16, 'length': 16, 'energy': 5},
                {'name': 'peak', 'start': 32, 'length': 16, 'energy': 7},
                {'name': 'decay', 'start': 48, 'length': 16, 'energy': 3},
            ]
        }
