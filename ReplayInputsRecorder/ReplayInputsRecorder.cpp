#include "pch.h"
#include "ReplayInputsRecorder.h"


BAKKESMOD_PLUGIN(ReplayInputsRecorder, "ReplayInputsRecorder", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void ReplayInputsRecorder::onLoad()
{
	_globalCvarManager = cvarManager;

	//Create "ReplayInputsRecorder" directory in data folder if not already existsing
	std::filesystem::path dataFolderPath = gameWrapper->GetDataFolder() / "ReplayInputsRecorder";
	if (!std::filesystem::exists(dataFolderPath))
	{
		try
		{
			std::filesystem::create_directory(dataFolderPath);
		}
		catch (const std::exception& e)
		{
			LOG("Error during creation of directory \"ReplayInputsRecorder\" in bakkesmod data folder !");
		}
	}

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded",
		[this](std::string eventname) {

			Export();
		});

	gameWrapper->HookEvent("Function TAGame.Replay_TA.StartRecord",
		[this](std::string eventname) {

			IsReplayRecording = true;
			LOG("Replay start recording");
		});

	gameWrapper->HookEvent("Function TAGame.Replay_TA.StopRecord",
		[this](std::string eventname) {

			IsReplayRecording = false;
			LOG("Replay stop recording");
		});

	gameWrapper->HookEventWithCaller<ActorWrapper>("Function TAGame.Replay_TA.Tick",
		[this](ActorWrapper caller, void* params, std::string eventname) {

			if (!IsReplayRecording)
			{
				//LOG("Replay is not recording !");
				return;
			}

			UReplay_TA_execTick_Params* param = (UReplay_TA_execTick_Params*)params;
			if (!param) return;

			int replay_ta_numFrame = *reinterpret_cast<int*>(caller.memory_address + REPLAY_TA_NUM_FRAME_OFFSET);
			//int replay_ta_currentFrame = *reinterpret_cast<int*>(caller.memory_address + REPLAY_TA_CURRENT_FRAME_OFFSET);
			float replay_ta_accumulatedDeltaTime = *reinterpret_cast<float*>(caller.memory_address + REPLAY_TA_ACCUMULATED_DELTA_TIME_OFFSET);

			//LOG("caller address : {}", Hex(caller.memory_address, sizeof(caller.memory_address)));
			//LOG("replay_ta numFrame : {}", replay_ta_numFrame);
			////LOG("replay_ta currentFrame : {}", replay_ta_currentFrame);
			//LOG("replay_ta accummulated delta time : {}", replay_ta_accumulatedDeltaTime);

			//This means that the replay adds a new key frame (didn't find a better way to detect it, it doesn't seem that any event is fired when a new key frame is pushed)
			//So we save the inputs for all the players at this same time
			if (replay_ta_accumulatedDeltaTime == 0.f)
			{
				RecordFrame(replay_ta_numFrame - 1);
			}
		});

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		RenderCanvas(canvas);
		});

	//This is just to debug
	cvarManager->registerNotifier("exportreplay", [&](std::vector<std::string> args) {
		Export();
		}, "", PERMISSION_ALL);
}


//This is just to debug. It displays the inputs when watching the replay. It's pretty bad, need to remove
void ReplayInputsRecorder::RenderCanvas(CanvasWrapper canvas)
{
	if (!gameWrapper->IsInReplay()) return;

	ReplayServerWrapper replayServer = gameWrapper->GetGameEventAsReplay();
	if (!replayServer) return;

	if (record.frameMap.size() <= 0) return;

	int currentFrame = replayServer.GetCurrentReplayFrame();
	ControllerInput inputs;

	if (currentFrame > record.frameMap.size() - 1) return;

	for (const Player& player : record.frameMap[currentFrame].players)
	{
		inputs = player.state.inputs;
		break;
	}

	canvas.SetColor(255, 255, 255, 255);
	canvas.SetPosition(Vector2{ 10, 0 });
	canvas.DrawString("Throttle : " + std::to_string(inputs.Throttle), 2.f, 2.f);
	canvas.SetPosition(Vector2{ 10, 30 });
	canvas.DrawString("Steer : " + std::to_string(inputs.Steer), 2.f, 2.f);
	canvas.SetPosition(Vector2{ 10, 60 });
	canvas.DrawString("Pitch : " + std::to_string(inputs.Pitch), 2.f, 2.f);
	canvas.SetPosition(Vector2{ 10, 90 });
	canvas.DrawString("Yaw : " + std::to_string(inputs.Yaw), 2.f, 2.f);
	canvas.SetPosition(Vector2{ 10, 120 });
	canvas.DrawString("Roll : " + std::to_string(inputs.Roll), 2.f, 2.f);
	canvas.SetPosition(Vector2{ 10, 150 });
	canvas.DrawString("DodgeForward : " + std::to_string(inputs.DodgeForward), 2.f, 2.f);
	canvas.SetPosition(Vector2{ 10, 180 });
	canvas.DrawString("DodgeStrafe : " + std::to_string(inputs.DodgeStrafe), 2.f, 2.f);
	canvas.SetPosition(Vector2{ 10, 210 });
	canvas.DrawString("Handbrake : " + std::to_string(inputs.Handbrake), 2.f, 2.f);
	canvas.SetPosition(Vector2{ 10, 240 });
	canvas.DrawString("Jump : " + std::to_string(inputs.Jump), 2.f, 2.f);
	canvas.SetPosition(Vector2{ 10, 270 });
	canvas.DrawString("ActivateBoost : " + std::to_string(inputs.ActivateBoost), 2.f, 2.f);
	canvas.SetPosition(Vector2{ 10, 300 });
	canvas.DrawString("HoldingBoost : " + std::to_string(inputs.HoldingBoost), 2.f, 2.f);
	canvas.SetPosition(Vector2{ 10, 330 });
	canvas.DrawString("Jumped : " + std::to_string(inputs.Jumped), 2.f, 2.f);
}

void ReplayInputsRecorder::RecordFrame(int frameIndex)
{
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) return;

	if (record.frameMap.size() == 0)
	{
		record.localPlayer_gamepadSettings = gameWrapper->GetSettings().GetGamepadSettings();
	}

	BallWrapper ball = server.GetBall();
	if (ball)
	{
		record.frameMap[frameIndex].ball = BallState(ball);
	}

	for (CarWrapper car : server.GetCars())
	{
		if (!car) continue;

		bool isLocalPlayer = (car.memory_address == gameWrapper->GetLocalCar().memory_address);
		record.frameMap[frameIndex].players.push_back(Player(car, isLocalPlayer));
	}
}

void ReplayInputsRecorder::Export()
{
	std::filesystem::path replayDirectory = gameWrapper->GetDataFolder() / "ReplayInputsRecorder" / DateTimeToString();

	try
	{
		std::filesystem::create_directory(replayDirectory);
	}
	catch (const std::exception& e)
	{
		LOG("Can't export replay !");
		LOG("Error when creating replay directory : {}\n{}", replayDirectory.string(), e.what());
		return;
	}

	ExportReplay(replayDirectory, gameWrapper->GetCurrentGameState());
	ExportFrames(replayDirectory);

	//clear record
	record = Record();
}

void ReplayInputsRecorder::ExportReplay(std::filesystem::path replayDirectory, ServerWrapper server)
{
	if (!server)
	{
		LOG("Could not export replay, server is NULL!");
		return;
	}

	// Get ReplayDirector
	ReplayDirectorWrapper replayDirector = server.GetReplayDirector();
	if (!replayDirector)
	{
		LOG("Could not export replay, director is NULL!");
		return;
	}

	// Get Replay wrapper
	ReplaySoccarWrapper soccarReplay = replayDirector.GetReplay();
	if (!soccarReplay)
	{
		LOG("Could not export replay, replay is NULL!");
		return;
	}

	soccarReplay.StopRecord();

	std::string replayName = DateTimeToString();
	std::filesystem::path replayFilePath = replayDirectory / std::string(replayName + ".replay");

	soccarReplay.SetReplayName(replayName);
	LOG("set replay name to : {}", replayName);

	soccarReplay.ExportReplay(replayFilePath);
	LOG("Exported replay to : {}", replayFilePath.string());
}

void ReplayInputsRecorder::ExportFrames(std::filesystem::path replayDirectory)
{
	try
	{
		std::filesystem::path replayInputsFilePath = replayDirectory / std::string(DateTimeToString() + ".json");
		std::ofstream JSONFile(replayInputsFilePath);

		//convert record to json
		nlohmann::json record_json = record;

		JSONFile << record_json.dump();
		JSONFile.close();

		LOG("Record saved in : {}", replayDirectory.string());
	}
	catch (const std::exception& e)
	{
		LOG("Error exporting record in : {}\n{}", replayDirectory.string(), e.what());
	}
}


std::string ReplayInputsRecorder::DateTimeToString()
{
	std::time_t currentTime = std::time(nullptr);
	std::tm tm = *std::localtime(&currentTime);

	return std::to_string(1900 + tm.tm_year) + "." + std::to_string(tm.tm_mon + 1) + "." + std::to_string(tm.tm_mday) + "-" + std::to_string(tm.tm_hour) + "." + std::to_string(tm.tm_min) + "." + std::to_string(tm.tm_sec);
}

std::string ReplayInputsRecorder::Hex(uintptr_t address, uint64_t width)
{
	std::ostringstream stream;
	stream << "0x" << std::setfill('0') << std::setw(width) << std::right << std::uppercase << std::hex << address;
	return stream.str();
}


void ReplayInputsRecorder::onUnload()
{
}