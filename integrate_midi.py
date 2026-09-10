#!/usr/bin/env python3
"""Integrate MIDI Learn into AudioForge plugins"""
import sys
import re
from pathlib import Path

def integrate_plugin(plugin_dir):
    """Integrate MIDI Learn into a plugin"""
    plugin_path = Path(plugin_dir)
    plugin_name = plugin_path.name

    print(f"🔧 Integrating {plugin_name}...")

    # 1. Update PluginProcessor.h
    header_path = plugin_path / "Source" / "PluginProcessor.h"
    if not header_path.exists():
        print(f"  ✗ Header not found: {header_path}")
        return False

    with open(header_path, 'r') as f:
        header = f.read()

    # Skip if already integrated
    if 'MIDILearnManager' in header:
        print(f"  ⚠ Already integrated")
        return True

    # Add include after last #include
    includes = [m.start() for m in re.finditer(r'^#include', header, re.MULTILINE)]
    if includes:
        last_include_line = header[:includes[-1]].count('\n')
        lines = header.split('\n')
        lines.insert(last_include_line + 1, '#include "../../../midi/MIDILearnManager.h"')
        header = '\n'.join(lines)

    # Add accessor before private:
    header = re.sub(
        r'(\n)(private:)',
        r'\1    AudioForge::MIDILearnManager& getMidiLearnManager() { return midiLearnManager_; }\n\n\2',
        header
    )

    # Add member before JUCE_DECLARE
    header = re.sub(
        r'(\n    )(JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR)',
        r'\1AudioForge::MIDILearnManager midiLearnManager_;\n\n\2',
        header
    )

    with open(header_path, 'w') as f:
        f.write(header)

    print(f"  ✓ Header updated")

    # 2. Update PluginProcessor.cpp
    cpp_path = plugin_path / "Source" / "PluginProcessor.cpp"
    if not cpp_path.exists():
        print(f"  ✗ Implementation not found: {cpp_path}")
        return False

    with open(cpp_path, 'r') as f:
        cpp = f.read()

    # Update constructor - find apvts_ initialization and add midiLearnManager after it
    cpp = re.sub(
        r'(apvts_\([^)]+createParameterLayout\(\)\))\s*\{',
        r'\1,\n      midiLearnManager_(apvts_) {',
        cpp
    )

    # Update processBlock - add MIDI processing
    # Handle both juce::MidiBuffer& and juce::MidiBuffer& midi
    cpp = re.sub(
        r'(void\s+\w+::processBlock[^{]+juce::MidiBuffer&)(\s+midi)?\s*\)',
        r'\1 midi)',
        cpp
    )

    # Add MIDI processing after ScopedNoDenormals or at start of processBlock
    if 'midiLearnManager_.processMidiMessage' not in cpp:
        cpp = re.sub(
            r'(void\s+\w+::processBlock[^{]+\{[^\n]*\n(?:[^\n]*ScopedNoDenormals[^\n]*\n)?)',
            r'\1    for (const auto metadata : midi)\n        midiLearnManager_.processMidiMessage(metadata.getMessage());\n',
            cpp,
            count=1
        )

    # Update getStateInformation
    cpp = re.sub(
        r'(std::unique_ptr<juce::XmlElement>\s+(\w+)\([^)]*createXml\(\)\);)\s*\n\s*copyXmlToBinary',
        r'\1\n    \2->addChildElement(midiLearnManager_.saveToXml().release());\n    copyXmlToBinary',
        cpp
    )

    # Update setStateInformation - find the xml variable name
    xml_var_match = re.search(r'std::unique_ptr<juce::XmlElement>\s+(\w+)\(getXmlFromBinary', cpp)
    if xml_var_match:
        xml_var = xml_var_match.group(1)
        # Add MIDI load after replaceState
        cpp = re.sub(
            r'(apvts_\.replaceState\(juce::ValueTree::fromXml\(\*' + xml_var + r'\)\);)',
            r'\1\n        if (auto* midiXml = ' + xml_var + r'->getChildByName("MIDILearnMappings"))\n            midiLearnManager_.loadFromXml(*midiXml);',
            cpp
        )

    with open(cpp_path, 'w') as f:
        f.write(cpp)

    print(f"  ✓ Implementation updated")

    # 3. Update CMakeLists.txt
    cmake_path = plugin_path / "CMakeLists.txt"
    if not cmake_path.exists():
        print(f"  ✗ CMakeLists.txt not found")
        return False

    with open(cmake_path, 'r') as f:
        cmake = f.read()

    if 'MIDILearnManager.cpp' in cmake:
        print(f"  ⚠ CMakeLists already updated")
    else:
        # Find target_sources and add MIDI source
        cmake = re.sub(
            r'(target_sources\([^\)]+Source/[^\)]+\.cpp)',
            r'\1\n    ../../midi/MIDILearnManager.cpp',
            cmake
        )

        with open(cmake_path, 'w') as f:
            f.write(cmake)

        print(f"  ✓ CMakeLists updated")

    return True

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python integrate_midi.py <plugin_dir> [<plugin_dir> ...]")
        sys.exit(1)

    for plugin_dir in sys.argv[1:]:
        try:
            success = integrate_plugin(plugin_dir)
            if success:
                print(f"✅ {Path(plugin_dir).name} integrated\n")
            else:
                print(f"❌ {Path(plugin_dir).name} failed\n")
        except Exception as e:
            print(f"❌ {Path(plugin_dir).name} error: {e}\n")
