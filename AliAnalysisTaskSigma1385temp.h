#ifndef AliAnalysisTaskSigma1385temp_H
#define AliAnalysisTaskSigma1385temp_H

#include <THnSparse.h>
#include <TNtupleD.h>

#include <deque>

#include "AliAnalysisTaskSE.h"
#include "AliEventCuts.h"
class THistManager;
class AliPIDResponse;
class AliESDtrackCuts;

class AliAnalysisTaskSigma1385temp : public AliAnalysisTaskSE {
 public:
  AliAnalysisTaskSigma1385temp();
  AliAnalysisTaskSigma1385temp(const char* name, Bool_t MCcase);
  virtual ~AliAnalysisTaskSigma1385temp();

  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t* option);
  virtual void Terminate(Option_t* option);
  void SetFillQAPlot(Bool_t input) { fFillQAPlot = input; }
  void SetMixing(Bool_t setmixing) { fSetMixing = setmixing; }
  void SetnMix(Int_t nMix) { fnMix = nMix; }
  void SetIsPrimaryMC(Bool_t isprimarymc) { fIsPrimaryMC = isprimarymc; }
  void SetINEL(Bool_t input) { fIsINEL = input; }
  void SetHighMult(Bool_t input) { fIsHM = input; }
  void SetAssymCut(Bool_t input) { fUseAssymCut = input; }
  void SetFillnTuple(Bool_t fillntuple) { fFillnTuple = fillntuple; }

  // Setter for cut variables
  void SetFilterbitSigmaStarPion(Double_t lParameter) {
    fFilterBit = lParameter;
  }
  void SetMaxNsigSigmaStarPion(Double_t lParameter) {
    fTPCNsigSigmaStarPionCut = lParameter;
  }
  void SetMaxEtaSigmaStarPion(Double_t lParameter) {
    fSigmaStarPionEtaCut = lParameter;
  }
  void SetMaxVertexZSigmaStarPion(Double_t lParameter) {
    fSigmaStarPionZVertexCut = lParameter;
  }
  void SetMaxVertexXYsigSigmaStarPion(Double_t lParameter) {
    fSigmaStarPionXYVertexSigmaCut = lParameter;
  }

  void SetMaxNsigV0Proton(Double_t lParameter) {
    fTPCNsigLambdaProtonCut = lParameter;
  }
  void SetMaxNsigV0Pion(Double_t lParameter) {
    fTPCNsigLambdaPionCut = lParameter;
  }
  void SetMaxDCAV0daughters(Double_t lParameter) {
    fDCADistLambdaDaughtersCut = lParameter;
  }
  void SetMaxDCAPVV0(Double_t lParameter) { fDCArDistLambdaPVCut = lParameter; }
  void SetMaxDCAPVV0PosDaughter(Double_t lParameter) {
    fDCAPositiveTrack = lParameter;
  }
  void SetMaxDCAPVV0NegDaughter(Double_t lParameter) {
    fDCANegativeTrack = lParameter;
  }
  void SetMinCPAV0(Double_t lParameter) {
    fV0CosineOfPointingAngleCut = lParameter;
  }
  void SetMaxRapidityV0(Double_t lParameter) {
    fMaxLambdaRapidity = lParameter;
  }
  void SetLowRadiusV0(Double_t lParameter) { fLambdaLowRadius = lParameter; }
  void SetHighRadiusV0(Double_t lParameter) { fLambdaHighRadius = lParameter; }
  void SetLifetimeV0(Double_t lParameter) { fLambdaLifetime = lParameter; }
  void SetMaxMassWindowV0(Double_t lParameter) {
    fV0MassWindowCut = lParameter;
  }

  void SetSigmaStarRapidityCutHigh(Double_t lParameter) {
    fSigmaStarYCutHigh = lParameter;
  }
  void SetSigmaStarRapidityCutLow(Double_t lParameter) {
    fSigmaStarYCutLow = lParameter;
  }
  void SetSigmaStarAssymCutHigh(Double_t lParameter) {
    fSigmaStarAssymCutHigh = lParameter;
  }
  void SetSigmaStarAssymCutLow(Double_t lParameter) {
    fSigmaStarAssymCutLow = lParameter;
  }

  Bool_t GoodTracksSelection();
  Bool_t GoodV0Selection();
  void FillTracks();
  void FillNtuples();
  void FillMCinput(AliMCEvent* fMCEvent, int Fillbin = 0);
  void FillTrackToEventPool();
  Bool_t IsTrueSigmaStar(UInt_t v0, UInt_t pion, UInt_t BkgCheck = 0);
  double GetTPCnSigma(AliVTrack* track, AliPID::EParticleType type);
  void GetImpactParam(AliVTrack* track, Float_t p[2], Float_t cov[3]);
  void SetCutOpen();

  // helper
  THnSparse* CreateTHnSparse(TString name,
                             TString title,
                             Int_t ndim,
                             std::vector<TAxis> bins,
                             Option_t* opt = "");
  THnSparse* CreateTHnSparse(TString name,
                             TString title,
                             TString templ,
                             Option_t* opt = "");
  Long64_t FillTHnSparse(TString name,
                         std::vector<Double_t> x,
                         Double_t w = 1.);
  Long64_t FillTHnSparse(THnSparse* h,
                         std::vector<Double_t> x,
                         Double_t w = 1.);
  TAxis AxisFix(TString name, int nbin, Double_t xmin, Double_t xmax);
  TAxis AxisVar(TString name, std::vector<Double_t> bin);
  TAxis AxisStr(TString name, std::vector<TString> bin);

  AliEventCuts fEventCuts;  // Event cuts

 private:
  typedef std::vector<AliVTrack*> tracklist;
  typedef std::deque<tracklist> eventpool;
  typedef std::vector<std::vector<eventpool>> mixingpool;

  AliESDtrackCuts* fTrackCuts;   //!
  AliPIDResponse* fPIDResponse;  //!

  AliVEvent* fEvt;             //!
  AliMCEvent* fMCEvent;        //!
  THistManager* fHistos;       //!
  AliAODVertex* fVertex;       //!
  TNtupleD* fNtupleSigma1385;  //!
  TClonesArray* fMCArray;      //!

  Bool_t fIsAOD;        //!
  Bool_t fIsNano;       //!
  Bool_t fSetMixing;    //
  Bool_t fFillQAPlot;   //
  Bool_t fIsMC;         //
  Bool_t fIsPrimaryMC;  //
  Bool_t fFillnTuple;   //
  Bool_t fIsINEL;       //
  Bool_t fIsHM;         //
  Bool_t fUseAssymCut;  //

  mixingpool fEMpool;  //!
  TAxis fBinCent;      //!
  TAxis fBinZ;         //!
  Double_t fPosPV[3];  //!

  Double_t fCent;  //!
  Int_t fnMix;     //!
  Int_t fCentBin;  //!
  Int_t fZbin;     //!

  // Pion cuts
  UInt_t fFilterBit;                        //
  Double_t fTPCNsigSigmaStarPionCut;        //
  Double_t fSigmaStarPionEtaCut;            //
  Double_t fSigmaStarPionZVertexCut;        //
  Double_t fSigmaStarPionXYVertexSigmaCut;  //

  // Lambda cuts
  Double_t fTPCNsigLambdaProtonCut;      //
  Double_t fTPCNsigLambdaPionCut;        //
  Double_t fDCADistLambdaDaughtersCut;   //
  Double_t fDCArDistLambdaPVCut;         //
  Double_t fDCAPositiveTrack;            //
  Double_t fDCANegativeTrack;            //
  Double_t fV0CosineOfPointingAngleCut;  //
  Double_t fMaxLambdaRapidity;           //
  Double_t fLambdaLowRadius;             //
  Double_t fLambdaHighRadius;            //
  Double_t fLambdaLifetime;              //
  Double_t fV0MassWindowCut;             //

  // Sigma Star cut
  Double_t fSigmaStarYCutHigh;      //
  Double_t fSigmaStarYCutLow;       //
  Double_t fSigmaStarAssymCutHigh;  //
  Double_t fSigmaStarAssymCutLow;   //

  // Good track/v0 vector array
  std::vector<UInt_t> fGoodTrackArray;
  std::vector<std::vector<UInt_t>> fGoodV0Array;

  ClassDef(AliAnalysisTaskSigma1385temp, 11);
  // Add rapidity/radius/Lifetime/Y cut of lambda
  // Add NanoOption
  // 4: Add GetImpactParm function for nano
  // 5: Seprate MC Sparse, INEL study capability
  // 6: Update some of deafult vaules
  // 7: Add skipping option for QA histos
  // 8: Rebuild MC part
  // 9: Update class format
  // 10: Fix streamer issue
  // 11: Add Assym cut option
};

#endif