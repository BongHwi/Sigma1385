AliAnalysisTaskSigma1385temp* AddTaskSigma1385(
    const char* taskname = "Sigma1385",
    const char* option = "MB_Mix",
    int nmix = 10,
    const char* suffix = "MB") {
    AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) {
        return 0x0;
    }
    if (!mgr->GetInputEventHandler()) {
        return 0x0;
    }
    TString foption = option;
    Bool_t IsMC = kFALSE;
    if (foption.Contains("MC"))
        IsMC = kTRUE;
    AliAnalysisTaskSigma1385temp* taskSigma1385 =
        new AliAnalysisTaskSigma1385temp(Form("%s%s", taskname, suffix), IsMC);
    taskSigma1385->fEventCuts.fCentralityFramework = 1;
    taskSigma1385->fEventCuts.SetMaxVertexZposition(10);
    taskSigma1385->fEventCuts.SelectOnlyInelGt0(false);
    std::cout << "AddTaskSigma1385:: Option: " << option << std::endl;
    if (foption.Contains("MC")) {
        std::cout << "AliAnalysisTaskSigma1385temp:: MC mode " << std::endl;
        if (foption.Contains("Gen")) {
            taskSigma1385->SetIsPrimaryMC(kFALSE);  // default: kTRUE
            std::cout << "<GENERAL PURPOSE MC>" << std::endl;
        }
    }
    if (foption.Contains("pA")) {
        taskSigma1385->SetSigmaStarRapidityCutLow(0);  // default: -0.5
        std::cout << "AliAnalysisTaskSigma1385temp:: pA mode " << std::endl;
    }
    if (foption.Contains("Ap")) {
        taskSigma1385->SetSigmaStarRapidityCutHigh(0);  // default: 0.5
        std::cout << "AliAnalysisTaskSigma1385temp:: Ap mode " << std::endl;
    }
    if (foption.Contains("HM")) {
        taskSigma1385->SetHighMult(kTRUE); // default: kFALSE
        taskSigma1385->fEventCuts.fTriggerMask =
            AliVEvent::kHighMultV0;  // default: kINT7
        std::cout << "AliAnalysisTaskSigma1385temp:: HighMultV0 mode "
                  << std::endl;
    }
    if (foption.Contains("Mix")) {
        taskSigma1385->SetMixing(kTRUE);
        taskSigma1385->SetnMix(nmix);

        // If we have a TrackMixer task, use it.
        auto tasks = (TObjArray*)mgr->GetTasks();
        auto IsMixer = kFALSE;
        for (int ntask = 0; ntask < tasks->GetEntries(); ntask++) {
            if((tasks->At(ntask))->InheritsFrom(AliAnalysisTaskTrackMixertemp::Class())){
                taskSigma1385->SetMixerTask((AliAnalysisTaskTrackMixertemp*)tasks->At(ntask));
                ((AliAnalysisTaskTrackMixertemp*)tasks->At(ntask))->SetnMix(nmix);
                IsMixer = kTRUE;
                break;
            }
        }
        // If we don't have a TrackMixer task, use Built-in mixer.
        if(!IsMixer) {
            taskSigma1385->SetUseBuiltinMixer(kTRUE);
            std::cout << "Built-in Mixer mode" << std::endl;
        }
        std::cout << "Event Mix mode: " << nmix << "times" << std::endl;
    }

    if (!taskSigma1385)
        return 0x0;
    mgr->AddTask(taskSigma1385);

    AliAnalysisDataContainer* cinput = mgr->GetCommonInputContainer();
    mgr->ConnectInput(taskSigma1385, 0, cinput);

    AliAnalysisDataContainer* coutputSigma1385 = mgr->CreateContainer(
        Form("%s%s", taskname, suffix), TList::Class(),
        AliAnalysisManager::kOutputContainer, "AnalysisResults.root");
    mgr->ConnectOutput(taskSigma1385, 1, coutputSigma1385);

    AliAnalysisDataContainer* coutputSigma1385Tuple = mgr->CreateContainer(
        Form("%s%s_tree",taskname, suffix), TNtuple::Class(), AliAnalysisManager::kOutputContainer,
        "AnalysisResults.root");
    coutputSigma1385Tuple->SetSpecialOutput();
    mgr->ConnectOutput(taskSigma1385, 2, coutputSigma1385Tuple);

    return taskSigma1385;
}
