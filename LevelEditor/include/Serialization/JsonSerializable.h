#pragma once
#include "Core/Core.hpp"

#include "vendor/nolhmann/json.hpp"

#define IMPLEMENT_SERIALIZABLE \
virtual void Deserialize(nlohmann::json& _json); \
virtual void Serialize(); \

template<typename T>
class JsonSerializable
{
public:
	virtual void Deserialize(nlohmann::json& _json) = 0;

	virtual void Serialize() = 0;
};