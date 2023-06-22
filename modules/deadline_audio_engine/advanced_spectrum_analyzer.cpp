#include "advanced_spectrum_analyzer.h"
//include from libnyquist
#include "libnyquist/include/libnyquist/Decoders.h"
//include from kissfft
#include "sfizz/src/external/kiss_fft/kiss_fft.h"
#include <thirdparty/embree/kernels/subdiv/tessellation_cache.h>

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

	//adding silence to the beginning and to the end
	//PackedFloat32Array silence;
	for (int i = 0; i < fft_size / 2; i++) {
		samples.insert(0, 0.0f);
		samples.push_back(0.0f);
	}
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
	kiss_fft_cpx *cx_in = memnew(kiss_fft_cpx[fft_size]);
	kiss_fft_cpx *cx_out = memnew(kiss_fft_cpx[fft_size]);
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
		for (int i = 0; i < frequency_limit_count; i++) {
			linear_magnitudes.push_back(sqrt(cx_out[i].r * cx_out[i].r + cx_out[i].i * cx_out[i].i));
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

Array ASA::analyze_spectrum(PackedByteArray bytes, const bool use_2_ffts, const int fft_size_low, const float low_high_exponent_low, const float overamplification_multiplier_low, const int fft_size_high, const float low_high_exponent_high, const float overamplification_multiplier_high, const int hop_size, const int subdivision, const float tuning) {

	//converting godot::PackedByteArray to PackedByteArray
	//I figured I'll be better off reading the file with Godot, because:
	//I don't need to worry about converting godot::String to string
	//utf8 and other stuff isn't messed up in the filename
	//I can open the file the exact same way as GDScriptAudioImport.gd opens it
	//and I might as well do it in GDScript
	//this conversion is much easier than converting the path
	//I'm also concerned because on Windows Godot still C:/returns/paths/this/way
	//which libnyquist most probably won't be able to handle without me doing conversions
	std::vector<uint8_t> encoded_audio;
	for (int i = 0; i < bytes.size(); i++) {
		encoded_audio.push_back(bytes[i]);
	}

	//use libnyquist to get the raw audio data
	std::shared_ptr<nqr::AudioData> file_data = std::make_shared<nqr::AudioData>();
	nqr::NyquistIO loader;
	loader.Load(file_data.get(), encoded_audio);

	const int sample_rate = file_data->sampleRate;
	const int original_sample_size = file_data->samples.size();
	const int channel_count = file_data->channelCount;
	const int sample_size = original_sample_size / channel_count;
	//using cout because I couldn't find an easy way to print ints with Godot::print
	print_line("Sample rate is " + String::num(sample_rate, 0));
	print_line("Original sample size is " + String::num(original_sample_size, 0));
	print_line("Channel count is " + String::num(channel_count, 0));

	PackedFloat32Array samples;
	//averaging all channels so samples become mono
	for (int i = 0; i < sample_size; i++) {
		samples.push_back(accumulate(
			file_data->samples.begin() + channel_count * i,
			file_data->samples.begin() + channel_count * (i + 1),
			0.0f) / channel_count);
	}

	//now we call analyze_subspectrum, once or twice, and then normalize the magnitudes
	std::vector<std::vector<float>> magnitudes_low = analyze_subspectrum(samples, sample_rate, fft_size_low, low_high_exponent_low, overamplification_multiplier_low, hop_size, subdivision, tuning);
	if (use_2_ffts) {
		std::vector<std::vector<float>> magnitudes_high = analyze_subspectrum(samples, sample_rate, fft_size_high, low_high_exponent_high, overamplification_multiplier_high, hop_size, subdivision, tuning);

		if (magnitudes_low.size() != magnitudes_high.size() || magnitudes_low[0].size() != magnitudes_high[0].size()) {
			print_line("Warning! The resolutions of the low and high magnitudes aren't the same!");
		}
		const int width = MIN(magnitudes_low.size(), magnitudes_high.size());
		const int height = MIN(magnitudes_low[0].size(), magnitudes_high[0].size());

		magnitudes.clear();
		for (int x = 0; x < width; x++) {
			std::vector<float> merged_magnitudes;
			for (int y = 0; y < height; y++) {
				const float normalized = y / (height - 1.0f);
				//we're using an easing called easeInOutExpo to interpolate between the low and high ffts
				//code source: https://easings.net/en#easeInOutExpo
				const float weight = normalized < 0.5f ? pow(2, 20 * normalized - 10) / 2 : (2 - pow(2, -20 * normalized + 10)) / 2;
				const float magnitude = (1.0f - weight) * magnitudes_low[x][y] + weight * magnitudes_high[x][y];
				merged_magnitudes.push_back(MIN(magnitude, 1.0f));
			}
			magnitudes.push_back(merged_magnitudes);
		}
	}
	else {
		//we sadly need to loop through once again even if we only do one fft, I'm not that concerned with performance
		//knowing that it's much better than gdscript anyway, but I hope that these aren't slowing things down terribly
		//and by that I mean that hopefully this takes at most 0.1 s
		//we need to loop through again to clamp the magnitudes at 1, because we can't do it in analyze_subspectrum,
		//because we need the overamplified values when there are 2 ffts
		magnitudes = magnitudes_low;
		const int height = magnitudes[0].size();
		for (auto& tick_magnitudes : magnitudes) {
			for (int i = 0; i < height; i++) {
				if (tick_magnitudes[i] > 1.0f) {
					tick_magnitudes[i] = 1.0f;
				}
			}
		}
	}

	print_line("Size of the data is " + String::num(magnitudes.size(), 0) + " by " + String::num(magnitudes[0].size(), 0));
	Array return_array;
	return_array.append(sample_rate);
	return_array.append(magnitudes.size());
	return_array.append(magnitudes[0].size());
	return return_array;
}

Array ASA::generate_images(Ref<Gradient> color_scheme) {
	Array images;
	//we need to split the image into multiple parts, because Godot's Image width (and height) is max 16384 (=Image::MAX_WIDTH)
	const int max_width = Image::MAX_WIDTH;
	const int number_of_images = magnitudes.size() / max_width;
	for (int image_count = 0; image_count <= number_of_images; image_count++) {
		const int width = image_count == number_of_images ? magnitudes.size() - max_width * image_count : max_width;
		std::vector<std::vector<float>> current_magnitudes(magnitudes.begin() + max_width * image_count,
			magnitudes.begin() + max_width * image_count + width);
		const int height = current_magnitudes[0].size();
		print_line("Creating image with width " + String::num(width, 0) + " and height " + String::num(height, 0));
		PackedByteArray image_data;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Color color = color_scheme->get_color_at_offset(current_magnitudes[x][y]);
				image_data.append(color.get_r8());
				image_data.append(color.get_g8());
				image_data.append(color.get_b8());
				image_data.append(color.get_a8());
			}
		}
		//there's also Image* but they say this is better and I actually got it working
		Ref<Image> image;
		image.instantiate();
		image->create_from_data(width, height, false, Image::FORMAT_RGBA8, image_data);
		//we want low pitch to be down and high pitch to be up
		image->flip_y();
		images.append(image);
	}
	print_line("Returning " + String::num(images.size(), 0) + " images");
	return images;
}

PackedInt32Array ASA::guess_notes(const float note_on_threshold, const float note_off_threshold, const float octave_removal_multiplier, const int minimum_length, const float volume_multiplier, const float percussion_removal) {

	print_line("Guessing notes...");
	const int subdivision = magnitudes[0].size() / 128;
	Vector<PackedFloat32Array> note_strengths;
	for (auto& current_magnitudes : magnitudes) {
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
						magnitude += current_magnitudes[(note - 1) * subdivision + i] * (-abs(i * 2.0f / (subdivision - 1) - 1.0f)) * percussion_removal;
					}
				}
			if (note < 127) {
				for (int i = 0; i < subdivision; i++) {
					//same thing but for one note higher
					//notice that lower magnitudes and higher magnitudes < 0 so we add them, but they only compensate negatively
					magnitude += current_magnitudes[(note + 1) * subdivision + i] * (-abs(i * 2.0f / (subdivision - 1) - 1.0f)) * percussion_removal;
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
				magnitude -= note_strengths[tick][note - 12] * octave_removal_multiplier;
			}
			if (!is_note_playing && magnitude >= note_on_threshold) {
				is_note_playing = true;
				note_begin_tick = tick;
			}
			if (is_note_playing && magnitude > peak_magnitude) {
				peak_magnitude = magnitude;
			}
			if (is_note_playing && magnitude <= note_off_threshold) {
				is_note_playing = false;
				//we only add the note if it exceeds the minimum length given
				//because we don't want to clutter the image with many short notes
				if (tick - note_begin_tick >= minimum_length) {
					notes.append(note_begin_tick);
					notes.append(tick);
					notes.append(note);
					notes.append(MIN(int(peak_magnitude * 128 * volume_multiplier + 0.5f), 127));
				}
				note_begin_tick = -1;
				peak_magnitude = 0.0f;
			}
		}
		//if we run out of width but there's still a note on
		if (is_note_playing && width - note_begin_tick >= minimum_length) {
			notes.append(note_begin_tick);
			notes.append(width);
			notes.append(note);
			notes.append(MIN(int(peak_magnitude * 128 * volume_multiplier + 0.5), 127));
		}
	}
	print_line(String::num(notes.size() / 4, 0) + " notes guessed");
	return notes;
}

void ASA::_bind_methods() {
	ClassDB::bind_method(D_METHOD("analyze_spectrum", "buffer", "use_2_ffts", "fft_size_low", "low_high_exponent_low", "overamplification_multiplier_low", "fft_size_high", "low_high_exponent_high", "overamplification_multiplier_high", "hop_size", "subdivision", "tuning"), &ASA::analyze_spectrum);
	ClassDB::bind_method(D_METHOD("generate_images", "color_scheme"), &ASA::generate_images);
	ClassDB::bind_method(D_METHOD("guess_notes", "note_on_threshold", "note_off_threshold", "octave_removal_multiplier", "minimum_length", "volume_multiplier", "percussion_remova"), &ASA::guess_notes);
}

ASANode::ASANode()
{
	//source of current color scheme (purple-red-yellow-white):
	//https://sourceforge.net/projects/isse/
	//and Ubuntu's color scheme
	color_scheme.instantiate();
	color_scheme->set_offsets({ 0, 0.088889, 0.316667, 0.638889, 1.0 });
	color_scheme->set_colors({ {0.25098, 0.243137, 0.282353, 1.0}, {0.466667, 0.129412, 0.435294, 1.0}, {0.866667, 0.282353, 0.0784314, 1.0 }, { 1.0, 1.0, 0.0, 1.0},{ 1.0, 1.0, 1.0, 1.0}});
	analyzer.instantiate();

	conversion_options_dialog = memnew(Window); // = $"/root/main_window/conversion_options_dialog"
	add_child(conversion_options_dialog);
}

Array ASANode::analyze_spectrum()
{
	if (!analyzer.is_valid())
		return;
	conversion_options_dialog->read_in_fft_options();
	print_line("Loading audio file ", options.misc.file_path);
	Ref<FileAccess> file;
	file.instantiate();
	file->open(options.misc.file_path, FileAccess::READ);
	PackedByteArray bytes = file->get_buffer(file->get_length());
	Array return_array = analyzer->analyze_spectrum(bytes, options.fft.use_2_ffts, options.fft.fft_size_low, options.fft.low_high_exponent_low, options.fft.overamplification_multiplier_low, options.fft.fft_size_high, options.fft.low_high_exponent_high, options.fft.overamplification_multiplier_high, options.fft.hop_size, options.fft.subdivision, options.fft.tuning);
	sample_rate = return_array[0];
	texture_size = Vector2(return_array[1], return_array[2]);

	Array images = analyzer->generate_images(color_scheme);
	Array spectrum_sprites;
	for (auto i = 0; i < images.size(); ++i) {
		Ref<ImageTexture> image_texture = ImageTexture::create_from_image(images[i]);
		Ref<Sprite2D> spectrum_sprite;
		spectrum_sprite.instantiate();
		spectrum_sprite->set_centered(false);
		spectrum_sprite->set_texture(image_texture);
		spectrum_sprites.append(spectrum_sprite);
	}
	return spectrum_sprites;
}

PackedInt32Array ASANode::get_guessed_notes()
{
	conversion_options_dialog->read_in_note_recognition_options();
	return analyzer->guess_notes(options.note_recognition.note_on_threshold, options.note_recognition.note_off_threshold, options.note_recognition.octave_removal_multiplier, options.note_recognition.minimum_length, options.note_recognition.volume_multiplier, options.note_recognition.percussion_removal);
}

ASA::ASA() {

}

void ASAGraphScrollContainer::input(const Ref<InputEvent>& p_event)
{
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_null())
		return;

	if (mb->get_button_index() != MouseButton::WHEEL_UP && mb->get_button_index() != MouseButton::WHEEL_DOWN)
		return;

	if (mb->is_command_or_control_pressed())
	{
		const float factor_x = mb->get_button_index() == MouseButton::WHEEL_UP ? 1.2 : 1.0 / 1.2;
		const float factor_y = mb->is_shift_pressed() ? 1.0 : factor_x;
		zoom(Vector2(factor_x, factor_y));
		get_viewport()->set_input_as_handled();
	}
}

void ASAGraphScrollContainer::reset()
{
	set_h_scroll(0);
	set_v_scroll(0);
	graph_area->set_scale(Size2(1, 1));
	calculate_new_spectrum_sprites();
	calculate_new_notes();
}
void ASAGraphScrollContainer::calculate_new_spectrum_sprites()
{
	notes->remove_notes();
	new_spectrum_sprites = spectrum_analyzer.analyze_spectrum();
	for (spectrum_sprite : spectrum_sprites.get_children())
	{
		spectrum_sprite.queue_free();
		var current_width = 0;
		for (spectrum_sprite : new_spectrum_sprites)
		{
			spectrum_sprite->get_position().x = current_width;
			current_width += spectrum_sprite.texture.get_width();
			spectrum_sprites->add_child(spectrum_sprite);
			stripes->update();
			update_graph_spacer();
		}
	}
}
void ASAGraphScrollContainer::calculate_new_notes()
{
	notes->remove_notes();
	notes->add_notes(spectrum_analyzer->get_guessed_notes());
}

void ASAGraphScrollContainer::zoom(Size2 p_scale)
{
	graph_area->set_scale(graph_area->get_scale() * p_scale);
	update_graph_spacer();
	// we need to temporarily raise the limit so it isn't capped,
	// because the container's resize is only in the queue
	get_h_scroll_bar()->set_max(graph_spacer->get_minimum_size().x * 2);
	get_v_scroll_bar()->set_max(graph_spacer->get_minimum_size().y * 2);
	set_h_scroll(get_h_scroll() * get_scale().x + (get_scale().x - 1) * get_size().x / 2);
	set_v_scroll(get_v_scroll() * get_scale().y + (get_scale().y - 1) * get_size().y / 2);
}

void ASAGraphScrollContainer::update_graph_spacer()
{
	Vector2 current_size = analyzer->get_texture_size() * graph_area->get_scale();
	graph_spacer->set_min_size(current_size);
	piano_scroll_container->update_length(current_size);
	timeline_scroll_container->update_length(current_size);
}

ASAGraphScrollContainer::ASAGraphScrollContainer(AudioToMidiTimelineScrollContainer* p_timeline_scroll_container)
	: timeline_scroll_container(p_timeline_scroll_container)
{
	analyzer.instantiate();
}

void ASAGraphScrollContainer::_bind_methods()
{
}


ASAGraphScrollContainer::~ASAGraphScrollContainer()
{
}

void AudioToMidiEditor::_bind_methods()
{
}

AudioToMidiEditor::AudioToMidiEditor()
{
	set_anchor(SIDE_RIGHT, 1.0);
	set_anchor(SIDE_BOTTOM, 1.0);
		//theme = ExtResource( 17 )
	set_meta("_edit_use_anchors_", false);
	
	VBoxContainer* toolbar_separator = memnew(VBoxContainer);
	toolbar_separator->set_anchor(SIDE_RIGHT, 1.0);
	toolbar_separator->set_anchor(SIDE_BOTTOM, 1.0);
	toolbar_separator->set_meta("_edit_use_anchors_", false);
	add_child(toolbar_separator);

	HBoxContainer* toolbar = memnew(HBoxContainer);
	toolbar_separator->add_child(toolbar);
	toolbar->set_anchors_and_offsets_preset(Control::PRESET_HCENTER_WIDE);
	
	toolbar->set_custom_minimum_size(Size2(200.0, 25.0));
	Control* spacer = toolbar_separator->add_spacer();
	spacer->set_custom_minimum_size(Size2(0, 20.0));
	//toolbar->script = ExtResource( 7 )	
	Control* spacer_left = toolbar->add_spacer();
	spacer_left->set_custom_minimum_size(Size2(45.0, 25.0));
	BitField<Control::SizeFlags> size_flags;
	size_flags.set_flag(Control::SIZE_EXPAND_FILL);
	spacer_left->set_h_size_flags(size_flags);

	Button* open_wave_file_button = memnew(Button);
	toolbar->add_child(open_wave_file_button);
	open_wave_file_button->set_offset(SIDE_LEFT, 65.0);
	open_wave_file_button->set_offset(SIDE_RIGHT, 184.0);
	open_wave_file_button->set_offset(SIDE_BOTTOM, 25.0);
	open_wave_file_button->set_tooltip_text("Load a wave audio file to be processed.");
	open_wave_file_button->set_focus_mode(FOCUS_NONE);
	open_wave_file_button->set_text("Open wave file");

	Button* save_midi_file_button = memnew(Button);
	toolbar->add_child(save_midi_file_button);
	save_midi_file_button->set_offset(SIDE_LEFT, 204.0);
	save_midi_file_button->set_offset(SIDE_RIGHT, 312.0);
	save_midi_file_button->set_offset(SIDE_BOTTOM, 25.0);
	save_midi_file_button->set_tooltip_text("Serialize the notes you see into a midi file.");
	save_midi_file_button->set_focus_mode(FOCUS_NONE);
	save_midi_file_button->set_text("Save midi file");

	Control* spacer_midleft = memnew(Control);
	toolbar->add_child(spacer_midleft);
	spacer_midleft->set_offset(SIDE_LEFT, 332.0);
	spacer_midleft->set_offset(SIDE_RIGHT, 377.0);
	spacer_midleft->set_offset(SIDE_BOTTOM, 25.0);
	spacer_midleft->set_h_size_flags(Control::SIZE_EXPAND_FILL);

	Button* conversion_options_button = memnew(Button);
	toolbar->add_child(conversion_options_button);
	conversion_options_button->set_offset(SIDE_LEFT, 397.0);
	conversion_options_button->set_offset(SIDE_RIGHT, 546.0);
	conversion_options_button->set_offset(SIDE_BOTTOM, 25.0);
	conversion_options_button->set_tooltip_text("Options regarding the image generation and note recognition.");
	conversion_options_button->set_focus_mode(FOCUS_NONE);
	conversion_options_button->set_text("Conversion options");

	Button* general_options_button = memnew(Button);
	toolbar->add_child(general_options_button);
	general_options_button->set_offset(SIDE_LEFT, 566.0);
	general_options_button->set_offset(SIDE_RIGHT, 691.0);
	general_options_button->set_offset(SIDE_BOTTOM, 25.0);
	general_options_button->set_tooltip_text("Options regarding the playback cursor and screen position.");
	general_options_button->set_focus_mode(FOCUS_NONE);
	general_options_button->set_text("General options");
	general_options_button->set_meta("_editor_description_", "");

	Control* spacer_middle = memnew(Control);
	toolbar->add_child(spacer_middle);
	spacer_middle->set_offset(SIDE_LEFT, 711.0);
	spacer_middle->set_offset(SIDE_RIGHT, 756.0);
	spacer_middle->set_offset(SIDE_BOTTOM, 25.0);
	spacer_middle->set_h_size_flags(Control::SIZE_EXPAND_FILL);

	OptionButton* source_option_combobox = memnew(OptionButton);
	toolbar->add_child(source_option_combobox);
	source_option_combobox->set_offset(SIDE_LEFT, 776.0);
	source_option_combobox->set_offset(SIDE_RIGHT, 976.0);
	source_option_combobox->set_offset(SIDE_BOTTOM, 25.0);
	source_option_combobox->set_custom_minimum_size(Size2( 200, 0 ));
	source_option_combobox->set_tooltip_text("Which audio source should be played: wave or midi?");
	source_option_combobox->set_focus_mode(FOCUS_NONE);
	source_option_combobox->set_text("Play midi and wave [1]");
	source_option_combobox->set_text_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	source_option_combobox->add_item("Play midi and source[1]");
	source_option_combobox->add_item("Play midi only[2]");
	source_option_combobox->add_item("Play wave only[3]");
	source_option_combobox->add_item("Midi left, wave right[4]");
	source_option_combobox->add_item("Midi right, wave left[5]");
	source_option_combobox->select(0);
	source_option_combobox->set_meta("_editor_description_", "");

	Button* hide_notes_button = memnew(Button);
	toolbar->add_child(hide_notes_button);
	hide_notes_button->set_offset(SIDE_LEFT, 996.0);
	hide_notes_button->set_offset(SIDE_RIGHT, 1085.0);
	hide_notes_button->set_offset(SIDE_BOTTOM, 25.0);
	hide_notes_button->set_tooltip_text("Make the horizontal colored lines temporarily invisible.");
	hide_notes_button->set_focus_mode(FOCUS_NONE);
	hide_notes_button->set_toggle_mode(true);
	hide_notes_button->set_text("Hide notes");
	hide_notes_button->set_meta("_edit_use_anchors_", false);

	Control* spacer_midright = memnew(Control);
	toolbar->add_child(spacer_midright);
	spacer_midright->set_offset(SIDE_LEFT, 1105.0);
	spacer_midright->set_offset(SIDE_RIGHT, 1150.0);
	spacer_midright->set_offset(SIDE_BOTTOM, 25.0);
	spacer_midright->set_h_size_flags(Control::SIZE_EXPAND_FILL);

	Button* help_button = memnew(Button);
	toolbar->add_child(help_button);
	help_button->set_offset(SIDE_LEFT, 1170.0);
	help_button->set_offset(SIDE_RIGHT, 1215.0);
	help_button->set_offset(SIDE_BOTTOM, 25.0);
	help_button->set_tooltip_text("Show shortcuts and version info.");
	help_button->set_focus_mode(FOCUS_NONE);
	help_button->set_text("Help");
	help_button->set_meta("_edit_use_anchors_", false);

	Control* spacer_right = memnew(Control);
	toolbar->add_child(spacer_right);
	spacer_right->set_offset(SIDE_LEFT, 1235.0);
	spacer_right->set_offset(SIDE_RIGHT, 1280.0);
	spacer_right->set_offset(SIDE_BOTTOM, 25.0);
	spacer_right->set_h_size_flags(Control::SIZE_EXPAND_FILL);

	GridContainer* working_area = memnew(GridContainer);
	toolbar_separator->add_child(working_area);
	working_area->set_offset(SIDE_TOP, 29.0);
	working_area->set_offset(SIDE_RIGHT, 1280.0);
	working_area->set_offset(SIDE_BOTTOM, 720.0);
	working_area->set_v_size_flags(3);
	working_area->set_columns(2);

	Control* bar_spacer = memnew(Control);
	working_area->add_child(bar_spacer);
	bar_spacer->set_offset(SIDE_RIGHT, 40.0);
	bar_spacer->set_offset(SIDE_BOTTOM, 30.0);
	bar_spacer->set_custom_minimum_size(Size2( 40, 30 ));
	bar_spacer->set_mouse_filter(MOUSE_FILTER_IGNORE);

	ScrollContainer* timeline_scroll_container = memnew(ScrollContainer);
	working_area->add_child(timeline_scroll_container);
	timeline_scroll_container->set_offset(SIDE_LEFT, 44.0);
	timeline_scroll_container->set_offset(SIDE_RIGHT, 1280.0);
	timeline_scroll_container->set_offset(SIDE_BOTTOM, 30.0);
	timeline_scroll_container->set_mouse_filter(MOUSE_FILTER_IGNORE);
	//timeline_scroll_container->script = ExtResource( 2 )

	Control* timeline_bar = memnew(Control);
	timeline_scroll_container->add_child(timeline_bar);
	timeline_bar->set_offset(SIDE_BOTTOM, 31.0);
	timeline_bar->set_custom_minimum_size(Size2( 0, 31 ));
	//timeline_bar->script = ExtResource( 6 )

	Line2D* playback_cursor = memnew(Line2D);
	timeline_bar->add_child(playback_cursor);
	playback_cursor->set_width(1.0);
	playback_cursor->set_default_color(Color( 1, 1, 1, 1 ));
	playback_cursor->set_antialiased(true);
	//playback_cursor->script = ExtResource( 10 )

	Area2D* area = memnew(Area2D);
	playback_cursor->add_child(area);

	CollisionShape2D* collision = memnew(CollisionShape2D);
	area->add_child(collision);
	//area->shape = SubResource( 1 )

	ScrollContainer* piano_scroll_container = memnew(ScrollContainer);
	working_area->add_child(piano_scroll_container);
	piano_scroll_container->set_offset(SIDE_TOP, 34.0);
	piano_scroll_container->set_offset(SIDE_RIGHT, 40.0);
	piano_scroll_container->set_offset(SIDE_BOTTOM, 691.0);
	piano_scroll_container->set_mouse_filter(MOUSE_FILTER_IGNORE);
	//piano_scroll_container->script = ExtResource( 4 )

	Control* piano_bar = memnew(Control);
	piano_scroll_container->add_child(piano_bar);
	piano_bar->set_offset(SIDE_RIGHT, 41.0);
	piano_bar->set_custom_minimum_size(Size2( 41, 0 ));
	//piano_bar->script = ExtResource( 5 )

	graph_scroll_container = memnew(ASAGraphScrollContainer());
	working_area->add_child(graph_scroll_container);
	graph_scroll_container->set_offset(SIDE_LEFT, 44.0);
	graph_scroll_container->set_offset(SIDE_TOP, 34.0);
	graph_scroll_container->set_offset(SIDE_RIGHT, 1280.0);
	graph_scroll_container->set_offset(SIDE_BOTTOM, 691.0);
	graph_scroll_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	graph_scroll_container->set_v_size_flags(3);
	//graph_scroll_container->script = ExtResource( 3 )

	Control* graph_spacer = memnew(Control);
	graph_scroll_container->add_child(graph_spacer);

	Node2D* graph_area = memnew(Node2D);
	graph_spacer->add_child(graph_area);
	graph_spacer->set_z_index(-1);

	Node2D* spectrum_sprites = memnew(Node2D);
	graph_area->add_child(spectrum_sprites);

	Node2D* stripes = memnew(Node2D);
	graph_area->add_child(stripes);
	//graph_area->script = ExtResource( 8 )

	Node2D* notes = memnew(Node2D);
	graph_area->add_child(notes);
	//graph_area->script = ExtResource( 9 )

	FileDialog* open_wave_file_dialog = memnew(FileDialog);
	add_child(open_wave_file_dialog);
	//open_wave_file_dialog->set_anchor(SIDE_LEFT,  0.5);
	//open_wave_file_dialog->set_anchor(SIDE_TOP, 0.5);
	//open_wave_file_dialog->set_anchor(SIDE_RIGHT, 0.5);
	//open_wave_file_dialog->set_anchor(SIDE_BOTTOM, 0.5);
	//open_wave_file_dialog->set_offset(SIDE_LEFT, -400.0);
	//open_wave_file_dialog->set_offset(SIDE_TOP, -300.0);
	//open_wave_file_dialog->set_offset(SIDE_RIGHT, 400.0);
	//open_wave_file_dialog->set_offset(SIDE_BOTTOM, 300.0);
	open_wave_file_dialog->set_min_size(Size2( 400, 200 ));
	open_wave_file_dialog->set_title("Open a wave file");
	//open_wave_file_dialog->set_resizable(true);
	open_wave_file_dialog->set_mode_overrides_title(false);
	//open_wave_file_dialog->set_mode(0);
	//open_wave_file_dialog->set_access(2);
	open_wave_file_dialog->set_filters({ "*.wav ; WAV files", "*.ogg ; OGG Vorbis files", "*.mp3 ; MP3 files" });
	open_wave_file_dialog->set_current_dir("/");
	open_wave_file_dialog->set_current_path("/");
	//open_wave_file_dialog->script = ExtResource( 12 ));
	open_wave_file_dialog->set_meta("_edit_use_anchors_", false);

	FileDialog* save_midi_file_dialog = memnew(FileDialog);
	add_child(save_midi_file_dialog);
	//save_midi_file_dialog->set_anchor(SIDE_LEFT,  0.5);
	//save_midi_file_dialog->set_anchor(SIDE_TOP, 0.5);
	//save_midi_file_dialog->set_anchor(SIDE_RIGHT, 0.5);
	//save_midi_file_dialog->set_anchor(SIDE_BOTTOM, 0.5);
	//save_midi_file_dialog->set_offset(SIDE_LEFT, -400.0);
	//save_midi_file_dialog->set_offset(SIDE_TOP, -300.0);
	//save_midi_file_dialog->set_offset(SIDE_RIGHT, 400.0);
	//save_midi_file_dialog->set_offset(SIDE_BOTTOM, 300.0);
	save_midi_file_dialog->set_min_size(Size2( 400, 200 ));
	save_midi_file_dialog->set_title("Save the midi file");
	//save_midi_file_dialog->set_resizable(true);
	//save_midi_file_dialog->set_access(2);
	save_midi_file_dialog->set_filters({ "*.mid ; MIDI files" });
	save_midi_file_dialog->set_current_dir("/");
	save_midi_file_dialog->set_current_path("/");
	//save_midi_file_dialog->script = ExtResource( 13 )

	conversion_options_dialog = memnew(ASAConversionOptionsDialog(graph_scroll_container));
	add_child(conversion_options_dialog);
	//conversion_options_dialog->set_anchor(SIDE_LEFT,  0.5);
	//conversion_options_dialog->set_anchor(SIDE_TOP, 0.5);
	//conversion_options_dialog->set_anchor(SIDE_RIGHT, 0.5);
	//conversion_options_dialog->set_anchor(SIDE_BOTTOM, 0.5);
	//conversion_options_dialog->set_offset(SIDE_LEFT, -300.0);
	//conversion_options_dialog->set_offset(SIDE_TOP, -250.0);
	//conversion_options_dialog->set_offset(SIDE_RIGHT, 300.0);
	//conversion_options_dialog->set_offset(SIDE_BOTTOM, 250.0);
	conversion_options_dialog->set_min_size(Size2( 600, 500 ));
	conversion_options_dialog->set_title("Conversion options");
	//conversion_options_dialog->script = ExtResource( 1 )
	conversion_options_dialog->set_meta("_edit_use_anchors_", false);

	CenterContainer* center = memnew(CenterContainer);
	conversion_options_dialog->add_child(center);
	center->set_anchor(SIDE_RIGHT, 1.0);
	center->set_anchor(SIDE_BOTTOM, 1.0);
	center->set_meta("_edit_use_anchors_", false);

	VBoxContainer* bottom_separator = memnew(VBoxContainer);
	center->add_child(bottom_separator);
	bottom_separator->set_offset(SIDE_LEFT, 16.0);
	bottom_separator->set_offset(SIDE_TOP, 24.0);
	bottom_separator->set_offset(SIDE_RIGHT, 584.0);
	bottom_separator->set_offset(SIDE_BOTTOM, 476.0);
	//bottom_separator->custom_constants/separation = 30);

	HBoxContainer* options_body = memnew(HBoxContainer);
	bottom_separator->add_child(options_body);
	options_body->set_offset(SIDE_RIGHT, 539.0);
	options_body->set_offset(SIDE_BOTTOM, 352.0);
	//options_body->custom_constants/separation = 40

	VBoxContainer* fft_part = memnew(VBoxContainer);
	options_body->add_child(fft_part);
	fft_part->set_offset(SIDE_RIGHT, 289.0);
	fft_part->set_offset(SIDE_BOTTOM, 352.0);
	//fft_part->custom_constants/separation = 20

	Label* fft_options_label = memnew(Label);
	fft_part->add_child(fft_options_label);
	fft_options_label->set_offset(SIDE_RIGHT, 289.0);
	fft_options_label->set_offset(SIDE_BOTTOM, 14.0);
	fft_options_label->set_text("FFT options");
	fft_options_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);

	Button* two_ffts_option = memnew(Button);
	fft_part->add_child(two_ffts_option);
	two_ffts_option->set_offset(SIDE_LEFT, 87.0);
	two_ffts_option->set_offset(SIDE_TOP, 34.0);
	two_ffts_option->set_offset(SIDE_RIGHT, 201.0);
	two_ffts_option->set_offset(SIDE_BOTTOM, 54.0);
	two_ffts_option->set_tooltip_text("If this is on, we calculate 2 separate FFTs, one for lower notes"
										"and one for higher notes. We then interpolate between them,"
										"using mostly the low FFT's data for lower notes, and the high"
										"FFT's data for higher notes. This tries to complement the"
										"limitation of FFTs described in FFT size.");
	two_ffts_option->set_h_size_flags(4);
	two_ffts_option->set_toggle_mode(true);
	two_ffts_option->set_pressed(true);
	two_ffts_option->set_text("Calculate 2 FFTs");
	two_ffts_option->set_meta("_edit_use_anchors_", false);

	GridContainer* grid_double = memnew(GridContainer);
	fft_part->add_child(grid_double);
	grid_double->set_offset(SIDE_TOP, 74.0);
	grid_double->set_offset(SIDE_RIGHT, 289.0);
	grid_double->set_offset(SIDE_BOTTOM, 220.0);
	//grid_double->custom_constants/vseparation = 20
	//grid_double->custom_constants/hseparation = 10
	grid_double->set_columns(3);

	Control* spacer = memnew(Control);
	grid_double->add_child(spacer);
	spacer->set_offset(SIDE_RIGHT, 121.0);
	spacer->set_offset(SIDE_BOTTOM, 14.0);
	spacer->set_meta("_edit_use_anchors_", false);

	Label* low_label = memnew(Label);
	grid_double->add_child(low_label);
	low_label->set_offset(SIDE_LEFT, 131.0);
	low_label->set_offset(SIDE_RIGHT, 205.0);
	low_label->set_offset(SIDE_BOTTOM, 14.0);
	low_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	low_label->set_text("Low");
	low_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);

	Label* high_label = memnew(Label);
	grid_double->add_child(high_label);
	high_label->set_offset(SIDE_LEFT, 215.0);
	high_label->set_offset(SIDE_RIGHT, 289.0);
	high_label->set_offset(SIDE_BOTTOM, 14.0);
	high_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	high_label->set_text("High");
	high_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);

	Label* fft_size_label = memnew(Label);
	grid_double->add_child(fft_size_label);
	fft_size_label->set_offset(SIDE_TOP, 39.0);
	fft_size_label->set_offset(SIDE_RIGHT, 121.0);
	fft_size_label->set_offset(SIDE_BOTTOM, 53.0);
	fft_size_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	fft_size_label->set_text("FFT size");
	fft_size_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* fft_size_low_option = memnew(SpinBox);
	grid_double->add_child(fft_size_low_option);
	fft_size_low_option->set_offset(SIDE_LEFT, 131.0);
	fft_size_low_option->set_offset(SIDE_TOP, 34.0);
	fft_size_low_option->set_offset(SIDE_RIGHT, 205.0);
	fft_size_low_option->set_offset(SIDE_BOTTOM, 58.0);
	fft_size_low_option->set_tooltip_text("The amount of samples used for the FFT calculation."
											"If bigger, lower notes will be more precise vertically"
											"if smaller, notes will be more precise horizontally."
											"Please use a 2's exponent!");
	fft_size_low_option->set_v_size_flags(3);
	fft_size_low_option->set_min(128.0);
	fft_size_low_option->set_max(65536.0);
	fft_size_low_option->set_step(128.0);
	fft_size_low_option->set_value(128.0);
	fft_size_low_option->set_exp_ratio(true);
	fft_size_low_option->set_use_rounded_values(true);

	SpinBox* fft_size_high_option = memnew(SpinBox);
	grid_double->add_child(fft_size_high_option);
	fft_size_high_option->set_offset(SIDE_LEFT, 215.0);
	fft_size_high_option->set_offset(SIDE_TOP, 34.0);
	fft_size_high_option->set_offset(SIDE_RIGHT, 289.0);
	fft_size_high_option->set_offset(SIDE_BOTTOM, 58.0);
	fft_size_high_option->set_tooltip_text("The amount of samples used for the FFT calculation."
											"If bigger, lower notes will be more precise vertically"
											"if smaller, notes will be more precise horizontally."
											"Please use a 2's exponent!");
	fft_size_high_option->set_v_size_flags(3);
	fft_size_high_option->set_min(128.0);
	fft_size_high_option->set_max(65536.0);
	fft_size_high_option->set_step(128.0);
	fft_size_high_option->set_value(128.0);
	fft_size_high_option->set_exp_ratio(true);
	fft_size_high_option->set_use_rounded_values(true);

	Label* low_high_exponent_label = memnew(Label);
	grid_double->add_child(low_high_exponent_label);
	low_high_exponent_label->set_offset(SIDE_TOP, 83.0);
	low_high_exponent_label->set_offset(SIDE_RIGHT, 121.0);
	low_high_exponent_label->set_offset(SIDE_BOTTOM, 97.0);
	low_high_exponent_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	low_high_exponent_label->set_text("Low-high exponent");
	low_high_exponent_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* low_high_exponent_low_option = memnew(SpinBox);
	grid_double->add_child(low_high_exponent_low_option);
	low_high_exponent_low_option->set_offset(SIDE_LEFT, 131.0);
	low_high_exponent_low_option->set_offset(SIDE_TOP, 78.0);
	low_high_exponent_low_option->set_offset(SIDE_RIGHT, 205.0);
	low_high_exponent_low_option->set_offset(SIDE_BOTTOM, 102.0);
	low_high_exponent_low_option->set_tooltip_text("This controls the magnitude correction applied."
													"If bigger, higher notes will be louder, if smaller, lower notes will be louder."
													"You can try using it against upper harmonics by lowering it.");
	low_high_exponent_low_option->set_v_size_flags(3);
	low_high_exponent_low_option->set_max(1.5);
	low_high_exponent_low_option->set_step(0.0);

	SpinBox* low_high_exponent_high_option = memnew(SpinBox);
	grid_double->add_child(low_high_exponent_high_option);
	low_high_exponent_high_option->set_offset(SIDE_LEFT, 215.0);
	low_high_exponent_high_option->set_offset(SIDE_TOP, 78.0);
	low_high_exponent_high_option->set_offset(SIDE_RIGHT, 289.0);
	low_high_exponent_high_option->set_offset(SIDE_BOTTOM, 102.0);
	low_high_exponent_high_option->set_tooltip_text("This controls the magnitude correction applied."
													"If bigger, higher notes will be louder, if smaller, lower notes will be louder."
													"You can try using it against upper harmonics by lowering it.");
	low_high_exponent_high_option->set_v_size_flags(3);
	low_high_exponent_high_option->set_max(1.5);
	low_high_exponent_high_option->set_step(0.0);

	Label* overamplification_label = memnew(Label);
	grid_double->add_child(overamplification_label);
	overamplification_label->set_offset(SIDE_TOP, 127.0);
	overamplification_label->set_offset(SIDE_RIGHT, 121.0);
	overamplification_label->set_offset(SIDE_BOTTOM, 141.0);
	overamplification_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	overamplification_label->set_text("Overamplification");
	overamplification_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* overamplification_low_option = memnew(SpinBox);
	grid_double->add_child(overamplification_low_option);
	overamplification_low_option->set_offset(SIDE_LEFT, 131.0);
	overamplification_low_option->set_offset(SIDE_TOP, 122.0);
	overamplification_low_option->set_offset(SIDE_RIGHT, 205.0);
	overamplification_low_option->set_offset(SIDE_BOTTOM, 146.0);
	overamplification_low_option->set_tooltip_text("The normalized magnitudes are multiplied by this, making everything louder.);"
													"If the whole picture is too dark and there are few notes recognized"
													"(e.g. classical music or piano music), try making this bigger (even 8 e.g.)."
													"The overamplified magnitudes are then clamped at the max value (1).");
	overamplification_low_option->set_v_size_flags(3);
	overamplification_low_option->set_min(1.0);
	overamplification_low_option->set_max(20.0);
	overamplification_low_option->set_step(0.0);
	overamplification_low_option->set_value(1.0);

	SpinBox* overamplification_high_option = memnew(SpinBox);
	grid_double->add_child(overamplification_high_option);
	overamplification_high_option->set_offset(SIDE_LEFT, 215.0);
	overamplification_high_option->set_offset(SIDE_TOP, 122.0);
	overamplification_high_option->set_offset(SIDE_RIGHT, 289.0);
	overamplification_high_option->set_offset(SIDE_BOTTOM, 146.0);
	overamplification_high_option->set_tooltip_text("The normalized magnitudes are multiplied by this, making everything louder.);"
													"If the whole picture is too dark and there are few notes recognized"
													"(e.g. classical music or piano music), try making this bigger (even 8 e.g.)."
													"The overamplified magnitudes are then clamped at the max value (1).");
	overamplification_high_option->set_v_size_flags(3);
	overamplification_high_option->set_min(1.0);
	overamplification_high_option->set_max(20.0);
	overamplification_high_option->set_step(0.0);
	overamplification_high_option->set_value(1.0);

	GridContainer* grid_single = memnew(GridContainer);
	fft_part->add_child(grid_single);
	grid_single->set_offset(SIDE_LEFT, 66.0);
	grid_single->set_offset(SIDE_TOP, 240.0);
	grid_single->set_offset(SIDE_RIGHT, 223.0);
	grid_single->set_offset(SIDE_BOTTOM, 352.0);
	grid_single->set_h_size_flags(4);
	//grid_single->custom_constants/vseparation = 20
	//grid_single->custom_constants/hseparation = 10
	grid_single->set_columns(2);

	Label* hop_size_label = memnew(Label);
	grid_single->add_child(hop_size_label);
	hop_size_label->set_offset(SIDE_TOP, 5.0);
	hop_size_label->set_offset(SIDE_RIGHT, 73.0);
	hop_size_label->set_offset(SIDE_BOTTOM, 19.0);
	hop_size_label->set_text("Hop size");
	hop_size_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* hop_size_option = memnew(SpinBox);
	grid_single->add_child(hop_size_option);
	hop_size_option->set_offset(SIDE_LEFT, 83.0);
	hop_size_option->set_offset(SIDE_RIGHT, 157.0);
	hop_size_option->set_offset(SIDE_BOTTOM, 24.0);
	hop_size_option->set_tooltip_text("The amount of samples jumped between FFT calculations."
										"The horizontal resolution will be better when low."
										"Too low numbers don't matter much, but will slow down calculation a lot."
										"Not beneficial to use less than the quarter of FFT size."
										"I don't think you have to use 2's exponents here, though I'd still recommend it.");
	hop_size_option->set_v_size_flags(3);
	hop_size_option->set_min(32.0);
	hop_size_option->set_max(65536.0);
	hop_size_option->set_step(128.0);
	hop_size_option->set_value(32.0);
	hop_size_option->set_exp_ratio(true);
	hop_size_option->set_use_rounded_values(true);

	Label* subdivision_label = memnew(Label);
	grid_single->add_child(subdivision_label);
	subdivision_label->set_offset(SIDE_TOP, 49.0);
	subdivision_label->set_offset(SIDE_RIGHT, 73.0);
	subdivision_label->set_offset(SIDE_BOTTOM, 63.0);
	subdivision_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	subdivision_label->set_text("Subdivision");
	subdivision_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* subdivision_option = memnew(SpinBox);
	grid_single->add_child(subdivision_option);
	subdivision_option->set_offset(SIDE_LEFT, 83.0);
	subdivision_option->set_offset(SIDE_TOP, 44.0);
	subdivision_option->set_offset(SIDE_RIGHT, 157.0);
	subdivision_option->set_offset(SIDE_BOTTOM, 68.0);
	subdivision_option->set_tooltip_text("The amount of pixels to calculate with the linear to exponential frequency calculation per note."
										"This determines the vertical resolution, though FFT size is more responsible for it."
										"I recommend using odd numbers here, so the \"main\" frequency is in the middle pixel.");
	subdivision_option->set_v_size_flags(3);
	subdivision_option->set_min(1.0);
	subdivision_option->set_max(255.0);
	subdivision_option->set_value(1.0);
	subdivision_option->set_use_rounded_values(true);

	Label* tuning_label = memnew(Label);
	grid_single->add_child(tuning_label);
	tuning_label->set_offset(SIDE_TOP, 93.0);
	tuning_label->set_offset(SIDE_RIGHT, 73.0);
	tuning_label->set_offset(SIDE_BOTTOM, 107.0);
	tuning_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tuning_label->set_text("Tuning");
	tuning_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* tuning_option = memnew(SpinBox);
	grid_single->add_child(tuning_option);
	tuning_option->set_offset(SIDE_LEFT, 83.0);
	tuning_option->set_offset(SIDE_TOP, 88.0);
	tuning_option->set_offset(SIDE_RIGHT, 157.0);
	tuning_option->set_offset(SIDE_BOTTOM, 112.0);
	tuning_option->set_tooltip_text("The frequency corresponding to the A4 note, also called as midi note 69."
									"This is used when calculating the image, so if the notes that are played"
									"fall between two piano keys, then change this."
									"This doesn't change the wave playback.");
	tuning_option->set_v_size_flags(3);
	tuning_option->set_min(400.0);
	tuning_option->set_max(500.0);
	tuning_option->set_step(0.0);
	tuning_option->set_value(400.0);
	tuning_option->set_allow_greater(true);
	tuning_option->set_allow_lesser(true);

	VBoxContainer* recognition_part = memnew(VBoxContainer);
	options_body->add_child(recognition_part);
	recognition_part->set_offset(SIDE_LEFT, 329.0);
	recognition_part->set_offset(SIDE_RIGHT, 539.0);
	recognition_part->set_offset(SIDE_BOTTOM, 352.0);
	//recognition_part->custom_constants/separation = 20);

	Label* recognition_options_label = memnew(Label);
	recognition_part->add_child(recognition_options_label);
	recognition_options_label->set_offset(SIDE_RIGHT, 210.0);
	recognition_options_label->set_offset(SIDE_BOTTOM, 14.0);
	recognition_options_label->set_text("Note recognition options");
	recognition_options_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);

	GridContainer* grid = memnew(GridContainer);
	recognition_part->add_child(grid);
	grid->set_offset(SIDE_TOP, 34.0);
	grid->set_offset(SIDE_RIGHT, 210.0);
	grid->set_offset(SIDE_BOTTOM, 278.0);
	grid->set_h_size_flags(4);
	//grid->custom_constants/vseparation = 20);
	//grid->custom_constants/hseparation = 10);
	grid->set_columns(2);

	Label* note_on_threshold_label = memnew(Label);
	grid->add_child(note_on_threshold_label);
	note_on_threshold_label->set_offset(SIDE_TOP, 5.0);
	note_on_threshold_label->set_offset(SIDE_RIGHT, 126.0);
	note_on_threshold_label->set_offset(SIDE_BOTTOM, 19.0);
	note_on_threshold_label->set_text("Note on threshold");
	note_on_threshold_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* note_on_threshold_option = memnew(SpinBox);
	grid->add_child(note_on_threshold_option);
	note_on_threshold_option->set_offset(SIDE_LEFT, 136.0);
	note_on_threshold_option->set_offset(SIDE_RIGHT, 210.0);
	note_on_threshold_option->set_offset(SIDE_BOTTOM, 24.0);
	note_on_threshold_option->set_tooltip_text("The magnitude required to trigger a note on, between 0 and 1."
												"This determines how sensitive the recognition is."
												"The note on's place is also affected by the negative delay."
												"If this is small, there will be more notes recognized.");
	note_on_threshold_option->set_max(1.0);
	note_on_threshold_option->set_step(0.0);

	Label* note_off_threshold_label = memnew(Label);
	grid->add_child(note_off_threshold_label);
	note_off_threshold_label->set_offset(SIDE_TOP, 49.0);
	note_off_threshold_label->set_offset(SIDE_RIGHT, 126.0);
	note_off_threshold_label->set_offset(SIDE_BOTTOM, 63.0);
	note_off_threshold_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	note_off_threshold_label->set_text("Note off threshold");
	note_off_threshold_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* note_off_threshold_option = memnew(SpinBox);
	grid->add_child(note_off_threshold_option);
	note_off_threshold_option->set_offset(SIDE_LEFT, 136.0);
	note_off_threshold_option->set_offset(SIDE_TOP, 44.0);
	note_off_threshold_option->set_offset(SIDE_RIGHT, 210.0);
	note_off_threshold_option->set_offset(SIDE_BOTTOM, 68.0);
	note_off_threshold_option->set_tooltip_text("If the magnitude of the given note falls below this value,"
												"it will trigger a note off. The next note on will be then"
												"determined by note on threshold. The value should be"
												"between 0 and note on threshold. If it's closer to note on"
												"threshold, there will be more, shorter notes, if it's closer"
												"to 0, there will be less, longer notes. Be aware that notes"
												"not exceeding minimum length will be deleted.");
	note_off_threshold_option->set_max(1.0);
	note_off_threshold_option->set_step(0.0);

	Label* octave_removal_label = memnew(Label);
	grid->add_child(octave_removal_label);
	octave_removal_label->set_offset(SIDE_TOP, 93.0);
	octave_removal_label->set_offset(SIDE_RIGHT, 126.0);
	octave_removal_label->set_offset(SIDE_BOTTOM, 107.0);
	octave_removal_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	octave_removal_label->set_text("Octave removal");
	octave_removal_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* octave_removal_option = memnew(SpinBox);
	grid->add_child(octave_removal_option);
	octave_removal_option->set_offset(SIDE_LEFT, 136.0);
	octave_removal_option->set_offset(SIDE_TOP, 88.0);
	octave_removal_option->set_offset(SIDE_RIGHT, 210.0);
	octave_removal_option->set_offset(SIDE_BOTTOM, 112.0);
	octave_removal_option->set_tooltip_text("The magnitude of the note one octave below will be multiplied"
											"by this and then subtracted from this note's magnitude"
											"when calculating the thresholds. Make this bigger if the"
											"upper harmonics are an octave above and you want to get rid"
											"of them. Make this smaller if you need the notes the system gets rid off.");
	octave_removal_option->set_max(1.0);
	octave_removal_option->set_step(0.0);

	Label* minimum_length_label = memnew(Label);
	grid->add_child(minimum_length_label);
	minimum_length_label->set_offset(SIDE_TOP, 137.0);
	minimum_length_label->set_offset(SIDE_RIGHT, 126.0);
	minimum_length_label->set_offset(SIDE_BOTTOM, 151.0);
	minimum_length_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	minimum_length_label->set_text("Minimum length");
	minimum_length_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* minimum_length_option = memnew(SpinBox);
	grid->add_child(minimum_length_option);
	minimum_length_option->set_offset(SIDE_LEFT, 136.0);
	minimum_length_option->set_offset(SIDE_TOP, 132.0);
	minimum_length_option->set_offset(SIDE_RIGHT, 210.0);
	minimum_length_option->set_offset(SIDE_BOTTOM, 156.0);
	minimum_length_option->set_tooltip_text("Notes not reaching this value will automatically be removed."
											"This tries to complement many, short notes that are recognized."
											"Set this to 0 if you want all the notes, set this higher if you"
											"have too many short notes, and want them gone."
											"This is dependent on hop size: if hop size is smaller,"
											"this should be bigger to achieve the same effect (in seconds).");
	minimum_length_option->set_max(50.0);
	minimum_length_option->set_use_rounded_values(true);

	Label* volume_multiplier_label = memnew(Label);
	grid->add_child(volume_multiplier_label);
	volume_multiplier_label->set_offset(SIDE_TOP, 181.0);
	volume_multiplier_label->set_offset(SIDE_RIGHT, 126.0);
	volume_multiplier_label->set_offset(SIDE_BOTTOM, 195.0);
	volume_multiplier_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	volume_multiplier_label->set_text("Volume multiplier");
	volume_multiplier_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* volume_multiplier_option = memnew(SpinBox);
	grid->add_child(volume_multiplier_option);
	volume_multiplier_option->set_offset(SIDE_LEFT, 136.0);
	volume_multiplier_option->set_offset(SIDE_TOP, 176.0);
	volume_multiplier_option->set_offset(SIDE_RIGHT, 210.0);
	volume_multiplier_option->set_offset(SIDE_BOTTOM, 200.0);
	volume_multiplier_option->set_tooltip_text("The velocity / volume of the midi notes are multiplied by this,"
												"then clamped at 127, the maximum. The difference between"
												"this and overamplification is that 1) overamplification can be"
												"applied to only lower / higher notes and 2) overamplification"
												"also affects the image and the note on and off thresholds.");
	volume_multiplier_option->set_min(1.0);
	volume_multiplier_option->set_max(20.0);
	volume_multiplier_option->set_step(0.0);
	volume_multiplier_option->set_value(1.0);

	Label* percussion_removal_label = memnew(Label);
	grid->add_child(percussion_removal_label);
	percussion_removal_label->set_offset(SIDE_TOP, 225.0);
	percussion_removal_label->set_offset(SIDE_RIGHT, 126.0);
	percussion_removal_label->set_offset(SIDE_BOTTOM, 239.0);
	percussion_removal_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	percussion_removal_label->set_text("Percussion removal");
	percussion_removal_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);

	SpinBox* percussion_removal_option = memnew(SpinBox);
	grid->add_child(percussion_removal_option);
	percussion_removal_option->set_offset(SIDE_LEFT, 136.0);
	percussion_removal_option->set_offset(SIDE_TOP, 220.0);
	percussion_removal_option->set_offset(SIDE_RIGHT, 210.0);
	percussion_removal_option->set_offset(SIDE_BOTTOM, 244.0);
	percussion_removal_option->set_tooltip_text("We don't want the sound of drums to be converted into a"
												"melodic instrument, so we try compensating that by"
												"subtracting the one semitone higher and one semitone"
												"lower notes from the current one (it's a bit more complex"
												"then that). If it evenly spreads across multiple notes at a"
												"given time, there won't be any notes recognized."
												"This value controls the value that is subtracted, so with"
												"a value of 1, it is fully compensated, and with a value of"
												"0 no compensation is calculated."
												"This may help getting the real pitch of deep notes too."
												"However, 2 notes, that are 1 semitone apart, next to"
												"each other could be misunderstood as percussion."
												"Try lowering this if you don't have any percussion in"
												"your music.");
	percussion_removal_option->set_max(2.0);
	percussion_removal_option->set_step(0.0);

	HBoxContainer* recalculate_separator = memnew(HBoxContainer);
	bottom_separator->add_child(recalculate_separator);
	recalculate_separator->set_offset(SIDE_LEFT, 38.0);
	recalculate_separator->set_offset(SIDE_TOP, 382.0);
	recalculate_separator->set_offset(SIDE_RIGHT, 500.0);
	recalculate_separator->set_offset(SIDE_BOTTOM, 402.0);
	recalculate_separator->set_h_size_flags(4);
	//recalculate_separator->custom_constants/separation = 20);

	Button* recalculate_fft = memnew(Button);
	recalculate_separator->add_child(recalculate_fft);
	recalculate_fft->set_offset(SIDE_RIGHT, 110.0);
	recalculate_fft->set_offset(SIDE_BOTTOM, 20.0);
	recalculate_fft->set_tooltip_text("Press this to recalculate the image, but not the notes.");
	recalculate_fft->set_text("Recalculate FFT");

	Button* recalculate_notes = memnew(Button);
	recalculate_separator->add_child(recalculate_notes);
	recalculate_notes->set_offset(SIDE_LEFT, 130.0);
	recalculate_notes->set_offset(SIDE_RIGHT, 324.0);
	recalculate_notes->set_offset(SIDE_BOTTOM, 20.0);
	recalculate_notes->set_tooltip_text("Press this to recalculate the notes, but not the image.");
	recalculate_notes->set_text("Recalculate note recognition");

	Button* recalculate_both = memnew(Button);
	recalculate_separator->add_child(recalculate_both);
	recalculate_both->set_offset(SIDE_LEFT, 344.0);
	recalculate_both->set_offset(SIDE_RIGHT, 462.0);
	recalculate_both->set_offset(SIDE_BOTTOM, 20.0);
	recalculate_both->set_tooltip_text("Press this to recalculate everything.");
	recalculate_both->set_text("Recalculate both");

	/*Window* general_options_dialog = memnew(Window);
	add_child(general_options_dialog);
	//general_options_dialog->set_anchor(SIDE_LEFT,  0.5);
	//general_options_dialog->set_anchor(SIDE_TOP, 0.5);
	//general_options_dialog->set_anchor(SIDE_RIGHT, 0.5);
	//general_options_dialog->set_anchor(SIDE_BOTTOM, 0.5);
	//general_options_dialog->set_offset(SIDE_LEFT, -250.0);
	//general_options_dialog->set_offset(SIDE_TOP, -75.0);
	//general_options_dialog->set_offset(SIDE_RIGHT, 250.0);
	//general_options_dialog->set_offset(SIDE_BOTTOM, 75.0);
	general_options_dialog->set_min_size(Size2( 500, 150 ));
	general_options_dialog->set_title("General options");
	//general_options_dialog->script = ExtResource( 11 )*/

	//CenterContainer* center = memnew(CenterContainer);
	//general_options_dialog->add_child(center);
	//center->set_anchor(SIDE_RIGHT, 1.0);
	//center->set_anchor(SIDE_BOTTOM, 1.0);
	//center->set_meta("_edit_use_anchors_", false);

	//GridContainer* grid = memnew(GridContainer);
	//center->add_child(grid);
	//grid->set_offset(SIDE_LEFT, 24.0);
	//grid->set_offset(SIDE_TOP, 40.0);
	//grid->set_offset(SIDE_RIGHT, 476.0);
	//grid->set_offset(SIDE_BOTTOM, 110.0);
	//grid->custom_constants/vseparation = 20);
	//grid->custom_constants/hseparation = 10);
	//grid->set_columns(2);
	//grid->set_meta("_edit_use_anchors_", false);

	//Label* playback_label = memnew(Label);
	//grid->add_child(playback_label);
	//playback_label->set_offset(SIDE_TOP, 3.0);
	//playback_label->set_offset(SIDE_RIGHT, 200.0);
	//playback_label->set_offset(SIDE_BOTTOM, 17.0);
	//playback_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	//playback_label->set_text("Where should playback restart?");
	//playback_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	//playback_label->set_meta("_edit_use_anchors_", false);

	//Button* playback_option = memnew(Button);
	//grid->add_child(playback_option);
	//playback_option->set_offset(SIDE_LEFT, 210.0);
	//playback_option->set_offset(SIDE_RIGHT, 410.0);
	//playback_option->set_offset(SIDE_BOTTOM, 20.0);
	//playback_option->set_tooltip_text("This option makes the triangle follow playback."
	//									"The triangle is where playback restarts after a pause.");
	//playback_option->set_focus_mode(FOCUS_NONE);
	//playback_option->set_toggle_mode(true);
	//playback_option->set_text("Continue playback");

	//Label* follow_label = memnew(Label);
	//grid->add_child(follow_label);
	//follow_label->set_offset(SIDE_TOP, 43.0);
	//follow_label->set_offset(SIDE_RIGHT, 200.0);
	//follow_label->set_offset(SIDE_BOTTOM, 57.0);
	//follow_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	//follow_label->set_text("How to follow playback?");
	//follow_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	//follow_label->set_meta("_edit_use_anchors_", false);

	//OptionButton* follow_option = memnew(OptionButton);
	//grid->add_child(follow_option);
	//follow_option->set_offset(SIDE_LEFT, 210.0);
	//follow_option->set_offset(SIDE_TOP, 40.0);
	//follow_option->set_offset(SIDE_RIGHT, 410.0);
	//follow_option->set_offset(SIDE_BOTTOM, 60.0);
	//follow_option->set_custom_minimum_size(Size2( 220, 0 ));
	//follow_option->set_tooltip_text("How the screen follows the playback line.");
	//follow_option->set_focus_mode(FOCUS_NONE);
	//follow_option->set_text("Jump if playing off-screen");
	//follow_option->set_text_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	//follow_option->items = [ "Jump if playing off-screen", null, false, 0, null, "Don't follow playback", null, false, 1, null, "Cursor always left", null, false, 2, null, "Cursor always middle", null, false, 3, null ]
	//follow_option->select(0);

	//AcceptDialog* help_dialog = memnew(AcceptDialog);
	//add_child(help_dialog);
	//help_dialog->set_anchor(SIDE_LEFT,  0.5);
	//help_dialog->set_anchor(SIDE_TOP, 0.5);
	//help_dialog->set_anchor(SIDE_RIGHT, 0.5);
	//help_dialog->set_anchor(SIDE_BOTTOM, 0.5);
	//help_dialog->set_offset(SIDE_LEFT, -231.0);
	//help_dialog->set_offset(SIDE_TOP, -54.5);
	//help_dialog->set_offset(SIDE_RIGHT, 231.0);
	//help_dialog->set_offset(SIDE_BOTTOM, 54.5);
	//help_dialog->set_title("Help");
	//help_dialog->set_text("Navigation controls:"
	//						"You can scroll up/down and left/right using (Shift + ) Mouse Wheel Up/Down, like you're probably used to in every other program."
	//						"You can use Ctrl + Mouse Wheel Up/Down to zoom in and out."
	//						"You can also use Ctrl + Shift + Mouse Wheel Up/Down to zoom only vertically."
	//						"You can zoom out vertically and zoom back in normally to zoom only horizontally."
	//						"You can play the piano with Left Click, and reposition the starting position triangle in the timeline with Left Click."
	//						""
	//						"Note editing controls:"
	//						"You add notes with Left Clicking in the main area, and dragging it left/right to make it longer."
	//						"You can remove a note with Right Clicking on it."
	//						"You can resize a note with (Left Click) dragging its left or right side."
	//						"You can change a note's volume (velocity) with (Left Click) dragging the middle of the note up or down."
	//						"Resizing and velocity change are also indicated with the mouse cursor change."
	//						"The notes' transparency reflect their volume, and hovering over the middle of the note will show the volume precisely."
	//						""
	//						"Shortcuts:"
	//						"Use the numbers 1-5 to change what should be played back (wave, midi, or both, and with or without panning), see the toolbar for possible options.");
	//
	//script = ExtResource( 15 )


	open_wave_file_button->connect("pressed", callable_mp(this, AudioToMidiEditor::_on_open_wave_file_button_pressed));
	save_midi_file_button->connect("pressed", callable_mp(this, AudioToMidiEditor::_on_save_midi_file_button_pressed));
	conversion_options_button->connect("pressed", callable_mp(this, AudioToMidiEditor::_on_conversion_options_button_pressed));
	general_options_button->connect("pressed", callable_mp(this, AudioToMidiEditor::_on_general_options_button_pressed));
	source_option_combobox->connect("item_selected", callable_mp(this, AudioToMidiEditor::_on_source_option_item_selected));
	hide_notes_button->connect("toggled", callable_mp(this, AudioToMidiEditor::_on_hide_notes_button_toggled));
	help_button->connect("pressed", callable_mp(this, AudioToMidiEditor::_on_help_button_pressed));
	area->connect("area_entered", callable_mp(this, AudioToMidiEditor::_on_area_area_entered));
	area->connect("area_exited", callable_mp(this, AudioToMidiEditor::_on_area_area_exited));
	graph_spacer->connect("gui_input", callable_mp(this, AudioToMidiEditor::_on_graph_spacer_gui_input));
	//welcome_dialog->connect("confirmed", callable_mp(this, AudioToMidiEditor::_on_welcome_dialog_confirmed));
	open_wave_file_dialog->connect("file_selected", callable_mp(this, AudioToMidiEditor::_on_open_wave_file_dialog_file_selected));
	save_midi_file_dialog->connect("file_selected", callable_mp(this, AudioToMidiEditor::_on_save_midi_file_dialog_file_selected));
	two_ffts_option->connect("toggled", callable_mp(this, AudioToMidiEditor::_on_two_ffts_option_toggled));
	recalculate_fft->connect("pressed", callable_mp(this, AudioToMidiEditor::_on_recalculate_fft_pressed));
	recalculate_notes->connect("pressed", callable_mp(this, AudioToMidiEditor::_on_recalculate_notes_pressed));
	recalculate_both->connect("pressed", callable_mp(this, AudioToMidiEditor::_on_recalculate_both_pressed));
	//playback_option->connect("toggled", callable_mp(this, AudioToMidiEditor::_on_playback_option_toggled));
	//follow_option->connect("item_selected", callable_mp(this, AudioToMidiEditor::_on_follow_option_item_selected));
}

void AudioToMidiEditor::_on_open_wave_file_button_pressed()
{
}
void AudioToMidiEditor::_on_save_midi_file_button_pressed()
{
	//save_midi_file_dialog->popup_centered();
}
void AudioToMidiEditor::_on_conversion_options_button_pressed()
{
	conversion_options_dialog->popup_centered();
}
void AudioToMidiEditor::_on_general_options_button_pressed()
{
	//general_options_dialog->popup_centered();
}
void AudioToMidiEditor::_on_source_option_item_selected()
{

}
void AudioToMidiEditor::_on_hide_notes_button_toggled()
{

}
void AudioToMidiEditor::_on_help_button_pressed()
{

}
void AudioToMidiEditor::_on_area_area_entered()
{

}
void AudioToMidiEditor::_on_area_area_exited()
{

}
void AudioToMidiEditor::_on_graph_spacer_gui_input()
{

}
void AudioToMidiEditor::_on_welcome_dialog_confirmed()
{

}
void AudioToMidiEditor::_on_open_wave_file_dialog_file_selected()
{
	//options->misc.file_path = path
	//	OS.set_window_title(path.get_file() + " - MeloMIDI")
	//	wave_player.load_file()
	//	graph_scroll_container.reset()
}
void AudioToMidiEditor::_on_save_midi_file_dialog_file_selected()
{

}
void AudioToMidiEditor::_on_two_ffts_option_toggled()
{

}
void AudioToMidiEditor::_on_recalculate_fft_pressed()
{

}
void AudioToMidiEditor::_on_recalculate_notes_pressed()
{

}
void AudioToMidiEditor::_on_recalculate_both_pressed()
{

}
void AudioToMidiEditor::_on_playback_option_toggled()
{

}
void AudioToMidiEditor::_on_follow_option_item_selected()
{

}

// ==========================================================================
void AudioToMidiTimeLineBar::input(const Ref<InputEvent>& p_event)
{
	Ref<InputEventMouse> event_mouse = p_event;
	if (event_mouse.is_null() || !event_mouse->get_button_mask().has_flag(MouseButtonMask::LEFT))
		return;

	Ref<InputEventMouseButton> event_mouse_button = p_event;
	if (!event_mouse_button.is_null())
		dragging = event_mouse_button->is_pressed();

	if (dragging)
		player->replay_cursor_position_percent = CLAMP(event_mouse_button->get_position().x, 0, get_size().x - 1) / get_size().x;

	queue_redraw();
}

void AudioToMidiTimeLineBar::_draw()
{
	if (get_size().x == 0)
		return;

	
	Ref<Font> font = get_theme_font("font", "Label");
	//we want at most 10 time messages on screen(1280px / 128px = 10text on - screen)
	//so max_texts_to_draw is for the full timeline
	float max_texts_to_draw = get_size().x / 128.0;
	float full_time = spectrum_analyzer->get_texture_size().x * options->fft.hop_size / spectrum_analyzer->get_sample_rate();
	float min_seconds_per_text_drawn = full_time / max_texts_to_draw;
	float step_size = pow(10, ceil(3 * log(min_seconds_per_text_drawn) / log(10)) / 3);
	int order_of_magnitude = floor(log(step_size) / log(10));

	step_size = Math::snapped(step_size, pow(10, order_of_magnitude));
	int texts_to_draw = ceil(full_time / step_size);
	for (auto i = 0; i < texts_to_draw; ++i)
	{
		float current_sec = i * step_size;
		Vector2 current_pixel = Vector2(current_sec * get_size().x / full_time + 1, 0);
		int minutes = current_sec / 60;
		float seconds = current_sec - minutes * 60;
		String opt_zero = seconds < 10 ? "0" : "";
		draw_line(current_pixel, current_pixel + Vector2(0, get_size().y - 1), Color(1.0, 1.0, 1.0));
		draw_string(font, current_pixel + Vector2(3, 20), String().format(Variant({ minutes, opt_zero, seconds }), "%s:%s%s"));
	}

	float cursor_position = player->replay_cursor_position_percent * get_size().x;
	PackedVector2Array points = PackedVector2Array({ Vector2(cursor_position, get_size().y), Vector2(cursor_position - 8, -1), Vector2(cursor_position + 8, -1) });

	draw_polygon(points, { Color(1.0, 1.0, 1.0) });
}

AudioToMidiTimeLineBar::AudioToMidiTimeLineBar(ASA::Options* p_options, ASANode *p_spectrum_analyzer, AdvancedAudioStreamPlayer* p_player)
	: options(p_options), spectrum_analyzer(p_spectrum_analyzer), player(p_player)
{
}

// ==========================================================================
void ASAConversionOptionsDialog::_notification(int p_what)
{
	switch (p_what)
	{
	case NOTIFICATION_READY:
		//we want to display the(default) values from the options singleton
		//fft options
		two_ffts_option->set_pressed(options->fft.use_2_ffts);
		fft_size_low_option->set_value(options->fft.fft_size_low);
		low_high_exponent_low_option->set_value(options->fft.low_high_exponent_low);
		overamplification_low_option->set_value(options->fft.overamplification_multiplier_low);
		fft_size_high_option->set_value(options->fft.fft_size_high);
		low_high_exponent_high_option->set_value(options->fft.low_high_exponent_high);
		overamplification_high_option->set_value(options->fft.overamplification_multiplier_high);
		hop_size_option->set_value(options->fft.hop_size);
		subdivision_option->set_value(options->fft.subdivision);
		tuning_option->set_value(options->fft.tuning);
		//recognition options
		note_on_threshold_option->set_value(options->note_recognition.note_on_threshold);
		note_off_threshold_option->set_value(options->note_recognition.note_off_threshold);
		octave_removal_option->set_value(options->note_recognition.octave_removal_multiplier);
		minimum_length_option->set_value(options->note_recognition.minimum_length);
		volume_multiplier_option->set_value(options->note_recognition.volume_multiplier);
		percussion_removal_option->set_value(options->note_recognition.percussion_removal);
		break;
	default:
		break;
	}
}
void ASAConversionOptionsDialog::read_in_fft_options()
{
	//this will be called from spectrum_analyzer, to
	//make sure that it calculates with these options
	options->fft.use_2_ffts = two_ffts_option->is_pressed();
	options->fft.fft_size_low = fft_size_low_option->get_value();
	options->fft.low_high_exponent_low = low_high_exponent_low_option->get_value();
	options->fft.overamplification_multiplier_low = overamplification_low_option->get_value();
	options->fft.fft_size_high = fft_size_high_option->get_value();
	options->fft.low_high_exponent_high = low_high_exponent_high_option->get_value();
	options->fft.overamplification_multiplier_high = overamplification_high_option->get_value();
	options->fft.hop_size = hop_size_option->get_value();
	options->fft.subdivision = subdivision_option->get_value();
	options->fft.tuning = tuning_option->get_value();
}
void ASAConversionOptionsDialog::read_in_note_recognition_options()
{
	options->note_recognition.note_on_threshold = note_on_threshold_option->get_value();
	options->note_recognition.note_off_threshold = note_off_threshold_option->get_value();
	options->note_recognition.octave_removal_multiplier = octave_removal_option->get_value();
	options->note_recognition.minimum_length = minimum_length_option->get_value();
	options->note_recognition.volume_multiplier = volume_multiplier_option->get_value();
	options->note_recognition.percussion_removal = percussion_removal_option->get_value();
}

void ASAConversionOptionsDialog::_on_two_ffts_option_toggled(bool p_button_pressed)
{
	fft_size_high_option->set_editable(p_button_pressed);
	low_high_exponent_high_option->set_editable(p_button_pressed);
	overamplification_high_option->set_editable(p_button_pressed);
}

void ASAConversionOptionsDialog::_on_recalculate_fft_pressed()
{
	if (options->misc.file_path != "")
		advanced_spectrum_midi_player->calculate_new_spectrum_sprites();
}
void ASAConversionOptionsDialog::_on_recalculate_notes_pressed()
{
	if (options->misc.file_path != "" )
	{
		advanced_spectrum_midi_player->calculate_new_notes();
	}
}

void ASAConversionOptionsDialog::_on_recalculate_both_pressed()
{
	if (options->misc.file_path != "" )
	{
		advanced_spectrum_midi_player->calculate_new_spectrum_sprites();
		advanced_spectrum_midi_player->calculate_new_notes();
	}
}

// ==========================================================================
ASAConversionOptionsDialog::ASAConversionOptionsDialog(ASA::Options* p_options, ASAGraphScrollContainer* p_advanced_spectrum_midi_player)
	: options(p_options), advanced_spectrum_midi_player(p_advanced_spectrum_midi_player)
{
}

void AudioToMidiTimelineScrollContainer::_notification(int p_what)
{
}

// ==========================================================================
AudioToMidiTimelineScrollContainer::AudioToMidiTimelineScrollContainer()
{
	timeline_bar = memnew(AudioToMidiTimeLineBar());
}

// ==========================================================================
void AdvancedSpectrumAnalyzerPlugin::_bind_methods()
{
}

void AdvancedSpectrumAnalyzerPlugin::_window_changed(bool p_visible)
{
	make_floating->set_visible(!p_visible);
}
void AdvancedSpectrumAnalyzerPlugin::_menu_item_pressed(int p_index)
{
	switch (p_index) {
	case ASA_OPEN_AUDIO: {
		//String base_path = FileSystemDock::get_singleton()->get_current_path().get_base_dir();
	} break;
	case ASA_SAVE_MIDI: {
		String base_path = FileSystemDock::get_singleton()->get_current_path().get_base_dir();
		//EditorNode::get_singleton()->save_resource(edited_shaders[index].shader);
		//shader_create_dialog->config(base_path.path_join("new_shader"), false, false, 2);
		//shader_create_dialog->popup_centered();
	} break;
	case ASA_SHOW_CONVERSION_OPTIONS: {
		audio_to_midi_editor->_on_conversion_options_button_pressed();
	} break;
	case ASA_SHOW_GENERAL_OPTIONS: {
		audio_to_midi_editor->_on_general_options_button_pressed();
	} break;

	default:
		break;
	}
}

void AdvancedSpectrumAnalyzerPlugin::set_edited_audio(const Ref<AudioStream>& p_audio_stream)
{
	edited_audio = p_audio_stream;
	advanced_audio_stream_player->set_stream(edited_audio);
}

AdvancedSpectrumAnalyzerPlugin::AdvancedSpectrumAnalyzerPlugin()
{
	window_wrapper = memnew(WindowWrapper);
	window_wrapper->set_window_title(TTR("Advanced Spectrum Analyzer"));
	window_wrapper->set_margins_enabled(true);
	
	advanced_audio_stream_player = memnew(AdvancedAudioStreamPlayer);
	add_child(advanced_audio_stream_player);

	VBoxContainer* vb = memnew(VBoxContainer);
	HBoxContainer* menu_hb = memnew(HBoxContainer);
	vb->add_child(menu_hb);

	main_split = memnew(HSplitContainer);
	Ref<Shortcut> make_floating_shortcut = ED_SHORTCUT_AND_COMMAND("advanced_spectrum_analyzer_editor/make_floating", TTR("Make Floating"));
	window_wrapper->set_wrapped_control(main_split, make_floating_shortcut);
	file_menu = memnew(MenuButton);
	file_menu->set_text(TTR("File"));
	file_menu->get_popup()->add_item(TTR("Open audio..."), ASA_OPEN_AUDIO);
	file_menu->get_popup()->add_item(TTR("Save midifile..."), ASA_SAVE_MIDI);
	file_menu->get_popup()->add_separator();
	file_menu->get_popup()->add_item(TTR("Show conversion options"), ASA_SHOW_CONVERSION_OPTIONS);
	file_menu->get_popup()->add_item(TTR("Show general options"), ASA_SHOW_GENERAL_OPTIONS);
	file_menu->get_popup()->add_separator();
	file_menu->get_popup()->connect("id_pressed", callable_mp(this, &AdvancedSpectrumAnalyzerPlugin::_menu_item_pressed));
	menu_hb->add_child(file_menu);

	if (window_wrapper->is_window_available()) {
		Control* padding = memnew(Control);
		padding->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		menu_hb->add_child(padding);

		make_floating = memnew(ScreenSelect);
		make_floating->set_flat(true);
		make_floating->set_tooltip_text(TTR("Make the shader editor floating."));
		make_floating->connect("request_open_in_screen", callable_mp(window_wrapper, &WindowWrapper::enable_window_on_screen).bind(true));

		menu_hb->add_child(make_floating);
		window_wrapper->connect("window_visibility_changed", callable_mp(this, &AdvancedSpectrumAnalyzerPlugin::_window_changed));
	}
		
	button = EditorNode::get_singleton()->add_bottom_panel_item(TTR("AdvancedSpectrumAnalyzer"), window_wrapper);

	HBoxContainer *timeline_container = memnew(HBoxContainer);
	timeline_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_split->add_child(timeline_container);

	audio_to_midi_editor = memnew(AudioToMidiEditor);
	audio_to_midi_editor->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	audio_to_midi_editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	timeline_container->add_child(audio_to_midi_editor);
}

AdvancedSpectrumAnalyzerPlugin::~AdvancedSpectrumAnalyzerPlugin()
{
}
