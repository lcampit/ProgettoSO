Memory allocator module

Il modulo è formato da due parti:

    allocator Contiene le funzioni di gestione del buddy allocator e i suoi livelli.
    bit_map Contiene le funzioni di inizializzazione, setting e stampa della bitmap.

L'allocatore sfrutta la bitmap per la gestione dello spazio occupato in memoria, segnando a 1 i bit occupati e lasciando a 0 quelli liberi. La memoria è gestita tramite buddy.

Il file testalloc testa il funzionamento dell'allocatore settando e poi liberando i blocchi di memoria.
