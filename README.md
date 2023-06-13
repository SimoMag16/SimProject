# SimProject

La problematica in oggetto è stata affrontata mediante l’implementazione di alcune classi: Cilindro per la geometria, Part per il trasporto delle particelle, Punto per creare oggetti che rappresentassero punti nello spazio e Track per la ricostruzione della traccia. Il codice che fruisce delle suddette classi si è suddiviso in 3 programmi: SimTansTree.C, SmearTans.C e RecoTans.C.

SimTansTree.C si propone di simulare gli aspetti del fenomeno fisico e una volta impostato il numero di eventi desiderato estrae da una specifica distribuzione di probabilità la molteplicità nonché la posizione del vertice d’interazione e la direzione iniziale di ciascuna particella. A questo punto viene simulato il trasporto e calcolato il punto d’impatto delle particelle nei tre diversi strati di materiale: beam pipe, layer 1 e 2 al silicio. Ciò è stato possibile realizzando un’apposita member function della classe Part denominata Hit, inoltre impostando su True il booleano doScat è possibile implementare mediante una seconda member function che permette di introdurre l’effetto di deviazione di traiettoria delle particelle dovuto al loro passaggio in spessori diversi di diversi materiali. Infine il programma salva in un file chiamato htree.root le coordinate d’impatto negli starti attivi del rivelatore (layer 1 e 2).

SmearTans.C è stato sviluppato per simulare l’effetto del rivelatore nell’acquisizione dei dati sperimentali ossia delle posizioni di impatto delle particelle sui layers di silicio. I nuovi punti ottenuti vengono poi
