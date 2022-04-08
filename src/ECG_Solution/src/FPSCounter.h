#pragma once
#include <string>
#include <assert.h>

/// @brief handles the logic for frames per second calculation
class FPSCounter
{
public:

	explicit FPSCounter(float avgIntervalSec = 0.5f) : avgIntervalSec_(avgIntervalSec)
	{
		assert(avgIntervalSec > 0.0f);
	}

	/// @brief calculate average frames per second
	/// @param deltaSeconds time since last frame
	/// @return 
	bool tick(
		float deltaSeconds) {
		numFrames_++;
		accumulatedTime_ += deltaSeconds; if (accumulatedTime_ < avgIntervalSec_)
			return false;
		currentFPS_ = static_cast<float>(
			numFrames_ / accumulatedTime_);
		fps = std::to_string(currentFPS_);
		numFrames_ = 0;
		accumulatedTime_ = 0;
		return true;
	}

	std::string FPSCounter::getFPS() const { return fps; }

private:
	const float avgIntervalSec_ = 0.5f;
	unsigned int numFrames_ = 0;
	double accumulatedTime_ = 0;
	float currentFPS_ = 0.0f;
	std::string fps = "60";
};