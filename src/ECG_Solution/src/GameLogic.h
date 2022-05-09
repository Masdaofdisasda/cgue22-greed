#pragma once
#include <utility>

#include "Utils.h"

class game_logic
{
public:
	explicit game_logic(std::shared_ptr<global_state> state, PerFrameData& perframe_data);


	void update() const;

private:
	std::shared_ptr<global_state> state_;
	PerFrameData* perframe_data_{};
	float exit_height_ = 500.0f;
	float player_size_ = 1.8f;
};

inline game_logic::game_logic(std::shared_ptr<global_state> state, PerFrameData& perframe_data)
: state_(std::move(state)), perframe_data_(&perframe_data)
{}

inline void game_logic::update() const
{
	if (perframe_data_->view_pos.y > exit_height_+ player_size_)
	{
		state_->won = true;
	}
	if (perframe_data_->view_pos.y < state_->lava_height + player_size_)
	{
		state_->lost = true;
	}
}

