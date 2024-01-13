#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

#include "Record.h"


#define REPLAY_TA_NUM_FRAME_OFFSET 0x110
#define REPLAY_TA_CURRENT_FRAME_OFFSET 0x15C
#define REPLAY_TA_ACCUMULATED_DELTA_TIME_OFFSET 0x1F0


class ReplayInputsRecorder: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow/*, public BakkesMod::Plugin::PluginWindow*/
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

	bool IsReplayRecording = false;

	Record record;
	void RecordFrame(int frameIndex);

	void Export();
	void ExportReplay(std::filesystem::path replayDirectory, ServerWrapper server);
	void ExportFrames(std::filesystem::path replayDirectory);

	
	void RenderCanvas(CanvasWrapper canvas);

	std::string DateTimeToString();
	std::string Hex(uintptr_t address, uint64_t width);


	// Inherited via PluginSettingsWindow
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;


	void renderFrame(Frame& frame);

	// Inherited via PluginWindow
	/*

	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "ReplayInputsRecorder";

	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;
	
	*/
};

