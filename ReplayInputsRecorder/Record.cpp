#include "pch.h"
#include "Record.h"


//functions to serialize to json
void to_json(nlohmann::json& j, const ControllerInput& c)
{
	j = nlohmann::json{
		{"Throttle", c.Throttle},
		{"Steer", c.Steer},
		{"Pitch", c.Pitch},
		{"Yaw", c.Yaw},
		{"Roll", c.Roll},
		{"DodgeForward", c.DodgeForward},
		{"DodgeStrafe", c.DodgeStrafe},
		{"Handbrake", int(c.Handbrake)},
		{"Jump", int(c.Jump)},
		{"ActivateBoost", int(c.ActivateBoost)},
		{"HoldingBoost", int(c.HoldingBoost)},
		{"Jumped", int(c.Jumped)}
	};
}

//functions to deserialize to json
void from_json(const nlohmann::json& j, ControllerInput& c)
{
	j.at("Throttle").get_to(c.Throttle);
	j.at("Steer").get_to(c.Steer);
	j.at("Pitch").get_to(c.Pitch);
	j.at("Yaw").get_to(c.Yaw);
	j.at("Roll").get_to(c.Roll);
	j.at("DodgeForward").get_to(c.DodgeForward);
	j.at("DodgeStrafe").get_to(c.DodgeStrafe);
	c.Handbrake = j["Handbrake"].get<unsigned long>();
	c.Jump = j["Jump"].get<unsigned long>();
	c.ActivateBoost = j["ActivateBoost"].get<unsigned long>();
	c.HoldingBoost = j["HoldingBoost"].get<unsigned long>();
	c.Jumped = j["Jumped"].get<unsigned long>();
}


//functions to serialize to json
void to_json(nlohmann::json& j, const Record& c)
{
	nlohmann::json frameMap_json;
	for (const auto& framePair : c.frameMap)
	{
		frameMap_json.push_back(framePair.second);
	}

	j = nlohmann::json{
		{"localPlayer_gamepadSettings", c.localPlayer_gamepadSettings},
		{"frames", frameMap_json}
	};
}

//functions to deserialize to json
void from_json(const nlohmann::json& j, Record& c)
{
	j.at("localPlayer_gamepadSettings").get_to(c.localPlayer_gamepadSettings);

	for (int index = 0; index < j["frames"].size(); index++)
	{
		const auto& frame_json = j["frames"][index];
		c.frameMap.insert({ index, frame_json.get<Frame>() });
	}
}
