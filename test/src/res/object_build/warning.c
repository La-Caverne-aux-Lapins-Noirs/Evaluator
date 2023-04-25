// Fichier a compiler en .o

void func(void)
{
  void *ptr = 42; // Pour générer un avertissement.

  (void)ptr;
}

