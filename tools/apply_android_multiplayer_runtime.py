#!/usr/bin/env python3
"""Apply GTA IV Universal-owned Android multiplayer UI/status integration.

This ports the reviewed Android-facing changes from the experimental runtime
PR into the Universal build pipeline. The upstream Android runtime stays pinned
to the audited baseline; no runtime-repository PR or moving branch is required.
"""
from pathlib import Path
import subprocess
import sys

PIN = "be597d42d12abb04a552fd87e3853028499ae835"
if len(sys.argv) != 2:
    raise SystemExit("usage: apply_android_multiplayer_runtime.py <runtime/xhyn-android>")
root = Path(sys.argv[1]).resolve()
actual = subprocess.check_output(
    ["git", "-C", str(root), "rev-parse", "HEAD"], text=True
).strip()
if actual != PIN:
    raise SystemExit(f"Refusing unexpected Android runtime: {actual} (expected {PIN})")


def replace_once(path: str, old: str, new: str) -> None:
    file = root / path
    source = file.read_text(encoding="utf-8")
    if new in source:
        return
    count = source.count(old)
    if count != 1:
        raise SystemExit(f"Expected exactly one patch anchor in {path}; found {count}")
    file.write_text(source.replace(old, new, 1), encoding="utf-8")


# Native read-only JNI bridge to the real XAM live-compatibility runtime.
replace_once(
    "glue/rexglue-sdk-main/gta4-recomp/src/android/android_platform_stubs.cpp",
    "#include <cstdlib>\n#include <jni.h>",
    "#include <cstdlib>\n#include <iomanip>\n#include <sstream>\n#include <jni.h>",
)
replace_once(
    "glue/rexglue-sdk-main/gta4-recomp/src/android/android_platform_stubs.cpp",
    """  if(phone_rumble_method)rex::input::SetTouchGamepadRumbleProvider(&AndroidPhoneRumble);
  if(type)env->DeleteLocalRef(type);env->DeleteLocalRef(activity);
}

}
void AndroidRegisterMobileSettingsCallbacks()""",
    """  if(phone_rumble_method)rex::input::SetTouchGamepadRumbleProvider(&AndroidPhoneRumble);
  if(type)env->DeleteLocalRef(type);env->DeleteLocalRef(activity);
}

std::string AndroidMultiplayerStatusJson() {
  auto* runtime = rex::Runtime::instance();
  auto* kernel = runtime ? runtime->kernel_state() : nullptr;
  auto* live = kernel ? kernel->live_compatibility() : nullptr;
  if (!live) {
    return R"({\"backend\":\"starting\",\"state\":\"connecting\",\"directory_ready\":false,\"active_session_id\":\"0000000000000000\",\"online_port\":0})";
  }

  const auto backend = live->config().backend;
  const char* backend_name = backend == rex::system::xam::LiveBackend::kLan ? "LAN"
      : backend == rex::system::xam::LiveBackend::kCommunity ? "COMMUNITY" : "OFFLINE";
  const auto state = live->state();
  const char* state_name = state == rex::system::xam::LiveState::kAvailable ? "available"
      : state == rex::system::xam::LiveState::kConnecting ? "connecting"
      : state == rex::system::xam::LiveState::kError ? "error" : "offline";
  auto* directory = live->session_directory();
  std::ostringstream out;
  out << "{\\"backend\\":\\"" << backend_name << "\\","
      << "\\"state\\":\\"" << state_name << "\\","
      << "\\"directory_ready\\":" << (directory && directory->ready() ? "true" : "false") << ","
      << "\\"active_session_id\\":\\"" << std::hex << std::setw(16) << std::setfill('0')
      << live->active_session_id() << "\\","
      << "\\"online_port\\":" << std::dec << live->online_port() << ","
      << "\\"discovery_port\\":" << live->config().lan_discovery_port << ","
      << "\\"community_matchmaking_supported\\":false}";
  return out.str();
}

}
void AndroidRegisterMobileSettingsCallbacks()""",
)
replace_once(
    "glue/rexglue-sdk-main/gta4-recomp/src/android/android_platform_stubs.cpp",
    """  if(key.size()>96)return env->NewStringUTF("");
  return env->NewStringUTF(rex::cvar::GetFlagByName(key).c_str());""",
    """  if(key.size()>96)return env->NewStringUTF("");
  if (key == "gta4_multiplayer_status") {
    const auto status = AndroidMultiplayerStatusJson();
    return env->NewStringUTF(status.c_str());
  }
  return env->NewStringUTF(rex::cvar::GetFlagByName(key).c_str());""",
)

# Opt-in Galaxy A14 low-load preset: it only stages settings until the user saves.
replace_once(
    "os/android/app/src/main/java/com/libertyrecomp/MobileSettingsUi.java",
    """            if(selected[0].equals("Performance")&&query.isEmpty())ModernUi.gridAction(a,rows,"Balanced settings",()->{draft.put("android_gpu_turbo","false");draft.put("android_thermal_guard","true");draft.put("ui_battery_refresh","true");if(sustainedSupported(a))draft.put("ui_sustained","true");draft.put("audio_low_latency","false");updatePending.run();render[0].run();});""",
    """            if(selected[0].equals("Performance")&&query.isEmpty())ModernUi.gridAction(a,rows,"Balanced settings",()->{draft.put("android_gpu_turbo","false");draft.put("android_thermal_guard","true");draft.put("ui_battery_refresh","true");if(sustainedSupported(a))draft.put("ui_sustained","true");draft.put("audio_low_latency","false");updatePending.run();render[0].run();});
            if(selected[0].equals("Performance")&&query.isEmpty())ModernUi.gridAction(a,rows,"Galaxy A14 low-load test profile",()->{
                draft.put("resolution","540p");
                draft.put("gta4_native_upscaler","native");
                draft.put("gta4_frame_limit","30");
                draft.put("gta4_dynamic_draw_distance","true");
                draft.put("gta4_draw_distance_scale","0.60");
                draft.put("gta4_shadow_map_base_size","256");
                draft.put("gta4_native_shadow_interval","0");
                draft.put("gta4_reflection_resolution","quarter");
                draft.put("gta4_water_reflection_resolution","quarter");
                draft.put("gta4_mirror_reflection_resolution","quarter");
                draft.put("gta4_reflection_aa","off");
                draft.put("gta4_reflection_capture_distance","near");
                draft.put("gta4_native_water_reflection_interval","2");
                draft.put("gta4_native_environment_reflection_interval","2");
                draft.put("gta4_motion_blur","false");
                draft.put("gta4_native_max_queued_frames","2");
                draft.put("audio_adaptive_buffer","true");
                draft.put("audio_low_latency","false");
                draft.put("android_thermal_guard","true");
                draft.put("ui_battery_refresh","true");
                helpTitle.setText("GALAXY A14 LOW-LOAD TEST PROFILE");
                helpText.setText("Stages 540p output, a 30 FPS cap, reduced draw distance, shadows and reflections, two queued frames, adaptive audio buffering, and thermal protection. This is a reversible test preset, not a guarantee of a target frame rate. Press Save to apply it; settings marked restart take effect on the next launch.");
                updatePending.run();render[0].run();
            });""",
)

# Live multiplayer status tab. Host/join remains the retail game's phone flow.
replace_once(
    "os/android/app/src/main/java/com/libertyrecomp/SessionMenuUi.java",
    """   List<Button> buttons=new ArrayList<>();String[] names={"Session","Touch controls","Controller","Diagnostics"},icons={"info","touch","controls","diagnostics"};
   final java.util.function.Consumer<String> change=name->{state.scroll.put(state.tab,scroll.getScrollY());state.tab=name;content.removeAllViews();for(int n=0;n<buttons.size();n++)ModernUi.selected(buttons.get(n),names[n].equals(name));
    String[] keys=name.equals("Touch controls")?new String[]{"Edit mobile controls","Control settings"}:name.equals("Controller")?new String[]{"Controller settings","Edit Xbox controls"}:name.equals("Diagnostics")?new String[]{"Input test (30 seconds)","Capture renderer timings","Capture shadow issue","Share report"}:new String[]{"Achievements","Cheats","Settings"};""",
    """   List<Button> buttons=new ArrayList<>();final TextView[] multiplayerDetails={null};String[] names={"Session","Multiplayer","Touch controls","Controller","Diagnostics"},icons={"info","info","touch","controls","diagnostics"};
   final java.util.function.Consumer<String> change=name->{state.scroll.put(state.tab,scroll.getScrollY());state.tab=name;content.removeAllViews();multiplayerDetails[0]=null;for(int n=0;n<buttons.size();n++)ModernUi.selected(buttons.get(n),names[n].equals(name));
    if(name.equals("Multiplayer")){multiplayerDetails[0]=ModernUi.text(a,multiplayerSummary(setting(runtime,"gta4_multiplayer_status")),13,ModernUi.TEXT);multiplayerDetails[0].setTextIsSelectable(true);content.addView(multiplayerDetails[0]);}
    String[] keys=name.equals("Multiplayer")?new String[]{}:name.equals("Touch controls")?new String[]{"Edit mobile controls","Control settings"}:name.equals("Controller")?new String[]{"Controller settings","Edit Xbox controls"}:name.equals("Diagnostics")?new String[]{"Input test (30 seconds)","Capture renderer timings","Capture shadow issue","Share report"}:new String[]{"Achievements","Cheats","Settings"};""",
)
replace_once(
    "os/android/app/src/main/java/com/libertyrecomp/SessionMenuUi.java",
    "details.setText(d);",
    "details.setText(d);if(state.tab.equals(\"Multiplayer\")&&multiplayerDetails[0]!=null)multiplayerDetails[0].setText(multiplayerSummary(setting(runtime,\"gta4_multiplayer_status\")));",
)
replace_once(
    "os/android/app/src/main/java/com/libertyrecomp/SessionMenuUi.java",
    """  private SessionMenuUi(){}""",
    """  private static String multiplayerSummary(String raw){
   try{
    JSONObject status=new JSONObject(raw);
    String backend=status.optString("backend","unknown"),state=status.optString("state","unknown");
    boolean ready=status.optBoolean("directory_ready",false);
    StringBuilder out=new StringBuilder();
    out.append("ANDROID MULTIPLAYER\\n");
    if("LAN".equals(backend)){
     out.append(ready&&"available".equals(state)?"LAN session discovery: READY":"LAN session discovery: NOT READY");
     out.append("\\n\\nUse the Multiplayer option in GTA IV's in-game phone to host or search for a local session. Both devices must be on the same Wi-Fi/local network.");
     out.append("\\n\\nInternet/community matchmaking is not enabled in this Android runtime yet.");
    }else if("COMMUNITY".equals(backend)){
     out.append("Community matchmaking is selected but is not supported by this Android build. Select LAN mode for local-network sessions.");
    }else if("OFFLINE".equals(backend)){
     out.append("Multiplayer backend is set to OFFLINE. LAN sessions are disabled in the current runtime configuration.");
    }else{
     out.append("Multiplayer services are still starting. Check again in a moment.");
    }
    String session=status.optString("active_session_id","0000000000000000");
    if(!"0000000000000000".equals(session))out.append("\\n\\nActive session: ").append(session);
    int discoveryPort=status.optInt("discovery_port",0);if(discoveryPort>0)out.append("\\nDiscovery port: ").append(discoveryPort);
    int port=status.optInt("online_port",0);if(port>0)out.append("\\nGame network port: ").append(port);
    String error=status.optString("directory_error","");if(!error.isEmpty())out.append("\\n\\nRuntime detail: ").append(error);
    return out.toString();
   }catch(Exception ignored){return "Multiplayer status is not available yet. The Android runtime currently targets LAN sessions; internet matchmaking is not enabled.";}
  }
  private SessionMenuUi(){}""",
)

print("Applied Universal-owned Android multiplayer status bridge, pause-panel tab, and opt-in A14 preset.")
print("This patch reports the actual runtime state; it does not fake host/join or claim internet matchmaking.")
