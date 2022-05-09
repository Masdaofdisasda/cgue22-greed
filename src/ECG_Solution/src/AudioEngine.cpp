#include "AudioEngine.h"

audio_engine::~audio_engine()
{
	engine_->drop();
}

sound_fx::sound_fx()
{
	steps->setDefaultVolume(0.3f);
	jump->setDefaultVolume(0.3f);
	collect->setDefaultVolume(0.3f);
}

void sound_fx::update(const event event)
{
	switch (event)
	{
	case fx_step:
		engine_->play2D(steps, true);
		break;
	case fx_still:
		engine_->stopAllSounds();
		break;
	case fx_jump:
		engine_->play2D(jump);
		break;
	case fx_collect:
		engine_->play2D(collect);
		break;
	default:;
	}
}

music::music()
{
	loading_music->setDefaultVolume(0.3f);
	colleting_music->setDefaultVolume(0.3f);
}

void music::update(const event event)
{
	switch (event)
	{
	case loading:
		engine_->stopAllSounds();
		engine_->play2D(loading_music, true);
		break;
	case collecting:
		engine_->stopAllSounds();
		engine_->play2D(colleting_music, true);
		break;
	default:;
	}
}

