#ifndef ASA_RESOURCE_H
#define ASA_RESOURCE_H

#include <core/variant/variant.h>
#include <scene/2d/sprite_2d.h>
#include <scene/audio/audio_stream_player.h>
#include <core/io/image.h>
#include <scene/resources/gradient.h>
#include <scene/resources/segment_shape_2d.h>

// =======================================================
struct ASAOptions
{
	enum ScreenFollowsCursor
	{
		JUMP_IF_PLAYING_OFFSCREEN,
		DONT_FOLLOW_PLAYBACK,
		CURSOR_ALWAYS_LEFT,
		CURSOR_ALWAYS_MIDDLE
	};
	enum SoundSourceOption
	{
		PLAY_MIDI_AND_WAVE,
		PLAY_MIDI_ONLY,
		PLAY_WAVE_ONLY,
		MIDI_LEFT_WAVE_RIGHT,
		MIDI_RIGHT_WAVE_LEFT
	};
	struct FFT {
		inline bool operator==(const FFT& other) const
		{
			return other.low_high_exponent_low == this->low_high_exponent_low
				&& other.overamplification_multiplier_low == this->overamplification_multiplier_low
				&& other.low_high_exponent_high == this->low_high_exponent_high
				&& other.overamplification_multiplier_high == this->overamplification_multiplier_high
				&& other.tuning == this->tuning
				&& other.fft_size_low == this->fft_size_low
				&& other.fft_size_high == this->fft_size_high
				&& other.hop_size == this->hop_size
				&& other.subdivision == this->subdivision
				&& other.use_2_ffts == this->use_2_ffts;
		};
		inline bool operator!=(const FFT& other) const
		{
			if (this->operator==(other))
				return false;
			return true;
		};

		real_t low_high_exponent_low = 0.6;
		real_t overamplification_multiplier_low = 2.0;
		real_t low_high_exponent_high = 0.6;
		real_t overamplification_multiplier_high = 2.0;
		real_t tuning = 440.0;
		int fft_size_low = 16384;
		int fft_size_high = 4096;
		int hop_size = 1024;
		int subdivision = 9;
		bool use_2_ffts = true;
	} fft;

	struct NoteRecognition {
		inline bool operator==(const NoteRecognition& other) const
		{
			return other.minimum_length == this->minimum_length
				&& other.note_on_threshold == this->note_on_threshold
				&& other.note_off_threshold == this->note_off_threshold
				&& other.octave_removal_multiplier == this->octave_removal_multiplier
				&& other.volume_multiplier == this->volume_multiplier
				&& other.percussion_removal == this->percussion_removal;
		};
		inline bool operator!=(const NoteRecognition& other) const
		{
			if (this->operator==(other))
				return false;
			return true;
		};
		int minimum_length = 4;
		real_t note_on_threshold = 0.1;
		real_t note_off_threshold = 0.05;
		real_t octave_removal_multiplier = 0.2;
		real_t volume_multiplier = 2.0;
		real_t percussion_removal = 1.0;
	} note_recognition;

	struct General {
		inline bool operator==(const General& other) const
		{
			return other.triangle_follows_cursor == this->triangle_follows_cursor
				&& other.hide_notes == this->hide_notes
				&& other.screen_follows_cursor == this->screen_follows_cursor
				&& other.sound_source_option == this->sound_source_option;
		};
		inline bool operator!=(const General& other) const
		{
			if (this->operator==(other))
				return false;
			return true;
		};
		bool triangle_follows_cursor = false;
		bool hide_notes = false;
		ScreenFollowsCursor screen_follows_cursor = JUMP_IF_PLAYING_OFFSCREEN;
		SoundSourceOption sound_source_option = PLAY_MIDI_AND_WAVE;
	} general;

	struct Misc {
		inline bool operator==(const Misc& other) const
		{
			return other.file_path == this->file_path;
		}
		inline bool operator!=(const Misc& other) const
		{
			if (this->operator==(other))
				return false;
			return true;
		};
		String file_path = "";
	} misc;

	Array spectrum;
	Array images;
	List<Sprite2D*> sprites;
	ObjectID id;
	Ref<AudioStream> audio_stream;
	Ref<AudioStreamPlayback> audio_stream_playback;
	SafeFlag generating;
	float length = 0.0f;
	Vector<Vector<float>> magnitudes;
	Thread* thread = nullptr;
	Ref<Gradient> color_scheme;

	ASAOptions()
	{}
	ASAOptions(const ASAOptions& other)
	{
		this->spectrum = other.spectrum;
		this->images = other.images;
		this->sprites = other.sprites;
		this->id = other.id;
		this->audio_stream = other.audio_stream;
		this->audio_stream_playback = other.audio_stream_playback;
		this->generating.set_to(other.generating.is_set());
		this->length = other.length;
		this->magnitudes = other.magnitudes;
		this->thread = other.thread;
		this->color_scheme = other.color_scheme;

		this->fft.fft_size_high = other.fft.fft_size_high;
		this->fft.fft_size_high = other.fft.fft_size_high;
		this->fft.low_high_exponent_low = other.fft.low_high_exponent_low;
		this->fft.overamplification_multiplier_low = other.fft.overamplification_multiplier_low;
		this->fft.low_high_exponent_high = other.fft.low_high_exponent_high;
		this->fft.overamplification_multiplier_high = other.fft.overamplification_multiplier_high;
		this->fft.tuning = other.fft.tuning;
		this->fft.fft_size_low = other.fft.fft_size_low;
		this->fft.fft_size_high = other.fft.fft_size_high;
		this->fft.hop_size = other.fft.hop_size;
		this->fft.subdivision = other.fft.subdivision;
		this->fft.use_2_ffts = other.fft.use_2_ffts;

		this->note_recognition.minimum_length = other.note_recognition.minimum_length;
		this->note_recognition.note_on_threshold = other.note_recognition.note_on_threshold;
		this->note_recognition.note_off_threshold = other.note_recognition.note_off_threshold;
		this->note_recognition.octave_removal_multiplier = other.note_recognition.octave_removal_multiplier;
		this->note_recognition.volume_multiplier = other.note_recognition.volume_multiplier;
		this->note_recognition.percussion_removal = other.note_recognition.percussion_removal;

		this->general.triangle_follows_cursor = other.general.triangle_follows_cursor;
		this->general.hide_notes = other.general.hide_notes;
		this->general.screen_follows_cursor = other.general.screen_follows_cursor;
		this->general.sound_source_option = other.general.sound_source_option;

		this->misc.file_path = other.misc.file_path;
	}

	ASAOptions operator=(const ASAOptions& other)
	{
		return ASAOptions(other);
	}

	inline bool operator==(const ASAOptions& other) const
	{
		return other.fft == this->fft
			&& other.note_recognition == this->note_recognition
			&& other.general == this->general
			&& other.misc == this->misc
			&& other.id == this->id
			&& other.sprites.size() == this->sprites.size()
			&& other.sprites.id() == this->sprites.id();
	}
	inline bool operator!=(const ASAOptions& other) const
	{
		return other.fft != this->fft
			|| other.note_recognition != this->note_recognition
			|| other.general != this->general
			|| other.misc != this->misc
			|| other.id != this->id
			|| other.sprites.size() != this->sprites.size()
			|| other.sprites.id() != this->sprites.id();
	}
};

#endif //ASA_RESOURCE_H
