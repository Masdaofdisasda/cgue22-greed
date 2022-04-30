#pragma once
#include <string>
#include <assert.h>

/// @brief handles the logic for frames per second calculation
class fps_counter
{
public:

	explicit fps_counter(const float avg_interval_sec = 0.5f) : avg_interval_sec_(avg_interval_sec)
	{
		assert(avg_interval_sec > 0.0f);
	}

	/// @brief calculate average frames per second
	/// @param deltaSeconds time since last frame
	/// @return 
	bool tick(
		float deltaSeconds) {
		num_frames_++;
		accumulated_time_ += static_cast<double>(deltaSeconds); if (accumulated_time_ < static_cast<double>(avg_interval_sec_))
			return false;
		current_fps_ = static_cast<float>(
			num_frames_ / accumulated_time_);
		fps_ = std::to_string(current_fps_);
		num_frames_ = 0;
		accumulated_time_ = 0;
		return true;
	}

	std::string get_fps() const { return fps_; }

private:
	const float avg_interval_sec_ = 0.5f;
	unsigned int num_frames_ = 0;
	double accumulated_time_ = 0;
	float current_fps_ = 0.0f;
	std::string fps_ = "60";
};