#!/usr/bin/env python3
"""Update plugin PluginProcessor.cpp files for MIDI Learn integration"""
import re
import sys

def update_processor_cpp(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # 1. Update constructor - add midiLearnManager_(apvts_)
    # Find constructor initialization list ending
    content = re.sub(
        r'(apvts_\([^)]+\))\s*\{',
        r'\1,\n      midiLearnManager_(apvts_) {',
        content
    )

    # 2. Update processBlock - add MIDI learn processing
    # Find processBlock and add MIDI processing after ScopedNoDenormals or at start
    content = re.sub(
        r'(void\s+\w+::processBlock[^{]+\{[^}]*?juce::ScopedNoDenormals[^;]+;)',
        r'\1\n    for (const auto metadata : midi)\n        midiLearnManager_.processMidiMessage(metadata.getMessage());',
        content,
        flags=re.DOTALL
    )

    # If no ScopedNoDenormals, add after opening brace
    if 'midiLearnManager_.processMidiMessage' not in content:
        content = re.sub(
            r'(void\s+\w+::processBlock[^{]+\{)',
            r'\1\n    for (const auto metadata : midi)\n        midiLearnManager_.processMidiMessage(metadata.getMessage());',
            content
        )

    # 3. Update getStateInformation - add MIDI mappings save
    content = re.sub(
        r'(std::unique_ptr<juce::XmlElement>\s+\w+\(.*?createXml\(\)\);)\s*copyXmlToBinary',
        r'\1\n    x->addChildElement(midiLearnManager_.saveToXml().release());\n    copyXmlToBinary',
        content,
        flags=re.DOTALL
    )

    # 4. Update setStateInformation - add MIDI mappings load
    content = re.sub(
        r'(apvts_\.replaceState\([^)]+\);)',
        r'\1\n        if (auto* midiXml = \1->getChildByName("MIDILearnMappings"))\n            midiLearnManager_.loadFromXml(*midiXml);',
        content
    )

    # Fix the variable name in getChildByName (use 'x' or 'xml' or whatever is in scope)
    # Find the variable name used in setStateInformation
    xml_var_match = re.search(r'std::unique_ptr<juce::XmlElement>\s+(\w+)\(getXmlFromBinary', content)
    if xml_var_match:
        xml_var = xml_var_match.group(1)
        content = content.replace('->getChildByName', f'{xml_var}->getChildByName')

    with open(filepath, 'w') as f:
        f.write(content)

    return True

if __name__ == '__main__':
    for filepath in sys.argv[1:]:
        try:
            update_processor_cpp(filepath)
            print(f"✓ Updated {filepath}")
        except Exception as e:
            print(f"✗ Failed {filepath}: {e}")
