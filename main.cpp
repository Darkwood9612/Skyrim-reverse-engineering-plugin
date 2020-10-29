#include "skse64/PluginAPI.h"
#include "skse64_common/skse_version.h"

// CommonLibSSE
#include <SKSE/API.h>
#include <SKSE/Interfaces.h>
#include <SKSE/Stubs.h>

#include "skse64/GameRTTI.h"

#include "HookDamage.h"

#include <shlobj.h>

#define PLUGIN_NAME "ReverseHook"
#define PLUGIN_VERSION 0
static uint32_t KillMove = 1 << 14;

void SetupFridaHooks();

UInt32 g_skseVersion;
PluginHandle g_pluginHandle = kPluginHandle_Invalid;
SKSEMessagingInterface* g_messaging;

typedef float (*FnCalcDamage)(Actor*, float, float);
RelocAddr<FnCalcDamage> fnCalcDamage(0x005E4800);

// No damage has been done yet when these event handlers are called.
// After all the processing of the event handlers is completed, damage is done
// after CalcDamage is called by Skyrim.exe.

// Multiplier correction by difficulty
float CalcDamage(Actor* actor, float damage, UInt8 arg3)
{
  float a3 = (float)arg3;
  return -(fnCalcDamage(actor, -damage, a3));
}

void FallDamageEventHandler(Actor* target, float* damage,
                            TESObjectREFR* attacker, UInt32 arg3)
{
  if (!target->magicTarget.Unk_04() &&
      (target->flags2 & KillMove) == 0) {
    float dmg = CalcDamage(target, *damage, arg3);
    if (dmg > 0.0f) {
      //
    }
  }
}

void ReflectDamageEventHandler(Actor* target, float* damage,
                               TESObjectREFR* attacker, UInt32 arg3)
{
  if (!target->magicTarget.Unk_04() &&
      (target->flags2 & KillMove) == 0) {
    float dmg = CalcDamage(target, *damage, arg3);
    if (dmg > 0.0f) {
      //
    }
  }
}

void WaterDamageEventHandler(Actor* target, float* damage,
                             TESObjectREFR* attacker, UInt32 arg3)
{
  if (!target->magicTarget.Unk_04() &&
      (target->flags2 & KillMove) == 0) {
    float dmg = CalcDamage(target, *damage, arg3);
    if (dmg > 0.0f) {
      //
    }
  }
}

void MagicDamageEventHandler(Actor* target, float* damage,
                             TESObjectREFR* attacker, UInt32 arg3,
                             ActiveEffect* activeEffect, bool* crit)
{
  if (!target->magicTarget.Unk_04() &&
      (target->flags2 & KillMove) == 0) {
    float dmg = CalcDamage(target, *damage, arg3);
    if (dmg > 0.0f) {
      //
    }
  }
}

// Health ID is 24
void ActorValueEventHandler(Actor* target, UInt32 arg1, UInt32 id,
                            float* damage, TESObjectREFR* attacker)
{
  float value = -*damage;

  //
}

// Health ID is 24
void MagicHealEventHandler(Actor* target, UInt32 arg1, UInt32 id, float* heal,
                           TESObjectREFR* attacker, ActiveEffect* activeEffect)
{
  if (*heal > 0.0f) {
    TESObjectREFR* caster = nullptr;
    UInt32 handle = activeEffect->unk34;
    if (handle != *g_invalidRefHandle) {
      NiPointer<TESObjectREFR> refPtr;
      if (LookupREFRByHandle(handle, refPtr))
        caster = refPtr.get();
    }

    if (activeEffect->effect->mgef->properties.archetype ==
        EffectSetting::Properties::kArchetype_Absorb) {
      /*if (activeEffect->magicTarget)
        caster = activeEffect->magicTarget->Unk_02();*/
    }

    //
  }
}

/*
hit position
*(NiPoint3*)((char*)r15 + 0x0)
attacker handle
*(UInt32*)((char*)r15 + 0x18)
weapon
*(TESObjectWEAP**)((char*)r15 + 0x38)
power attack
*(bool*)((char*)r15 + 0x82)
critical
(*(UInt32*)((char*)r15 + 0x80) & 8) != 0
sneak
(*(UInt32*)((char*)r15 + 0x80) & 0x800) != 0
bash
(*(UInt32*)((char*)r15 + 0x80) & 0xC000) != 0
block
(*(UInt16*)((char*)r15 + 0x80) & 3) != 0
*/
void PhysicalDamageEventHandler(Actor* target, float* damage,
                                TESObjectREFR* attacker, UInt32 arg3,
                                UInt64 r15)
{
  if (!target->magicTarget.Unk_04() &&
      (target->flags2 & KillMove) == 0) {
    float dmg = CalcDamage(target, *damage, arg3);
    if (dmg > 0.0f) {
      std::string actorName = target->GetName();
      MessageBoxA(NULL, (*(TESObjectWEAP**)((char*)r15 + 0x38))->GetName(),
                  actorName.c_str(),
                  MB_OK);
    }
  }
}

void Init(HookDamage::RegisterEvent_t registerFunc)
{
  using HookDamage::Hook;

  registerFunc(Hook::kFallDamage, FallDamageEventHandler, 100);
  registerFunc(Hook::kReflectDamage, ReflectDamageEventHandler, 100);
  registerFunc(Hook::kWaterDamage, WaterDamageEventHandler, 100);
  registerFunc(Hook::kMagicDamage, MagicDamageEventHandler, 100);
  registerFunc(Hook::kActorValue, ActorValueEventHandler, 100);
  registerFunc(Hook::kMagicHeal, MagicHealEventHandler, 100);
  registerFunc(Hook::kPhysicalDamage, PhysicalDamageEventHandler, 100);
}

void HookDamageMessageHandler(SKSEMessagingInterface::Message* msg)
{
  if (msg->type == HookDamage::kType_Register) {
    if (msg->dataLen == HookDamage::kHookDamageVersion) {
      Init(static_cast<HookDamage::RegisterEvent_t>(msg->data));
    } else {
      _MESSAGE("Incompatible HookDamage version : required %d : found %d",
               HookDamage::kHookDamageVersion, msg->dataLen);
    }
  }
}

void SKSEMessageHandler(SKSEMessagingInterface::Message* msg)
{
  if (msg->type == SKSEMessagingInterface::kMessage_PostPostLoad) {
    if (!g_messaging->RegisterListener(g_pluginHandle, "HookDamage",
                                       HookDamageMessageHandler))
      _MESSAGE("HookDamage not loaded");
  }
}

extern "C" {
__declspec(dllexport) bool SKSEPlugin_Query(const SKSE::QueryInterface* skse,
                                            SKSE::PluginInfo* info)
{
  info->infoVersion = SKSE::PluginInfo::kVersion;
  info->name = PLUGIN_NAME;
  info->version = PLUGIN_VERSION;

  if (skse->IsEditor()) {
    _FATALERROR("loaded in editor, marking as incompatible");
    return false;
  }

  g_messaging = (SKSEMessagingInterface*)(reinterpret_cast<const SKSEInterface*>(skse))
                  ->QueryInterface(kInterface_Messaging);

  if (!g_messaging) {
    _MESSAGE("couldn't get messaging interface");
    return false;
  }
  if (g_messaging->interfaceVersion < 1) {
    _MESSAGE("messaging interface too old (%d expected %d)",
             g_messaging->interfaceVersion, 1);
    return false;
  }

  return true;
}

__declspec(dllexport) bool SKSEPlugin_Load(const SKSEInterface* skse)
{
  SetupFridaHooks();

  g_messaging->RegisterListener(g_pluginHandle, "SKSE", SKSEMessageHandler);

  return true;
}
};
