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

private:
	std::list<observer*> observer_list_;
	std::shared_ptr<global_state> state_;
	PerFrameData* perframe_data_{};
	float exit_height_ = 100.0f;
	float lava_trigger_height_ = 7.0f;
	float player_size_ = 1.8f;
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
	if (state_->won == true || state_->lost == true)
		return;

	if (state_->lava_triggered == false && perframe_data_->view_pos.y > lava_trigger_height_ + player_size_)
	{
		state_->lava_triggered = true;
		notify_observers(escape);
	}

	if (perframe_data_->view_pos.y > exit_height_+ player_size_)
	{
		state_->won = true;
		notify_observers(fx_won);
	}
	if (perframe_data_->view_pos.y < state_->lava_height + player_size_)
	{
		state_->lost = true;
		notify_observers(fx_lost);
	}
}

