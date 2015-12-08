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

  //Compile the contents of the sub-module
  gSystem->CompileMacro("src/submodules/datacollectorreaderlibs/TrackInfo/TrackInfo.cxx","k");
  gSystem->CompileMacro("src/submodules/datacollectorreaderlibs/PrimaryVertexInfo/PrimaryVertexInfo.cxx","k");
  gSystem->CompileMacro("src/submodules/datacollectorreaderlibs/EventInfo/EventInfo.cxx","k");
  gSystem->CompileMacro("src/submodules/ParticleInfo/ParticleInfo/ParticleInfo.cxx","k");

  //Set the Include Path to your header files
  gSystem->SetIncludePath(TString::Format("-I$ROOTSYS/include -I%s/inc",gSystem->pwd()));

  //Compile your source code
  gSystem->CompileMacro("src/analysis/readerExample.cxx","k");

}
