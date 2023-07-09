#include "advanced_spectrum_analyzer.h"
#include "scene/gui/view_panner.h"
#include "../sfizz/src/external/kiss_fft/kiss_fft.h"
#if TOOLS_ENABLED
#include <editor/progress_dialog.h>
#include <editor/editor_settings.h>
#endif // TOOLS_ENABLED

// ===========================================================================
// Advanced Spectrum Analyzer
//static ASA* singleton = nullptr;
float ASA::remap(const float input_min, const float input_max, const float value, const float output_min, const float output_max) {
	//inverse lerp and lerp from
	//https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/inverse-lerp-a-super-useful-yet-often-overlooked-function-r5230/
	const float ratio = (value - input_min) / (input_max - input_min);
	return (1.0f - ratio) * output_min + ratio * output_max;
}

std::vector<float> ASA::convert_magnitudes_to_exponential_frequencies(const PackedFloat32Array& linear_magnitudes, const float low_high_exponent, const int subdivision, const float tuning, const float frequency_to_count_ratio, float& max_magnitude) {
	//frequency_to_count_ratio = sample_rate / fft_size, because count * frequency_to_count_ratio = frequency
	//now we apply a conversion from linear frequencies to exponential
	//meaning every +x rows mean *y frequencies, not +z frequencies
	//a note for those who know what they are doing: this is my Frankenstein thing, and
	//I made this "algorithm" from my "horse sense"
	//I didn't do much research on it nor do I understand stuff like discrete cosine transform
	//but this code should do something remotely similar to this:
	//https://en.wikipedia.org/wiki/Mel-frequency_cepstrum

	std::vector<float> exponential_magnitudes;
	for (int note = 0; note < 128; note++) {
		//midi note 69 is 440hz (or tuning for custom), and every +12 midi note is *2 in frequency
		//this is the (central) frequency of the note
		const float note_frequency = tuning * pow(2, (note - 69) / 12.0f);
		for (int sub = ceil(-subdivision / 2.0f); sub < ceil(subdivision / 2.0f); sub++) {
			//this is the central frequency of the current subdivision
			const float frequency = note_frequency * pow(2, sub / 12.0f / subdivision);
			//this is the bottom frequency of the current subdivision, it is halfway between the
			//bottom neighbour frequency and this frequency (beween their center) (not linearly, geometrically/exponentially)
			const float from_frequency = frequency / pow(2, 1 / 24.0f / subdivision);
			//same but the upper limit
			const float to_frequency = frequency * pow(2, 1 / 24.0f / subdivision);
			//these correspond to the indexes of the FFT output (count of floats)
			int begin_count = ceil(from_frequency / frequency_to_count_ratio);
			int end_count = ceil(to_frequency / frequency_to_count_ratio);

			float sum = 0.0f;
			if (begin_count == end_count) {
				//for low notes we lerp between the 2 closest values
				begin_count--;
				const float begin_frequency = begin_count * frequency_to_count_ratio;
				const float end_frequency = end_count * frequency_to_count_ratio;
				sum = remap(begin_frequency, end_frequency, frequency, linear_magnitudes[begin_count], linear_magnitudes[end_count]);
			}
			else {
				//for high notes we add all the in-range values together
				for (int i = begin_count; i < end_count; i++) {
					sum += linear_magnitudes[i];
				}
			}
			//compensation, because we can only add whole numbers so
			//we need to smooth the border between where 1 and where 2 or more are added
			//and also magnitudes are probably 2* as loud 1 octave lower
			//because there are /2 less samples/frequencies analyzed
			//but it is configurable, because people might need upper harmonics less
			//the /440 is chosen (frequency of A4), because it's almost in the middle (note 69 out of 0-127)
			sum *= pow(frequency / 440.0f, low_high_exponent) / (end_count - begin_count);
			if (sum > max_magnitude) {
				max_magnitude = sum;
			}
			exponential_magnitudes.push_back(sum);
		}
	}
	return exponential_magnitudes;
}

std::vector<std::vector<float>> ASA::analyze_subspectrum(PackedFloat32Array samples, const int sample_rate, const int fft_size, const float low_high_exponent, const float overamplification_multiplier, const int hop_size, const int subdivision, const float tuning) {
	//for a short overall description of what's happening (at least partially): https://stackoverflow.com/a/4678313
	print_line("Calculating subspectrum with parameters:");

	//samples.insert(0, silence);
	//samples.insert(samples.size() - 1, silence);

	//we don't need all the frequencies, we only need them below 14000 Hz, because the others are above the highest midi note
	const int frequency_limit_count = 14000 * fft_size / sample_rate;
	const int sample_size = samples.size();
	print_line("FFT size is " + String::num(fft_size, 0));
	print_line("Hop size is " + String::num(hop_size, 0));
	print_line("Sample size is " + String::num(sample_size, 0));
	print_line("Frequency limit count is " + String::num(frequency_limit_count, 0));

	//we go through the whole wave file, and we calculate an fft with the size of fft_size, after jumping by hop_size
	float max_magnitude = 0.0f;
	std::vector<std::vector<float>> return_magnitudes;
	kiss_fft_cfg cfg = kiss_fft_alloc(fft_size, 0, 0, 0);
	kiss_fft_cpx *cx_in = new kiss_fft_cpx[fft_size];
	kiss_fft_cpx *cx_out = new kiss_fft_cpx[fft_size];
	for (int i = 0; i < fft_size; i++) {
		cx_in[i].i = 0.0f;
	}
	for (int current_position = 0; current_position < sample_size - fft_size; current_position += hop_size) {

		for (int i = 0; i < fft_size; i++) {
			//Hann window function, source: https://github.com/Kryszak/AudioSpectrum/blob/master/Mp3Player.cpp#L114
			cx_in[i].r = samples[current_position + i] * 0.5f * (1 - cos(2 * Math_PI * i / (fft_size - 1)));
		}
		kiss_fft(cfg, cx_in, cx_out);
		PackedFloat32Array linear_magnitudes;
		linear_magnitudes.resize(frequency_limit_count);
		unsigned ctn = 0;
		for (auto &lm : linear_magnitudes) {
			lm  = sqrt(cx_out[ctn].r * cx_out[ctn].r + cx_out[ctn].i * cx_out[ctn].i);
			ctn++;
		}
		std::vector<float> exponential_magnitudes = convert_magnitudes_to_exponential_frequencies(linear_magnitudes, low_high_exponent, subdivision, tuning, float(sample_rate) / fft_size, max_magnitude);
		return_magnitudes.push_back(exponential_magnitudes);
	}
	//because of unknown reasons using kiss_fft_free throws an error
	//but this should be here according to everything, mainly https://github.com/mborgerding/kissfft#usage
	//so the absence of this probably causes some memory leak or something
	//the strange thing is when I used this outside of GDNative, it worked just fine
	//kiss_fft_free(cfg)
	//they want me to use this, which I don't know if it's good, but it works so far
	::free(cfg);

	//we don't cap magnitudes at 1, they will be capped after the merge
	max_magnitude /= overamplification_multiplier;
	const int height = return_magnitudes[0].size();
	for (auto& tick_magnitudes : return_magnitudes) {
		for (int i = 0; i < height; i++) {
			tick_magnitudes[i] /= max_magnitude;
		}
	}
	print_line("Size of the subdata is " + String::num(return_magnitudes.size(), 0) + String::num(height, 0));
	return return_magnitudes;
}

void ASA::analyze_spectrum(ASAOptions* p_analysis, const bool p_force_recalc) {
	ERR_FAIL_COND(p_analysis == nullptr);

	float len_s = p_analysis->audio_stream->get_length();
	if (len_s == 0) {
		len_s = 60 * 5; //five minutes
	}

	int frames = AudioServer::get_singleton()->get_mix_rate() * len_s;

	Vector<uint8_t> maxmin;
	int pw = frames / 20;
	maxmin.resize(pw * 2);
	{
		uint8_t* ptr = maxmin.ptrw();
		for (int i = 0; i < pw * 2; i++) {
			ptr[i] = 127;
		}
	}
	p_analysis->length = len_s;

	if (p_analysis->audio_stream_playback.is_valid()) {
		p_analysis->thread = memnew(Thread);
		p_analysis->thread->set_name("AudioStreamAnalyzer");
		ProgressDialog::get_singleton()->add_task("analyze_audio", TTR("Analyzing Audio"), 100);
		_analyze_thread(p_analysis);
		//p_analysis->thread->start(_analyze_thread, p_analysis);
		//p_analysis->thread->wait_to_finish();
		ProgressDialog::get_singleton()->end_task("analyze_audio");
	}
	//return p_analysis->spectrum;
}

void ASA::_analyze_thread(void* p_analysis) {
	ASAOptions* analysis = static_cast<ASAOptions*>(p_analysis);
	float muxbuff_chunk_s = 0.25;

	int frames_total = AudioServer::get_singleton()->get_mix_rate() * analysis->length;

	PackedFloat32Array _samples;
	PackedFloat32Array samples;
	PackedFloat32Array zeroed_half_fft;
	_samples.resize_zeroed(frames_total);
	zeroed_half_fft.resize_zeroed(analysis->fft.fft_size_low / 2);
	//samples.resize_zeroed(frames_total + analysis->fft.fft_size_low);
	//adding silence to the beginning and to the end
	//PackedFloat32Array silence;
	for (int i = 0; i < analysis->fft.fft_size_low / 2; i++) {
		samples.insert(0, 0.0f);
		samples.push_back(0.0f);
	}

	int mixbuff_chunk_frames = AudioServer::get_singleton()->get_mix_rate() * muxbuff_chunk_s;

	Vector<AudioFrame> mix_chunk;
	mix_chunk.resize(frames_total); // switched back from AudioServer::get_singleton()->thread_get_mix_buffer_size() RJ 12.06.23

	ProgressDialog::get_singleton()->task_step("analyze_audio", TTR("Reading data"), 0);
	analysis->audio_stream_playback->start();
	analysis->audio_stream_playback->mix(mix_chunk.ptrw(), 1.0, uint64_t(frames_total));

	ProgressDialog::get_singleton()->task_step("analyze_audio", TTR("Reading data"), 10);
	/*for (int i = 0; i < mix_chunk.size(); i++) {
		encoded_audio.push_back(bytes[i]);
	}*/

	//averaging all channels so samples become mono
	{
		unsigned ctn = 0;
		for (auto& s : _samples)
		{
			s = mix_chunk[ctn].l;
			s += mix_chunk[ctn++].r;
			s /= 2;
		}
	}
	samples.append_array(zeroed_half_fft);
	samples.append_array(_samples);
	samples.append_array(zeroed_half_fft);

	//for (int i = 0; i < mix_chunk.size(); i++) {
	//	samples.set(i, (mix_chunk[i].l + mix_chunk[i].r) / 2);
	ProgressDialog::get_singleton()->task_step("analyze_audio", TTR("Reading data"), 20);
	//}


	//use libnyquist to get the raw audio data
	/*std::shared_ptr<nqr::AudioData> file_data = std::make_shared<nqr::AudioData>();
	nqr::NyquistIO loader;
	loader.Load(file_data.get(), encoded_audio);*/

	const int sample_rate = AudioServer::get_singleton()->get_mix_rate();
	const int original_sample_size = frames_total * 2;
	const int channel_count = 2;
	const int sample_size = frames_total;
	//using cout because I couldn't find an easy way to print ints with Godot::print
	print_line("Sample rate is " + String::num(sample_rate, 0));
	print_line("Original sample size is " + String::num(original_sample_size, 0));
	print_line("Channel count is " + String::num(channel_count, 0));
	//now we call analyze_subspectrum, once or twice, and then normalize the magnitudes
	std::vector<std::vector<float>> magnitudes_low = analyze_subspectrum(samples, sample_rate,
																		analysis->fft.fft_size_low,
																		analysis->fft.low_high_exponent_low,
																		analysis->fft.overamplification_multiplier_low,
																		analysis->fft.hop_size,
																		analysis->fft.subdivision,
																		analysis->fft.tuning);
	if (analysis->fft.use_2_ffts) {
		ProgressDialog::get_singleton()->task_step("analyze_audio", TTR("HQ FFT Analysis"), 20);
		std::vector<std::vector<float>> magnitudes_high = analyze_subspectrum(samples, sample_rate,
			analysis->fft.fft_size_high,
			analysis->fft.low_high_exponent_high,
			analysis->fft.overamplification_multiplier_high,
			analysis->fft.hop_size,
			analysis->fft.subdivision,
			analysis->fft.tuning);

		if (magnitudes_low.size() != magnitudes_high.size() || magnitudes_low[0].size() != magnitudes_high[0].size()) {
			print_line("Warning! Magnitudes sizes unequal!");
		}
		const int width = MIN(magnitudes_low.size(), magnitudes_high.size());
		const int height = MIN(magnitudes_low[0].size(), magnitudes_high[0].size());

		analysis->magnitudes.clear();
		unsigned ctn = 0;
		for (int x = 0; x < width; x++) {
			Vector<float> merged_magnitudes;
			merged_magnitudes.resize(height);
			unsigned y = 0;
			for (auto &mm : merged_magnitudes) {
			//for (int y = 0; y < height; y++) {
				const float normalized = y / (height - 1.0f);
				//we're using an easing called easeInOutExpo to interpolate between the low and high ffts
				//code source: https://easings.net/en#easeInOutExpo
				const float weight = normalized < 0.5f ? pow(2, 20 * normalized - 10) / 2 : (2 - pow(2, -20 * normalized + 10)) / 2;
				const float magnitude = (1.0f - weight) * magnitudes_low[x][y] + weight * magnitudes_high[x][y];
				mm = MIN(magnitude, 1.0f);
				y++;
			}
			ProgressDialog::get_singleton()->task_step("analyze_audio", TTR("HQ FFT Analysis"), 80.0 / (double)(height) * ctn++ + 20);
			analysis->magnitudes.push_back(merged_magnitudes);
		}
	}
	else {
		//we sadly need to loop through once again even if we only do one fft, I'm not that concerned with performance
		//knowing that it's much better than gdscript anyway, but I hope that these aren't slowing things down terribly
		//and by that I mean that hopefully this takes at most 0.1 s
		//we need to loop through again to clamp the magnitudes at 1, because we can't do it in analyze_subspectrum,
		//because we need the overamplified values when there are 2 ffts
		ProgressDialog::get_singleton()->task_step("analyze_audio", TTR("FFT Analysis"), 10);
		analysis->magnitudes.resize(magnitudes_low.size());
		unsigned ctn = 0;
		for (auto& ml : magnitudes_low)
			magnitudes_low[ctn++] = ml;

		const int height = analysis->magnitudes[0].size();
		ctn = 0;
		for (auto& tick_magnitudes : analysis->magnitudes) {
			for (auto& h : tick_magnitudes) {
				if (h > 1.0f) {
					h = 1.0f;
				}
			}
			ProgressDialog::get_singleton()->task_step("analyze_audio", TTR("FFT Analysis"), 80.0 / (double)(analysis->magnitudes.size()) * ctn++ + 20);
		}
	}

	print_line("Size of the data is " + String::num(analysis->magnitudes.size(), 0) + " by " + String::num(analysis->magnitudes[0].size(), 0));
	analysis->spectrum.append(sample_rate);
	analysis->spectrum.append(analysis->magnitudes.size());
	analysis->spectrum.append(analysis->magnitudes[0].size());
}

void ASA::generate_images(ASAOptions* p_analysis) {
	//ERR_FAIL_COND_V(p_analysis->thread == nullptr, Array());
	_generate_images(p_analysis);
	//p_analysis->thread->start(_generate_images, p_analysis);
	//p_analysis->thread->wait_to_finish();
	//return p_analysis->images;
}

void ASA::_generate_images(void* p_analysis) {
	ASAOptions* analysis = static_cast<ASAOptions*>(p_analysis);

	std::vector<std::vector<float>> converted_magnitudes;
	converted_magnitudes.resize(analysis->magnitudes.size());
	unsigned ctn = 0;

	analysis->images.clear();
	// needs to be optimied to Vector<Vector<<float>>() instead std::vector<std::vector<float>>()
	for (auto &cm : converted_magnitudes)
	{
		const PackedFloat32Array& am = analysis->magnitudes[ctn++];
		cm.resize(am.size());
		unsigned internal_ctn = 0;
		for (auto& c : cm)
			c = am[internal_ctn++];
	}

	//we need to split the image into multiple parts, because Godot's Image width (and height) is max 16384 (=Image::MAX_WIDTH)
	const int max_width = Image::MAX_WIDTH;
	const int number_of_images = converted_magnitudes.size() / max_width;
	for (int image_count = 0; image_count <= number_of_images; image_count++) {
		const int width = image_count == number_of_images ? converted_magnitudes.size() - max_width * image_count : max_width;
		std::vector<std::vector<float>> current_magnitudes(converted_magnitudes.begin() + max_width * image_count,
			converted_magnitudes.begin() + max_width * image_count + width);
		const int height = current_magnitudes[0].size();
		print_line("Creating image with width " + String::num(width, 0) + " and height " + String::num(height, 0));
		PackedByteArray image_data;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Color color = analysis->color_scheme->get_color_at_offset(current_magnitudes[x][y]);
				image_data.append(color.get_r8());
				image_data.append(color.get_g8());
				image_data.append(color.get_b8());
				image_data.append(color.get_a8());
			}
		}
		//there's also Image* but they say this is better and I actually got it working
		Ref<Image> image = Image::create_from_data(width, height, false, Image::FORMAT_RGBA8, image_data);
		//we want low pitch to be down and high pitch to be up
		image->flip_y();
		analysis->images.append(image);
	}
	print_line("Returning " + String::num(analysis->images.size(), 0) + " images");
}

PackedInt32Array ASA::guess_notes(ASAOptions* p_options) {
	print_line("Guessing notes...");
	const int subdivision = p_options->magnitudes[0].size() / 128;
	Vector<PackedFloat32Array> note_strengths;
	for (auto& current_magnitudes : p_options->magnitudes) {
		PackedFloat32Array these_strengths;
		for (int note = 0; note < 128; note++) {
			float magnitude = 0;
				for (int i = 0; i < subdivision; i++) {
					//we want to calculate a window-like thing, as we want the "inner" subdivisions to matter more
					//then the "outer" ones, this is just an absolute value thing so the window will look
					//something like this:  /\  with a peak of 3 and the sides being 1
					magnitude += current_magnitudes[note * subdivision + i] * (3.0f - abs(i * 4.0f / (subdivision - 1) - 2.0f));
				}
			// we divide by 2 so that the maximum value becomes 1, because it ranges [0;2]
			magnitude /= 2.0f;
				if (note > 0) {
					for (int i = 0; i < subdivision; i++) {
						//same thing, but now it looks like this:  /\  with a peak of 0 and the sides being -1
						//the purpose of this is to kinda exclude if there's a real note here, otherwise the goal of this whole thing is
						//to exclude drums and other "noise" that spreads across multiple notes, so if we subtract the magnitudes from
						//one note above and from one note below, from the current one, we get a rough idea if there's actually a note here
						magnitude += current_magnitudes[(note - 1) * subdivision + i] * (-abs(i * 2.0f / (subdivision - 1) - 1.0f)) * p_options->note_recognition.percussion_removal;
					}
				}
			if (note < 127) {
				for (int i = 0; i < subdivision; i++) {
					//same thing but for one note higher
					//notice that lower magnitudes and higher magnitudes < 0 so we add them, but they only compensate negatively
					magnitude += current_magnitudes[(note + 1) * subdivision + i] * (-abs(i * 2.0f / (subdivision - 1) - 1.0f)) * p_options->note_recognition.percussion_removal;
				}
			}
			//we're clamping it at 0, meaning it can't be negative even when the surrounding notes are louder then the current one
			these_strengths.push_back(MAX(magnitude / subdivision, 0.0f));
		}
		note_strengths.push_back(these_strengths);
	}

	//now we run a second round which actually decides where note ons and offs should be
	//and optionally subtracts a fraction of the one octave lower magnitude
	const int width = note_strengths.size();
	//the format of notes: 4 ints represent a note, I just can't return it better
	//in the following order: begin_tick, end_tick, note, velocity, next_begin_tick, etc...
	PackedInt32Array notes;
	for (int note = 0; note < 128; note++) {
		bool is_note_playing = false;
		int note_begin_tick = -1;
		float peak_magnitude = 0.0f;
		for (int tick = 0; tick < width; tick++) {
			float magnitude = note_strengths[tick][note];
			if (note >= 12) {
				magnitude -= note_strengths[tick][note - 12] * p_options->note_recognition.octave_removal_multiplier;
			}
			if (!is_note_playing && magnitude >= p_options->note_recognition.note_on_threshold) {
				is_note_playing = true;
				note_begin_tick = tick;
			}
			if (is_note_playing && magnitude > peak_magnitude) {
				peak_magnitude = magnitude;
			}
			if (is_note_playing && magnitude <= p_options->note_recognition.note_off_threshold) {
				is_note_playing = false;
				//we only add the note if it exceeds the minimum length given
				//because we don't want to clutter the image with many short notes
				if (tick - note_begin_tick >= p_options->note_recognition.minimum_length) {
					notes.append(note_begin_tick);
					notes.append(tick);
					notes.append(note);
					notes.append(MIN(int(peak_magnitude * 128 * p_options->note_recognition.volume_multiplier + 0.5f), 127));
				}
				note_begin_tick = -1;
				peak_magnitude = 0.0f;
			}
		}
		//if we run out of width but there's still a note on
		if (is_note_playing && width - note_begin_tick >= p_options->note_recognition.minimum_length) {
			notes.append(note_begin_tick);
			notes.append(width);
			notes.append(note);
			notes.append(MIN(int(peak_magnitude * 128 * p_options->note_recognition.volume_multiplier + 0.5), 127));
		}
	}
	print_line(String::num(notes.size() / 4, 0) + " notes guessed");
	return notes;
}

void ASA::_bind_methods() {
	/*ClassDB::bind_method(D_METHOD("analyze_spectrum", ), &ASA::analyze_spectrum);
	ClassDB::bind_method(D_METHOD("generate_images"), &ASA::generate_images);

	ClassDB::bind_method("_update_emit", &ASA::_update_emit);

	ADD_SIGNAL(MethodInfo("preview_updated", PropertyInfo(Variant::INT, "obj_id")));*/
}

ASA::ASA() {
}


// ===========================================================================
// ASANode
void ASANode::_notification(int p_what) {
	switch (p_what) {
	case NOTIFICATION_PROCESS: {
		List<ObjectID> to_erase;
		for (KeyValue<ObjectID, ASAOptions>& E : analyzes) {
			if (!E.value.generating.is_set()) {
				if (E.value.thread) {
					E.value.thread->wait_to_finish();
					memdelete(E.value.thread);
					E.value.thread = nullptr;
				}
				if (!ObjectDB::get_instance(E.key)) { //no longer in use, get rid of preview
					to_erase.push_back(E.key);
				}
			}
		}

		while (to_erase.front()) {
			analyzes.erase(to_erase.front()->get());
			to_erase.pop_front();
		}
	} break;
	}
}

List<Sprite2D*> ASANode::_generate_images(ASAOptions* p_analysis)
{
	ERR_FAIL_COND_V(analyzer.is_null(), List<Sprite2D*>());
	ERR_FAIL_COND_V(color_scheme.is_null(), List<Sprite2D*>());
	p_analysis->color_scheme = color_scheme;
	//Array images = analyzer->generate_images(p_analysis);
	analyzer->generate_images(p_analysis);

	List<Sprite2D*> spectrum_sprites;
	for (auto i = 0; i < p_analysis->images.size(); ++i) {
		const Ref<Image> &image = p_analysis->images[i];
		Ref<ImageTexture> image_texture = ImageTexture::create_from_image(image);
		Sprite2D* spectrum_sprite = memnew(Sprite2D);
		spectrum_sprite->set_centered(false);
		spectrum_sprite->set_texture(image_texture);
		spectrum_sprites.push_back(spectrum_sprite);
	}
	p_analysis->sprites = spectrum_sprites;
	return spectrum_sprites;
}

List<Sprite2D*>* ASANode::get_sprites(const ObjectID& id)
{
	if (analyzes.has(id))
		return &analyzes[id].sprites;

	return nullptr;
}
Array ASANode::get_images(const ObjectID& id) const
{
	if (analyzes.has(id))
		return analyzes[id].images;

	return Array();
}

PackedInt32Array ASANode::guess_notes(const ObjectID& id)
{
	ERR_FAIL_COND_V(analyzer.is_null(), PackedInt32Array());
	ERR_FAIL_COND_V(analyzes.has(id) == false, PackedInt32Array());

	PackedInt32Array result = analyzer->guess_notes(&analyzes[id]);
	return result;
}

List<Sprite2D*> ASANode::analyze_spectrum_sprites(const Ref<AudioStream>& p_audio_stream)
{
	ASAOptions* analysis = _analyze_spectrum(p_audio_stream);
	if (analysis)
	{
		return analyzes[p_audio_stream->get_instance_id()].sprites;
	}
	return List<Sprite2D*>();
}
Array ASANode::analyze_spectrum_images(const Ref<AudioStream>& p_audio_stream)
{
	ASAOptions* analysis = _analyze_spectrum(p_audio_stream);
	if (analysis)
	{
		return analyzes[p_audio_stream->get_instance_id()].images;
	}
	return Array();
}
ASAOptions* ASANode::_analyze_spectrum(const Ref<AudioStream>& p_audio_stream)
{
	ERR_FAIL_COND_V(analyzer.is_null(), nullptr);
	ERR_FAIL_COND_V(color_scheme.is_null(), nullptr);
	ERR_FAIL_COND_V(p_audio_stream.is_null(), nullptr);

	ASAOptions* analysis = nullptr;
	if (analyzes.has(p_audio_stream->get_instance_id()))
	{
		analysis = &analyzes[p_audio_stream->get_instance_id()];
	}
	else
	{
		analysis = memnew(ASAOptions);
		analyzes[p_audio_stream->get_instance_id()] = *analysis;
	}
	analysis->audio_stream = p_audio_stream;
	analysis->audio_stream_playback = analysis->audio_stream->instantiate_playback();
	analysis->generating.set();
	analysis->id = p_audio_stream->get_instance_id();

	//print_line("Loading audio file ", options.misc.file_path);
	//Ref<FileAccess> file;
	//file.instantiate();
	//file->open(options.misc.file_path, FileAccess::READ);
	//PackedByteArray bytes = file->get_buffer(file->get_length());
	//bool recalc = last_options.has(p_audio_stream->get_instance_id()) && last_options[p_audio_stream->get_instance_id()] != options;
	//Array result = analyzer->analyze_spectrum(analysis, false);
	analyzer->analyze_spectrum(analysis, false);
	set_sample_rate(analysis->spectrum[0]);
	set_texture_size(Vector2(analysis->spectrum[1], analysis->spectrum[2]));
	_generate_images(analysis);
	return analysis;
}

void ASANode::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_analyzer", "analyzer"), &ASANode::set_analyzer);
	ClassDB::bind_method(D_METHOD("get_analyzer"), &ASANode::get_analyzer);
	ClassDB::bind_method(D_METHOD("set_sample_rate", "sample_rate"), &ASANode::set_sample_rate);
	ClassDB::bind_method(D_METHOD("get_sample_rate"), &ASANode::get_sample_rate);
	ClassDB::bind_method(D_METHOD("set_texture_size", "texture_size"), &ASANode::set_texture_size);
	ClassDB::bind_method(D_METHOD("get_texture_size"), &ASANode::get_texture_size);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "analyzer"), "set_analyzer", "get_analyzer");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "texture_size"), "set_texture_size", "get_texture_size");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sample_rate"), "set_sample_rate", "get_sample_rate");
}

ASANode::ASANode()
{
	//source of current color scheme (purple-red-yellow-white):
	//https://sourceforge.net/projects/isse/
	//and Ubuntu's color scheme
	color_scheme.instantiate();
	color_scheme->set_offsets({ 0, 0.088889, 0.316667, 0.638889, 1.0 });
	color_scheme->set_colors({ {0.25098, 0.243137, 0.282353, 0.0}, {0.466667, 0.129412, 0.435294, 0.5}, {0.866667, 0.282353, 0.0784314, 1.0 }, { 1.0, 1.0, 0.0, 1.0},{ 1.0, 1.0, 1.0, 1.0} });
	analyzer.instantiate();
}

int Timeline::get_name_limit() const {
	return 0;
}

void Timeline::gui_input(const Ref<InputEvent>& p_event) {
	ERR_FAIL_COND(p_event.is_null());

	if (panner->gui_input(p_event)) {
		accept_event();
		return;
	}

	const Ref<InputEventMouseButton> mb = p_event;

	if (mb.is_valid() && mb->is_pressed() && mb->is_alt_pressed() && mb->get_button_index() == MouseButton::WHEEL_UP) {
		accept_event();
	}

	if (mb.is_valid() && mb->is_pressed() && mb->is_alt_pressed() && mb->get_button_index() == MouseButton::WHEEL_DOWN) {
		accept_event();
	}

	if (mb.is_valid() && mb->is_pressed() && mb->get_button_index() == MouseButton::LEFT && hsize_rect.has_point(mb->get_position())) {
		dragging_hsize = true;
		dragging_hsize_from = mb->get_position().x;
		dragging_hsize_at = name_limit;
	}

	if (mb.is_valid() && !mb->is_pressed() && mb->get_button_index() == MouseButton::LEFT && dragging_hsize) {
		dragging_hsize = false;
	}
	if (mb.is_valid() && mb->get_position().x > get_name_limit() && mb->get_position().x < (get_size().width)) {
		if (!panner->is_panning() && mb->get_button_index() == MouseButton::LEFT) {
			int x = mb->get_position().x - get_name_limit();

			float ofs = x / get_zoom_scale() + get_value();
			emit_signal(SNAME("timeline_changed"), ofs, false, mb->is_alt_pressed());
			dragging_timeline = true;
		}
	}

	if (dragging_timeline && mb.is_valid() && mb->get_button_index() == MouseButton::LEFT && !mb->is_pressed()) {
		dragging_timeline = false;
	}

	Ref<InputEventMouseMotion> mm = p_event;

	if (mm.is_valid()) {
		if (dragging_hsize) {
			int ofs = mm->get_position().x - dragging_hsize_from;
			name_limit = dragging_hsize_at + ofs;
			queue_redraw();
			emit_signal(SNAME("name_limit_changed"));
			play_position->queue_redraw();
		}
		if (dragging_timeline) {
			int x = mm->get_position().x - get_name_limit();
			float ofs = x / get_zoom_scale() + get_value();
			emit_signal(SNAME("timeline_changed"), ofs, false, mm->is_alt_pressed());
		}
	}
}

void Timeline::_pan_callback(Vector2 p_scroll_vec, Ref<InputEvent> p_event) {
	set_value(get_value() - p_scroll_vec.x / get_zoom_scale());
}

void Timeline::_zoom_callback(float p_zoom_factor, Vector2 p_origin, Ref<InputEvent> p_event) {
	double current_zoom_value = get_zoom()->get_value();
	get_zoom()->set_value(MAX(0.01, current_zoom_value * p_zoom_factor));
}

void Timeline::_zoom_changed(double) {
	queue_redraw();
	play_position->queue_redraw();
	emit_signal(SNAME("zoom_changed"));
}

float Timeline::get_zoom_scale() const {
	float zv = zoom ? zoom->get_max() - zoom->get_value() : 1.0f;
	if (zv < 1) {
		zv = 1.0 - zv;
		return Math::pow(1.0f + zv, 8.0f) * 100;
	}
	else {
		return 1.0 / Math::pow(zv, 8.0f) * 100;
	}
}

float Timeline::get_active_width() const {
	float key_range = get_size().width - get_name_limit();

	int zoomw = key_range;
	float scale = get_zoom_scale();
	int h = get_size().height;

	float l = current_audio_stream.is_valid() ? current_audio_stream->get_length() : 1.0;
	if (l <= 0) {
		l = 0.0001; // Avoid crashor.
	}

	float end_px = (l - get_value()) * scale;
	float begin_px = -get_value() * scale;

	return end_px - begin_px;
}

void Timeline::set_zoom(Range* p_zoom) {
	zoom = p_zoom;
	zoom->connect("value_changed", callable_mp(this, &Timeline::_zoom_changed));
}

void Timeline::_play_position_draw() {
	if (!current_audio_stream.is_valid() || play_position_pos < 0) {
		return;
	}

	float scale = get_zoom_scale();
	int h = get_size().height;

	int px = (-get_value() + play_position_pos) * scale + get_name_limit();

	if (px >= get_name_limit() && px < (get_size().width)) {
		Color color = get_theme_color(SNAME("accent_color"), SNAME("Editor"));
		play_position->draw_line(Point2(px, 0), Point2(px, h), color, Math::round(2 * EDSCALE));
	}
}

void Timeline::_length_changed(double p_new_len) {
	if (editing) {
		return;
	}
	p_new_len = MAX(0.0001, p_new_len);
	
	queue_redraw();
	emit_signal(SNAME("length_changed"), p_new_len);
}

void Timeline::set_play_position(float p_pos) {
	play_position_pos = p_pos;
	play_position->queue_redraw();
}

float Timeline::get_play_position() const
{
	return play_position_pos;
}

void Timeline::update_play_position() {
	play_position->queue_redraw();
}

void Timeline::update_values() {
	if (editing) {
		return;
	}

	editing = true;
	//
	
	//
	editing = false;
}

void Timeline::set_use_fps(bool p_use_fps) {
	use_fps = p_use_fps;
	queue_redraw();
}

bool Timeline::is_using_fps() const {
	return use_fps;
}

void Timeline::set_current_stream(const Ref<Resource>& p_resource)
{
	if (p_resource.is_null())
		return;

	if (p_resource->is_class("AudioStream"))
	{
		current_audio_stream = p_resource;
	}
}

Ref<Resource> Timeline::get_current_stream()
{
	if (current_audio_stream.is_valid())
		return current_audio_stream;

	return current_audio_stream;
}
void Timeline::set_hscroll(HScrollBar* p_hscroll)
{
	hscroll = p_hscroll;
}

Control::CursorShape Timeline::get_cursor_shape(const Point2& p_pos) const {
	if (dragging_hsize || hsize_rect.has_point(p_pos)) {
		// Indicate that the track name column's width can be adjusted
		return Control::CURSOR_HSIZE;
	}
	else {
		return get_default_cursor_shape();
	}
}

void Timeline::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("zoom_changed"));
	ADD_SIGNAL(MethodInfo("timeline_changed", PropertyInfo(Variant::FLOAT, "position"), PropertyInfo(Variant::BOOL, "drag"), PropertyInfo(Variant::BOOL, "timeline_only")));

	ClassDB::bind_method(D_METHOD("update_values"), &Timeline::update_values);
}

void Timeline::_notification(int p_what) {
	switch (p_what) {
	case NOTIFICATION_ENTER_TREE: {
		play_position->connect("draw", callable_mp(this, &Timeline::_play_position_draw));
	}
	case NOTIFICATION_THEME_CHANGED: {
#if TOOLS_ENABLED
		panner->setup((ViewPanner::ControlScheme)EDITOR_GET("editors/panning/animation_editors_panning_scheme").operator int(), ED_GET_SHORTCUT("canvas_item_editor/pan_view"), bool(EDITOR_GET("editors/panning/simple_panning")));
	} break;

	case EditorSettings::NOTIFICATION_EDITOR_SETTINGS_CHANGED: {
		panner->setup((ViewPanner::ControlScheme)EDITOR_GET("editors/panning/animation_editors_panning_scheme").operator int(), ED_GET_SHORTCUT("canvas_item_editor/pan_view"), bool(EDITOR_GET("editors/panning/simple_panning")));
#endif
	} break;

	case NOTIFICATION_RESIZED: {
	} break;

	case NOTIFICATION_DRAW: {
		int key_range = get_size().width - get_name_limit();


		Ref<Font> font = get_theme_font(SNAME("font"), SNAME("Label"));
		int font_size = get_theme_font_size(SNAME("font_size"), SNAME("Label"));
		Color color = get_theme_color(SNAME("font_color"), SNAME("Label"));

		int zoomw = key_range;
		float scale = get_zoom_scale();
		int h = get_size().height;

		float l = current_audio_stream.is_valid() ? current_audio_stream->get_length() : 1.0;
		if (l <= 0) {
			l = 0.0001; // Avoid crashor.
		}

		{
			float time_min = 0;
			float time_max = l;

			//float extra = (zoomw / scale) * 0.5;

			//time_max += extra;
			set_min(time_min);
			set_max(time_max);

			if (hscroll)
			{
				if (zoomw / scale < (time_max - time_min)) {
					hscroll->show();

				}
				else {
					hscroll->hide();
				}
			}
		}

		set_page(zoomw / scale);

		int end_px = (l - get_value()) * scale;
		int begin_px = -get_value() * scale;
		Color notimecol = get_theme_color(SNAME("dark_color_2"), SNAME("Editor"));
		Color timecolor = color;
		timecolor.a = 0.2;
		Color linecolor = color;
		linecolor.a = 0.2;

		{
			draw_rect(Rect2(Point2(get_name_limit(), 0), Point2(zoomw - 1, h)), notimecol);

			if (begin_px < zoomw && end_px > 0) {
				if (begin_px < 0) {
					begin_px = 0;
				}
				if (end_px > zoomw) {
					end_px = zoomw;
				}

				draw_rect(Rect2(Point2(get_name_limit() + begin_px, 0), Point2(end_px - begin_px, h)), timecolor);
			}
			//float extra = (zoomw / scale) * 0.5;
		}

		Color color_time_sec = color;
		Color color_time_dec = color;
		color_time_dec.a *= 0.5;
#define SC_ADJ 100
		int dec = 1;
		int step = 1;
		int decimals = 2;
		bool step_found = false;

		const float period_width = font->get_char_size('.', font_size).width;
		float max_digit_width = font->get_char_size('0', font_size).width;
		for (int i = 1; i <= 9; i++) {
			const float digit_width = font->get_char_size('0' + i, font_size).width;
			max_digit_width = MAX(digit_width, max_digit_width);
		}
		const int max_sc = int(Math::ceil(zoomw / scale));
		const int max_sc_width = String::num(max_sc).length() * max_digit_width;

		while (!step_found) {
			int min = max_sc_width;
			if (decimals > 0) {
				min += period_width + max_digit_width * decimals;
			}

			static const int _multp[3] = { 1, 2, 5 };
			for (int i = 0; i < 3; i++) {
				step = (_multp[i] * dec);
				if (step * scale / SC_ADJ > min) {
					step_found = true;
					break;
				}
			}
			if (step_found) {
				break;
			}
			dec *= 10;
			decimals--;
			if (decimals < 0) {
				decimals = 0;
			}
		}

		/*
		if (use_fps) {
			float step_size = animation->get_step();
			if (step_size > 0) {
				int prev_frame_ofs = -10000000;

				for (int i = 0; i < zoomw; i++) {
					float pos = get_value() + double(i) / scale;
					float prev = get_value() + (double(i) - 1.0) / scale;

					int frame = pos / step_size;
					int prev_frame = prev / step_size;

					bool sub = Math::floor(prev) == Math::floor(pos);

					if (frame != prev_frame && i >= prev_frame_ofs) {
						draw_line(Point2(get_name_limit() + i, 0), Point2(get_name_limit() + i, h), linecolor, Math::round(EDSCALE));

						draw_string(font, Point2(get_name_limit() + i + 3 * EDSCALE, (h - font->get_height(font_size)) / 2 + font->get_ascent(font_size)).floor(), itos(frame), HORIZONTAL_ALIGNMENT_LEFT, zoomw - i, font_size, sub ? color_time_dec : color_time_sec);
						prev_frame_ofs = i + font->get_string_size(itos(frame), HORIZONTAL_ALIGNMENT_LEFT, -1, font_size).x + 5 * EDSCALE;
					}
				}
			}

		}
		else */
		{
			for (int i = 0; i < zoomw; i++) {
				float pos = get_value() + double(i) / scale;
				float prev = get_value() + (double(i) - 1.0) / scale;

				int sc = int(Math::floor(pos * SC_ADJ));
				int prev_sc = int(Math::floor(prev * SC_ADJ));
				bool sub = (sc % SC_ADJ);

				if ((sc / step) != (prev_sc / step) || (prev_sc < 0 && sc >= 0)) {
					int scd = sc < 0 ? prev_sc : sc;
					draw_line(Point2(get_name_limit() + i, 0), Point2(get_name_limit() + i, h), linecolor, Math::round(EDSCALE));
					draw_string(font, Point2(get_name_limit() + i + 3, (h - font->get_height(font_size)) / 2 + font->get_ascent(font_size)).floor(), String::num((scd - (scd % step)) / double(SC_ADJ), decimals), HORIZONTAL_ALIGNMENT_LEFT, zoomw - i, font_size, sub ? color_time_dec : color_time_sec);
				}
			}
		}

		draw_line(Vector2(0, get_size().height), get_size(), linecolor, Math::round(EDSCALE));
		update_values();
	} break;
	}
}

Timeline::Timeline()
{
#if TOOLS_ENABLED
	name_limit = 0; // 150 * EDSCALE;
#else
	name_limit = 0; // 150;
#endif

	play_position = memnew(Control);
	play_position->set_mouse_filter(MOUSE_FILTER_PASS);
	add_child(play_position);
	play_position->set_anchors_and_offsets_preset(PRESET_FULL_RECT);

	panner.instantiate();
	panner->set_callbacks(callable_mp(this, &Timeline::_pan_callback), callable_mp(this, &Timeline::_zoom_callback));
	panner->set_pan_axis(ViewPanner::PAN_AXIS_HORIZONTAL);

	set_layout_direction(Control::LAYOUT_DIRECTION_LTR);
}

// ===========================================================================
// Analyzer Graph Container MIDI Stripes
void AudioGraphEditor::_draw()
{
	//draw_rect(get_rect(), Color(0.5, 0.0, 0.0));
	float end = get_size().width;
	//float one_seminote_gap = texture_size.y / 128.0;
	float one_seminote_gap = get_size().height / 128.0;
	for (auto note = 0; note < 128; ++note)
	{
		switch (note % 12)
		{
		case 1:
		case 3:
		case 6:
		case 8:
		case 10:
		{
			// black keys
			float height = (127 - note) * one_seminote_gap + resolution / 2.0;
			draw_line(Vector2(0, height), Vector2(end, height), color, resolution);
		}
		break;
		case 4:
		case 11:
			// b/c or e/f
			draw_line(Vector2(0, (127 - note) * one_seminote_gap), Vector2(end, (127 - note) * one_seminote_gap), color, 2.0);
		default:
			break;
		}
	}
}
void AudioGraphEditor::remove_all_notes()
{
	for (auto m_track : midi_note_tracks)
		m_track.value->remove_notes();
}

void AudioGraphEditor::add_notes(const PackedInt32Array& p_notes)
{
	// filtering which note belings to which
	// track happens inside MIDINoteTrackContainer::add_nodes()
	// function
	for (auto& m_track : midi_note_tracks)
		m_track.value->add_notes(p_notes);
}

void AudioGraphEditor::_notification(int p_what)
{
	switch (p_what)
	{
	case NOTIFICATION_ENTER_TREE:
	{
		break;
	}
	case NOTIFICATION_RESIZED:
	{
		for (auto &m_track : midi_note_tracks)
			m_track.value->update_y_position(get_size().height);
		break;
	}
	case NOTIFICATION_DRAW:
	{
		_draw();
		/*for (auto i = 0; i < 127; ++i)
			draw_rect(Rect2(midi_note_tracks[i]->get_position(), midi_note_tracks[i]->get_position() + Vector2(get_size().width, (double)get_size().height / 128.0)), Color(0.5, 0.0, 0.0, 0.3));*/
		break;
	}
	default:
		break;
	}
}

void AudioGraphEditor::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_color", "color"), &AudioGraphEditor::set_color);
	ClassDB::bind_method(D_METHOD("get_color"), &AudioGraphEditor::get_color);
	//ClassDB::bind_method(D_METHOD("set_texture_size", "texture_size"), &ASAGraphScrollContainerStripes::set_texture_size);
	//ClassDB::bind_method(D_METHOD("get_texture_size"), &ASAGraphScrollContainerStripes::get_texture_size);
	ClassDB::bind_method(D_METHOD("set_resolution", "hop_size"), &AudioGraphEditor::set_resolution);
	ClassDB::bind_method(D_METHOD("get_resolution"), &AudioGraphEditor::get_resolution);

	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "resolution"), "set_resolution", "get_resolution");
	//ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "texture_size"), "set_texture_size", "get_texture_size");
}

AudioGraphEditor::AudioGraphEditor()
{
	for (auto i = 0; i < 127; ++i)
	{
		MIDINoteTrackContainer* midi_note_track = memnew(MIDINoteTrackContainer(i));
		midi_note_tracks[i] = memnew(MIDINoteTrackContainer(i));
	}
}

// ===========================================================================
// Audio To Midi Editor


void AudioToMidiEditor::reset()
{
	/*/if (graph_scroll_container)
	{
		graph_scroll_container->set_h_scroll(0);
		graph_scroll_container->set_v_scroll(0);
	}*/
	
	calculate_new_spectrum_sprites();
	calculate_new_notes();
}

List<Sprite2D*> AudioToMidiEditor::get_spectrum_sprites(const ObjectID& p_id)
{
	// very ugly -> copies all the time -> needs optimization
	if (List<Sprite2D*>* sprites = analyzer_node->get_sprites(p_id))
		return *sprites;
	return List<Sprite2D*>();
}

Array AudioToMidiEditor::get_spectrum_images(const ObjectID& p_id) const
{
	if (analyzer_node)
		return analyzer_node->get_images(p_id);
	return Array();
}

bool AudioToMidiEditor::calculate_new_spectrum_sprites()
{
	if (current_audio_stream.is_null())
		return false;

	stripes->remove_all_notes();
	/*List<Sprite2D*> new_spectrum_sprites = analyzer_node->analyze_spectrum(current_audio_stream);
	List<Sprite2D*> old_spectrum_sprites;
	for (auto i = 0; i < spectrum_sprites->get_child_count(); ++i)
	{
		Sprite2D* spectrum_sprite = Object::cast_to<Sprite2D>(spectrum_sprites->get_child(i));
		if (!spectrum_sprite)
			continue;

		old_spectrum_sprites.push_back(spectrum_sprite);
	}
	for (auto s : old_spectrum_sprites)
	{
		spectrum_sprites->remove_child(s);
		memdelete(s);
	}
	sprites_original_width = 0;
	for (auto i = 0; i < new_spectrum_sprites.size(); ++i)
	{
		Sprite2D* spectrum_sprite = Object::cast_to<Sprite2D>(new_spectrum_sprites[i]);
		if (!spectrum_sprite)
			continue;

		spectrum_sprite->set_position(Point2(sprites_original_width, spectrum_sprite->get_position().y));
		sprites_original_width += spectrum_sprite->get_texture()->get_width();
		spectrum_sprites->add_child(spectrum_sprite);
	}
	*/
	Array images = analyzer_node->analyze_spectrum_images(current_audio_stream);
	
	List<TextureRect*> old_texture_rects;
	for (auto i = 0; i < stripes->get_child_count(); ++i)
	{
		TextureRect* texture_rect = Object::cast_to<TextureRect>(stripes->get_child(i));
		if (!texture_rect)
			continue;

		old_texture_rects.push_back(texture_rect);
	}
	for (auto s : old_texture_rects)
	{
		stripes->remove_child(s);
		memdelete(s);
	}

	for (auto i = 0; i < images.size(); ++i)
	{
		TextureRect* texture_rect = memnew(TextureRect);
		stripes->add_child(texture_rect);
		Ref<ImageTexture> new_texture = ImageTexture::create_from_image(images[i]);
		texture_rect->set_texture(new_texture);
		texture_rect->set_anchors_preset(PRESET_FULL_RECT);
		texture_rect->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
		texture_rect->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
		texture_rect->set_v_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	}

	if (timeline)
		timeline->set_current_stream(current_audio_stream);

	//update_graph_spacer();
	force_update_transform();
	queue_sort();
	queue_redraw();

	if (images.size() < 1)
		return false;

	return true;
}

bool AudioToMidiEditor::calculate_new_notes()
{
	ERR_FAIL_COND_V(current_audio_stream.is_null(), false);
	ERR_FAIL_COND_V(analyzer_node == nullptr, false);
	ERR_FAIL_COND_V(stripes == nullptr, false);

	stripes->remove_all_notes();
	PackedInt32Array guessed_notes = analyzer_node->guess_notes(current_audio_stream->get_instance_id());
	stripes->add_notes(guessed_notes);
	return guessed_notes.size() > 0;
}
/*
void AudioToMidiEditor::zoom(Size2 p_scale)
{
	_zoom_vertical(graph_area->get_scale().y  * p_scale.y);
	//graph_scroll_container->force_update_transform();
}*/

void AudioToMidiEditor::update_zoom()
{
	if (current_audio_stream.is_null())
		return;

	const float pixel_width = timeline->get_active_width();

	ERR_FAIL_COND(graph_scroll_container == nullptr);
	graph_scroll_container->get_h_scroll_bar()->set_max(hscroll->get_max());
	graph_scroll_container->get_h_scroll_bar()->set_min(hscroll->get_min());
	graph_scroll_container->get_h_scroll_bar()->set_value(hscroll->get_value());

	piano_bar->set_custom_minimum_size(Vector2(piano_scroll_container->get_size().x, graph_scroll_container->get_size().y * vertical_zoom));

	stripes->set_custom_minimum_size(Size2(pixel_width, vertical_zoom * graph_scroll_container->get_size().height));
	stripes->set_size(Size2(pixel_width, vertical_zoom * graph_scroll_container->get_size().height));
	
	queue_redraw();
	//_zoom_horizontal(zoom_scale);
}
/*
void AudioToMidiEditor::_zoom_horizontal(float p_zoom_x)
{
	ERR_FAIL_COND(graph_scroll_container == nullptr || graph_hb == nullptr);
	//const float scale_correct = (real_t)sprites_original_width / 1000.0;
	//graph_area->set_scale( Size2(p_zoom_x / scale_correct, MAX(1.0, graph_area->get_scale().y)));
	//update_graph_spacer(Size2(p_zoom_x, 1.0), true, false);
	//graph_scroll_container->force_update_transform();

	piano_bar->set_custom_minimum_size(Vector2(piano_scroll_container->get_size().x, MAX(piano_bar->get_custom_minimum_size().y, piano_scroll_container->get_size().y)));

	stripes->set_custom_minimum_size(Size2(p_zoom_x * (graph_scroll_container->get_size().width), MAX(graph_scroll_container->get_size().height, stripes->get_custom_minimum_size().y)));
	stripes->set_size(Size2(p_zoom_x * (graph_scroll_container->get_size().width), MAX(graph_scroll_container->get_size().height, stripes->get_size().y)));

	queue_redraw();
}
*/
void AudioToMidiEditor::_zoom_vertical_step(bool p_zoom_in)
{
	if (p_zoom_in)
		vertical_zoom *= 1.2;
	else
		vertical_zoom /= 1.2;
	if (vertical_zoom < 1.0)
	{
		vertical_zoom = 1.0;
	}
	update_zoom();
}
/*
void AudioToMidiEditor::_zoom_vertical(float p_zoom_y)
{
	ERR_FAIL_COND(graph_scroll_container == nullptr || graph_area == nullptr);
	graph_area->set_scale(Size2(graph_area->get_scale().x, p_zoom_y));
	update_graph_spacer(Size2(1.0, p_zoom_y), false, true);
	//graph_scroll_container->force_update_transform();
}

void AudioToMidiEditor::update_graph_spacer(Size2 p_scale, bool p_only_x, bool p_only_y)
{
	force_update_transform();

	ERR_FAIL_COND(main == nullptr);
	ERR_FAIL_COND(stripes == nullptr);
	ERR_FAIL_COND(piano_scroll_container == nullptr);

	p_scale.y = MAX(1.0, p_scale.y);

	//ERR_FAIL_COND(timeline_scroll_container == nullptr);

	Size2 min_size;
	if (!current_audio_stream.is_null() && is_analyzed(current_audio_stream->get_instance_id()))
	{
		Size2 new_size = analyzer_node->get_texture_size() * graph_area->get_scale();
		min_size = new_size;
	}
	else
	{
		Size2 current_min_size = get_size() - Size2(5.0, 5.0);
		//graph_scroll_container->set_custom_minimum_size(current_min_size);

		Size2 new_size = analyzer_node->get_texture_size() * graph_area->get_scale();
		min_size.x = new_size.x < current_min_size.x ? current_min_size.x : new_size.x;
		min_size.y = new_size.y < current_min_size.y ? current_min_size.y : new_size.y;
	}
	if (p_only_y)
	{
		min_size = Size2(stripes->get_custom_minimum_size().x, min_size.y);
		stripes->set_custom_minimum_size(Size2(stripes->get_custom_minimum_size().x, p_scale.y * (graph_scroll_container->get_size().height)));
		stripes->set_size(Size2(stripes->get_size().x, min_size.y));
	}
	else if (p_only_x)
	{
		min_size = Size2(min_size.x, stripes->get_custom_minimum_size().y);
		stripes->set_custom_minimum_size(Size2(p_scale.x * (graph_scroll_container->get_size().width), stripes->get_custom_minimum_size().y));
		stripes->set_size(Size2(p_scale.x * (graph_scroll_container->get_size().width), stripes->get_size().y));
	}
	else
	{
		stripes->set_custom_minimum_size(Size2(p_scale.x * (graph_scroll_container->get_size().width), p_scale.y * graph_scroll_container->get_size().height));
		stripes->set_size(Size2(p_scale.x * (graph_scroll_container->get_size().width), p_scale.y * graph_scroll_container->get_size().height));
	}
	//stripes->set_texture_size(min_size);
	//graph_spacer->update_minimum_size();
	if (!p_only_x)
	{
		piano_bar->set_custom_minimum_size(Vector2(piano_scroll_container->get_h_scroll_bar()->get_custom_minimum_size().x, min_size.y));
	}

	queue_redraw();
	//timeline->set_
	//timeline_scroll_container->update_length(min_size.x);
}
*/
void AudioToMidiEditor::_timeline_changed(double p_pos, bool p_drag, bool p_timeline_only)
{
	play_position_pos = p_pos;

	if (!is_visible_in_tree()) {
		return;
	}

	if (current_audio_stream.is_null()) {
		return;
	}
}

bool AudioToMidiEditor::is_analyzed(const ObjectID& p_id)
{
	if (!graph_scroll_container)
		return false;

	return get_spectrum_images(p_id).size() > 0;
}

void AudioToMidiEditor::_notification(int p_what)
{
	switch (p_what)
	{
	case NOTIFICATION_READY:
		graph_scroll_container->get_v_scroll_bar()->share(piano_scroll_container->get_v_scroll_bar());
		if (!analyzer_node)
			return;
		analyzer_node->set_texture_size(main->get_size());
		force_update_transform();
		//update_graph_spacer();
		break;
	case NOTIFICATION_RESIZED:
		//update_graph_spacer();
		update_zoom();
	case NOTIFICATION_THEME_CHANGED:
		if (play_button)
		{
#if TOOLS_ENABLED
			play_button->set_icon(get_theme_icon(SNAME("Play"), SNAME("EditorIcons")));
#else
			play_button->set_text(">");
#endif
		}
		break;
	case NOTIFICATION_DRAW:
		{
			Color color = get_theme_color(SNAME("dark_color"), SNAME("Editor"));
			color.a /= 2;
			draw_rect(bottom_vb->get_rect(), color);
			//play_position->queue_redraw();
		}
		break;
	default:
		break;
	}
}

void AudioToMidiEditor::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_audio_stream", "audio_stream"), &AudioToMidiEditor::set_audio_stream);
	ClassDB::bind_method(D_METHOD("get_audio_stream"), &AudioToMidiEditor::get_audio_stream);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "audio_stream", PROPERTY_HINT_RESOURCE_TYPE, "AudioStream"), "set_audio_stream", "get_audio_stream");

	//GLOBAL_DEF("audio/analysis/")
}

void AudioToMidiEditor::set_audio_stream(const Ref<AudioStream> &p_audio_stream)
{
	current_audio_stream = p_audio_stream;

	if (timeline)
		timeline->set_current_stream(p_audio_stream);

	update_zoom();
}

Ref<AudioStream> AudioToMidiEditor::get_audio_stream()
{
	return current_audio_stream;
}

bool AudioToMidiEditor::guess_midi_notes(bool p_mono_mode)
{
	ERR_FAIL_COND_V(current_audio_stream.is_null(), false);
	return calculate_new_notes();		
}

bool AudioToMidiEditor::analyze_spectrum() {
	ERR_FAIL_COND_V(current_audio_stream.is_null(), false);

	Ref<AudioStreamPlayback> playback = current_audio_stream->instantiate_playback();
	return calculate_new_spectrum_sprites();
}

PackedInt32Array AudioToMidiEditor::get_guessed_notes()
{
	ERR_FAIL_COND_V(analyzer_node == nullptr, PackedInt32Array());
	ERR_FAIL_COND_V(current_audio_stream.is_null(), PackedInt32Array());

	return analyzer_node->guess_notes(current_audio_stream->get_instance_id());
}

void AudioToMidiEditor::_update_scroll(double p_value)
{
	//stripes->set_position( Point2(-(p_value)*timeline->get_zoom_scale(), stripes->get_position().y));
	graph_scroll_container->set_h_scroll(p_value * timeline->get_zoom_scale());
}

AudioToMidiEditor::AudioToMidiEditor()
{
	set_mouse_filter(MOUSE_FILTER_PASS);
	set_anchors_preset(Control::PRESET_FULL_RECT);
	set_h_size_flags(Control::SIZE_EXPAND_FILL);
	set_v_size_flags(Control::SIZE_EXPAND_FILL);

	main = memnew(HBoxContainer);
	add_child(main);
	main->set_anchors_preset(Control::PRESET_FULL_RECT);
	main->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	main->set_v_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	main->set_clip_children_mode(CanvasItem::CLIP_CHILDREN_ONLY);

	// Play and Piano Bar Container

	VBoxContainer* play_and_piano_bar_container = memnew(VBoxContainer);
	main->add_child(play_and_piano_bar_container);
	play_button = memnew(Button);
	play_and_piano_bar_container->add_child(play_button);
	play_button->set_custom_minimum_size(Size2(40, 32));
	play_button->set_flat(true);

	piano_scroll_container = memnew(ScrollContainer);
	play_and_piano_bar_container->add_child(piano_scroll_container);
	piano_scroll_container->set_custom_minimum_size(Size2(40.0, 120.0));
	piano_scroll_container->set_horizontal_scroll_mode(ScrollContainer::SCROLL_MODE_DISABLED);
	piano_scroll_container->set_vertical_scroll_mode(ScrollContainer::SCROLL_MODE_SHOW_NEVER);
	piano_scroll_container->set_v_size_flags(SizeFlags::SIZE_EXPAND_FILL);

	piano_bar = memnew(PianoRollBar);
	piano_scroll_container->add_child(piano_bar);
	piano_bar->set_v_size_flags(SizeFlags::SIZE_EXPAND);
	piano_bar->connect("zoom", callable_mp(this, &AudioToMidiEditor::_zoom_vertical_step));

	// Timeline Container

	VBoxContainer* timeline_container = memnew(VBoxContainer);
	main->add_child(timeline_container);
	timeline_container->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);

	timeline = memnew(Timeline);
	timeline_container->add_child(timeline);
	timeline->connect("timeline_changed", callable_mp(this, &AudioToMidiEditor::_timeline_changed));
	timeline->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	timeline->set_custom_minimum_size(Size2(0, 32));


	graph_scroll_container = memnew(ScrollContainer);
	timeline_container->add_child(graph_scroll_container);
	graph_scroll_container->set_anchors_preset(Control::PRESET_FULL_RECT);
	graph_scroll_container->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	graph_scroll_container->set_v_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	graph_scroll_container->set_horizontal_scroll_mode(ScrollContainer::SCROLL_MODE_SHOW_NEVER);
	
	//graph_spacer = memnew(PanelContainer);
	//graph_scroll_container->add_child(graph_spacer);
	//graph_spacer->set_z_index(-1);

	stripes = memnew(AudioGraphEditor);
	//graph_spacer->add_child(stripes);
	graph_scroll_container->add_child(stripes);
	//stripes->set_anchors_preset(Control::PRESET_FULL_RECT);
	//stripes->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	//stripes->set_v_size_flags(SizeFlags::SIZE_EXPAND_FILL);

	//graph_area = memnew(Node2D);
	//graph_spacer->add_child(graph_area);
	//stripes->add_child(graph_area);
	//graph_spacer->add_child(graph_area);
	//stripes->add_child(graph_area);

	//spectrum_sprites = memnew(Node2D);
	//graph_area->add_child(spectrum_sprites);
	
	//graph_area->add_child(notes);

	bottom_vb = memnew(VBoxContainer);
	add_child(bottom_vb);

	hscroll = memnew(HScrollBar);
	bottom_vb->add_child(hscroll);
	hscroll->share(timeline);
	//hscroll->share(graph_scroll_container->get_h_scroll_bar());
	hscroll->hide();
	hscroll->connect("value_changed", callable_mp(this, &AudioToMidiEditor::_update_scroll));
	hscroll->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	hscroll->set_custom_minimum_size(Size2(0, 16));

	HBoxContainer* bottom_hb = memnew(HBoxContainer);
	bottom_vb->add_child(bottom_hb);
	bottom_hb->set_custom_minimum_size(Size2(0, 16));
	Control* spacer = bottom_hb->add_spacer();
	spacer->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);

	zoom_icon = memnew(TextureRect);
	zoom_icon->set_v_size_flags(SIZE_SHRINK_CENTER);
	bottom_hb->add_child(zoom_icon);
	zoom_slider = memnew(HSlider);
	zoom_slider->set_step(0.01);
	zoom_slider->set_min(0.0);
	zoom_slider->set_max(2.0);
	zoom_slider->set_value(1.0);
	zoom_slider->set_custom_minimum_size(Size2(200, 0) * EDSCALE);
	zoom_slider->set_v_size_flags(SIZE_SHRINK_CENTER);
	bottom_hb->add_child(zoom_slider);
	timeline->set_zoom(zoom_slider);
	timeline->set_hscroll(hscroll);
	timeline->connect("zoom_changed", callable_mp(this, &AudioToMidiEditor::update_zoom));

	analyzer_node = memnew(ASANode);
	add_child(analyzer_node);
}
