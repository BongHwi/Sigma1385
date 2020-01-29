#if !defined(__CINT__) || defined(__CLING__)
#include "AliAODInputHandler.h"
#include "AliAnalysisAlien.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskSigma1385temp.h"
#include "AliESDInputHandler.h"
#endif

vector<Int_t> LHC16k = {
    258537, 258499, 258477, 258456, 258454, 258452, 258426, 258393, 258391,
    258387, 258359, 258336, 258332, 258307, 258306, 258303, 258302, 258301,
    258299, 258278, 258274, 258273, 258271, 258270, 258258, 258257, 258256,
    258204, 258203, 258202, 258198, 258197, 258178, 258117, 258114, 258113,
    258109, 258108, 258107, 258063, 258062, 258060, 258059, 258053, 258049,
    258045, 258042, 258041, 258039, 258019, 258017, 258014, 258012, 258008,
    258003, 257992, 257989, 257986, 257979, 257963, 257960, 257957, 257939,
    257937, 257936, 257892, 257855, 257853, 257851, 257850, 257804, 257803,
    257800, 257799, 257798, 257797, 257773, 257765, 257757, 257754, 257737,
    257735, 257734, 257733, 257727, 257725, 257724, 257697, 257694, 257692,
    257691, 257689, 257688, 257687, 257685, 257684, 257682, 257644, 257642,
    257636, 257635, 257632, 257630, 257606, 257605, 257604, 257601, 257595,
    257594, 257592, 257590, 257588, 257587, 257566, 257562, 257561, 257560,
    257541, 257540, 257539, 257537, 257531, 257530, 257492, 257491, 257490,
    257488, 257487, 257474, 257468, 257457, 257433, 257364, 257358, 257330,
    257322, 257320, 257318, 257260, 257224, 257209, 257206, 257204, 257144,
    257141, 257139, 257138, 257137, 257136, 257100, 257095, 257092, 257086,
    257084, 257082, 257080, 257077, 257028, 257026, 257021, 257012, 257011,
    256944, 256942, 256941, 256697, 256695, 256694, 256692, 256691, 256684,
    256681, 256677, 256676, 256658, 256620, 256619, 256592, 256591, 256589,
    256567, 256565, 256564, 256562, 256560, 256557, 256556, 256554, 256552,
    256514, 256512, 256510, 256506, 256504};  // for run

class AliAnalysisGrid;
void run(const char* taskname = "Sigma1385",
         const char* option =
             "LHC16k_MB_Mix_AOD_range"  // when scanning AOD, add "AOD"
         ,
         const char* gridmode = "local"  // or "terminate" to merge
         ,
         UInt_t istart = 0,
         UInt_t iend = 1,
         const char* localorgrid = "local") {
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

    gInterpreter->ProcessLine(
        Form(".include %s/include", gSystem->ExpandPathName("$ROOTSYS")));
    gInterpreter->ProcessLine(
        Form(".include %s/include", gSystem->ExpandPathName("$ALICE_ROOT")));
    gInterpreter->ProcessLine(
        Form(".include %s/include", gSystem->ExpandPathName("$ALICE_PHYSICS")));

    bool isaa = kFALSE;
    bool ismc = kFALSE;
    bool isaod = kFALSE;
    bool setmixing = kFALSE;
    bool vertexer = false;
    int nmix = 10;
    bool highmult = kFALSE;
    TString foption = option;
    const char* suffix = "MB";
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

    // Physics Selection
    AliPhysicsSelectionTask* physSelTask =
        reinterpret_cast<AliPhysicsSelectionTask*>(gInterpreter->ExecuteMacro(
            Form("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C(%d,kTRUE)",
                 ismc)));
    if (!physSelTask) {
        Printf("no physSelTask");
        return;
    }
    // Multiplicity selection
    AliMultSelectionTask* MultSlection =
        reinterpret_cast<AliMultSelectionTask*>(gInterpreter->ExecuteMacro(
            "$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/"
            "AddTaskMultSelection.C"));
    if (!MultSlection) {
        Printf("no MultSlection");
        return;
    }
    MultSlection->SetAddInfo(kTRUE);
    MultSlection->SetSelectedTriggerClass(AliVEvent::kAny);
    // MultSlection->SetAlternateOADBforEstimators("LHC16k"); //if needed
    // PID response
    AliAnalysisTask* fPIDResponse =
        reinterpret_cast<AliAnalysisTaskSigma1385temp*>(
            gInterpreter->ExecuteMacro(Form(
                "$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C(%d)", ismc)));
    if (!fPIDResponse) {
        Printf("no fPIDResponse");
        return;
    }
    
    gInterpreter->LoadMacro("AliAnalysisTaskSigma1385temp.cxx+g");
    
    AliAnalysisTaskSigma1385temp* myTask =
        reinterpret_cast<AliAnalysisTaskSigma1385temp*>(gInterpreter->ExecuteMacro(
            Form("AddTaskSigma1385.C(\"%s\",\"%s\",%i,\"%s\")", taskname, option,
                 nmix, suffix)));

     //mgr->SetDebugLevel(3);
    if (!mgr->InitAnalysis())
        return;
    mgr->PrintStatus();

    // start analysis
    Printf("Starting Analysis....");

    //----LOCAL MODE-------------------------------------------------
    if (strcmp(localorgrid, "local") == 0) {
        TChain* chain = new TChain("ESDTree");
        std::stringstream esdChain;
        if (isaod) {
            esdChain << ".x " << gSystem->Getenv("ALICE_PHYSICS")
                     << "/PWG/EMCAL/macros/CreateAODChain.C(";
            if (!ismc)
                esdChain << "\""
                         << "data_aod.txt"
                         << "\", ";
            else
                esdChain << "\""
                         << "data_aod_MC.txt"
                         << "\", ";
            esdChain << 1 << ", ";
            esdChain << 0 << ", ";
            esdChain << std::boolalpha << kFALSE << ");";
        }
        else{
            esdChain << ".x " << gSystem->Getenv("ALICE_PHYSICS")
                     << "/PWG/EMCAL/macros/CreateESDChain.C(";
            if (!ismc)
                esdChain << "\""
                         << "data.txt"
                         << "\", ";
            else
                esdChain << "\""
                         << "data_MC.txt"
                         << "\", ";
            esdChain << 1 << ", ";
            esdChain << 0 << ", ";
            esdChain << std::boolalpha << kFALSE << ");";
        }
        chain = reinterpret_cast<TChain*>(
            gROOT->ProcessLine(esdChain.str().c_str()));

        chain->Lookup();
        mgr->StartAnalysis(localorgrid, chain);
    }
    //----GRID  MODE-------------------------------------------------
    else {
        // create the alien handler and attach it to the manager
        AliAnalysisAlien* plugin = new AliAnalysisAlien();
        plugin->AddIncludePath(
            "-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include "
            "-I$ALICE_PHYSICS/include");
        // plugin->Load("libpythia6_4_21.so");
        plugin->SetAnalysisSource("AliAnalysisTaskSigma1385temp.cxx");
        plugin->SetAdditionalLibs(
            "AliAnalysisTaskSigma1385temp.cxx AliAnalysisTaskSigma1385temp.h "
            "libpythia6_4_21.so");
        plugin->SetAliPhysicsVersion("vAN-202000127_ROOT6-1");
        plugin->SetAPIVersion("V1.1x");
        if (!ismc)
            plugin->SetRunPrefix("000");
        // plugin->SetDropToShell(0);

        if (foption.Contains("LHC16k")) {
            if (ismc) {
                if (foption.Contains("Gen")) {
                    plugin->SetGridDataDir(
                        "/alice/sim/2018/LHC18f1");  // general use, LHC18f1 for
                                                     // pass2, LHC17d20a1 for
                                                     // pass1
                } else {
                    plugin->SetGridDataDir(
                        "/alice/sim/2018/LHC18c6b4");  // resonance injected
                }
                plugin->SetDataPattern("*AliESDs.root");
            } else {
                plugin->SetGridDataDir("/alice/data/2016/LHC16k");
                    if(!isaod)
                        plugin->SetDataPattern("pass2/*/AliESDs.root");
                    else //AOD
                        plugin->SetDataPattern("pass2/AOD208/*/AliAOD.root");
            }
            Int_t end = LHC16k.size();
            if (foption.Contains("test"))
                end = 1;
            for (auto i = 0; i < end; i++){
                if (foption.Contains("range")){
                    if(i < istart) continue;
                    if(i > iend) break;
                }
                plugin->AddRunNumber(LHC16k.at(i));
            }
        }
        if (foption.Contains("SYS"))
            plugin->SetSplitMaxInputFileNumber(4000);
        else
            plugin->SetSplitMaxInputFileNumber(8000);
        plugin->SetExecutable(Form("%s%s.sh", taskname, option));
        plugin->SetTTL(40000);
        plugin->SetJDLName(Form("%s%s.jdl", taskname, option));
        plugin->SetKeepLogs(kTRUE);
        // plugin->SetMaxMergeStages(3);
        plugin->SetMaxMergeFiles(100);
        plugin->SetMergeViaJDL(kTRUE);
        plugin->SetOneStageMerging(kFALSE);
        plugin->SetCheckCopy(kFALSE);
        // plugin->SetNrunsPerMaster(kFALSE);
        plugin->SetUseSubmitPolicy(kTRUE);

        plugin->SetGridWorkingDir(Form("%s%s", taskname, option));
        plugin->SetGridOutputDir("out");

        // plugin->SetOutputToRunNo(kTRUE);
        plugin->SetOverwriteMode(kTRUE);
        plugin->SetUser("blim");
        mgr->SetGridHandler(plugin);

        plugin->SetRunMode(gridmode);
        if (strcmp(gridmode, "test") == 0)
            plugin->SetNtestFiles(1);

        mgr->StartAnalysis(localorgrid);
    }
}
