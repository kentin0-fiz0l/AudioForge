# AI Producer - Autonomous Music Generation for Ableton Live
# Remote Script Entry Point

from .AIProducer import AIProducer

def create_instance(c_instance):
    """Create and return script instance"""
    return AIProducer(c_instance)
