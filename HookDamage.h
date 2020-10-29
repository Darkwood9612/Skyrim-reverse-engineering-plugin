#pragma once

#include "skse64/GameReferences.h"

#include <map>

namespace HookDamage
{
	enum { kHookDamageVersion = 1 };

	enum class Hook : UInt32
	{
		kFallDamage = 0,
		kReflectDamage = 1,
		kWaterDamage = 2,
		kMagicDamage = 3,
		kActorValue = 4,
		kMagicHeal = 5,
		kEvadeDamage = 6, //No need to use
		kPhysicalDamage = 7
	};

	enum Type : UInt32
	{
		kType_Register = 0
	};

	typedef void(*FallDamageEventHandler_t)(Actor* target, float* damage, TESObjectREFR* attacker, UInt32 arg3);
	extern std::map<UInt32, FallDamageEventHandler_t> fallDamageEventHandlers;

	typedef void(*ReflectDamageEventHandler_t)(Actor* target, float* damage, TESObjectREFR* attacker, UInt32 arg3);
	extern std::map<UInt32, ReflectDamageEventHandler_t> reflectDamageEventHandlers;

	typedef void(*WaterDamageEventHandler_t)(Actor* target, float* damage, TESObjectREFR* attacker, UInt32 arg3);
	extern std::map<UInt32, WaterDamageEventHandler_t> waterDamageEventHandlers;

	typedef void(*MagicDamageEventHandler_t)(Actor* target, float* damage, TESObjectREFR* attacker, UInt32 arg3, ActiveEffect* activeEffect, bool* crit);
	extern std::map<UInt32, MagicDamageEventHandler_t> magicDamageEventHandlers;

	typedef void(*ActorValueEventHandler_t)(Actor* target, UInt32 arg1, UInt32 id, float* damage, TESObjectREFR* attacker);
	extern std::map<UInt32, ActorValueEventHandler_t> actorValueEventHandlers;

	typedef void(*MagicHealEventHandler_t)(Actor* target, UInt32 arg1, UInt32 id, float* heal, TESObjectREFR* attacker, ActiveEffect* activeEffect);
	extern std::map<UInt32, MagicHealEventHandler_t> magicHealEventHandlers;

	typedef void(*EvadeDamageEventHandler_t)(Actor* target, UInt64 r15, bool* evade);
	extern std::map<UInt32, EvadeDamageEventHandler_t> evadeDamageEventHandlers;

	typedef void(*PhysicalDamageEventHandler_t)(Actor* target, float* damage, TESObjectREFR* attacker, UInt32 arg3, UInt64 r15);
	extern std::map<UInt32, PhysicalDamageEventHandler_t> physicalDamageEventHandlers;

	//Set the priority to a number between 100 and 1000.
	//You cannot add multiple handlers with the same priority to the same event.
	typedef bool(*RegisterEvent_t)(Hook hookType, void* func, UInt32 priority);
	bool RegisterEvent(Hook hookType, void* func, UInt32 priority);
}