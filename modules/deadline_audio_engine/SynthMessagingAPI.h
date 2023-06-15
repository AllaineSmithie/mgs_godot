#ifndef SYNTH_MESSAGING_API_H
#define SYNTH_MESSAGING_API_H

#include <core/string/ustring.h>

namespace SynthMessagingAPI
{
	const char* hello = "/hello";
	const char* num_regions = "/num_regions";
	const char* num_groups = "/num_groups";
	const char* num_masters = "/num_masters";
	const char* num_curves = "/num_curves";
	const char* num_samples = "/num_samples";
	const char* octave_offset = "/octave_offset";
	const char* note_offset = "/note_offset";
	const char* num_outputs = "/num_outputs";

	//----------------------------------------------------------------------

	const char* key_slots = "/key/slots";
	const char* keyN_label = "/key&/label";

	//----------------------------------------------------------------------

	const char* root_path = "/root_path";
	const char* image = "/image";
	const char* image_controls = "/image_controls";

	//----------------------------------------------------------------------

	const char* sw_last_slots = "/sw/last/slots";
	const char* sw_last_current = "/sw/last/current";
	const char* sw_last_N_label = "/sw/last/&/label";

	//----------------------------------------------------------------------

	const char* cc_slots = "/cc/slots";
	const char* ccN_default = "/cc&/default";
	const char* ccN_value = "/cc&/value";
	const char* ccN_label = "/cc&/label";
	const char* cc_changed = "/cc/changed";
	const char* cc_changed_tilde = "/cc/changed~";
	const char* sustain_or_sostenuto_slots = "/sustain_or_sostenuto/slots";
	const char* aftertouch = "/aftertouch";
	const char* poly_aftertouch_N = "/poly_aftertouch/&";
	const char* pitch_bend = "/pitch_bend";

	//----------------------------------------------------------------------

	const char* mem_buffers = "/mem/buffers";

	//----------------------------------------------------------------------

	const char* regionN_delay = "/region&/delay";
	const char* regionN_sample = "/region&/sample";
	const char* regionN_direction = "/region&/direction";
	const char* regionN_delay_random = "/region&/delay_random";
	const char* regionN_delay_ccN = "/region&/delay_cc&";
	const char* regionN_offset = "/region&/offset";
	const char* regionN_offset_random = "/region&/offset_random";
	const char* regionN_offset_ccN = "/region&/offset_cc&";
	const char* regionN_end = "/region&/end";
	const char* regionN_end_ccN = "/region&/end_cc&";
	const char* regionN_enabled = "/region&/enabled";
	const char* regionN_trigger_on_note = "/region&/trigger_on_note";
	const char* regionN_trigger_on_cc = "/region&/trigger_on_cc";
	const char* regionN_count = "/region&/count";
	const char* regionN_loop_range = "/region&/loop_range";
	const char* regionN_loop_start_ccN = "/region&/loop_start_cc&";
	const char* regionN_loop_end_ccN = "/region&/loop_end_cc&";
	const char* regionN_loop_mode = "/region&/loop_mode";
	const char* regionN_loop_crossfade = "/region&/loop_crossfade";
	const char* regionN_loop_count = "/region&/loop_count";
	const char* regionN_output = "/region&/output";
	const char* regionN_group = "/region&/group";
	const char* regionN_off_by = "/region&/off_by";
	const char* regionN_off_mode = "/region&/off_mode";
	const char* regionN_key_range = "/region&/key_range";
	const char* regionN_off_time = "/region&/off_time";
	const char* regionN_pitch_keycenter = "/region&/pitch_keycenter";
	const char* regionN_vel_range = "/region&/vel_range";
	const char* regionN_bend_range = "/region&/bend_range";
	const char* regionN_program_range = "/region&/program_range";
	const char* regionN_cc_rangeN = "/region&/cc_range&";
	const char* regionN_sw_last = "/region&/sw_last";
	const char* regionN_sw_label = "/region&/sw_label";
	const char* regionN_sw_up = "/region&/sw_up";
	const char* regionN_sw_down = "/region&/sw_down";
	const char* regionN_sw_previous = "/region&/sw_previous";
	const char* regionN_sw_vel = "/region&/sw_vel";
	const char* regionN_chanaft_range = "/region&/chanaft_range";
	const char* regionN_polyaft_range = "/region&/polyaft_range";
	const char* regionN_bpm_range = "/region&/bpm_range";
	const char* regionN_rand_range = "/region&/rand_range";
	const char* regionN_seq_length = "/region&/seq_length";
	const char* regionN_seq_position = "/region&/seq_position";
	const char* regionN_trigger = "/region&/trigger";
	const char* regionN_start_cc_rangeN = "/region&/start_cc_range&";
	const char* regionN_volume = "/region&/volume";
	const char* regionN_volume_ccN = "/region&/volume_cc&";
	const char* regionN_volume_stepccN = "/region&/volume_stepcc&";
	const char* regionN_volume_smoothccN = "/region&/volume_smoothcc&";
	const char* regionN_volume_curveccN = "/region&/volume_curvecc&";
	const char* regionN_pan = "/region&/pan";
	const char* regionN_pan_ccN = "/region&/pan_cc&";
	const char* regionN_pan_stepccN = "/region&/pan_stepcc&";
	const char* regionN_pan_smoothccN = "/region&/pan_smoothcc&";
	const char* regionN_pan_curveccN = "/region&/pan_curvecc&";
	const char* regionN_width = "/region&/width";
	const char* regionN_width_ccN = "/region&/width_cc&";
	const char* regionN_width_stepccN = "/region&/width_stepcc&";
	const char* regionN_width_smoothccN = "/region&/width_smoothcc&";
	const char* regionN_width_curveccN = "/region&/width_curvecc&";
	const char* regionN_position = "/region&/position";
	const char* regionN_position_ccN = "/region&/position_cc&";
	const char* regionN_position_stepccN = "/region&/position_stepcc&";
	const char* regionN_position_smoothccN = "/region&/position_smoothcc&";
	const char* regionN_position_curveccN = "/region&/position_curvecc&";
	const char* regionN_amplitude = "/region&/amplitude";
	const char* regionN_amplitude_ccN = "/region&/amplitude_cc&";
	const char* regionN_amplitude_stepccN = "/region&/amplitude_stepcc&";
	const char* regionN_amplitude_smoothccN = "/region&/amplitude_smoothcc&";
	const char* regionN_amplitude_curveccN = "/region&/amplitude_curvecc&";
	const char* regionN_amp_keycenter = "/region&/amp_keycenter";
	const char* regionN_amp_keytrack = "/region&/amp_keytrack";
	const char* regionN_amp_veltrack = "/region&/amp_veltrack";
	const char* regionN_amp_veltrack_ccN = "/region&/amp_veltrack_cc&";
	const char* regionN_amp_veltrack_curveccN = "/region&/amp_veltrack_curvecc&";
	const char* regionN_amp_random = "/region&/amp_random";
	const char* regionN_xfin_key_range = "/region&/xfin_key_range";
	const char* regionN_xfout_key_range = "/region&/xfout_key_range";
	const char* regionN_xfin_vel_range = "/region&/xfin_vel_range";
	const char* regionN_xfout_vel_range = "/region&/xfout_vel_range";
	const char* regionN_xfin_cc_rangeN = "/region&/xfin_cc_range&";
	const char* regionN_xfout_cc_rangeN = "/region&/xfout_cc_range&";
	const char* regionN_xf_keycurve = "/region&/xf_keycurve";
	const char* regionN_xf_velcurve = "/region&/xf_velcurve";
	const char* regionN_xf_cccurve = "/region&/xf_cccurve";
	const char* regionN_global_volume = "/region&/global_volume";
	const char* regionN_master_volume = "/region&/master_volume";
	const char* regionN_group_volume = "/region&/group_volume";
	const char* regionN_global_amplitude = "/region&/global_amplitude";
	const char* regionN_master_amplitude = "/region&/master_amplitude";
	const char* regionN_group_amplitude = "/region&/group_amplitude";
	const char* regionN_pitch_keytrack = "/region&/pitch_keytrack";
	const char* regionN_pitch_veltrack = "/region&/pitch_veltrack";
	const char* regionN_pitch_veltrack_ccN = "/region&/pitch_veltrack_cc&";
	const char* regionN_pitch_veltrack_curveccN = "/region&/pitch_veltrack_curvecc&";
	const char* regionN_pitch_random = "/region&/pitch_random";
	const char* regionN_transpose = "/region&/transpose";
	const char* regionN_pitch = "/region&/pitch";
	const char* regionN_pitch_ccN = "/region&/pitch_cc&";
	const char* regionN_pitch_stepccN = "/region&/pitch_stepcc&";
	const char* regionN_pitch_smoothccN = "/region&/pitch_smoothcc&";
	const char* regionN_pitch_curveccN = "/region&/pitch_curvecc&";
	const char* regionN_bend_up = "/region&/bend_up";
	const char* regionN_bend_down = "/region&/bend_down";
	const char* regionN_bend_step = "/region&/bend_step";
	const char* regionN_bend_smooth = "/region&/bend_smooth";
	const char* regionN_ampeg_attack = "/region&/ampeg_attack";
	const char* regionN_ampeg_delay = "/region&/ampeg_delay";
	const char* regionN_ampeg_decay = "/region&/ampeg_decay";
	const char* regionN_ampeg_hold = "/region&/ampeg_hold";
	const char* regionN_ampeg_release = "/region&/ampeg_release";
	const char* regionN_ampeg_start = "/region&/ampeg_start";
	const char* regionN_ampeg_sustain = "/region&/ampeg_sustain";
	const char* regionN_ampeg_depth = "/region&/ampeg_depth";
	const char* regionN_ampeg_velNattack = "/region&/ampeg_vel&attack";
	const char* regionN_ampeg_velNdelay = "/region&/ampeg_vel&delay";
	const char* regionN_ampeg_velNdecay = "/region&/ampeg_vel&decay";
	const char* regionN_ampeg_velNhold = "/region&/ampeg_vel&hold";
	const char* regionN_ampeg_velNrelease = "/region&/ampeg_vel&release";
	const char* regionN_ampeg_velNsustain = "/region&/ampeg_vel&sustain";
	const char* regionN_ampeg_velNdepth = "/region&/ampeg_vel&depth";
	const char* regionN_ampeg_dynamic = "/region&/ampeg_dynamic";
	const char* regionN_fileg_dynamic = "/region&/fileg_dynamic";
	const char* regionN_pitcheg_dynamic = "/region&/pitcheg_dynamic";
	const char* regionN_note_polyphony = "/region&/note_polyphony";
	const char* regionN_note_selfmask = "/region&/note_selfmask";
	const char* regionN_rt_dead = "/region&/rt_dead";
	const char* regionN_sustain_sw = "/region&/sustain_sw";
	const char* regionN_sostenuto_sw = "/region&/sostenuto_sw";
	const char* regionN_sustain_cc = "/region&/sustain_cc";
	const char* regionN_sostenuto_cc = "/region&/sostenuto_cc";
	const char* regionN_sustain_lo = "/region&/sustain_lo";
	const char* regionN_sostenuto_lo = "/region&/sostenuto_lo";
	const char* regionN_oscillator_phase = "/region&/oscillator_phase";
	const char* regionN_oscillator_quality = "/region&/oscillator_quality";
	const char* regionN_oscillator_mode = "/region&/oscillator_mode";
	const char* regionN_oscillator_multi = "/region&/oscillator_multi";
	const char* regionN_oscillator_detune = "/region&/oscillator_detune";
	const char* regionN_oscillator_mod_depth = "/region&/oscillator_mod_depth";

	// TODO: detune cc, mod depth cc

	const char* regionN_effectN = "/region&/effect&";
	const char* regionN_ampeg_attack_ccN = "/region&/ampeg_attack_cc&";
	const char* regionN_ampeg_decay_ccN = "/region&/ampeg_decay_cc&";
	const char* regionN_ampeg_delay_ccN = "/region&/ampeg_delay_cc&";
	const char* regionN_ampeg_hold_ccN = "/region&/ampeg_hold_cc&";
	const char* regionN_ampeg_release_ccN = "/region&/ampeg_release_cc&";
	const char* regionN_ampeg_start_ccN = "/region&/ampeg_start_cc&";
	const char* regionN_ampeg_sustain_ccN = "/region&/ampeg_sustain_cc&";
	const char* regionN_filterN_cutoff = "/region&/filter&/cutoff";
	const char* regionN_filterN_resonance = "/region&/filter&/resonance";
	const char* regionN_filterN_gain = "/region&/filter&/gain";
	const char* regionN_filterN_keycenter = "/region&/filter&/keycenter";
	const char* regionN_filterN_keytrack = "/region&/filter&/keytrack";
	const char* regionN_filterN_veltrack = "/region&/filter&/veltrack";
	const char* regionN_filterN_veltrack_ccN = "/region&/filter&/veltrack_cc&";
	const char* regionN_filterN_veltrack_curveccN = "/region&/filter&/veltrack_curvecc&";
	const char* regionN_filterN_type = "/region&/filter&/type";
	const char* regionN_eqN_gain = "/region&/eq&/gain";
	const char* regionN_eqN_bandwidth = "/region&/eq&/bandwidth";
	const char* regionN_eqN_frequency = "/region&/eq&/frequency";
	const char* regionN_eqN_velNfreq = "/region&/eq&/vel&freq";
	const char* regionN_eqN_velNgain = "/region&/eq&/vel&gain";
	const char* regionN_eqN_type = "/region&/eq&/type";
	const char* regionN_lfoN_wave = "/region&/lfo&/wave";
	const char* regionN_egN_pointN_time = "/region&/eg&/point&/time";
	const char* regionN_egN_pointN_time_ccN = "/region&/eg&/point&/time_cc&";
	const char* regionN_egN_pointN_level = "/region&/eg&/point&/level";
	const char* regionN_egN_pointN_level_ccN = "/region&/eg&/point&/level_cc&";

	//----------------------------------------------------------------------
	// Setting values
	// Note: all these must be rt-safe within the parseOpcode method in region

	const char* set_sample_quality = "/sample_quality";
	const char* set_oscillator_quality = "/oscillator_quality";
	const char* set_freewheeling_sample_quality = "/freewheeling_sample_quality";
	const char* set_freewheeling_oscillator_quality = "/freewheeling_oscillator_quality";
	const char* set_sustain_cancels_release = "/sustain_cancels_release";
	const char* set_regionN_pitch_keycenter = regionN_pitch_keycenter;
	const char* set_regionN_loop_mode = regionN_loop_mode;
	const char* set_regionN_filterN_type = regionN_filterN_type;
	const char* set_regionN_lfoN_wave = regionN_lfoN_wave;
	const char* set_regionN_lfoN_waveN = "/region&/lfo&/wave&";

	//----------------------------------------------------------------------
	// Voices

	const char* num_active_voices = "/num_active_voices";
	const char* voiceN_trigger_value = "/voice&/trigger_value";
	const char* voiceN_trigger_number = "/voice&/trigger_number";
	const char* voiceN_trigger_type = "/voice&/trigger_type";
	const char* voiceN_remaining_delay = "/voice&/remaining_delay";
	const char* voiceN_source_position = "/voice&/source_position";

}
#endif
