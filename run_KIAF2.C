#if !defined(__CINT__) || defined(__CLING__)
#include "AliAODInputHandler.h"
#include "AliAnalysisManager.h"
#include "AliMCEventHandler.h"
#include "AliAnalysisTaskSigma1385PM.h"
#endif

void run_KIAF2(const char* dataset = "test1.list",
              const char* option = "AOD_Mix") {
  // gSystem->Load("libTree.so");
  // gSystem->Load("libGeom.so");
  // gSystem->Load("libVMC.so");
  // gSystem->Load("libPhysics.so");
  // gSystem->Load("libSTEERBase. so");
  // gSystem->Load("libESD.so");
  // gSystem->Load("libANALYSIS.so");
  // gSystem->Load("libOADB.so");
  // gSystem->Load("libANALYSISalice.so");
  // gSystem->Load("libpythia6_4_21.so");

  // gSystem->Setenv("alien_CLOSE_SE", "working_disk_SE");
  
  bool ismc = kFALSE;
  bool setmixing = kFALSE;
  bool isaod = kFALSE;
  int nmix = 10;
  TString foption = option;
  const char* taskname = "Sigma1385MB";
  const char* suffix = "_";
  if (foption.Contains("MC"))
    ismc = kTRUE;
  if (foption.Contains("AOD"))
    isaod = true;

  // analysis manager
  AliAnalysisManager* mgr =
      new AliAnalysisManager(Form("%s%s", taskname, option));
  AliInputEventHandler* handler;
  if (isaod)
    handler = new AliAODInputHandler();
  else
    handler = new AliESDInputHandler();

  // handler->SetNeedField(1);
  mgr->SetInputEventHandler(handler);

  if (ismc) {
    AliMCEventHandler* mcHandler = new AliMCEventHandler();
    mgr->SetMCtruthEventHandler(mcHandler);
  }
  gInterpreter->ExecuteMacro("$ALICE_PHYSICS/PWG/DevNanoAOD/macros/AddTaskNanoAODnormalisation.C");
  AliAnalysisTaskSigma1385PM* myTask =
    reinterpret_cast<AliAnalysisTaskSigma1385PM*>(
        gInterpreter->ExecuteMacro(
            Form("$ALICE_PHYSICS/PWGLF/RESONANCES/PostProcessing/Sigma1385/AddTaskSigma1385.C(\"%s\",\"%s\",%i,\"%s\")", taskname,
                  option, nmix, suffix)));
  if (!mgr->InitAnalysis())
    return;
  mgr->PrintStatus();

  // start analysis
  Printf("Starting Analysis....");

  // TChain* chain = CreateESDChain(dataset,-1);
  TChain* chain = new TChain("ESDTree");
  std::stringstream esdChain;
  if (isaod) {
    esdChain << ".x " << gSystem->Getenv("ALICE_PHYSICS")
             << "/PWG/EMCAL/macros/CreateAODChain.C(";
    esdChain << "\"" << dataset << "\", -1);";
  } else {
    esdChain << ".x " << gSystem->Getenv("ALICE_PHYSICS")
             << "/PWG/EMCAL/macros/CreateESDChain.C(";
    esdChain << "\"" << dataset << "\", -1);";
  }
  chain = reinterpret_cast<TChain*>(gROOT->ProcessLine(esdChain.str().c_str()));
  mgr->StartAnalysis("local", chain);
}
