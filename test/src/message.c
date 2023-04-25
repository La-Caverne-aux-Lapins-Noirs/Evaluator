/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		<string.h>
#include		<assert.h>
#include		"technocore.h"

int			main(void)
{
  t_message		message;
  size_t		cnt;
  size_t		i;
  int			j;

  for (j = 0; j < 2; ++j)
    {
      memset(&message, 0, sizeof(message));
      for (i = 0, cnt = 0; i + 10 < NBRCELL(message.message); i += 10, ++cnt)
	assert(add_message(&message, "Message %c\n", 'X'));
      assert(!add_message(&message, "Message %c\n", 'X'));
      for (i = 0; i < cnt; ++i)
	assert(strncmp("Message X\n", &message.message[i * 10], 10) == 0);
      assert(message_len(&message) == cnt * 10);
      assert(get_message(&message) == &message.message[0]);
      purge_message(&message);
      assert(message_len(&message) == 0);
    }
  return (EXIT_SUCCESS);
}

