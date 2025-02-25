void compileTANS() {
    // Lista delle classi da compilare
    std::vector<std::string> classFiles = {
        "Cilindro.C",  
        "Punto.C",  
        "Part.C",   
        "Track.C"
    };

    // Lista dei programmi da compilare
    std::vector<std::string> programFiles = {
        "SimTans.C", 
        "SmearTans.C",
        "RecoTans.C",
        "AnalysisTans.C"  
    };

    // Compilazione delle classi con il comando .L nomefile.C+
    for (const auto& classFile : classFiles) {
        std::cout << "Compilando la classe " << classFile << " con .L ..." << std::endl;
        gInterpreter->LoadMacro((classFile + "+").c_str());
    }

    // Compilazione dei programmi con il comando .L nomefile.C+
    for (const auto& programFile : programFiles) {
        std::cout << "Compilando il programma " << programFile << " con .L ..." << std::endl;
        gInterpreter->LoadMacro((programFile + "+").c_str());
    }

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    // SimTans ha i seguenti argomenti
    // SimTans(numero_eventi, outpufile, doScat, doIf, multUniform, zUniform, seed)
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    
    // Esecuzione dei programmi dopo la compilazione
    std::vector<std::string> functionsToRun = {
        "SimTans(1000000, \"hfileSim.root\", 1, 1, 0, 0, 347)", 
        "SmearTans(\"hfileSim.root\", \"hfileSmear.root\")",
        "RecoTans(\"hfileSim.root\", \"hfileSmear.root\", \"hfileReco.root\")",
        "AnalysisTans(\"hfileSim.root\", \"hfileReco.root\", \"hfileAnalysis.root\")"  
    };

    for (const auto& functionCall : functionsToRun) {
        std::cout << "Eseguendo " << functionCall << "..." << std::endl;
        gROOT->ProcessLine(functionCall.c_str());
    }
}