/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2023
**
** TechnoCore
*/

#include		"technocore.h"

typedef struct		s_mcq
{
  int			correct;
  int			incomplete;
  int			wrong;
  int			mandatory;
}			t_mcq;

t_technocore_result	evaluate_mcq(const char				*argv,
				     t_bunny_configuration		*gen,
				     t_bunny_configuration		*exe,
				     t_technocore_activity		*act)
{
  const char		*cfile;
  t_bunny_configuration	*answers;
  t_bunny_configuration	*ques;
  t_mcq			global;
  t_mcq			local;
  int			score;
  int			maxscore;
  int			minscore;
  bool			aem;
  int			qcnt;
  int			i;

  (void)argv;
  (void)gen;
  if (!bunny_configuration_getf(exe, NULL, "Questions[0]"))
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Missing questions for MCQ %s.\n",
		  bunny_configuration_get_address(exe)
		  );
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (!bunny_configuration_getf(exe, &cfile, "AnswersFile"))
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer,
		  "Missing answers file for MCQ %s.\n",
		  bunny_configuration_get_address(exe)
		  );
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP
  if (!(answers = bunny_open_configuration(cfile, NULL)))
    { // LCOV_EXCL_START
      if (access(cfile, F_OK))
	add_exercise_message(act, dict_get_pattern("InvalidAnswerFile"));
      else
	add_exercise_message(act, dict_get_pattern("NoAnswersFound"));
      return (TC_FAILURE);
    } // LCOV_EXCL_STOP

  // Default values
  global.correct = 1;
  global.incomplete = 0;
  global.wrong = -1;
  global.mandatory = false;

  // Optional configuration
  bunny_configuration_getf(gen, &global.correct, "IfFullCorrect");
  bunny_configuration_getf(gen, &global.incomplete, "IfIncomplete");
  bunny_configuration_getf(gen, &global.wrong, "IfWrong");
  bunny_configuration_getf(gen, &global.mandatory, "AnswersAreMandatory");

  score = 0;
  maxscore = 0;
  qcnt = 0;
  for (i = 0; bunny_configuration_getf(exe, &ques, "Questions[%d]", i); ++i)
    {
      const char	*text;
      const char	*lansw;
      t_bunny_configuration *bansw;
      int		goodansw[32];
      int		len;
            
      // Récupération des bonnes réponses dans la configuration
      // Le format doit être compatible avec le CSV d'histoire de l'informatique
      if (bunny_configuration_getf(ques, &lansw, "[0][1]"))
	{
	  const char *toks[2] = {",", NULL};
	  const char * const *spl = bunny_split(lansw, toks, false);

	  bunny_configuration_getf(ques, &text, "[0][0]");
	  len = bunny_split_len(spl);
	  if (len >= NBRCELL(goodansw))
	    { // LCOV_EXCL_START
	      add_message(&gl_technocore.error_buffer,
			  "Too many potential answers for questions %s.",
			  bunny_configuration_get_address(ques));
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  for (int k = 0; k < len; ++k)
	    goodansw[k] = atoi(spl[k]);
	  goodansw[len] = -1;
	  bunny_delete_split(spl);
	}
      else if (bunny_configuration_getf(ques, &bansw, "Answers"))
	{
	  if (!bunny_configuration_getf(ques, &text, "Question"))
	    continue ;
	  if ((len = bunny_configuration_casesf(bansw, ".")) >= NBRCELL(goodansw))
	    { // LCOV_EXCL_START
	      add_message(&gl_technocore.error_buffer,
			  "Too many potential answers for questions %s.",
			  bunny_configuration_get_address(ques));
	      return (TC_CRITICAL);
	    } // LCOV_EXCL_STOP
	  if (len == 0)
	    {
	      bunny_configuration_getf(bansw, &goodansw[0], ".");
	      len += 1;
	    }
	  else
	    for (int k = 0; bunny_configuration_getf(bansw, &goodansw[k], "[%d]", k); ++k);
	  goodansw[len] = -1;
	}
      else
	continue ;

      // Local configuration
      local = global;
      bunny_configuration_getf(ques, &local.correct, "IfFullCorrect");
      bunny_configuration_getf(ques, &local.incomplete, "IfIncomplete");
      bunny_configuration_getf(ques, &local.wrong, "IfWrong");
      bunny_configuration_getf(ques, &local.mandatory, "Mandatory");
      maxscore += local.correct;
      
      int	cansw;
      int	mistake;
      int	j;

      mistake = 0;
      for (int k = 0; bunny_configuration_getf(answers, &cansw, "[%d][%d]", qcnt, k); ++k)
	{
	  for (j = 0; j < len; ++j)
	    if (cansw == goodansw[j])
	      break ;
	  if (j != len) // On a trouvé une bonne réponse
	    {
	      bunny_memswap(&goodansw[j], &goodansw[len - 1], sizeof(goodansw[0]));
	      goodansw[len -= 1] = -1;
	    }
	  else // La réponse est mauvaise
	    mistake += 1;
	}
      qcnt += 1;
      aem = true;
      // Raté
      if (mistake != 0)
	{
	  aem = add_exercise_message(act, dict_get_pattern("MCQBadAnswer"), text);
	  score -= abs(local.wrong);
	  if (aem && local.mandatory)
	    break ;
	}
      // Il en manque
      else if (len != 0)
	{
	  aem = add_exercise_message(act, dict_get_pattern("MCQIncompleteAnswer"), text);
	  score += local.incomplete;
	  if (aem && local.mandatory)
	    break ;
	}
      // On a tout trouvé sans faire de betise
      else
	score += local.correct;

      if (aem == false)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Cannot add report error message inside MCQ module.");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
    }

  // On a quitté avant d'avoir fini - une question obligatoire a été ratée
  if (bunny_configuration_getf(exe, NULL, "Questions[%d]", i))
    {
      if (add_exercise_message(act, dict_get_pattern("MCQWasMandatory")) == false)
	{ // LCOV_EXCL_START
	  add_message(&gl_technocore.error_buffer, "Cannot add report mandatory question message inside MCQ module.");
	  return (TC_CRITICAL);
	} // LCOV_EXCL_STOP
    }

  if (!bunny_configuration_getf(exe, &minscore, "MinimalScore"))
    minscore = maxscore / 2;

  aem = true;
  if (score < minscore)
    aem = add_exercise_message(act, dict_get_pattern("MCQFailure"), score, minscore, maxscore);
  else
    aem = add_exercise_message(act, dict_get_pattern("MCQSuccess"), score, minscore, maxscore);
  if (aem == false)
    { // LCOV_EXCL_START
      add_message(&gl_technocore.error_buffer, "Cannot add conclusion to MCQ module.");
      return (TC_CRITICAL);
    } // LCOV_EXCL_STOP

  return (score < minscore ? TC_FAILURE : TC_SUCCESS);
}

