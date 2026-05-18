/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
**
** TechnoCore
*/

#include		<assert.h>
#include		<string.h>
#include		<unistd.h>
#include		"technocore.h"

static void		check_match(void)
{
  t_audio_diff_configuration cnf;
  t_audio_diff		diff;
  int16_t		ref[8] = {0, 1000, -1000, 2048, -2048, 12, 42, -42};
  int16_t		user[8] = {0, 1001, -1002, 2048, -2047, 12, 42, -42};
  int16_t		out[8];

  init_audio_diff_configuration(&cnf);
  assert(cnf.amplitude_tolerance == 512);
  assert(cnf.neighbor_radius == 0);
  assert(cnf.sample_rate == 44100);
  assert(cnf.channels == 1);
  assert(tc_audio_compare_pcm16(user, NBRCELL(user), ref, NBRCELL(ref),
				 &cnf, &diff, out));
  assert(diff.checked_samples == NBRCELL(ref));
  assert(diff.matched_samples == NBRCELL(ref));
  assert(diff.mismatched_samples == 0);
  assert(diff.max_delta <= cnf.amplitude_tolerance);
}

static void		check_mismatch(void)
{
  t_audio_diff_configuration cnf;
  t_audio_diff		diff;
  int16_t		ref[4] = {0, 1000, -1000, 0};
  int16_t		user[4] = {0, 1000, 9000, 0};

  init_audio_diff_configuration(&cnf);
  cnf.amplitude_tolerance = 32;
  assert(!tc_audio_compare_pcm16(user, NBRCELL(user), ref, NBRCELL(ref),
				  &cnf, &diff, NULL));
  assert(diff.mismatched_samples == 1);
  assert(diff.max_delta > cnf.amplitude_tolerance);
}

static void		check_neighbor(void)
{
  t_audio_diff_configuration cnf;
  t_audio_diff		diff;
  int16_t		ref[5] = {0, 0, 32000, 0, 0};
  int16_t		user[5] = {0, 0, 0, 32000, 0};

  init_audio_diff_configuration(&cnf);
  cnf.amplitude_tolerance = 0;
  assert(!tc_audio_compare_pcm16(user, NBRCELL(user), ref, NBRCELL(ref),
				  &cnf, &diff, NULL));
  cnf.neighbor_radius = 1;
  assert(tc_audio_compare_pcm16(user, NBRCELL(user), ref, NBRCELL(ref),
				 &cnf, &diff, NULL));
}

static void		check_wav(void)
{
  int16_t		samples[6] = {0, INT16_MAX, INT16_MIN, 123, -456, 0};

  unlink("tc_audio_test.wav");
  assert(tc_audio_save_wav_pcm16("tc_audio_test.wav", samples,
				 NBRCELL(samples), 44100, 1));
  assert(file_exists("tc_audio_test.wav"));
  unlink("tc_audio_test.wav");
  assert(tc_audio_save_wav_pcm16("tc_audio_test.wav", samples,
				 NBRCELL(samples), 48000, 2));
  assert(file_exists("tc_audio_test.wav"));
  unlink("tc_audio_test.wav");
}

static void		check_do_audio_diff(void)
{
  t_technocore_activity	act;
  int16_t		ref[2] = {0, 1000};
  int16_t		user[2] = {0, 9000};

  memset(&act, 0, sizeof(act));
  assert(dict_open());
  assert(dict_set_language("EN"));
  assert(act.current_report = bunny_new_configuration());
  assert(do_audio_diff(&act, "wave", user, NBRCELL(user),
		       ref, NBRCELL(ref), NULL) == TC_FAILURE);
  assert(act.nbr_artefacts == 3);
  assert(file_exists(act.artefacts[0]));
  assert(file_exists(act.artefacts[1]));
  assert(file_exists(act.artefacts[2]));
  for (size_t i = 0; i < act.nbr_artefacts; ++i)
    {
      unlink(act.artefacts[i]);
      bunny_free(act.artefacts[i]);
    }
  bunny_delete_configuration(act.current_report);
}

int			main(void)
{
  check_match();
  check_mismatch();
  check_neighbor();
  check_wav();
  check_do_audio_diff();
  return (EXIT_SUCCESS);
}
