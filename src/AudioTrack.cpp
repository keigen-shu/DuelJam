#include "AudioTrack.hpp"

////    GUI consts    /////////////////////////////////////////////////
constexpr int   kPadding    = 4     , kmPadding     = 2;
constexpr int   kWidth      = 40    , kmWidth       = 12;

constexpr int   kSliderWidth    = 2 * kPadding + 1;


const sizei kEQGainSize = {
	static_cast<int>(static_cast<float>(kWidth - 3 * kPadding) / 3.0f),
	kWidth + 1
};

const sizei kEQFreqSize = {
	static_cast<int>(static_cast<float>(kWidth - 2 * kPadding) / 2.0f),
	kSliderWidth
};

const sizei kPanSize        = { kWidth - 2 * kPadding, kSliderWidth };
const sizei kMeterSize      = { kPadding - 1 , 120 };   // Single meter

constexpr int   kMarkerWidth    = kPadding;
const sizei kFaderSize      = {(kWidth - 3 * kPadding) / 3, 120 + 2 * kPadding };
const sizei kButtonSize     = {(kWidth - 3 * kPadding) / 2, kPadding * 2 };

const sizei kSize           = {
	kWidth,
	kEQGainSize.height + kEQFreqSize.height + kPanSize.height + kMeterSize.height + kButtonSize.height * 2 + 9 * kPadding
};

const point2i   koEQGain    = { kPadding, kPadding };
const point2i   koEQFreq    = { kPadding, koEQGain.y + kEQGainSize.height + kPadding };
const int       kySplit     = koEQFreq.y + kEQFreqSize.height + kPadding * 2;
const point2i   koPan       = { kPadding, kPadding + kySplit };
const point2i   koMeter     = { kPadding, koPan.y + kPanSize.height + kPadding };
const point2i   koButton    = { kPadding, koMeter.y + kFaderSize.height + kPadding };

////    Mini Mode
const sizei     kmMeterSize     = { kmPadding, 120 };
const sizei     kmButtonSize    = { kmWidth - 2 * kmPadding, kmWidth - kmPadding * 2 };

const sizei     kmSize      = { kmWidth, kmMeterSize.height + kmButtonSize.height * 2 + kmPadding * 5 };

const point2i   kmoMeter    = { kmPadding, kmPadding };
const point2i   kmoButton   = { kmPadding, kmoMeter.y + kmMeterSize.height + kmPadding * 2 };

sizei AudioTrack::_getSize(bool mini) { return mini ? kmSize : kSize; }

// const clan::BoxGeometry kGeometry

////    AudioTrack class    ///////////////////////////////////////////
AudioTrack::AudioTrack(
		awe::Source::Track* source
)   : clan::View()
	, mTrack(source)
	, m3BEQ (new awe::Filter::TBEQ<2>(
			mTrack->getConfig().sampleRate,
			600.0, 8000.0, 1.0, 1.0, 1.0
			))
	, mMixer(new awe::Filter::AscMixer<2>(
			1.0, 0.0, awe::Filter::AscMixer<2>::IEType::LINEAR
			// Track mixers are linear. Sample mixers are radial
			))
	, mMeter(new awe::Filter::AscMetering(
			mTrack->getConfig().sampleRate / 2,
			1.0))

	, mGCsdvEQGainL()
	, mGCsdvEQGainM()
	, mGCsdvEQGainH()

	, mGCsdhEQFreqL()
	, mGCsdhEQFreqH()

	, mGCsdvGain()
	, mGCsdhPan ()

	, mGCbtnMute()
	, mGCbtnToggleSize()
{
	mTrack->getRack().attach_filter(m3BEQ);
	mTrack->getRack().attach_filter(mMixer);
	mTrack->getRack().attach_filter(mMeter);

	////    BACKGROUND    ////

	box_style.set_background(clan::Colorf::white);
	box_style.set_width  (kSize.width);
	box_style.set_height (kSize.height);
	box_style.set_padding(kPadding, kPadding);

	////    EQ GAIN    ////

	clan::BoxStyle kEQGainBoxStyle;
	kEQGainBoxStyle.set_width (kEQGainSize.width);
	kEQGainBoxStyle.set_height(kEQGainSize.height);
	kEQGainBoxStyle.set_margin(kPadding / 4, kPadding / 2);

	mGCsdvEQGainL.box_style = kEQGainBoxStyle;
	mGCsdvEQGainM.box_style = kEQGainBoxStyle;
	mGCsdvEQGainH.box_style = kEQGainBoxStyle;

	mGCsdvEQGainL.mcBackground = clan::Colorf{ 1.0f, 0.9f, 0.9f };
	mGCsdvEQGainM.mcBackground = clan::Colorf{ 0.9f, 1.0f, 0.9f };
	mGCsdvEQGainH.mcBackground = clan::Colorf{ 0.9f, 0.9f, 1.0f };

	mGCsdvEQGainL.set_direction(UI::Slider::Direction::DOWN);
	mGCsdvEQGainM.set_direction(UI::Slider::Direction::DOWN);
	mGCsdvEQGainH.set_direction(UI::Slider::Direction::DOWN);

	mGCsdvEQGainL.set_range(-10, 10, 0);
	mGCsdvEQGainM.set_range(-10, 10, 0);
	mGCsdvEQGainH.set_range(-10, 10, 0);

	mGCsdvEQGainL.set_value(0);
	mGCsdvEQGainM.set_value(0);
	mGCsdvEQGainH.set_value(0);

	mGCsdvEQGainL.func_value_changed().connect(this, &AudioTrack::eq_gain_changed);
	mGCsdvEQGainM.func_value_changed().connect(this, &AudioTrack::eq_gain_changed);
	mGCsdvEQGainH.func_value_changed().connect(this, &AudioTrack::eq_gain_changed);

	mGCsdvEQGainL.set_focus_policy(clan::FocusPolicy::reject);
	mGCsdvEQGainM.set_focus_policy(clan::FocusPolicy::reject);
	mGCsdvEQGainH.set_focus_policy(clan::FocusPolicy::reject);

	////    EQ FREQ    ////

	clan::BoxStyle kEQFreqBoxStyle;
	kEQFreqBoxStyle.set_width(kEQFreqSize.width);
	kEQFreqBoxStyle.set_height(kEQFreqSize.height);
	kEQFreqBoxStyle.set_margin(kPadding / 2, kPadding / 2);

	mGCsdhEQFreqL.box_style = kEQFreqBoxStyle;
	mGCsdhEQFreqH.box_style = kEQFreqBoxStyle;

	mGCsdhEQFreqL.mcBackground = clan::Colorf{ 1.0f, 1.0f, 0.8f };
	mGCsdhEQFreqH.mcBackground = clan::Colorf{ 0.8f, 1.0f, 1.0f };

	mGCsdhEQFreqL.set_direction(UI::Slider::Direction::RIGHT);
	mGCsdhEQFreqH.set_direction(UI::Slider::Direction::RIGHT);

	mGCsdhEQFreqL.set_range(0, 20, 8);
	mGCsdhEQFreqH.set_range(0, 20, 16);

	mGCsdhEQFreqL.func_value_changed().connect(this, &AudioTrack::eq_freq_changed);
	mGCsdhEQFreqH.func_value_changed().connect(this, &AudioTrack::eq_freq_changed);

	mGCsdhEQFreqL.set_value(8);
	mGCsdhEQFreqH.set_value(16);

	mGCsdhEQFreqL.set_focus_policy(clan::FocusPolicy::reject);
	mGCsdhEQFreqH.set_focus_policy(clan::FocusPolicy::reject);

	////    PAN    ////
	clan::BoxStyle kPanStyle;
	kPanStyle.set_width(kPanSize.width);
	kPanStyle.set_height(kPanSize.height);
	kPanStyle.set_margin(kPadding, kPadding / 2);

	mGCsdhPan.box_style = kPanStyle;
	mGCsdhPan.set_direction(UI::Slider::Direction::RIGHT);
	mGCsdhPan.set_range(-10, 10, 0);
	mGCsdhPan.set_value(0);

	mGCsdhPan.func_value_changed().connect(this, &AudioTrack::mixer_value_changed);

	mGCsdhPan.set_focus_policy(clan::FocusPolicy::reject);

	////    FADER    ////
	clan::BoxStyle kFaderStyle;
	kFaderStyle.set_width(kFaderSize.width);
	kFaderStyle.set_height(kFaderSize.height);
	kFaderStyle.set_margin(kPadding, kPadding / 2);

	mGCsdvGain.set_direction(UI::Slider::Direction::DOWN);
	mGCsdvGain.set_range(48, 119, 96); // 48 ~ 120; tick step 2; page step 8
	mGCsdvGain.set_value(96);

	mGCsdvGain.func_value_changed().connect(this, &AudioTrack::mixer_value_changed);

	mGCsdvGain.set_focus_policy(clan::FocusPolicy::reject);

	////    MUTE and TOGGLE MINI MODE BUTTONS    ////
	this->size_toggled(false);

	mGCbtnMute.mcBackground[1] = { 1.0f, 0.6f, 0.6f }; // Red illumination when pressed
	mGCbtnMute.mcBackground[3] = { 1.0f, 0.8f, 1.0f };
	mGCbtnMute.mcBackground[4] = { 1.0f, 0.4f, 0.6f };

	mGCbtnMute      .func_toggled().connect(this, &AudioTrack::mute_toggled);
	mGCbtnToggleSize.func_toggled().connect(this, &AudioTrack::size_toggled);

	mGCbtnMute      .set_focus_policy(clan::FocusPolicy::reject);
	mGCbtnToggleSize.set_focus_policy(clan::FocusPolicy::reject);
}

AudioTrack::~AudioTrack() {
	mTrack->getRack().detach_filter(3);
	mTrack->getRack().detach_filter(2);
	mTrack->getRack().detach_filter(1);
	mTrack->getRack().detach_filter(0);

	delete mMeter;
	delete mMixer;
	delete m3BEQ;
}

////    GUI Component Methods    //////////////////////////////////
void AudioTrack::render_content(clan::Canvas &canvas)
{
	//  CONSTANTS
	const auto _getMeterLEDColor = [] (awe::Aint const &x) -> clan::Colorf
	{
		/**/ if (x < 72) return clan::Colorf::green;
		else if (x < 96) return clan::Colorf::orange;
		else             return clan::Colorf::red;
	};

	const auto _getOverclipColor = [] (awe::Aint const &x) -> clan::Colorf
	{
		/**/ if (x < 40) return clan::Colorf::green;
		else if (x < 60) return clan::Colorf::orange;
		else             return { 1.0f, 0.0f, 0.0f, (x > 100) ? 1.0f : 0.75f };
	};

	//  VARIABLES
	float mxVol;

	awe::Asintf mtPeak;
	awe::Asintf mtRMS;
	awe::Asintf mtOCI;

	{   //  Calculate metering
		const float mScale = 2.0f;                      // Half-scale (because we took abs(-1.0f to -1.0f))
		const float mRange = -awe::dBFS_limit / mScale;

		auto _rescale_metering = [this, mRange, mScale] (float &x) {
			x = 20.0f * log10(x);
			x = (x == x) ? ( (x > -mRange) ? (x + mRange) : 0.0f) : 0.0f;
			x *= mScale;
		};

		awe::Asfloatf mtPeakf, mtRMSf;

		std::lock_guard<std::mutex> o_lock(mTrack->getMutex());

		mxVol   = mMixer->getVol();
		mtPeakf = mMeter->getPeak();
		mtRMSf  = mMeter->getAvgRMS();
		mtOCI   = mMeter->getOCI();

		// _rescale_metering(mxVol);
		mxVol = awe::to_dBFS(mxVol);
		mtPeakf(_rescale_metering);
		mtRMSf (_rescale_metering);

		mtPeak[0] = mtPeakf[0], mtPeak[1] = mtPeakf[1];
		mtRMS [0] = mtRMSf [0], mtRMS [1] = mtRMSf [1];
	}

	////    DRAW    ///////////////////////////////////////////////////

	const clan::Colorf clrMeterMarker = !this->mGCbtnMute.isOn()
		? clan::Colorf { 0.0f, 1.0f, 1.0f, 0.6f }
	: clan::Colorf { 0.1f, 0.1f, 0.1f, 1.0f }
	;

	//  Mini mode
	if (this->mGCbtnToggleSize.isOn())
	{   //  Meter Background
		canvas.fill_rect(
				recti(kmoMeter, sizei(
						kmWidth             - 2 * kmPadding,
						kMeterSize.height   + 1 * kmPadding
						)),
				clan::Colorf::black
				);

		{   //  Meter Bars
			const int oT = kmoMeter.y + 1;
			const int  w = kmMeterSize.width;

			//  Gain line
			canvas.draw_line(
					kmoMeter.x          , kmoMeter.y + 1 + mxVol,
					kmWidth - kmoMeter.x, kmoMeter.y + 1 + mxVol,
					clrMeterMarker
					);
			canvas.draw_line(
					kmWidth - kmoMeter.x - 1, kmoMeter.y + 1 + mxVol,
					kmWidth - kmoMeter.x - 1, kmoMeter.y + 2 + mxVol,
					clrMeterMarker
					);
			canvas.draw_line(
					kmWidth - kmoMeter.x, kmoMeter.y + 1 + mxVol,
					kmWidth - kmoMeter.x, kmoMeter.y + 3 + mxVol,
					clrMeterMarker
					);

			const int oL = kmoMeter.x + 1;
			const int oR = oL + w + 1;

			for(awe::Aint p = 0; p < 120; p += w+1)
			{
				clan::Colorf cRMS  = _getMeterLEDColor(p);
				clan::Colorf cPeak = { cRMS.r * 0.6f, cRMS.g * 0.6f, cRMS.b * 0.6f };
				clan::Colorf cNone = { 0.1f, 0.1f, 0.1f };

				/**/ if (mtRMS [0] > p)
					canvas.fill_rect(oL, oT+p, oL+w, oT+p+w, cRMS);
				else if (mtPeak[0] > p)
					canvas.fill_rect(oL, oT+p, oL+w, oT+p+w, cPeak);
				else
					canvas.fill_rect(oL, oT+p, oL+w, oT+p+w, cNone);

				/**/ if (mtRMS [1] > p)
					canvas.fill_rect(oR, oT+p, oR+w, oT+p+w, cRMS);
				else if (mtPeak[1] > p)
					canvas.fill_rect(oR, oT+p, oR+w, oT+p+w, cPeak);
				else
					canvas.fill_rect(oR, oT+p, oR+w, oT+p+w, cNone);
			}

			// TODO Implement audio overclip indicator for mini mode
		}

		return;
	}
	//  Large mode

	//  Draw component split line
	canvas.draw_line( 1, kySplit, box_style.width() - 1, kySplit, { 0.8f, 0.8f, 0.8f } );

	//  Meter Background
	canvas.fill_rect(
			recti(koMeter, sizei(
					2 * kPadding + 2 * kMeterSize.width + 1,
					2 * kPadding + kMeterSize.height
					)),
			clan::Colorf::black
			);


	//  Meter Bars
	{
		const int oT = kPadding + koMeter.y + 1;
		const int w  = kMeterSize.width;

		//  Gain line
		canvas.draw_line(
				koMeter.x + kmPadding                       , oT + mxVol,
				koMeter.x + kmPadding + kPadding + w * 2 + 1, oT + mxVol,
				clrMeterMarker
				);

		const int oL = koMeter.x + kPadding;
		const int oR = oL + w + 1;

		for(awe::Aint p = 0; p < 120; p += w+1)
		{
			clan::Colorf cRMS  = _getMeterLEDColor(p);
			clan::Colorf cPeak = { cRMS.r * 0.6f, cRMS.g * 0.6f, cRMS.b * 0.6f };
			clan::Colorf cNone = { 0.1f, 0.1f, 0.1f };

			/**/ if (mtRMS [0] > p)
				canvas.fill_rect(oL, oT+p, oL+w, oT+p+w, cRMS);
			else if (mtPeak[0] > p)
				canvas.fill_rect(oL, oT+p, oL+w, oT+p+w, cPeak);
			else
				canvas.fill_rect(oL, oT+p, oL+w, oT+p+w, cNone);

			/**/ if (mtRMS [1] > p)
				canvas.fill_rect(oR, oT+p, oR+w, oT+p+w, cRMS);
			else if (mtPeak[1] > p)
				canvas.fill_rect(oR, oT+p, oR+w, oT+p+w, cPeak);
			else
				canvas.fill_rect(oR, oT+p, oR+w, oT+p+w, cNone);
		}

		//  ON / OFF indicator ( currently tied to mute )
		canvas.fill_triangle(
				point2f ( oL - 3, oT - 3 ),
				point2f ( oL + w, oT - 3 ),
				point2f ( oL + w, oT - 1 ),
				clrMeterMarker
				);

		canvas.fill_triangle(
				point2f(oR    , oT-3 ),
				point2f(oR+w+4, oT-3 ),
				point2f(oR    , oT-1 ),
				clrMeterMarker
				);

		//  Overclip indicator
		canvas.draw_line(oL-2, oT+121, oL+w  , oT+121, _getOverclipColor(mtOCI[0]));
		canvas.draw_line(oR  , oT+121, oR+w+2, oT+121, _getOverclipColor(mtOCI[1]));
	}

	{   //  Markers for Meter bar and Fader
		//  draw dB lines, y + 1 because of dBFS round up
		int o = 4 * kPadding + 2 * kMeterSize.width + 1;
		int t = 1 + koMeter.y + kPadding;

		// > +0dB area
		canvas.fill_rect(
				o - kmPadding   , t + 96,
				o + kMarkerWidth, t + 120,
				{ 1.0f, 0.9f, 0.9f }
				);

		for(int i = t; i <= t + kMeterSize.height; i += 12)
			canvas.draw_line(
					o               , i,
					o + kMarkerWidth, i,
					clan::Colorf::grey
					);

		// -48 dBFS line
		canvas.draw_line(
				o - 1, t + 48, o + kMarkerWidth, t + 48,
				clan::Colorf::black
				);
		canvas.draw_line(
				o - 1, t, o - 1, t + 48,
				clan::Colorf::black
				);

		// 0 dBFS line
		canvas.draw_line(
				o - kmPadding   , t + 96,
				o + kMarkerWidth, t + 96,
				clan::Colorf::black
				);
	}
}

void AudioTrack::eq_gain_changed()
{
	float l = mGCsdvEQGainL.get_value(); l = l / 10.0f + 1.0f; // -10 ~ 10 -> 0.0f -> 2.0f
	float m = mGCsdvEQGainM.get_value(); m = m / 10.0f + 1.0f;
	float h = mGCsdvEQGainH.get_value(); h = h / 10.0f + 1.0f;

	clan::Console::write_line("LG = %1, MG = %2, HG = %3", l, m, h);
	m3BEQ->set_gain(l,m,h);
}

void AudioTrack::eq_freq_changed()
{
	float l = mGCsdhEQFreqL.get_value(); l = 2.0f * pow(10.0f, 1.0 + l / 10.0f); // 0 ~ 20 ->  200 ~  2000Hz
	float h = mGCsdhEQFreqH.get_value(); h = 2.0f * pow(10.0f, 2.0 + h / 10.0f); // 0 ~ 20 -> 2000 ~ 20000Hz

	clan::Console::write_line("LPF = %1, HPF = %2, SR = %3", l, h, mTrack->getConfig().sampleRate);
	m3BEQ->set_freq(l,h);
}

void AudioTrack::mixer_value_changed()
{
	float x = -mGCsdhPan.get_value(); x = x / 10.0f;
	float y = mGCsdvGain.get_value();
	if (y < -awe::dBFS_limit)
		y = awe::from_dBFS(y + awe::dBFS_limit);
	else
		y = awe::from_dBFS((mGCsdvGain.get_value() - 96.0f) / 2.0f);

	clan::Console::write_line("Pan = %1, Gain = %2", x, y);
	mMixer->setPan(x);
	mMixer->setVol(y);
}

void AudioTrack::mute_toggled(bool mute)
{
	awe::ArenderConfig config = mTrack->getConfig();
	config.quality = mute ? awe::ArenderConfig::Quality::MUTE : awe::ArenderConfig::Quality::DEFAULT;
	mTrack->setConfig(config);
}

void AudioTrack::size_toggled(bool mini)
{
	//  Toggle controller visibility
	mGCsdvEQGainL.set_hidden(!mini);
	mGCsdvEQGainM.set_hidden(!mini);
	mGCsdvEQGainH.set_hidden(!mini);
	mGCsdhEQFreqL.set_hidden(!mini);
	mGCsdhEQFreqH.set_hidden(!mini);

	mGCsdhPan .set_hidden(!mini);
	mGCsdvGain.set_hidden(!mini);

	// Resize elements
	if (mini) {
		box_style.set_width(kmSize.width);
		box_style.set_height(kmSize.height);

		mGCbtnMute.box_style.set_width(kmButtonSize.width);
		mGCbtnMute.box_style.set_height(kmButtonSize.height);
		mGCbtnMute.box_style.set_margin(kmPadding, kmPadding);

		mGCbtnToggleSize.box_style.set_width(kmButtonSize.width);
		mGCbtnToggleSize.box_style.set_height(kmButtonSize.height);
		mGCbtnToggleSize.box_style.set_margin(kmPadding, kmPadding);
	} else {
		box_style.set_width(kSize.width);
		box_style.set_height(kSize.height);

		mGCbtnMute.box_style.set_width(kButtonSize.width);
		mGCbtnMute.box_style.set_height(kButtonSize.height);
		mGCbtnMute.box_style.set_margin(kPadding, kPadding);

		mGCbtnToggleSize.box_style.set_width(kButtonSize.width);
		mGCbtnToggleSize.box_style.set_height(kButtonSize.height);
		mGCbtnToggleSize.box_style.set_margin(kPadding, kPadding);
	}
}
