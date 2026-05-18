/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2026
**
** TechnoCore
*/

#include			"technocore.h"

void				set_u16le
(unsigned char			*buf,
 uint16_t			value)
{
  buf[0] = value & 0xFF;
  buf[1] = (value >> 8) & 0xFF;
}

void				set_u32le
(unsigned char			*buf,
 uint32_t			value)
{
  buf[0] = value & 0xFF;
  buf[1] = (value >> 8) & 0xFF;
  buf[2] = (value >> 16) & 0xFF;
  buf[3] = (value >> 24) & 0xFF;
}

