#include "Main.h"
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "AutoDump", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "AutoDump", __VA_ARGS__)

static std::atomic<bool> g_dumpRunning{false};
static std::mutex g_dumpMutex;

static uintptr_t GetModuleBase(const char* moduleName) {
    std::ifstream maps("/proc/self/maps");
    if (!maps.is_open()) return 0;
    std::string line;
    std::string target = moduleName;
    while (std::getline(maps, line)) {
        if (line.find(target) == std::string::npos) continue;
        size_t dash = line.find('-');
        if (dash == std::string::npos) continue;
        uintptr_t addr = strtoull(line.substr(0, dash).c_str(), nullptr, 16);
        if (addr != 0) return addr;
    }
    return 0;
}

static bool IsMapped(uintptr_t addr, size_t size) {
    std::ifstream maps("/proc/self/maps");
    if (!maps.is_open()) return false;
    std::string line;
    while (std::getline(maps, line)) {
        uintptr_t start = 0, end = 0;
        if (sscanf(line.c_str(), "%lx-%lx", &start, &end) != 2) continue;
        if (addr >= start && (addr + size) <= end) return true;
    }
    return false;
}

static uintptr_t ResolveGameBase() {
    uintptr_t base = GetModuleBase("libUnreal.so");
    if (base != 0) {
        LOGI("libUnreal.so base = 0x%lx", base);
        return base;
    }
    base = GetModuleBase("libUE4.so");
    if (base != 0) {
        LOGI("libUE4.so base = 0x%lx", base);
        return base;
    }
    return 0;
}

static bool VerifyChain(uintptr_t base, uintptr_t offset, const char* name) {
    if (base == 0) return false;

    uintptr_t addr = base + offset;
    LOGI("%s: addr = 0x%lx", name, addr);

    if (!IsMapped(addr, 8)) {
        LOGE("%s: addr not mapped", name);
        return false;
    }

    uintptr_t value = *(uintptr_t*)addr;
    LOGI("%s: value = 0x%lx", name, value);

    if (value == 0) {
        LOGE("%s: value is null", name);
        return false;
    }

    if (!IsMapped(value, 8)) {
        LOGE("%s: value not mapped", name);
        return false;
    }

    return true;
}

static bool EnsureDirectory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) return true;
    std::string cur;
    for (size_t i = 0; i < path.size(); i++) {
        cur += path[i];
        if (path[i] == '/' && cur.size() > 1) mkdir(cur.c_str(), 0777);
    }
    mkdir(path.c_str(), 0777);
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

static void AutoDumpThread() {
    LOGI("AutoDump thread started");
    std::this_thread::sleep_for(std::chrono::seconds(60));

    if (g_dumpRunning.exchange(true)) {
        LOGI("Dump already running");
        return;
    }

    uintptr_t base = 0;
    int waited = 0;
    while (base == 0 && waited < 180) {
        base = ResolveGameBase();
        if (base == 0) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            waited += 3;
        }
    }

    if (base == 0) {
        LOGE("Game library not found, abort");
        g_dumpRunning = false;
        return;
    }

    Offsets::Fortnitev2130();

    LOGI("GNames offset = 0x%lx", (uintptr_t)Offsets::GNames);
    LOGI("GUObjectArray offset = 0x%lx", (uintptr_t)Offsets::GUObjectArray);
    LOGI("GWorld offset = 0x%lx", (uintptr_t)Offsets::GWorld);

    if (!VerifyChain(base, Offsets::GNames, "GNames")) {
        LOGE("GNames verification failed");
        g_dumpRunning = false;
        return;
    }

    if (!VerifyChain(base, Offsets::GUObjectArray, "GUObjectArray")) {
        LOGE("GUObjectArray verification failed");
        g_dumpRunning = false;
        return;
    }

    std::string dumpPath = "/storage/emulated/0/Android/data/com.epicgames.fortnite/files/SDK";

    if (!EnsureDirectory(dumpPath)) {
        LOGE("Cannot create dump directory");
        g_dumpRunning = false;
        return;
    }

    Menu.DumpLocation = dumpPath.c_str();

    isFastDump = false;
    isRebuiltLibDump = false;
    isStringsDump = true;
    isObjectsDump = true;
    isSDKUDump = true;
    isSDKWDump = true;
    isActorsDump = true;
    isBoneDump = true;

    LOGI("Starting DumpLib");

    {
        std::lock_guard<std::mutex> lock(g_dumpMutex);
        DumpLib(Menu.DumpLocation);
    }

    LOGI("DumpLib finished");
    g_dumpRunning = false;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    Menu.MenuName = "Android UE Dumper";
    Menu.Credits = "Made by Ascarre";
    Menu.isRunning = true;

    std::thread(DumperThread).detach();
    std::thread(AutoDumpThread).detach();

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM* vm, void* reserved) {
    Menu.isRunning = false;
}
