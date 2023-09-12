Valutazione della Performance di un Rivelatore di Vertice
1 Abstract
La problematica in oggetto `e stata affrontata mediante l’implementazione di quattro classi: Cilin-
dro.C per la geometria, Part.C per il trasporto delle particelle, Punto.C per creare oggetti
che rappresentassero punti nello spazio e Track.C per la ricostruzione della traccia. Il codice che
fruisce delle suddette classi `e stato suddiviso in 4 programmi distinti: SimTans.C, SmearTans.C,
RecoTans.C e AnalysisTans.C.
2 Classi
L’implementazione delle seguenti classi comprende un default constructor, uno standard construc-
tor, un copy constructor, un destuctor, un assignment operator e le funzioni Set, Get e print ove
necessario. Sono elencate in seguito le classi utilizzate e si descrive lo scopo di ciascuna di esse
attraverso l’elenco dei suoi data members e delle sue member functions.
2.1 Cilindro
Questa classe viene usata per descrivere la geometria del rivelatore e permette di creare un cilindro,
specificandone i data members: altezza (fH), raggio di base (fR) e spessore della superficie laterale
(fG); queste caratteristiche possono essere stampate all’occorrenza mediante la member function
print.
2.2 Punto
La posizione di un punto sulla superficie di un cilindro di raggio fissato `e determinata dalla sua
coordinata Z (data member fZ) e dall’angolo azimutale φ rispetto all’asse delle X (data member
fPhi). La classe implementa inoltre una member function (smear) che permette lo smearing dei
punti, ossia l’aggiunta di una piccola perturbazione della posizione d’impatto che simuli l’errore
strumentale: in particolare, le fZ e fPhi originali vengono alterate sommando un numero estratto
da una distribuzione Gaussiana.
2.3 Part
Questa classe contiene tutte le informazioni relative a una certa particella quali la posizione iniziale
(fX, fY, fZ), la direzione intrapresa (fTh, fPh), il momento fP, la carica fC, l’indice fInd e l’indice
dell’evento fIndEv. La member function Hit permette di determinare il punto d’impatto della
particella con la Beam Pipe o i layers di rivelazione a seconda della sua traiettoria. Dando in input
lo spessore W degli elementi cilindrici e la Radiation Lenght (LR, dipendente dal materiale) `e
possibile, tramite la member function scat, simulare il multiple scattering della particella e quindi
determinarne la nuova direzione in seguito all’attraversamento dello spessore di materiale.
2.4 Track
La classe contiene oggetti che rappresentano la traccia di una particella: i data mebers sono l’indice
della traccia (fInd), l’indice dell’evento (fIndEv) e i punti di intersezione con due i layers di Silicio
(fZ1 e fZ2). Il coefficiente angolare e l’intercetta della traiettoria rettilinea nel piando (z, r) sono
calcolati e restituiti in output dalla member function GetPar.
1
3 Programmi
3.1 SimTans.C
SimTans.C si propone di simulare gli aspetti del fenomeno fisico.
Una volta impostato il numero di eventi desiderato la simulazione estrae la molteplicit`a (numero di
particelle prodotte dall’interazione) di ciascun evento da una distribuzione di probabilit`a che pu`o
essere uniforme o seguire l’andamento assegnato, in base al valore del booleano multUniform.
Vengono poi estratte con distribuzioni Gaussiane le coordinate X e Y del vertice d’interazione
, mentre la coodinata Z `e distribuita uniformemente se `e vero il booleano zUniform, altrimenti
anch’essa ha una distribuzione Gaussiana. Si estrae infine la direzione iniziale di ciascuna particella,
caratterizzata dall’angolo azimutale φ (ditribuito uniformemente) e dall’angolo polare θ, legato alla
pseudorapidit`a η, a sua volta estratta da una distribuzione assegnata. L’impulso della particella `e
estratto da una distribuzione uniforme tra 0.6 GeV/c e 0.8 GeV/c, mentre la carica `e fissata a 1.
A questo punto viene simulato il trasporto e calcolato il punto d’impatto delle particelle nei tre
diversi strati di materiale: beam pipe al berillio, layer 1 e 2 al silicio. Ci`o `e stato possibile
realizzando un’apposita member function della classe Part denominata Hit; inoltre impostando
su true il booleano doScat `e possibile implementare, mediante una seconda member function di
Part (scat), l’effetto di deviazione dela particella dovuto al passaggio nella materia. Il booleano
di doIf consente invece di considerare l’accettanza limitata del rivelatore. Infine il programma
salva in un file il cui nome `e dato in input le coordinate dei punti d’impatto negli strati attivi del
rivelatore (layer 1 e 2).
3.2 SmearTans.C
SmearTans.C `e stato sviluppato per simulare l’effetto del rivelatore nell’acquisizione dei dati
sperimentali, ossia sulle posizioni d’impatto delle particelle sui layers di silicio.
In questa fase sono stati anche inseriti i punti spuri che simulano il rumore di fondo del rivelatore:
il numero di punti spuri ha una distribuzione uniforme, con valore massimo fissato a 2 per il primo
layer e a 2 volte il rapporto dei raggi per il secondo layer, in modo da tenere conto della diversa
superficie laterale dei due rivelatori.
I nuovi dati ottenuti vengono poi salvati in un nuovo file, che verr`a poi passato al programma di
ricostruzione.
3.3 RecoTans.C
Questo programma usa i dati forniti da SmearTans.C per ricostruire le tracce delle particelle in
ciascun evento e da queste risalire al vertice di generazione di ciascuna collisione.
Attraverso un loop sui punti d’impatto nel primo layer si ricerca, per ciascuno di essi, un punto
d’impatto sul secondo layer che non si discosti in φ per pi`u di 0.01 radianti (che `e comunque
possibile variare), utilizzando un loop innestato sulle hits nel secondo layer; una volta trovata una
coppia di punti che soddisfa il taglio, viene creata una traccia utilizzando la classe Track.
A questo punto, si interseca ciascuna traccia con la Beam Line per ottenere una serie di vertici
“candidati”. Per stiamare la coordinata Z del vertice di interazione si riportano i candidati su un
istogramma di 128 bin tra -20 cm e 20 cm; si cerca quindi il bin con il massimo numero di conteggi
(moda) e si fa la media dei candidati che si distibuiscono in questo bin. Qualora fosse presente
pi`u di un massimo `e stato svolto un rebinning con parametro 2. Se la ricerca del massimo fallisce,
l’evento viene scartato: dall’analisi di questi eventi risulta infatti che i punti candidati come vertice
sono troppo separati tra loro.
I risultati ottenuti sono salvati su un file root.
3.4 AnalysisTans.C
Questo programma permette di confrontare la verit`a Monte Carlo (risultato si SimTans.C) con
la ricostruzione sperimentale (risultato di RecoTans.C), in modo da valutare le prestazioni del
rivelatore di vertice in questione.
