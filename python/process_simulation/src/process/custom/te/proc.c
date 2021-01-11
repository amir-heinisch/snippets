#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "proc.h"
//#include "includes/tmwtypes.h"


#define IS_PARAM_DOUBLE(pVal) (mxIsNumeric(pVal)  && !mxIsLogical(pVal) &&\
                               !mxIsEmpty(pVal)   && !mxIsSparse(pVal) &&\
                               !mxIsComplex(pVal) && mxIsDouble(pVal))    

struct ProcessValues {
    doublereal xmeas[41], xmeasadd[32], xmeasdist[21], xmeasmonitor[62], 
               xmeascomp[96], xmv[12];
};

struct DistVector{
    doublereal idv[29];
};

struct RandState {
    doublereal g;
	doublereal measnoise;
	doublereal procdist;
};

struct ComponentData{
	doublereal avp[8], bvp[8], cvp[8], ah[8], bh[8], ch[8], ag[8], bg[8], 
		cg[8], av[8], ad[8], bd[8], cd[8], xmw[8];
};

struct Process{
	doublereal    uclr[8]  ,    ucvr[8]  ,    utlr     ,    utvr     , 
                   xlr[8]  ,     xvr[8]  ,     etr     ,     esr     ,
   			       tcr     ,     tkr     ,     dlr     ,     vlr     ,  
                   vvr     ,     vtr     ,     ptr     ,     ppr[8]  ,
			      crxr[8]  ,      rr[4]  ,      rh     ,     fwr     ,
                   twr     ,     qur     ,     hwr     ,     uar     , 
                 ucls[8]   ,    ucvs[8]  ,    utls     ,    utvs     , 
                  xls[8]   ,     xvs[8]  ,     ets     ,     ess     ,  
                  tcs      ,     tks     ,     dls     ,     vls     ,
                  vvs      ,     vts     ,     pts     ,     pps[8]  ,
	 	          fws      ,     tws     ,     qus     ,     hws     , 
                 uclc[8]   ,    utlc     ,     xlc[8]  ,     etc     ,
                  esc      ,     tcc     ,     dlc     ,     vlc     , 
                  vtc      ,     quc     ,    
                 ucvv[8]   ,    utvv     ,     xvv[8]  ,     etv     ,    
                  esv      ,     tcv     ,     tkv     ,     vtv     , 
                  ptv      ,     vcv[12] ,    vrng[12] ,    vtau[12] , 
                  ftm[13]  ,     fcm[104],     xst[104],    xmws[13] , 
         	   	  hst[13]  ,     tst[13] ,     sfr[8]  ,  cpflmx     , 
               cpprmx      ,    cpdh     ,    tcwr     ,    tcws     , 
   		          htr[3]   ,    agsp     ,    xdel[41] , xdeladd[24]  ,  
                  xns[41]  ,  xnsadd[34] ,    tgas     ,   tprod     ,     
                  vst[12]  ,    ivst[12];
};

struct RandProcess {
	doublereal adist[20], bdist[20], cdist[20], ddist[20], tlast[20], 
               tnext[20], hspan[20], hzero[20], sspan[20], szero[20], 
               spspan[20];
	doublereal idvwlk[20];
	doublereal rdumm;
};

struct stModelData {                                                            
    struct ProcessValues pv_; // set in teinit
    struct DistVector dvec_; // set in teinit, except for the last entry
    struct RandState randsd_; // set in teinit
    struct ComponentData const_; // set in teinit
    struct Process teproc_; // set in teinit
    struct RandProcess wlk_; // set in teinit
    char msg[256]; // only ever set in cas of valid shut-down constraints
    doublereal code_sd; // set manually in main
    doublereal tlastcomp; // set at the end of teinit
    integer MSFlag; // set at the beginning of teinit
};  


const integer NX     = 50;
const int NU         = 12;
const int NIDV       = 28;
const int NY         = 41;
const int NYADD      = 32;
const int NYDIST     = 21;
const int NYMONITOR  = 62;
const int NYCOMP     = 96;
const int NPAR       = 3;

const integer c__50 = 50;
const integer c__12 = 12;
const integer c__21 = 21;
const integer c__153 = 153;
const integer c__586 = 586;
const integer c__139 = 139;
const integer c__6 = 6;
const integer c__1 = 1;
const integer c__0 = 0;
const integer c__41 = 41;
const integer c__2 = 2;
const integer c__3 = 3;
const integer c__4 = 4;
const integer c__5 = 5;
const integer c__7 = 7;
const integer c__8 = 8;
const doublereal c_b73 = 1.1544;
const doublereal c_b74 = .3735;
const integer c__9 = 9;
const integer c__10 = 10;
const integer c__11 = 11;
const doublereal c_b123 = 4294967296.;
const integer c__13 = 13;
const integer c__14 = 14;
const integer c__15 = 15;
const integer c__16 = 16;
const integer c__17 = 17;
const integer c__18 = 18;
const integer c__19 = 19;
const integer c__20 = 20;


  



/*-------------------------------------------------------------------------
-                               t e i n i t                               -
-------------------------------------------------------------------------*/
int teinit(void *ModelData, 
                  const integer *nn, doublereal *time, doublereal *yy, 
                  doublereal *yp, doublereal *rseed, doublereal *MSFlag){
  /*----------------------------- Variables -----------------------------*/
  struct stModelData *tcModelData;
  #define isd ((doublereal *)&(*tcModelData).dvec_ + 28)
  integer i__;

  /*Typcast of Dataset Pointer*/
  tcModelData = ModelData;

  /*Parameter adjustments*/
  --yp;
  --yy;

  /*--------------------------- Function Body ---------------------------*/
  /*Evaluation of Structure Parameter*/
  if (MSFlag == NULL){
    (*tcModelData).MSFlag = 0;
  }else{
    (*tcModelData).MSFlag = (integer)*MSFlag;
  } //if (MSFlag == NULL){
  
  if(((*tcModelData).MSFlag & 0x8000) > 0){
    (*tcModelData).MSFlag = (integer)0x8000;
  } //if((*tcModelData).MSFlag & 0x8000) > 0){
  
  /*Component Data*/
  (*tcModelData).const_.xmw[0] = (float)2.;           //in g/mol / kg/kmol
  (*tcModelData).const_.xmw[1] = (float)25.4;         //in g/mol / kg/kmol
  (*tcModelData).const_.xmw[2] = (float)28.;          //in g/mol / kg/kmol
  (*tcModelData).const_.xmw[3] = (float)32.;          //in g/mol / kg/kmol
  (*tcModelData).const_.xmw[4] = (float)46.;          //in g/mol / kg/kmol
  (*tcModelData).const_.xmw[5] = (float)48.;          //in g/mol / kg/kmol
  (*tcModelData).const_.xmw[6] = (float)62.;          //in g/mol / kg/kmol
  (*tcModelData).const_.xmw[7] = (float)76.;          //in g/mol / kg/kmol  

  (*tcModelData).const_.avp[0] = (float)0.;           //in ln(Torr)
  (*tcModelData).const_.avp[1] = (float)0.;           //in ln(Torr)
  (*tcModelData).const_.avp[2] = (float)0.;           //in ln(Torr)
  (*tcModelData).const_.avp[3] = (float)15.92;        //in ln(Torr)
  (*tcModelData).const_.avp[4] = (float)16.35;        //in ln(Torr)
  (*tcModelData).const_.avp[5] = (float)16.35;        //in ln(Torr)
  (*tcModelData).const_.avp[6] = (float)16.43;        //in ln(Torr)
  (*tcModelData).const_.avp[7] = (float)17.21;        //in ln(Torr)
  (*tcModelData).const_.bvp[0] = (float)0.;           //in ln(Torr)*°C
  (*tcModelData).const_.bvp[1] = (float)0.;           //in ln(Torr)*°C
  (*tcModelData).const_.bvp[2] = (float)0.;           //in ln(Torr)*°C
  (*tcModelData).const_.bvp[3] = (float)-1444.;       //in ln(Torr)*°C
  (*tcModelData).const_.bvp[4] = (float)-2114.;       //in ln(Torr)*°C
  (*tcModelData).const_.bvp[5] = (float)-2114.;       //in ln(Torr)*°C
  (*tcModelData).const_.bvp[6] = (float)-2748.;       //in ln(Torr)*°C
  (*tcModelData).const_.bvp[7] = (float)-3318.;       //in ln(Torr)*°C
  (*tcModelData).const_.cvp[0] = (float)0.;           //in °C
  (*tcModelData).const_.cvp[1] = (float)0.;           //in °C
  (*tcModelData).const_.cvp[2] = (float)0.;           //in °C
  (*tcModelData).const_.cvp[3] = (float)259.;         //in °C
  (*tcModelData).const_.cvp[4] = (float)265.5;        //in °C
  (*tcModelData).const_.cvp[5] = (float)265.5;        //in °C
  (*tcModelData).const_.cvp[6] = (float)232.9;        //in °C
  (*tcModelData).const_.cvp[7] = (float)249.6;        //in °C
  
  (*tcModelData).const_.ad[0] = (float)1.;            //in ?
  (*tcModelData).const_.ad[1] = (float)1.;            //in ?
  (*tcModelData).const_.ad[2] = (float)1.;            //in ?
  (*tcModelData).const_.ad[3] = (float)23.3;          //in ?
  (*tcModelData).const_.ad[4] = (float)33.9;          //in ?
  (*tcModelData).const_.ad[5] = (float)32.8;          //in ?
  (*tcModelData).const_.ad[6] = (float)49.9;          //in ?
  (*tcModelData).const_.ad[7] = (float)50.5;          //in ?
  (*tcModelData).const_.bd[0] = (float)0.;            //in ?
  (*tcModelData).const_.bd[1] = (float)0.;            //in ?
  (*tcModelData).const_.bd[2] = (float)0.;            //in ?
  (*tcModelData).const_.bd[3] = (float)-.07;          //in ?
  (*tcModelData).const_.bd[4] = (float)-.0957;        //in ?
  (*tcModelData).const_.bd[5] = (float)-.0995;        //in ?
  (*tcModelData).const_.bd[6] = (float)-.0191;        //in ?
  (*tcModelData).const_.bd[7] = (float)-.0541;        //in ?
  (*tcModelData).const_.cd[0] = (float)0.;            //in ?
  (*tcModelData).const_.cd[1] = (float)0.;            //in ?
  (*tcModelData).const_.cd[2] = (float)0.;            //in ?
  (*tcModelData).const_.cd[3] = (float)-2e-4;         //in ?
  (*tcModelData).const_.cd[4] = (float)-1.52e-4;      //in ?
  (*tcModelData).const_.cd[5] = (float)-2.33e-4;      //in ?
  (*tcModelData).const_.cd[6] = (float)-4.25e-4;      //in ?
  (*tcModelData).const_.cd[7] = (float)-1.5e-4;       //in ?

  (*tcModelData).const_.ah[0] = 1e-6;                 //in ?
  (*tcModelData).const_.ah[1] = 1e-6;                 //in ?
  (*tcModelData).const_.ah[2] = 1e-6;                 //in ?
  (*tcModelData).const_.ah[3] = 9.6e-7;               //in ?
  (*tcModelData).const_.ah[4] = 5.73e-7;              //in ?
  (*tcModelData).const_.ah[5] = 6.52e-7;              //in ?
  (*tcModelData).const_.ah[6] = 5.15e-7;              //in ?
  (*tcModelData).const_.ah[7] = 4.71e-7;              //in ?
  (*tcModelData).const_.bh[0] = (float)0.;            //in ?
  (*tcModelData).const_.bh[1] = (float)0.;            //in ?
  (*tcModelData).const_.bh[2] = (float)0.;            //in ?
  (*tcModelData).const_.bh[3] = 8.7e-9;               //in ?
  (*tcModelData).const_.bh[4] = 2.41e-9;              //in ?
  (*tcModelData).const_.bh[5] = 2.18e-9;              //in ?
  (*tcModelData).const_.bh[6] = 5.65e-10;             //in ?
  (*tcModelData).const_.bh[7] = 8.7e-10;              //in ?
  (*tcModelData).const_.ch[0] = (float)0.;            //in ?
  (*tcModelData).const_.ch[1] = (float)0.;            //in ?
  (*tcModelData).const_.ch[2] = (float)0.;            //in ?
  (*tcModelData).const_.ch[3] = 4.81e-11;             //in ?
  (*tcModelData).const_.ch[4] = 1.82e-11;             //in ?
  (*tcModelData).const_.ch[5] = 1.94e-11;             //in ?
  (*tcModelData).const_.ch[6] = 3.82e-12;             //in ?
  (*tcModelData).const_.ch[7] = 2.62e-12;             //in ?
  
  (*tcModelData).const_.av[0] = 1e-6;                 //in ?
  (*tcModelData).const_.av[1] = 1e-6;                 //in ?
  (*tcModelData).const_.av[2] = 1e-6;                 //in ?
  (*tcModelData).const_.av[3] = 8.67e-5;              //in ?
  (*tcModelData).const_.av[4] = 1.6e-4;               //in ?
  (*tcModelData).const_.av[5] = 1.6e-4;               //in ?
  (*tcModelData).const_.av[6] = 2.25e-4;              //in ?
  (*tcModelData).const_.av[7] = 2.09e-4;              //in ?
  (*tcModelData).const_.ag[0] = 3.411e-6;             //in ?
  (*tcModelData).const_.ag[1] = 3.799e-7;             //in ?
  (*tcModelData).const_.ag[2] = 2.491e-7;             //in ?
  (*tcModelData).const_.ag[3] = 3.567e-7;             //in ?
  (*tcModelData).const_.ag[4] = 3.463e-7;             //in ?
  (*tcModelData).const_.ag[5] = 3.93e-7;              //in ?
  (*tcModelData).const_.ag[6] = 1.7e-7;               //in ?
  (*tcModelData).const_.ag[7] = 1.5e-7;               //in ?
  (*tcModelData).const_.bg[0] = 7.18e-10;             //in ?
  (*tcModelData).const_.bg[1] = 1.08e-9;              //in ?
  (*tcModelData).const_.bg[2] = 1.36e-11;             //in ?
  (*tcModelData).const_.bg[3] = 8.51e-10;             //in ?
  (*tcModelData).const_.bg[4] = 8.96e-10;             //in ?
  (*tcModelData).const_.bg[5] = 1.02e-9;              //in ?
  (*tcModelData).const_.bg[6] = 0.;                   //in ?
  (*tcModelData).const_.bg[7] = 0.;                   //in ?
  (*tcModelData).const_.cg[0] = 6e-13;                //in ?
  (*tcModelData).const_.cg[1] = -3.98e-13;            //in ?
  (*tcModelData).const_.cg[2] = -3.93e-14;            //in ?
  (*tcModelData).const_.cg[3] = -3.12e-13;            //in ?
  (*tcModelData).const_.cg[4] = -3.27e-13;            //in ?
  (*tcModelData).const_.cg[5] = -3.12e-13;            //in ?
  (*tcModelData).const_.cg[6] = 0.;                   //in ?
  (*tcModelData).const_.cg[7] = 0.;                   //in ?
  
  /*Initial State of Process (Mode 1)*/
  yy[1] = (float) 11.9521761061849;//10.40491389;                         //in lb-mol
  yy[2] = (float) 7.95030022058702;//4.363996017;                         //in lb-mol
  yy[3] = (float) 4.86845372488200;//7.570059737;                         //in lb-mol
  yy[4] = (float) 0.272972868512175;//.4230042431;                         //in lb-mol
  yy[5] = (float) 18.1709579375667;//24.15513437;                         //in lb-mol
  yy[6] = (float) 6.07568576053507;// 2.942597645;                         //in lb-mol
  yy[7] = (float) 138.772474462396;//154.3770655;                         //in lb-mol
  yy[8] = (float) 136.128059744242;//159.186596;                          //in lb-mol
  yy[9] = (float) 2.52405024210640;// 2.808522723;                         //in ?
  yy[10] = (float)62.6271383951264;//63.75581199;                        //in lb-mol
  yy[11] = (float) 41.6563128383175;//26.74026066;                        //in lb-mol
  yy[12] = (float) 25.5223018884014;//46.38532432;                        //in lb-mol
  yy[13] = (float) 0.151685102518388;//.2464521543;                        //in lb-mol
  yy[14] = (float) 10.7461565441956;//15.20484404;                        //in lb-mol
  yy[15] = (float) 3.59337121663502;//1.852266172;                        //in lb-mol
  yy[16] = (float)52.2790822759519;// 52.44639459;                        //in lb-mol
  yy[17] = (float) 41.1175035154160;//41.20394008;                        //in lb-mol
  yy[18] = (float) 0.648822831791654;//.569931776;                         //in ?
  yy[19] = (float) 0.424789724873510;//.4306056376;                        //in ?
  yy[20] = 0.00788270636650422; //.0079906200783;                            //in ?
  yy[21] = (float)0.893372204888619; //.9056036089;                        //in ?
  yy[22] =  0.00960279385622587;//.016054258216;                             //in ?
  yy[23] = (float)0.515068583569485;//.7509759687;                        //in ?
  yy[24] = 0.166817153416254;//.088582855955;                             //in ?
  yy[25] = (float) 48.1831228961260;//48.27726193;                        //in ?
  yy[26] = (float) 39.4205245719891;//39.38459028;                        //in ?
  yy[27] = (float) 0.380153203870486;//.3755297257;                        //in ?
  yy[28] = (float) 113.684158463393;//107.7562698;                        //in ?
  yy[29] = (float) 52.6117303770829;//29.77250546;                        //in ?
  yy[30] = (float) 66.6613463158927;//88.32481135;                        //in ?
  yy[31] = (float) 21.2121653363644;//23.03929507;                        //in ?
  yy[32] = (float) 58.9419682906028;//62.85848794;                        //in ?
  yy[33] = (float) 14.3366591278114;//5.546318688;                        //in ?
  yy[34] = (float) 17.6490988806259;//11.92244772;                        //in ?
  yy[35] = (float) 8.51217326549827;//5.555448243;                        //in ?
  yy[36] = (float) 1.13798965779542;//.9218489762;                        //in ?
  /*Cooling Water Outlet Temperatures*/
  yy[37] = (float) 102.480028232595;//94.59927549;                        //in °C
  yy[38] = (float) 92.2625307053972;//77.29698353;                        //in °C
  /*Valve Positions*/
  yy[39] = (float) 62.8065594603242;//63.05263039;                        //in %
  yy[40] = (float) 53.2860269669116;//53.97970677;                        //in %
  yy[41] = (float) 26.6612540091053;//24.64355755;                        //in %
  yy[42] = (float) 60.4848010831412;//61.30192144;                        //in %
  yy[43] = (float) 4.40000000000000e-323;//22.21;                              //in %
  yy[44] = (float) 24.2346888157974;//40.06374673;                        //in %
  yy[45] = (float) 37.2091027669474;//38.1003437;                         //in %
  yy[46] = (float) 46.4308923440716;//46.53415582;                        //in %
  yy[47] = (float) 8.20000000000000e-322;//47.44573456;                        //in %
  yy[48] = (float) 35.9445883694097;//41.10581288;                        //in %
  yy[49] = (float) 12.2095478800146;//18.11349055;                        //in %
  yy[50] = (float) 99.9999999999999;//50.;                                //in %
  

  for (i__ = 1; i__ <= 12; ++i__){
    (*tcModelData).pv_.xmv[i__ - 1] = yy[i__ + 38];
    
    (*tcModelData).teproc_.vcv[i__ - 1] = (*tcModelData).pv_.xmv[i__ - 1];
    (*tcModelData).teproc_.vst[i__ - 1] = 2.;         //in %
    
    (*tcModelData).teproc_.ivst[i__ - 1] = (float)0.; //in 1
/* L200: */
  }

  /*Nominal Flowrate through Valves*/
  (*tcModelData).teproc_.vrng[0] = (float)400.;       //in lb-mol/h
  (*tcModelData).teproc_.vrng[1] = (float)400.;       //in lb-mol/h
  (*tcModelData).teproc_.vrng[2] = (float)100.;       //in lb-mol/h
  (*tcModelData).teproc_.vrng[3] = (float)1500.;      //in lb-mol/h
  (*tcModelData).teproc_.vrng[6] = (float)1500.;      //in lb-mol/h
  (*tcModelData).teproc_.vrng[7] = (float)1e3;        //in lb-mol/h
  (*tcModelData).teproc_.vrng[8] = (float).03;        //in ?
  (*tcModelData).teproc_.vrng[9] = (float)1e3;        //in gpm
  (*tcModelData).teproc_.vrng[10] = (float)1200.;     //in gpm
  
  /*Volumes of Vessels*/
  (*tcModelData).teproc_.vtr = (float)1300.;          //in ft³
  (*tcModelData).teproc_.vts = (float)3500.;          //in ft³
  (*tcModelData).teproc_.vtc = (float)156.5;          //in ft³  
  (*tcModelData).teproc_.vtv = (float)5e3;            //in ?
  
  (*tcModelData).teproc_.htr[0] = .06899381054;       //in ?
  (*tcModelData).teproc_.htr[1] = .05;                //in ?
  (*tcModelData).teproc_.hwr = (float)7060.;          //in ?
  (*tcModelData).teproc_.hws = (float)11138.;         //in ?
  (*tcModelData).teproc_.sfr[0] = (float).995;        //in 1
  (*tcModelData).teproc_.sfr[1] = (float).991;        //in 1
  (*tcModelData).teproc_.sfr[2] = (float).99;         //in 1
  (*tcModelData).teproc_.sfr[3] = (float).916;        //in 1
  (*tcModelData).teproc_.sfr[4] = (float).936;        //in 1
  (*tcModelData).teproc_.sfr[5] = (float).938;        //in 1
  (*tcModelData).teproc_.sfr[6] = .058;               //in 1
  (*tcModelData).teproc_.sfr[7] = .0301;              //in 1
  
  /*Stream 2*/
  (*tcModelData).teproc_.xst[0] = (float)0.;          //in mol%
  (*tcModelData).teproc_.xst[1] = (float)1e-4;        //in mol%
  (*tcModelData).teproc_.xst[2] = (float)0.;          //in mol%
  (*tcModelData).teproc_.xst[3] = (float).9999;       //in mol%
  (*tcModelData).teproc_.xst[4] = (float)0.;          //in mol%
  (*tcModelData).teproc_.xst[5] = (float)0.;          //in mol%
  (*tcModelData).teproc_.xst[6] = (float)0.;          //in mol%
  (*tcModelData).teproc_.xst[7] = (float)0.;          //in mol%
  (*tcModelData).teproc_.tst[0] = (float)45.;         //in °C
  
  /*Stream 3*/
  (*tcModelData).teproc_.xst[8] = (float)0.;          //in mol%
  (*tcModelData).teproc_.xst[9] = (float)0.;          //in mol%
  (*tcModelData).teproc_.xst[10] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[11] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[12] = (float).9999;      //in mol%
  (*tcModelData).teproc_.xst[13] = (float)1e-4;       //in mol%
  (*tcModelData).teproc_.xst[14] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[15] = (float)0.;         //in mol%
  (*tcModelData).teproc_.tst[1] = (float)45.;         //in °C

  /*Stream 1*/
  (*tcModelData).teproc_.xst[16] = (float).9999;      //in mol%
  (*tcModelData).teproc_.xst[17] = (float)1e-4;       //in mol%
  (*tcModelData).teproc_.xst[18] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[19] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[20] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[21] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[22] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[23] = (float)0.;         //in mol%
  (*tcModelData).teproc_.tst[2] = (float)45.;         //in °C

  /*Stream 4*/
  (*tcModelData).teproc_.xst[24] = (float).485;       //in mol%
  (*tcModelData).teproc_.xst[25] = (float).005;       //in mol%
  (*tcModelData).teproc_.xst[26] = (float).51;        //in mol%
  (*tcModelData).teproc_.xst[27] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[28] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[29] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[30] = (float)0.;         //in mol%
  (*tcModelData).teproc_.xst[31] = (float)0.;         //in mol%
  (*tcModelData).teproc_.tst[3] = (float)45.;         //in °C
  
  (*tcModelData).teproc_.cpflmx = (float)280275.;     //in ?
  (*tcModelData).teproc_.cpprmx = (float)1.3;         //in ?
  
  /*Time Constants of Valves*/
  (*tcModelData).teproc_.vtau[0] = (float)8.;         //in seconds
  (*tcModelData).teproc_.vtau[1] = (float)8.;         //in seconds
  (*tcModelData).teproc_.vtau[2] = (float)6.;         //in seconds
  (*tcModelData).teproc_.vtau[3] = (float)9.;         //in seconds
  (*tcModelData).teproc_.vtau[4] = (float)7.;         //in seconds
  (*tcModelData).teproc_.vtau[5] = (float)5.;         //in seconds
  (*tcModelData).teproc_.vtau[6] = (float)5.;         //in seconds
  (*tcModelData).teproc_.vtau[7] = (float)5.;         //in seconds
  (*tcModelData).teproc_.vtau[8] = (float)120.;       //in seconds
  (*tcModelData).teproc_.vtau[9] = (float)5.;         //in seconds
  (*tcModelData).teproc_.vtau[10] = (float)5.;        //in seconds
  (*tcModelData).teproc_.vtau[11] = (float)5.;        //in seconds 
  for (i__ = 1; i__ <= 12; ++i__){
    (*tcModelData).teproc_.vtau[i__ - 1] /= (float)3600.;
/* L300: */
  } //for (i__ = 1; i__ <= 12; ++i__){
  
  /*Seed of Random Generator*/
  if (rseed == NULL){
    (*tcModelData).randsd_.g = 1431655765.;
    (*tcModelData).randsd_.measnoise = 1431655765.;
    (*tcModelData).randsd_.procdist = 1431655765.;
  }else{
    (*tcModelData).randsd_.g = *rseed;
    (*tcModelData).randsd_.measnoise = *rseed;
    (*tcModelData).randsd_.procdist = *rseed;
  } //if (rseed == NULL){
    
  /*Amplitudes of Measurement Noise*/
  if (((*tcModelData).MSFlag & 0x10) > 0){
    for (i__ = 1; i__ <= 41; ++i__){
      (*tcModelData).teproc_.xns[i__ - 1] = 0.;       
    } //for (i__ = 1; i__ <= 41; ++i__){
    
    for (i__ = 1; i__ <= 34; ++i__){
      (*tcModelData).teproc_.xnsadd[i__ - 1] = 0.;    
    } //for (i__ = 1; i__ <= 13; ++i__){
  }else{
    (*tcModelData).teproc_.xns[0] = .0012;            //in kscmh
    (*tcModelData).teproc_.xns[1] = 18.;              //in kg/h
    (*tcModelData).teproc_.xns[2] = 22.;              //in kg/h
    (*tcModelData).teproc_.xns[3] = .05;              //in kscmh
    (*tcModelData).teproc_.xns[4] = .2;               //in kscmh
    (*tcModelData).teproc_.xns[5] = .21;              //in kscmh
    (*tcModelData).teproc_.xns[6] = .3;               //in kPa gauge
    (*tcModelData).teproc_.xns[7] = .5;               //in %
    (*tcModelData).teproc_.xns[8] = .01;              //in °C
    (*tcModelData).teproc_.xns[9] = .0017;            //in kscmh
    (*tcModelData).teproc_.xns[10] = .01;             //in °C
    (*tcModelData).teproc_.xns[11] = 1.;              //in %
    (*tcModelData).teproc_.xns[12] = .3;              //in kPa gauge
    (*tcModelData).teproc_.xns[13] = .125;            //in m³/h
    (*tcModelData).teproc_.xns[14] = 1.;              //in %
    (*tcModelData).teproc_.xns[15] = .3;              //in kPa gauge
    (*tcModelData).teproc_.xns[16] = .115;            //in m³/h
    (*tcModelData).teproc_.xns[17] = .01;             //in °C
    (*tcModelData).teproc_.xns[18] = 1.15;            //in kg/h
    (*tcModelData).teproc_.xns[19] = .2;              //in kW
    (*tcModelData).teproc_.xns[20] = .01;             //in °C
    (*tcModelData).teproc_.xns[21] = .01;             //in °C
    (*tcModelData).teproc_.xns[22] = .25;             //in mol %
    (*tcModelData).teproc_.xns[23] = .1;              //in mol %
    (*tcModelData).teproc_.xns[24] = .25;             //in mol %
    (*tcModelData).teproc_.xns[25] = .1;              //in mol %
    (*tcModelData).teproc_.xns[26] = .25;             //in mol %
    (*tcModelData).teproc_.xns[27] = .025;            //in mol %
    (*tcModelData).teproc_.xns[28] = .25;             //in mol %
    (*tcModelData).teproc_.xns[29] = .1;              //in mol %
    (*tcModelData).teproc_.xns[30] = .25;             //in mol %
    (*tcModelData).teproc_.xns[31] = .1;              //in mol %
    (*tcModelData).teproc_.xns[32] = .25;             //in mol %
    (*tcModelData).teproc_.xns[33] = .025;            //in mol %
    (*tcModelData).teproc_.xns[34] = .05;             //in mol %
    (*tcModelData).teproc_.xns[35] = .05;             //in mol %
    (*tcModelData).teproc_.xns[36] = .01;             //in mol %
    (*tcModelData).teproc_.xns[37] = .01;             //in mol %
    (*tcModelData).teproc_.xns[38] = .01;             //in mol %
    (*tcModelData).teproc_.xns[39] = .5;              //in mol %
    (*tcModelData).teproc_.xns[40] = .5;              //in mol %

    (*tcModelData).teproc_.xnsadd[0] = .01;           //in °C
    (*tcModelData).teproc_.xnsadd[1] = .01;           //in °C
    (*tcModelData).teproc_.xnsadd[2] = .01;           //in °C
    (*tcModelData).teproc_.xnsadd[3] = .01;           //in °C
    (*tcModelData).teproc_.xnsadd[4] = .01;           //in °C
    (*tcModelData).teproc_.xnsadd[5] = .125;          //in m³/h
    (*tcModelData).teproc_.xnsadd[6] = .01;           //in °C
    (*tcModelData).teproc_.xnsadd[7] = .125;          //in m³/h
    (*tcModelData).teproc_.xnsadd[8] = .01;           //in °C
    (*tcModelData).teproc_.xnsadd[9] = .01;           //in °C
    (*tcModelData).teproc_.xnsadd[10] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[11] = .1;           //in mol %
    (*tcModelData).teproc_.xnsadd[12] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[13] = .1;           //in mol %
    (*tcModelData).teproc_.xnsadd[14] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[15] = .025;         //in mol %
    (*tcModelData).teproc_.xnsadd[16] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[17] = .1;           //in mol %
    (*tcModelData).teproc_.xnsadd[18] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[19] = .1;           //in mol %
    (*tcModelData).teproc_.xnsadd[20] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[21] = .025;         //in mol %
    (*tcModelData).teproc_.xnsadd[22] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[23] = .1;           //in mol %
    (*tcModelData).teproc_.xnsadd[24] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[25] = .1;           //in mol %
    (*tcModelData).teproc_.xnsadd[26] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[27] = .025;         //in mol %
    (*tcModelData).teproc_.xnsadd[28] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[29] = .1;           //in mol %
    (*tcModelData).teproc_.xnsadd[30] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[31] = .1;           //in mol %
    (*tcModelData).teproc_.xnsadd[32] = .25;          //in mol %
    (*tcModelData).teproc_.xnsadd[33] = .025;         //in mol %
  } //if (((*tcModelData).MSFlag & 0x10) > 0){
  
  /*Initilization of Disturbance Flags*/
  for (i__ = 1; i__ <= 28; ++i__){
    (*tcModelData).dvec_.idv[i__ - 1] = (float)0.;    //in 1
/* L500: */
  } //for (i__ = 1; i__ <= 28; ++i__){
  
  /*Data of Disturbance Processes*/
  (*tcModelData).wlk_.hspan[0] = .2;                  //in h
  (*tcModelData).wlk_.hzero[0] = .5;                  //in h
  (*tcModelData).wlk_.sspan[0] = .03;                 //in mole-ratio
  (*tcModelData).wlk_.szero[0] = .485;                //in mole-ratio
  (*tcModelData).wlk_.spspan[0] = 0.;                 //in mole-ratio/h

  (*tcModelData).wlk_.hspan[1] = .7;                  //in h
  (*tcModelData).wlk_.hzero[1] = 1.;                  //in h
  (*tcModelData).wlk_.sspan[1] = .003;                //in mole-ratio
  (*tcModelData).wlk_.szero[1] = .005;                //in mole-ratio
  (*tcModelData).wlk_.spspan[1] = 0.;                 //in mole-ratio/h
  
  (*tcModelData).wlk_.hspan[2] = .25;                 //in h
  (*tcModelData).wlk_.hzero[2] = .5;                  //in h
  (*tcModelData).wlk_.sspan[2] = 10.;                 //in °C
  (*tcModelData).wlk_.szero[2] = 45.;                 //in °C
  (*tcModelData).wlk_.spspan[2] = 0.;                 //in °C/h
  
  (*tcModelData).wlk_.hspan[3] = .7;                  //in h
  (*tcModelData).wlk_.hzero[3] = 1.;                  //in h
  (*tcModelData).wlk_.sspan[3] = 10.;                 //in °C
  (*tcModelData).wlk_.szero[3] = 45.;                 //in °C
  (*tcModelData).wlk_.spspan[3] = 0.;                 //in °C/h
  
  (*tcModelData).wlk_.hspan[4] = .15;                 //in h
  (*tcModelData).wlk_.hzero[4] = .25;                 //in h
  (*tcModelData).wlk_.sspan[4] = 10.;                 //in °C
  (*tcModelData).wlk_.szero[4] = 35.;                 //in °C
  (*tcModelData).wlk_.spspan[4] = 0.;                 //in °C/h
  
  (*tcModelData).wlk_.hspan[5] = .15;                 //in h
  (*tcModelData).wlk_.hzero[5] = .25;                 //in h
  (*tcModelData).wlk_.sspan[5] = 10.;                 //in °C
  (*tcModelData).wlk_.szero[5] = 40.;                 //in °C
  (*tcModelData).wlk_.spspan[5] = 0.;                 //in °C/h
  
  (*tcModelData).wlk_.hspan[6] = 1.;                  //in h
  (*tcModelData).wlk_.hzero[6] = 2.;                  //in h
  (*tcModelData).wlk_.sspan[6] = .25;                 //in 1
  (*tcModelData).wlk_.szero[6] = 1.;                  //in 1
  (*tcModelData).wlk_.spspan[6] = 0.;                 //in 1/h
  
  (*tcModelData).wlk_.hspan[7] = 1.;                  //in h
  (*tcModelData).wlk_.hzero[7] = 2.;                  //in h
  (*tcModelData).wlk_.sspan[7] = .25;                 //in 1
  (*tcModelData).wlk_.szero[7] = 1.;                  //in 1
  (*tcModelData).wlk_.spspan[7] = 0.;                 //in 1/h
  
  (*tcModelData).wlk_.hspan[8] = .4;                  //in h
  (*tcModelData).wlk_.hzero[8] = .5;                  //in h
  (*tcModelData).wlk_.sspan[8] = .25;                 //in ?
  (*tcModelData).wlk_.szero[8] = 0.;                  //in ?
  (*tcModelData).wlk_.spspan[8] = 0.;                 //in ?/h
  
  (*tcModelData).wlk_.hspan[9] = 1.5;                 //in h
  (*tcModelData).wlk_.hzero[9] = 2.;                  //in h
  (*tcModelData).wlk_.sspan[9] = 0.;                  //in ?
  (*tcModelData).wlk_.szero[9] = 0.;                  //in ?
  (*tcModelData).wlk_.spspan[9] = 0.;                 //in ?/h
  
  (*tcModelData).wlk_.hspan[10] = 2.;                 //in h
  (*tcModelData).wlk_.hzero[10] = 3.;                 //in h
  (*tcModelData).wlk_.sspan[10] = 0.;                 //in ?
  (*tcModelData).wlk_.szero[10] = 0.;                 //in ?
  (*tcModelData).wlk_.spspan[10] = 0.;                //in ?/h
  
  (*tcModelData).wlk_.hspan[11] = 1.5;                //in h
  (*tcModelData).wlk_.hzero[11] = 2.;                 //in h
  (*tcModelData).wlk_.sspan[11] = 0.;                 //in ?
  (*tcModelData).wlk_.szero[11] = 0.;                 //in ?
  (*tcModelData).wlk_.spspan[11] = 0.;                //in ?/h
  
  (*tcModelData).wlk_.hspan[12] = .15;                //in h
  (*tcModelData).wlk_.hzero[12] = .25;                //in h
  (*tcModelData).wlk_.sspan[12] = 10.;                //in °C
  (*tcModelData).wlk_.szero[12] = 45.;                //in °C
  (*tcModelData).wlk_.spspan[12] = 0.;                //in °C/h

  (*tcModelData).wlk_.hspan[13] = .25;                //in h
  (*tcModelData).wlk_.hzero[13] = .5;                 //in h
  (*tcModelData).wlk_.sspan[13] = 10.;                //in °C
  (*tcModelData).wlk_.szero[13] = 45.;                //in °C
  (*tcModelData).wlk_.spspan[13] = 0.;                //in °C/h

  (*tcModelData).wlk_.hspan[14] = .15;                //in h
  (*tcModelData).wlk_.hzero[14] = .25;                //in h
  (*tcModelData).wlk_.sspan[14] = 5.;                 //in lb-mol/h
  (*tcModelData).wlk_.szero[14] = 100.;               //in lb-mol/h
  (*tcModelData).wlk_.spspan[14] = 0.;                //in lb-mol/h²

  (*tcModelData).wlk_.hspan[15] = .25;                //in h
  (*tcModelData).wlk_.hzero[15] = .5;                 //in h
  (*tcModelData).wlk_.sspan[15] = 20.;                //in lb-mol/h
  (*tcModelData).wlk_.szero[15] = 400.;               //in lb-mol/h
  (*tcModelData).wlk_.spspan[15] = 0.;                //in lb-mol/h²

  (*tcModelData).wlk_.hspan[16] = .25;                //in h
  (*tcModelData).wlk_.hzero[16] = .5;                 //in h
  (*tcModelData).wlk_.sspan[16] = 20.;                //in lb-mol/h
  (*tcModelData).wlk_.szero[16] = 400.;               //in lb-mol/h
  (*tcModelData).wlk_.spspan[16] = 0.;                //in lb-mol/h²

  (*tcModelData).wlk_.hspan[17] = .7;                 //in h
  (*tcModelData).wlk_.hzero[17] = 1.;                 //in h
  (*tcModelData).wlk_.sspan[17] = 75.;                //in lb-mol/h
  (*tcModelData).wlk_.szero[17] = 1500.;              //in lb-mol/h
  (*tcModelData).wlk_.spspan[17] = 0.;                //in lb-mol/h²

  (*tcModelData).wlk_.hspan[18] = .1;                 //in h
  (*tcModelData).wlk_.hzero[18] = .2;                 //in h
  (*tcModelData).wlk_.sspan[18] = 50.;                //in gpm
  (*tcModelData).wlk_.szero[18] = (float)1e3;         //in gpm
  (*tcModelData).wlk_.spspan[18] = 0.;                //in gpm/h

  (*tcModelData).wlk_.hspan[19] = .1;                 //in h
  (*tcModelData).wlk_.hzero[19] = .2;                 //in h
  (*tcModelData).wlk_.sspan[19] = 60.;                //in gpm
  (*tcModelData).wlk_.szero[19] = 1200.;              //in gpm
  (*tcModelData).wlk_.spspan[19] = 0.;                //in gpm/h
  
  /*Initilization of Disturbance Processes Parameters*/
  for (i__ = 1; i__ <= 20; ++i__){
    (*tcModelData).wlk_.tlast[i__ - 1] = 0.;          //in h
	(*tcModelData).wlk_.tnext[i__ - 1] = .1;          //in h
	(*tcModelData).wlk_.adist[i__ - 1] =              //unit depends on 
                  (*tcModelData).wlk_.szero[i__ - 1]; //value
	(*tcModelData).wlk_.bdist[i__ - 1] = 0.;          //in x/h
	(*tcModelData).wlk_.cdist[i__ - 1] = 0.;          //in x/h²
	(*tcModelData).wlk_.ddist[i__ - 1] = 0.;          //in x/h³
/* L550: */
  } //for (i__ = 1; i__ <= 12; ++i__){
  
  (*tcModelData).tlastcomp = -1.;  
  *time = (float)0.;
  tefunc(ModelData, nn, time, &yy[1], &yp[1], 0);

  #undef isd
  
  return 0;
} /* teinit */


/*-------------------------------------------------------------------------
-                               t e f u n c                               -
-------------------------------------------------------------------------*/
int tefunc(void *ModelData, 
                  const integer *nn, doublereal *time, doublereal *yy, 
                  doublereal *yp, shortint Callflag){
  /*----------------------------- Variables -----------------------------*/
  integer i__1;
  doublereal d__1;

  struct stModelData *tcModelData;
  int distende;
  int distindex[16];
  int distnum;
  int distindch;
  doublereal flms;
  doublereal xcmp[41];
  doublereal xcmpadd[24];
  doublereal hwlk;
  doublereal vpos[12];
  doublereal xmns;
  doublereal swlk;
  doublereal spwlk;
  doublereal vovrl;
  doublereal rg;
  doublereal flcoef;
  doublereal pr;
  doublereal tmpfac;
  doublereal uarlev;
  doublereal r1f;
  doublereal r2f;
  doublereal uac;
  doublereal fin[8];
  doublereal dlp;
  doublereal vpr;
  doublereal uas;
  doublereal prate;
  integer aux;
  integer i__;
  integer j__;
  #define isd ((doublereal *)&(*tcModelData).dvec_ + 28)
    
  /*Typcast of Dataset Pointer*/  
  tcModelData = ModelData;

  /*Parameter adjustments*/
  --yp;
  --yy;

  /*--------------------------- Function Body ---------------------------*/
  if((((*tcModelData).tlastcomp != *time) || (Callflag == 1)) &&
     (((*tcModelData).MSFlag & 0x8000) == 0)){
    /*Limiting of Disturbance Activations*/
    for (i__ = 1; i__ <= 28; ++i__){
      if ((*tcModelData).dvec_.idv[i__ - 1] < (float)0.){
  	    (*tcModelData).dvec_.idv[i__ - 1] = (float)0.;
	  } //if ((*tcModelData).dvec_.idv[i__ - 1] < 0){
      if ((*tcModelData).dvec_.idv[i__ - 1] > (float)1.){
	    (*tcModelData).dvec_.idv[i__ - 1] = (float)1.;
      } //if ((*tcModelData).dvec_.idv[i__ - 1] > 1){
      /* L500: */
    } //for (i__ = 1; i__ <= 20; ++i__){
    
    /*Assignment of Disturbance Activations*/
    (*tcModelData).wlk_.idvwlk[0]  = (*tcModelData).dvec_.idv[7];
    (*tcModelData).wlk_.idvwlk[1]  = (*tcModelData).dvec_.idv[7];
    (*tcModelData).wlk_.idvwlk[2]  = (*tcModelData).dvec_.idv[8];
    (*tcModelData).wlk_.idvwlk[3]  = (*tcModelData).dvec_.idv[9];
    (*tcModelData).wlk_.idvwlk[4]  = (*tcModelData).dvec_.idv[10];
    (*tcModelData).wlk_.idvwlk[5]  = (*tcModelData).dvec_.idv[11];
    (*tcModelData).wlk_.idvwlk[6]  = (*tcModelData).dvec_.idv[12];
    (*tcModelData).wlk_.idvwlk[7]  = (*tcModelData).dvec_.idv[12];
    (*tcModelData).wlk_.idvwlk[8]  = (*tcModelData).dvec_.idv[15];
    (*tcModelData).wlk_.idvwlk[9]  = (*tcModelData).dvec_.idv[16];
    (*tcModelData).wlk_.idvwlk[10] = (*tcModelData).dvec_.idv[17];
    (*tcModelData).wlk_.idvwlk[11] = (*tcModelData).dvec_.idv[19];
    (*tcModelData).wlk_.idvwlk[12] = (*tcModelData).dvec_.idv[20];
    (*tcModelData).wlk_.idvwlk[13] = (*tcModelData).dvec_.idv[21];
    (*tcModelData).wlk_.idvwlk[14] = (*tcModelData).dvec_.idv[22];
    (*tcModelData).wlk_.idvwlk[15] = (*tcModelData).dvec_.idv[23];
    (*tcModelData).wlk_.idvwlk[16] = (*tcModelData).dvec_.idv[24];
    (*tcModelData).wlk_.idvwlk[17] = (*tcModelData).dvec_.idv[25];
    (*tcModelData).wlk_.idvwlk[18] = (*tcModelData).dvec_.idv[26];
    (*tcModelData).wlk_.idvwlk[19] = (*tcModelData).dvec_.idv[27];
    
    if (((*tcModelData).MSFlag & 0x40) > 0){
      /*Recalculation of Disturbance Process Parameters - Determination of
        Processes to be Updated (1 - 9/13 - 20)*/
      distnum = 0;
      for (i__ = 1; i__ <= 20; ++i__){
        if (*time >= (*tcModelData).wlk_.tnext[i__ - 1]){
          distindex[distnum] = i__ - 1; 
          distnum++;
        } //if (*time >= (*tcModelData).wlk_.tnext[i__ - 1]){
        
        /*Step-over of 10 through 12*/
        if(i__ == 9){
          i__ = i__ + 3;
        }        
      } //for (i__ = 1; i__ <= 20; ++i__){ */
    
      /*Recalculation of Disturbance Process Parameters - Sorting (1 - 9/
        13 - 20)*/
      for(i__ = 1; i__ < distnum; ++i__){
        distindch = distindex[i__];
        j__ = i__;
        while((j__ > 0) && 
             ((*tcModelData).wlk_.tnext[j__ - 1] > 
                                    (*tcModelData).wlk_.tnext[distindch])){
          distindex[j__] = distindex[j__ - 1];
          j__--;
        } //while((j__ > 0) && ...
        distindex[j__] = distindch;
      } //for(i__ = 1; i__ < distnum; ++i__){
    
      /*Recalculation of Disturbance Process Parameters - Update (1 - 9/
        13 - 20)*/
      for(i__ = 0; i__ < distnum; ++i__){
  	    hwlk = (*tcModelData).wlk_.tnext[distindex[i__]] - 
                                 (*tcModelData).wlk_.tlast[distindex[i__]];
	    
        swlk = (*tcModelData).wlk_.adist[distindex[i__]] + 
               hwlk * ((*tcModelData).wlk_.bdist[distindex[i__]] + 
                   hwlk * ((*tcModelData).wlk_.cdist[distindex[i__]] + 
                       hwlk * (*tcModelData).wlk_.ddist[distindex[i__]]));
	    
        spwlk = 
           (*tcModelData).wlk_.bdist[distindex[i__]] + 
           hwlk * ((*tcModelData).wlk_.cdist[distindex[i__]] * 2. +
                    hwlk * 3. * (*tcModelData).wlk_.ddist[distindex[i__]]);
	    
        (*tcModelData).wlk_.tlast[distindex[i__]] = 
                                 (*tcModelData).wlk_.tnext[distindex[i__]];
	    
        tesub5_(ModelData, &swlk, &spwlk, 
                &(*tcModelData).wlk_.adist[distindex[i__]], 
                &(*tcModelData).wlk_.bdist[distindex[i__]], 
                &(*tcModelData).wlk_.cdist[distindex[i__]], 
                &(*tcModelData).wlk_.ddist[distindex[i__]], 
                &(*tcModelData).wlk_.tlast[distindex[i__]], 
   	            &(*tcModelData).wlk_.tnext[distindex[i__]], 
                &(*tcModelData).wlk_.hspan[distindex[i__]],
                &(*tcModelData).wlk_.hzero[distindex[i__]], 
                &(*tcModelData).wlk_.sspan[distindex[i__]], 
                &(*tcModelData).wlk_.szero[distindex[i__]], 
                &(*tcModelData).wlk_.spspan[distindex[i__]], 
                &(*tcModelData).wlk_.idvwlk[distindex[i__]]);
      } //for(i__ = 1; i__ <= distunm; ++i__){

      /*Recalculation of Disturbance Process Parameters - Determination of
        Processes to be Updated (10 - 12)*/
      distnum = 0;
      for (i__ = 10; i__ <= 12; ++i__){
        if (*time >= (*tcModelData).wlk_.tnext[i__ - 1]){
          distindex[distnum] = i__ - 1;
          distnum++;
        } //if (*time >= (*tcModelData).wlk_.tnext[i__ - 1]){
      } //for (i__ = 1; i__ <= 9; ++i__){ */
    
      /*Recalculation of Disturbance Process Parameters - Sorting (10 - 
        12)*/
      for(i__ = 1; i__ < distnum; ++i__){
        distindch = distindex[i__];
        j__ = i__;
        while((j__ > 0) && 
             ((*tcModelData).wlk_.tnext[j__ - 1] > 
                                    (*tcModelData).wlk_.tnext[distindch])){
          distindex[j__] = distindex[j__ - 1];
          j__--;
        } //while((j__ > 0) && ...
        distindex[j__] = distindch;
      } //for(i__ = 1; i__ < distnum; ++i__){
    
      /*Recalculation of Disturbance Process Parameters - Update (10 - 
        12)*/
      for(i__ = 0; i__ < distnum; ++i__){
        hwlk = (*tcModelData).wlk_.tnext[distindex[i__]] - 
                                 (*tcModelData).wlk_.tlast[distindex[i__]];
	    
        swlk = (*tcModelData).wlk_.adist[distindex[i__]] + 
               hwlk * ((*tcModelData).wlk_.bdist[distindex[i__]] + 
                   hwlk * ((*tcModelData).wlk_.cdist[distindex[i__]] + 
                       hwlk * (*tcModelData).wlk_.ddist[distindex[i__]]));
	    
        spwlk = 
           (*tcModelData).wlk_.bdist[distindex[i__]] + 
           hwlk * ((*tcModelData).wlk_.cdist[distindex[i__]] * 2. +
		  	        hwlk * 3. * (*tcModelData).wlk_.ddist[distindex[i__]]);
    
        (*tcModelData).wlk_.tlast[distindex[i__]] = 
                                 (*tcModelData).wlk_.tnext[distindex[i__]];   
        if (swlk > .1){
	      (*tcModelData).wlk_.adist[distindex[i__]] = swlk;
		  (*tcModelData).wlk_.bdist[distindex[i__]] = spwlk;
		  (*tcModelData).wlk_.cdist[distindex[i__]] = 
                                           -(swlk * 3. + spwlk * .2) / .01;
		  (*tcModelData).wlk_.ddist[distindex[i__]] = 
                                           (swlk * 2. + spwlk * .1) / .001;
		  (*tcModelData).wlk_.tnext[distindex[i__]] = 
                            (*tcModelData).wlk_.tlast[distindex[i__]] + .1;
	    }else{
          aux = -1;
		  hwlk = (*tcModelData).wlk_.hspan[distindex[i__]] * 
                           tesub7_(ModelData, &aux) + 
                           (*tcModelData).wlk_.hzero[distindex[i__]];
	      (*tcModelData).wlk_.adist[distindex[i__]] = swlk;
		  (*tcModelData).wlk_.bdist[distindex[i__]] = spwlk;

          /* Computing 2nd power */
		  d__1 = hwlk;
		  (*tcModelData).wlk_.cdist[distindex[i__]] = 
                ((*tcModelData).wlk_.idvwlk[distindex[i__]] - 
                                             2*spwlk*d__1) / (d__1 * d__1);
		  (*tcModelData).wlk_.ddist[distindex[i__]] = 
                                                     spwlk / (d__1 * d__1);
		  (*tcModelData).wlk_.tnext[distindex[i__]] = 
                          (*tcModelData).wlk_.tlast[distindex[i__]] + hwlk;
        } //if (swlk > .1){
      } //for(i__ = 0; i__ < distnum; ++i__){      
    }else{
      /* Original Code of J.J. Downs & E.F. Vogel and N.L. Ricker [2]*/
      /*Recalculation of Disturbance Process Parameters (1 - 9/13 - 20)*/
      if(((*tcModelData).MSFlag & 0x8000) > 0){
        distende = 9;
      }else{
        distende = 20;
      } //if(((*tcModelData).MSFlag & 0x8000) > 0){
      
      for (i__ = 1; i__ <= distende; ++i__){
        if (*time >= (*tcModelData).wlk_.tnext[i__ - 1]){
  	      hwlk = (*tcModelData).wlk_.tnext[i__ - 1] - 
                                        (*tcModelData).wlk_.tlast[i__ - 1];
	    
          swlk = (*tcModelData).wlk_.adist[i__ - 1] + 
                 hwlk * ((*tcModelData).wlk_.bdist[i__ - 1] + 
                         hwlk * ((*tcModelData).wlk_.cdist[i__ - 1] + 
                               hwlk * (*tcModelData).wlk_.ddist[i__ - 1]));
	    
          spwlk = (*tcModelData).wlk_.bdist[i__ - 1] + 
                  hwlk * ((*tcModelData).wlk_.cdist[i__ - 1] * 2. +
		                  hwlk * 3. * (*tcModelData).wlk_.ddist[i__ - 1]);
	    
          (*tcModelData).wlk_.tlast[i__ - 1] = 
                                        (*tcModelData).wlk_.tnext[i__ - 1];
	    
          tesub5_(ModelData, &swlk, &spwlk, 
                  &(*tcModelData).wlk_.adist[i__ - 1], 
                  &(*tcModelData).wlk_.bdist[i__ - 1], 
                  &(*tcModelData).wlk_.cdist[i__ - 1], 
                  &(*tcModelData).wlk_.ddist[i__ - 1], 
                  &(*tcModelData).wlk_.tlast[i__ - 1], 
  			      &(*tcModelData).wlk_.tnext[i__ - 1], 
                  &(*tcModelData).wlk_.hspan[i__ - 1],
                  &(*tcModelData).wlk_.hzero[i__ - 1], 
                  &(*tcModelData).wlk_.sspan[i__ - 1], 
                  &(*tcModelData).wlk_.szero[i__ - 1], 
                  &(*tcModelData).wlk_.spspan[i__ - 1], 
                  &(*tcModelData).wlk_.idvwlk[i__ - 1]);
        } //if (*time >= (*tcModelData).wlk_.tnext[i__ - 1]){

        /*Step-over of 10 through 12*/
        if(i__ == 9){
          i__ = i__ + 3;
        }        
      /* L900: */
      } //for (i__ = 1; i__ <= 20; ++i__){ */
       
      /*Recalculation of Disturbance Process Parameters (10 - 12)*/
      for (i__ = 10; i__ <= 12; ++i__){
        if (*time >= (*tcModelData).wlk_.tnext[i__ - 1]){
          hwlk = (*tcModelData).wlk_.tnext[i__ - 1] - 
                                        (*tcModelData).wlk_.tlast[i__ - 1];
	    
          swlk = (*tcModelData).wlk_.adist[i__ - 1] + 
                 hwlk * ((*tcModelData).wlk_.bdist[i__ - 1] + 
                        hwlk * ((*tcModelData).wlk_.cdist[i__ - 1] + 
                               hwlk * (*tcModelData).wlk_.ddist[i__ - 1]));
	    
          spwlk = (*tcModelData).wlk_.bdist[i__ - 1] + 
                  hwlk * ((*tcModelData).wlk_.cdist[i__ - 1] * 2. +
	     	  	          hwlk * 3. * (*tcModelData).wlk_.ddist[i__ - 1]);
	    
          (*tcModelData).wlk_.tlast[i__ - 1] = 
                                        (*tcModelData).wlk_.tnext[i__ - 1];   
          if (swlk > .1){
   	        (*tcModelData).wlk_.adist[i__ - 1] = swlk;
		    (*tcModelData).wlk_.bdist[i__ - 1] = spwlk;
		    (*tcModelData).wlk_.cdist[i__ - 1] = 
                                           -(swlk * 3. + spwlk * .2) / .01;
		    (*tcModelData).wlk_.ddist[i__ - 1] = 
                                           (swlk * 2. + spwlk * .1) / .001;
		    (*tcModelData).wlk_.tnext[i__ - 1] = 
                                   (*tcModelData).wlk_.tlast[i__ - 1] + .1;
	      }else{
            aux = -1;
		    hwlk = (*tcModelData).wlk_.hspan[i__ - 1] * 
                   tesub7_(ModelData, &aux) + 
                                        (*tcModelData).wlk_.hzero[i__ - 1];
   	        (*tcModelData).wlk_.adist[i__ - 1] = swlk;
		    (*tcModelData).wlk_.bdist[i__ - 1] = spwlk;

            /* Computing 2nd power */
		    d__1 = hwlk;
		    (*tcModelData).wlk_.cdist[i__ - 1] = 
                       ((*tcModelData).wlk_.idvwlk[i__ - 1] -
                                             2*spwlk*d__1) / (d__1 * d__1);
		    (*tcModelData).wlk_.ddist[i__ - 1] = spwlk / (d__1 * d__1);
		    (*tcModelData).wlk_.tnext[i__ - 1] = 
                                 (*tcModelData).wlk_.tlast[i__ - 1] + hwlk;
          } //if (swlk > .1){
	    } //if (*time >= (*tcModelData).wlk_.tnext[i__ - 1]){
      /* L910: */
      } //for (i__ = 10; i__ <= 12; ++i__){
    } //if (((*tcModelData).MSFlag & 0x40) > 0){

  /*Initilization of Disturbance Processes Parameters*/
    if (*time == 0.){
	  for (i__ = 1; i__ <= 20; ++i__){
	    (*tcModelData).wlk_.adist[i__ - 1] =  
                                        (*tcModelData).wlk_.szero[i__ - 1];
	    (*tcModelData).wlk_.bdist[i__ - 1] = 0.;
	    (*tcModelData).wlk_.cdist[i__ - 1] = 0.;
	    (*tcModelData).wlk_.ddist[i__ - 1] = 0.;
	    (*tcModelData).wlk_.tlast[i__ - 1] = 0.;
	    (*tcModelData).wlk_.tnext[i__ - 1] = .1;
      /* L950: */
      } //for (i__ = 1; i__ <= 20; ++i__){
    } //if (*time == 0.){
   
    /*Determination of Disturbed Values*/
    (*tcModelData).teproc_.xst[24] = tesub8_(ModelData, &c__1, time) - 
                                   (*tcModelData).dvec_.idv[0] * .03 - 
                                   (*tcModelData).dvec_.idv[1] * .00243719;
    (*tcModelData).teproc_.xst[25] = tesub8_(ModelData, &c__2, time) + 
                                   (*tcModelData).dvec_.idv[1] * .005;
    (*tcModelData).teproc_.xst[26] = 1. - (*tcModelData).teproc_.xst[24] - 
                                   (*tcModelData).teproc_.xst[25];
    (*tcModelData).teproc_.tst[0]  = tesub8_(ModelData, &c__3, time) + 
                                   (*tcModelData).dvec_.idv[2] * 5.;
    (*tcModelData).teproc_.tst[3]  = tesub8_(ModelData, &c__4, time);
    (*tcModelData).teproc_.tcwr    = tesub8_(ModelData, &c__5, time) + 
                                   (*tcModelData).dvec_.idv[3] * 5.;
    (*tcModelData).teproc_.tcws    = tesub8_(ModelData, &c__6, time) + 
                                   (*tcModelData).dvec_.idv[4] * 5.;
    r1f = tesub8_(ModelData, &c__7, time);   
    r2f = tesub8_(ModelData, &c__8, time);
    (*tcModelData).teproc_.tst[2]   = tesub8_(ModelData, &c__13, time);
    (*tcModelData).teproc_.tst[1]   = tesub8_(ModelData, &c__14, time);
    (*tcModelData).teproc_.vrng[2]  = tesub8_(ModelData, &c__15, time);
    (*tcModelData).teproc_.vrng[0]  = tesub8_(ModelData, &c__16, time);
    (*tcModelData).teproc_.vrng[1]  = tesub8_(ModelData, &c__17, time);
    (*tcModelData).teproc_.vrng[3]  = tesub8_(ModelData, &c__18, time);
    (*tcModelData).teproc_.vrng[9]  = tesub8_(ModelData, &c__19, time);
    (*tcModelData).teproc_.vrng[10] = tesub8_(ModelData, &c__20, time);

    /*Setting of Disturbance Outputs*/
    (*tcModelData).pv_.xmeasdist[0]  = (*tcModelData).teproc_.xst[24]*100;
    (*tcModelData).pv_.xmeasdist[1]  = (*tcModelData).teproc_.xst[25]*100;
    (*tcModelData).pv_.xmeasdist[2]  = (*tcModelData).teproc_.xst[26]*100;
    (*tcModelData).pv_.xmeasdist[3]  = (*tcModelData).teproc_.tst[0];
    (*tcModelData).pv_.xmeasdist[4]  = (*tcModelData).teproc_.tst[3];
    (*tcModelData).pv_.xmeasdist[5]  = (*tcModelData).teproc_.tcwr;
    (*tcModelData).pv_.xmeasdist[6]  = (*tcModelData).teproc_.tcws;   
    (*tcModelData).pv_.xmeasdist[7]  = r1f;
    (*tcModelData).pv_.xmeasdist[8]  = r2f;
    (*tcModelData).pv_.xmeasdist[9]  = tesub8_(ModelData, &c__9, time);
    (*tcModelData).pv_.xmeasdist[10] = tesub8_(ModelData, &c__10, time);
    (*tcModelData).pv_.xmeasdist[11] = tesub8_(ModelData, &c__11, time);
    (*tcModelData).pv_.xmeasdist[12] = tesub8_(ModelData, &c__12, time);
    (*tcModelData).pv_.xmeasdist[13] = (*tcModelData).teproc_.tst[2];
    (*tcModelData).pv_.xmeasdist[14] = (*tcModelData).teproc_.tst[1];
    (*tcModelData).pv_.xmeasdist[15] = (*tcModelData).teproc_.vrng[2]*
                                       (float).454;
    (*tcModelData).pv_.xmeasdist[16] = (*tcModelData).teproc_.vrng[0]*
                                       (float).454;
    (*tcModelData).pv_.xmeasdist[17] = (*tcModelData).teproc_.vrng[1]*
                                       (float).454;
    (*tcModelData).pv_.xmeasdist[18] = (*tcModelData).teproc_.vrng[3]*
                                       (float).454;
    (*tcModelData).pv_.xmeasdist[19] = (*tcModelData).teproc_.vrng[9]*
                                        (float)0.003785411784 * (float)60.;
    (*tcModelData).pv_.xmeasdist[20] = (*tcModelData).teproc_.vrng[10]*
                                        (float)0.003785411784 * (float)60.;
        
    /*Retrieving of Current States*/
    for (i__ = 1; i__ <= 3; ++i__){
      (*tcModelData).teproc_.ucvr[i__ - 1] = yy[i__];
  	  (*tcModelData).teproc_.ucvs[i__ - 1] = yy[i__ + 9];
  	  (*tcModelData).teproc_.uclr[i__ - 1] = (float)0.;
	  (*tcModelData).teproc_.ucls[i__ - 1] = (float)0.;
    /* L1010: */
    } //for (i__ = 1; i__ <= 3; ++i__){
    
    for (i__ = 4; i__ <= 8; ++i__){
	  (*tcModelData).teproc_.uclr[i__ - 1] = yy[i__];
	  (*tcModelData).teproc_.ucls[i__ - 1] = yy[i__ + 9];
    /* L1020: */
    } //for (i__ = 4; i__ <= 8; ++i__){
    
    for (i__ = 1; i__ <= 8; ++i__){
	  (*tcModelData).teproc_.uclc[i__ - 1] = yy[i__ + 18];
	  (*tcModelData).teproc_.ucvv[i__ - 1] = yy[i__ + 27];
    /* L1030: */
    } //for (i__ = 1; i__ <= 8; ++i__){
    (*tcModelData).teproc_.etr = yy[9];
    (*tcModelData).teproc_.ets = yy[18];
    (*tcModelData).teproc_.etc = yy[27];
    (*tcModelData).teproc_.etv = yy[36];
    (*tcModelData).teproc_.twr = yy[37];
    (*tcModelData).teproc_.tws = yy[38];
    for (i__ = 1; i__ <= 12; ++i__){
	  vpos[i__ - 1] = yy[i__ + 38];
    /* L1035: */
    } //for (i__ = 1; i__ <= 12; ++i__){
  
    /*Calculation of Collective Holdup*/
    (*tcModelData).teproc_.utlr = (float)0.;
    (*tcModelData).teproc_.utls = (float)0.;
    (*tcModelData).teproc_.utlc = (float)0.;
    (*tcModelData).teproc_.utvv = (float)0.;
    for (i__ = 1; i__ <= 8; ++i__){
      (*tcModelData).teproc_.utlr += (*tcModelData).teproc_.uclr[i__ - 1];
	  (*tcModelData).teproc_.utls += (*tcModelData).teproc_.ucls[i__ - 1];
	  (*tcModelData).teproc_.utlc += (*tcModelData).teproc_.uclc[i__ - 1];
	  (*tcModelData).teproc_.utvv += (*tcModelData).teproc_.ucvv[i__ - 1];
    /* L1040: */
    } //for (i__ = 1; i__ <= 8; ++i__){

    /*Calculation of Component Concentration*/
    for (i__ = 1; i__ <= 8; ++i__){
	  (*tcModelData).teproc_.xlr[i__ - 1] = 
        (*tcModelData).teproc_.uclr[i__ - 1] / (*tcModelData).teproc_.utlr;
  	  (*tcModelData).teproc_.xls[i__ - 1] = 
        (*tcModelData).teproc_.ucls[i__ - 1] / (*tcModelData).teproc_.utls;
	  (*tcModelData).teproc_.xlc[i__ - 1] = 
        (*tcModelData).teproc_.uclc[i__ - 1] / (*tcModelData).teproc_.utlc;
	  (*tcModelData).teproc_.xvv[i__ - 1] = 
        (*tcModelData).teproc_.ucvv[i__ - 1] / (*tcModelData).teproc_.utvv;
    /* L1050: */
    } //for (i__ = 1; i__ <= 8; ++i__){

    /*Calculation of Specific Internal Energy*/
    (*tcModelData).teproc_.esr = 
                  (*tcModelData).teproc_.etr / (*tcModelData).teproc_.utlr;
    (*tcModelData).teproc_.ess = 
                  (*tcModelData).teproc_.ets / (*tcModelData).teproc_.utls;
    (*tcModelData).teproc_.esc = 
                  (*tcModelData).teproc_.etc / (*tcModelData).teproc_.utlc;
    (*tcModelData).teproc_.esv = 
                  (*tcModelData).teproc_.etv / (*tcModelData).teproc_.utvv; 
  
    /*Calculation of Temperatures*/
    tesub2_(ModelData, (*tcModelData).teproc_.xlr, 
            &(*tcModelData).teproc_.tcr, &(*tcModelData).teproc_.esr, 
            &c__0);
    (*tcModelData).teproc_.tkr = (*tcModelData).teproc_.tcr + 
                                 (float)273.15;
  
    tesub2_(ModelData, (*tcModelData).teproc_.xls, 
            &(*tcModelData).teproc_.tcs, &(*tcModelData).teproc_.ess, 
            &c__0);
    (*tcModelData).teproc_.tks = (*tcModelData).teproc_.tcs + 
                                 (float)273.15;
  
    tesub2_(ModelData, (*tcModelData).teproc_.xlc, 
            &(*tcModelData).teproc_.tcc, &(*tcModelData).teproc_.esc, 
            &c__0);
  
    tesub2_(ModelData, (*tcModelData).teproc_.xvv, 
            &(*tcModelData).teproc_.tcv, &(*tcModelData).teproc_.esv, 
            &c__2);
    (*tcModelData).teproc_.tkv = (*tcModelData).teproc_.tcv + 
                                 (float)273.15;

    /*Calculation of Densities*/
    tesub4_(ModelData, (*tcModelData).teproc_.xlr, 
            &(*tcModelData).teproc_.tcr, &(*tcModelData).teproc_.dlr);
    tesub4_(ModelData, (*tcModelData).teproc_.xls, 
            &(*tcModelData).teproc_.tcs, &(*tcModelData).teproc_.dls);
    tesub4_(ModelData, (*tcModelData).teproc_.xlc, 
            &(*tcModelData).teproc_.tcc, &(*tcModelData).teproc_.dlc);

    /*Calculation of Volume of Liquid and Vapor Phase*/
    (*tcModelData).teproc_.vlr =
                  (*tcModelData).teproc_.utlr / (*tcModelData).teproc_.dlr;
    (*tcModelData).teproc_.vls = 
                  (*tcModelData).teproc_.utls / (*tcModelData).teproc_.dls;
    (*tcModelData).teproc_.vlc = 
                  (*tcModelData).teproc_.utlc / (*tcModelData).teproc_.dlc;
    (*tcModelData).teproc_.vvr = 
                   (*tcModelData).teproc_.vtr - (*tcModelData).teproc_.vlr;
    (*tcModelData).teproc_.vvs = 
                   (*tcModelData).teproc_.vts - (*tcModelData).teproc_.vls;

    /*Calculation of Pressure*/
    (*tcModelData).teproc_.ptr = (float)0.;
    (*tcModelData).teproc_.pts = (float)0.;

    rg = (float)998.9;
    for (i__ = 1; i__ <= 3; ++i__){
	  (*tcModelData).teproc_.ppr[i__ - 1] = 
                  (*tcModelData).teproc_.ucvr[i__ - 1] * rg * 
                  (*tcModelData).teproc_.tkr / (*tcModelData).teproc_.vvr;
	  (*tcModelData).teproc_.ptr += (*tcModelData).teproc_.ppr[i__ - 1];

      (*tcModelData).teproc_.pps[i__ - 1] = 
                  (*tcModelData).teproc_.ucvs[i__ - 1] * rg * 
                  (*tcModelData).teproc_.tks / (*tcModelData).teproc_.vvs;
	  (*tcModelData).teproc_.pts += (*tcModelData).teproc_.pps[i__ - 1];
    /* L1110: */
    } //for (i__ = 1; i__ <= 3; ++i__){
    
    for (i__ = 4; i__ <= 8; ++i__){
	  vpr = exp((*tcModelData).const_.avp[i__ - 1] + 
                (*tcModelData).const_.bvp[i__ - 1] / 
                                     ((*tcModelData).teproc_.tcr + 
                                      (*tcModelData).const_.cvp[i__ - 1]));
	  (*tcModelData).teproc_.ppr[i__ - 1] = 
                                 vpr * (*tcModelData).teproc_.xlr[i__ - 1];
	  (*tcModelData).teproc_.ptr += (*tcModelData).teproc_.ppr[i__ - 1];

	  vpr = exp((*tcModelData).const_.avp[i__ - 1] + 
                (*tcModelData).const_.bvp[i__ - 1] / 
                                     ((*tcModelData).teproc_.tcs +
		                              (*tcModelData).const_.cvp[i__ - 1]));
	  (*tcModelData).teproc_.pps[i__ - 1] = 
                                 vpr * (*tcModelData).teproc_.xls[i__ - 1];
	  (*tcModelData).teproc_.pts += (*tcModelData).teproc_.pps[i__ - 1];
    /* L1120: */
    } //for (i__ = 4; i__ <= 8; ++i__){

    (*tcModelData).teproc_.ptv = (*tcModelData).teproc_.utvv * rg * 
                  (*tcModelData).teproc_.tkv / (*tcModelData).teproc_.vtv;
    
    /*Calculation of Component Concentration in Vapor Phase (Reactor and
      Separator)*/
    for (i__ = 1; i__ <= 8; ++i__){
	  (*tcModelData).teproc_.xvr[i__ - 1] = 
          (*tcModelData).teproc_.ppr[i__ - 1] / (*tcModelData).teproc_.ptr;
	  (*tcModelData).teproc_.xvs[i__ - 1] = 
          (*tcModelData).teproc_.pps[i__ - 1] / (*tcModelData).teproc_.pts;
    /* L1130: */
    } //for (i__ = 1; i__ <= 8; ++i__){

    /*Calculation of Collective Holdup of Components in Vapor Phase (Reac-
      tor and Separator)*/
    (*tcModelData).teproc_.utvr = (*tcModelData).teproc_.ptr * 
              (*tcModelData).teproc_.vvr / rg / (*tcModelData).teproc_.tkr;
    (*tcModelData).teproc_.utvs = (*tcModelData).teproc_.pts * 
             (*tcModelData).teproc_.vvs / rg / (*tcModelData).teproc_.tks;
    
    /*Calculation of Single Holdup of Components in Vapor Phase (Reactor 
      and Separator)*/
    for (i__ = 4; i__ <= 8; ++i__){
	  (*tcModelData).teproc_.ucvr[i__ - 1] = 
         (*tcModelData).teproc_.utvr * (*tcModelData).teproc_.xvr[i__ - 1];
  	  (*tcModelData).teproc_.ucvs[i__ - 1] = 
         (*tcModelData).teproc_.utvs * (*tcModelData).teproc_.xvs[i__ - 1];
    /* L1140: */
    } //for (i__ = 4; i__ <= 8; ++i__){

    
    /*Reaction kinetics*/
    (*tcModelData).teproc_.rr[0] = exp((float)31.5859536 - 
              (float)20130.85052843482 / (*tcModelData).teproc_.tkr) * r1f;
    (*tcModelData).teproc_.rr[1] = exp((float)3.00094014 - 
	          (float)10065.42526421741 / (*tcModelData).teproc_.tkr) * r2f;
    (*tcModelData).teproc_.rr[2] = exp((float)53.4060443 - 
	          (float)30196.27579265224 / (*tcModelData).teproc_.tkr);
    (*tcModelData).teproc_.rr[3] = (*tcModelData).teproc_.rr[2] * 
                                                                .767488334;
  
    if ((*tcModelData).teproc_.ppr[0] > (float)0. && 
                                (*tcModelData).teproc_.ppr[2] > (float)0.){
	  r1f = pow_dd((*tcModelData).teproc_.ppr, &c_b73);
	  r2f = pow_dd(&(*tcModelData).teproc_.ppr[2], &c_b74);

      (*tcModelData).teproc_.rr[0] = (*tcModelData).teproc_.rr[0] * 
                                 r1f * r2f * (*tcModelData).teproc_.ppr[3];
  	  (*tcModelData).teproc_.rr[1] = (*tcModelData).teproc_.rr[1] * 
                                 r1f * r2f * (*tcModelData).teproc_.ppr[4];
    }else{
	  (*tcModelData).teproc_.rr[0] = (float)0.;
	  (*tcModelData).teproc_.rr[1] = (float)0.;
    } //if ((*tcModelData).teproc_.ppr[0] > (float)0. && ... 
    (*tcModelData).teproc_.rr[2] = (*tcModelData).teproc_.rr[2] * 
             (*tcModelData).teproc_.ppr[0] * (*tcModelData).teproc_.ppr[4];
    (*tcModelData).teproc_.rr[3] = (*tcModelData).teproc_.rr[3] * 
             (*tcModelData).teproc_.ppr[0] * (*tcModelData).teproc_.ppr[3];
  
    for (i__ = 1; i__ <= 4; ++i__){
	  (*tcModelData).teproc_.rr[i__ - 1] *= (*tcModelData).teproc_.vvr;
    /* L1200: */
    } //for (i__ = 1; i__ <= 4; ++i__){
    
    /*Consumption and Creation of Components in Reactor*/
    (*tcModelData).teproc_.crxr[0] = -(*tcModelData).teproc_.rr[0] - 
               (*tcModelData).teproc_.rr[1] - (*tcModelData).teproc_.rr[2];
    (*tcModelData).teproc_.crxr[2] = -(*tcModelData).teproc_.rr[0] - 
               (*tcModelData).teproc_.rr[1];
    (*tcModelData).teproc_.crxr[3] = -(*tcModelData).teproc_.rr[0] - 
               (*tcModelData).teproc_.rr[3] * 1.5;
    (*tcModelData).teproc_.crxr[4] = -(*tcModelData).teproc_.rr[1] - 
               (*tcModelData).teproc_.rr[2];
    (*tcModelData).teproc_.crxr[5] = (*tcModelData).teproc_.rr[2] + 
               (*tcModelData).teproc_.rr[3];
    (*tcModelData).teproc_.crxr[6] = (*tcModelData).teproc_.rr[0];
    (*tcModelData).teproc_.crxr[7] = (*tcModelData).teproc_.rr[1];
    (*tcModelData).teproc_.rh = 
            (*tcModelData).teproc_.rr[0] * (*tcModelData).teproc_.htr[0] + 
            (*tcModelData).teproc_.rr[1] * (*tcModelData).teproc_.htr[1];


    (*tcModelData).teproc_.xmws[0] = (float)0.;
    (*tcModelData).teproc_.xmws[1] = (float)0.;
    (*tcModelData).teproc_.xmws[5] = (float)0.;
    (*tcModelData).teproc_.xmws[7] = (float)0.;
    (*tcModelData).teproc_.xmws[8] = (float)0.;
    (*tcModelData).teproc_.xmws[9] = (float)0.;
    for (i__ = 1; i__ <= 8; ++i__){
	  (*tcModelData).teproc_.xst[i__ + 39] = 
                                       (*tcModelData).teproc_.xvv[i__ - 1];
	  (*tcModelData).teproc_.xst[i__ + 55] = 
                                       (*tcModelData).teproc_.xvr[i__ - 1];
	  (*tcModelData).teproc_.xst[i__ + 63] = 
                                       (*tcModelData).teproc_.xvs[i__ - 1];
	  (*tcModelData).teproc_.xst[i__ + 71] = 
                                       (*tcModelData).teproc_.xvs[i__ - 1];
	  (*tcModelData).teproc_.xst[i__ + 79] =
                                       (*tcModelData).teproc_.xls[i__ - 1];
	  (*tcModelData).teproc_.xst[i__ + 95] =
                                       (*tcModelData).teproc_.xlc[i__ - 1];
	
	  (*tcModelData).teproc_.xmws[0] += 
                                    (*tcModelData).teproc_.xst[i__ - 1] * 
                                    (*tcModelData).const_.xmw[i__ - 1];
	  (*tcModelData).teproc_.xmws[1] += 
                                    (*tcModelData).teproc_.xst[i__ + 7] * 
                                    (*tcModelData).const_.xmw[i__ - 1];
	  (*tcModelData).teproc_.xmws[5] += 
                                    (*tcModelData).teproc_.xst[i__ + 39] * 
                                    (*tcModelData).const_.xmw[i__ - 1];
	  (*tcModelData).teproc_.xmws[7] += 
                                    (*tcModelData).teproc_.xst[i__ + 55] * 
                                    (*tcModelData).const_.xmw[i__ - 1];
	  (*tcModelData).teproc_.xmws[8] += 
                                    (*tcModelData).teproc_.xst[i__ + 63] * 
                                    (*tcModelData).const_.xmw[i__ - 1];
	  (*tcModelData).teproc_.xmws[9] += 
                                    (*tcModelData).teproc_.xst[i__ + 71] * 
                                    (*tcModelData).const_.xmw[i__ - 1];
    /* L2010: */
    } //for (i__ = 1; i__ <= 8; ++i__){

    (*tcModelData).teproc_.tst[5] = (*tcModelData).teproc_.tcv;
    (*tcModelData).teproc_.tst[7] = (*tcModelData).teproc_.tcr;
    (*tcModelData).teproc_.tst[8] = (*tcModelData).teproc_.tcs;
    (*tcModelData).teproc_.tst[9] = (*tcModelData).teproc_.tcs;
    (*tcModelData).teproc_.tst[10] = (*tcModelData).teproc_.tcs;
    (*tcModelData).teproc_.tst[12] = (*tcModelData).teproc_.tcc;
    tesub1_(ModelData, (*tcModelData).teproc_.xst, 
            (*tcModelData).teproc_.tst, (*tcModelData).teproc_.hst, &c__1);
    tesub1_(ModelData, &(*tcModelData).teproc_.xst[8], 
            &(*tcModelData).teproc_.tst[1], &(*tcModelData).teproc_.hst[1], 
            &c__1);
    tesub1_(ModelData, &(*tcModelData).teproc_.xst[16], 
            &(*tcModelData).teproc_.tst[2], &(*tcModelData).teproc_.hst[2], 
            &c__1);
    tesub1_(ModelData, &(*tcModelData).teproc_.xst[24], 
            &(*tcModelData).teproc_.tst[3], &(*tcModelData).teproc_.hst[3],
            &c__1);
    tesub1_(ModelData, &(*tcModelData).teproc_.xst[40], 
            &(*tcModelData).teproc_.tst[5], &(*tcModelData).teproc_.hst[5],
            &c__1);
    tesub1_(ModelData, &(*tcModelData).teproc_.xst[56], 
            &(*tcModelData).teproc_.tst[7], &(*tcModelData).teproc_.hst[7], 
           &c__1);
    tesub1_(ModelData, &(*tcModelData).teproc_.xst[64], 
            &(*tcModelData).teproc_.tst[8], &(*tcModelData).teproc_.hst[8], 
            &c__1);
    (*tcModelData).teproc_.hst[9] = (*tcModelData).teproc_.hst[8];
    tesub1_(ModelData, &(*tcModelData).teproc_.xst[80], 
            &(*tcModelData).teproc_.tst[10], &(*tcModelData).teproc_.hst[10], 
            &c__0);
    tesub1_(ModelData, &(*tcModelData).teproc_.xst[96], 
            &(*tcModelData).teproc_.tst[12], &(*tcModelData).teproc_.hst[12], 
            &c__0);
    (*tcModelData).teproc_.ftm[0] = vpos[0] * 
                            (*tcModelData).teproc_.vrng[0] / (float)100.;
    (*tcModelData).teproc_.ftm[1] = vpos[1] * 
                            (*tcModelData).teproc_.vrng[1] / (float)100.;
    (*tcModelData).teproc_.ftm[2] = vpos[2] *
                            (1. - (*tcModelData).dvec_.idv[5]) * 
                            (*tcModelData).teproc_.vrng[2] / (float)100.;
    (*tcModelData).teproc_.ftm[3] = vpos[3] * 
                            (1. - (*tcModelData).dvec_.idv[6] * .2) * 
                            (*tcModelData).teproc_.vrng[3] / (float)100. + 
                            1e-10;
    (*tcModelData).teproc_.ftm[10] = vpos[6] * 
                            (*tcModelData).teproc_.vrng[6] / (float)100.;
    (*tcModelData).teproc_.ftm[12] = vpos[7] * 
                            (*tcModelData).teproc_.vrng[7] / (float)100.;
  
    uac = vpos[8] * (*tcModelData).teproc_.vrng[8] * 
                    (tesub8_(ModelData, &c__9, time) + 1.) / (float)100.;
    (*tcModelData).teproc_.fwr = vpos[9] * 
                            (*tcModelData).teproc_.vrng[9] / (float)100.;
    (*tcModelData).teproc_.fws = vpos[10] *
                            (*tcModelData).teproc_.vrng[10] / (float)100.;
    (*tcModelData).teproc_.agsp = (vpos[11] + (float)150.) / (float)100.;
  
    dlp = (*tcModelData).teproc_.ptv - (*tcModelData).teproc_.ptr;
    if (dlp < (float)0.){
	  dlp = (float)0.;
    } //if (dlp < (float)0.){
    flms = sqrt(dlp) * 1937.6;
    (*tcModelData).teproc_.ftm[5] = flms / (*tcModelData).teproc_.xmws[5];

    dlp = (*tcModelData).teproc_.ptr - (*tcModelData).teproc_.pts;
    if (dlp < (float)0.){
	  dlp = (float)0.;
    } //if (dlp < (float)0.){   
    flms = sqrt(dlp) * 4574.21 * 
                            (1. - tesub8_(ModelData, &c__12, time) * .25);
    (*tcModelData).teproc_.ftm[7] = flms / (*tcModelData).teproc_.xmws[7];
  
    dlp = (*tcModelData).teproc_.pts - (float)760.;
    if (dlp < (float)0.){
	  dlp = (float)0.;
    } //if (dlp < (float)0.){
  
    flms = vpos[5] * .151169 * sqrt(dlp);
    (*tcModelData).teproc_.ftm[9] = flms / (*tcModelData).teproc_.xmws[9];
    pr = (*tcModelData).teproc_.ptv / (*tcModelData).teproc_.pts;
    if (pr < (float)1.){
  	  pr = (float)1.;
    } //if (pr < (float)1.){
    if (pr > (*tcModelData).teproc_.cpprmx){
	  pr = (*tcModelData).teproc_.cpprmx;
    } //if (pr > (*tcModelData).teproc_.cpprmx){
    flcoef = (*tcModelData).teproc_.cpflmx / 1.197;

    /* Computing 3rd power */
    d__1 = pr;
    flms = (*tcModelData).teproc_.cpflmx + 
         flcoef * ((float)1. - d__1 * (d__1 * d__1));
    (*tcModelData).teproc_.cpdh = 
         flms * ((*tcModelData).teproc_.tcs + 273.15) * 1.8e-6 * 1.9872 * 
	     ((*tcModelData).teproc_.ptv - (*tcModelData).teproc_.pts) / 
         ((*tcModelData).teproc_.xmws[8] * (*tcModelData).teproc_.pts);
    dlp = (*tcModelData).teproc_.ptv - (*tcModelData).teproc_.pts;
    if (dlp < (float)0.){
	  dlp = (float)0.;
    } //if (dlp < (float)0.){
    flms -= vpos[4] * 53.349 * sqrt(dlp);
    if (flms < .001){
	  flms = .001;
    } //if (flms < .001){
    (*tcModelData).teproc_.ftm[8] = flms / (*tcModelData).teproc_.xmws[8];
    (*tcModelData).teproc_.hst[8] += 
               (*tcModelData).teproc_.cpdh / (*tcModelData).teproc_.ftm[8];
    for (i__ = 1; i__ <= 8; ++i__){
	  (*tcModelData).teproc_.fcm[i__ - 1] = 
                                    (*tcModelData).teproc_.xst[i__ - 1] * 
                                    (*tcModelData).teproc_.ftm[0];
  	  (*tcModelData).teproc_.fcm[i__ + 7] = 
                                    (*tcModelData).teproc_.xst[i__ + 7] * 
                                    (*tcModelData).teproc_.ftm[1];
   	  (*tcModelData).teproc_.fcm[i__ + 15] = 
                                    (*tcModelData).teproc_.xst[i__ + 15] * 
                                    (*tcModelData).teproc_.ftm[2];
	  (*tcModelData).teproc_.fcm[i__ + 23] = 
                                    (*tcModelData).teproc_.xst[i__ + 23] * 
                                    (*tcModelData).teproc_.ftm[3];
	  (*tcModelData).teproc_.fcm[i__ + 39] = 
                                    (*tcModelData).teproc_.xst[i__ + 39] *
                                    (*tcModelData).teproc_.ftm[5];
	  (*tcModelData).teproc_.fcm[i__ + 55] = 
                                    (*tcModelData).teproc_.xst[i__ + 55] * 
                                    (*tcModelData).teproc_.ftm[7];
	  (*tcModelData).teproc_.fcm[i__ + 63] = 
                                    (*tcModelData).teproc_.xst[i__ + 63] * 
                                    (*tcModelData).teproc_.ftm[8];
	  (*tcModelData).teproc_.fcm[i__ + 71] = 
                                    (*tcModelData).teproc_.xst[i__ + 71] * 
                                    (*tcModelData).teproc_.ftm[9];
	  (*tcModelData).teproc_.fcm[i__ + 79] = 
                                    (*tcModelData).teproc_.xst[i__ + 79] * 
                                    (*tcModelData).teproc_.ftm[10];
	  (*tcModelData).teproc_.fcm[i__ + 95] = 
                                    (*tcModelData).teproc_.xst[i__ + 95] * 
                                    (*tcModelData).teproc_.ftm[12];
    /* L5020: */
    } //for (i__ = 1; i__ <= 8; ++i__){
  
    if ((*tcModelData).teproc_.ftm[10] > (float).1){
	  if ((*tcModelData).teproc_.tcc > (float)170.){
	    tmpfac = (*tcModelData).teproc_.tcc - (float)120.262;
	  }else if ((*tcModelData).teproc_.tcc < (float)5.292){
	    tmpfac = (float).1;
	  }else{
	    tmpfac = (float)363.744 / ((float)177. - 
                 (*tcModelData).teproc_.tcc) - (float)2.22579488;
      } //if ((*tcModelData).teproc_.tcc > (float)170.){
    
	  vovrl = (*tcModelData).teproc_.ftm[3] / 
              (*tcModelData).teproc_.ftm[10] * tmpfac;
	  (*tcModelData).teproc_.sfr[3] = vovrl * (float) 8.501  / 
                                     (vovrl * (float)8.501 + (float)1.);
	  (*tcModelData).teproc_.sfr[4] = vovrl * (float)11.402  /  
                                     (vovrl * (float)11.402 + (float)1.);
	  (*tcModelData).teproc_.sfr[5] = vovrl * (float)11.795  / 
                                     (vovrl * (float)11.795 + (float)1.);
	  (*tcModelData).teproc_.sfr[6] = vovrl * (float)  .048  / 
                                     (vovrl * (float).048 + (float)1.);
	  (*tcModelData).teproc_.sfr[7] = vovrl * (float)  .0242 / 
                                     (vovrl * (float).0242 + (float)1.);
    }else{
	  (*tcModelData).teproc_.sfr[3] = (float).9999;
	  (*tcModelData).teproc_.sfr[4] = (float).999;
	  (*tcModelData).teproc_.sfr[5] = (float).999;
	  (*tcModelData).teproc_.sfr[6] = (float).99;
	  (*tcModelData).teproc_.sfr[7] = (float).98;
    } //if ((*tcModelData).teproc_.ftm[10] > (float).1){
  
    for (i__ = 1; i__ <= 8; ++i__){
	  fin[i__ - 1] = (float)0.;
	  fin[i__ - 1] += (*tcModelData).teproc_.fcm[i__ + 23];
	  fin[i__ - 1] += (*tcModelData).teproc_.fcm[i__ + 79];
    /* L6010: */
    } //for (i__ = 1; i__ <= 8; ++i__){
    (*tcModelData).teproc_.ftm[4] = (float)0.;
    (*tcModelData).teproc_.ftm[11] = (float)0.;
    for (i__ = 1; i__ <= 8; ++i__){
	  (*tcModelData).teproc_.fcm[i__ + 31] = 
                       (*tcModelData).teproc_.sfr[i__ - 1] * fin[i__ - 1];
	  (*tcModelData).teproc_.fcm[i__ + 87] = 
                       fin[i__ - 1] - (*tcModelData).teproc_.fcm[i__ + 31];
    
	  (*tcModelData).teproc_.ftm[4]  += 
                                      (*tcModelData).teproc_.fcm[i__ + 31];
  	  (*tcModelData).teproc_.ftm[11] += 
                                      (*tcModelData).teproc_.fcm[i__ + 87];
    /* L6020: */
    } //for (i__ = 1; i__ <= 8; ++i__){
    for (i__ = 1; i__ <= 8; ++i__){
	  (*tcModelData).teproc_.xst[i__ + 31] =
                                   (*tcModelData).teproc_.fcm[i__ + 31] / 
                                   (*tcModelData).teproc_.ftm[4];
	  (*tcModelData).teproc_.xst[i__ + 87] = 
                                   (*tcModelData).teproc_.fcm[i__ + 87] / 
                                   (*tcModelData).teproc_.ftm[11];
    /* L6030: */
    } //for (i__ = 1; i__ <= 8; ++i__){
    (*tcModelData).teproc_.tst[4] = (*tcModelData).teproc_.tcc;
    (*tcModelData).teproc_.tst[11] = (*tcModelData).teproc_.tcc;
    tesub1_(ModelData, &(*tcModelData).teproc_.xst[32], 
            &(*tcModelData).teproc_.tst[4], &(*tcModelData).teproc_.hst[4], 
            &c__1);
    tesub1_(ModelData, &(*tcModelData).teproc_.xst[88], 
            &(*tcModelData).teproc_.tst[11],  
            &(*tcModelData).teproc_.hst[11],&c__0);
    (*tcModelData).teproc_.ftm[6] = (*tcModelData).teproc_.ftm[5];
    (*tcModelData).teproc_.hst[6] = (*tcModelData).teproc_.hst[5];
    (*tcModelData).teproc_.tst[6] = (*tcModelData).teproc_.tst[5];
    for (i__ = 1; i__ <= 8; ++i__){
	  (*tcModelData).teproc_.xst[i__ + 47] = 
                                      (*tcModelData).teproc_.xst[i__ + 39];
  	  (*tcModelData).teproc_.fcm[i__ + 47] =
                                      (*tcModelData).teproc_.fcm[i__ + 39];
    /* L6130: */
    } //for (i__ = 1; i__ <= 8; ++i__){
    
    /*Calculation of Heat Transfer in Reactor*/
    if ((*tcModelData).teproc_.vlr / (float)7.8 > (float)50.){
	  uarlev = (float)1.;
    }else if ((*tcModelData).teproc_.vlr / (float)7.8 < (float)10.){
	  uarlev = (float)0.;
    }else{
	  uarlev = (*tcModelData).teproc_.vlr * (float).025 / (float)7.8 - 
               (float).25;
    } //if ((*tcModelData).teproc_.vlr / (float)7.8 > (float)50.){

    /* Computing 2nd power */
    d__1 = (*tcModelData).teproc_.agsp;
    (*tcModelData).teproc_.uar = uarlev * 
          (d__1 * d__1 * (float)-.5 + 
          (*tcModelData).teproc_.agsp * (float)2.75 - (float)2.5) * .85549;
    (*tcModelData).teproc_.qur = (*tcModelData).teproc_.uar * 
          ((*tcModelData).teproc_.twr - (*tcModelData).teproc_.tcr) * 
          (1. - tesub8_(ModelData, &c__10, time) * .35);

    /*Calculation of Heat Transfer in Condenser (Separator)*/
    /* Computing 4th power */
    d__1  = (*tcModelData).teproc_.ftm[7] / (float)3528.73;
    d__1 *= d__1;
    uas   = ((float)1. - (float)1. / (d__1 * d__1 + (float)1.)) * 
	                                                        (float).404655;
    (*tcModelData).teproc_.qus = uas * 
            ((*tcModelData).teproc_.tws - (*tcModelData).teproc_.tst[7]) * 
	        (1. - tesub8_(ModelData, &c__11, time) * .25);
    
    /*Calculation of Heat Transfer in Stripper*/
    (*tcModelData).teproc_.quc = 0.;
    if ((*tcModelData).teproc_.tcc < (float)100.){
	  (*tcModelData).teproc_.quc = uac * 
                                 ((float)100. - (*tcModelData).teproc_.tcc);
    } //if ((*tcModelData).teproc_.tcc < (float)100.){
    
    (*tcModelData).tlastcomp = *time;
  }else{
    for (i__ = 1; i__ <= 12; ++i__){
	  vpos[i__ - 1] = yy[i__ + 38];
    } //for (i__ = 1; i__ <= 12; ++i__){
  } //if((((*tcModelData).tlastcomp < *time) || (Callflag == 1)) && ...
  
  /*Setting of Measured Values*/
  if((Callflag < 2) && (((*tcModelData).MSFlag & 0x8000) == 0)){
    (*tcModelData).pv_.xmeas[0] = (*tcModelData).teproc_.ftm[2] * 
                              (float).359 / (float)35.3145; 
    (*tcModelData).pv_.xmeas[1] = (*tcModelData).teproc_.ftm[0] * 
                              (*tcModelData).teproc_.xmws[0] * (float).454;
    (*tcModelData).pv_.xmeas[2] = (*tcModelData).teproc_.ftm[1] * 
                              (*tcModelData).teproc_.xmws[1] * (float).454;
    (*tcModelData).pv_.xmeas[3] = (*tcModelData).teproc_.ftm[3] * 
                              (float).359 / (float)35.3145;
    (*tcModelData).pv_.xmeas[4] = (*tcModelData).teproc_.ftm[8] * 
                              (float).359 / (float)35.3145;
    (*tcModelData).pv_.xmeas[5] = (*tcModelData).teproc_.ftm[5] * 
                              (float).359 / (float)35.3145;
    (*tcModelData).pv_.xmeas[6] = ((*tcModelData).teproc_.ptr - 
               (float)760.) / (float)760. * (float)101.325;
    (*tcModelData).pv_.xmeas[7] = ((*tcModelData).teproc_.vlr - 
               (float)84.6) / (float)666.7 * (float)100.;
    (*tcModelData).pv_.xmeas[8] = (*tcModelData).teproc_.tcr;
    (*tcModelData).pv_.xmeas[9] = (*tcModelData).teproc_.ftm[9] * 
                              (float).359 / (float)35.3145;
    (*tcModelData).pv_.xmeas[10] = (*tcModelData).teproc_.tcs;
    (*tcModelData).pv_.xmeas[11] = ((*tcModelData).teproc_.vls - 
               (float)27.5) / (float)290. * (float)100.;
    (*tcModelData).pv_.xmeas[12] = ((*tcModelData).teproc_.pts - 
               (float)760.) / (float)760. * (float)101.325;
    (*tcModelData).pv_.xmeas[13] = (*tcModelData).teproc_.ftm[10] / 
                              (*tcModelData).teproc_.dls / (float)35.3145;
    (*tcModelData).pv_.xmeas[14] = ((*tcModelData).teproc_.vlc - 
               (float)78.25) / (*tcModelData).teproc_.vtc * (float)100.;
    (*tcModelData).pv_.xmeas[15] = ((*tcModelData).teproc_.ptv - 
               (float)760.) / (float)760. * (float)101.325;
    (*tcModelData).pv_.xmeas[16] = (*tcModelData).teproc_.ftm[12] / 
                              (*tcModelData).teproc_.dlc / (float)35.3145;
    (*tcModelData).pv_.xmeas[17] = (*tcModelData).teproc_.tcc;
    (*tcModelData).pv_.xmeas[18] = (*tcModelData).teproc_.quc * 
                              1040. * (float).454; 
    (*tcModelData).pv_.xmeas[19] = (*tcModelData).teproc_.cpdh * 392.7;
    (*tcModelData).pv_.xmeas[19] = (*tcModelData).teproc_.cpdh * 293.07;
    (*tcModelData).pv_.xmeas[20] = (*tcModelData).teproc_.twr;
    (*tcModelData).pv_.xmeas[21] = (*tcModelData).teproc_.tws;
    
    /*Übergabe der zusätzlichen Ausgänge*/
    if(((*tcModelData).MSFlag & 0x1) == 1){
      (*tcModelData).pv_.xmeasadd[0] = (*tcModelData).teproc_.tst[2];
      (*tcModelData).pv_.xmeasadd[1] = (*tcModelData).teproc_.tst[0];
      (*tcModelData).pv_.xmeasadd[2] = (*tcModelData).teproc_.tst[1];
      (*tcModelData).pv_.xmeasadd[3] = (*tcModelData).teproc_.tst[3];
      (*tcModelData).pv_.xmeasadd[4] = (*tcModelData).teproc_.tcwr;
      (*tcModelData).pv_.xmeasadd[5] = (*tcModelData).teproc_.fwr * 
                 (float)0.003785411784 * (float)60.;
      (*tcModelData).pv_.xmeasadd[6] = (*tcModelData).teproc_.tcws;
      (*tcModelData).pv_.xmeasadd[7] = (*tcModelData).teproc_.fws * 
                 (float)0.003785411784 * (float)60.;
    } //if((*ModelData).MSFlag & 0x1) == 1){

    /*Checking of Shut-Down-Constraints*/
    *isd = (float)0.;
    if ((*tcModelData).pv_.xmeas[6] > (float)3e3){
      *isd = (float)1.;
      // DEBUG
      sprintf((*tcModelData).msg, "High Reactor Pressure!!  Shutting down.");
      // printf("High Reactor Pressure!!  Shutting down.\n");
    } //if ((*tcModelData).pv_.xmeas[6] > (float)3e3){
  
    if ((*tcModelData).teproc_.vlr / (float)35.3145 > (float)24.){
      *isd = (float)2.;
      // DEBUG
      sprintf((*tcModelData).msg, "High Reactor Liquid Level!!  Shutting down.");
      // printf("High Reactor Liquid Level!!  Shutting down.\n");
    } //if ((*tcModelData).teproc_.vlr / (float)35.3145 > (float)24.){
  
    if ((*tcModelData).teproc_.vlr / (float)35.3145 < (float)2.){
      *isd = (float)3.;
      // DEBUG
	  sprintf((*tcModelData).msg, "Low Reactor Liquid Level!!  Shutting down.");
	  // printf("Low Reactor Liquid Level!!  Shutting down.\n");
    } //if ((*tcModelData).teproc_.vlr / (float)35.3145 < (float)2.){
  
    if ((*tcModelData).pv_.xmeas[8] > (float)175.){
     *isd = (float)4.;
     // DEBUG
     sprintf((*tcModelData).msg, "High Reactor Temperature!!  Shutting down.");
     // printf("High Reactor Temperature!!  Shutting down.\n");
    } //if ((*tcModelData).pv_.xmeas[8] > (float)175.){
  
    if ((*tcModelData).teproc_.vls / (float)35.3145 > (float)12.){
      *isd = (float)5.;
     // DEBUG
     sprintf((*tcModelData).msg, "High Separator Liquid Level!!  Shutting down.");
     // printf("High Separator Liquid Level!!  Shutting down.\n");
    } //if ((*tcModelData).teproc_.vls / (float)35.3145 > (float)12.){
  
    if ((*tcModelData).teproc_.vls / (float)35.3145 < (float)1.){
      *isd = (float)6.;
     // DEBUG
     sprintf((*tcModelData).msg, "Low Separator Liquid Level!!  Shutting down.");
     // printf("Low Separator Liquid Level!!  Shutting down.\n");
    } //if ((*tcModelData).teproc_.vls / (float)35.3145 < (float)1.){
  
    if ((*tcModelData).teproc_.vlc / (float)35.3145 > (float)8.){
	  *isd = (float)7.;
     // DEBUG
     sprintf((*tcModelData).msg, "High Stripper Liquid Level!!  Shutting down.");
     // printf("High Stripper Liquid Level!!  Shutting down.\n");
    } //if ((*tcModelData).teproc_.vlc / (float)35.3145 > (float)8.){
  
    if ((*tcModelData).teproc_.vlc / (float)35.3145 < (float)1.){
      *isd = (float)8.;
     // DEBUG
      sprintf((*tcModelData).msg, "Low Stripper Liquid Level!!  Shutting down.");
      // printf("Low Stripper Liquid Level!!  Shutting down.\n");
    } //if ((*tcModelData).teproc_.vlc / (float)35.3145 < (float)1.){
    
    /*Adding of Measurement Noise*/
    if (*time > (float)0. && *isd == (float)0.){
      for (i__ = 1; i__ <= 22; ++i__){
	    tesub6_(ModelData, &(*tcModelData).teproc_.xns[i__ - 1], &xmns);
	    (*tcModelData).pv_.xmeas[i__ - 1] += xmns;
      /* L6500: */
      } //for (i__ = 1; i__ <= 22; ++i__){

      if(((*tcModelData).MSFlag & 0x1) == 1){
        for (i__ = 1; i__ <= 8; ++i__){
	      tesub6_(ModelData, &(*tcModelData).teproc_.xnsadd[i__ - 1], 
                  &xmns);
	      (*tcModelData).pv_.xmeasadd[i__ - 1] += xmns;
        /* L6500: */
        } //for (i__ = 1; i__ <= 22; ++i__){      
      }//if(((*tcModelData).MSFlag & 0x1) == 1){
    } //if (*time > (float)0. && *isd == 0){

    /*Analyzer Outputs*/
    xcmp[22] = (*tcModelData).teproc_.xst[48] * (float)100.;
    xcmp[23] = (*tcModelData).teproc_.xst[49] * (float)100.;
    xcmp[24] = (*tcModelData).teproc_.xst[50] * (float)100.;
    xcmp[25] = (*tcModelData).teproc_.xst[51] * (float)100.;
    xcmp[26] = (*tcModelData).teproc_.xst[52] * (float)100.;
    xcmp[27] = (*tcModelData).teproc_.xst[53] * (float)100.;
    xcmp[28] = (*tcModelData).teproc_.xst[72] * (float)100.;
    xcmp[29] = (*tcModelData).teproc_.xst[73] * (float)100.;
    xcmp[30] = (*tcModelData).teproc_.xst[74] * (float)100.;
    xcmp[31] = (*tcModelData).teproc_.xst[75] * (float)100.;
    xcmp[32] = (*tcModelData).teproc_.xst[76] * (float)100.;
    xcmp[33] = (*tcModelData).teproc_.xst[77] * (float)100.;
    xcmp[34] = (*tcModelData).teproc_.xst[78] * (float)100.;
    xcmp[35] = (*tcModelData).teproc_.xst[79] * (float)100.;
    xcmp[36] = (*tcModelData).teproc_.xst[99] * (float)100.;
    xcmp[37] = (*tcModelData).teproc_.xst[100] * (float)100.;
    xcmp[38] = (*tcModelData).teproc_.xst[101] * (float)100.;
    xcmp[39] = (*tcModelData).teproc_.xst[102] * (float)100.;
    xcmp[40] = (*tcModelData).teproc_.xst[103] * (float)100.;
  
    if(((*tcModelData).MSFlag & 0x1) == 1){
      xcmpadd[0]  = (*tcModelData).teproc_.xst[16] * (float)100.;
      xcmpadd[1]  = (*tcModelData).teproc_.xst[17] * (float)100.;
      xcmpadd[2]  = (*tcModelData).teproc_.xst[18] * (float)100.;
      xcmpadd[3]  = (*tcModelData).teproc_.xst[19] * (float)100.;
      xcmpadd[4]  = (*tcModelData).teproc_.xst[20] * (float)100.;
      xcmpadd[5]  = (*tcModelData).teproc_.xst[21] * (float)100.;

      xcmpadd[6]  = (*tcModelData).teproc_.xst[0] * (float)100.;
      xcmpadd[7]  = (*tcModelData).teproc_.xst[1] * (float)100.;
      xcmpadd[8]  = (*tcModelData).teproc_.xst[2] * (float)100.;
      xcmpadd[9]  = (*tcModelData).teproc_.xst[3] * (float)100.;
      xcmpadd[10] = (*tcModelData).teproc_.xst[4] * (float)100.;
      xcmpadd[11] = (*tcModelData).teproc_.xst[5] * (float)100.;

      xcmpadd[12] = (*tcModelData).teproc_.xst[8] * (float)100.;
      xcmpadd[13] = (*tcModelData).teproc_.xst[9] * (float)100.;
      xcmpadd[14] = (*tcModelData).teproc_.xst[10] * (float)100.;
      xcmpadd[15] = (*tcModelData).teproc_.xst[11] * (float)100.;
      xcmpadd[16] = (*tcModelData).teproc_.xst[12] * (float)100.;
      xcmpadd[17] = (*tcModelData).teproc_.xst[13] * (float)100.;
      
      xcmpadd[18] = (*tcModelData).teproc_.xst[24] * (float)100.;
      xcmpadd[19] = (*tcModelData).teproc_.xst[25] * (float)100.;
      xcmpadd[20] = (*tcModelData).teproc_.xst[26] * (float)100.;
      xcmpadd[21] = (*tcModelData).teproc_.xst[27] * (float)100.;
      xcmpadd[22] = (*tcModelData).teproc_.xst[28] * (float)100.;
      xcmpadd[23] = (*tcModelData).teproc_.xst[29] * (float)100.;
    } //if(((*tcModelData).MSFlag & 0x1) == 1){
    
    if (*time == 0.){
      for (i__ = 23; i__ <= 41; ++i__){
        (*tcModelData).teproc_.xdel[i__ - 1] = xcmp[i__ - 1];
	    (*tcModelData).pv_.xmeas[i__ - 1] = xcmp[i__ - 1];
      /* L7010: */
	  } //for (i__ = 23; i__ <= 41; ++i__){
      
      if(((*tcModelData).MSFlag & 0x1) == 1){
        for (i__ = 1; i__ <= 24; ++i__){
          (*tcModelData).teproc_.xdeladd[i__ - 1] = xcmpadd[i__ - 1];
	      (*tcModelData).pv_.xmeasadd[i__ + 7] = xcmpadd[i__ - 1];
	    } //for (i__ = 23; i__ <= 41; ++i__){
      } //if((*ModelData).MSFlag & 0x1) == 1){ 
      
      (*tcModelData).teproc_.tgas = (float).1;
	  (*tcModelData).teproc_.tprod = (float).25;
    } //if (*time == 0.){
    
    if (*time >= (*tcModelData).teproc_.tgas){
      for (i__ = 23; i__ <= 36; ++i__){
        (*tcModelData).pv_.xmeas[i__ - 1] = 
                                      (*tcModelData).teproc_.xdel[i__ - 1];
	    tesub6_(ModelData, &(*tcModelData).teproc_.xns[i__ - 1], &xmns);
	    (*tcModelData).pv_.xmeas[i__ - 1] += xmns;
        
	    (*tcModelData).teproc_.xdel[i__ - 1] = xcmp[i__ - 1];
      /* L7020: */
      } //for (i__ = 23; i__ <= 36; ++i__){
      
      if(((*tcModelData).MSFlag & 0x1) == 1){      
        for (i__ = 1; i__ <= 24; ++i__){
          (*tcModelData).pv_.xmeasadd[i__ + 7] = 
                                   (*tcModelData).teproc_.xdeladd[i__ - 1];
  	      tesub6_(ModelData, &(*tcModelData).teproc_.xnsadd[i__ - 1], 
                  &xmns);
	      (*tcModelData).pv_.xmeasadd[i__ + 7] += xmns;
        
	      (*tcModelData).teproc_.xdeladd[i__ - 1] = xcmpadd[i__ - 1];
        } //for (i__ = 23; i__ <= 36; ++i__){
      }//if(((*tcModelData).MSFlag & 0x1) == 1){
      (*tcModelData).teproc_.tgas += (float).1;
    } //if (*time >= (*tcModelData).teproc_.tgas){
    
    if (*time >= (*tcModelData).teproc_.tprod){
      for (i__ = 37; i__ <= 41; ++i__){
	    (*tcModelData).pv_.xmeas[i__ - 1] = 
                                      (*tcModelData).teproc_.xdel[i__ - 1];
	    tesub6_(ModelData, &(*tcModelData).teproc_.xns[i__ - 1], &xmns);
	    (*tcModelData).pv_.xmeas[i__ - 1] += xmns;
        
	    (*tcModelData).teproc_.xdel[i__ - 1] = xcmp[i__ - 1];
      /* L7030: */
	  } //for (i__ = 37; i__ <= 41; ++i__){
	  (*tcModelData).teproc_.tprod += (float).25;
    } //if (*time >= (*tcModelData).teproc_.tprod){
    
    /*Monitoring Outputs*/
    if(((*tcModelData).MSFlag & 0x4) > 1){
      (*tcModelData).pv_.xmeasmonitor[0]  = 
                             (*tcModelData).teproc_.crxr[0] * (float).454;
      (*tcModelData).pv_.xmeasmonitor[1]  = 
                             (*tcModelData).teproc_.crxr[2] * (float).454;
      (*tcModelData).pv_.xmeasmonitor[2]  = 
                             (*tcModelData).teproc_.crxr[3] * (float).454;
      (*tcModelData).pv_.xmeasmonitor[3]  = 
                             (*tcModelData).teproc_.crxr[4] * (float).454;
      (*tcModelData).pv_.xmeasmonitor[4]  = 
                             (*tcModelData).teproc_.crxr[5] * (float).454;
      (*tcModelData).pv_.xmeasmonitor[5]  = 
                             (*tcModelData).teproc_.crxr[6] * (float).454;
      (*tcModelData).pv_.xmeasmonitor[6]  = 
                             (*tcModelData).teproc_.crxr[7] * (float).454;
      (*tcModelData).pv_.xmeasmonitor[7]  = (*tcModelData).teproc_.ppr[0] / 
                                           (float)760. * (float)101.325;
      (*tcModelData).pv_.xmeasmonitor[8]  = (*tcModelData).teproc_.ppr[1] / 
                                           (float)760. * (float)101.325;
      (*tcModelData).pv_.xmeasmonitor[9]  = (*tcModelData).teproc_.ppr[2] / 
                                           (float)760. * (float)101.325;
      (*tcModelData).pv_.xmeasmonitor[10] = (*tcModelData).teproc_.ppr[3] / 
                                           (float)760. * (float)101.325;
      (*tcModelData).pv_.xmeasmonitor[11] = (*tcModelData).teproc_.ppr[4] / 
                                           (float)760. * (float)101.325;
      (*tcModelData).pv_.xmeasmonitor[12] = (*tcModelData).teproc_.ppr[5] / 
                                           (float)760. * (float)101.325;
      (*tcModelData).pv_.xmeasmonitor[13] = (*tcModelData).teproc_.ppr[6] / 
                                           (float)760. * (float)101.325;
      (*tcModelData).pv_.xmeasmonitor[14] = (*tcModelData).teproc_.ppr[7] / 
                                           (float)760. * (float)101.325;
      (*tcModelData).pv_.xmeasmonitor[15] = (*tcModelData).teproc_.xst[48]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[16] = (*tcModelData).teproc_.xst[49]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[17] = (*tcModelData).teproc_.xst[50]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[18] = (*tcModelData).teproc_.xst[51]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[19] = (*tcModelData).teproc_.xst[52]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[20] = (*tcModelData).teproc_.xst[53]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[21] = (*tcModelData).teproc_.xst[72]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[22] = (*tcModelData).teproc_.xst[73]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[23] = (*tcModelData).teproc_.xst[74]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[24] = (*tcModelData).teproc_.xst[75]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[25] = (*tcModelData).teproc_.xst[76]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[26] = (*tcModelData).teproc_.xst[77]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[27] = (*tcModelData).teproc_.xst[78]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[28] = (*tcModelData).teproc_.xst[79]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[29] = (*tcModelData).teproc_.xst[99]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[30] =(*tcModelData).teproc_.xst[100]*
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[31] =(*tcModelData).teproc_.xst[101]*
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[32] =(*tcModelData).teproc_.xst[102]*
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[33] =(*tcModelData).teproc_.xst[103]*
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[34] = (*tcModelData).teproc_.xst[16]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[35] = (*tcModelData).teproc_.xst[17]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[36] = (*tcModelData).teproc_.xst[18]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[37] = (*tcModelData).teproc_.xst[19]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[38] = (*tcModelData).teproc_.xst[20]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[39] = (*tcModelData).teproc_.xst[21]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[40] = (*tcModelData).teproc_.xst[0] * 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[41] = (*tcModelData).teproc_.xst[1] * 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[42] = (*tcModelData).teproc_.xst[2] * 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[43] = (*tcModelData).teproc_.xst[3] * 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[44] = (*tcModelData).teproc_.xst[4] * 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[45] = (*tcModelData).teproc_.xst[5] * 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[46] = (*tcModelData).teproc_.xst[8] * 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[47] = (*tcModelData).teproc_.xst[9] * 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[48] = (*tcModelData).teproc_.xst[10]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[49] = (*tcModelData).teproc_.xst[11]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[50] = (*tcModelData).teproc_.xst[12]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[51] = (*tcModelData).teproc_.xst[13]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[52] = (*tcModelData).teproc_.xst[24]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[53] = (*tcModelData).teproc_.xst[25]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[54] = (*tcModelData).teproc_.xst[26]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[55] = (*tcModelData).teproc_.xst[27]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[56] = (*tcModelData).teproc_.xst[28]* 
                                           (float)100.;
      (*tcModelData).pv_.xmeasmonitor[57] = (*tcModelData).teproc_.xst[29]* 
                                           (float)100.;
      
      prate = (float)211.3 * yy[46] / (float)46.534 + (float)1.0e-6;
      (*tcModelData).pv_.xmeasmonitor[58] = 
          (float)100 * ((float).0536 * (*tcModelData).pv_.xmeas[19] + 
                        (float).0318 * (*tcModelData).pv_.xmeas[18] +
                        (*tcModelData).pv_.xmeas[9] * (float).44791 * 
                           ((float)2.209 * (*tcModelData).pv_.xmeas[28] +
                            (float)6.177 * (*tcModelData).pv_.xmeas[30] +
                            (float)22.06 * (*tcModelData).pv_.xmeas[31] +
                            (float)14.56 * (*tcModelData).pv_.xmeas[32] +
                            (float)17.89 * (*tcModelData).pv_.xmeas[33] +
                            (float)30.44 * (*tcModelData).pv_.xmeas[34] +
                            (float)22.94 * (*tcModelData).pv_.xmeas[35]) +
                        prate * 
                           ((float).2206 * (*tcModelData).pv_.xmeas[36] +
                            (float).1456 * (*tcModelData).pv_.xmeas[37] + 
                            (float).1789 * (*tcModelData).pv_.xmeas[38])
                       ) / prate;
      (*tcModelData).pv_.xmeasmonitor[59] = 
        (float)100 * ((float).0536 * (*tcModelData).teproc_.cpdh * 293.07 + 
                      (float).0318 * (*tcModelData).teproc_.quc *  1040. * 
                      (float).454 +
                      (*tcModelData).teproc_.ftm[9] * 
                      (float).359 / (float)35.3145 * (float).44791 * 
                           ((float)2.209 * xcmp[28] +
                            (float)6.177 * xcmp[30] +
                            (float)22.06 * xcmp[31] +
                            (float)14.56 * xcmp[32] +
                            (float)17.89 * xcmp[33] +
                            (float)30.44 * xcmp[34] +
                            (float)22.94 * xcmp[35]) +
                      prate * 
                           ((float).2206 * xcmp[36] +
                            (float).1456 * xcmp[37] + 
                            (float).1789 * xcmp[38])
                     ) / prate;
      (*tcModelData).pv_.xmeasmonitor[60] = 
        ((float).0536 * (*tcModelData).pv_.xmeas[19] + 
         (float).0318 * (*tcModelData).pv_.xmeas[18] +
         (*tcModelData).pv_.xmeas[9] * (float).44791 * 
             ((float)2.209 * (*tcModelData).pv_.xmeas[28] +
              (float)6.177 * (*tcModelData).pv_.xmeas[30] +
              (float)22.06 * (*tcModelData).pv_.xmeas[31] +
              (float)14.56 * (*tcModelData).pv_.xmeas[32] +
              (float)17.89 * (*tcModelData).pv_.xmeas[33] +
              (float)30.44 * (*tcModelData).pv_.xmeas[34] +
              (float)22.94 * (*tcModelData).pv_.xmeas[35]) +
         prate * 
             ((float).2206 * (*tcModelData).pv_.xmeas[36] +
              (float).1456 * (*tcModelData).pv_.xmeas[37] + 
              (float).1789 * (*tcModelData).pv_.xmeas[38]));
      (*tcModelData).pv_.xmeasmonitor[61] = 
        ((float).0536 * (*tcModelData).teproc_.cpdh * 293.07 + 
         (float).0318 * (*tcModelData).teproc_.quc *  1040. * (float).454 +
         (*tcModelData).teproc_.ftm[9] * 
         (float).359 / (float)35.3145 * (float).44791 * 
             ((float)2.209 * xcmp[28] +
              (float)6.177 * xcmp[30] +
              (float)22.06 * xcmp[31] +
              (float)14.56 * xcmp[32] +
              (float)17.89 * xcmp[33] +
              (float)30.44 * xcmp[34] +
              (float)22.94 * xcmp[35]) +
         prate * 
             ((float).2206 * xcmp[36] +
              (float).1456 * xcmp[37] + 
              (float).1789 * xcmp[38]));
    } //if((*ModelData).MSFlag & 0x4) > 1){

    /*Output of Component Concentrations*/
    if(((*tcModelData).MSFlag & 0x8) > 1){
      for (i__ = 1; i__ <= 8; ++i__){
        (*tcModelData).pv_.xmeascomp[0  + i__ - 1] =
                                  (*tcModelData).teproc_.xst[0  + i__ - 1];
        (*tcModelData).pv_.xmeascomp[8  + i__ - 1] = 
                                  (*tcModelData).teproc_.xst[8  + i__ - 1];
        (*tcModelData).pv_.xmeascomp[16 + i__ - 1] = 
                                  (*tcModelData).teproc_.xst[16 + i__ - 1];
        (*tcModelData).pv_.xmeascomp[24 + i__ - 1] = 
                                  (*tcModelData).teproc_.xst[24 + i__ - 1];
        (*tcModelData).pv_.xmeascomp[32 + i__ - 1] = 
                                  (*tcModelData).teproc_.xst[32 + i__ - 1];
        (*tcModelData).pv_.xmeascomp[40 + i__ - 1] = 
                                  (*tcModelData).teproc_.xst[40 + i__ - 1];
        (*tcModelData).pv_.xmeascomp[48 + i__ - 1] = 
                                  (*tcModelData).teproc_.xst[56 + i__ - 1];
        (*tcModelData).pv_.xmeascomp[56 + i__ - 1] = 
                                  (*tcModelData).teproc_.xst[64 + i__ - 1];
        (*tcModelData).pv_.xmeascomp[64 + i__ - 1] = 
                                  (*tcModelData).teproc_.xst[72 + i__ - 1];
        (*tcModelData).pv_.xmeascomp[72 + i__ - 1] = 
                                  (*tcModelData).teproc_.xst[80 + i__ - 1];
        (*tcModelData).pv_.xmeascomp[80 + i__ - 1] = 
                                  (*tcModelData).teproc_.xst[88 + i__ - 1];
        (*tcModelData).pv_.xmeascomp[88 + i__ - 1] = 
                                  (*tcModelData).teproc_.xst[96 + i__ - 1];
      } //for (i__ = 1; i__ <= 8; ++i__){
    } //if(((*tcModelData).MSFlag & 0x8) > 1){
  } //if((Callflag < 2) && (((*tcModelData).MSFlag & 0x8000) == 0)){
  
  /*Calculation of State Derivative*/
  if(((Callflag == 2) ||(Callflag == 0)) &&
     (((*tcModelData).MSFlag & 0x8000) == 0)){
    for (i__ = 1; i__ <= 8; ++i__){
  	  yp[i__]      = (*tcModelData).teproc_.fcm[i__ + 47] - 
	                 (*tcModelData).teproc_.fcm[i__ + 55] + 
                     (*tcModelData).teproc_.crxr[i__ - 1];
      yp[i__ + 9]  = (*tcModelData).teproc_.fcm[i__ + 55] - 
                     (*tcModelData).teproc_.fcm[i__ + 63] - 
		             (*tcModelData).teproc_.fcm[i__ + 71] - 
                     (*tcModelData).teproc_.fcm[i__ + 79];
      yp[i__ + 18] = (*tcModelData).teproc_.fcm[i__ + 87] - 
                     (*tcModelData).teproc_.fcm[i__ + 95];
	  yp[i__ + 27] = (*tcModelData).teproc_.fcm[i__ - 1]  + 
                     (*tcModelData).teproc_.fcm[i__ + 7]  + 
	      	         (*tcModelData).teproc_.fcm[i__ + 15] + 
                     (*tcModelData).teproc_.fcm[i__ + 31] + 
		             (*tcModelData).teproc_.fcm[i__ + 63] - 
                     (*tcModelData).teproc_.fcm[i__ + 39];
    /* L9010: */
    } //for (i__ = 1; i__ <= 8; ++i__){
    
    yp[9] = (*tcModelData).teproc_.hst[6] * (*tcModelData).teproc_.ftm[6] - 
            (*tcModelData).teproc_.hst[7] * (*tcModelData).teproc_.ftm[7] + 
            (*tcModelData).teproc_.rh + (*tcModelData).teproc_.qur;
/* 		Here is the "correct" version of the separator energy balance: */
/* 	YP(18)=HST(8)*FTM(8)- */
/*    .(HST(9)*FTM(9)-cpdh)- */
/*    .HST(10)*FTM(10)- */
/*    .HST(11)*FTM(11)+ */
/*    .QUS */
/* 		Here is the original version */
    yp[18] = 
           (*tcModelData).teproc_.hst[7]  * (*tcModelData).teproc_.ftm[7] - 
           (*tcModelData).teproc_.hst[8]  * (*tcModelData).teproc_.ftm[8] -
           (*tcModelData).teproc_.hst[9]  * (*tcModelData).teproc_.ftm[9] - 
	       (*tcModelData).teproc_.hst[10] * (*tcModelData).teproc_.ftm[10]+
           (*tcModelData).teproc_.qus;
    yp[27] = 
           (*tcModelData).teproc_.hst[3]  * (*tcModelData).teproc_.ftm[3] + 
           (*tcModelData).teproc_.hst[10] * (*tcModelData).teproc_.ftm[10]-
           (*tcModelData).teproc_.hst[4]  * (*tcModelData).teproc_.ftm[4] - 
	       (*tcModelData).teproc_.hst[12] * (*tcModelData).teproc_.ftm[12]+
           (*tcModelData).teproc_.quc;
    yp[36] = 
           (*tcModelData).teproc_.hst[0] * (*tcModelData).teproc_.ftm[0] + 
           (*tcModelData).teproc_.hst[1] * (*tcModelData).teproc_.ftm[1] + 
           (*tcModelData).teproc_.hst[2] * (*tcModelData).teproc_.ftm[2] + 
	       (*tcModelData).teproc_.hst[4] * (*tcModelData).teproc_.ftm[4] + 
           (*tcModelData).teproc_.hst[8] * (*tcModelData).teproc_.ftm[8] - 
           (*tcModelData).teproc_.hst[5] * (*tcModelData).teproc_.ftm[5];
    
    yp[37] = ((*tcModelData).teproc_.fwr * (float)500.53 * 
              ((*tcModelData).teproc_.tcwr - (*tcModelData).teproc_.twr) -
  	          (*tcModelData).teproc_.qur * 1e6 / (float)1.8) / 
             (*tcModelData).teproc_.hwr;
    yp[38] = ((*tcModelData).teproc_.fws * (float)500.53 * 
              ((*tcModelData).teproc_.tcws - (*tcModelData).teproc_.tws) - 
   	          (*tcModelData).teproc_.qus * 1e6 / (float)1.8) / 
             (*tcModelData).teproc_.hws;

    (*tcModelData).teproc_.ivst[9]  = (*tcModelData).dvec_.idv[13];
    (*tcModelData).teproc_.ivst[10] = (*tcModelData).dvec_.idv[14];
    (*tcModelData).teproc_.ivst[4]  = (*tcModelData).dvec_.idv[18];
    (*tcModelData).teproc_.ivst[6]  = (*tcModelData).dvec_.idv[18];
    (*tcModelData).teproc_.ivst[7]  = (*tcModelData).dvec_.idv[18];
    (*tcModelData).teproc_.ivst[8]  = (*tcModelData).dvec_.idv[18];
    for (i__ = 1; i__ <= 12; ++i__){
      if ((*time == 0.) || 
          (d__1 = (*tcModelData).teproc_.vcv[i__ - 1] - 
                  (*tcModelData).pv_.xmv[i__ - 1], 
  		   abs(d__1)) > (*tcModelData).teproc_.vst[i__ - 1] * 
                        (*tcModelData).teproc_.ivst[i__ - 1]){
	    (*tcModelData).teproc_.vcv[i__ - 1] = 
                                           (*tcModelData).pv_.xmv[i__ - 1];
	  } //if ((*time == 0.) || ...
    
      /*Constraints of Manipulated Variable*/
	  if ((*tcModelData).teproc_.vcv[i__ - 1] < (float)0.){
  	    (*tcModelData).teproc_.vcv[i__ - 1] = (float)0.;
	  } //if ((*tcModelData).teproc_.vcv[i__ - 1] < (float)0.){
	  if ((*tcModelData).teproc_.vcv[i__ - 1] > (float)100.){
	    (*tcModelData).teproc_.vcv[i__ - 1] = (float)100.;
	  } //if ((*tcModelData).teproc_.vcv[i__ - 1] > (float)100.){
    
	  yp[i__ + 38] = 
                   ((*tcModelData).teproc_.vcv[i__ - 1] - vpos[i__ - 1]) / 
		           (*tcModelData).teproc_.vtau[i__ - 1];
    /* L9020: */
    } //for (i__ = 1; i__ <= 12; ++i__){

    if (*time > (float)0. && *isd != (float)0.){
	  return (int)*isd;
      i__1 = *nn;
	  for (i__ = 1; i__ <= i__1; ++i__){
        yp[i__] = (float)0.;
      /* L9030: */
	  } //for (i__ = 1; i__ <= i__1; ++i__){
    } //if (*time > (float)0. && *isd != 0){
  } //if(((Callflag == 3) ||(Callflag == 0)) && ...
  
  #undef isd
  
  return 0;
} /* tefunc_ */


/*-------------------------------------------------------------------------
-                              t e s u b 1 _                              -
-------------------------------------------------------------------------*/
static int tesub1_(void *ModelData, 
                   doublereal *z__, doublereal *t, doublereal *h__, 
                   const integer *ity){
  /*----------------------------- Variables -----------------------------*/
  doublereal d__1;
  struct stModelData *tcModelData;
  integer i__;
  doublereal r__, hi;

  /*Typcast of Dataset Pointer*/  
  tcModelData = ModelData;

  /*Parameter adjustments*/
  --z__;

  /*--------------------------- Function Body ---------------------------*/
  if (*ity == 0){
    *h__ = 0.;
    for (i__ = 1; i__ <= 8; ++i__){
      /* Computing 2nd power */
	  d__1 = *t;
	  /*Integration von Enthalpie-Gleichung (temperaturabhängigkeit) 
        in tesub3_*/
	  hi = *t * ((*tcModelData).const_.ah[i__ - 1] + 
                 (*tcModelData).const_.bh[i__ - 1] * *t / 2. + 
                 (*tcModelData).const_.ch[i__ - 1] * (d__1 * d__1) / 3.);
      hi *= 1.8;
	  *h__ += z__[i__] * (*tcModelData).const_.xmw[i__ - 1] * hi;
/* L100: */
    } //for (i__ = 1; i__ <= 8; ++i__){
  }else{
    *h__ = 0.;
    for (i__ = 1; i__ <= 8; ++i__){
      /* Computing 2nd power */
	  d__1 = *t;
	  hi = *t * ((*tcModelData).const_.ag[i__ - 1] + 
                 (*tcModelData).const_.bg[i__ - 1] * *t / 2. + 
	   	         (*tcModelData).const_.cg[i__ - 1] * (d__1 * d__1) / 3.);
      hi *= 1.8;
      hi += (*tcModelData).const_.av[i__ - 1];
      *h__ += z__[i__] * (*tcModelData).const_.xmw[i__ - 1] * hi;
/* L200: */
    } //for (i__ = 1; i__ <= 8; ++i__){
  } //if (*ity == 0){
    
  if (*ity == 2){
    r__ = 3.57696e-6;
    *h__ -= r__ * (*t + (float)273.15);
  } //if (*ity == 2){
  
  return 0;
} /* tesub1_ */


/*-------------------------------------------------------------------------
-                              t e s u b 2 _                              -
-------------------------------------------------------------------------*/
static int tesub2_(void *ModelData, 
                   doublereal *z__, doublereal *t, doublereal *h__, 
                   const integer *ity){
  /*----------------------------- Variables -----------------------------*/
  integer j;
  doublereal htest;
  doublereal dh;
  doublereal dt, err, tin;

  /*Parameter adjustments*/
  --z__;

  /*--------------------------- Function Body ---------------------------*/
  tin = *t;
  for (j = 1; j <= 100; ++j){
	tesub1_(ModelData, &z__[1], t, &htest, ity);
	err = htest - *h__;
	tesub3_(ModelData, &z__[1], t, &dh, ity);
	dt = -err / dh;
	*t += dt;
/* L250: */
    if (abs(dt) < 1e-12){
	  goto L300;
    } //if (abs(dt) < 1e-12){
  } //for (j = 1; j <= 100; ++j){
  *t = tin;
  
  /*------------------------ B R A N C H M A R K ------------------------*/
  L300:
  /*------------------------ B R A N C H M A R K ------------------------*/
  return 0;
} /* tesub2_ */


/*-------------------------------------------------------------------------
-                              t e s u b 3 _                              -
-------------------------------------------------------------------------*/
static int tesub3_(void *ModelData, 
                   doublereal *z__, doublereal *t, doublereal *dh, 
   		           const integer *ity){
  /*----------------------------- Variables -----------------------------*/
  doublereal d__1;
  struct stModelData *tcModelData;
  integer i__;
  doublereal r__, dhi;

  /*Typcast of Dataset Pointer*/
  tcModelData = ModelData;

  /*Parameter adjustments*/
  --z__;

  /*--------------------------- Function Body ---------------------------*/
  if (*ity == 0){
    *dh = 0.;
	for (i__ = 1; i__ <= 8; ++i__){
/* Computing 2nd power */
	  d__1 = *t;
	  dhi = (*tcModelData).const_.ah[i__ - 1] +
            (*tcModelData).const_.bh[i__ - 1] * *t + 
		    (*tcModelData).const_.ch[i__ - 1] * (d__1 * d__1);
      dhi *= 1.8;
      *dh += z__[i__] * (*tcModelData).const_.xmw[i__ - 1] * dhi;
/* L100: */
	} //for (i__ = 1; i__ <= 8; ++i__){
  }else{
	*dh = 0.;
	for (i__ = 1; i__ <= 8; ++i__){
/* Computing 2nd power */
	  d__1 = *t;
	  dhi = (*tcModelData).const_.ag[i__ - 1] + 
            (*tcModelData).const_.bg[i__ - 1] * *t +
		    (*tcModelData).const_.cg[i__ - 1] * (d__1 * d__1);
      dhi *= 1.8;
      *dh += z__[i__] * (*tcModelData).const_.xmw[i__ - 1] * dhi;
/* L200: */
	} //for (i__ = 1; i__ <= 8; ++i__){
  } //if (*ity == 0){
  
  if (*ity == 2){
	r__ = 3.57696e-6;
	*dh -= r__;
  } //if (*ity == 2){
  
  return 0;
} /* tesub3_ */


/*-------------------------------------------------------------------------
-                              t e s u b 4 _                              -
-------------------------------------------------------------------------*/
static int tesub4_(void *ModelData, 
                   doublereal *x, doublereal *t, doublereal *r__){
  /*----------------------------- Variables -----------------------------*/
  struct stModelData *tcModelData;
  integer i__;
  doublereal v;
  
  /*Typcast of Dataset Pointer*/
  tcModelData = ModelData;  

  /*Parameter adjustments*/
  --x;

  /*--------------------------- Function Body ---------------------------*/
  v = (float)0.;
  for (i__ = 1; i__ <= 8; ++i__){
	v += x[i__] * (*tcModelData).const_.xmw[i__ - 1] / 
         ((*tcModelData).const_.ad[i__ - 1] + 
          ((*tcModelData).const_.bd[i__ - 1] + 
           (*tcModelData).const_.cd[i__ - 1] * *t) * *t);
/* L10: */
  } //for (i__ = 1; i__ <= 8; ++i__){
  *r__ = (float)1. / v;
  return 0;
} /* tesub4_ */


/*-------------------------------------------------------------------------
-                              t e s u b 5 _                              -
-------------------------------------------------------------------------*/
static int tesub5_(void *ModelData, 
                   doublereal *s, doublereal *sp, doublereal *adist, 
                   doublereal *bdist, doublereal *cdist, doublereal *ddist,
                   doublereal *tlast, doublereal *tnext, doublereal *hspan,
   			       doublereal *hzero, doublereal *sspan, doublereal *szero,
			       doublereal *spspan, doublereal *idvflag){
  /*----------------------------- Variables -----------------------------*/
  doublereal h__;
  integer i__;
  doublereal s1;
  doublereal s1p;
  doublereal d__1;

  /*--------------------------- Function Body ---------------------------*/
  i__    = -1;
  h__    = *hspan * tesub7_(ModelData, &i__) + *hzero;
  s1     = *sspan * tesub7_(ModelData, &i__) * *idvflag + *szero;
  s1p    = *spspan * tesub7_(ModelData, &i__) * *idvflag;
  
  /* Computing 0th power */
  *adist = *s;
  /* Computing 1rd power */
  *bdist = *sp;   
  /* Computing 2nd power */
  d__1   = h__;
  *cdist = ((s1 - *s) * 3. - h__ * (s1p + *sp * 2.)) / (d__1 * d__1);
  /* Computing 3rd power */
  d__1   = h__;
  *ddist = ((*s - s1) * 2. + h__ * (s1p + *sp)) / (d__1 * (d__1 * d__1));

  *tnext = *tlast + h__;
  
  return 0;
} /* tesub5_ */


/*-------------------------------------------------------------------------
-                              t e s u b 6 _                              -
-------------------------------------------------------------------------*/
static int tesub6_(void *ModelData, doublereal *std, doublereal *x){
  /*----------------------------- Variables -----------------------------*/
  integer i__;
  
  /*--------------------------- Function Body ---------------------------*/
  *x = 0.;
  for (i__ = 1; i__ <= 12; ++i__){
    *x += tesub7_(ModelData, &i__);
  } //for (i__ = 1; i__ <= 12; ++i__){
    
  *x = (*x - 6.) * *std;
  
  return 0;
} /* tesub6_ */


/*-------------------------------------------------------------------------
-                              t e s u b 7 _                              -
-------------------------------------------------------------------------*/
static doublereal tesub7_(void *ModelData, integer *i__){
  /*----------------------------- Variables -----------------------------*/
  struct stModelData *tcModelData;
  doublereal ret_val, *d__1, c_b78;
  
  /*Typcast of Dataset Pointer*/
  tcModelData = ModelData;
  
  
  /*--------------------------- Function Body ---------------------------*/
  c_b78 = 4294967296.;
  
  /*Generation of Random Numbers for Measurment Noise*/
  if (*i__ >= 0){
    if(((*tcModelData).MSFlag & 0x20) > 0){
      d__1 = &(*tcModelData).randsd_.measnoise;
    }else{
      d__1 = &(*tcModelData).randsd_.g;
    }

    *d__1 = *d__1 * 9228907.;
    *d__1 = d_mod(d__1, &c_b78);    
    
    ret_val = *d__1 / 4294967296.;
  } //if (*i__ >= 0){
  
  /*Generation of Random Numbers for Process Disturbances*/
  if (*i__ < 0){
    if(((*tcModelData).MSFlag & 0x20) > 0){
      d__1 = &(*tcModelData).randsd_.procdist;
    }else{
      d__1 = &(*tcModelData).randsd_.g;
    }
    
    *d__1 = *d__1 * 9228907.;
    *d__1 = d_mod(d__1, &c_b78);
    
    ret_val = *d__1 * 2.0 / 4294967296. - 1.;
  } //if (*i__ < 0){
  
  return ret_val;
} /* tesub7_ */


/*-------------------------------------------------------------------------
-                              t e s u b 8 _                              -
-------------------------------------------------------------------------*/
static doublereal tesub8_(void *ModelData, 
                          const integer *i__, doublereal *t){
  /*----------------------------- Variables -----------------------------*/
  doublereal ret_val;
  struct stModelData *tcModelData;
  doublereal h__;

  /*Typcast of Dataset Pointer*/
  tcModelData = ModelData;

  /*--------------------------- Function Body ---------------------------*/
  h__ = *t - (*tcModelData).wlk_.tlast[*i__ - 1];
  ret_val = (*tcModelData).wlk_.adist[*i__ - 1] + 
            h__ * ((*tcModelData).wlk_.bdist[*i__ - 1] + 
                   h__ * ((*tcModelData).wlk_.cdist[*i__ - 1] + 
                          h__ * (*tcModelData).wlk_.ddist[*i__ - 1]));
  return ret_val;
} /* tesub8_ */


/*-------------------------------------------------------------------------
-                               ? ? ? ? ? ?                               -
-------------------------------------------------------------------------*/
#ifdef KR_headers
  #ifdef IEEE_drem
    double drem();
  #else
    double floor();
  #endif
  double d_mod(x,y) doublereal *x, *y;
#else
  #ifdef IEEE_drem
    double drem(double, double);
  #else
    #undef abs
  #endif
  double d_mod(doublereal *x, doublereal *y)
#endif
{
#ifdef IEEE_drem
  double xa, ya, z;
  
  if ((ya = *y) < 0.){
    ya = -ya;
  } //if ((ya = *y) < 0.){
  
  z = drem(xa = *x, ya);
  if (xa > 0){
	if (z < 0){
      z += ya;
    } //if (z < 0){
  }else{
    if (z > 0){
      z -= ya;
    } //if (z > 0){
  } //if (xa > 0){
  return z;
#else
	double quotient;
	if( (quotient = *x / *y) >= 0)
		quotient = floor(quotient);
	else
		quotient = -floor(-quotient);
	return(*x - (*y) * quotient );
#endif
} //?? 


/*-------------------------------------------------------------------------
-                               ? ? ? ? ? ?                               -
-------------------------------------------------------------------------*/
#ifdef KR_headers
  double pow();
  double pow_dd(ap, bp) doublereal *ap, *bp;
#else
  #undef abs
  double pow_dd(doublereal *ap, const doublereal *bp)
#endif
{
  return(pow(*ap, *bp) );
} //?? 
