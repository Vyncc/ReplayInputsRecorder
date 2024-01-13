#include "pch.h"
#include "ReplayInputsRecorder.h"

std::string ReplayInputsRecorder::GetPluginName()
{
	return "ReplayInputsRecorder";
}

void ReplayInputsRecorder::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> ReplayInputRecorder
void ReplayInputsRecorder::RenderSettings()
{
	ImGui::Text("Frames Count : %d", record.frameMap.size());

	//This is just to debug
	if (ImGui::Button("Save Frames To File"))
	{
		gameWrapper->Execute([&](GameWrapper* gw) {
			Export();
			});
	}

	//This is just to debug
	for (int n = 0; n < record.frameMap.size(); n++)
	{
		if (ImGui::TreeNode(std::to_string(n).c_str()))
		{
			renderFrame(record.frameMap[n]);

			ImGui::TreePop();
		}
	}
}

void ReplayInputsRecorder::renderFrame(Frame& frame)
{
	for (const Player& player : frame.players)
	{
		if (ImGui::CollapsingHeader(std::string(player.name + " | " + player.uid).c_str()))
		{
			const ControllerInput& inputs = player.state.inputs;
			ImGui::Text("Throttle : %f", inputs.Throttle);
			ImGui::Text("Steer : %f", inputs.Steer);
			ImGui::Text("Pitch : %f", inputs.Pitch);
			ImGui::Text("Yaw : %f", inputs.Yaw);
			ImGui::Text("Roll : %f", inputs.Roll);
			ImGui::Text("DodgeForward : %f", inputs.DodgeForward);
			ImGui::Text("DodgeStrafe : %f", inputs.DodgeStrafe);
			ImGui::Text("Handbrake : %d", inputs.Handbrake);
			ImGui::Text("Jump : %d", inputs.Jump);
			ImGui::Text("ActivateBoost : %d", inputs.ActivateBoost);
			ImGui::Text("HoldingBoost : %d", inputs.HoldingBoost);
			ImGui::Text("Jumped : %d", inputs.Jumped);
		}
	}
}

/*
// Do ImGui rendering here
void ReplayInputsRecorder::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

// Name of the menu that is used to toggle the window.
std::string ReplayInputsRecorder::GetMenuName()
{
	return "ReplayInputsRecorder";
}

// Title to give the menu
std::string ReplayInputsRecorder::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void ReplayInputsRecorder::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool ReplayInputsRecorder::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool ReplayInputsRecorder::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void ReplayInputsRecorder::OnOpen()
{
	isWindowOpen_ = true;
}

// Called when window is closed
void ReplayInputsRecorder::OnClose()
{
	isWindowOpen_ = false;
}
*/
