#pragma once
#include "SineWave.h"

namespace stk {

	class Scalable_SineWave :
		public SineWave
	{
	protected:
		StkFloat scale_; // [0 - 1.0];


	public:
		//! Default constructor.
		Scalable_SineWave(void)
		{
			this->scale_ = 1.0;
		}

		void setScale(StkFloat scaler)
		{
			this->scale_ = scaler;
		}

		StkFloat getScale() const
		{
			return this->scale_;
		}

		//override implicitly
		StkFloat tick(void)
		{
			StkFloat tmp = this->SineWave::tick();
			tmp = tmp * scale_;
			this->lastFrame_[0] = tmp;
			return tmp;
		}

		//override implicitly
		StkFrames& tick(StkFrames& frames, unsigned int channel = 0) 
		{
			frames = this->SineWave::tick(frames, channel);

			StkFloat* samples = &frames[channel];
			StkFloat tmp = 0.0;

			unsigned int hop = frames.channels();
			for (unsigned int i = 0; i < frames.frames(); i++, samples += hop) {

				tmp = (*samples) * scale_;
				*samples = tmp;
			}

			lastFrame_[0] = tmp;
			return frames;
		}
	};

}


