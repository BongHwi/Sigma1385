#if !defined(__CINT__) || defined(__CLING__)
#include "AliAODInputHandler.h"
#include "AliAnalysisAlien.h"
#include "AliPhysicsSelectionTask.h"
#include "AliMultSelectionTask.h"
#include "AliAnalysisManager.h"
#include "AliMCEventHandler.h"
#include "AliAnalysisTaskSigma1385PM.h"
#include "AliAnalysisTaskSigma1385temp.h"
#include "AliAnalysisTaskTrackMixertemp.h"
#include "AliESDInputHandler.h"
#endif

void run_KIAF(const char* dataset = "test1.list",
              const char* option = "AOD_Mix_Dev_Nano") {
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
  bool isNano = kFALSE;
  bool setmixing = kFALSE;
  bool isaod = kFALSE;
  int nmix = 10;
  bool highmult = kFALSE;
  TString foption = option;
  const char* taskname = "Sigma1385MB";
  const char* tasknameMixer = "TrackMixer";
  const char* suffix = "_";
  if (foption.Contains("MC"))
    ismc = kTRUE;
  if (foption.Contains("Dev"))
    isDev = kTRUE;
  if (foption.Contains("AOD"))
    isaod = true;
  if (foption.Contains("Nano"))
    isNano = true;

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
  if(!isNano){
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
  }
  if(isNano)
    gInterpreter->ExecuteMacro("$ALICE_PHYSICS/PWG/DevNanoAOD/macros/AddTaskNanoAODnormalisation.C");
  if (isDev) {
    gInterpreter->LoadMacro("AliAnalysisTaskTrackMixertemp.cxx+g");
    gInterpreter->LoadMacro("AliAnalysisTaskSigma1385temp.cxx+g");
    AliAnalysisTaskTrackMixertemp* myTaskMixer =
        reinterpret_cast<AliAnalysisTaskTrackMixertemp*>(gInterpreter->ExecuteMacro(
            Form("AddTaskTrackMixer.C(\"%s\",\"%s\",%i,\"%s\")", tasknameMixer, option,
                 nmix, suffix)));
    std::vector<TString> options = {"_","_cpv1","_cpv2","_DCAd1","_DCAd2",
                                    "_DCApvlambda1","_DCApvlambda2", "_lifetime1",
                                    "_lifetime2","_masscut1","_masscut2","_pid1",
                                    "_pid2","_pid3","_pid4","_pionr1","_pionr2","_pionz1",
                                    "_pionz2"};
    std::vector<AliAnalysisTaskSigma1385temp*> myTasks;
    for(auto val: options){
      myTasks.push_back(
        reinterpret_cast<AliAnalysisTaskSigma1385temp*>(
            gInterpreter->ExecuteMacro(
                Form("AddTaskSigma1385.C(\"%s\",\"%s\",%i,\"%s\")", taskname,
                     option, nmix, val.Data()))));
    }
    for(auto Checktask: myTasks){
      Checktask->fEventCuts.SetManualMode();
      Checktask->fEventCuts.SetupRun2pp();
      Checktask->fEventCuts.OverrideAutomaticTriggerSelection(AliVEvent::kINT7);
      Checktask->fEventCuts.SelectOnlyInelGt0(kFALSE);
      Checktask->SetFillQAPlot(kFALSE);
      Checktask->SetMixing(kTRUE);
	  Checktask->SetMinCPAV0(0.98);
	  Checktask->SetLowRadiusV0(0.2);
	  Checktask->SetHighRadiusV0(200);
    }
    myTasks[0]->SetFillQAPlot(kTRUE);
    myTasks[1]->SetMinCPAV0(0.99);
    myTasks[2]->SetMinCPAV0(0.97);
    myTasks[3]->SetMaxDCAV0daughters(0.35);
    myTasks[4]->SetMaxDCAV0daughters(0.67);
    myTasks[5]->SetMaxDCAPVV0(0.22);
    myTasks[6]->SetMaxDCAPVV0(0.4);
    myTasks[7]->SetLifetimeV0(28);
    myTasks[8]->SetLifetimeV0(32);
    myTasks[9]->SetMaxMassWindowV0(0.013);
    myTasks[10]->SetMaxMassWindowV0(0.0076);
    myTasks[11]->SetMaxNsigSigmaStarPion(2.5);
    myTasks[12]->SetMaxNsigSigmaStarPion(3.5);
    myTasks[13]->SetMaxNsigV0Proton(2.5);
    myTasks[13]->SetMaxNsigV0Pion(2.5);
    myTasks[14]->SetMaxNsigV0Proton(3.5);
    myTasks[14]->SetMaxNsigV0Pion(3.5);
    myTasks[15]->SetMaxVertexXYsigSigmaStarPion(6.0);
    myTasks[16]->SetMaxVertexXYsigSigmaStarPion(8.0);
    myTasks[17]->SetMaxVertexZSigmaStarPion(1.8);
    myTasks[18]->SetMaxVertexZSigmaStarPion(2.2);

  } else {
      // AliAnalysisTaskTrackMixer* myTaskMixer =
      //     reinterpret_cast<AliAnalysisTaskTrackMixer*>(gInterpreter->ExecuteMacro(
      //         Form("$ALICE_PHYSICS/PWGLF/RESONANCES/PostProcessing/Sigma1385/AddTaskTrackMixer.C(\"%s\",\"%s\",%i,\"%s\")", tasknameMixer, option,
      //             nmix, suffix)));
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
  if(isNano){
    esdChain << ".x " << "CreateNanoAODChain.C(";
    esdChain << "\"" << dataset << "\", -1);";
  }
  else if (isaod) {
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
