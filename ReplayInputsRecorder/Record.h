#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "nlohmann-JSON/json.hpp"


//functions to serialize/deserialize to json
void to_json(nlohmann::json& j, const ControllerInput& c);
void from_json(const nlohmann::json& j, ControllerInput& c);

//macro to serialize/deserialize to json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vector, X, Y, Z)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Rotator, Pitch, Yaw, Roll)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GamepadSettings, ControllerDeadzone, DodgeInputThreshold, SteeringSensitivity, AirControlSensitivity)

struct PlayerState
{
	PlayerState() {}
	PlayerState(CarWrapper car) {
		if (!car) return;

		teamNum = car.GetTeamNum2();
		BoostWrapper boost = car.GetBoostComponent();
		if (boost)
		{
			boostAmount = boost.GetCurrentBoostAmount();
		}
		location = car.GetLocation();
		Rotation = car.GetRotation();
		Velocity = car.GetVelocity();
		AngularVelocity = car.GetAngularVelocity();
		b_superSonic = car.GetbSuperSonic();
		b_canJump = car.HasFlip();
		b_jumped = car.GetbJumped();
		b_doubledJumped = car.GetbDoubleJumped();
		b_isdodging = car.IsDodging();
		time_offGround = car.GetTimeOffGround();
		time_onGround = car.GetTimeOnGround();
		inputs = car.GetInput();
	}
	~PlayerState() {}

	int teamNum;
	float boostAmount;
	Vector location;
	Rotator Rotation;
	Vector Velocity;
	Vector AngularVelocity;
	bool b_superSonic;
	bool b_canJump;
	bool b_jumped;
	bool b_doubledJumped;
	bool b_isdodging;
	float time_offGround;
	float time_onGround;
	ControllerInput inputs;
};

//macro to serialize/deserialize to json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerState, teamNum, boostAmount, location, Rotation, Velocity, AngularVelocity, b_superSonic, b_canJump, b_jumped, b_doubledJumped,
	b_isdodging, time_offGround, time_onGround, inputs)

struct BallState
{
	BallState() {}
	BallState(BallWrapper ball) {
		if (!ball) return;

		location = ball.GetLocation();
		Rotation = ball.GetRotation();
		Velocity = ball.GetVelocity();
		AngularVelocity = ball.GetAngularVelocity();
	}
	~BallState() {}

	Vector location;
	Rotator Rotation;
	Vector Velocity;
	Vector AngularVelocity;
};

//macro to serialize/deserialize to json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BallState, location, Rotation, Velocity, AngularVelocity)

struct Player
{
	Player() {}
	Player(std::string _uid, std::string _name) : uid(_uid), name(_name) {}
	Player(PriWrapper pri, bool _isLocalPlayer = false) {
		if (pri)
		{
			uid = pri.GetUniqueIdWrapper().GetIdString();
			name = pri.GetPlayerName().ToString();
			state = PlayerState(pri.GetCar());
		}
		else
		{
			uid = "ERROR";
			name = "ERROR";
		}

		isLocalPlayer = _isLocalPlayer;
	}
	Player(CarWrapper car, bool _isLocalPlayer = false) {
		PriWrapper pri = car.GetPRI();
		if (pri)
		{
			uid = pri.GetUniqueIdWrapper().GetIdString();
			name = pri.GetPlayerName().ToString();
			state = PlayerState(car);
		}
		else
		{
			uid = "ERROR";
			name = "ERROR";
		}

		isLocalPlayer = _isLocalPlayer;
	}
	~Player() {}

	std::string uid;
	std::string name;
	bool isLocalPlayer;
	PlayerState state;
};

//macro to serialize/deserialize to json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Player, uid, name, isLocalPlayer, state)


struct Frame
{
	Frame() {}
	~Frame() {}

	BallState ball;
	std::vector<Player> players;
};

//macro to serialize/deserialize to json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Frame, ball, players)


struct Record
{
	GamepadSettings localPlayer_gamepadSettings;
	std::map<int, Frame> frameMap;
};

//functions to serialize/deserialize to json
void to_json(nlohmann::json& j, const Record& c);
void from_json(const nlohmann::json& j, Record& c);