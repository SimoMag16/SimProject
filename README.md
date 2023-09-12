Valutazione Performance di un Rivelatore di Vertice

La problematica in oggetto è stata affrontata mediante l’implementazione di quattro classi: \textbf{Cilindro.C} per la geometria, \textbf{Part.C} per il trasporto delle particelle, \textbf{Punto.C} per creare oggetti che rappresentassero punti nello spazio e \textbf{Track.C} per la ricostruzione della traccia. Il codice che fruisce delle suddette classi è stato suddiviso in 4 programmi distinti: \textbf{SimTans.C}, \textbf{SmearTans.C}, \textbf{RecoTans.C} e \textbf{AnalysisTans.C}. 

\section{Classi}
L'implementazione delle seguenti classi comprende un \textit{default constructor}, uno \textit{standard constructor}, un \textit{copy constructor}, un \textit{destuctor}, un \textit{assignment operator} e le funzioni \textbf{Set}, \textbf{Get} e \textbf{print} ove necessario. Sono elencate in seguito le classi utilizzate e si descrive lo scopo di ciascuna di esse attraverso l'elenco dei suoi \textit{data members} e delle sue \textit{member functions}.

\subsection{Cilindro}
Questa classe viene usata per descrivere la geometria del rivelatore e permette di creare un cilindro, specificandone i \textit{data members}: altezza (\textbf{fH}), raggio di base (\textbf{fR}) e spessore della superficie laterale (\textbf{fG}); queste caratteristiche possono essere stampate all'occorrenza mediante la \textit{member function} \textbf{print}.

\subsection{Punto}
La posizione di un punto sulla superficie di un cilindro di raggio fissato è determinata dalla sua coordinata Z (\textit{data member} \textbf{fZ}) e dall'angolo azimutale $\phi$ rispetto all'asse delle X (\textit{data member} \textbf{fPhi}). 
La classe implementa inoltre una \textit{member function} (\textbf{smear}) che permette lo \textit{smearing} dei punti, ossia l'aggiunta di una piccola perturbazione della posizione d'impatto che simuli l'errore strumentale: in particolare, le \textbf{fZ} e \textbf{fPhi} originali vengono alterate sommando un numero estratto da una distribuzione Gaussiana.

\subsection{Part}
Questa classe contiene tutte le informazioni relative a una certa particella quali la posizione iniziale (\textbf{fX}, \textbf{fY}, \textbf{fZ}), la direzione intrapresa (\textbf{fTh}, \textbf{fPh}), il momento \textbf{fP}, la carica \textbf{fC}, l'indice \textbf{fInd} e l'indice dell'evento \textbf{fIndEv}.
La \textit{member function} \textbf{Hit} permette di determinare il punto d'impatto della particella con la Beam Pipe o i layers di rivelazione a seconda della sua traiettoria.
Dando in input lo spessore \textbf{W} degli elementi cilindrici e la Radiation Lenght (\textbf{LR}, dipendente dal materiale) è possibile, tramite la \textit{member function} \textbf{scat}, simulare il \textit{multiple scattering} della particella e quindi determinarne la nuova direzione in seguito all'attraversamento dello spessore di materiale.

\subsection{Track}
La classe contiene oggetti che rappresentano la traccia di una particella: i \textit{data mebers} sono l'indice della traccia (\textbf{fInd}), l'indice dell'evento (\textbf{fIndEv}) e i punti di intersezione con due i layers di Silicio (\textbf{fZ1} e \textbf{fZ2}).
Il coefficiente angolare e l'intercetta della traiettoria rettilinea nel piando ($z,r$) sono calcolati e restituiti in output dalla \textit{member function} \textbf{GetPar}.

\newpage
\section{Programmi}

\subsection{SimTans.C}
\textbf{SimTans.C} si propone di simulare gli aspetti del fenomeno fisico.\\
Una volta impostato il numero di eventi desiderato la simulazione estrae la molteplicità (numero di particelle prodotte dall'interazione) di ciascun evento da una distribuzione di probabilità che può essere uniforme o seguire l'andamento assegnato, in base al valore del booleano \textbf{multUniform}. Vengono poi estratte con distribuzioni Gaussiane le coordinate X e Y del vertice d’interazione , mentre la coodinata Z è distribuita uniformemente se è vero il booleano \textbf{zUniform}, altrimenti anch'essa ha una distribuzione Gaussiana. Si estrae infine la direzione iniziale di ciascuna particella, caratterizzata dall'angolo azimutale $\phi$ (ditribuito uniformemente) e dall'angolo polare $\theta$, legato alla pseudorapidità $\eta$, a sua volta estratta da una distribuzione assegnata. L'impulso della particella è estratto da una distribuzione uniforme tra 0.6 GeV/c e 0.8 GeV/c, mentre la carica è fissata a 1.\\
A questo punto viene simulato il trasporto e calcolato il punto d’impatto delle particelle nei tre diversi strati di materiale: beam pipe al berillio, layer 1 e 2 al silicio. Ciò è stato possibile realizzando un’apposita \textbf{member function} della classe \textbf{Part} denominata \textbf{Hit}; inoltre impostando su true il booleano \textbf{doScat} è possibile implementare, mediante una seconda \textit{member function} di \textbf{Part} (\textbf{scat}), l’effetto di deviazione dela particella dovuto al passaggio nella materia. Il booleano di \textbf{doIf} consente invece di considerare l'accettanza limitata del rivelatore.
Infine il programma salva in un file il cui nome è dato in input le coordinate dei punti d’impatto negli strati attivi del rivelatore (layer 1 e 2).

\subsection{SmearTans.C}
\textbf{SmearTans.C} è stato sviluppato per simulare l’effetto del rivelatore nell’acquisizione dei dati sperimentali,  ossia sulle posizioni d'impatto delle particelle sui layers di silicio. \\
In questa fase sono stati anche inseriti i punti spuri che simulano il rumore di fondo del rivelatore: il numero di punti spuri ha una distribuzione uniforme, con valore massimo fissato a 2 per il primo layer e a 2 volte il rapporto dei raggi per il secondo layer, in modo da tenere conto della diversa superficie laterale dei due rivelatori.\\
I nuovi dati ottenuti vengono poi salvati in un nuovo file, che verrà poi passato al programma di ricostruzione.

\subsection{RecoTans.C}
Questo programma usa i dati forniti da \textbf{SmearTans.C} per ricostruire le tracce delle particelle in ciascun evento e da queste risalire al vertice di generazione di ciascuna collisione.\\
Attraverso un loop sui punti d'impatto nel primo layer si ricerca, per ciascuno di essi, un punto d'impatto sul secondo layer che non si discosti in $\phi$ per più di 0.01 radianti (che è comunque possibile variare), utilizzando un loop innestato sulle \textit{hits} nel secondo layer; una volta trovata una coppia di punti che soddisfa il taglio, viene creata una traccia utilizzando la classe \textbf{Track}.\\
A questo punto, si interseca ciascuna traccia con la \textit{Beam Line} per ottenere una serie di vertici ``candidati".
Per stiamare la coordinata Z del vertice di interazione si riportano i candidati su un istogramma di 128 bin tra -20 cm e 20 cm; si cerca quindi il bin con il massimo numero di conteggi (moda) e si fa la media dei candidati che si distibuiscono in questo bin. Qualora fosse presente più di un massimo è stato svolto un rebinning con parametro 2. Se la ricerca del massimo fallisce, l'evento viene scartato: dall'analisi di questi eventi risulta infatti che i punti candidati come vertice sono troppo separati tra loro.\\
I risultati ottenuti sono salvati su un file root.

\subsection{AnalysisTans.C}

Questo programma permette di confrontare la verità Monte Carlo (risultato si \textbf{SimTans.C}) con la ricostruzione sperimentale (risultato di \textbf{RecoTans.C}), in modo da valutare le prestazioni del rivelatore di vertice in questione.

\subsubsection{Residui}
Di seguito vengono riporatati gli istogrammi dei residui complessivi, ossia gli scarti tra la Z simulata e la Z ricostruita.
\begin{figure}[h]
\centering
\includegraphics[width=0.8\textwidth]{Resid.pdf} 
\caption{Residui complessivi: Z$_{\mbox{rec}}$-Z$_{\mbox{sim}}$}
\end{figure}
\newpage
Si può notare che queste distibuzioni non seguono un profilo Gaussiano.
Se si interpola la distribuzione dei residui a molteplicità fissata con una curva a campana si può testare l'ipotesi di andamento gaussiano utilizzando un test di $\chi^2$:
\begin{figure}[H]
\centering
\begin{subfigure}{0.4\textwidth}
\includegraphics[width=\linewidth ]{Res4.pdf} 
\caption{Residui a bassa molteplicità (4)}
\end{subfigure}
\hspace{1.5cm}
\begin{subfigure}{0.4\textwidth}
\includegraphics[width=\linewidth ]{Res40.pdf} 
\caption{Residui ad alta mlteplicità (40)}
\end{subfigure}
\end{figure}
\subsubsection{Efficienza}
Per quanto riguarda l'efficienza del rivelatore, essa può essere valutata in funzione della molteplicità.
\begin{figure}[H]
\centering
\begin{subfigure}{0.4\textwidth}
\includegraphics[width=\linewidth ]{effMU.pdf} 
\caption{Efficienza in funzione della molteplicità, con distribuzione di molteplicità uniforme}
\end{subfigure}
\hspace{1.5cm}
\begin{subfigure}{0.4\textwidth}
\includegraphics[width=\linewidth ]{effM.pdf} 
\caption{Efficienza in funzione della molteplicità, con distribuzione di molteplicità assegnata}
\end{subfigure}
\end{figure}
Dal confronto dei grafici in Figura 4 non si evince una differenza significativa nei valori dell'efficienza.\\
Nei grafici seguenti viene invece riportata l'efficienza in funzione della Z del vertice simulato.
\begin{figure}[H]
\centering
\begin{subfigure}{0.4\textwidth}
\includegraphics[width=\linewidth ]{effZU.pdf} 
\caption{Efficienza vs Z vertice, se la Z è estratta uniformemente}
\end{subfigure}
\hspace{1.5cm}
\begin{subfigure}{0.4\textwidth}
\includegraphics[width=\linewidth ]{effZ.pdf} 
\caption{Efficienza vs Z vertice, se la Z è estratta gaussianamente}
\end{subfigure}
\end{figure}
Si osserva che, utilizzando una distribuzione uniforme di Z vertice, si ottiene un'efficienza migliore, ad eccezione dei punti agli estremi destro e sinistro.\\
Un risultato ancora migliore per l'efficienza in funzione di Z si trova utilizzando una distribuzione di molteplicità uniforme.
\begin{figure}[H]
\centering
\includegraphics[width=0.6\linewidth]{effZ_mU.pdf} 
\caption{Efficienza in funzione di Z se la molteplicità ha distribuzione uniforme e Z gaussiana}
\end{figure}

\subsubsection{Risoluzione}
L'andamento della risoluzione del rivelatore è riportato per due distribuzioni di molteplicità utilizzate:
\begin{figure}[H]
\centering
\begin{subfigure}{0.4\textwidth}
\includegraphics[width=\linewidth ]{RisMU.pdf} 
\caption{Risoluzione in funzione della molteplicità, con distribuzione di molteplicità uniforme}
\end{subfigure}
\hspace{1.5cm}
\begin{subfigure}{0.4\textwidth}
\includegraphics[width=\linewidth ]{RisM.pdf} 
\caption{Risoluzione in funzione della molteplicità, con distribuzione di molteplicità assegnata}
\end{subfigure}
\end{figure}
I due casi portano allo stesso andamento della risoluzione, come si può osservare nel grafico di sovrapposizione
\begin{figure}[H]
\centering
\includegraphics[width=0.6\linewidth]{cfrM.pdf} 
\caption{Confronto Risoluzione vs Molteplicità a distribuzioni di molteplicità diverse (rosso: uniforme, nero:assegnata)}
\end{figure}
Infine è stata valutata la risoluzione del rivelatore a seconda della distribuzione da cui è stata estratta la Z del vertice:
\begin{figure}[H]
\centering
\begin{subfigure}{0.4\textwidth}
\includegraphics[width=\linewidth]{RisZU.pdf} 
\caption{Risoluzione vs Z vertice se la Z è estratta uniformemente}
\end{subfigure}
\hspace{1.5cm}
\begin{subfigure}{0.42\textwidth}
\includegraphics[width=\linewidth]{RisZ.pdf} 
\caption{Risoluzione vs se la Z è estratta gaussianamente}
\end{subfigure}
\end{figure}
Si riporta quindi la sovrapposizione dei due grafici
\begin{figure}[H]
\centering
\includegraphics[width=0.6\linewidth]{cfrZ.pdf} 
\caption{Confronto Risoluzione vs Z a distribuzioni di Z diverse (arancione: uniforme, nero: gaussiana)}
\end{figure}
Complessivamente si osserva che utilizzando una distribuzione uniforme di molteplicità, la risoluzione in funzione di Z è migliore (vedi figura seguente)
\begin{figure}[H]
\centering
\includegraphics[width=0.6\linewidth]{RisZ_MU.pdf} 
\caption{Risoluzione vs Z se la molteplicità ha distribuzione uniforme e Z gaussiana}
\end{figure}

\end{document}
