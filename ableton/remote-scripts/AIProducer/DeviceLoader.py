# Device Loader for AI Producer
# Provides helpful instrument recommendations

class DeviceLoader:
    """Helps users load the right instruments"""

    def __init__(self, app, log_function):
        self.app = app
        self.log = log_function

    def load_instrument(self, track, instrument_type):
        """
        Recommend which instrument to use
        Full automatic loading requires Max for Live or complex browser navigation
        For now, we give clear instructions
        """

        recommendations = {
            'drums': {
                'name': 'Drum Rack',
                'path': 'Browser → Drums → Drum Rack',
                'tip': 'Default drum sampler with 16 pads'
            },
            'bass': {
                'name': 'Operator',
                'path': 'Browser → Instruments → Operator',
                'tip': 'FM synth perfect for bass'
            },
            'chords': {
                'name': 'Analog',
                'path': 'Browser → Instruments → Analog',
                'tip': 'Warm analog-style synth for pads'
            },
            'lead': {
                'name': 'Wavetable',
                'path': 'Browser → Instruments → Wavetable',
                'tip': 'Modern synth great for leads'
            }
        }

        if instrument_type in recommendations:
            rec = recommendations[instrument_type]
            self.log("━" * 50)
            self.log(f"🎹 TO HEAR SOUND:")
            self.log(f"   1. Drag '{rec['name']}' onto '{track.name}' track")
            self.log(f"   2. Path: {rec['path']}")
            self.log(f"   3. {rec['tip']}")
            self.log("━" * 50)
            return True

        return False
