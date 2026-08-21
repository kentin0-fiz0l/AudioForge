/**
 * AudioForge Test Runner
 *
 * Main entry point for running all JUCE unit tests.
 * Executes DSP, plugin, and integration tests.
 */

#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <iostream>

// Forward declarations of test classes (defined in separate files)
class GainProcessingTests;
class PanningTests;
class ParameterSmoothingTests;
class SimpleGainPluginTests;
class PanUtilPluginTests;
class PluginIntegrationTests;

int main(int argc, char* argv[])
{
    // Initialize JUCE
    juce::ScopedJuceInitialiser_GUI juceInit;

    // Create test runner
    juce::UnitTestRunner runner;

    std::cout << "========================================\n";
    std::cout << "    AudioForge Test Suite\n";
    std::cout << "========================================\n\n";

    // Parse command line arguments
    bool verbose = false;
    juce::String categoryFilter;

    for (int i = 1; i < argc; ++i)
    {
        juce::String arg(argv[i]);

        if (arg == "-v" || arg == "--verbose")
            verbose = true;
        else if (arg.startsWith("--category="))
            categoryFilter = arg.substring(11);
    }

    // Get all registered tests
    auto& tests = juce::UnitTest::getAllTests();

    if (categoryFilter.isNotEmpty())
    {
        std::cout << "Running tests in category: " << categoryFilter << "\n\n";

        // Run only tests matching the category
        for (auto* test : tests)
        {
            if (juce::String(test->getCategory()) == categoryFilter)
                runner.runTests({test}, 0); // 0 = use default seed
        }
    }
    else
    {
        std::cout << "Running all tests (" << tests.size() << " test suites)\n\n";

        // Run all tests
        runner.runAllTests(0); // 0 = use default seed
    }

    // Print results
    std::cout << "\n========================================\n";
    std::cout << "    Test Results\n";
    std::cout << "========================================\n";

    int numResults = runner.getNumResults();
    int numPassed = 0;
    int numFailed = 0;

    for (int i = 0; i < numResults; ++i)
    {
        auto* result = runner.getResult(i);

        if (result->failures == 0)
            numPassed++;
        else
            numFailed++;

        if (verbose || result->failures > 0)
        {
            std::cout << "\n" << result->unitTestName << ":\n";
            std::cout << "  Passed: " << result->passes << "\n";
            std::cout << "  Failed: " << result->failures << "\n";

            if (result->failures > 0)
            {
                std::cout << "  Failure messages:\n";
                for (const auto& msg : result->messages)
                    std::cout << "    - " << msg << "\n";
            }
        }
    }

    std::cout << "\n========================================\n";
    std::cout << "Total Test Suites: " << numResults << "\n";
    std::cout << "Passed: " << numPassed << "\n";
    std::cout << "Failed: " << numFailed << "\n";
    std::cout << "========================================\n";

    // Return 0 for success, 1 for failure
    return (numFailed == 0) ? 0 : 1;
}
