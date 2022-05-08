#pragma once

#include <irrKlang/irrKlang.h>
#include "observer.h"

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


class sound_fx final : public audio_engine
{
public:
	void update(const event event) override;
};

class music final : public audio_engine
{
public:
	void update(const event event) override;
};


