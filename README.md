**Performance Evaluation of a Vertex Detector**

NB: Launch compileTANS.C on ROOT with the command .x compileTANS.C

The problem at hand was addressed through the implementation of four classes: Cilindro.C for the geometry, Part.C for particle transport, Punto.C for creating objects representing points in space, and Track.C for track reconstruction.
The code that uses these classes has been divided into four separate programs: SimTans.C, SmearTans.C, RecoTans.C, and AnalysisTans.C.

**2. Classes**

Each of the following classes includes a default constructor, a standard constructor, a copy constructor, a destructor, an assignment operator, and Set, Get, and print functions where necessary.
Below are the implemented classes, along with the description of their purpose, data members, and member functions.

**2.1 Cilindro**

This class is used to describe the detector geometry and allows the creation of a cylinder by specifying the following data members: height (fH), base radius (fR), and thickness of the lateral surface (fG).
These characteristics can be printed when needed using the print member function.

**2.2 Punto**

The position of a point on the surface of a cylinder with fixed radius is determined by its Z coordinate (data member fZ) and the azimuthal angle φ relative to the X-axis (data member fPhi).
The class also implements a smear member function, which performs smearing of the points — i.e., adding a small perturbation to the impact position to simulate measurement uncertainty. Specifically, the original fZ and fPhi values are altered by adding random numbers drawn from a Gaussian distribution.

**2.3 Part**

This class contains all information related to a given particle, such as initial position (fX, fY, fZ), direction (fTh, fPh), momentum (fP), charge (fC), index (fInd), and event index (fIndEv).
The Hit member function determines the impact point of the particle with the beam pipe or the detector layers depending on its trajectory.
Given as input the thickness W of the cylindrical elements and the Radiation Length (LR) (which depends on the material), it is possible — through the scat function — to simulate multiple scattering of the particle and thus determine its new direction after crossing the material.

**2.4 Track**

This class contains objects representing the track of a particle. The data members are: track index (fInd), event index (fIndEv), and the intersection points with the two silicon layers (fZ1 and fZ2).
The slope and intercept of the straight trajectory in the (z, r) plane are computed and returned by the GetPar member function.

**3. Programs**
**3.1 SimTans.C**

SimTans.C aims to simulate the physical phenomena.
Once the desired number of events is set, the simulation draws the multiplicity (number of particles produced in the interaction) of each event from a probability distribution, which can be uniform or follow a predefined trend, depending on the boolean multUniform.

Then, the X and Y coordinates of the interaction vertex are drawn from Gaussian distributions, while the Z coordinate is uniformly distributed if zUniform is true; otherwise, it also follows a Gaussian distribution.
Next, the initial direction of each particle is extracted, characterized by the azimuthal angle φ (uniformly distributed) and the polar angle θ, which is related to the pseudorapidity η, itself drawn from a predefined distribution.
The particle momentum is uniformly distributed between 0.6 and 0.8 GeV/c, and the charge is fixed to 1.

The program then simulates particle transport and calculates the impact points on the three material layers: the beryllium beam pipe, and silicon layers 1 and 2.
This is done through the Hit member function of the Part class.
By setting the boolean doScat to true, one can also enable multiple scattering effects through the scat member function of Part.
The doIf boolean allows for considering the limited acceptance of the detector.

Finally, the program saves to an output file (whose name is provided as input) the coordinates of the impact points on the active detector layers (layers 1 and 2).

**3.2 SmearTans.C**

SmearTans.C was developed to simulate the detector response during data acquisition — that is, to model the effect on the measured impact positions on the silicon layers.

At this stage, spurious points simulating detector background noise are also introduced.
The number of such points follows a uniform distribution, with a maximum value of 2 for the first layer and 2 × (radius ratio) for the second layer, to account for the different lateral surfaces of the two detectors.

The new, “smeared” data are then saved in a new file, which will later be used by the reconstruction program.

**3.3 RecoTans.C**

This program uses the data provided by SmearTans.C to reconstruct the particle tracks in each event and, from these, infer the interaction vertex of each collision.

Using a loop over impact points in the first layer, the program searches, for each point, a corresponding impact point on the second layer that differs by no more than 0.01 radians in φ (this value can be changed).
Once a matching pair is found, a Track object is created.

Each reconstructed track is then intersected with the beam line to obtain a set of vertex candidates.
To estimate the Z-coordinate of the interaction vertex, the candidates are filled into a histogram with 128 bins between −20 cm and +20 cm.
The bin with the highest number of entries (the mode) is identified, and the mean Z value of the candidates in that bin is taken as the reconstructed vertex position.
If more than one maximum is found, a rebinning with factor 2 is performed.
If the peak search fails, the event is discarded: an analysis of such cases shows that the vertex candidates are too widely separated.

The obtained results are saved in a ROOT file.

**3.4 AnalysisTans.C**

This program allows the comparison between Monte Carlo truth (output of SimTans.C) and reconstructed data (output of RecoTans.C), in order to evaluate the performance of the vertex detector.
