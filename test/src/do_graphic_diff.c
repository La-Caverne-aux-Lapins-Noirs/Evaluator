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

static unsigned int	make_color(unsigned char			red,
					   unsigned char			green,
					   unsigned char			blue,
					   unsigned char			alpha)
{
  t_color		col;

  col.full = 0;
  col.argb[ALPHA_CMP] = alpha;
  col.argb[RED_CMP] = red;
  col.argb[GREEN_CMP] = green;
  col.argb[BLUE_CMP] = blue;
  return (col.full);
}

static void		check_match(void)
{
  t_graphic_diff_configuration cnf;
  t_graphic_diff	diff;
  unsigned int		ref[9];
  unsigned int		user[9];
  unsigned int		out[9];

  memset(ref, 0, sizeof(ref));
  memset(user, 0, sizeof(user));
  init_graphic_diff_configuration(&cnf);
  assert(cnf.component_tolerance == 40);
  assert(cnf.neighbor_radius == 1);
  ref[4] = make_color(128, 64, 32, 255);
  user[4] = ref[4];
  assert(tc_graphic_compare_buffers(user, ref, 3, 3, &cnf, &diff, out));
  assert(diff.compared_pixels == 1);
  assert(diff.matched_pixels == 1);
  assert(diff.mismatched_pixels == 0);
  assert(diff.trap_pixels == 0);
}

static void		check_neighbor(void)
{
  t_graphic_diff_configuration cnf;
  t_graphic_diff	diff;
  unsigned int		ref[9];
  unsigned int		user[9];

  memset(ref, 0, sizeof(ref));
  memset(user, 0, sizeof(user));
  init_graphic_diff_configuration(&cnf);
  ref[4] = make_color(255, 0, 0, 255);
  user[5] = ref[4];
  assert(tc_graphic_compare_buffers(user, ref, 3, 3, &cnf, &diff, NULL));
  cnf.neighbor_radius = 0;
  assert(!tc_graphic_compare_buffers(user, ref, 3, 3, &cnf, &diff, NULL));
  assert(diff.mismatched_pixels == 1);
}

static void		check_trap(void)
{
  t_graphic_diff_configuration cnf;
  t_graphic_diff	diff;
  unsigned int		ref[1];
  unsigned int		user[1];

  init_graphic_diff_configuration(&cnf);
  ref[0] = make_color(10, 20, 30, 5);
  user[0] = make_color(10, 20, 31, 5);
  assert(!tc_graphic_compare_buffers(user, ref, 1, 1, &cnf, &diff, NULL));
  assert(diff.trap_pixels == 1);
  assert(diff.mismatched_pixels == 0);
}

static void		check_bmp(void)
{
  unsigned int		pixels[6];

  for (size_t i = 0; i < NBRCELL(pixels); ++i)
    pixels[i] = make_color(i * 20, i * 10, i * 5, 255);
  unlink("tc_graphic_test.bmp");
  assert(tc_graphic_save_bmp24("tc_graphic_test.bmp", pixels, 3, 2));
  assert(file_exists("tc_graphic_test.bmp"));
  unlink("tc_graphic_test.bmp");
  assert(tc_graphic_save_bmp24("tc_graphic_test.bmp", pixels, 4, 1));
  assert(file_exists("tc_graphic_test.bmp"));
  unlink("tc_graphic_test.bmp");
}

int			main(void)
{
  check_match();
  check_neighbor();
  check_trap();
  check_bmp();
  return (EXIT_SUCCESS);
}
