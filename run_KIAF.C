#if !defined(__CINT__) || defined(__CLING__)
#include "AliAODInputHandler.h"
#include "AliAnalysisAlien.h"
#include "AliPhysicsSelectionTask.h"
#include "AliMultSelectionTask.h"
#include "AliAnalysisManager.h"
#include "AliMCEventHandler.h"
#include "AliAnalysisTaskSigma1385PM.h"
#include "AliAnalysisTaskSigma1385temp.h"
#include "AliESDInputHandler.h"
#endif

void run_KIAF(const char* dataset = "test1.list",
              const char* option = "AOD_SYS_MC_Mix") {
  gSystem->Load("libTree.so");
  gSystem->Load("libGeom.so");
  gSystem->Load("libVMC.so");
  gSystem->Load("libPhysics.so");
  gSystem->Load("libSTEERBase. so");
  gSystem->Load("libESD.so");
  gSystem->Load("libANALYSIS.so");
  gSystem->Load("libOADB.so");
  gSystem->Load("libANALYSISalice.so");
  gSystem->Load("libpythia6_4_21.so");

  gSystem->Setenv("alien_CLOSE_SE", "working_disk_SE");
  // ROOT 6 MODE
  // add aliroot indlude path
  gInterpreter->ProcessLine(
      Form(".include %s/include", gSystem->ExpandPathName("$ROOTSYS")));
  gInterpreter->ProcessLine(
      Form(".include %s/include", gSystem->ExpandPathName("$ALICE_ROOT")));
  gInterpreter->ProcessLine(
      Form(".include %s/include", gSystem->ExpandPathName("$ALICE_PHYSICS")));

  bool isaa = kFALSE;
  bool ismc = kFALSE;
  bool isDev = kFALSE;
  bool setmixing = kFALSE;
  bool isaod = kFALSE;
  int nmix = 10;
  bool highmult = kFALSE;
  TString foption = option;
  const char* taskname = "Sigma1385";
  const char* suffix = "MB_";
  if (foption.Contains("MC"))
    ismc = kTRUE;
  if (foption.Contains("Dev"))
    isDev = kTRUE;
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
  //
  // Physics Selection
  AliPhysicsSelectionTask* physSelTask =
      reinterpret_cast<AliPhysicsSelectionTask*>(gInterpreter->ExecuteMacro(
          Form("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C(%d)",
               ismc)));
  // Multiplicity selection
  AliMultSelectionTask* MultSlection =
      reinterpret_cast<AliMultSelectionTask*>(gInterpreter->ExecuteMacro(
          "$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/"
          "AddTaskMultSelection.C"));
  MultSlection->SetAddInfo(kTRUE);
  MultSlection->SetSelectedTriggerClass(AliVEvent::kAny);
  // MultSlection->SetAlternateOADBforEstimators("LHC16k"); //if needed
  // PID response
  AliAnalysisTask* fPIDResponse =
      reinterpret_cast<AliAnalysisTask*>(gInterpreter->ExecuteMacro(
          Form("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C(%d)", ismc)));
  if (isDev) {
    gInterpreter->LoadMacro("AliAnalysisTaskSigma1385temp.cxx+g");
    // AliAnalysisTaskXi1530temp *myTask =
    // reinterpret_cast<AliAnalysisTaskXi1530temp*>(gInterpreter->ExecuteMacro(Form("AddTaskXi1530.c(\"%s\",\"%s\",%i,%d,%d,%d,%d)",taskname,option,nmix,highmult,isaa,ismc,setmixing)));

    AliAnalysisTaskSigma1385temp* myTask =
        reinterpret_cast<AliAnalysisTaskSigma1385temp*>(
            gInterpreter->ExecuteMacro(
                Form("AddTaskSigma1385.C(\"%s\",\"%s\",%i,\"%s\")", taskname,
                     option, nmix, suffix)));

    // myTask->SetAssymCut(kTRUE);
    // myTask->SetSigmaStarAssymCutHigh(0.95);
    // myTask->SetSigmaStarAssymCutLow(0.3);
    // myTask->SetFillQAPlot(kTRUE);
    // if(ismc)
    //		myTask->SetFillnTuple(true);
    // myTask->SetLowRadiusV0(0.5);
    // myTask->SetHighRadiusV0(200);
    // myTask->SetMaxDCAV0daughters(0.5);
    // myTask->SetMaxDCAPVV0PosDaughter(0.0);
    // myTask->SetMaxDCAPVV0NegDaughter(0.0);
  } else {
      AliAnalysisTaskSigma1385PM* myTask =
        reinterpret_cast<AliAnalysisTaskSigma1385PM*>(
            gInterpreter->ExecuteMacro(
                Form("$ALICE_PHYSICS/PWGLF/RESONANCES/PostProcessing/Sigma1385/AddTaskSigma1385.C(\"%s\",\"%s\",%i,\"%s\")", taskname,
                     option, nmix, suffix)));
  }
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
