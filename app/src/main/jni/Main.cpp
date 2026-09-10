#include "Main.h"
#include <chrono>

static void AutoDumpThread() {
    std::this_thread::sleep_for(std::chrono::seconds(60));

    Menu.DumpLocation = "/storage/emulated/0/Android/data/com.epicgames.fortnite/files/SDK";

    isFastDump = false;
    isRebuiltLibDump = false;
    isStringsDump = true;
    isObjectsDump = true;
    isSDKUDump = true;
    isSDKWDump = true;
    isActorsDump = true;
    isBoneDump = true;

    DumpLib(Menu.DumpLocation);
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    Menu.MenuName = "Android UE Dumper";
    Menu.Credits = "Made by Ascarre";
    Menu.isRunning = true;

    Offsets::Fortnitev2130();

    std::thread(DumperThread).detach();
    std::thread(AutoDumpThread).detach();

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM* vm, void* reserved) {
    Menu.isRunning = false;
}
