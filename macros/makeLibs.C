//This gets called by the makefile to compile all your source code. Add to it as needed!

void makeLibs(TString opt=""){

  //Remove the contents of the binary directory if option "clean" is called
  if (opt == "clean"){
    gSystem->Exec("rm -f bin/*");
    return;
  }

  //Check to make sure the bin directory exists. If not create it.
  if (gSystem->OpenDirectory("bin") == 0)
    gSystem->MakeDirectory("bin");

  //Set the Build and Include Directories
  gSystem->SetBuildDir("$PWD/bin/",true);
  gSystem->SetIncludePath(TString::Format("-I$ROOTSYS/include -I%s/inc",gSystem->pwd()));

  //Compile the contents of the sub-module
  gSystem->CompileMacro("src/submodules/datacollectorreaderlibs/TrackInfo/TrackInfo.cxx","k");
  gSystem->CompileMacro("src/submodules/datacollectorreaderlibs/PrimaryVertexInfo/PrimaryVertexInfo.cxx","k");
  gSystem->CompileMacro("src/submodules/datacollectorreaderlibs/EventInfo/EventInfo.cxx","k");
  gSystem->CompileMacro("src/submodules/ParticleInfo/ParticleInfo/ParticleInfo.cxx","k");

  //Glauber Source
  gSystem->CompileMacro("src/submodules/glaubermcmodel/src/GlauberClass.cxx","k");
  gSystem->CompileMacro("src/submodules/glaubermcmodel/src/ReturnStructs.cxx","k");
  gSystem->CompileMacro("src/submodules/glaubermcmodel/src/GlauberModel.cxx","k");
  gSystem->CompileMacro("src/submodules/glaubermcmodel/src/FindBestFitNegativeBinomialParameters.cxx","k");
  gSystem->CompileMacro("src/submodules/glaubermcmodel/src/FindCentralityBinCuts.cxx","k");
  gSystem->CompileMacro("src/submodules/glaubermcmodel/src/DetermineCentralityBin.cxx","k");
  gSystem->CompileMacro("src/submodules/glaubermcmodel/src/FindNpartNcollDistributions.cxx","k");
  gSystem->CompileMacro("src/submodules/glaubermcmodel/src/PrintResults.cxx","k");


  //Compile your source code
  gSystem->CompileMacro("userfiles/UserCuts.cxx","k");
  gSystem->CompileMacro("src/analysis/readerExample.cxx","k");
  gSystem->CompileMacro("src/analysis/centralityVariableDistributions.cxx","k");
  gSystem->CompileMacro("src/analysis/skimmerAndBinner.cxx","k");

}
