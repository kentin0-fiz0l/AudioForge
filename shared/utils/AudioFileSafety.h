#pragma once

#include <juce_audio_formats/juce_audio_formats.h>
#include <filesystem>
#include <vector>

#if JUCE_MAC
 #include <sys/mount.h>
#elif JUCE_LINUX || JUCE_BSD
 #include <sys/vfs.h>
#elif JUCE_WINDOWS
 // From <windows.h>, which isn't included here: it would clash with JUCE's names
 extern "C" __declspec(dllimport) unsigned int __stdcall GetDriveTypeW(const wchar_t*);
 extern "C" __declspec(dllimport) unsigned long __stdcall GetFileAttributesW(const wchar_t*);
 extern "C" __declspec(dllimport) unsigned long __stdcall GetLastError();
#endif

namespace AudioForge {
namespace FileSafety {

/**
 * Checks for files named by something the user didn't write themselves: a
 * saved project, an SFZ instrument, a preset from elsewhere.
 *
 * A file on another machine is refused unless it's in a library folder the
 * user approved (approvedRoots): on Windows, opening one sends the user's
 * credentials to that host, and anywhere it tells the host what was opened.
 * That covers \\host\share paths, mapped network drives, mounted shares and
 * automounts (/net). A file whose header claims an impossible sample rate or
 * channel count is refused before anything is allocated from those numbers.
 */

constexpr double minSampleRate = 8000.0;
constexpr double maxSampleRate = 384000.0;
constexpr int maxChannels = 8;

/** Whether a path names a file on another machine. */
inline bool isNetworkPath(const juce::String& path)
{
    const auto p = path.trimStart();
    return p.startsWith("\\\\") || p.startsWith("//") || p.startsWith("\\/") || p.startsWith("/\\")
        || p.contains("://");
}

/** `file`'s path as std::filesystem takes it (wide on Windows, so any name works). */
inline std::filesystem::path fsPath(const juce::File& file)
{
   #if JUCE_WINDOWS
    return std::filesystem::path(file.getFullPathName().toWideCharPointer());
   #else
    return std::filesystem::path(file.getFullPathName().toStdString());
   #endif
}

#if JUCE_MAC || JUCE_LINUX || JUCE_BSD
/** Whether `path` is an automount's folder or inside one, where looking would mount it. */
inline bool isAutomount(const juce::String& path)
{
   #if JUCE_MAC  // its volumes ignore case: /NET is /net
    for (const auto* folder : { "/net", "/Network", "/home", "/System/Volumes/Data/net",
                                "/System/Volumes/Data/Network", "/System/Volumes/Data/home" })
        if (path.equalsIgnoreCase(folder) || path.startsWithIgnoreCase(juce::String(folder) + "/"))
            return true;
   #else
    for (const auto* folder : { "/net", "/misc" })
        if (path == folder || path.startsWith(juce::String(folder) + "/"))
            return true;
   #endif
    return false;
}

/**
 * Whether the folder `at` (already reached without mounting anything) is an
 * automount, configured anywhere: looking up a name inside it would mount it.
 * When it can't tell, it says it is.
 */
inline bool isAutofs(const std::filesystem::path& at)
{
    struct statfs fs;
    if (statfs(at.c_str(), &fs) != 0)
        return true;
   #if JUCE_MAC
    return juce::String(fs.f_fstypename) == "autofs";
   #else
    return (unsigned long) fs.f_type == 0x0187;  // AUTOFS_SUPER_MAGIC
   #endif
}

/**
 * The deepest folder of `path` that exists, with its links followed one step
 * at a time (lstat, never stat), so that nothing inside an automount is ever
 * looked up. False if the path, or a link along it, leads into one, or the
 * links go round.
 */
inline bool resolveWithoutMounting(const juce::String& path, std::filesystem::path& deepest)
{
    std::vector<std::string> todo;  // components still to walk, last first
    const auto addComponents = [&todo](const std::filesystem::path& p)
    {
        std::vector<std::string> parts;
        for (const auto& c : p.relative_path())
            parts.push_back(c.string());
        todo.insert(todo.end(), parts.rbegin(), parts.rend());
    };
    addComponents(std::filesystem::path(path.toStdString()));
    std::filesystem::path at = "/";
    int links = 0;
    while (! todo.empty())
    {
        const auto part = todo.back();
        todo.pop_back();
        if (part.empty() || part == ".")
            continue;
        if (part == "..")
        {
            at = at.parent_path();
            continue;
        }
        const auto next = at / part;
        if (isAutomount(juce::String(next.string())) || isAutofs(at))
            return false;
        std::error_code ec;
        const auto status = std::filesystem::symlink_status(next, ec);
        if (ec || ! std::filesystem::exists(status))
            break;  // what's left doesn't exist yet: `at` is as deep as it goes
        if (std::filesystem::is_symlink(status))
        {
            const auto target = std::filesystem::read_symlink(next, ec);
            if (ec || ++links > 40)
                return false;
            if (target.is_absolute())
                at = "/";
            addComponents(target);
            continue;
        }
        at = next;
    }
    deepest = at;
    return true;
}
#endif

#if JUCE_LINUX || JUCE_BSD
/** Whether a file system (statfs's f_type) is one on this machine's own disks or memory. */
inline bool isLocalFileSystem(unsigned long type)
{
    switch (type)
    {
        case 0xEF53:      // ext2, ext3, ext4
        case 0x58465342:  // XFS
        case 0x9123683E:  // Btrfs
        case 0x2FC12FC1:  // ZFS
        case 0xF2F52010:  // F2FS
        case 0xCA451A4E:  // bcachefs
        case 0x52654973:  // ReiserFS
        case 0x3153464A:  // JFS
        case 0x3434:      // NILFS
        case 0x01021994:  // tmpfs
        case 0x858458F6:  // ramfs
        case 0x794C7630:  // overlayfs (containers)
        case 0x73717368:  // SquashFS
        case 0x4D44:      // FAT
        case 0x2011BAB0:  // exFAT
        case 0x5346544E:  // NTFS
        case 0x7366746E:  // NTFS (ntfs3)
        case 0x482B:      // HFS+
        case 0x9660:      // ISO 9660
        case 0x15013346:  // UDF
        case 0xF15F:      // eCryptfs
            return true;
        default:
            return false;  // NFS, SMB, FUSE (sshfs), Ceph and the rest: another machine, or can't tell
    }
}
#endif

/**
 * Whether `file` is on another machine, or might be: a network path, or on a
 * volume that isn't one of this machine's own. Links along the path are
 * followed to where they lead, and an automount (/net) is refused by name,
 * never looked inside, since looking would mount it. When it can't tell, it
 * says remote: a library there loads once its folder is approved.
 */
inline bool isRemote(const juce::File& file)
{
    const auto path = file.getFullPathName();
    if (isNetworkPath(path))
        return true;

   #if JUCE_MAC || JUCE_LINUX || JUCE_BSD
    std::filesystem::path deepest;
    if (! resolveWithoutMounting(path, deepest))
        return true;
    struct statfs fs;
    if (statfs(deepest.c_str(), &fs) != 0)
        return true;
    #if JUCE_MAC
    return (fs.f_flags & MNT_LOCAL) == 0;
    #else
    return ! isLocalFileSystem((unsigned long) fs.f_type);
    #endif
   #elif JUCE_WINDOWS
    // A drive letter, mapped to a share or not; then no folder along the way
    // may be a link (a junction or symlink can lead to a share). A link's own
    // attributes are read without following it.
    if (path.length() < 3 || path[1] != ':' || path[2] != '\\')
        return true;
    switch (GetDriveTypeW(path.substring(0, 3).toWideCharPointer()))
    {
        case 2: case 3: case 5: case 6:  // removable, fixed, CD, RAM disk: this machine's
            break;
        default:  // remote (a mapped share), or no such drive
            return true;
    }
    for (int i = path.indexOfChar(3, '\\'); ; i = path.indexOfChar(i + 1, '\\'))
    {
        const auto prefix = i < 0 ? path : path.substring(0, i);
        const auto attributes = GetFileAttributesW(prefix.toWideCharPointer());
        if (attributes == 0xFFFFFFFFul)
        {
            const auto error = GetLastError();
            return error != 2 && error != 3;  // only "not found" means what's left doesn't exist yet
        }
        if ((attributes & 0x400ul) != 0)  // FILE_ATTRIBUTE_REPARSE_POINT
            return true;
        if (i < 0)
            return false;
    }
   #else
    return true;
   #endif
}

/**
 * Where the user's approved library folders are listed, one per line. Tests
 * point it elsewhere.
 */
inline juce::File& rootsFile()
{
    static juce::File file = []
    {
        auto settings = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
       #if JUCE_MAC
        settings = settings.getChildFile("Application Support");  // JUCE gives ~/Library itself
       #endif
        return settings.getChildFile("AudioForge").getChildFile("library-roots.txt");
    }();
    return file;
}

/**
 * The library folders the user approved: those in rootsFile, and those in
 * AUDIOFORGE_LIBRARY_ROOTS (separated as PATH is, for the render host).
 */
inline juce::StringArray approvedRoots()
{
    juce::StringArray roots;
    if (rootsFile().existsAsFile())
        roots.addLines(rootsFile().loadFileAsString());
   #if JUCE_WINDOWS
    roots.addTokens(juce::SystemStats::getEnvironmentVariable("AUDIOFORGE_LIBRARY_ROOTS", {}), ";", {});
   #else
    roots.addTokens(juce::SystemStats::getEnvironmentVariable("AUDIOFORGE_LIBRARY_ROOTS", {}), ":", {});
   #endif
    roots.trim();
    roots.removeEmptyStrings();
    return roots;
}

/** Whether `file` is in an approved library folder (only its name is read). */
inline bool isApproved(const juce::File& file)
{
    const auto path = file.getFullPathName();
    for (const auto& root : approvedRoots())
    {
        if (! juce::File::isAbsolutePath(root))
            continue;
        const auto folder = juce::File(root).getFullPathName();
       #if JUCE_WINDOWS || JUCE_MAC
        const bool inside = path.equalsIgnoreCase(folder) || path.startsWithIgnoreCase(folder + juce::File::getSeparatorString());
       #else
        const bool inside = path == folder || path.startsWith(folder + juce::File::getSeparatorString());
       #endif
        if (inside && ! path.contains(juce::String("..") + juce::File::getSeparatorString()))
            return true;
    }
    return false;
}

/** Adds `folder` to the approved library folders (the user chose a file in it). */
inline void approve(const juce::File& folder)
{
    if (isApproved(folder))
        return;
    rootsFile().getParentDirectory().createDirectory();
    rootsFile().appendText(folder.getFullPathName() + "\n");
}

/** Why `file` isn't loaded from where it is, or an empty string if it may be. */
inline juce::String refusal(const juce::File& file)
{
    const auto path = file.getFullPathName();
    if (path.contains("://"))
        return "URLs aren't loaded: " + path;
    if (isApproved(file) || ! isRemote(file))
        return {};
    return "network files load only from an approved library folder (choose one by hand, or list it in "
           + rootsFile().getFullPathName() + "): " + path;
}

/**
 * The same for a path as a saved state or an SFZ wrote it (`named`), once
 * resolved: a network name is refused as written, before resolving can make
 * it look local.
 */
inline juce::String refusal(const juce::String& named, const juce::File& resolved)
{
    if (isNetworkPath(named) && ! isApproved(resolved))
        return "network paths aren't loaded: " + named;
    return refusal(resolved);
}

/** Why a reader's header can't be trusted, or an empty string if it can. */
inline juce::String checkReader(const juce::AudioFormatReader& reader)
{
    if (! (reader.sampleRate >= minSampleRate && reader.sampleRate <= maxSampleRate))
        return "unsupported sample rate " + juce::String(reader.sampleRate) + " Hz";
    if (reader.numChannels < 1 || reader.numChannels > (unsigned int) maxChannels)
        return "unsupported channel count " + juce::String(reader.numChannels);
    if (reader.lengthInSamples <= 0)
        return "no audio";
    return {};
}

constexpr juce::int64 maxTextBytes = 16 * 1024 * 1024;

/**
 * Reads a text file (an SFZ instrument) into `text`, or says in `error` why
 * not. Only a regular file is read (not a device or a pipe), and no more than
 * maxBytes of it: anything longer is refused rather than cut short.
 */
inline bool readText(const juce::File& file, juce::String& text, juce::String& error,
                     juce::int64 maxBytes = maxTextBytes)
{
    std::error_code ec;
    if (! std::filesystem::is_regular_file(fsPath(file), ec))
    {
        error = "not a regular file: " + file.getFullPathName();
        return false;
    }
    juce::FileInputStream in(file);
    if (! in.openedOk())
    {
        error = "can't open " + file.getFullPathName();
        return false;
    }
    juce::MemoryBlock data;
    in.readIntoMemoryBlock(data, (juce::pointer_sized_int) (maxBytes + 1));
    if ((juce::int64) data.getSize() > maxBytes)
    {
        error = "larger than " + juce::String(maxBytes / (1024 * 1024)) + " MB: " + file.getFullPathName();
        return false;
    }
    text = juce::String::createStringFromData(data.getData(), (int) data.getSize());
    return true;
}

} // namespace FileSafety
} // namespace AudioForge
