fileSystem module

Il modulo è formato da due parti:
  - DiskDriver
    Scrive blocchi sul disco rigido e ne gestisce il collegamento tra blocchi
    contigui.
  - fileSystem
    Gestisce file e cartelle di un fileSystem, salvando sul disco rigido il
    necessario e recuperando le informazioni dal disco

Il Filesystem gestisce la gerarchia di cartelle e file all'interno del sistema.
Permette di creare nuovi file, eliminarne, creare cartelle ecc ecc.

Nel diskdriver sono tenuti i blocchi di dati, puntati dalle struct del filesystem,
che compongono i file salvati e le cartelle create.


