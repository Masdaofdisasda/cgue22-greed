#pragma once

#include <irrKlang/irrKlang.h>
#include "observer.h"
#include <vector>

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
	irrklang::ISoundSource* steps_ = engine_->addSoundSourceFromFile("../../assets/media/fx_stepsLoop.mp3");
	irrklang::ISoundSource* won_ = engine_->addSoundSourceFromFile("../../assets/media/fx_won.mp3");
	irrklang::ISoundSource* lost_ = engine_->addSoundSourceFromFile("../../assets/media/fx_lost.mp3");

	std::vector<irrklang::ISoundSource*> drop_ = {
		engine_->addSoundSourceFromFile("../../assets/media/fx_drop1.mp3"),
		engine_->addSoundSourceFromFile("../../assets/media/fx_drop2.mp3"),
		engine_->addSoundSourceFromFile("../../assets/media/fx_drop3.mp3")
	};
	std::vector<irrklang::ISoundSource*> jump_ = {
		engine_->addSoundSourceFromFile("../../assets/media/fx_jump1.mp3"),
		engine_->addSoundSourceFromFile("../../assets/media/fx_jump2.mp3"),
		engine_->addSoundSourceFromFile("../../assets/media/fx_jump3.mp3")
	};
	std::vector<irrklang::ISoundSource*> loot_ = {
		engine_->addSoundSourceFromFile("../../assets/media/fx_loot1.mp3"),
		engine_->addSoundSourceFromFile("../../assets/media/fx_loot2.mp3"),
		engine_->addSoundSourceFromFile("../../assets/media/fx_loot3.mp3")
	};
	std::vector<irrklang::ISoundSource*> mock_ = {
		engine_->addSoundSourceFromFile("../../assets/media/mock1.mp3"),
		engine_->addSoundSourceFromFile("../../assets/media/mock2.mp3"),
		engine_->addSoundSourceFromFile("../../assets/media/mock3.mp3"),
		engine_->addSoundSourceFromFile("../../assets/media/mock4.mp3"),
		engine_->addSoundSourceFromFile("../../assets/media/mock5.mp3"),
		engine_->addSoundSourceFromFile("../../assets/media/mock6.mp3")
	};
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
	irrklang::ISoundSource* ost_loading_ = engine_->addSoundSourceFromFile("../../assets/media/ost_loading.mp3");
	irrklang::ISoundSource* ost_collecting_ = engine_->addSoundSourceFromFile("../../assets/media/ost_collecting.mp3");
	irrklang::ISoundSource* ost_escape_ = engine_->addSoundSourceFromFile("../../assets/media/ost_escape.mp3");
	irrklang::ISoundSource* ost_win_ = engine_->addSoundSourceFromFile("../../assets/media/ost_win.mp3");
	irrklang::ISoundSource* ost_lost_ = engine_->addSoundSourceFromFile("../../assets/media/ost_lost.mp3");

	irrklang::ISoundSource* amb_drops_ = engine_->addSoundSourceFromFile("../../assets/media/amb_drops.mp3");
};


