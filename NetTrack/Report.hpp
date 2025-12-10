#pragma once
#include <string>;
#include <nlohmann/json.hpp>
class Report
{
private:
	nlohmann::json result;

public:
	Report(std::string ruleName)
	{
		result["rule"] = ruleName;
	}

	nlohmann::json& operator[](const std::string& key)
	{
		return result[key];
	}

	const nlohmann::json& operator[](const std::string& key) const
	{
		return result.at(key);
	}

	nlohmann::json& getJson() { return result; }
	const nlohmann::json& getJson() const { return result; }
};