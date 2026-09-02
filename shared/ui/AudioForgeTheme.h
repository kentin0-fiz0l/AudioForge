/*
  ==============================================================================

    AudioForgeTheme.h
    Created: 1 Sep 2026
    Author:  AudioForge Team

    Centralized design system for all AudioForge plugins.
    Provides consistent colors, typography, spacing, and component styles.

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace AudioForge
{

/**
 * @brief Color palette for AudioForge plugins
 *
 * Dark theme optimized for audio production environments.
 * Colors chosen for readability and professional appearance.
 */
namespace Colors
{
    // Backgrounds
    const juce::Colour Background      = juce::Colour(0xff1a1a1a);  // Main dark background
    const juce::Colour Surface         = juce::Colour(0xff2a2a2a);  // Elevated surfaces (panels, cards)
    const juce::Colour SurfaceVariant  = juce::Colour(0xff2d2d2d);  // Alternative surface color

    // Brand Colors
    const juce::Colour Primary         = juce::Colour(0xff4a9eff);  // Blue - primary actions, accents
    const juce::Colour PrimaryDark     = juce::Colour(0xff3578d4);  // Darker blue for hover states
    const juce::Colour Accent          = juce::Colour(0xffff6b35);  // Orange - highlights, warnings

    // Text
    const juce::Colour Text            = juce::Colours::white;      // Primary text
    const juce::Colour TextSecondary   = juce::Colour(0xff888888);  // Secondary text (subtitles, labels)
    const juce::Colour TextDisabled    = juce::Colour(0xff555555);  // Disabled state

    // Metering (Audio Level Display)
    const juce::Colour MeterLow        = juce::Colour(0xff4ade80);  // Green - safe levels (-inf to -12dB)
    const juce::Colour MeterMid        = juce::Colour(0xfffbbf24);  // Yellow - caution (-12 to -3dB)
    const juce::Colour MeterHigh       = juce::Colour(0xffef4444);  // Red - clipping (-3 to 0dB)
    const juce::Colour MeterBackground = juce::Colour(0xff333333);  // Meter track background

    // Control States
    const juce::Colour ControlOutline  = juce::Colour(0xff444444);  // Slider/button outlines
    const juce::Colour ControlFill     = juce::Colour(0xff3a3a3a);  // Slider/button fill
    const juce::Colour ControlActive   = Primary;                   // Active/selected state

    // Borders & Dividers
    const juce::Colour Border          = juce::Colour(0xff333333);  // Subtle borders
    const juce::Colour Divider         = juce::Colour(0xff2a2a2a);  // Section dividers
}

/**
 * @brief Typography scale for consistent text sizing
 */
namespace Typography
{
    // Font Sizes
    const float Title          = 24.0f;  // Plugin name
    const float Subtitle       = 14.0f;  // Plugin category/description
    const float Heading        = 16.0f;  // Section headings
    const float Body           = 12.0f;  // Parameter labels, general text
    const float Small          = 10.0f;  // Auxiliary info, tooltips

    // Font Weights (JUCE Font::FontStyleFlags)
    const int Regular = juce::Font::plain;
    const int Bold    = juce::Font::bold;
}

/**
 * @brief Spacing scale for consistent layouts
 *
 * Based on 8px grid system for harmonious spacing.
 */
namespace Spacing
{
    const int Tiny     = 4;   // 4px  - Tight spacing
    const int Small    = 8;   // 8px  - Default padding
    const int Medium   = 16;  // 16px - Component spacing
    const int Large    = 24;  // 24px - Section spacing
    const int XLarge   = 32;  // 32px - Major section breaks
    const int XXLarge  = 48;  // 48px - Between major UI areas
}

/**
 * @brief Component dimensions for standard UI elements
 */
namespace Dimensions
{
    // Sliders
    const int RotarySize       = 60;   // Standard rotary slider diameter
    const int RotaryLarge      = 80;   // Large rotary (primary controls)
    const int LinearHeight     = 30;   // Linear slider height
    const int TextBoxWidth     = 70;   // Slider text box width
    const int TextBoxHeight    = 18;   // Slider text box height

    // Buttons
    const int ButtonHeight     = 25;   // Standard button height
    const int ButtonWidth      = 60;   // Standard button width (solo/bypass/etc)

    // Meters
    const int MeterHeight      = 20;   // Horizontal meter height
    const int MeterWidth       = 150;  // Standard meter width

    // Window Sizes (by plugin complexity)
    const int WindowSmall      = 400;  // Simple plugins (width)
    const int WindowMedium     = 600;  // Mid-complexity plugins
    const int WindowLarge      = 900;  // Complex plugins (multiband, etc)
    const int WindowHeight     = 300;  // Standard height (simple)
    const int WindowHeightTall = 600;  // Tall height (complex)
}

/**
 * @brief Standard UI layouts and helper functions
 */
namespace Layout
{
    /**
     * @brief Draw standard title bar (plugin name + subtitle)
     *
     * @param g Graphics context
     * @param title Plugin name (e.g., "AudioForge Limiter")
     * @param subtitle Plugin category (e.g., "True Peak Limiting")
     * @param width Window width
     */
    inline void drawTitleBar(juce::Graphics& g,
                             const juce::String& title,
                             const juce::String& subtitle,
                             int width)
    {
        // Title
        g.setColour(Colors::Text);
        g.setFont(juce::Font(Typography::Title, Typography::Bold));
        g.drawText(title, 0, 10, width, 30, juce::Justification::centred);

        // Subtitle (if provided)
        if (subtitle.isNotEmpty())
        {
            g.setFont(juce::Font(Typography::Subtitle, Typography::Regular));
            g.setColour(Colors::Primary);
            g.drawText(subtitle, 0, 35, width, 20, juce::Justification::centred);
        }
    }

    /**
     * @brief Draw horizontal audio level meter with gradient coloring
     *
     * @param g Graphics context
     * @param x X position
     * @param y Y position
     * @param width Meter width
     * @param height Meter height
     * @param level Current level (0.0 = -inf, 1.0 = 0dB)
     */
    inline void drawMeter(juce::Graphics& g,
                         int x, int y, int width, int height,
                         float level)
    {
        // Meter background
        g.setColour(Colors::MeterBackground);
        g.fillRect(x, y, width, height);

        // Clamp level to valid range
        level = juce::jlimit(0.0f, 1.0f, level);

        // Calculate fill width
        int fillWidth = static_cast<int>(level * width);

        if (fillWidth > 0)
        {
            // Color gradient: green → yellow → red
            juce::Colour meterColor;

            if (level < 0.6f)  // -inf to -12dB (safe)
                meterColor = Colors::MeterLow;
            else if (level < 0.85f)  // -12 to -3dB (caution)
                meterColor = Colors::MeterMid;
            else  // -3 to 0dB (clipping danger)
                meterColor = Colors::MeterHigh;

            g.setColour(meterColor);
            g.fillRect(x, y, fillWidth, height);
        }
    }

    /**
     * @brief Draw section divider line
     *
     * @param g Graphics context
     * @param x X position
     * @param y Y position
     * @param width Line width
     */
    inline void drawDivider(juce::Graphics& g, int x, int y, int width)
    {
        g.setColour(Colors::Divider);
        g.drawLine(static_cast<float>(x), static_cast<float>(y),
                   static_cast<float>(x + width), static_cast<float>(y),
                   1.0f);
    }
}

/**
 * @brief Plugin categories for consistent subtitles
 */
namespace Categories
{
    const juce::String Dynamics      = "Dynamics Processor";
    const juce::String EQ            = "Equalizer";
    const juce::String Modulation    = "Modulation Effect";
    const juce::String Delay         = "Delay Effect";
    const juce::String Reverb        = "Reverb Effect";
    const juce::String Distortion    = "Distortion Effect";
    const juce::String Utility       = "Utility";
    const juce::String Synthesizer   = "Synthesizer";
    const juce::String Mastering     = "Mastering Tool";
}

} // namespace AudioForge
