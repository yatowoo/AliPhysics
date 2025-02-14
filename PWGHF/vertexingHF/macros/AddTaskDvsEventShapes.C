AliAnalysisTaskSEDvsEventShapes *AddTaskDvsEventShapes(Int_t system=0,
                                                       Bool_t readMC=kFALSE,
                                                       Int_t MCOption=0,
                                                       Int_t pdgMeson=411,
                                                       TString finDirname="Loose",
                                                       TString filename="",
                                                       TString finAnObjname="AnalysisCuts",
                                                       Bool_t CalculateSphericity=kFALSE, // Add Sphericity calculations
                                                       Int_t SoSparseChecks=0,/*0 mult, 1 multUncorr, 2 NoPid, 3 All*/
                                                       Double_t ptMin=0.15,
                                                       Double_t ptMax=10.,
                                                       Double_t etaMin=-0.8,
                                                       Double_t etaMax=0.8,
                                                       Int_t minMult=3,
                                                       Double_t phiStepSizeDeg=0.1,
                                                       Int_t filtbit1=256,
                                                       Int_t filtbit2=512,
                                                       TString estimatorFilename="",
                                                       Double_t refMult=9.26,
                                                       Bool_t subtractDau=kFALSE,
                                                       Bool_t subtractDauFromSphero=kFALSE, //Subtract D0 dau track from Sphero calculation
                                                       Bool_t RemoveD0fromDstar=kFALSE, //remove D0 from Dstar: to switch it on, make subtractDauFromSphero = kFALSE
                                                       Int_t NchWeight=0,
                                                       Bool_t PtWeight=kFALSE,
                                                       Int_t recoEstimator = AliAnalysisTaskSEDvsEventShapes::kNtrk10,
                                                       Int_t MCEstimator = AliAnalysisTaskSEDvsEventShapes::kEta10,
							                           Bool_t isPPbData=kFALSE,
							                           Int_t year=18, 
                                                       Bool_t unweightS0=kFALSE)
{
    //
    // Macro for the AliAnalysisTaskSE for D candidates vs Multiplicity as a function of Event shape variables
    // Invariant mass histogram in Sphero(i)city, pt and multiplicity bins in a 3D histogram
    //   different estimators implemented
    //==============================================================================
    
    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) {
        ::Error("AddTaskDvsEventShapes", "No analysis manager to connect to.");
    }
    
    Bool_t stdcuts=kFALSE;
    TFile* filecuts;
    if( filename.EqualTo("") ) {
        stdcuts=kTRUE;
    } else {
        filecuts=TFile::Open(filename.Data());
        if(!filecuts ||(filecuts&& !filecuts->IsOpen())){
            Printf("FATAL: Input file not found : check your cut object");
	    return NULL;
        }
    }
    
    
    //Analysis Task
    AliRDHFCuts *analysiscuts=0x0;
    
    TString Name="";
    if(pdgMeson==411){
        if(stdcuts) {
            analysiscuts = new AliRDHFCutsDplustoKpipi();
            if (system == 0) analysiscuts->SetStandardCutsPP2010();
            else analysiscuts->SetStandardCutsPbPb2011();
        }
        else analysiscuts = (AliRDHFCutsDplustoKpipi*)filecuts->Get(finAnObjname);
        Name="Dplus";
    }else if(pdgMeson==421){
        if(stdcuts) {
            analysiscuts = new AliRDHFCutsD0toKpi();
            if (system == 0) analysiscuts->SetStandardCutsPP2010();
            else analysiscuts->SetStandardCutsPbPb2011();
        }
        else analysiscuts = (AliRDHFCutsD0toKpi*)filecuts->Get(finAnObjname);
        Name="D0";
    }else if(pdgMeson==413){
        if(stdcuts) {
            analysiscuts = new AliRDHFCutsDStartoKpipi();
            if (system == 0) analysiscuts->SetStandardCutsPP2010();
            else analysiscuts->SetStandardCutsPbPb2011();
        }
        else analysiscuts = (AliRDHFCutsDStartoKpipi*)filecuts->Get(finAnObjname);
        Name="DStar";
    }
    
    AliAnalysisTaskSEDvsEventShapes *dEvtShapeTask = new AliAnalysisTaskSEDvsEventShapes("dEvtShapeAnalysis",pdgMeson,analysiscuts,isPPbData);
    dEvtShapeTask->SetReadMC(readMC);
    dEvtShapeTask->SetS0unweight(unweightS0);
    dEvtShapeTask->SetDebugLevel(0);
    dEvtShapeTask->SetUseBit(kTRUE);
    dEvtShapeTask->SetDoImpactParameterHistos(kFALSE);
    dEvtShapeTask->SetFillSoSparseForMultUncorrNoPid(SoSparseChecks); //Set Fill THnSparse for Spherocity
    dEvtShapeTask->SetEventShapeParameters(ptMin, ptMax, etaMin, etaMax, minMult, phiStepSizeDeg, filtbit1, filtbit2); //parameters to calculate Sphero(i)city
    dEvtShapeTask->SetCalculationsForSphericity(CalculateSphericity);
    dEvtShapeTask->SetSubtractTrackletsFromDaughters(subtractDau);
    dEvtShapeTask->SetRecomputeSpherocityWithoutDau(subtractDauFromSphero);
    dEvtShapeTask->SetRemoveD0fromDstar(RemoveD0fromDstar);
    dEvtShapeTask->SetMultiplicityEstimator(recoEstimator);
    dEvtShapeTask->SetMCPrimariesEstimator(MCEstimator);
    dEvtShapeTask->SetMCOption(MCOption);
    if(isPPbData) dEvtShapeTask->SetIsPPbData();
    
    if(NchWeight){
        TH1F *hNchPrimaries = NULL;
        TH1F *hMeasNchPrimaries = NULL;
        if(NchWeight==1){
            if(isPPbData) {
                hNchPrimaries = (TH1F*)filecuts->Get("hNtrUnCorrEvWithDWeight"); // MC distribution
            }
            else hNchPrimaries = (TH1F*)filecuts->Get("hGenPrimaryParticlesInelGt0");
            if(hNchPrimaries) {
                dEvtShapeTask->UseMCNchWeight(NchWeight);
                dEvtShapeTask->SetHistoNchWeight(hNchPrimaries);
            } else {
                Printf("FATAL: Histogram for Nch multiplicity weights not found");
                return 0x0;
            }
            hMeasNchPrimaries = (TH1F*)filecuts->Get("hMeasNtrUnCorrEvWithD"); // data distribution
            if(hMeasNchPrimaries) {
                dEvtShapeTask->SetMeasuredNchHisto(hMeasNchPrimaries);
            }
        }
        else if(NchWeight==2){
            hNchPrimaries = (TH1F*)filecuts->Get("hNtrUnCorrEvWithDWeight"); // MC distribution
            hMeasNchPrimaries = (TH1F*)filecuts->Get("hMeasNtrUnCorrEvWithD"); // data distribution
            if(hNchPrimaries && hMeasNchPrimaries) {
                dEvtShapeTask->UseMCNchWeight(NchWeight);
                dEvtShapeTask->SetHistoNchWeight(hNchPrimaries);
                dEvtShapeTask->SetMeasuredNchHisto(hMeasNchPrimaries);
            } else {
                Printf("FATAL: Histogram for Ntrk multiplicity weights not found");
                return 0x0;
            }
        }
    }
    dEvtShapeTask->UsePtWeight(PtWeight);
    
    if(pdgMeson==421) {
        dEvtShapeTask->SetMassLimits(1.5648,2.1648);
        dEvtShapeTask->SetNMassBins(200);
    }else if(pdgMeson==411)dEvtShapeTask->SetMassLimits(pdgMeson,0.2);
    
    if(estimatorFilename.EqualTo("") ) {
        printf("Estimator file not provided, multiplcity corrected histograms will not be filled\n");
    } else{
        
        TFile* fileEstimator=TFile::Open(estimatorFilename.Data());
        if(!fileEstimator)  {
            Printf("FATAL: File with multiplicity estimator not found\n");
            return NULL;
        }
        
        dEvtShapeTask->SetReferenceMultiplcity(refMult);
        
        const Char_t* profilebasename="SPDmult10";
        if(recoEstimator==AliAnalysisTaskSEDvsEventShapes::kVZEROA || recoEstimator==AliAnalysisTaskSEDvsEventShapes::kVZEROAEq) profilebasename="VZEROAmult";
        else if(recoEstimator==AliAnalysisTaskSEDvsEventShapes::kVZERO || recoEstimator==AliAnalysisTaskSEDvsEventShapes::kVZEROEq) profilebasename="VZEROMmult";
        cout<<endl<<endl<<" profilebasename="<<profilebasename<<endl<<endl;
        
        if (isPPbData) {    //Only use two profiles if pPb
            const Char_t* periodNames[2] = {"LHC13b", "LHC13c"};
            TProfile* multEstimatorAvg[2];
            for(Int_t ip=0; ip<2; ip++) {
                cout<< " Trying to get "<<Form("%s_%s",profilebasename,periodNames[ip])<<endl;
                multEstimatorAvg[ip] = (TProfile*)(fileEstimator->Get(Form("%s_%s",profilebasename,periodNames[ip]))->Clone(Form("%s_%s_clone",profilebasename,periodNames[ip])));
                if (!multEstimatorAvg[ip]) {
                    Printf("Multiplicity estimator for %s not found! Please check your estimator file",periodNames[ip]);
                    return NULL;
                }
            }
            dEvtShapeTask->SetMultiplVsZProfileLHC13b(multEstimatorAvg[0]);
            dEvtShapeTask->SetMultiplVsZProfileLHC13c(multEstimatorAvg[1]);
        }
        else {  ///////////////average SPD tracklet correction for pp data
          if(year==10)
	    {  const Char_t* periodNames[4] = {"LHC10b", "LHC10c", "LHC10d", "LHC10e"};
            TProfile* multEstimatorAvg[4];
            for(Int_t ip=0; ip<4; ip++) {
                multEstimatorAvg[ip] = (TProfile*)(fileEstimator->Get(Form("%s_%s",profilebasename,periodNames[ip]))->Clone(Form("%s_%s_clone",profilebasename,periodNames[ip])));
                if (!multEstimatorAvg[ip]) {
                    Printf("Multiplicity estimator for %s not found! Please check your estimator file",periodNames[ip]);
                    return NULL;
                }
            }
            dEvtShapeTask->SetMultiplVsZProfileLHC10b(multEstimatorAvg[0]);
            dEvtShapeTask->SetMultiplVsZProfileLHC10c(multEstimatorAvg[1]);
            dEvtShapeTask->SetMultiplVsZProfileLHC10d(multEstimatorAvg[2]);
            dEvtShapeTask->SetMultiplVsZProfileLHC10e(multEstimatorAvg[3]);
	  }
 else if(year ==16)
	{
      const Char_t* periodNames[10]={"LHC16d","LHC16e","LHC16g","LHC16h_1", "LHC16h_2","LHC16j","LHC16k","LHC16l","LHC16o","LHC16p"};
      TProfile *multEstimatorAvg[10];
      for(Int_t ip=0;ip<10; ip++){
        multEstimatorAvg[ip] = (TProfile*)(fileEstimator->Get(Form("%s_%s",profilebasename,periodNames[ip]))->Clone(Form("%s_%s_clone",profilebasename,periodNames[ip])));
  if (!multEstimatorAvg[ip]) {
    Printf("Multiplicity estimator for %s not found! Please check your estimator file",periodNames[ip]);
    return NULL;
  }
      }
      dEvtShapeTask->SetMultiplVsZProfileLHC16d(multEstimatorAvg[0]);
      dEvtShapeTask->SetMultiplVsZProfileLHC16e(multEstimatorAvg[1]);
      dEvtShapeTask->SetMultiplVsZProfileLHC16g(multEstimatorAvg[2]);
      dEvtShapeTask->SetMultiplVsZProfileLHC16h1(multEstimatorAvg[3]);
      dEvtShapeTask->SetMultiplVsZProfileLHC16h2(multEstimatorAvg[4]);
      dEvtShapeTask->SetMultiplVsZProfileLHC16j(multEstimatorAvg[5]);
      dEvtShapeTask->SetMultiplVsZProfileLHC16k(multEstimatorAvg[6]);
      dEvtShapeTask->SetMultiplVsZProfileLHC16l(multEstimatorAvg[7]);
      dEvtShapeTask->SetMultiplVsZProfileLHC16o(multEstimatorAvg[8]);
      dEvtShapeTask->SetMultiplVsZProfileLHC16p(multEstimatorAvg[9]);

	}//2016 data
      else if(year == 17)
	{
     const Char_t* periodNames[10]={"LHC17e","LHC17f","LHC17h","LHC17i", "LHC17j","LHC17k","LHC17l","LHC17m","LHC17o","LHC17r"};
      TProfile *multEstimatorAvg[10];
      for(Int_t ip=0;ip<10; ip++){
        multEstimatorAvg[ip] = (TProfile*)(fileEstimator->Get(Form("%s_%s",profilebasename,periodNames[ip]))->Clone(Form("%s_%s_clone",profilebasename,periodNames[ip])));
        if (!multEstimatorAvg[ip]) {
        Printf("Multiplicity estimator for %s not found! Please check your estimator file",periodNames[ip]);
        return NULL;
        }
      }
      dEvtShapeTask->SetMultiplVsZProfileLHC17e(multEstimatorAvg[0]);
      dEvtShapeTask->SetMultiplVsZProfileLHC17f(multEstimatorAvg[1]);
      dEvtShapeTask->SetMultiplVsZProfileLHC17h(multEstimatorAvg[2]);
      dEvtShapeTask->SetMultiplVsZProfileLHC17i(multEstimatorAvg[3]);
      dEvtShapeTask->SetMultiplVsZProfileLHC17j(multEstimatorAvg[4]);
      dEvtShapeTask->SetMultiplVsZProfileLHC17k(multEstimatorAvg[5]);
      dEvtShapeTask->SetMultiplVsZProfileLHC17l(multEstimatorAvg[6]);
      dEvtShapeTask->SetMultiplVsZProfileLHC17m(multEstimatorAvg[7]);
      dEvtShapeTask->SetMultiplVsZProfileLHC17o(multEstimatorAvg[8]);
      dEvtShapeTask->SetMultiplVsZProfileLHC17r(multEstimatorAvg[9]);
	}//2017 data
      else if(year == 18)
	{
const Char_t* periodNames[14]={"LHC18b","LHC18d","LHC18e","LHC18f", "LHC18g","LHC18h","LHC18i","LHC18j","LHC18k","LHC18l","LHC18m","LHC18n","LHC18o","LHC18p"};
      TProfile *multEstimatorAvg[14];
      for(Int_t ip=0;ip<14; ip++){
        multEstimatorAvg[ip] = (TProfile*)(fileEstimator->Get(Form("%s_%s",profilebasename,periodNames[ip]))->Clone(Form("%s_%s_clone",profilebasename,periodNames[ip])));
        if (!multEstimatorAvg[ip]) {
        Printf("Multiplicity estimator for %s not found! Please check your estimator file",periodNames[ip]);
        return NULL;
        }
      }
      dEvtShapeTask->SetMultiplVsZProfileLHC18b(multEstimatorAvg[0]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18d(multEstimatorAvg[1]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18e(multEstimatorAvg[2]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18f(multEstimatorAvg[3]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18g(multEstimatorAvg[4]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18h(multEstimatorAvg[5]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18i(multEstimatorAvg[6]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18j(multEstimatorAvg[7]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18k(multEstimatorAvg[8]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18l(multEstimatorAvg[9]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18m(multEstimatorAvg[10]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18n(multEstimatorAvg[11]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18o(multEstimatorAvg[12]);
      dEvtShapeTask->SetMultiplVsZProfileLHC18p(multEstimatorAvg[13]);
    }//2018 data

	}
    }
    mgr->AddTask(dEvtShapeTask);
    
    // Create containers for input/output
    
    TString inname = "cinput";
    TString outname = "coutput";
    TString cutsname = "coutputCuts";
    TString normname = "coutputNorm";
    TString profname = "coutputProf";
    TString effname = "coutputEffCorr";
    
    inname += Name.Data();
    outname += Name.Data();
    cutsname += Name.Data();
    normname += Name.Data();
    profname += Name.Data();
    effname += Name.Data();
    inname += finDirname.Data();
    outname += finDirname.Data();
    cutsname += finDirname.Data();
    normname += finDirname.Data();
    profname += finDirname.Data();
    effname += finDirname.Data();
    
    AliAnalysisDataContainer *cinput = mgr->CreateContainer(inname,TChain::Class(),AliAnalysisManager::kInputContainer);
    
    TString outputfile = AliAnalysisManager::GetCommonFileName();
    outputfile += ":PWG3_D2H_DEvtShape_";
    outputfile += Name.Data(); 
    outputfile += finDirname.Data(); 
    
    AliAnalysisDataContainer *coutputCuts = mgr->CreateContainer(cutsname,TList::Class(),AliAnalysisManager::kOutputContainer,outputfile.Data());
    AliAnalysisDataContainer *coutput = mgr->CreateContainer(outname,TList::Class(),AliAnalysisManager::kOutputContainer,outputfile.Data());
    AliAnalysisDataContainer *coutputNorm = mgr->CreateContainer(normname,TList::Class(),AliAnalysisManager::kOutputContainer,outputfile.Data());
    AliAnalysisDataContainer *coutputProf = mgr->CreateContainer(profname,TList::Class(),AliAnalysisManager::kOutputContainer,outputfile.Data());
    AliAnalysisDataContainer *coutputEffCorr = 0x0;
    if(readMC) coutputEffCorr = mgr->CreateContainer(effname,TList::Class(),AliAnalysisManager::kOutputContainer,outputfile.Data());

    
    mgr->ConnectInput(dEvtShapeTask,0,mgr->GetCommonInputContainer());
    
    mgr->ConnectOutput(dEvtShapeTask,1,coutput);
    mgr->ConnectOutput(dEvtShapeTask,2,coutputCuts);
    mgr->ConnectOutput(dEvtShapeTask,3,coutputNorm);
    mgr->ConnectOutput(dEvtShapeTask,4,coutputProf);
    if(readMC) mgr->ConnectOutput(dEvtShapeTask,5,coutputEffCorr);
    
    return dEvtShapeTask;
}
