/*
 * To build, set up your Release configuration like this:
 *
 * [Runtime Library]
 * Multi-threaded (/MT)
 *
 * Visit www.frida.re to learn more about Frida.
 */

#include <frida/frida-gum.h>

#include <RE/ConsoleLog.h>
#include <RE/TESObjectREFR.h>
#include <sstream>
#include <windows.h>

#include <RE/Effect.h>
#include <RE/EffectSetting.h>
#include <RE/IngredientItem.h>

typedef struct _ExampleListener ExampleListener;
typedef enum _ExampleHookId ExampleHookId;

struct _ExampleListener
{
  GObject parent;
};

enum _ExampleHookId
{
  HOOK_SEND_ANIMATION_EVENT,
  DRAW_SHEATHE_WEAPON_ACTOR,
  DRAW_SHEATHE_WEAPON_PC,
  CALCULATION_MAGNITUDE_ALCHEMY_CRAFT
};

static void example_listener_iface_init(gpointer g_iface, gpointer iface_data);

#define EXAMPLE_TYPE_LISTENER (example_listener_get_type())
G_DECLARE_FINAL_TYPE(ExampleListener, example_listener, EXAMPLE, LISTENER,
                     GObject)
G_DEFINE_TYPE_EXTENDED(ExampleListener, example_listener, G_TYPE_OBJECT, 0,
                       G_IMPLEMENT_INTERFACE(GUM_TYPE_INVOCATION_LISTENER,
                                             example_listener_iface_init))

void SetupFridaHooks()
{
  GumInterceptor* interceptor;
  GumInvocationListener* listener;

  gum_init_embedded();

  interceptor = gum_interceptor_obtain();
  listener = (GumInvocationListener*)g_object_new(EXAMPLE_TYPE_LISTENER, NULL);

  int r;

  gum_interceptor_begin_transaction(interceptor);

  r = gum_interceptor_attach(
    interceptor, (void*)(REL::Module::BaseAddr() + 0x868EC0), listener,
    GSIZE_TO_POINTER(CALCULATION_MAGNITUDE_ALCHEMY_CRAFT));

  if (GUM_ATTACH_OK != r) {
    char buf[1025];
    sprintf_s(buf, "Interceptor failed with %d", int(r));
    MessageBox(0, buf, "Error", MB_ICONERROR);
  }

  gum_interceptor_end_transaction(interceptor);
}

static void example_listener_on_enter(GumInvocationListener* listener,
                                      GumInvocationContext* ic)
{
  ExampleListener* self = EXAMPLE_LISTENER(listener);
  auto hook_id = gum_invocation_context_get_listener_function_data(ic);

  auto _ic = (_GumInvocationContext*)ic;

  switch ((size_t)hook_id) {
    case CALCULATION_MAGNITUDE_ALCHEMY_CRAFT: {

      auto numIngridientPtr = _ic->cpu_context->r13
        ? (uint8_t*)(_ic->cpu_context->r13 + 0x130)
        : nullptr;
      const uint8_t numIngridient = numIngridientPtr ? *numIngridientPtr : 0;

      std::vector<int32_t> arraySelectedIndex;
      arraySelectedIndex.resize(numIngridient);

      for (size_t i = 0; i < numIngridient; ++i) {

        auto indexSelectedIngredient = _ic->cpu_context->r13
          ? (uint32_t*)(_ic->cpu_context->r13 + 0x120 + (i * 4))
          : nullptr;

        arraySelectedIndex[i] =
          indexSelectedIngredient ? *indexSelectedIngredient : 0;
      }

      std::vector<RE::IngredientItem*> arraySelectedIngredient;
      arraySelectedIngredient.resize(numIngridient);

      uint64_t* rcx =
        reinterpret_cast<uint64_t*>(_ic->cpu_context->r13 + 0x100);

      for (size_t i = 0; i < numIngridient; ++i) {

        auto IngredientItemPtr = _ic->cpu_context->r13
          ? (RE::IngredientItem**)((*rcx) + (arraySelectedIndex[i] * 2) * 8)
          : nullptr;

        RE::IngredientItem** ingredientItem =
          reinterpret_cast<RE::IngredientItem**>(*IngredientItemPtr);

        arraySelectedIngredient[i] = *ingredientItem;
      }

      float& magnitude = *reinterpret_cast<float*>(_ic->cpu_context->rdx);

      uint32_t& duration =
        *reinterpret_cast<uint32_t*>(_ic->cpu_context->rdx + 8);

      RE::EffectSetting** effectSetting =
        reinterpret_cast<RE::EffectSetting**>(_ic->cpu_context->rdx + 0x10);

      float newMagnitude = 0.f;
      uint32_t newDuration = 0;
      uint8_t numOKIngredients = 0;

      for (auto ingredient : arraySelectedIngredient) {
        for (auto effect : ingredient->effects) {
          if ((*effectSetting)->formID == effect->baseEffect->formID) {
            numOKIngredients++;
            newMagnitude += effect->effectItem.magnitude;
            newDuration += effect->effectItem.duration;
          }
        }
        RE::ConsoleLog::GetSingleton()->Print("ingredient: %s",
                                              ingredient->fullName.c_str());
         RE::ConsoleLog::GetSingleton()->Print("knownEffectFlags: %p",
                                              &ingredient->gamedata.knownEffectFlags);

      }

      if (numOKIngredients > 0) {
        magnitude = float(newMagnitude / numOKIngredients);
        duration = float(newDuration / numOKIngredients);
      }

      RE::ConsoleLog::GetSingleton()->Print(
      "LearnEffect func adrres: %p", (REL::Module::BaseAddr() + 9948560));
    } break;
  }
}

static void example_listener_on_leave(GumInvocationListener* listener,
                                      GumInvocationContext* ic)
{
  ExampleListener* self = EXAMPLE_LISTENER(listener);
  auto hook_id = gum_invocation_context_get_listener_function_data(ic);

  switch ((size_t)hook_id) {

  }
}

static void example_listener_class_init(ExampleListenerClass* klass)
{
  (void)EXAMPLE_IS_LISTENER;
#ifndef _MSC_VER
  (void)glib_autoptr_cleanup_ExampleListener;
#endif
}

static void example_listener_iface_init(gpointer g_iface, gpointer iface_data)
{
  auto iface = (GumInvocationListenerInterface*)g_iface;

  iface->on_enter = example_listener_on_enter;
  iface->on_leave = example_listener_on_leave;
}

static void example_listener_init(ExampleListener* self)
{
}
