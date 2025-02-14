///////////////////////////////////////////////////////////////////
//                                                               //            
// AddCaloHFEpp                                                  //
// Author: T. Suzuki Univ. of Tsukuba                            //
//                                                               //
///////////////////////////////////////////////////////////////////
class AliAnalysisDataContainer;

AliAnalysisTaskCaloHFEpp* AddTaskCaloHFEpp(TString name = "name",
		TString dataname = "dataname",
		Bool_t flagEG1,
		Bool_t flagEG2,
		Bool_t flagDG1,
		Bool_t flagDG2,
		Bool_t SetFlagClsTypeEMC,
		Bool_t SetFlagClsTypeDCAL,
		Bool_t fMC,
		Double_t TrackEtaMin,
		Double_t TrackEtaMax,
		Int_t NTPCClust,
		Int_t NITSClust,
		Int_t NCrossedRow,
		Double_t DCAxy,
		Double_t DCAz,
		Double_t NsigmaMin,
		Double_t NsigmaMax,
		Double_t M20Min,
		Double_t M20Max,
		Double_t EopMin,
		Double_t EopMax,
		Double_t coneR,
		Double_t ptAsso,
		Double_t mimcle,
		TString  pte = "pte",
		Double_t MassMin,
		Double_t nref,
		Double_t nrefV0,
		TString estimatorFilename,
		Int_t minNtr,
		Int_t maxNtr,
                Int_t mtype)
{
    // get the manager via the static access member. since it's static, you don't need
    // an instance of the class to call the function
    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) {
        return 0x0;
    }
    // get the input event handler, again via a static method. 
    // this handler is part of the managing system and feeds events
    // to your task
    if (!mgr->GetInputEventHandler()) {
        return 0x0;
    }



    // by default, a file is open for writing. here, we get the filename
    TString fileName = AliAnalysisManager::GetCommonFileName();
    fileName += ":CaloHFEpp";      // create a subfolder in the file
    // now we create an instance of your task
    AliAnalysisTaskCaloHFEpp* task = new AliAnalysisTaskCaloHFEpp(name.Data());   
    task -> SetEG1(flagEG1);
    task -> SetEG2(flagEG2);
    task -> SetDG1(flagDG1);
    task -> SetDG2(flagDG2);
    task -> SetfFlagClsTypeEMC(SetFlagClsTypeEMC);
    task -> SetfFlagClsTypeDCAL(SetFlagClsTypeDCAL);

    task -> SetTrackEta(TrackEtaMin,TrackEtaMax);
    task -> SetTrackClust(NTPCClust,NITSClust,NCrossedRow);
    task -> SetDCA(DCAxy,DCAz);
    task -> SetNsigma(NsigmaMin,NsigmaMax);
    task -> SetM20(M20Min,M20Max);
    task -> SetEop(EopMin,EopMax);
    task -> SetConeR(coneR);
    task -> SetptAsso(ptAsso);
    task -> SetMimClE(mimcle);
    task -> SetptCut(pte);
    task -> SetMassMin(MassMin);
    task -> SetNref(nref);
    task -> SetNrefV0(nrefV0);
    task -> SetMinNtr(minNtr);
    task -> SetMaxNtr(maxNtr);
    task -> SetEstimatorFile(estimatorFilename);
    task -> SetMultType(mtype);

    /*
    TFile* fEstimator=TFile::Open(estimatorFilename.Data());
    if(!fEstimator){
	    AliFatal("File with multiplicity estimator not found\n");
	    return;
    }

    // Get weight for N_{tracklet}
    TH1D* weightNtrkl = (TH1D*)fEstimator->Get("weightNtrkl")->Clone("weightNtrkl_clone");
    if(!weightNtrkl){
	    AliFatal("Multiplicity estimator for weight not found! Please check your estimator file");
	    return;
    }
    task->SetWeightNtrkl(weightNtrkl);
   */

    if(!task) return 0x0;

    // add your task to the manager
    mgr->AddTask(task);

    TString containerName = mgr->GetCommonFileName();
    containerName += ":PWGHF_hfeCalpp";
    TString SubcontainerName = Form("hfeCalpp");
    SubcontainerName += name;
    AliAnalysisDataContainer *cinput  = mgr->GetCommonInputContainer();
    AliAnalysisDataContainer *coutput1 = mgr->CreateContainer(SubcontainerName, TList::Class(),AliAnalysisManager::kOutputContainer, containerName.Data());
    mgr->ConnectInput(task, 0, cinput);
    mgr->ConnectOutput(task, 1, coutput1); 

    return task;
}
