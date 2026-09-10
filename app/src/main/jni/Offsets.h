#pragma once

#include "Includes.h"

bool isLibDumped = false, isFastDump = false, isRebuiltLibDump = false, isStringsDump = false, isStringDumped = false, isObjectsDump = false, isObjectsDumped = false, isSDKUDump = false, isSDKUDumped = false, isSDKWDump = false, isSDKWDumped = false, isActorsDump = false, isActorsDumped = false, isBoneDump = false, isBoneDumped = false;

namespace Offsets {
    uintptr_t PointerSize = sizeof(void*); // 8-bytes on ARM64

    bool isUE423 = true, isDereferencing = false, isXorDecrypt = false;

    // EngineOffsets (Fortnite v21.30 ARM64)
    uintptr_t GWorld = 0xDE34970;        // GEngine / WorldContext Entry
    uintptr_t GNames = 0xDC960C0;        // FNamePool Base
    uintptr_t GUObjectArray = 0xDCD6E08; // FUObjectArray

    // TNameEntry
    uintptr_t TNameEntryToNameString = 0x10;

    // FNamePool
    uintptr_t GNamesToFNamePool = 0x0;
    uintptr_t FNamePoolToBlocks = 0x10;
    uintptr_t FNameStride = 0x2;         // UE5 Alignment Stride
    uintptr_t FNamePoolToCurrentBlock = 0x8;
    uintptr_t FNamePoolToCurrentByteCursor = 0xC;

    // FNameEntry
    uintptr_t FNameEntryHeader = 0x0;
    uintptr_t FNameEntryToLenBit = 0x6;
    uintptr_t FNameEntryToString = 0x2;

    // FUObjectArray
    uintptr_t FUObjectArrayToTUObjectArray = 0x10; 
    uintptr_t FUObjectItemPadd = 0x0;
    uintptr_t FUObjectItemSize = 0x18;  // [Object* | Flags | ClusterIndex | SerialNumber]

    // TUObjectArray
    uintptr_t TUObjectArrayToNumElements = 0x14;

    // UObject
    uintptr_t UObjectToInternalIndex = 0xC;
    uintptr_t UObjectToClassPrivate = 0x10;
    uintptr_t UObjectToFNameIndex = 0x18; // UObjectToClassPrivate + PointerSize
    uintptr_t UObjectToOuterPrivate = 0x20; // UObjectToFNameIndex + PointerSize

    // FField (UE5 Base Field)
    uintptr_t FFieldToClass = 0x8;
    uintptr_t FFieldToNext = 0x18;        // FFieldToClass + 0x10
    uintptr_t FFieldToName = 0x20;        // FFieldToNext + PointerSize

    // UField
    uintptr_t UFieldToNext = 0x28;

    // UStruct / UClass (UE5 Properties Root)
    uintptr_t UStructToSuperStruct = 0x40;
    uintptr_t UStructToChildren = 0x48;   // UStructToSuperStruct + PointerSize
    uintptr_t UStructToChildProperties = 0x50; // Pointer to FProperty Chain

    // UFunction
    uintptr_t UFunctionToFunctionFlags = 0xB0;
    uintptr_t UFunctionToFunc = 0xD8;     // UFunctionToFunctionFlags + UFieldToNext (0xB0 + 0x28)

    // FProperty (UE5 Base Property Offsets)
    uintptr_t UPropertyToElementSize = 0x3C;
    uintptr_t UPropertyToPropertyFlags = 0x40;
    uintptr_t UPropertyToOffsetInternal = 0x4C;

    // FBoolProperty
    uintptr_t UBoolPropertyToFieldSize = 0x78;
    uintptr_t UBoolPropertyToByteOffset = 0x79; // 0x78 + 0x1
    uintptr_t UBoolPropertyToByteMask = 0x7A;   // 0x79 + 0x1
    uintptr_t UBoolPropertyToFieldMask = 0x7B;  // 0x7A + 0x1

    // FObjectProperty
    uintptr_t UObjectPropertyToPropertyClass = 0x78;

    // FMapProperty
    uintptr_t UMapPropertyToKeyProp = 0x80;
    uintptr_t UMapPropertyToValueProp = 0x88;   // UMapPropertyToKeyProp + PointerSize

    // FSetProperty
    uintptr_t USetPropertyToElementProp = 0x80;

    // FStructProperty
    uintptr_t UStructPropertyToStruct = 0x80;

    // FArrayProperty
    uintptr_t UArrayPropertyToInnerProperty = 0x80;

    // FClassProperty
    uintptr_t UClassPropertyToMetaClass = 0x80;

    // FInterfaceProperty
    uintptr_t UInterfacePropertyToUInterface = 0x80;

    // UWorld
    uintptr_t UWorldToPersistentLevel = 0x30;

    // ULevel
    uintptr_t ULevelToAActors = 0x98;
    uintptr_t ULevelToAActorsCount = 0xA0; // ULevelToAActors + PointerSize

    void Fortnitev2130() {
        Memory.GameName = "Fortnite v21.30";
        Memory.TargetProcess = "libUnreal.so";

        isUE423 = true; // Maintains 64-bit alignment logic for UE5 structure readers

        GWorld = 0xDE34970;
        GNames = 0xDC960C0;
        GUObjectArray = 0xDCD6E08;
    }
};
