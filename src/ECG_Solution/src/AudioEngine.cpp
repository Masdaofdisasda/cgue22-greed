#include "AudioEngine.h"

audio_engine::~audio_engine()
{
	engine_->drop();
}

sound_fx::sound_fx()
{
	steps_->setDefaultVolume(0.3f);
	won_->setDefaultVolume(0.3f);
	lost_->setDefaultVolume(0.3f);

	for (irrklang::ISoundSource* src : drop_)
		src->setDefaultVolume(0.3f);

	for (irrklang::ISoundSource* src : jump_)
		src->setDefaultVolume(0.3f);

	for (irrklang::ISoundSource* src : loot_)
		src->setDefaultVolume(0.3f);

}

void sound_fx::update(const event event)
{
	switch (event)
	{
	case fx_step:
		engine_->play2D(steps_, true);
		break;
	case fx_still:
		engine_->stopAllSounds();
		break;
	case fx_drop:
		engine_->play2D(drop_[rand() % drop_.size()]);
		break;
	case fx_jump:
		engine_->play2D(jump_[rand() % jump_.size()]);
		break;
	case fx_collect:
		engine_->play2D(loot_[rand() % loot_.size()]);
		break;
	case fx_won:
		engine_->play2D(won_);
		break;
	case fx_lost:
		engine_->play2D(lost_);
		break;
	default:;
	}
}

music::music()
{
	ost_loading_->setDefaultVolume(0.3f);
	ost_collecting_->setDefaultVolume(0.3f);

	amb_drops_->setDefaultVolume(0.3f);
}

void music::update(const event event)
{
	switch (event)
	{
	case loading:
		engine_->stopAllSounds();
		engine_->play2D(ost_loading_, true);
		break;
	case collecting:
		engine_->stopAllSounds();
		engine_->play2D(ost_collecting_, true);
		engine_->play2D(amb_drops_, true);
		break;
	case fx_lost:
		engine_->stopAllSounds();
		break;
	default:;
	}
}

