# Security Policy

## Supported Versions

We provide security updates for the following versions of AudioForge:

| Version | Supported          |
| ------- | ------------------ |
| 1.1.x   | :white_check_mark: |
| 1.0.x   | :white_check_mark: |
| < 1.0   | :x:                |

## Reporting a Vulnerability

We take security seriously. If you discover a security vulnerability in AudioForge, please report it responsibly.

### How to Report

**DO NOT** open a public GitHub issue for security vulnerabilities.

Instead, please report security issues via one of the following methods:

1. **Email** (preferred): security@audioforge.art
2. **GitHub Security Advisory**: [Report a vulnerability](https://github.com/username/AudioForge/security/advisories/new)

### What to Include

Please provide the following information in your report:

- **Description**: Clear description of the vulnerability
- **Impact**: What can an attacker do with this vulnerability?
- **Reproduction steps**: Step-by-step instructions to reproduce
- **Affected versions**: Which versions are affected?
- **Proof of concept**: Code or files demonstrating the issue (if applicable)
- **Suggested fix**: If you have ideas for a fix (optional)

### Response Timeline

We will acknowledge your report within **48 hours** and provide:

- Confirmation of receipt
- Initial assessment of severity
- Expected timeline for investigation and fix

We aim to:

- **Low severity**: Fix within 90 days
- **Medium severity**: Fix within 30 days
- **High/Critical severity**: Fix within 7 days

### Disclosure Policy

We follow **coordinated disclosure**:

1. You report the vulnerability privately
2. We confirm and investigate the issue
3. We develop and test a fix
4. We release a security update
5. We publicly disclose the vulnerability (with your permission) after users have had time to update

### Security Updates

Security updates are released as:

- **Patch versions** (e.g., v1.1.1) for minor fixes
- **Minor versions** (e.g., v1.2.0) for more significant changes

Users are notified via:
- GitHub releases with `[SECURITY]` tag
- Announcement in GitHub Discussions
- Website security advisory page (if critical)

## Known Security Considerations

### Plugin Loading

AudioForge plugins are loaded as native libraries (.vst3, .component) by your DAW. As with any native plugin:

- **Trust your source**: Only download AudioForge from official sources (GitHub releases, audioforge.fluxstudio.art)
- **Verify signatures**: macOS and Windows releases are code-signed to verify authenticity
- **Check hashes**: Compare SHA-256 checksums provided in releases

### Code Signing

**macOS**:
- Plugins are signed with **Developer ID Application** certificate
- DMG installer is **notarized** by Apple
- No Gatekeeper warnings on macOS 10.15+ (Catalina and later)

**Windows**:
- Plugins and installer are signed with **Authenticode** certificate
- Initial releases may show SmartScreen warnings until reputation builds

**Linux**:
- No code signing (standard for open-source Linux software)
- Verify package integrity with SHA-256 checksums

### Build from Source

If you prefer to build from source for security auditing:

```bash
git clone https://github.com/username/AudioForge.git
cd AudioForge
git verify-commit HEAD  # Verify commit signature (if GPG signed)
./build-all.sh          # Build all plugins
```

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed build instructions.

## Security Best Practices for Users

### General

1. **Keep plugins updated**: Always use the latest version
2. **Download from official sources**: GitHub releases or audioforge.fluxstudio.art
3. **Verify signatures**: Check code signatures before installation
4. **Use updated DAW**: Keep your DAW software up to date
5. **Scan downloads**: Run antivirus scans on downloaded files (optional but recommended)

### For Developers

If you're building from source or contributing:

1. **Review dependencies**: Audit JUCE version and third-party libraries
2. **Enable compiler warnings**: Use `-Wall -Wextra -Werror` flags
3. **Run static analysis**: Use cppcheck or clang-tidy
4. **Fuzz testing**: Test with randomized/malformed audio inputs
5. **Memory safety**: Use sanitizers (AddressSanitizer, UndefinedBehaviorSanitizer)

## Scope

### In Scope

The following are considered in-scope for security reports:

- **Code execution**: Arbitrary code execution via malformed audio or MIDI
- **Memory corruption**: Buffer overflows, use-after-free, etc.
- **Privilege escalation**: Installer or plugin gaining elevated permissions
- **Data exfiltration**: Plugin leaking sensitive data
- **Denial of service**: Crashes that can be triggered reliably
- **Supply chain**: Compromised dependencies or build artifacts

### Out of Scope

The following are **not** considered security vulnerabilities:

- **Audio artifacts**: Clicks, pops, or distortion (these are bugs, not security issues)
- **DAW crashes**: Crashes caused by DAW bugs, not our plugins
- **Social engineering**: Phishing or impersonation attacks
- **Brute force**: Attacks requiring unrealistic computational resources
- **Theoretical vulnerabilities**: Without proof of concept or real-world impact

## Security Tooling

We use the following security tools in our development process:

- **Static analysis**: cppcheck, clang-tidy
- **Dependency scanning**: GitHub Dependabot
- **Compiler sanitizers**: AddressSanitizer, UndefinedBehaviorSanitizer
- **Fuzz testing**: AFL++ (planned for future releases)

## Hall of Fame

We appreciate security researchers who responsibly disclose vulnerabilities. With your permission, we'll list your name here after a fix is released.

*(No reports yet - we're waiting for our first responsible disclosure!)*

## Contact

For security-related questions or concerns:

- **Email**: security@audioforge.art
- **GitHub**: [Security Advisories](https://github.com/username/AudioForge/security/advisories)

For general questions, use [GitHub Discussions](https://github.com/username/AudioForge/discussions).

---

Thank you for helping keep AudioForge and our users safe!
