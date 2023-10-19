/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCentre
*/

#define			_DEFAULT_SOURCE
#include		<stdbool.h>
#include		<string.h>
#include		<sys/types.h>
#include		<dirent.h>
#include		<stdio.h>

bool			retrieve_all_files(char			*path,
					   size_t		maxpath,
					   char			**files,
					   size_t		*browse,
					   size_t		cells,
					   size_t		cellsize,
					   const char		*ext)
{
  char			*basepath;
  int			curlen;
  DIR			*dir;
  struct dirent		*dirent;

  basepath = &path[curlen = strlen(path)];
  dir = opendir(path);
  while ((dirent = readdir(dir)))
    {
      if (dirent->d_name[0] == '.')
	continue ;
      if (dirent->d_type != DT_DIR)
	{
	  if (strncmp(ext, &dirent->d_name[strlen(dirent->d_name) - strlen(ext)], strlen(ext)) != 0)
	    continue ;
	  snprintf(files[*browse], cellsize, "%s/%s", path, dirent->d_name);
	  if (*browse + 1 >= cells)
	    return (false);
	  *browse += 1;
	}
      else
	{
	  if (curlen + strlen(dirent->d_name) >= maxpath)
	    {
	      strcat(basepath, dirent->d_name);
	      if (!retrieve_all_files(path, maxpath, files, browse, cells, cellsize, ext))
		return (false);
	      *basepath = '\0';
	    }
	  else
	    return (false);
	}
    }
  closedir(dir);
  return (true);
}
