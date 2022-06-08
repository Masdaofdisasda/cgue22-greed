#pragma once
#include <utility>

#include "Utils.h"
#include "observer.h"

class game_logic : subject
{
public:
	explicit game_logic(std::shared_ptr<global_state> state, PerFrameData& perframe_data);

	void add_observer(observer& observer) override;

	void remove_observer(observer& observer) override;
	void notify_observers(event event) override;

	void update();

	void calculate_score() const;

private:
	std::list<observer*> observer_list_;
	std::shared_ptr<global_state> state_;
	PerFrameData* perframe_data_{};
	float exit_height_ = 70.0f;
	float lava_trigger_height_ = 7.0f;
	float player_size_ = 1.8f;
	std::vector<glm::vec3> checkpoints_ = {
		glm::vec3(0.0f, 2.0f, 0.0f), // gold treasure
		glm::vec3(0.0f, 7.0f, 0.0f), // lvl 0 lava rise trigger
		glm::vec3(-17.0f, 24.0f, 1.0f), // lvl 1 exit jumping
		glm::vec3(-18.0f, 35.0f, 2.0f), // lvl 2 exit precision
		glm::vec3(16.0f, 48.0f, 0.0f), // lvl 3 exit climbing
		glm::vec3(0.0f, 70.0f, -21.0f), // lvl 4 exit physic

		glm::vec3(0.0f, 200.0f, 0.0f) // prevent crashes
	};
	int checkpoint_ = -1;
};

inline game_logic::game_logic(std::shared_ptr<global_state> state, PerFrameData& perframe_data)
: state_(std::move(state)), perframe_data_(&perframe_data)
{}

inline void game_logic::add_observer(observer& observer)
{
	observer_list_.push_back(&observer);
}

inline void game_logic::remove_observer(observer& observer)
{
	// TODO
}

inline void game_logic::notify_observers(const event event)
{
	for (observer* obs : observer_list_)
		obs->update(event);
}

inline void game_logic::update()
{
	//tutorial
	if (perframe_data_->delta_time.y > 5 && perframe_data_->delta_time.y < 15)
		state_->display_walk_tutorial = true;
	if (perframe_data_->delta_time.y > 25 && perframe_data_->delta_time.y < 35)
		state_->display_pause_tutorial = true;
	if (perframe_data_->delta_time.y > 45 && perframe_data_->delta_time.y < 55)
		state_->display_jump_tutorial = true;
	if (perframe_data_->delta_time.y > 60 && state_->lava_triggered == false)
		state_->display_loot_obj = true;
	if (perframe_data_->delta_time.y > 60 && state_->lava_triggered == true)
		state_->display_escape_obj = true;

	if (perframe_data_->view_pos.y > state_->waypoint.y + player_size_)
	{
		++checkpoint_;
		state_->waypoint = checkpoints_[checkpoint_];
	}

	if (state_->won == true || state_->lost == true)
		return;

	if (state_->lava_triggered == false && perframe_data_->view_pos.y > lava_trigger_height_ + player_size_)
	{
		state_->lava_triggered = true;
		notify_observers(escape);
	}

	if (perframe_data_->normal_map.z < 50000 && state_->lava_triggered)
	{
		perframe_data_->normal_map.z += 60 * perframe_data_->delta_time.x;
	}

	if (!state_->using_debug_camera && !state_->using_animation_camera)
	{
		if (perframe_data_->view_pos.y > exit_height_+ player_size_)
		{
			state_->won = true;
			notify_observers(fx_still);
			notify_observers(fx_won);
			state_->time_of_death = perframe_data_->delta_time.y;
		}
		if (perframe_data_->view_pos.y < state_->lava_height + player_size_)
		{
			state_->lost = true;
			notify_observers(fx_still);
			notify_observers(fx_lost);
			state_->time_of_death = perframe_data_->delta_time.y;
		}
	}
	calculate_score();
}

inline void game_logic::calculate_score() const
{
	const int distance_score = std::max(static_cast<int32_t>(perframe_data_->view_pos.y - (state_->lava_height + player_size_)), 0);
	state_->time_bonus = std::max(3 * 60 - static_cast<int32_t>(perframe_data_->delta_time.y),0) * distance_score;
	const int score_multiplier = checkpoint_ + 1;
	state_->item_score = state_->total_cash * score_multiplier;
	
	state_->score = state_->item_score + state_->time_bonus;
}

