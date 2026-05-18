/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
**
** TechnoCore
*/

#include			<ctype.h>
#include			<limits.h>
#include			<stdint.h>
#include			<stdio.h>
#include			<stdlib.h>
#include			<string.h>
#include			"technocore.h"

static unsigned int		make_color
(unsigned char			red,
 unsigned char			green,
 unsigned char			blue)
{
  t_color			col;

  col.full = 0;
  col.argb[ALPHA_CMP] = 255;
  col.argb[RED_CMP] = red;
  col.argb[GREEN_CMP] = green;
  col.argb[BLUE_CMP] = blue;
  return (col.full);
}

void				init_graphic_diff_configuration
(t_graphic_diff_configuration	*cnf)
{
  if (cnf == NULL)
    return ;
  cnf->component_tolerance = 40;
  cnf->neighbor_radius = 1;
  cnf->ignore_zero_reference = true;
  cnf->use_alpha_traps = true;
}

static const t_graphic_diff_configuration *get_cnf
(const t_graphic_diff_configuration *cnf,
 t_graphic_diff_configuration	*local)
{
  if (cnf != NULL)
    return (cnf);
  init_graphic_diff_configuration(local);
  return (local);
}

static bool			is_prime_alpha
(unsigned char			alpha)
{
  if (alpha < 2)
    return (false);
  for (unsigned int i = 2; i * i <= alpha; ++i)
    if (alpha % i == 0)
      return (false);
  return (true);
}

static bool			close_component
(unsigned char			a,
 unsigned char			b,
 int				tolerance)
{
  return (abs((int)a - (int)b) <= tolerance);
}

static bool			close_color
(unsigned int			a,
 unsigned int			b,
 int				tolerance)
{
  t_color			ca;
  t_color			cb;
  
  ca.full = a;
  cb.full = b;
  for (int i = 0; i < 4; ++i)
    if (close_component(ca.argb[i], cb.argb[i], tolerance) == false)
      return (false);
  return (true);
}

static bool			matching_neighbor
(const unsigned int		*user,
 unsigned int			ref,
 unsigned int			width,
 unsigned int			height,
 unsigned int			x,
 unsigned int			y,
 const t_graphic_diff_configuration *cnf)
{
  int				radius;

  radius = cnf->neighbor_radius;
  if (radius < 0)
    radius = 0;
  for (int dy = -radius; dy <= radius; ++dy)
    for (int dx = -radius; dx <= radius; ++dx)
      {
	int			nx = (int)x + dx;
	int			ny = (int)y + dy;

	if (nx < 0 || ny < 0 || nx >= (int)width || ny >= (int)height)
	  continue ;
	if (close_color(ref, user[nx + ny * width], cnf->component_tolerance))
	  return (true);
      }
  return (false);
}

static void			clear_graphic_diff
(t_graphic_diff			*diff,
 unsigned int			width,
 unsigned int			height)
{
  memset(diff, 0, sizeof(*diff));
  diff->width = width;
  diff->height = height;
}

bool				tc_graphic_compare_buffers
(const unsigned int		*user,
 const unsigned int		*ref,
 unsigned int			width,
 unsigned int			height,
 const t_graphic_diff_configuration *config,
 t_graphic_diff			*diff,
 unsigned int			*diff_pixels)
{
  t_graphic_diff_configuration	local;
  const t_graphic_diff_configuration *cnf;
  t_graphic_diff		local_diff;
  unsigned int			match_color;
  unsigned int			mismatch_color;
  unsigned int			trap_color;
  unsigned int			background_color;

  if (user == NULL || ref == NULL || width == 0 || height == 0)
    return (false);
  if (diff == NULL)
    diff = &local_diff;
  clear_graphic_diff(diff, width, height);
  cnf = get_cnf(config, &local);
  // Construction des couleurs du diff
  match_color = make_color(32, 32, 32);
  mismatch_color = make_color(255, 0, 0);
  trap_color = make_color(255, 0, 255);
  background_color = make_color(0, 0, 0);

  for (unsigned int y = 0; y < height; ++y)
    for (unsigned int x = 0; x < width; ++x)
      {
	unsigned int		index = x + y * width;
	t_color			refcol;

	refcol.full = ref[index];
	// On ignore les pixels non dessiné dans la reference
	// pour pouvoir remplir de bruit l'image étudiante
	if (cnf->ignore_zero_reference && ref[index] == 0)
	  {
	    if (diff_pixels != NULL)
	      diff_pixels[index] = background_color;
	    continue ;
	  }
	diff->compared_pixels += 1;
	if (cnf->use_alpha_traps && is_prime_alpha(refcol.argb[ALPHA_CMP]))
	  {
	    if (ref[index] != user[index])
	      {
		diff->trap_pixels += 1;
		if (diff_pixels != NULL)
		  diff_pixels[index] = trap_color;
		continue ;
	      }
	    diff->matched_pixels += 1;
	    if (diff_pixels != NULL)
	      diff_pixels[index] = match_color;
	    continue ;
	  }
	if (matching_neighbor(user, ref[index], width, height, x, y, cnf))
	  {
	    diff->matched_pixels += 1;
	    if (diff_pixels != NULL)
	      diff_pixels[index] = match_color;
	  }
	else
	  {
	    diff->mismatched_pixels += 1;
	    if (diff_pixels != NULL)
	      diff_pixels[index] = mismatch_color;
	  }
      }
  return (diff->mismatched_pixels == 0 && diff->trap_pixels == 0);
}

bool				tc_graphic_compare_pixelarray
(const t_bunny_pixelarray	*user,
 const t_bunny_pixelarray	*ref,
 const t_graphic_diff_configuration *cnf,
 t_graphic_diff			*diff,
 unsigned int			*diff_pixels)
{
  if (user == NULL || ref == NULL || user->pixels == NULL || ref->pixels == NULL)
    return (false);
  if (user->clipable.buffer.width != ref->clipable.buffer.width ||
      user->clipable.buffer.height != ref->clipable.buffer.height)
    return (false);
  return (tc_graphic_compare_buffers
	  ((const unsigned int*)user->pixels,
	   (const unsigned int*)ref->pixels,
	   ref->clipable.buffer.width,
	   ref->clipable.buffer.height,
	   cnf,
	   diff,
	   diff_pixels));
}

static bool			write_bmp_header
(FILE				*fp,
 unsigned int			width,
 unsigned int			height,
 uint32_t			row_size)
{
  unsigned char			file_header[14];
  unsigned char			dib_header[40];
  uint32_t			pixel_data_size;
  uint32_t			file_size;

  pixel_data_size = row_size * height;
  file_size = sizeof(file_header) + sizeof(dib_header) + pixel_data_size;
  memset(file_header, 0, sizeof(file_header));
  memset(dib_header, 0, sizeof(dib_header));
  file_header[0] = 'B';
  file_header[1] = 'M';
  set_u32le(&file_header[2], file_size);
  set_u32le(&file_header[10], sizeof(file_header) + sizeof(dib_header));
  set_u32le(&dib_header[0], sizeof(dib_header));
  set_u32le(&dib_header[4], width);
  set_u32le(&dib_header[8], height);
  set_u16le(&dib_header[12], 1);
  set_u16le(&dib_header[14], 24);
  set_u32le(&dib_header[20], pixel_data_size);
  if (fwrite(file_header, sizeof(file_header), 1, fp) != 1)
    return (false);
  return (fwrite(dib_header, sizeof(dib_header), 1, fp) == 1);
}

bool				tc_graphic_save_bmp24
(const char			*file,
 const unsigned int		*pixels,
 unsigned int			width,
 unsigned int			height)
{
  unsigned char			padding[3] = {0, 0, 0};
  uint32_t			row_size;
  FILE				*fp;

  if (file == NULL || pixels == NULL || width == 0 || height == 0)
    return (false);
  if (width > (UINT32_MAX - 3) / 3)
    return (false);
  row_size = ((width * 3 + 3) / 4) * 4;
  if (height > UINT32_MAX / row_size)
    return (false);
  if ((fp = fopen(file, "wb")) == NULL)
    return (false);
  if (write_bmp_header(fp, width, height, row_size) == false)
    goto Fail;
  for (int y = (int)height - 1; y >= 0; --y)
    {
      for (unsigned int x = 0; x < width; ++x)
	{
	  unsigned char	pixel[3];
	  t_color	col;

	  col.full = pixels[x + y * width];
	  pixel[0] = col.argb[BLUE_CMP];
	  pixel[1] = col.argb[GREEN_CMP];
	  pixel[2] = col.argb[RED_CMP];
	  if (fwrite(pixel, sizeof(pixel), 1, fp) != 1)
	    goto Fail;
	}
      if (row_size > width * 3 &&
	  fwrite(padding, row_size - width * 3, 1, fp) != 1)
	goto Fail;
    }
  return (fclose(fp) == 0);

 Fail:
  fclose(fp);
  return (false);
}

bool				tc_graphic_save_pixelarray_bmp24
(const char			*file,
 const t_bunny_pixelarray	*pix)
{
  if (pix == NULL || pix->pixels == NULL)
    return (false);
  return (tc_graphic_save_bmp24
	  (file,
	   (const unsigned int*)pix->pixels,
	   pix->clipable.buffer.width,
	   pix->clipable.buffer.height));
}

static void			sanitize_name
(char				*out,
 size_t				outlen,
 const char			*name)
{
  size_t			j;

  if (outlen == 0)
    return ;
  if (name == NULL || name[0] == '\0')
    name = "graphic";
  for (j = 0; name[j] && j + 1 < outlen; ++j)
    if (isalnum((unsigned char)name[j]) || name[j] == '_' || name[j] == '-')
      out[j] = name[j];
    else
      out[j] = '_';
  out[j] = '\0';
  if (j == 0)
    snprintf(out, outlen, "graphic");
}

static bool			make_artefact_name
(char				*out,
 size_t				outlen,
 const char			*name,
 size_t				index,
 const char			*suffix)
{
  char				safe[80];
  int				ret;

  sanitize_name(&safe[0], sizeof(safe), name);
  ret = snprintf(out, outlen, "graphic_%02zu_%s_%s.bmp", index, safe, suffix);
  return (ret > 0 && (size_t)ret < outlen);
}

static t_technocore_result	add_graphic_artefact
(t_technocore_activity		*act,
 const char			*file)
{
  if (add_artefact(act, file) == false)
    {
      add_message(&gl_technocore.error_buffer,
		  "Cannot add graphic artefact %s to report.\n", file);
      return (TC_CRITICAL);
    }
  return (TC_SUCCESS);
}

static t_technocore_result	write_graphic_artefacts
(t_technocore_activity		*act,
 const char			*name,
 const t_bunny_pixelarray	*user,
 const t_bunny_pixelarray	*ref,
 const unsigned int		*diff_pixels)
{
  char				user_file[160];
  char				ref_file[160];
  char				diff_file[160];
  size_t			index;

  index = act->nbr_artefacts;
  if (make_artefact_name(ref_file, sizeof(ref_file), name, index, "ref") == false ||
      make_artefact_name(user_file, sizeof(user_file), name, index, "user") == false ||
      make_artefact_name(diff_file, sizeof(diff_file), name, index, "diff") == false)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Cannot build graphic artefact names for %s.\n", name ? name : "graphic");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (tc_graphic_save_pixelarray_bmp24(ref_file, ref) == false ||
      tc_graphic_save_pixelarray_bmp24(user_file, user) == false ||
      tc_graphic_save_bmp24(diff_file, diff_pixels,
			    ref->clipable.buffer.width,
			    ref->clipable.buffer.height) == false)
    {
      add_message(&gl_technocore.error_buffer,
		  "Cannot write graphic artefacts for %s.\n", name ? name : "graphic");
      return (TC_CRITICAL);
    }
  if (add_graphic_artefact(act, ref_file) != TC_SUCCESS ||
      add_graphic_artefact(act, user_file) != TC_SUCCESS ||
      add_graphic_artefact(act, diff_file) != TC_SUCCESS)
    return (TC_CRITICAL);
  return (TC_SUCCESS);
}

t_technocore_result		do_graphic_diff
(t_technocore_activity		*act,
 const char			*name,
 const t_bunny_pixelarray	*user,
 const t_bunny_pixelarray	*ref,
 const t_graphic_diff_configuration *cnf)
{
  t_graphic_diff		diff;
  unsigned int			*diff_pixels;
  size_t			len;

  if (user == NULL || ref == NULL || user->pixels == NULL || ref->pixels == NULL)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Cannot compare null graphic buffers for %s.\n", name ? name : "graphic");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (user->clipable.buffer.width != ref->clipable.buffer.width ||
      user->clipable.buffer.height != ref->clipable.buffer.height)
    {
      if (add_exercise_message
	  (act, dict_get_pattern("GraphicDimensionsAreDifferent"),
	   name ? name : "graphic",
	   user->clipable.buffer.width,
	   user->clipable.buffer.height,
	   ref->clipable.buffer.width,
	   ref->clipable.buffer.height) == false)
	return (TC_CRITICAL);
      return (TC_FAILURE);
    }
  len = ref->clipable.buffer.width * ref->clipable.buffer.height;
  if ((diff_pixels = malloc(sizeof(*diff_pixels) * len)) == NULL)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Cannot allocate graphic diff for %s.\n", name ? name : "graphic");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (tc_graphic_compare_pixelarray(user, ref, cnf, &diff, diff_pixels))
    {
      free(diff_pixels);
      return (TC_SUCCESS);
    }
  if (write_graphic_artefacts(act, name, user, ref, diff_pixels) != TC_SUCCESS)
    {
      free(diff_pixels);
      return (TC_CRITICAL);
    }
  free(diff_pixels);
  if (add_exercise_message
      (act, dict_get_pattern("PicturesAreDifferent"),
       name ? name : "graphic",
       diff.mismatched_pixels,
       diff.trap_pixels,
       diff.compared_pixels) == false)
    return (TC_CRITICAL);
  return (TC_FAILURE);
}

