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
					   size_t		*browse,
					   size_t		cells,
					   size_t		cellsize,
					   char			files
					   [cells][cellsize],
					   const char		*ext)
{
  char			*basepath;
  size_t		curlen;
  DIR			*dir;
  struct dirent		*dirent;

  for (curlen = 0; path[curlen] != '\0'; curlen++);
  basepath = &path[curlen];
  if ((dir = opendir(path)) == false)
    return (false);
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
	  if (curlen + strlen(dirent->d_name) + 1 < maxpath)
	    {
	      basepath[0] = '/';
	      basepath[1] = '\0';
	      strcat(&basepath[1], dirent->d_name);
	      if (!retrieve_all_files(path, maxpath, browse, cells, cellsize, files, ext))
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
