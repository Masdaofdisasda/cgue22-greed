
#include "AudioEngine.h"

#include <iostream>

audio_engine::~audio_engine()
{
	engine_->drop();
}

void sound_fx::update(const event event)
{
	switch (event)
	{
	case fx_step:
		engine_->play2D("../../assets/media/steps.wav", true);
		break;
	case fx_still:
		engine_->stopAllSounds();
		break;
	case fx_jump:
		engine_->play2D("../../assets/media/jump.wav");
		break;
	case fx_collect:
		engine_->play2D("../../assets/media/fx_collect.wav", false);
		break;
	default:;
	}
}

void music::update(const event event)
{
	switch (event)
	{
	case loading:
		engine_->stopAllSounds();
		engine_->play2D("../../assets/media/EQ07 Prc Fantasy Perc 060.wav", true);
		break;
	case collecting:
		engine_->stopAllSounds();
		engine_->play2D("../../assets/media/Wolum - Greed Collecting.mp3", true);
		break;
	default:;
	}
}

