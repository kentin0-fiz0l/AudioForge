#!/usr/bin/env python3
"""
Automated Screenshot Capture for AudioForge Plugins
Uses subprocess and Quartz (macOS APIs) to capture plugin windows automatically
"""

import os
import sys
import time
import subprocess
import glob
from pathlib import Path

# Plugin configuration
PLUGINS = [
    ("PanUtil", "PanUtil"),
    ("SimpleEQ", "SimpleEQ"),
    ("SimpleComp", "SimpleComp"),
    ("Reverb", "Reverb"),
    ("CleanDelay", "CleanDelay"),
    ("Saturation", "Saturation"),
    ("WaveShaper", "WaveShaper"),
    ("ChorusFlanger", "ChorusFlanger"),
    ("SpectralFreeze", "SpectralFreeze"),
    ("FreezeFX", "FreezeFX"),
    ("BasicSynth", "BasicSynth"),
    ("FMSynth", "FMSynth"),
    ("WavetableSynth", "WavetableSynth"),
    ("PadSynth", "PadSynth"),
    ("DrumSynth", "DrumSynth"),
    ("OrganEmulator", "OrganEmulator"),
    ("GranularEngine", "GranularEngine"),
    ("SamplerPlugin", "Sampler"),
]

# Configuration
SCREENSHOT_DIR = "website/public/screenshots/plugins"
DOCS_SCREENSHOT_DIR = "docs/screenshots"
TARGET_WIDTH = 1200
TEMP_SCREENSHOT = "/tmp/audioforge_capture.png"

# Colors
class Colors:
    BLUE = '\033[0;34m'
    GREEN = '\033[0;32m'
    YELLOW = '\033[1;33m'
    RED = '\033[0;31m'
    BOLD = '\033[1m'
    NC = '\033[0m'

def print_header(text):
    print(f"{Colors.BOLD}{Colors.BLUE}{'='*50}{Colors.NC}")
    print(f"{Colors.BOLD}{Colors.BLUE}  {text}{Colors.NC}")
    print(f"{Colors.BOLD}{Colors.BLUE}{'='*50}{Colors.NC}")

def print_success(text):
    print(f"{Colors.GREEN}✓{Colors.NC} {text}")

def print_error(text):
    print(f"{Colors.RED}✗{Colors.NC} {text}")

def print_info(text):
    print(f"{Colors.YELLOW}→{Colors.NC} {text}")

def find_app(plugin):
    """Find the standalone .app for a plugin"""
    pattern = f"plugins/{plugin}/build/**/{plugin}.app"
    apps = glob.glob(pattern, recursive=True)
    return apps[0] if apps else None

def has_screenshot(display_name):
    """Check if plugin already has a screenshot"""
    screenshot_path = f"{SCREENSHOT_DIR}/{display_name}/{display_name.lower()}-main.png"
    return os.path.exists(screenshot_path)

def get_window_id(app_name):
    """Get window ID for an application"""
    try:
        # Use osascript to get window info
        script = f'''
        tell application "System Events"
            set appName to name of first process whose name contains "{app_name}"
            tell process appName
                set windowID to id of window 1
                return windowID
            end tell
        end tell
        '''
        result = subprocess.run(['osascript', '-e', script],
                              capture_output=True, text=True, timeout=5)
        if result.returncode == 0:
            return result.stdout.strip()
    except Exception as e:
        print_error(f"Could not get window ID: {e}")
    return None

def capture_window(app_name, output_path):
    """Capture a window screenshot"""
    try:
        # Method 1: Use window ID
        window_id = get_window_id(app_name)
        if window_id:
            result = subprocess.run(['screencapture', '-l', window_id, '-o', output_path],
                                  timeout=10)
            if result.returncode == 0 and os.path.exists(output_path):
                return True

        # Method 2: Fallback to capturing by process
        # Get list of windows and try to find the right one
        result = subprocess.run(['screencapture', '-l$(osascript -e \'tell app "System Events" to id of window 1 of process "' + app_name + '"\')', '-o', output_path],
                              shell=True, timeout=10)
        if result.returncode == 0 and os.path.exists(output_path):
            return True

    except Exception as e:
        print_error(f"Capture failed: {e}")

    return False

def optimize_screenshot(filepath):
    """Optimize screenshot size"""
    try:
        # Get original size
        result = subprocess.run(['du', '-h', filepath], capture_output=True, text=True)
        orig_size = result.stdout.split()[0] if result.returncode == 0 else "unknown"

        # Optimize
        subprocess.run(['sips', '-Z', str(TARGET_WIDTH), filepath],
                      capture_output=True, timeout=30)

        # Get new size
        result = subprocess.run(['du', '-h', filepath], capture_output=True, text=True)
        new_size = result.stdout.split()[0] if result.returncode == 0 else "unknown"

        return orig_size, new_size
    except Exception as e:
        print_error(f"Optimization failed: {e}")
        return "error", "error"

def close_app(plugin):
    """Close the plugin application"""
    try:
        # Try graceful quit first
        subprocess.run(['osascript', '-e', f'tell application "{plugin}" to quit'],
                      timeout=5)
        time.sleep(1)
    except:
        pass

    try:
        # Force quit if still running
        subprocess.run(['killall', plugin], stderr=subprocess.DEVNULL)
        time.sleep(1)
    except:
        pass

def capture_plugin(plugin, display_name, current, total):
    """Capture screenshot for one plugin"""
    print_header(f"{display_name} ({current}/{total})")

    # Check if already has screenshot
    if has_screenshot(display_name):
        print_success("Already has screenshot - skipping")
        print()
        return True

    # Find standalone app
    app_path = find_app(plugin)
    if not app_path:
        print_error(f"Standalone app not found for {plugin}")
        print_info("Attempting to build...")

        # Try to build
        try:
            os.chdir(f"plugins/{plugin}")
            if not os.path.exists("build"):
                subprocess.run(['cmake', '-B', 'build', '-DCMAKE_BUILD_TYPE=Release'],
                             check=True)
            subprocess.run(['cmake', '--build', 'build', '--config', 'Release',
                          '--target', f'{plugin}_Standalone'], check=True)
            os.chdir("../..")

            app_path = find_app(plugin)
            if not app_path:
                print_error("Build succeeded but app not found")
                return False
        except Exception as e:
            print_error(f"Build failed: {e}")
            os.chdir("../..")
            return False

    print_info(f"Found: {os.path.basename(app_path)}")

    # Launch the app
    print_info(f"Launching {display_name}...")
    try:
        subprocess.Popen(['open', app_path])
        time.sleep(4)  # Wait for app to fully launch
    except Exception as e:
        print_error(f"Failed to launch: {e}")
        return False

    # Capture the window
    print_info("Capturing screenshot...")
    screenshot_path = f"{SCREENSHOT_DIR}/{display_name}/{display_name.lower()}-main.png"

    # Create directory
    os.makedirs(f"{SCREENSHOT_DIR}/{display_name}", exist_ok=True)

    # Try to capture
    if capture_window(plugin, TEMP_SCREENSHOT):
        # Move to final location
        os.rename(TEMP_SCREENSHOT, screenshot_path)
        print_success(f"Screenshot captured")

        # Optimize
        orig_size, new_size = optimize_screenshot(screenshot_path)
        print_success(f"Optimized: {orig_size} → {new_size}")

        # Create backup
        os.makedirs(DOCS_SCREENSHOT_DIR, exist_ok=True)
        subprocess.run(['cp', screenshot_path,
                       f"{DOCS_SCREENSHOT_DIR}/{display_name}-screenshot.png"])
        print_success("Backup created")

        # Close app
        close_app(plugin)
        print_success("Plugin closed")
        print()
        return True
    else:
        print_error("Screenshot capture failed")
        close_app(plugin)
        print()
        return False

def main():
    """Main execution"""
    # Check we're in the right directory
    if not os.path.isdir("plugins"):
        print_error("Not in AudioForge root directory!")
        sys.exit(1)

    # Create directories
    os.makedirs(SCREENSHOT_DIR, exist_ok=True)
    os.makedirs(DOCS_SCREENSHOT_DIR, exist_ok=True)

    # Print intro
    subprocess.run(['clear'], check=False)
    print_header("AudioForge Automated Screenshot Capture")
    print()
    print("This script will automatically:")
    print("  1. Launch each plugin standalone app")
    print("  2. Capture a screenshot of the window")
    print("  3. Optimize and save it")
    print("  4. Close the plugin")
    print()
    print(f"Plugins to capture: {len(PLUGINS)}")
    print()
    print_info("Starting in 3 seconds... (Press Ctrl+C to cancel)")
    time.sleep(3)

    # Statistics
    total = len(PLUGINS)
    success = 0
    failed = 0

    # Process each plugin
    for i, (plugin, display_name) in enumerate(PLUGINS, 1):
        if capture_plugin(plugin, display_name, i, total):
            success += 1
        else:
            failed += 1
        time.sleep(1)

    # Summary
    subprocess.run(['clear'], check=False)
    print_header("Screenshot Capture Complete!")
    print()
    print(f"Results:")
    print(f"  Total plugins: {total}")
    print(f"  Successful: {success}")
    print(f"  Failed: {failed}")
    print()

    # Count total screenshots
    total_screenshots = len([d for d in os.listdir(SCREENSHOT_DIR)
                           if os.path.isdir(os.path.join(SCREENSHOT_DIR, d))])
    print_success(f"Total plugins with screenshots: {total_screenshots}/22")
    print()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print()
        print_info("Cancelled by user")
        sys.exit(0)
