#pragma once

#include <irrKlang/irrKlang.h>
#include "observer.h"

/**
 * \brief audio interface for every class that plays sound
 */
class audio_engine : public observer
{
public:
	~audio_engine() override;

	/**
	 * \brief starts/stops playback of sound files based on the event
	 * \param event that happened
	 */
	void update(const event event) override {}

protected:
	irrklang::ISoundEngine* engine_ = irrklang::createIrrKlangDevice();
};

/**
 * \brief plays sound effects when the player moves or collects items
 */
class sound_fx final : public audio_engine
{
public:
	sound_fx();
	void update(const event event) override;
private:
	irrklang::ISoundSource* steps = engine_->addSoundSourceFromFile("../../assets/media/steps.wav");
	irrklang::ISoundSource* jump = engine_->addSoundSourceFromFile("../../assets/media/jump.wav");
	irrklang::ISoundSource* collect = engine_->addSoundSourceFromFile("../../assets/media/fx_collect.wav");
};

/**
 * \brief plays the games soundtrack
 */
class music final : public audio_engine
{
public:
	music();

	void update(const event event) override;
private:
	irrklang::ISoundSource* loading_music = engine_->addSoundSourceFromFile("../../assets/media/EQ07 Prc Fantasy Perc 060.wav");
	irrklang::ISoundSource* colleting_music = engine_->addSoundSourceFromFile("../../assets/media/Wolum - Greed Collecting.mp3");
};


