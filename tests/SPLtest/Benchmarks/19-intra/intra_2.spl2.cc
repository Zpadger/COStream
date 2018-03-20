//改变8x8块节点的NxN模式的循环；影响工作量估计；
#define UInt unsigned int
//#define bool bool   //costream不支持bool类型，用unsigned char型代替
#define bool int
#define Pel short
#define Int int
#define Double double
#define Float float
#define Char char
#define UChar unsigned char
#define NULL 0x0000000000000000
#define true 1
#define false 0

#define MAX_DOUBLE                  1.7e+308    ///< max. value of double-type value

//typedef       long Int64;  //8 bytes
//typedef       unsigned long  UInt64 //8 bytes

typedef       long long         Int64;   //在linux下测试大小为 8 bytes
typedef       unsigned long long  UInt64;//在linux下测试大小为 8 bytes

struct TComDataCU;  //前向声明结构体
typedef struct TComDataCU TComDataCU;

struct TComSlice
{
	Double      m_lambdas[3];
	Int         m_iDepth;
	Int  m_aiNumRefIdx[2];    //  for multiple reference of current slice
	bool m_bLMvdL1Zero;
	Int  m_list1IdxToList0Idx[16];
	
};
typedef struct TComSlice TComSlice; 


struct TComPicSym
{
	UInt          m_uiWidthInCU;
	UInt          m_uiHeightInCU;
  
	UInt          m_uiMaxCUWidth;
	UInt          m_uiMaxCUHeight;
	UInt          m_uiMinCUWidth;
	UInt          m_uiMinCUHeight;
  
	UChar         m_uhTotalDepth;       ///< max. depth
	UInt          m_uiNumPartitions;
	UInt          m_uiNumPartInWidth;
	UInt          m_uiNumPartInHeight;
	UInt          m_uiNumCUsInFrame;
	
	TComSlice**   m_apcTComSlice;
	TComDataCU**  m_apcTComDataCU; 	
};

typedef struct TComPicSym TComPicSym;

struct TComPicYuv
{
	Pel*  m_apiPicBufY;           ///< Buffer (including margin)
	Pel*  m_apiPicBufU;
	Pel*  m_apiPicBufV;
  
	Pel*  m_piPicOrgY;            ///< m_apiPicBufY + m_iMarginLuma*getStride() + m_iMarginLuma
	Pel*  m_piPicOrgU;
	Pel*  m_piPicOrgV;
  
	// ------------------------------------------------------------------------------------------------
	//  Parameter for general YUV buffer usage
	// ------------------------------------------------------------------------------------------------
  
	Int   m_iPicWidth;            ///< Width of picture
	Int   m_iPicHeight;           ///< Height of picture
	
	Int*  m_cuOffsetY;
	Int*  m_cuOffsetC;
	Int*  m_buOffsetY;
	Int*  m_buOffsetC;
  
	Int   m_iLumaMarginX;
	Int   m_iLumaMarginY;
	Int   m_iChromaMarginX;
	Int   m_iChromaMarginY;
	
	bool  m_bIsBorderExtended;
	
};

typedef struct TComPicYuv TComPicYuv;

struct TComPic
{
	
	UInt                  m_uiTLayer;               //  Temporal layer
	bool                  m_bUsedByCurr;            //  Used by current picture
	bool                  m_bIsLongTerm;            //  IS long term picture
	
	TComPicSym*           m_apcPicSym;  
	TComPicYuv*           m_apcPicYuv[2];           //  Texture,  0:org / 1:rec
  
    TComPicYuv*           m_pcPicYuvPred;           //  Prediction
    TComPicYuv*           m_pcPicYuvResi;           //  Residual
	
};
typedef struct TComPic TComPic;

struct TComDataCU
{
	UInt m_uiCUAddr;
	UInt m_uiAbsIdxInLCU;
	UInt          m_uiCUPelX;           ///< CU position in a pixel (X)
    UInt          m_uiCUPelY;  
	TComPic*      m_pcPic;              ///< picture class pointer
	TComSlice*    m_pcSlice;            ///< slice header pointer
	
	UChar* m_puhDepth;  
	bool* m_skipFlag;
	Char* m_pePartSize;
	Char* m_pePredMode;         ///< array of prediction modes
	Char* m_CUTransquantBypass;
	//bool* m_CUTransquantBypass;   ///< array of cu_transquant_bypass flags
	Char* m_phQP;               ///< array of QP values
	
	UChar* m_puhTrIdx;
	UChar* m_puhCbf[3];   
	UChar* m_puhTransformSkip[3];
	UChar* m_puhLumaIntraDir; 
	
	Double m_dTotalCost;         ///< sum of partition RD costs
	UInt m_uiTotalDistortion; 
	UInt m_uiTotalBits;        ///< sum of partition bits
	UInt m_uiTotalBins;       ///< sum of partition bins 

	UChar*        m_puhChromaIntraDir;  ///< array of intra directions (chroma)
	UChar*        m_puhInterDir;	
	Char*         m_apiMVPIdx[2];       ///< array of motion vector predictor candidates
	Char*         m_apiMVPNum[2];  
};

struct TComYuv
{
  Pel*    m_apiBufY;
  Pel*    m_apiBufU;
  Pel*    m_apiBufV;
  
  // ------------------------------------------------------------------------------------------------------------------
  //  Parameter for general YUV buffer usage
  // ------------------------------------------------------------------------------------------------------------------
  
  UInt     m_iWidth;
  UInt     m_iHeight;
  UInt     m_iCWidth;
  UInt     m_iCHeight;
};
typedef struct TComYuv TComYuv;

struct TComTrQuant
{
  UInt     m_uiMaxTrSize;
  bool     m_bEnc;
  bool     m_useRDOQ;
  bool     m_useRDOQTS;
  bool     m_bUseAdaptQpSelect;
  bool     m_useTransformSkipFast;
  
};
typedef struct TComTrQuant TComTrQuant;

struct TEncSbac
{
	
};
typedef struct TEncSbac TEncSbac;

struct DistParam
{

  //Pel*  pOrg;   //!< 原始图像首地址 
  //Pel*  pCur;    //!< 参考图像首地址 
  //Int   iStrideOrg;   //!< 原始图像跨度  
  //Int   iStrideCur;  //!< 参考图像跨度 
  //Int   iRows;     //!< PU的宽度
  //Int   iCols;   //!< PU的高度
  //Int   iStep;
  //FpDistFunc DistFunc;  //!< 计算失真的函数指针  
  //Int   bitDepth;  //!< 位深
  bool    bApplyWeight;     // whether weithed prediction is used or not
};
typedef struct DistParam DistParam;

struct TEncSearch
{
	UChar*          m_puhQTTempTrIdx;
	UChar*          m_puhQTTempCbf[3];
	UChar*          m_puhQTTempTransformSkipFlag[3];
	
	Int* m_piYuvExt;
	Int m_iYuvExtStride;
	Int m_iYuvExtHeight;
	
	TComYuv   m_acYuvPred[2];
	TComYuv   m_cYuvPredTemp;
	
	TEncSbac*** m_pppcRDSbacCoder;
	TEncSbac* m_pcRDGoOnSbacCoder;
	DistParam       m_cDistParam;
	
	UInt  m_auiMVPIdxCost[3][3]; //th array bounds
	
};

typedef struct TEncSearch TEncSearch;

struct TComMv
{
	short m_iHor;
	short m_iVer;
};
typedef struct TComMv TComMv;

struct TComRdCost 
{
	Double  m_dLambda;
	double m_sqrtLamda;
	UInt  m_uiLambdaMotionSAD;
	TComMv m_mvPredictor;
	UInt   m_uiCost;
	
};
typedef struct TComRdCost TComRdCost;

struct TEncEntropyIf
{
	
};
typedef struct TEncEntropyIf TEncEntropyIf;

struct TEncEntropy
{
	UInt    m_uiBakAbsPartIdx;
	UInt    m_uiBakChromaOffset;
	UInt    m_bakAbsPartIdxCU;
	TEncEntropyIf*      m_pcEntropyCoderIf;
};
typedef struct TEncEntropy TEncEntropy;

struct TComMvField
{
	TComMv m_acMv;
	Int    m_iRefIdx;
};
typedef struct TComMvField TComMvField;

typedef struct _AMVPInfo
{
	TComMv m_acMvCand[3];  ///< array of motion vector predictor candidates
    Int    iN;                                ///< number of motion vector predictor candidates
}AMVPInfo;

struct TComCUMvField
{
  TComMv*   m_pcMv;
  TComMv*   m_pcMvd;
  Char*     m_piRefIdx;
  UInt      m_uiNumPartition;
  AMVPInfo  m_cAMVPInfo;  
};
typedef struct TComCUMvField TComCUMvField;



struct TEncTop
{
	//TEncSampleAdaptiveOffset m_cEncSAO;                     ///< sample adaptive offset class
	TEncSbac***             m_pppcRDSbacCoder;              ///< temporal storage for RD computation
    TEncSbac                m_cRDGoOnSbacCoder;             ///< going on SBAC model for RD stage
	TComRdCost m_cRdCost;
	TComTrQuant m_cTrQuant;
	TEncEntropy m_cEntropyCoder;
};

typedef struct TEncTop TEncTop;

struct TEncCu
{
	
};
typedef struct TEncCu TEncCu;

enum RefPicList
{
	REF_PIC_LIST_0 = 0,   ///< reference list 0
	REF_PIC_LIST_1 = 1,   ///< reference list 1
	REF_PIC_LIST_X = 100  ///< special mark
};

enum PartSize
{
  SIZE_2Nx2N = 0,           ///< symmetric motion partition,  2Nx2N
  SIZE_2NxN,            ///< symmetric motion partition,  2Nx N
  SIZE_Nx2N,            ///< symmetric motion partition,   Nx2N
  SIZE_NxN,             ///< symmetric motion partition,   Nx N
  SIZE_2NxnU,           ///< asymmetric motion partition, 2Nx( N/2) + 2Nx(3N/2)
  SIZE_2NxnD,           ///< asymmetric motion partition, 2Nx(3N/2) + 2Nx( N/2)
  SIZE_nLx2N,           ///< asymmetric motion partition, ( N/2)x2N + (3N/2)x2N
  SIZE_nRx2N,           ///< asymmetric motion partition, (3N/2)x2N + ( N/2)x2N
  SIZE_NONE = 15
};

enum PredMode
{
  MODE_INTER,           ///< inter-prediction mode
  MODE_INTRA,           ///< intra-prediction mode
  MODE_NONE = 15
};

int g_auiZscanToRaster[256] = {0,1,16,17,2,3,18,19,32,33,48,49,34,35,50,51,
                               4,5,20,21,6,7,22,23,36,37,52,53,38,39,54,55,
							   64,65,80,81,66,67,82,83,96,97,112,113,98,99,114,115,
							   68,69,84,85,70,71,86,87,100,101,116,117,102,103,118,119,
							   8,9,24,25,10,11,26,27,40,41,56,57,42,43,58,59,
							   12,13,28,29,14,15,30,31,44,45,60,61,46,47,62,63,
							   72,73,88,89,74,75,90,91,104,105,120,121,106,107,122,123,
							   76,77,92,93,78,79,94,95,108,109,124,125,110,111,126,127,
							   128,129,144,145,130,131,146,147,160,161,176,177,162,163,178,179,
							   132,133,148,149,134,135,150,151,164,165,180,181,166,167,182,183,
							   192,193,208,209,194,195,210,211,224,225,240,241,226,227,242,243,
							   196,197,212,213,198,199,214,215,228,229,244,245,230,231,246,247,
							   136,137,152,153,138,139,154,155,168,169,184,185,170,171,186,187,
							   140,141,156,157,142,143,158,159,172,173,188,189,174,175,190,191,
							   200,201,216,217,202,203,218,219,232,233,248,249,234,235,250,251,
							   204,205,220,221,206,207,222,223,236,237,252,253,238,239,254,255};
							   
int g_auiZscanToRaster[256] = {0,1,16,17,2,3,18,19,32,33,48,49,34,35,50,51,
                               4,5,20,21,6,7,22,23,36,37,52,53,38,39,54,55,
							   64,65,80,81,66,67,82,83,96,97,112,113,98,99,114,115,
							   68,69,84,85,70,71,86,87,100,101,116,117,102,103,118,119,
							   8,9,24,25,10,11,26,27,40,41,56,57,42,43,58,59,
							   12,13,28,29,14,15,30,31,44,45,60,61,46,47,62,63,
							   72,73,88,89,74,75,90,91,104,105,120,121,106,107,122,123,
							   76,77,92,93,78,79,94,95,108,109,124,125,110,111,126,127,
							   128,129,144,145,130,131,146,147,160,161,176,177,162,163,178,179,
							   132,133,148,149,134,135,150,151,164,165,180,181,166,167,182,183,
							   192,193,208,209,194,195,210,211,224,225,240,241,226,227,242,243,
							   196,197,212,213,198,199,214,215,228,229,244,245,230,231,246,247,
							   136,137,152,153,138,139,154,155,168,169,184,185,170,171,186,187,
							   140,141,156,157,142,143,158,159,172,173,188,189,174,175,190,191,
							   200,201,216,217,202,203,218,219,232,233,248,249,234,235,250,251,
							   204,205,220,221,206,207,222,223,236,237,252,253,238,239,254,255};

							   
int g_auiRasterToPelX[256] = {0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
                              0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							  0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60};
							   
							   
int g_auiRasterToPelY[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                              4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
							  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
							  12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
							  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
							  20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
							  24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
							  28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,
							  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
							  36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,
							  40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,
							  44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
							  48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,
							  52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,
							  56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,
							  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60};
							  
extern TEncTop* m_pcEncTop;
extern TEncCu* m_pcCuEncoder;
extern TComPic* pcPic;	//将pcPic作为外部变量，只能够实现单帧的区块滤波，若要实现多帧，将pcPic作为输入数据，放入In中，可定位到正确图像
//extern TComPicSym* 	pcPicSym = pcPic->m_apcPicSym;
extern TComPicSym* 	pcPicSym;
extern TComDataCU* pcCU;

extern TEncSbac*** m_pppcRDSbacCoder;
extern TEncSbac* m_pcRDGoOnSbacCoder;

//编译成功！！

//2Nx2N模式:亮度和色度
composite estIntraPredQT8x8(output stream<unsigned long ulpcTempCU,unsigned long ulpcOrigYuv, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out, 
                             input stream<unsigned long ulpcOrigYuv>In)
{
	Out = estIntraPred8x8(In)
	{
		UInt uiAbsZorderIdx;
		init
		{
			uiAbsZorderIdx = 0;
		}
		work
		{	
			//创建当前临时CU:rpcTempCU_2Nx2N; ;pcPredYuv;pcRecoYuv;pcResiYuv
			TComDataCU* rpcTempCU_2Nx2N = NULL;
			TComYuv* pcOrigYuv  = (TComYuv*)(In[0].ulpcOrigYuv); //从输入获得
			
			TComYuv *pcPredYuv = NULL,*pcRecoYuv = NULL,*pcResiYuv = NULL;  //原始YUV;2Nx2N模式的临时预测、重构、残余YUV
			
			UInt    uiDepth        = 3;  //当前CU的深度，0
			UInt    uiCurrPartNumb = 4;  //getNumPartitions()
			
			UInt    uiNumPU        = 1;  // 当前CU的分割模式（(SIZE_2Nx2N:1, SIZE_2NxN:2, SIZE_Nx2N:2, SIZE_NxN:4 ... )）； 1
			UInt    uiInitTrDepth  = 0; //!< 用于计算变换的深度，实际深度为该值+uiDepth  
			UInt    uiWidth        = 8; //!< 当前PU的宽度，如果又分成4个子块，则宽度除以2
			UInt    uiHeight       = 8; //!< 当前PU的高度，如果又分成4个子块，则高度除以2
			UInt    uiQNumParts    = 1;   // 最小的分区是4x4大小的块，这里计算出以该4x4块为单位的分割数，这么做便于计算当前CU的Zorder坐标  
			
			UInt    uiOverallDistY = 0;
			UInt    uiOverallDistC = 0;
			UInt    CandNum = 0;
			Double  CandCostList[ 35 ];
			UInt uiPartOffset = 0;
			UInt uiPU = 0;
			
			//TEncSbac*** m_pppcRDSbacCoder;
			
			//定义TcomTrQuant类型变量
			TComTrQuant m_cTrQuant;
			TComTrQuant* m_pcTrQuant = &m_cTrQuant;
			
			//定义TEncSearch类型变量
			TEncSearch m_cSearch;  //调用构造函数完成对象初始化；由于是在栈上开辟的空间，所以退出该work后自动调用析构函数进行对象析构
			TEncSearch* m_pcPredSearch = &m_cSearch; //获得指针
			
			//定义编码器
			TEncEntropy m_cEntropyCoder;
			
			//定义TComRdCost类型变量
			TComRdCost m_cRdCost;
			TComRdCost* m_pcRdCost = &m_cRdCost;
			
			//m_cTrQuant.init(32,true,true,true,true,0);
			initTComRdCost(m_pcRdCost, &(m_pcEncTop->m_cRdCost));
			initTComTrQuant(m_pcTrQuant, &(m_pcEncTop->m_cTrQuant));
			
			
			//m_pcTrQuant 和 m_pcRdCost均为新创建变量的地址，而不是原m_pcEncTop下的指针
			//initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&(m_pcEncTop->m_cEntropyCoder),m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
			initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&m_cEntropyCoder,m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
			
			createCurCU_cos(pcCU,rpcTempCU_2Nx2N,uiDepth,uiAbsZorderIdx); 
			createCurYuv_cos(pcPredYuv,8,8); createCurYuv_cos(pcRecoYuv,8,8); createCurYuv_cos(pcResiYuv,8,8);//创建原始YUV空间并初始化，注意释放空间
			
			uiAbsZorderIdx += uiCurrPartNumb;  //每处理完一个8x8块，Zorder自增4;
			if(uiAbsZorderIdx == 256) 
			{
				uiAbsZorderIdx = 0;
			}
			
			memset( rpcTempCU_2Nx2N->m_skipFlag,  0, (256>>(2*uiDepth)) );  //在uiDepth等于3时，有4个块
			memset( rpcTempCU_2Nx2N->m_pePartSize,0, (256>>(2*uiDepth)) );  //SIZE_2Nx2N = 0;
			memset( rpcTempCU_2Nx2N->m_pePredMode,1, (256>>(2*uiDepth)) );  //MODE_INTRA = 1
			
			setQPSubParts_cos( rpcTempCU_2Nx2N, 0, uiDepth );
			
			///////////////////////////////////
			//m_pppcRDSbacCoder[uhNextDepth][CI_CURR_BEST]->load(m_pppcRDSbacCoder[uiDepth][CI_CURR_BEST]);
			//////////////////////////////////
			{
				//===== init pattern for luma prediction =====
				bool bAboveAvail = false;
				bool bLeftAvail  = false;
				Int numModesAvailable     = 35; //total number of Intra modes
				UInt uiRdModeList[35];
				Int numModesForFullRD = 8;  //!< MPM数目  
				Int i = 0,modeIdx = 0;
				
				//===== determine set of modes to be tested (using prediction signal only) =====
				//
				//Pel* piOrg         = getLumaAddr_cos( pcOrigYuv,uiPU, uiWidth );
				//Pel* piPred        = getLumaAddr_cos( pcPredYuv,uiPU, uiWidth );  //预测YUV空间需要开辟
				//UInt uiStride      = getStride_cos(pcPredYuv);
				
				Pel* piOrg  = pcOrigYuv->m_apiBufY + ( (uiPU * uiWidth)&(pcOrigYuv->m_iWidth - 1) + (( uiPU * uiWidth )&~( pcOrigYuv->m_iWidth - 1 )) * uiWidth );
				Pel* piPred = pcPredYuv->m_apiBufY + ( (uiPU * uiWidth)&(pcPredYuv->m_iWidth - 1) + (( uiPU * uiWidth )&~( pcOrigYuv->m_iWidth - 1 )) * uiWidth );
				UInt uiStride  = pcPredYuv->m_iWidth;
				
				initAdiPattern_cos(rpcTempCU_2Nx2N,uiPartOffset, uiInitTrDepth, m_pcPredSearch->m_piYuvExt, m_pcPredSearch->m_iYuvExtStride, m_pcPredSearch->m_iYuvExtHeight, bAboveAvail, bLeftAvail);
			
				for(i = 0; i < numModesForFullRD; i++ ) 
				{
					CandCostList[ i ] = MAX_DOUBLE;
				}
				
				for(modeIdx = 0; modeIdx < 35; modeIdx++ )
				{
				  UInt uiMode = modeIdx;
				  UInt uiSad,iModeBits;
				  double cost;
				  predIntraLumaAng_cos(m_pcPredSearch,rpcTempCU_2Nx2N,uiMode,piPred, uiStride, uiWidth, uiHeight, bAboveAvail, bLeftAvail);
				  
				  uiSad = calcHAD_cos(m_pcRdCost,piOrg, uiStride, piPred, uiStride, uiWidth, uiHeight);
				  
				  iModeBits = xModeBitsIntra_cos(m_pcPredSearch,rpcTempCU_2Nx2N, uiMode, uiPU, uiPartOffset, uiDepth, uiInitTrDepth);
				  cost = (double)uiSad + (double)iModeBits * getSqrtLambda_cos(m_pcRdCost);
				  
				  CandNum = xUpdateCandList_cos(m_pcPredSearch,uiMode, cost, numModesForFullRD, uiRdModeList, CandCostList);
				}
				//快速算法
				{
					Int uiPreds[3] = {-1, -1, -1};
					Int iMode = -1;  //!< 如果三个MPMs的前两个相同，则iMode=1，否则iMode=2  
					Int numCand = getIntraDirLumaPredictor_cos( rpcTempCU_2Nx2N,uiPartOffset, uiPreds, &iMode );  //!< 获取亮度帧内预测模式的三个MPMs  ,most probable mode
					Int i = 0,j = 0;
					if( iMode >= 0 )
					{
						numCand = iMode;
					}
				  
					for( j = 0; j < numCand; j++)
					{
						bool mostProbableModeIncluded = false;
						Int mostProbableMode = uiPreds[j];
						
						for( i = 0; i < numModesForFullRD; i++)
						{
						  mostProbableModeIncluded |= (mostProbableMode == uiRdModeList[i]);
						}
						if (!mostProbableModeIncluded)
						{
						  uiRdModeList[numModesForFullRD++] = mostProbableMode;
						}
					}
					
				}
				
				{
					UInt    uiBestPUMode  = 0;   //!< 存放最佳预测模式  
					UInt    uiBestPUDistY = 0;   //!< 存放最佳预测模式对应的亮度失真值 
					UInt    uiBestPUDistC = 0;   //!< 存放最佳预测模式对应的色度失真值 
					Double  dBestPUCost   = MAX_DOUBLE;  //!< 存放最佳预测模式对应的总代价 
					UInt uiMode = 0;
					
					UInt uiOrgMode = 0;  //存放最佳模式
					UInt uiCurrPartNumb =  256>>((uiDepth+uiInitTrDepth)<<1);
					// determine residual for partition
					UInt   uiPUDistY = 0;
					UInt   uiPUDistC = 0;
					Double dPUCost   = 0.0;
					
					for(uiMode = 0; uiMode < numModesForFullRD; uiMode++ )  //!< 遍历存储在uiRdModeList里的模式  
					{
					  // set luma prediction mode
					  UInt uiOrgMode = uiRdModeList[uiMode];
					  //rpcTempCU_2Nx2N->setLumaIntraDirSubParts ( uiOrgMode, uiPartOffset, uiDepth + uiInitTrDepth );
					  
					  // determine residual for partition
					  UInt   uiPUDistY = 0;
					  UInt   uiPUDistC = 0;
					  Double dPUCost   = 0.0;
					  
					  memset(rpcTempCU_2Nx2N->m_puhLumaIntraDir+uiPartOffset, uiOrgMode, sizeof(UChar)*uiCurrPartNumb);
					  
					  // set context models
					  load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
					  
					  
					  //整个亮度预测的函数中，两次调用xRecurIntraCodingQT,其中倒数第二个参数第一次为true，一次为false。该参数控制函数中的bCheckSplit的值，若为true，则TU只进行当前深度的计算，如果该值为false，则TU进行所有深度的计算。

					  xRecurIntraCodingQT_cos(m_pcPredSearch, rpcTempCU_2Nx2N, uiInitTrDepth, uiPartOffset, true, pcOrigYuv, pcPredYuv, pcResiYuv, uiPUDistY, uiPUDistC, true, dPUCost );

					  
					  // check r-d cost
					  if( dPUCost < dBestPUCost )  //!< 更新最佳预测模式相关参数     //Cbf:coded block flag, code block pattern,用于标记亮度、色差的残差系数的额特性，是否有DC、AC
					  {
						uiBestPUMode  = uiOrgMode;
						uiBestPUDistY = uiPUDistY;
						uiBestPUDistC = uiPUDistC;
						dBestPUCost   = dPUCost;
						
						xSetIntraResultQT_cos( m_pcPredSearch,rpcTempCU_2Nx2N, uiInitTrDepth, uiPartOffset, true, pcRecoYuv );    //将最新数据拷贝至重构图像
						
						memcpy(m_pcPredSearch->m_puhQTTempTrIdx,  rpcTempCU_2Nx2N->m_puhTrIdx + uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[0], rpcTempCU_2Nx2N->m_puhCbf[0]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[1], rpcTempCU_2Nx2N->m_puhCbf[1]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[2], rpcTempCU_2Nx2N->m_puhCbf[2]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], rpcTempCU_2Nx2N->m_puhTransformSkip[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcTempCU_2Nx2N->m_puhTransformSkip[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcTempCU_2Nx2N->m_puhTransformSkip[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
					  }
					} // Mode loop
					
				    uiOrgMode = uiBestPUMode;  //!<　设置模式为最佳预测模式  
					
					//rpcTempCU_2Nx2N->setLumaIntraDirSubParts ( uiOrgMode, 0, uiDepth + uiInitTrDepth );
					memset(rpcTempCU_2Nx2N->m_puhLumaIntraDir+uiPartOffset, uiOrgMode, sizeof(UChar)*uiCurrPartNumb);
					
					// set context models
					load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
					  
					
					//! 注意该函数倒数第二个参数，此时为false  
					xRecurIntraCodingQT_cos(m_pcPredSearch, rpcTempCU_2Nx2N, uiInitTrDepth, uiPartOffset, true, pcOrigYuv, pcPredYuv, pcResiYuv, uiPUDistY, uiPUDistC, false, dPUCost );
					  
					// check r-d cost
					if( dPUCost < dBestPUCost )
					{
						uiBestPUMode  = uiOrgMode;
						uiBestPUDistY = uiPUDistY;
						uiBestPUDistC = uiPUDistC;
						dBestPUCost   = dPUCost;
						
						xSetIntraResultQT_cos( m_pcPredSearch,rpcTempCU_2Nx2N, uiInitTrDepth, uiPartOffset, true, pcRecoYuv );    //将最新数据拷贝至重构图像
						
						memcpy(m_pcPredSearch->m_puhQTTempTrIdx,  rpcTempCU_2Nx2N->m_puhTrIdx + uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[0], rpcTempCU_2Nx2N->m_puhCbf[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[1], rpcTempCU_2Nx2N->m_puhCbf[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[2], rpcTempCU_2Nx2N->m_puhCbf[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], rpcTempCU_2Nx2N->m_puhTransformSkip[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcTempCU_2Nx2N->m_puhTransformSkip[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcTempCU_2Nx2N->m_puhTransformSkip[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
					}
					
					//--- update overall distortion ---
					uiOverallDistY += uiBestPUDistY;
					uiOverallDistC += uiBestPUDistC;
    
					//--- update transform index and cbf ---
					memcpy( rpcTempCU_2Nx2N->m_puhTrIdx  + uiPartOffset,  m_pcPredSearch->m_puhQTTempTrIdx,  uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_2Nx2N->m_puhCbf[0]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[0], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_2Nx2N->m_puhCbf[1]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[1], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_2Nx2N->m_puhCbf[2]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[2], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_2Nx2N->m_puhTransformSkip[0]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_2Nx2N->m_puhTransformSkip[1]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_2Nx2N->m_puhTransformSkip[2]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], uiCurrPartNumb * sizeof( UChar ) );
					
				    //=== update PU data ====
					memset(rpcTempCU_2Nx2N->m_puhLumaIntraDir + uiPartOffset, uiBestPUMode, sizeof(UChar)*uiCurrPartNumb);
					copyToPic_cos ( rpcTempCU_2Nx2N,uiDepth, uiPU, uiInitTrDepth );
				
				}			
			}
			
			//===== reset context models =====
			load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
  
			//===== set distortion (rate and r-d costs are determined later) =====
			//ruiDistC                   = uiOverallDistC;
			rpcTempCU_2Nx2N->m_uiTotalDistortion =  uiOverallDistY + uiOverallDistC;
			
			
			copyToPicLuma_cos( pcRecoYuv,rpcTempCU_2Nx2N, rpcTempCU_2Nx2N->m_uiCUAddr, rpcTempCU_2Nx2N->m_uiAbsIdxInLCU);
			
			//亮度预测结束
			//	
			
			//色度预测
			{
				UInt    uiBestMode  = 0;
				UInt    uiBestDist  = 0;
				Double  dBestCost   = MAX_DOUBLE;
	  
				//----- init mode list -----
				UInt  uiMinMode = 0;
				UInt  uiModeList[ 5 ] = {0,26,10,1,36};
				UInt  uiLumaMode = rpcTempCU_2Nx2N->m_puhLumaIntraDir[0];
				UInt  uiMaxMode = 5;
				UInt uiCurrPartNumb = (256>>(uiDepth<<1));
				Int i;
				UInt uiMode;
				for(i = 0; i < 4; i++ )
				{
					if( uiLumaMode == uiModeList[i] )
					{
					  uiModeList[i] = 34; // VER+8 mode
					  break;
					}
				}
				
				//----- check chroma modes -----
				for(uiMode = uiMinMode; uiMode < uiMaxMode; uiMode++ )
				{
					UInt    uiDist = 0;
					UInt    uiBits;
					Double  dCost;
					//----- restore context models -----
					load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
		
					//----- chroma coding -----
					//rpcTempCU_2Nx2N->setChromIntraDirSubParts  ( uiModeList[uiMode], 0, uiDepth );
					memset(rpcTempCU_2Nx2N->m_puhChromaIntraDir, uiModeList[uiMode], sizeof(UChar)*uiCurrPartNumb);
					
					xRecurIntraChromaCodingQT_cos(m_pcPredSearch, rpcTempCU_2Nx2N,  0, 0, pcOrigYuv, pcPredYuv, pcResiYuv, uiDist );
					
					load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
					
					uiBits = xGetIntraBitsQT_cos(m_pcPredSearch, rpcTempCU_2Nx2N, 0, 0, false, true, false );
					dCost  = ((Double)uiDist + (Double)((Int)(m_pcRdCost->m_dLambda * uiBits +.5)));
                    dCost = (Double)(UInt)floor(dCost);
		
					//----- compare -----
					if( dCost < dBestCost )
					{
						dBestCost   = dCost;
						uiBestDist  = uiDist;
						uiBestMode  = uiModeList[uiMode];
						//xSetIntraResultChromaQT( rpcTempCU_2Nx2N, 0, 0, pcRecoYuv );
						xSetIntraResultChromaQT_cos(m_pcPredSearch,rpcTempCU_2Nx2N, 0, 0, pcRecoYuv);
						memcpy( m_pcPredSearch->m_puhQTTempCbf[1], rpcTempCU_2Nx2N->m_puhCbf[1], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( m_pcPredSearch->m_puhQTTempCbf[2], rpcTempCU_2Nx2N->m_puhCbf[2], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcTempCU_2Nx2N->m_puhTransformSkip[1], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcTempCU_2Nx2N->m_puhTransformSkip[2], uiCurrPartNumb * sizeof( UChar ) );
					}
				}
	  
				//----- set data -----
				memcpy( rpcTempCU_2Nx2N->m_puhCbf[1], m_pcPredSearch->m_puhQTTempCbf[1], uiCurrPartNumb * sizeof( UChar ) );
				memcpy( rpcTempCU_2Nx2N->m_puhCbf[2], m_pcPredSearch->m_puhQTTempCbf[2], uiCurrPartNumb * sizeof( UChar ) );
				memcpy( rpcTempCU_2Nx2N->m_puhTransformSkip[1], m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], uiCurrPartNumb * sizeof( UChar ) );
				memcpy( rpcTempCU_2Nx2N->m_puhTransformSkip[2], m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], uiCurrPartNumb * sizeof( UChar ) );
				
				memset(rpcTempCU_2Nx2N->m_puhChromaIntraDir, uiBestMode, sizeof(UChar)*uiCurrPartNumb);
				rpcTempCU_2Nx2N->m_uiTotalDistortion  += uiBestDist - uiOverallDistC;
				  
				//由于我们是采用split的方式进行8x8块的两种模式，合并之后进行两次编码比较(在encodeRelated_8x8中)，在这里各自修复熵编码器的上下文等到join之后的encodeRelated_8x8中，其实只有一次即encodeRelated_8x8中for循环的第一次得到了修复
				//因此，这里修复没有作用
				//----- restore context models -----
				//load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
						
			}
			
			Out[0].ulpcTempCU  =  (unsigned long)rpcTempCU_2Nx2N;
			Out[0].ulpcOrigYuv = In[0].ulpcOrigYuv;  //由于采用split结构，另一分支可能还没执行完成，因此还不能删除原始Yuv对应的指针对应的空间
			Out[0].ulpcPredYuv = (unsigned long)pcPredYuv;
			Out[0].ulpcRecoYuv = (unsigned long)pcRecoYuv;
			Out[0].ulpcResiYuv = (unsigned long)pcResiYuv;
			
		}
		window
		{
			In sliding(1,1);
			Out tumbling(1);
		}
	};
	
}


//8x8块的N*N模式
composite estIntraPredQT8x8_NxN(output stream<unsigned long ulpcTempCU,unsigned long ulpcOrigYuv, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out, 
                                 input stream<unsigned long ulpcOrigYuv>In)
{
	//param int j;
	Out = estIntraPredQT8x8_NxN(In)
	{	
		UInt uiAbsZorderIdx;
		init
		{
			uiAbsZorderIdx = 0;
		}
		work
		{
			//创建当前临时CU:rpcTempCU_NxN; ;pcPredYuv;pcRecoYuv;pcResiYuv
			TComDataCU* rpcTempCU_NxN = NULL;
			TComYuv* pcOrigYuv  = (TComYuv*)(In[0].ulpcOrigYuv); //从输入获得
			TComYuv *pcPredYuv = NULL, *pcRecoYuv = NULL, *pcResiYuv = NULL;  //NxN模式的临时预测、重构、残余YUV
			
			UInt    uiDepth        = 3;  //当前CU的深度，0
			UInt    uiCurrPartNumb = (256>>(uiDepth<<1)); 
			
			UInt    uiNumPU        = 4;  // 当前CU的分割模式(SIZE_NxN:4 ... )
			int   	iNumPUiter = 0;
			UInt    uiInitTrDepth  = 1 ; //!< 用于计算变换的深度，实际深度为该值+uiDepth  
			UInt    uiWidth        = 4; //!< 当前PU的宽度，如果又分成4个子块，则宽度除以2
			UInt    uiHeight       = 4; //!< 当前PU的高度，如果又分成4个子块，则高度除以2
			UInt    uiQNumParts    = 1;   // 最小的分区是4x4大小的块，这里计算出以该4x4块为单位的分割数，这么做便于计算当前CU的Zorder坐标  
			//UInt    uiWidthBit     = pcCU->getIntraSizeIdx(0);
			UInt    uiOverallDistY = 0;
			UInt    uiOverallDistC = 0;
			UInt    CandNum = 0;
			Double  CandCostList[ 35 ];
			UInt uiPartOffset = 0;
			UInt uiPU = 0;
			
			
			//TEncTop* m_pcEncTop = (TEncTop*)(In[0].m_ulTEncTop); //将输入转换成有效指针
			//TEncSbac*** m_pppcRDSbacCoder;
			
			//定义TcomTrQuant类型变量
			TComTrQuant m_cTrQuant;
			TComTrQuant* m_pcTrQuant = &m_cTrQuant;
			
			//定义TEncSearch类型变量
			TEncSearch m_cSearch;  //调用构造函数完成对象初始化；由于是在栈上开辟的空间，所以退出该work后自动调用析构函数进行对象析构
			TEncSearch* m_pcPredSearch = &m_cSearch; //获得指针
			
			//定义编码器
			TEncEntropy m_cEntropyCoder;
			
			//定义TComRdCost类型变量
			TComRdCost m_cRdCost;
			TComRdCost* m_pcRdCost = &m_cRdCost;
			
			//m_cTrQuant.init(32,true,true,true,true,0);
			initTComRdCost(m_pcRdCost, &(m_pcEncTop->m_cRdCost));
			initTComTrQuant(m_pcTrQuant, &(m_pcEncTop->m_cTrQuant));
			//m_pcTrQuant 和 m_pcRdCost均为新创建变量的地址，而不是原m_pcEncTop下的指针
			//initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&(m_pcEncTop->m_cEntropyCoder),m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
			initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&m_cEntropyCoder,m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
			
			createCurCU_cos(pcCU,rpcTempCU_NxN,uiDepth,uiAbsZorderIdx); 
			createCurYuv_cos(pcPredYuv,8,8); createCurYuv_cos(pcRecoYuv,8,8); createCurYuv_cos(pcResiYuv,8,8);//创建原始YUV空间并初始化，注意释放空间
			
			uiAbsZorderIdx += uiCurrPartNumb;  //每处理完一个8x8块，Zorder自增4;
			if(uiAbsZorderIdx == 256)
			{
				uiAbsZorderIdx = 0;
			}
			
			// rpcTempCU->setSkipFlagSubParts( false, 0, uiDepth );
			//rpcTempCU->setPartSizeSubParts( eSize, 0, uiDepth );
            //rpcTempCU->setPredModeSubParts( MODE_INTRA, 0, uiDepth );
			memset( rpcTempCU_NxN->m_skipFlag, 0, 256>>(2*uiDepth) );
			memset( rpcTempCU_NxN->m_pePartSize, 3, 256>>(2*uiDepth) );  //SIZE_NxN = 3 模式
			memset( rpcTempCU_NxN->m_pePredMode,1, 256>>(2*uiDepth) );  // MODE_INTRA = 1;
			
			setQPSubParts_cos( rpcTempCU_NxN, 0, uiDepth );
			
			////////////////////////////////////熵编码器状态加载
			//m_pppcRDSbacCoder[uiDepth][CI_CURR_BEST]->load(m_pppcRDSbacCoder[uiDepth][CI_NEXT_BEST]);
			////////////////////////////////////
			
			//for(uiPU = 0;uiPU < uiNumPU;uiPU++,uiPartOffset += uiQNumParts) 
			//控制循环变量为1; 且iNumPUiter不自增；
			for(iNumPUiter = 0;iNumPUiter < 1;iNumPUiter)  //这种形式的for循环 costream支持否？？
			{
				//===== init pattern for luma prediction =====
				bool bAboveAvail = false;
				bool bLeftAvail  = false;
				Int numModesAvailable     = 35; //total number of Intra modes
				UInt uiRdModeList[35];
				Int numModesForFullRD = 8;  //!< MPM数目  
				Int i = 0,modeIdx = 0;
				//===== determine set of modes to be tested (using prediction signal only) =====
				//
				//Pel* piOrg         = getLumaAddr_cos( pcOrigYuv,uiPU, uiWidth );
				//Pel* piPred        = getLumaAddr_cos( pcPredYuv,uiPU, uiWidth );  //预测YUV空间需要开辟
				//UInt uiStride      = getStride_cos(pcPredYuv);
				
				Pel* piOrg  = pcOrigYuv->m_apiBufY + ( (uiPU * uiWidth)&(pcOrigYuv->m_iWidth - 1) + (( uiPU * uiWidth )&~( pcOrigYuv->m_iWidth - 1 )) * uiWidth );
				Pel* piPred = pcPredYuv->m_apiBufY + ( (uiPU * uiWidth)&(pcPredYuv->m_iWidth - 1) + (( uiPU * uiWidth )&~( pcOrigYuv->m_iWidth - 1 )) * uiWidth );
				UInt uiStride      = pcPredYuv->m_iWidth;
				
				// rpcTempCU_NxN->getPattern()->initPattern   ( rpcTempCU_NxN, uiInitTrDepth, uiPartOffset );
				// set luma parameters from CU data for accessing ADI data //!< 主要获取当前PU的邻域可用性，对参考样点进行设置及滤波  
				// rpcTempCU_NxN->getPattern()->initAdiPattern( rpcTempCU_NxN, uiPartOffset, uiInitTrDepth, m_piYuvExt, m_iYuvExtStride, m_iYuvExtHeight, bAboveAvail, bLeftAvail );
				initAdiPattern_cos(rpcTempCU_NxN,uiPartOffset, uiInitTrDepth, m_pcPredSearch->m_piYuvExt, m_pcPredSearch->m_iYuvExtStride, m_pcPredSearch->m_iYuvExtHeight, bAboveAvail, bLeftAvail);
			
				for(i = 0; i < numModesForFullRD; i++ ) 
				{
					CandCostList[ i ] = MAX_DOUBLE;
				}
				
				for(modeIdx = 0; modeIdx < 35; modeIdx++ )
				{
				  UInt uiMode = modeIdx;
				  UInt uiSad,iModeBits;
				  double cost;
				  predIntraLumaAng_cos(m_pcPredSearch,rpcTempCU_NxN,uiMode,piPred, uiStride, uiWidth, uiHeight, bAboveAvail, bLeftAvail);
				  
				  uiSad = calcHAD_cos(m_pcRdCost,piOrg, uiStride, piPred, uiStride, uiWidth, uiHeight);
				  
				  iModeBits = xModeBitsIntra_cos(m_pcPredSearch,rpcTempCU_NxN, uiMode, uiPU, uiPartOffset, uiDepth, uiInitTrDepth);
				  cost = (double)uiSad + (double)iModeBits * getSqrtLambda_cos(m_pcRdCost);
				  
				  CandNum = xUpdateCandList_cos(m_pcPredSearch,uiMode, cost, numModesForFullRD, uiRdModeList, CandCostList);
				}
				//快速算法
				{
					Int uiPreds[3] = {-1, -1, -1};
					Int iMode = -1;  //!< 如果三个MPMs的前两个相同，则iMode=1，否则iMode=2  
					Int numCand = getIntraDirLumaPredictor_cos( rpcTempCU_NxN,uiPartOffset, uiPreds, &iMode );  //!< 获取亮度帧内预测模式的三个MPMs  ,most probable mode
					Int i = 0,j = 0;
					if( iMode >= 0 )
					{
						numCand = iMode;
					}
				  
					for( j = 0; j < numCand; j++)
					{
						bool mostProbableModeIncluded = false;
						Int mostProbableMode = uiPreds[j];
						
						for( i = 0; i < numModesForFullRD; i++)
						{
						  mostProbableModeIncluded |= (mostProbableMode == uiRdModeList[i]);
						}
						if (!mostProbableModeIncluded)
						{
						  uiRdModeList[numModesForFullRD++] = mostProbableMode;
						}
					}
					
				}
				
				{
					UInt    uiBestPUMode  = 0;   //!< 存放最佳预测模式  
					UInt    uiBestPUDistY = 0;   //!< 存放最佳预测模式对应的亮度失真值 
					UInt    uiBestPUDistC = 0;   //!< 存放最佳预测模式对应的色度失真值 
					Double  dBestPUCost   = MAX_DOUBLE;  //!< 存放最佳预测模式对应的总代价 
					UInt    uiMode = 0;
					
					UInt uiOrgMode = 0;  //存放最佳模式
					UInt uiCurrPartNumb =  256>>((uiDepth+uiInitTrDepth)<<1);
					// determine residual for partition
					UInt   uiPUDistY = 0;
					UInt   uiPUDistC = 0;
					Double dPUCost   = 0.0;
					
					for(uiMode = 0; uiMode < numModesForFullRD; uiMode++ )  //!< 遍历存储在uiRdModeList里的模式  
					{
					  // set luma prediction mode
					  UInt uiOrgMode = uiRdModeList[uiMode];
					  //rpcTempCU_NxN->setLumaIntraDirSubParts ( uiOrgMode, uiPartOffset, uiDepth + uiInitTrDepth );
					  
					  // determine residual for partition
					  UInt   uiPUDistY = 0;
					  UInt   uiPUDistC = 0;
					  Double dPUCost   = 0.0;
					  
					  memset(rpcTempCU_NxN->m_puhLumaIntraDir+uiPartOffset, uiOrgMode, sizeof(UChar)*uiCurrPartNumb);
					  
					  // set context models
					  load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
					  
					  
					  //整个亮度预测的函数中，两次调用xRecurIntraCodingQT,其中倒数第二个参数第一次为true，一次为false。该参数控制函数中的bCheckSplit的值，若为true，则TU只进行当前深度的计算，如果该值为false，则TU进行所有深度的计算。

					  xRecurIntraCodingQT_cos(m_pcPredSearch, rpcTempCU_NxN, uiInitTrDepth, uiPartOffset, true, pcOrigYuv, pcPredYuv, pcResiYuv, uiPUDistY, uiPUDistC, true, dPUCost );

					  
					  // check r-d cost
					  if( dPUCost < dBestPUCost )  //!< 更新最佳预测模式相关参数     //Cbf:coded block flag, code block pattern,用于标记亮度、色差的残差系数的额特性，是否有DC、AC
					  {
						uiBestPUMode  = uiOrgMode;
						uiBestPUDistY = uiPUDistY;
						uiBestPUDistC = uiPUDistC;
						dBestPUCost   = dPUCost;
						
						xSetIntraResultQT_cos( m_pcPredSearch,rpcTempCU_NxN, uiInitTrDepth, uiPartOffset, true, pcRecoYuv );    //将最新数据拷贝至重构图像
						
						memcpy(m_pcPredSearch->m_puhQTTempTrIdx,  rpcTempCU_NxN->m_puhTrIdx  + uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[0], rpcTempCU_NxN->m_puhCbf[0] + uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[1], rpcTempCU_NxN->m_puhCbf[1] + uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[2], rpcTempCU_NxN->m_puhCbf[2] + uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], rpcTempCU_NxN->m_puhTransformSkip[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcTempCU_NxN->m_puhTransformSkip[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcTempCU_NxN->m_puhTransformSkip[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
					  }
					} // Mode loop
					
				    uiOrgMode = uiBestPUMode;  //!<　设置模式为最佳预测模式  
					
					//rpcTempCU_NxN->setLumaIntraDirSubParts ( uiOrgMode, 0, uiDepth + uiInitTrDepth );
					memset(rpcTempCU_NxN->m_puhLumaIntraDir+uiPartOffset, uiOrgMode, sizeof(UChar)*uiCurrPartNumb);
					
					// set context models
					load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
					  
					
					//! 注意该函数倒数第二个参数，此时为false  
					xRecurIntraCodingQT_cos(m_pcPredSearch, rpcTempCU_NxN, uiInitTrDepth, uiPartOffset, true, pcOrigYuv, pcPredYuv, pcResiYuv, uiPUDistY, uiPUDistC, false, dPUCost );
					  
					// check r-d cost
					if( dPUCost < dBestPUCost )
					{
						uiBestPUMode  = uiOrgMode;
						uiBestPUDistY = uiPUDistY;
						uiBestPUDistC = uiPUDistC;
						dBestPUCost   = dPUCost;
						
						xSetIntraResultQT_cos( m_pcPredSearch,rpcTempCU_NxN, uiInitTrDepth, uiPartOffset, true, pcRecoYuv );    //将最新数据拷贝至重构图像
						
						memcpy(m_pcPredSearch->m_puhQTTempTrIdx,  rpcTempCU_NxN->m_puhTrIdx + uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[0], rpcTempCU_NxN->m_puhCbf[0]+ uiPartOffset, uiCurrPartNumb* sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[1], rpcTempCU_NxN->m_puhCbf[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempCbf[2], rpcTempCU_NxN->m_puhCbf[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], rpcTempCU_NxN->m_puhTransformSkip[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcTempCU_NxN->m_puhTransformSkip[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcTempCU_NxN->m_puhTransformSkip[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
					}
					
					//--- update overall distortion ---
					uiOverallDistY += uiBestPUDistY;
					uiOverallDistC += uiBestPUDistC;
    
					//--- update transform index and cbf ---
					memcpy( rpcTempCU_NxN->m_puhTrIdx  + uiPartOffset, m_pcPredSearch->m_puhQTTempTrIdx,  uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_NxN->m_puhCbf[0]+ uiPartOffset, m_pcPredSearch->m_puhQTTempCbf[0], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_NxN->m_puhCbf[1]+ uiPartOffset, m_pcPredSearch->m_puhQTTempCbf[1], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_NxN->m_puhCbf[2]+ uiPartOffset, m_pcPredSearch->m_puhQTTempCbf[2], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_NxN->m_puhTransformSkip[0]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_NxN->m_puhTransformSkip[1]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcTempCU_NxN->m_puhTransformSkip[2]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], uiCurrPartNumb * sizeof( UChar ) );
					
				    
					//--- set reconstruction for next intra prediction blocks ---
					if( uiPU != uiNumPU - 1 )
					{
					  UInt    uiZOrder      = rpcTempCU_NxN->m_uiAbsIdxInLCU + uiPartOffset;
					  Pel*    piDes         = getLumaAddrPicYuv_cos(rpcTempCU_NxN,uiZOrder);
					  UInt    uiDesStride   = rpcTempCU_NxN->m_pcPic->m_apcPicYuv[1]->m_iPicWidth+160;  //336
					  Pel*    piSrc         = getLumaAddrYuv_cos(pcRecoYuv,uiPartOffset );
					  UInt    uiSrcStride   = pcRecoYuv->m_iWidth;
					  UInt uiY;
					  UInt uiX;
					  for(uiY = 0; uiY < uiHeight; uiY++, piSrc += uiSrcStride, piDes += uiDesStride )
					  {
						for(uiX = 0; uiX < uiWidth; uiX++ )
						{
						  piDes[ uiX ] = piSrc[ uiX ];
						}
					  }
					  
					}
					
					//=== update PU data ====
					memset(rpcTempCU_NxN->m_puhLumaIntraDir+ uiPartOffset, uiBestPUMode, sizeof(UChar)*uiCurrPartNumb);
					copyToPic_cos ( rpcTempCU_NxN,uiDepth, uiPU, uiInitTrDepth );
				
				}	

				uiPU++;  //每一次循环，uiPU自增；
				uiPartOffset += uiQNumParts;
				if( uiPU == uiNumPU) 
				{
					iNumPUiter++;  //控制循环结束
				}
			}
			
			//if(uiNumPU > 1)
			{
				// set Cbf for all blocks
				UInt uiCombCbfY = 0;
				UInt uiCombCbfU = 0;
				UInt uiCombCbfV = 0;
				UInt uiPartIdx  = 0;
				UInt uiPart;
				UInt uiOffs;
				for( uiPart = 0; uiPart < 4; uiPart++, uiPartIdx += uiQNumParts )
				{
					uiCombCbfY |= ((rpcTempCU_NxN->m_puhCbf[0][uiPartIdx]>>1)&0x1);
					uiCombCbfU |= ((rpcTempCU_NxN->m_puhCbf[1][uiPartIdx]>>1)&0x1);
					uiCombCbfV |= ((rpcTempCU_NxN->m_puhCbf[2][uiPartIdx]>>1)&0x1);
				}
				for( uiOffs = 0; uiOffs < 4 * uiQNumParts; uiOffs++ )
				{
				  rpcTempCU_NxN->m_puhCbf[0][uiOffs] |= uiCombCbfY;
				  rpcTempCU_NxN->m_puhCbf[1][uiOffs] |= uiCombCbfU;
				  rpcTempCU_NxN->m_puhCbf[2][uiOffs] |= uiCombCbfV;
				}	
			}
			
			//===== reset context models =====
			load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
  
			//===== set distortion (rate and r-d costs are determined later) =====
			//ruiDistC                   = uiOverallDistC;
			rpcTempCU_NxN->m_uiTotalDistortion =  uiOverallDistY + uiOverallDistC;
			
			
			copyToPicLuma_cos( pcRecoYuv,rpcTempCU_NxN, rpcTempCU_NxN->m_uiCUAddr, rpcTempCU_NxN->m_uiAbsIdxInLCU);
			
			//亮度预测结束
			//	
			
			//色度预测
			{
				UInt    uiBestMode  = 0;
				UInt    uiBestDist  = 0;
				Double  dBestCost   = MAX_DOUBLE;
	  
				//----- init mode list -----
				UInt  uiMinMode = 0;
				UInt  uiModeList[ 5 ] = {0,26,10,1,36};
				UInt  uiLumaMode = rpcTempCU_NxN->m_puhLumaIntraDir[0];
				UInt  uiMaxMode = 5;
				UInt  uiCurrPartNumb = (256>>(uiDepth<<1));
				Int i;
				UInt uiMode;
				for(i = 0; i < 4; i++ )
				{
					if( uiLumaMode == uiModeList[i] )
					{
					  uiModeList[i] = 34; // VER+8 mode
					  break;
					}
				}
				
				//----- check chroma modes -----
				for(uiMode = uiMinMode; uiMode < uiMaxMode; uiMode++ )
				{
					UInt    uiDist = 0;
					UInt    uiBits;
					Double  dCost;
					//----- restore context models -----
					load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
		
					//----- chroma coding -----
					//rpcTempCU_NxN->setChromIntraDirSubParts  ( uiModeList[uiMode], 0, uiDepth );
					memset(rpcTempCU_NxN->m_puhChromaIntraDir, uiModeList[uiMode], sizeof(UChar)*uiCurrPartNumb);
					
					xRecurIntraChromaCodingQT_cos(m_pcPredSearch, rpcTempCU_NxN,  0, 0, pcOrigYuv, pcPredYuv, pcResiYuv, uiDist );
					
					load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
					
					uiBits = xGetIntraBitsQT_cos(m_pcPredSearch, rpcTempCU_NxN, 0, 0, false, true, false );
					dCost  = ((Double)uiDist + (Double)((Int)(m_pcRdCost->m_dLambda * uiBits +.5)));
                    dCost = (Double)(UInt)floor(dCost);
		
					//----- compare -----
					if( dCost < dBestCost )
					{
						dBestCost   = dCost;
						uiBestDist  = uiDist;
						uiBestMode  = uiModeList[uiMode];
						//xSetIntraResultChromaQT( rpcTempCU_NxN, 0, 0, pcRecoYuv );
						xSetIntraResultChromaQT_cos(m_pcPredSearch,rpcTempCU_NxN, 0, 0, pcRecoYuv);
						memcpy( m_pcPredSearch->m_puhQTTempCbf[1], rpcTempCU_NxN->m_puhCbf[1], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( m_pcPredSearch->m_puhQTTempCbf[2], rpcTempCU_NxN->m_puhCbf[2], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcTempCU_NxN->m_puhTransformSkip[1], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcTempCU_NxN->m_puhTransformSkip[2], uiCurrPartNumb * sizeof( UChar ) );
					}
				}
	  
				//----- set data -----
				memcpy( rpcTempCU_NxN->m_puhCbf[1], m_pcPredSearch->m_puhQTTempCbf[1], uiCurrPartNumb * sizeof( UChar ) );
				memcpy( rpcTempCU_NxN->m_puhCbf[2], m_pcPredSearch->m_puhQTTempCbf[2], uiCurrPartNumb * sizeof( UChar ) );
				memcpy( rpcTempCU_NxN->m_puhTransformSkip[1], m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], uiCurrPartNumb * sizeof( UChar ) );
				memcpy( rpcTempCU_NxN->m_puhTransformSkip[2], m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], uiCurrPartNumb * sizeof( UChar ) );
				
				memset(rpcTempCU_NxN->m_puhChromaIntraDir, uiBestMode, sizeof(UChar)*uiCurrPartNumb);
				rpcTempCU_NxN->m_uiTotalDistortion  += uiBestDist - uiOverallDistC;
				  
				//----- restore context models -----//道理同上
				//load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
			}
			
			Out[0].ulpcTempCU  =  (unsigned long)rpcTempCU_NxN;	
			Out[0].ulpcOrigYuv = In[0].ulpcOrigYuv;
			Out[0].ulpcPredYuv = (unsigned long)pcPredYuv;
			Out[0].ulpcRecoYuv = (unsigned long)pcRecoYuv;
			Out[0].ulpcResiYuv = (unsigned long)pcResiYuv;
		}
		window
		{
			In sliding(1,1);
			Out tumbling(1);
		}
		
	};
}



//8x8块帧内模式处理之后的相关编码需单独处理
composite encodeRelated_8x8(output stream<unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out,
                             input stream<unsigned long ulpcTempCU, unsigned long ulpcOrigYuv,unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>In)
{
	Out = encodeRelated_8x8(In)
	{
		int uiPartUnitIdx;  //记录处理的个数
		UInt uiAbsZorderIdx;
		init
		{
			uiPartUnitIdx = 0;
			uiAbsZorderIdx = 0;
		}
		work
		{
			UInt    uiDepth        = 3;  //当前CU的深度，0
			UInt    uiCurrPartNumb = 256>>(uiDepth<<1);  
	
			TComDataCU* rpcTempCU = (TComDataCU*)(In[0].ulpcTempCU);
			TComYuv *pcOrigYuv = (TComYuv *)(In[0].ulpcOrigYuv);
			TComYuv *pcPredYuvTemp = (TComYuv *)(In[0].ulpcPredYuv);  //先用2Nx2N模式下的YUV初始化临时YUV
			TComYuv *pcRecoYuvTemp = (TComYuv *)(In[0].ulpcRecoYuv);
			TComYuv *pcResiYuvTemp = (TComYuv *)(In[0].ulpcResiYuv);
			
			TComDataCU* rpcBestCU = NULL;
			TComYuv *pcPredYuvBest = NULL, *pcRecoYuvBest = NULL, *pcResiYuvBest = NULL;
			
			//定义编码器
			TEncEntropy m_cEntropyCoder;
			TEncEntropy *m_pcEntropyCoder = &m_cEntropyCoder;
			
			//定义TEncSearch类型变量
			TEncSearch m_cSearch;  //调用构造函数完成对象初始化；由于是在栈上开辟的空间，所以退出该work后自动调用析构函数进行对象析构
			TEncSearch* m_pcPredSearch = &m_cSearch; //获得指针
			
			//定义TComRdCost类型变量
			TComRdCost m_cRdCost;
			TComRdCost* m_pcRdCost = &m_cRdCost;
			
			//定义TcomTrQuant类型变量
			TComTrQuant m_cTrQuant;
			TComTrQuant* m_pcTrQuant = &m_cTrQuant;
			
			double dRdCost;
			int i = 0;
			
			initTComRdCost(m_pcRdCost, &(m_pcEncTop->m_cRdCost));
			initTComTrQuant(m_pcTrQuant, &(m_pcEncTop->m_cTrQuant));
			
			//m_pcTrQuant 和 m_pcRdCost均为新创建变量的地址，而不是原m_pcEncTop下的指针
			//initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&(m_pcEncTop->m_cEntropyCoder),m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
			initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&m_cEntropyCoder,m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
			
			
			createCurCU_cos(pcCU,rpcBestCU,uiDepth,uiAbsZorderIdx); //创建新的BestCU
			createCurYuv_cos(pcPredYuvBest,8,8); createCurYuv_cos(pcRecoYuvBest,8,8); createCurYuv_cos(pcResiYuvBest,8,8);//创建原始YUV空间并初始化，注意释放空间
			
			for(i = 0; i < 2; i++)
			{
				//两种模式编码前都需要修复熵编码器的状态
				load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
				//
				
				encodeIntra(m_pcCuEncoder,m_pcEntropyCoder,rpcTempCU,uiDepth);
		  
				rpcTempCU->m_uiTotalBits = getNumberOfWrittenBits_cos(m_pcEntropyCoder);
				rpcTempCU->m_uiTotalBins = getBinsCoded_cos(m_pcEntropyCoder);
			
				dRdCost = ((Double)rpcTempCU->m_uiTotalDistortion + (Double)((Int)(rpcTempCU->m_uiTotalBits * (m_pcRdCost->m_dLambda)+.5)));
				dRdCost = (Double)(UInt)floor(dRdCost);
				rpcTempCU->m_dTotalCost = dRdCost;
		  
				store_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][2]);
				
				//xCheckDQP( rpcTempCU );
				//xCheckBestMode(rpcBestCU, rpcTempCU, uiDepth);
				if(rpcTempCU->m_dTotalCost < rpcBestCU->m_dTotalCost)
				{
					TComYuv* pcYuv;
					// Change Information data
					TComDataCU* pcCU = rpcBestCU;
					rpcBestCU = rpcTempCU;
					rpcTempCU = pcCU;

					// Change Prediction data
					pcYuv = pcPredYuvBest;
					pcPredYuvBest = pcPredYuvTemp;
					pcPredYuvTemp = pcYuv;

					// Change Reconstruction data
					pcYuv = pcRecoYuvBest;
					pcRecoYuvBest = pcRecoYuvTemp;
					pcRecoYuvTemp = pcYuv;

					pcYuv = NULL;
					pcCU  = NULL;
					
					// store temp best CI for next CU coding
					store_cos(m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][2],m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][1]);   
				}
				
				destoryCurCU_cos(rpcTempCU); //删除临时CU单元
				destoryCurYuv_cos(pcPredYuvTemp);
				destoryCurYuv_cos(pcRecoYuvTemp); //删除临时CU开辟的空间;并置指针为NULL，
				destoryCurYuv_cos(pcResiYuvTemp);
				
				if(i == 0) 
				{
					rpcTempCU = (TComDataCU*)(In[1].ulpcTempCU);
					pcPredYuvTemp = (TComYuv *)(In[1].ulpcPredYuv);  //用NxN模式的YUV初始化临时YUV
					pcRecoYuvTemp = (TComYuv *)(In[1].ulpcRecoYuv);
					pcResiYuvTemp = (TComYuv *)(In[1].ulpcResiYuv);
				}
				
			}
			
			destoryCurYuv_cos(pcOrigYuv);  //删除原始Yuv
			//rpcTempCU = NULL;pcPredYuvTemp = NULL;pcRecoYuvTemp = NULL;pcResiYuvTemp = NULL;
			
			//事实上到该for循环结束时CI_NEXT_BEST已获取,即m_pppcRDSbacCoder[3][1]状态已经完成，下一个8x8块可以进行  //通过互斥变量看能否实现
			
			//同时当4个8X8子块均处理时，可以将状态传至上一层，原代码为m_pppcRDSbacCoder[uhNextDepth][CI_NEXT_BEST]->store(m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]);
			if(++uiPartUnitIdx == 4)  
			{
				//m_pppcRDSbacCoder[uiDepth][1]->store(m_pppcRDSbacCoder[uiDepth-1][2]);
				uiPartUnitIdx = 0;
				store_cos(m_pppcRDSbacCoder[uiDepth][1], m_pppcRDSbacCoder[uiDepth-1][0]);
			}
			
			uiAbsZorderIdx += uiCurrPartNumb;  //每处理完一个8x8块，Zorder自增4;
			if(uiAbsZorderIdx == 256) 
			{
				uiAbsZorderIdx = 0;
			}
			
			//编码最佳CU
			resetBits_cos(m_pcEntropyCoder);
			encodeSplitFlag_cos(m_pcEntropyCoder,rpcBestCU, 0, uiDepth, true );  // 当第3号8x8 编码后，4个8x8块拷贝至16x16临时CU中，然后对该临时CU编码，
			rpcBestCU->m_uiTotalBits += getNumberOfWrittenBits_cos(m_pcEntropyCoder); // split bits
			rpcBestCU->m_uiTotalBins += getBinsCoded_cos(m_pcEntropyCoder);
			//rpcBestCU->getTotalCost()  = m_pcRdCost->calcRdCost( rpcBestCU->getTotalBits(), rpcBestCU->getTotalDistortion() );
			dRdCost = ((Double)rpcBestCU->m_uiTotalDistortion + (Double)((Int)(rpcBestCU->m_uiTotalBits * (m_pcRdCost->m_dLambda)+.5)));
			dRdCost = (Double)(UInt)floor(dRdCost);
			rpcBestCU->m_dTotalCost = dRdCost;
				
			copyToPic_cos(rpcBestCU,uiDepth);                                                     // Copy Best data to Picture for next partition prediction.

			xCopyYuv2Pic_cos(m_pcCuEncoder,rpcBestCU,uiDepth);    //至此，一个8x8块的计算全部完成
			
			Out[0].ulpcTempCU = (unsigned long)rpcBestCU;
			Out[0].ulpcPredYuv = (unsigned long)pcPredYuvBest;
			Out[0].ulpcRecoYuv = (unsigned long)pcRecoYuvBest;
			Out[0].ulpcResiYuv = (unsigned long)pcResiYuvBest;
		}
		window
		{
			In sliding(2,2);  // 2Nx2N模式和 NxN模式的join合并后的输入
			Out tumbling(1);
		}
	};
}

composite xCheckRDCostIntra8x8_cos(output stream<unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out, 
                                    input stream<unsigned long ulpcOrigYuv>In)
{	
	stream<unsigned long ulpcTempCU,unsigned long ulpcOrigYuv, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out2;
	Out2 = splitjoin(In)
	{
		int i;
		split duplicate(1);
		for(i=0; i<2; i++)
			if(i == 0 )add estIntraPredQT8x8();  //8x8块的2N*2N模式
		    else add estIntraPredQT8x8_NxN();  //8x8块的N*N模式
		join roundrobin(1);
			
	};
	Out = encodeRelated_8x8(Out2)();
}


//将某一8*8最终的CU状态及YUV拷贝至上一层16*16 CU的相应位置,也即16*16的分割模式
composite copyPartTo_16x16(output stream<unsigned long ulpcTempCU, unsigned long ulpcOrigYuv, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out, 
							input stream<unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>In)
{
	Out = xcopyPartTo_16x16(In)
	{
		int uiPartUnitIdx;
		int uiAbsZorderIdx;  //记录处理的16x16块的起始zorder;
		UChar uiDepth;
		UChar uhWidth;
        UChar uhHeight;
		
		//必须放在外部定义，因为它们对应的空间不是在每次work上都执行
		TComDataCU* rpcTempCU_16x16;  
		TComYuv *pcOrigYuv;		
		TComYuv *pcPredYuvTemp, *pcRecoYuvTemp, *pcResiYuvTemp;
	  
		init 
		{
			uiPartUnitIdx = 0;
			uiAbsZorderIdx = 0;
			uiDepth  = 2;
			uhWidth  = 16;
			uhHeight = 16;
			
			rpcTempCU_16x16 = NULL;
			pcOrigYuv = NULL;  pcPredYuvTemp= NULL;   pcRecoYuvTemp = NULL;   pcResiYuvTemp = NULL;  //16x16块的临时, 预测,重构,残余 YUV
			                   
		}
		work
		{
	        //pcCU为LCU的指针
			
			TComPic* pcPic = pcCU->m_pcPic;
			TComPicYuv* pcPicYuvSrc = pcPic->m_apcPicYuv[0];
			
			int y;
			Pel* pDst, *pDstU, *pDstV;
			Pel* pSrc, *pSrcU, *pSrcV;
			UInt iDstStride, iSrcStride,iWidth,iCWidth;
			
			//TComYuv* m_ppcOrigYuv = NULL;
			//Int iQP = pcCU->m_phQP[0];
			//
			UInt iCuAddr = pcCU->m_uiCUAddr;
			double dRdCost;
			
			TComDataCU* pcSubBestPartCU = (TComDataCU*)(In[0].ulpcTempCU);//待完成	
			TComYuv *pcPredYuvBest_8x8 = (TComYuv*)(In[0].ulpcPredYuv);
			TComYuv *pcRecoYuvBest_8x8 = (TComYuv*)(In[0].ulpcRecoYuv); 
			TComYuv *pcResiYuvBest_8x8 = (TComYuv*)(In[0].ulpcResiYuv);  //根据输入完成
			
			//定义编码器
			//TEncEntropy m_cEntropyCoder;
			//TEncEntropy *m_pcEntropyCoder = &m_cEntropyCoder;
			
			//定义TComRdCost类型变量
			//TComRdCost m_cRdCost;
			//TComRdCost* m_pcRdCost = &m_cRdCost;
			TEncEntropy* m_pcEntropyCoder =  &(m_pcEncTop->m_cEntropyCoder);
			
			TComRdCost* m_pcRdCost = &(m_pcEncTop->m_cRdCost);
			
			if(uiPartUnitIdx== 0)  //说明是在处理一个新的16x16块
			{	
				//createCurCU_cos(pcCU,rpcBestCU_16x16,uiDepth,uiAbsZorderIdx);  //创建最佳CU空间并初始化 , 注意使用完毕应要销毁空间
				createCurCU_cos(pcCU,rpcTempCU_16x16,uiDepth,uiAbsZorderIdx); 
				createCurYuv_cos(pcOrigYuv,16,16);  //创建原始YUV空间并初始化，注意释放空间
				createCurYuv_cos(pcPredYuvTemp,16,16);   
				
				createCurYuv_cos(pcRecoYuvTemp,16,16);  
				
				createCurYuv_cos(pcResiYuvTemp,16,16);  
				
				//pcOrigYuv 原始yuv数据拷贝
				//等价代码：m_ppcOrigYuv[2]->copyFromPicYuv( pcPic->getPicYuvOrg(), rpcBestCU->getAddr(), rpcBestCU->getZorderIdxInCU() );
			   {
					int  y;
					short* pDst  = pcOrigYuv->m_apiBufY; 
					short* pDstU = pcOrigYuv->m_apiBufU;
					short* pDstV = pcOrigYuv->m_apiBufV;
					//Pel* pSrc     = pcPicYuvSrc->getLumaAddr ( iCuAddr, uiAbsZorderIdx );
					Pel* pSrc    = pcPicYuvSrc->m_piPicOrgY + pcPicYuvSrc->m_cuOffsetY[iCuAddr] + pcPicYuvSrc->m_buOffsetY[g_auiZscanToRaster[uiAbsZorderIdx]];
					Pel* pSrcU   = pcPicYuvSrc->m_piPicOrgU + pcPicYuvSrc->m_buOffsetC[iCuAddr] + pcPicYuvSrc->m_buOffsetC[g_auiZscanToRaster[uiAbsZorderIdx]];
					Pel* pSrcV   = pcPicYuvSrc->m_piPicOrgV + pcPicYuvSrc->m_buOffsetC[iCuAddr] + pcPicYuvSrc->m_buOffsetC[g_auiZscanToRaster[uiAbsZorderIdx]];
					
					//copyFromPicLuma
					UInt  iDstStride  =  pcOrigYuv->m_iWidth;
					UInt  iSrcStride  = pcPicYuvSrc->m_iPicWidth + ((pcPicYuvSrc->m_iLumaMarginX)<<1);
					UInt  iWidth = pcOrigYuv->m_iWidth;
					UInt  iCWidth = pcOrigYuv->m_iCWidth;
					for ( y = pcOrigYuv->m_iHeight; y != 0; y-- )
					{
						memcpy( pDst, pSrc, sizeof(short)*iWidth);
						pDst += iDstStride;
						pSrc += iSrcStride;
					}
					
					//copyFromPicChroma
					iDstStride = pcOrigYuv->m_iCWidth;
					iSrcStride =  (pcPicYuvSrc->m_iPicWidth)>>1 + ((pcPicYuvSrc->m_iChromaMarginX)<<1);
					for ( y = pcOrigYuv->m_iCHeight; y != 0; y-- )
					{
						memcpy( pDstU, pSrcU, sizeof(Pel)*(iCWidth) );
						memcpy( pDstV, pSrcV, sizeof(Pel)*(iCWidth) );
						pSrcU += iSrcStride;
						pSrcV += iSrcStride;
						pDstU += iDstStride;
						pDstV += iDstStride;
					} 				
				}
				
				uiAbsZorderIdx += 16;  //每处理完一个16x16块，Zorder自增16;
			}
			
			//pcSubBestPartCU为第uiPartUnitIdx个8x8子块对应的最佳CU；需要根据输入转化为对应的指针
			//rpcTempCU_16x16->copyPartFrom( pcSubBestPartCU, uiPartUnitIdx, uiDepth+1 );         // Keep best part data to current temporary data.
			copyPartFrom_cos(rpcTempCU_16x16,pcSubBestPartCU,uiPartUnitIdx,uiDepth+1);
			
			//xCopyYuv2Tmp( pcSubBestPartCU->getTotalNumPart()*uiPartUnitIdx, uiDepth+1 ); 执行：pcRecoYuvBest[uiDepth+1]->copyToPartYuv( pcRecoYuvTemp[uiDepth], pcSubBestPartCU->getTotalNumPart()*uiPartUnitIdx );
			{
				
				//计算第uiPartUnitIdx个8x8子块在16x16块中对应位置
				UInt uiDstPartIdx = 4*uiPartUnitIdx; // pcSubBestPartCU->getTotalNumPart()*uiPartUnitIdx
				
				pSrc  = pcRecoYuvBest_8x8->m_apiBufY;
				pDst  =     pcRecoYuvTemp->m_apiBufY + ( g_auiRasterToPelX[g_auiZscanToRaster[uiDstPartIdx]] + g_auiRasterToPelY[g_auiZscanToRaster[uiDstPartIdx]]* pcRecoYuvTemp->m_iWidth );
				
				pSrcU = pcRecoYuvBest_8x8->m_apiBufU;
				pSrcV = pcRecoYuvBest_8x8->m_apiBufV;
				pDstU =     pcRecoYuvTemp->m_apiBufU + ((g_auiRasterToPelX[g_auiZscanToRaster[uiDstPartIdx]] + g_auiRasterToPelY[g_auiZscanToRaster[uiDstPartIdx]]* pcRecoYuvTemp->m_iCWidth)>>1);
				pDstV =     pcRecoYuvTemp->m_apiBufV + ((g_auiRasterToPelX[g_auiZscanToRaster[uiDstPartIdx]] + g_auiRasterToPelY[g_auiZscanToRaster[uiDstPartIdx]]* pcRecoYuvTemp->m_iCWidth)>>1);
  
				iSrcStride = pcRecoYuvBest_8x8->m_iWidth;
				iDstStride =     pcRecoYuvTemp->m_iWidth;
				
				y = pcRecoYuvBest_8x8->m_iHeight;
				for( y; y!=0; y-- )
				{
					memcpy(pDst, pSrc, sizeof(Pel)*iSrcStride);
					pDst += iDstStride;
					pSrc += iSrcStride;
				}
				
				iSrcStride = pcRecoYuvBest_8x8->m_iCWidth;
				iDstStride =     pcRecoYuvTemp->m_iCWidth;
				
				y = pcRecoYuvBest_8x8->m_iCHeight;
				for ( y; y != 0; y-- )
				{
					memcpy( pDstU, pSrcU, sizeof(Pel)*(iSrcStride) );
					memcpy( pDstV, pSrcV, sizeof(Pel)*(iSrcStride) );
					pSrcU += iSrcStride;
					pSrcV += iSrcStride;
					pDstU += iDstStride;
					pDstV += iDstStride;
				}	
			}
			
			//最佳8x8块已拷贝结束，可销毁CU和YUV空间
			destoryCurCU_cos(pcSubBestPartCU);
			destoryCurYuv_cos(pcPredYuvBest_8x8);
			destoryCurYuv_cos(pcRecoYuvBest_8x8);
			destoryCurYuv_cos(pcResiYuvBest_8x8);
			
			if(++uiPartUnitIdx == 4) //4个8x8块均接收到，对临时最佳16x16块编码
			{
				uiPartUnitIdx = 0; //重置0，用于标记下一个16x16块的开始
				
				/*m_pcEntropyCoder->resetBits();  //这里的熵编码器的上一个状态是encodeRelated_8x8中第3号8x8进行encodeSplitFlag_cos编码结束之后的状态
				m_pcEntropyCoder->encodeSplitFlag( rpcTempCU_16x16, 0, uiDepth, true );

				rpcTempCU_16x16->getTotalBits() += m_pcEntropyCoder->getNumberOfWrittenBits(); // split bits
				rpcTempCU_16x16->getTotalBins() += ((TEncBinCABAC *)((TEncSbac*)m_pcEntropyCoder->m_pcEntropyCoderIf)->getEncBinIf())->getBinsCoded();
				rpcTempCU_16x16->getTotalCost()  = m_pcRdCost->calcRdCost( rpcTempCU_16x16->getTotalBits(), rpcTempCU_16x16->getTotalDistortion() );
			    */
				resetBits_cos(m_pcEntropyCoder);
				encodeSplitFlag_cos(m_pcEntropyCoder,rpcTempCU_16x16, 0, uiDepth, true );  // 当第3号8x8 编码后，4个8x8块拷贝至16x16临时CU中，然后对该临时CU编码，
				rpcTempCU_16x16->m_uiTotalBits += getNumberOfWrittenBits_cos(m_pcEntropyCoder); // split bits
				rpcTempCU_16x16->m_uiTotalBins += getBinsCoded_cos(m_pcEntropyCoder);
				//rpcBestCU->getTotalCost()  = m_pcRdCost->calcRdCost( rpcTempCU_16x16->getTotalBits(), rpcTempCU_16x16->getTotalDistortion() );
				dRdCost = ((Double)rpcTempCU_16x16->m_uiTotalDistortion + (Double)((Int)(rpcTempCU_16x16->m_uiTotalBits * (m_pcRdCost->m_dLambda)+.5)));
				dRdCost = (Double)(UInt)floor(dRdCost);
				rpcTempCU_16x16->m_dTotalCost = dRdCost;
				
				//m_pppcRDSbacCoder[uiDepth+1][CI_NEXT_BEST]->store(m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]);
				
				//xCheckBestMode( rpcBestCU_16x16, rpcTempCU_16x16, uiDepth);  //保存至最佳rpcBestCU_16x16
			}
			
			Out[0].ulpcTempCU = (unsigned long)rpcTempCU_16x16;
			Out[0].ulpcOrigYuv = (unsigned long)pcOrigYuv;
			Out[0].ulpcPredYuv = (unsigned long)pcPredYuvTemp;
			Out[0].ulpcRecoYuv = (unsigned long)pcRecoYuvTemp;
			Out[0].ulpcResiYuv = (unsigned long)pcResiYuvTemp;
		}
		window
		{
			In sliding(1,1);  //输入一个最佳的8x8并拷贝至临时16x16
			Out tumbling(1);  
		}	
	};
}

//16x16块的2Nx2N模式
composite xCheckRDCostIntra16x16_cos(output stream<unsigned long ulpcBestCU, unsigned long ulpcPredYuvBest, unsigned long ulpcRecoYuvBest, unsigned long ulpcResiYuvBest,
                                                   unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out,  
									  input stream<unsigned long ulpcTempCU, unsigned long ulpcOrigYuv, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>In)
{
	Out = xCheckRDCostIntra16x16(In)
	{
		int iCount = 0;
		int uiPartUnitIdx;
		int uiAbsZorderIdx;  //记录处理的16x16块的起始zorder;
		UChar uiDepth;  //当前CU的深度
		init 
		{
			iCount = 0;
			uiPartUnitIdx = 0;
			uiAbsZorderIdx = 0;
			uiDepth  = 2;	
		}
		work
		{
			//当输入个数满足4个时才开始执行
			if(++iCount == 4)
			{
				TComDataCU* rpcBestCU_16x16 = NULL;
				TComYuv *pcPredYuv= NULL, *pcRecoYuv = NULL, *pcResiYuv = NULL;  // 2Nx2N模式对应的最佳预测，重构和残余Yuv
				//不加Best是为了和亮度和色度预测代码中各Yuv名称保持一致
			
				TComYuv *pcOrigYuv = (TComYuv *)(In[0].ulpcOrigYuv); //事实上 In[0]~In[3]中数据是一样的
				
				UInt    uiCurrPartNumb = 256>>(uiDepth<<1);
					
				UInt    uiNumPU        = 1;  // 当前CU的分割模式（(SIZE_2Nx2N:1
				UInt    uiInitTrDepth  = 0 ; //!< 用于计算变换的深度，实际深度为该值+uiDepth  
				UInt    uiWidth = 16;  //!< 当前PU的宽度
				UInt    uiHeight = 16; //!< 当前PU的高度
				UInt    uiQNumParts    = 4;     
		
				UInt    uiOverallDistY = 0;
				UInt    uiOverallDistC = 0;
				UInt    CandNum = 0;
				Double  CandCostList[35];
				UInt uiPartOffset = 0;
				UInt uiPU = 0;
				
				//TEncSbac*** m_pppcRDSbacCoder;
				
				//定义TcomTrQuant类型变量
				TComTrQuant m_cTrQuant;
				TComTrQuant* m_pcTrQuant = &m_cTrQuant;
				
				//定义TEncSearch类型变量
				TEncSearch m_cSearch;  //调用构造函数完成对象初始化；由于是在栈上开辟的空间，所以退出该work后自动调用析构函数进行对象析构
				TEncSearch* m_pcPredSearch = &m_cSearch; //获得指针
				
				//定义编码器
				TEncEntropy m_cEntropyCoder;
				TEncEntropy *m_pcEntropyCoder = &m_cEntropyCoder;
				
				//定义TComRdCost类型变量
				TComRdCost m_cRdCost;
				TComRdCost* m_pcRdCost = &m_cRdCost;
				
				//m_cTrQuant.init(32,true,true,true,true,0);
				initTComRdCost(m_pcRdCost, &(m_pcEncTop->m_cRdCost));
				initTComTrQuant(m_pcTrQuant, &(m_pcEncTop->m_cTrQuant));
				//m_pcTrQuant 和 m_pcRdCost均为新创建变量的地址，而不是原m_pcEncTop下的指针
				//initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&(m_pcEncTop->m_cEntropyCoder),m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
				initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&m_cEntropyCoder,m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
				
				createCurCU_cos(pcCU,rpcBestCU_16x16,uiDepth,uiAbsZorderIdx);
				createCurYuv_cos(pcPredYuv,16,16);//创建原始YUV空间并初始化，注意释放空间
				createCurYuv_cos(pcRecoYuv,16,16);//创建原始YUV空间并初始化，注意释放空间
				createCurYuv_cos(pcResiYuv,16,16);//创建原始YUV空间并初始化，注意释放空间
				
				uiAbsZorderIdx += uiCurrPartNumb;  //每处理完一个8x8块，Zorder自增4;
				if(uiAbsZorderIdx == 256) 
				{
					uiAbsZorderIdx = 0;
				}
				
				// rpcTempCU->setSkipFlagSubParts( false, 0, uiDepth );
				//rpcTempCU->setPartSizeSubParts( eSize, 0, uiDepth );
				//rpcTempCU->setPredModeSubParts( MODE_INTRA, 0, uiDepth );
				memset( rpcBestCU_16x16->m_skipFlag, 0, 256>>(2*uiDepth) );  //在uiDepth等于3时，有4个块
				memset( rpcBestCU_16x16->m_pePartSize,0, 256>>(2*uiDepth) );  // SIZE_2Nx2N = 0;
				memset( rpcBestCU_16x16->m_pePredMode,1, 256>>(2*uiDepth) );  // MODE_INTRA = 1;
				
				setQPSubParts_cos( rpcBestCU_16x16, 0, uiDepth );
				
				///////////////////////////////////
				//m_pppcRDSbacCoder[uhNextDepth][CI_CURR_BEST]->load(m_pppcRDSbacCoder[uiDepth][CI_CURR_BEST]);
				//////////////////////////////////
				
				{
					//===== init pattern for luma prediction =====
					bool bAboveAvail = false;
					bool bLeftAvail  = false;
					Int numModesAvailable     = 35; //total number of Intra modes
					UInt uiRdModeList[35];
					Int numModesForFullRD = 3;  //!< MPM数目  
					Int i = 0,modeIdx = 0;
					//===== determine set of modes to be tested (using prediction signal only) =====
					//
					//Pel* piOrg         = getLumaAddr_cos( pcOrigYuv,uiPU, uiWidth );
					//Pel* piPred        = getLumaAddr_cos( pcPredYuv,uiPU, uiWidth );  //预测YUV空间需要开辟
					//UInt uiStride      = getStride_cos(pcPredYuv);
					Pel* piOrg  = pcOrigYuv->m_apiBufY + ( (uiPU * uiWidth)&(pcOrigYuv->m_iWidth - 1) + (( uiPU * uiWidth )&~( pcOrigYuv->m_iWidth - 1 )) * uiWidth );
					Pel* piPred = pcPredYuv->m_apiBufY + ( (uiPU * uiWidth)&(pcPredYuv->m_iWidth - 1) + (( uiPU * uiWidth )&~( pcOrigYuv->m_iWidth - 1 )) * uiWidth );
					UInt uiStride      = pcPredYuv->m_iWidth;
					
					// rpcBestCU_16x16->getPattern()->initPattern   ( rpcBestCU_16x16, uiInitTrDepth, uiPartOffset );
					// set luma parameters from CU data for accessing ADI data //!< 主要获取当前PU的邻域可用性，对参考样点进行设置及滤波  
					// rpcBestCU_16x16->getPattern()->initAdiPattern( rpcBestCU_16x16, uiPartOffset, uiInitTrDepth, m_piYuvExt, m_iYuvExtStride, m_iYuvExtHeight, bAboveAvail, bLeftAvail );
					initAdiPattern_cos(rpcBestCU_16x16,uiPartOffset, uiInitTrDepth, m_pcPredSearch->m_piYuvExt, m_pcPredSearch->m_iYuvExtStride, m_pcPredSearch->m_iYuvExtHeight, bAboveAvail, bLeftAvail);
				
					for(i = 0; i < numModesForFullRD; i++ ) 
					{
						CandCostList[ i ] = MAX_DOUBLE;
					}
					
					for(modeIdx = 0; modeIdx < 35; modeIdx++ )
					{
					  UInt uiMode = modeIdx;
					  UInt uiSad,iModeBits;
					  double cost;
					  predIntraLumaAng_cos(m_pcPredSearch,rpcBestCU_16x16,uiMode,piPred, uiStride, uiWidth, uiHeight, bAboveAvail, bLeftAvail);
					  
					  uiSad = calcHAD_cos(m_pcRdCost,piOrg, uiStride, piPred, uiStride, uiWidth, uiHeight);
					  
					  iModeBits = xModeBitsIntra_cos(m_pcPredSearch,rpcBestCU_16x16, uiMode, uiPU, uiPartOffset, uiDepth, uiInitTrDepth);
					  cost = (double)uiSad + (double)iModeBits * getSqrtLambda_cos(m_pcRdCost);
					  
					  CandNum = xUpdateCandList_cos(m_pcPredSearch,uiMode, cost, numModesForFullRD, uiRdModeList, CandCostList);
					}
					//快速算法
					{
						Int uiPreds[3] = {-1, -1, -1};
						Int iMode = -1;  //!< 如果三个MPMs的前两个相同，则iMode=1，否则iMode=2  
						Int numCand = getIntraDirLumaPredictor_cos( rpcBestCU_16x16,uiPartOffset, uiPreds, &iMode );  //!< 获取亮度帧内预测模式的三个MPMs  ,most probable mode
						Int i = 0,j = 0;
						if( iMode >= 0 )
						{
							numCand = iMode;
						}
					  
						for( j = 0; j < numCand; j++)
						{
							bool mostProbableModeIncluded = false;
							Int mostProbableMode = uiPreds[j];
							
							for( i = 0; i < numModesForFullRD; i++)
							{
							  mostProbableModeIncluded |= (mostProbableMode == uiRdModeList[i]);
							}
							if (!mostProbableModeIncluded)
							{
							  uiRdModeList[numModesForFullRD++] = mostProbableMode;
							}
						}
						
					}
					
					{
						UInt    uiBestPUMode  = 0;   //!< 存放最佳预测模式  
						UInt    uiBestPUDistY = 0;   //!< 存放最佳预测模式对应的亮度失真值 
						UInt    uiBestPUDistC = 0;   //!< 存放最佳预测模式对应的色度失真值 
						Double  dBestPUCost   = MAX_DOUBLE;  //!< 存放最佳预测模式对应的总代价 
						UInt uiMode = 0;
						
						UInt uiOrgMode = 0;  //存放最佳模式
						UInt uiCurrPartNumb =  256>>((uiDepth+uiInitTrDepth)<<1);
						// determine residual for partition
						UInt   uiPUDistY = 0;
						UInt   uiPUDistC = 0;
						Double dPUCost   = 0.0;
						
						for(uiMode = 0; uiMode < numModesForFullRD; uiMode++ )  //!< 遍历存储在uiRdModeList里的模式  
						{
						  // set luma prediction mode
						  UInt uiOrgMode = uiRdModeList[uiMode];
						  //rpcBestCU_16x16->setLumaIntraDirSubParts ( uiOrgMode, uiPartOffset, uiDepth + uiInitTrDepth );
						  
						  // determine residual for partition
						  UInt   uiPUDistY = 0;
						  UInt   uiPUDistC = 0;
						  Double dPUCost   = 0.0;
						  
						  memset(rpcBestCU_16x16->m_puhLumaIntraDir+uiPartOffset, uiOrgMode, sizeof(UChar)*uiCurrPartNumb);
						  
						  // set context models
						  load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
						  
						  
						  //整个亮度预测的函数中，两次调用xRecurIntraCodingQT,其中倒数第二个参数第一次为true，一次为false。该参数控制函数中的bCheckSplit的值，若为true，则TU只进行当前深度的计算，如果该值为false，则TU进行所有深度的计算。

						  xRecurIntraCodingQT_cos(m_pcPredSearch, rpcBestCU_16x16, uiInitTrDepth, uiPartOffset, true, pcOrigYuv, pcPredYuv, pcResiYuv, uiPUDistY, uiPUDistC, true, dPUCost );

						  
						  // check r-d cost
						  if( dPUCost < dBestPUCost )  //!< 更新最佳预测模式相关参数     //Cbf:coded block flag, code block pattern,用于标记亮度、色差的残差系数的额特性，是否有DC、AC
						  {
							uiBestPUMode  = uiOrgMode;
							uiBestPUDistY = uiPUDistY;
							uiBestPUDistC = uiPUDistC;
							dBestPUCost   = dPUCost;
							
							xSetIntraResultQT_cos( m_pcPredSearch,rpcBestCU_16x16, uiInitTrDepth, uiPartOffset, true, pcRecoYuv );    //将最新数据拷贝至重构图像
							
							memcpy(m_pcPredSearch->m_puhQTTempTrIdx,  rpcBestCU_16x16->m_puhTrIdx + uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[0], rpcBestCU_16x16->m_puhCbf[0]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[1], rpcBestCU_16x16->m_puhCbf[1]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[2], rpcBestCU_16x16->m_puhCbf[2]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], rpcBestCU_16x16->m_puhTransformSkip[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcBestCU_16x16->m_puhTransformSkip[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcBestCU_16x16->m_puhTransformSkip[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						  }
						} // Mode loop
						
						uiOrgMode = uiBestPUMode;  //!<　设置模式为最佳预测模式  
						
						//rpcBestCU_16x16->setLumaIntraDirSubParts ( uiOrgMode, 0, uiDepth + uiInitTrDepth );
						memset(rpcBestCU_16x16->m_puhLumaIntraDir+uiPartOffset, uiOrgMode, sizeof(UChar)*uiCurrPartNumb);
						
						// set context models
						load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
						  
						
						//! 注意该函数倒数第二个参数，此时为false  
						xRecurIntraCodingQT_cos(m_pcPredSearch, rpcBestCU_16x16, uiInitTrDepth, uiPartOffset, true, pcOrigYuv, pcPredYuv, pcResiYuv, uiPUDistY, uiPUDistC, false, dPUCost );
						  
						// check r-d cost
						if( dPUCost < dBestPUCost )
						{
							uiBestPUMode  = uiOrgMode;
							uiBestPUDistY = uiPUDistY;
							uiBestPUDistC = uiPUDistC;
							dBestPUCost   = dPUCost;
							
							xSetIntraResultQT_cos( m_pcPredSearch,rpcBestCU_16x16, uiInitTrDepth, uiPartOffset, true, pcRecoYuv );    //将最新数据拷贝至重构图像
							
							memcpy(m_pcPredSearch->m_puhQTTempTrIdx,  rpcBestCU_16x16->m_puhTrIdx + uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[0], rpcBestCU_16x16->m_puhCbf[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[1], rpcBestCU_16x16->m_puhCbf[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[2], rpcBestCU_16x16->m_puhCbf[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], rpcBestCU_16x16->m_puhTransformSkip[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcBestCU_16x16->m_puhTransformSkip[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcBestCU_16x16->m_puhTransformSkip[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						}
						
						//--- update overall distortion ---
						uiOverallDistY += uiBestPUDistY;
						uiOverallDistC += uiBestPUDistC;
		
						//--- update transform index and cbf ---
						memcpy( rpcBestCU_16x16->m_puhTrIdx  + uiPartOffset,  m_pcPredSearch->m_puhQTTempTrIdx,  uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_16x16->m_puhCbf[0]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[0], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_16x16->m_puhCbf[1]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[1], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_16x16->m_puhCbf[2]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[2], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_16x16->m_puhTransformSkip[0]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_16x16->m_puhTransformSkip[1]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_16x16->m_puhTransformSkip[2]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], uiCurrPartNumb * sizeof( UChar ) );
						
						//=== update PU data ====
						memset(rpcBestCU_16x16->m_puhLumaIntraDir + uiPartOffset, uiBestPUMode, sizeof(UChar)*uiCurrPartNumb);
						copyToPic_cos ( rpcBestCU_16x16,uiDepth, uiPU, uiInitTrDepth );
					
					}			
				}
				
				//===== reset context models =====
				load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
	  
				//===== set distortion (rate and r-d costs are determined later) =====
				//ruiDistC                   = uiOverallDistC;
				rpcBestCU_16x16->m_uiTotalDistortion =  uiOverallDistY + uiOverallDistC;
				
				
				copyToPicLuma_cos( pcRecoYuv,rpcBestCU_16x16, rpcBestCU_16x16->m_uiCUAddr, rpcBestCU_16x16->m_uiAbsIdxInLCU);
				
				//亮度预测结束
				//	
				
				//色度预测
				{
					UInt    uiBestMode  = 0;
					UInt    uiBestDist  = 0;
					Double  dBestCost   = MAX_DOUBLE;
		  
					//----- init mode list -----
					UInt  uiMinMode = 0;
					UInt  uiModeList[ 5 ] = {0,26,10,1,36};
					UInt  uiLumaMode = rpcBestCU_16x16->m_puhLumaIntraDir[0];
					UInt  uiMaxMode = 5;
					UInt  uiCurrPartNumb = 256>>(uiDepth<<1);
					Int i;
					UInt uiMode;
					for(i = 0; i < 4; i++ )
					{
						if( uiLumaMode == uiModeList[i] )
						{
						  uiModeList[i] = 34; // VER+8 mode
						  break;
						}
					}
					
					//----- check chroma modes -----
					for(uiMode = uiMinMode; uiMode < uiMaxMode; uiMode++ )
					{
						UInt    uiDist = 0;
						UInt    uiBits;
						Double  dCost;
						//----- restore context models -----
						load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
			
						//----- chroma coding -----
						//rpcBestCU_16x16->setChromIntraDirSubParts  ( uiModeList[uiMode], 0, uiDepth );
						memset(rpcBestCU_16x16->m_puhChromaIntraDir, uiModeList[uiMode], sizeof(UChar)*uiCurrPartNumb);
						
						xRecurIntraChromaCodingQT_cos(m_pcPredSearch, rpcBestCU_16x16,  0, 0, pcOrigYuv, pcPredYuv, pcResiYuv, uiDist );
						
						load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
						
						uiBits = xGetIntraBitsQT_cos(m_pcPredSearch, rpcBestCU_16x16, 0, 0, false, true, false );
						dCost  = ((Double)uiDist + (Double)((Int)(m_pcRdCost->m_dLambda * uiBits +.5)));
						dCost = (Double)(UInt)floor(dCost);
			
						//----- compare -----
						if( dCost < dBestCost )
						{
							dBestCost   = dCost;
							uiBestDist  = uiDist;
							uiBestMode  = uiModeList[uiMode];
							//xSetIntraResultChromaQT( rpcBestCU_16x16, 0, 0, pcRecoYuv );
							xSetIntraResultChromaQT_cos(m_pcPredSearch,rpcBestCU_16x16, 0, 0, pcRecoYuv);
							memcpy( m_pcPredSearch->m_puhQTTempCbf[1], rpcBestCU_16x16->m_puhCbf[1], uiCurrPartNumb * sizeof( UChar ) );
							memcpy( m_pcPredSearch->m_puhQTTempCbf[2], rpcBestCU_16x16->m_puhCbf[2], uiCurrPartNumb * sizeof( UChar ) );
							memcpy( m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcBestCU_16x16->m_puhTransformSkip[1], uiCurrPartNumb * sizeof( UChar ) );
							memcpy( m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcBestCU_16x16->m_puhTransformSkip[2], uiCurrPartNumb * sizeof( UChar ) );
						}
					}
		  
					//----- set data -----
					memcpy( rpcBestCU_16x16->m_puhCbf[1], m_pcPredSearch->m_puhQTTempCbf[1], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcBestCU_16x16->m_puhCbf[2], m_pcPredSearch->m_puhQTTempCbf[2], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcBestCU_16x16->m_puhTransformSkip[1], m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcBestCU_16x16->m_puhTransformSkip[2], m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], uiCurrPartNumb * sizeof( UChar ) );
					
					memset(rpcBestCU_16x16->m_puhChromaIntraDir, uiBestMode, sizeof(UChar)*uiCurrPartNumb);
					rpcBestCU_16x16->m_uiTotalDistortion  += uiBestDist - uiOverallDistC;
					  
					
					//----- restore context models -----
					load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );		
						
				}
				//至此，16x16块两种模式下的CU均获得，均作为输出
				destoryCurYuv_cos(pcOrigYuv);  //原始Yuv可以删除了
				
				Out[0].ulpcBestCU  = (unsigned long)rpcBestCU_16x16;
				Out[0].ulpcPredYuvBest = (unsigned long)pcPredYuv;  //当前以2Nx2N模式开辟的
				Out[0].ulpcRecoYuvBest = (unsigned long)pcRecoYuv;
				Out[0].ulpcResiYuvBest = (unsigned long)pcResiYuv;
				
				Out[0].ulpcTempCU = In[0].ulpcTempCU;  //从输入获得的4个子CU组成的临时CU
				Out[0].ulpcPredYuv = In[0].ulpcPredYuv;
				Out[0].ulpcRecoYuv = In[0].ulpcRecoYuv;
				Out[0].ulpcResiYuv = In[0].ulpcResiYuv;
				
				iCount = 0;
					
			}
			else
			{
				//输出数据为0；
			}
			
					
		}
		window
		{
			//In sliding(4,4); //输入窗口为4，当4个8x8块组成的临时最佳16x16块集齐后才进行16x16的2Nx2N模式
			//Out tumbling(1); //输出1个划分模式和2Nx2N模式的16x16块
			In sliding(1,1);  //控制滑动窗口为1，此时需要在work加执行的判断条件
			Out tumbling(1);
		}
	};
}

composite encodeRelated_16x16(output stream<unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out, 
							   input stream<unsigned long ulpcBestCU, unsigned long ulpcPredYuvBest, unsigned long ulpcRecoYuvBest, unsigned long ulpcResiYuvBest,
                                            unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>In)
{
	Out = encodeRelated_16x16(In)
	{
		int iCount = 0;
		int uiPartUnitIdx;  //记录处理的个数
		init
		{
			iCount = 0;
			uiPartUnitIdx = 0;
		}
		work
		{
			if(++iCount == 4)
			{
				//根据输入获得熵编码器指针
				//对应的YUV为pcPredYuvTemp,pcRecoYuvTemp
				//临时最佳CU和临时CU：rpcBestCU_16x16,rpcTempCU_16x16
				UInt uiDepth = 2; //-> m_puhDepth[0];
				double dRdCost;
				int i = 0;
				TComDataCU* rpcBestCU = (TComDataCU*)(In[0].ulpcBestCU);  // 2Nx2N模式对应的CU
				TComYuv *pcPredYuvBest = (TComYuv*)(In[0].ulpcPredYuvBest);  //根据输入做相应转换
				TComYuv *pcRecoYuvBest = (TComYuv*)(In[0].ulpcRecoYuvBest);
				TComYuv *pcResiYuvBest = (TComYuv*)(In[0].ulpcResiYuvBest);
				
				TComDataCU* rpcTempCU = (TComDataCU*)(In[0].ulpcTempCU); //划分模式下的CU
				TComYuv *pcPredYuvTemp = (TComYuv*)(In[0].ulpcPredYuv);  //根据输入做相应转换
				TComYuv *pcRecoYuvTemp = (TComYuv*)(In[0].ulpcRecoYuv);
				TComYuv *pcResiYuvTemp = (TComYuv*)(In[0].ulpcResiYuv);
				
				TEncEntropy* m_pcEntropyCoder =  &(m_pcEncTop->m_cEntropyCoder);
			
				TComRdCost* m_pcRdCost = &(m_pcEncTop->m_cRdCost);
				
				//定义TcomTrQuant类型变量
				//TComTrQuant m_cTrQuant;
				//TComTrQuant* m_pcTrQuant = &m_cTrQuant;
				
				encodeIntra(m_pcCuEncoder,m_pcEntropyCoder,rpcBestCU,uiDepth);
			  
				store_cos(m_pcRDGoOnSbacCoder,m_pppcRDSbacCoder[uiDepth][2]);  //使用全局的外部变量
			 
				rpcBestCU->m_uiTotalBits = getNumberOfWrittenBits_cos(m_pcEntropyCoder);  //计算2Nx2N模式的bits
				rpcBestCU->m_uiTotalBins = getBinsCoded_cos(m_pcEntropyCoder);
				
				dRdCost = ((Double)rpcBestCU->m_uiTotalDistortion + (Double)((Int)(rpcBestCU->m_uiTotalBits * (m_pcRdCost->m_dLambda)+.5)));
				dRdCost = (Double)(UInt)floor(dRdCost);
				rpcBestCU->m_dTotalCost = dRdCost;
			  
				//xCheckDQP( rpcTempCU );
				//xCheckBestMode(rpcBestCU, rpcTempCU, uiDepth);
				if(rpcTempCU->m_dTotalCost < rpcBestCU->m_dTotalCost)  //
				{
					TComYuv* pcYuv;
					// Change Information data
					TComDataCU* pcCU = rpcBestCU;
					rpcBestCU = rpcTempCU;
					rpcTempCU = pcCU;

					// Change Prediction data
					pcYuv = pcPredYuvBest;
					pcPredYuvBest = pcPredYuvTemp;
					pcPredYuvTemp = pcYuv;
						
					// Change Reconstruction data
					pcYuv = pcRecoYuvBest;
					pcRecoYuvBest = pcRecoYuvTemp;
					pcRecoYuvTemp = pcYuv;

					pcYuv = NULL;
					pcCU  = NULL;
						
					// store temp best CI for next CU coding
					store_cos(m_pppcRDSbacCoder[uiDepth][2],m_pppcRDSbacCoder[uiDepth][1]);   
				}
					
				destoryCurCU_cos(rpcTempCU);
				destoryCurYuv_cos(pcPredYuvTemp);
				destoryCurYuv_cos(pcRecoYuvTemp);
				destoryCurYuv_cos(pcResiYuvTemp) ;  //删除临时16x16 CU开辟的空间;并置指针为NULL，
				
				//事实上到该for循环结束时CI_NEXT_BEST已获取,即m_pppcRDSbacCoder[2][1]状态已经完成，下一个16*16块可以进行  //通过互斥变量看能否实现
				
				//同时当4个16x16子块均处理时，可以将状态传至上一层，原代码为m_pppcRDSbacCoder[uhNextDepth][CI_NEXT_BEST]->store(m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]);
				if(++uiPartUnitIdx == 4)  
				{
					//m_pppcRDSbacCoder[uiDepth][1]->store(m_pppcRDSbacCoder[uiDepth-1][2]);
					uiPartUnitIdx = 0;
					store_cos(m_pppcRDSbacCoder[uiDepth][1], m_pppcRDSbacCoder[uiDepth-1][0]);
				}
				
				resetBits_cos(m_pcEntropyCoder);
				encodeSplitFlag_cos(m_pcEntropyCoder,rpcBestCU, 0, uiDepth, true );  // 当第3号8x8 编码后，4个8x8块拷贝至16x16临时CU中，然后对该临时CU编码，
				rpcBestCU->m_uiTotalBits += getNumberOfWrittenBits_cos(m_pcEntropyCoder); // split bits
				rpcBestCU->m_uiTotalBins += getBinsCoded_cos(m_pcEntropyCoder);
				//rpcBestCU->getTotalCost()  = m_pcRdCost->calcRdCost( rpcBestCU->getTotalBits(), rpcBestCU->getTotalDistortion() );
				dRdCost = ((Double)rpcBestCU->m_uiTotalDistortion + (Double)((Int)(rpcBestCU->m_uiTotalBits * (m_pcRdCost->m_dLambda)+.5)));
				dRdCost = (Double)(UInt)floor(dRdCost);
				rpcBestCU->m_dTotalCost = dRdCost;
					
				copyToPic_cos(rpcBestCU,uiDepth);                                                     // Copy Best data to Picture for next partition prediction.

				xCopyYuv2Pic_cos(m_pcCuEncoder,rpcBestCU,uiDepth);    //至此，一个16x16块的最佳状态已经完全确定
			
				Out[0].ulpcTempCU = (unsigned long)rpcBestCU;
				Out[0].ulpcPredYuv = (unsigned long)pcPredYuvBest;
				Out[0].ulpcRecoYuv = (unsigned long)pcRecoYuvBest;
				Out[0].ulpcResiYuv = (unsigned long)pcResiYuvBest;
				
				iCount = 0;
				
			}
			
		
		}
		window
		{
			In sliding(1,1);
			Out tumbling(1);
		}
	};
}


//将某一16x16最终的CU状态及YUV拷贝至上一层32x32 CU的相应位置,也即32x32的分割模式
composite copyPartTo_32x32(output stream<unsigned long ulpcTempCU, unsigned long ulpcOrigYuv, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out, 
							input stream<unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>In)
{
	Out = xcopyPartTo_32x32(In)
	{
		int iCount = 0;
		int uiPartUnitIdx;
		int uiAbsZorderIdx;  //记录处理的32x32块的起始zorder;
		UChar uiDepth;
		UChar uhWidth;
        UChar uhHeight;
		
		TComDataCU* rpcTempCU_32x32; 
		TComYuv *pcOrigYuv;		
		TComYuv *pcPredYuvTemp, *pcRecoYuvTemp, *pcResiYuvTemp;
		
		init 
		{
			iCount = 0;
			uiPartUnitIdx = 0;
			uiAbsZorderIdx = 0;
			uiDepth  = 1;
			uhWidth  = 32;
			uhHeight = 32;
			rpcTempCU_32x32 = NULL;
			pcOrigYuv = NULL;  pcPredYuvTemp= NULL;   pcRecoYuvTemp = NULL;   pcResiYuvTemp = NULL;  //当前层即32x32块的临时Yuv
			                  
		}
		work
		{
			if(++iCount == 4)
			{
				//pcCU为LCU的指针
				TComPic* pcPic = pcCU->m_pcPic;
				TComPicYuv* pcPicYuvSrc = pcPic->m_apcPicYuv[0];
				
				int y;
				Pel* pDst, *pDstU, *pDstV;
				Pel* pSrc, *pSrcU, *pSrcV;
				UInt iDstStride, iSrcStride,iWidth,iCWidth;
				
				// TComYuv* m_ppcOrigYuv = NULL;
				// Int iQP = pcCU->m_phQP[0];
				
				UInt iCuAddr = pcCU->m_uiCUAddr;
				double dRdCost = 0.0;
				
				TComDataCU* pcSubBestPartCU = (TComDataCU*)(In[0].ulpcTempCU);//待完成	
				TComYuv *pcPredYuvBest_16x16 = (TComYuv*)(In[0].ulpcPredYuv);
				TComYuv *pcRecoYuvBest_16x16 = (TComYuv*)(In[0].ulpcRecoYuv); 
				TComYuv *pcResiYuvBest_16x16 = (TComYuv*)(In[0].ulpcResiYuv);  //根据输入完成
			  
				//定义编码器
				//TEncEntropy m_cEntropyCoder;
				//TEncEntropy *m_pcEntropyCoder = &m_cEntropyCoder;
				
				//定义TEncSearch类型变量,
				//TEncSearch m_cSearch;  //调用构造函数完成对象初始化；由于是在栈上开辟的空间，所以退出该work后自动调用析构函数进行对象析构
				//TEncSearch* m_pcPredSearch = &m_cSearch; //获得指针
				
				//定义TComRdCost类型变量
				//TComRdCost m_cRdCost;
				//TComRdCost* m_pcRdCost = &m_cRdCost;
				TEncEntropy* m_pcEntropyCoder =  &(m_pcEncTop->m_cEntropyCoder);
			
				TComRdCost* m_pcRdCost = &(m_pcEncTop->m_cRdCost);
				if(uiPartUnitIdx== 0)  //说明是在处理一个新的32x32块
				{	
					
					createCurCU_cos(pcCU,rpcTempCU_32x32,uiDepth,uiAbsZorderIdx); 
					createCurYuv_cos(pcOrigYuv,32,32);  //创建原始YUV空间并初始化，注意释放空间
					createCurYuv_cos(pcPredYuvTemp,32,32);   //创建原始YUV空间并初始化，注意释放空间
					createCurYuv_cos(pcRecoYuvTemp,32,32);  //创建原始YUV空间并初始化，注意释放空间
					createCurYuv_cos(pcResiYuvTemp,32,32);  //创建原始YUV空间并初始化，注意释放空间
					
					//pcOrigYuv 原始yuv数据拷贝
					//等价代码：m_ppcOrigYuv[1]->copyFromPicYuv( pcPic->getPicYuvOrg(), rpcBestCU->getAddr(), rpcBestCU->getZorderIdxInCU() );
					{
						int  y;
						short* pDst  = pcOrigYuv->m_apiBufY; 
						short* pDstU = pcOrigYuv->m_apiBufU;
						short* pDstV = pcOrigYuv->m_apiBufV;
						//Pel* pSrc     = pcPicYuvSrc->getLumaAddr ( iCuAddr, uiAbsZorderIdx );
						Pel* pSrc    = pcPicYuvSrc->m_piPicOrgY + pcPicYuvSrc->m_cuOffsetY[iCuAddr] + pcPicYuvSrc->m_buOffsetY[g_auiZscanToRaster[uiAbsZorderIdx]];
						Pel* pSrcU   = pcPicYuvSrc->m_piPicOrgU + pcPicYuvSrc->m_buOffsetC[iCuAddr] + pcPicYuvSrc->m_buOffsetC[g_auiZscanToRaster[uiAbsZorderIdx]];
						Pel* pSrcV   = pcPicYuvSrc->m_piPicOrgV + pcPicYuvSrc->m_buOffsetC[iCuAddr] + pcPicYuvSrc->m_buOffsetC[g_auiZscanToRaster[uiAbsZorderIdx]];
						
						//copyFromPicLuma
						UInt  iDstStride  =  pcOrigYuv->m_iWidth;
						UInt  iSrcStride  = pcPicYuvSrc->m_iPicWidth + ((pcPicYuvSrc->m_iLumaMarginX)<<1);
						UInt  iWidth = pcOrigYuv->m_iWidth;
						UInt  iCWidth = pcOrigYuv->m_iCWidth;
						for ( y = pcOrigYuv->m_iHeight; y != 0; y-- )
						{
							memcpy( pDst, pSrc, sizeof(short)*iWidth);
							pDst += iDstStride;
							pSrc += iSrcStride;
						}
						
						//copyFromPicChroma
						iDstStride = pcOrigYuv->m_iCWidth;
						iSrcStride =  (pcPicYuvSrc->m_iPicWidth)>>1 + ((pcPicYuvSrc->m_iChromaMarginX)<<1);
						for ( y = pcOrigYuv->m_iCHeight; y != 0; y-- )
						{
							memcpy( pDstU, pSrcU, sizeof(Pel)*(iCWidth) );
							memcpy( pDstV, pSrcV, sizeof(Pel)*(iCWidth) );
							pSrcU += iSrcStride;
							pSrcV += iSrcStride;
							pDstU += iDstStride;
							pDstV += iDstStride;
						} 				
					}
					
					uiAbsZorderIdx += 64;  //每处理完一个32x32块，Zorder自增64;
				}
				
				//pcSubBestPartCU为第uiPartUnitIdx个16x16子块对应的最佳CU；需要根据输入转化为对应的指针
				//rpcTempCU_32x32->copyPartFrom( pcSubBestPartCU, uiPartUnitIdx, uiDepth+1 );         // Keep best part data to current temporary data.
				copyPartFrom_cos(rpcTempCU_32x32,pcSubBestPartCU,uiPartUnitIdx,uiDepth+1);
				
				//xCopyYuv2Tmp( pcSubBestPartCU->getTotalNumPart()*uiPartUnitIdx, uiDepth+1 ); 执行：pcRecoYuvBest[uiDepth+1]->copyToPartYuv( pcRecoYuvTemp[uiDepth], pcSubBestPartCU->getTotalNumPart()*uiPartUnitIdx );
				{
					
					//计算第uiPartUnitIdx个16x16子块在32x32块中对应位置
					UInt uiDstPartIdx = 16*uiPartUnitIdx; // pcSubBestPartCU->getTotalNumPart()*uiPartUnitIdx
					
					pSrc  = pcRecoYuvBest_16x16->m_apiBufY;
					pDst  =     pcRecoYuvTemp->m_apiBufY + ( g_auiRasterToPelX[g_auiZscanToRaster[uiDstPartIdx]] + g_auiRasterToPelY[g_auiZscanToRaster[uiDstPartIdx]]* pcRecoYuvTemp->m_iWidth );
					
					pSrcU = pcRecoYuvBest_16x16->m_apiBufU;
					pSrcV = pcRecoYuvBest_16x16->m_apiBufV;
					pDstU =     pcRecoYuvTemp->m_apiBufU + ( g_auiRasterToPelX[g_auiZscanToRaster[uiDstPartIdx]] + (g_auiRasterToPelY[g_auiZscanToRaster[uiDstPartIdx]]* pcRecoYuvTemp->m_iCWidth)>>1);
					pDstV =     pcRecoYuvTemp->m_apiBufV + ( g_auiRasterToPelX[g_auiZscanToRaster[uiDstPartIdx]] + (g_auiRasterToPelY[g_auiZscanToRaster[uiDstPartIdx]]* pcRecoYuvTemp->m_iCWidth)>>1);
	  
					iSrcStride = pcRecoYuvBest_16x16->m_iWidth;
					iDstStride =     pcRecoYuvTemp->m_iWidth;
					
					y = pcRecoYuvBest_16x16->m_iHeight;
					for( y; y!=0; y-- )
					{
						memcpy(pDst, pSrc, sizeof(Pel)*iSrcStride);
						pDst += iDstStride;
						pSrc += iSrcStride;
					}
					
					iSrcStride = pcRecoYuvBest_16x16->m_iCWidth;
					iDstStride =     pcRecoYuvTemp->m_iCWidth;
					
					y = pcRecoYuvBest_16x16->m_iCHeight;
					for ( y; y != 0; y-- )
					{
						memcpy( pDstU, pSrcU, sizeof(Pel)*(iSrcStride) );
						memcpy( pDstV, pSrcV, sizeof(Pel)*(iSrcStride) );
						pSrcU += iSrcStride;
						pSrcV += iSrcStride;
						pDstU += iDstStride;
						pDstV += iDstStride;
					}	
				}
				
				//最佳16x16块已经拷贝完毕，相关空间可以销毁
				destoryCurCU_cos(pcSubBestPartCU);
				destoryCurYuv_cos(pcPredYuvBest_16x16);
				destoryCurYuv_cos(pcRecoYuvBest_16x16);
				destoryCurYuv_cos(pcResiYuvBest_16x16);
				
				if(++uiPartUnitIdx == 4) //4个16x16块均接收到，对临时最佳32x32块编码
				{
					uiPartUnitIdx = 0; //重置0，用于标记下一个32x32块的开始
					
					//m_pcEntropyCoder->resetBits();  //这里的熵编码器的上一个状态是encodeRelated_8x8中第3号8x8进行encodeSplitFlag_cos编码结束之后的状态
					//m_pcEntropyCoder->encodeSplitFlag( rpcTempCU_32x32, 0, uiDepth, true );

					//rpcTempCU_32x32->getTotalBits() += m_pcEntropyCoder->getNumberOfWrittenBits(); // split bits
					//rpcTempCU_32x32->getTotalBins() += ((TEncBinCABAC *)((TEncSbac*)m_pcEntropyCoder->m_pcEntropyCoderIf)->getEncBinIf())->getBinsCoded();
					//rpcTempCU_32x32->getTotalCost()  = m_pcRdCost->calcRdCost( rpcTempCU_32x32->getTotalBits(), rpcTempCU_32x32->getTotalDistortion() );

					resetBits_cos(m_pcEntropyCoder);
					encodeSplitFlag_cos(m_pcEntropyCoder,rpcTempCU_32x32, 0, uiDepth, true );  // 当第3号8x8 编码后，4个8x8块拷贝至16x16临时CU中，然后对该临时CU编码，
					rpcTempCU_32x32->m_uiTotalBits += getNumberOfWrittenBits_cos(m_pcEntropyCoder); // split bits
					rpcTempCU_32x32->m_uiTotalBins += getBinsCoded_cos(m_pcEntropyCoder);
					//rpcBestCU->getTotalCost()  = m_pcRdCost->calcRdCost( rpcTempCU_32x32->getTotalBits(), rpcTempCU_32x32->getTotalDistortion() );
					dRdCost = ((Double)rpcTempCU_32x32->m_uiTotalDistortion + (Double)((Int)(rpcTempCU_32x32->m_uiTotalBits * (m_pcRdCost->m_dLambda)+.5)));
					dRdCost = (Double)(UInt)floor(dRdCost);
					rpcTempCU_32x32->m_dTotalCost = dRdCost;
					
					
					//m_pppcRDSbacCoder[uiDepth+1][CI_NEXT_BEST]->store(m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]);

					//xCheckBestMode( rpcBestCU_32x32, rpcTempCU_32x32, uiDepth);  //保存至最佳rpcBestCU_32x32
				}
				
				Out[0].ulpcTempCU = (unsigned long)rpcTempCU_32x32;
				Out[0].ulpcOrigYuv= (unsigned long)pcOrigYuv;
				Out[0].ulpcPredYuv = (unsigned long)pcPredYuvTemp;
				Out[0].ulpcRecoYuv = (unsigned long)pcRecoYuvTemp;
				Out[0].ulpcResiYuv = (unsigned long)pcResiYuvTemp;
				
				iCount = 0;
			}
	        
		}
		window
		{
			In sliding(1,1);   //输入一个最佳的16x16并处理
			Out tumbling(1);
		}	
	};
}


//32x32块的2Nx2N模式
composite xCheckRDCostIntra32x32_cos(output stream<unsigned long ulpcBestCU, unsigned long ulpcPredYuvBest, unsigned long ulpcRecoYuvBest, unsigned long ulpcResiYuvBest,
                                                   unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out,
									  input stream<unsigned long ulpcTempCU, unsigned long ulpcOrigYuv, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>In)
{
	Out = xCheckRDCostIntra32x32(In)
	{
		int iCount = 0;
		int uiPartUnitIdx;
		int uiAbsZorderIdx;  //记录处理的32x32块的起始zorder;
		UChar uiDepth;  //当前CU的深度
		init 
		{
			uiPartUnitIdx = 0;
			uiAbsZorderIdx = 0;
			uiDepth  = 1;	
		}
		work
		{
			if(++iCount == 16)
			{
				TComDataCU* rpcBestCU_32x32 = NULL;
				TComYuv *pcPredYuv= NULL, *pcRecoYuv = NULL, *pcResiYuv = NULL;  // 2Nx2N模式对应的最佳预测，重构和残余Yuv
				//不加Best是为了和亮度和色度预测代码中各Yuv名称保持一致
			
				TComYuv *pcOrigYuv = (TComYuv *)(In[0].ulpcOrigYuv); //事实上 In[0]~In[3]中数据是一样的
				
				UInt    uiCurrPartNumb = 256>>(uiDepth<<1);
				
				UInt    uiNumPU        = 1;  // 当前CU的分割模式（(SIZE_2Nx2N:1
				UInt    uiInitTrDepth  = 0 ; //!< 用于计算变换的深度，实际深度为该值+uiDepth  
				UInt    uiWidth = 32;  //!< 当前PU的宽度
				UInt    uiHeight = 32; //!< 当前PU的高度
				UInt    uiQNumParts    = 16;     
				//UInt    uiWidthBit     = pcCU->getIntraSizeIdx(0);
				UInt    uiOverallDistY = 0;
				UInt    uiOverallDistC = 0;
				UInt    CandNum = 0;
				Double  CandCostList[35];
				UInt uiPartOffset = 0;
				UInt uiPU = 0;
				
				//TEncSbac*** m_pppcRDSbacCoder;
				
				//定义TcomTrQuant类型变量
				TComTrQuant m_cTrQuant;
				TComTrQuant* m_pcTrQuant = &m_cTrQuant;
				
				//定义TEncSearch类型变量
				TEncSearch m_cSearch;  //调用构造函数完成对象初始化；由于是在栈上开辟的空间，所以退出该work后自动调用析构函数进行对象析构
				TEncSearch* m_pcPredSearch = &m_cSearch; //获得指针
				
				//定义编码器
				TEncEntropy m_cEntropyCoder;
				
				//定义TComRdCost类型变量
				TComRdCost m_cRdCost;
				TComRdCost* m_pcRdCost = &m_cRdCost;
				
				//m_cTrQuant.init(32,true,true,true,true,0);
				initTComRdCost(m_pcRdCost, &(m_pcEncTop->m_cRdCost));
				initTComTrQuant(m_pcTrQuant, &(m_pcEncTop->m_cTrQuant));
				
				//m_pcTrQuant 和 m_pcRdCost均为新创建变量的地址，而不是原m_pcEncTop下的指针
				//initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&(m_pcEncTop->m_cEntropyCoder),m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
				initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&m_cEntropyCoder,m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
				
				createCurCU_cos(pcCU,rpcBestCU_32x32,uiDepth,uiAbsZorderIdx);
				createCurYuv_cos(pcPredYuv,32,32);//创建原始YUV空间并初始化，注意释放空间
				createCurYuv_cos(pcRecoYuv,32,32);//创建原始YUV空间并初始化，注意释放空间
				createCurYuv_cos(pcResiYuv,32,32);//创建原始YUV空间并初始化，注意释放空间
				
				uiAbsZorderIdx += uiCurrPartNumb;  //每处理完一个32x32块，Zorder自增64;
				if(uiAbsZorderIdx == 256) 
				{
					uiAbsZorderIdx = 0;
				}
				
				// rpcTempCU->setSkipFlagSubParts( false, 0, uiDepth );
				//rpcTempCU->setPartSizeSubParts( eSize, 0, uiDepth );
				//rpcTempCU->setPredModeSubParts( MODE_INTRA, 0, uiDepth );
				memset( rpcBestCU_32x32->m_skipFlag, 0, 256>>(2*uiDepth) );  //在uiDepth等于3时，有4个块
				memset( rpcBestCU_32x32->m_pePartSize,0, 256>>(2*uiDepth) );
				memset( rpcBestCU_32x32->m_pePredMode,1, 256>>(2*uiDepth) );
				
				setQPSubParts_cos( rpcBestCU_32x32, 0, uiDepth );
				
				///////////////////////////////////
				//m_pppcRDSbacCoder[uhNextDepth][CI_CURR_BEST]->load(m_pppcRDSbacCoder[uiDepth][CI_CURR_BEST]);
				//////////////////////////////////
				
				{
					//===== init pattern for luma prediction =====
					bool bAboveAvail = false;
					bool bLeftAvail  = false;
					Int numModesAvailable     = 35; //total number of Intra modes
					UInt uiRdModeList[35];
					Int numModesForFullRD = 3;  //!< MPM数目  
					Int i = 0,modeIdx = 0;
					//===== determine set of modes to be tested (using prediction signal only) =====
					//
					//Pel* piOrg         = getLumaAddr_cos( pcOrigYuv,uiPU, uiWidth );
					//Pel* piPred        = getLumaAddr_cos( pcPredYuv,uiPU, uiWidth );  //预测YUV空间需要开辟
					//UInt uiStride      = getStride_cos(pcPredYuv);
					Pel* piOrg  = pcOrigYuv->m_apiBufY + ( (uiPU * uiWidth)&(pcOrigYuv->m_iWidth - 1) + (( uiPU * uiWidth )&~( pcOrigYuv->m_iWidth - 1 )) * uiWidth );
					Pel* piPred = pcPredYuv->m_apiBufY + ( (uiPU * uiWidth)&(pcPredYuv->m_iWidth - 1) + (( uiPU * uiWidth )&~( pcOrigYuv->m_iWidth - 1 )) * uiWidth );
					UInt uiStride      = pcPredYuv->m_iWidth;
					
					// rpcBestCU_32x32->getPattern()->initPattern   ( rpcBestCU_32x32, uiInitTrDepth, uiPartOffset );
					// set luma parameters from CU data for accessing ADI data //!< 主要获取当前PU的邻域可用性，对参考样点进行设置及滤波  
					// rpcBestCU_32x32->getPattern()->initAdiPattern( rpcBestCU_32x32, uiPartOffset, uiInitTrDepth, m_piYuvExt, m_iYuvExtStride, m_iYuvExtHeight, bAboveAvail, bLeftAvail );
					initAdiPattern_cos(rpcBestCU_32x32,uiPartOffset, uiInitTrDepth, m_pcPredSearch->m_piYuvExt, m_pcPredSearch->m_iYuvExtStride, m_pcPredSearch->m_iYuvExtHeight, bAboveAvail, bLeftAvail);
				
					for(i = 0; i < numModesForFullRD; i++ ) 
					{
						CandCostList[ i ] = MAX_DOUBLE;
					}
					
					for(modeIdx = 0; modeIdx < 35; modeIdx++ )
					{
					  UInt uiMode = modeIdx;
					  UInt uiSad,iModeBits;
					  double cost;
					  predIntraLumaAng_cos(m_pcPredSearch,rpcBestCU_32x32,uiMode,piPred, uiStride, uiWidth, uiHeight, bAboveAvail, bLeftAvail);
					  
					  uiSad = calcHAD_cos(m_pcRdCost,piOrg, uiStride, piPred, uiStride, uiWidth, uiHeight);
					  
					  iModeBits = xModeBitsIntra_cos(m_pcPredSearch,rpcBestCU_32x32, uiMode, uiPU, uiPartOffset, uiDepth, uiInitTrDepth);
					  cost = (double)uiSad + (double)iModeBits * getSqrtLambda_cos(m_pcRdCost);
					  
					  CandNum = xUpdateCandList_cos(m_pcPredSearch,uiMode, cost, numModesForFullRD, uiRdModeList, CandCostList);
					}
					//快速算法
					{
						Int uiPreds[3] = {-1, -1, -1};
						Int iMode = -1;  //!< 如果三个MPMs的前两个相同，则iMode=1，否则iMode=2  
						Int numCand = getIntraDirLumaPredictor_cos( rpcBestCU_32x32,uiPartOffset, uiPreds, &iMode );  //!< 获取亮度帧内预测模式的三个MPMs  ,most probable mode
						Int i = 0,j = 0;
						if( iMode >= 0 )
						{
							numCand = iMode;
						}
					  
						for( j = 0; j < numCand; j++)
						{
							bool mostProbableModeIncluded = false;
							Int mostProbableMode = uiPreds[j];
							
							for( i = 0; i < numModesForFullRD; i++)
							{
							  mostProbableModeIncluded |= (mostProbableMode == uiRdModeList[i]);
							}
							if (!mostProbableModeIncluded)
							{
							  uiRdModeList[numModesForFullRD++] = mostProbableMode;
							}
						}
						
					}
					
					{
						UInt    uiBestPUMode  = 0;   //!< 存放最佳预测模式  
						UInt    uiBestPUDistY = 0;   //!< 存放最佳预测模式对应的亮度失真值 
						UInt    uiBestPUDistC = 0;   //!< 存放最佳预测模式对应的色度失真值 
						Double  dBestPUCost   = MAX_DOUBLE;  //!< 存放最佳预测模式对应的总代价 
						UInt uiMode = 0;
						
						UInt uiOrgMode = 0;  //存放最佳模式
						UInt uiCurrPartNumb =  256>>((uiDepth+uiInitTrDepth)<<1);
						// determine residual for partition
						UInt   uiPUDistY = 0;
						UInt   uiPUDistC = 0;
						Double dPUCost   = 0.0;
						
						for(uiMode = 0; uiMode < numModesForFullRD; uiMode++ )  //!< 遍历存储在uiRdModeList里的模式  
						{
						  // set luma prediction mode
						  UInt uiOrgMode = uiRdModeList[uiMode];
						  //rpcBestCU_32x32->setLumaIntraDirSubParts ( uiOrgMode, uiPartOffset, uiDepth + uiInitTrDepth );
						  
						  // determine residual for partition
						  UInt   uiPUDistY = 0;
						  UInt   uiPUDistC = 0;
						  Double dPUCost   = 0.0;
						  
						  memset(rpcBestCU_32x32->m_puhLumaIntraDir+uiPartOffset, uiOrgMode, sizeof(UChar)*uiCurrPartNumb);
						  
						  // set context models
						  load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
						  
						  
						  //整个亮度预测的函数中，两次调用xRecurIntraCodingQT,其中倒数第二个参数第一次为true，一次为false。该参数控制函数中的bCheckSplit的值，若为true，则TU只进行当前深度的计算，如果该值为false，则TU进行所有深度的计算。

						  xRecurIntraCodingQT_cos(m_pcPredSearch, rpcBestCU_32x32, uiInitTrDepth, uiPartOffset, true, pcOrigYuv, pcPredYuv, pcResiYuv, uiPUDistY, uiPUDistC, true, dPUCost );

						  
						  // check r-d cost
						  if( dPUCost < dBestPUCost )  //!< 更新最佳预测模式相关参数     //Cbf:coded block flag, code block pattern,用于标记亮度、色差的残差系数的额特性，是否有DC、AC
						  {
							uiBestPUMode  = uiOrgMode;
							uiBestPUDistY = uiPUDistY;
							uiBestPUDistC = uiPUDistC;
							dBestPUCost   = dPUCost;
							
							xSetIntraResultQT_cos( m_pcPredSearch,rpcBestCU_32x32, uiInitTrDepth, uiPartOffset, true, pcRecoYuv );    //将最新数据拷贝至重构图像
							
							memcpy(m_pcPredSearch->m_puhQTTempTrIdx,  rpcBestCU_32x32->m_puhTrIdx + uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[0], rpcBestCU_32x32->m_puhCbf[0]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[1], rpcBestCU_32x32->m_puhCbf[1]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[2], rpcBestCU_32x32->m_puhCbf[2]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], rpcBestCU_32x32->m_puhTransformSkip[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcBestCU_32x32->m_puhTransformSkip[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcBestCU_32x32->m_puhTransformSkip[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						  }
						} // Mode loop
						
						uiOrgMode = uiBestPUMode;  //!<　设置模式为最佳预测模式  
						
						//rpcBestCU_32x32->setLumaIntraDirSubParts ( uiOrgMode, 0, uiDepth + uiInitTrDepth );
						memset(rpcBestCU_32x32->m_puhLumaIntraDir+uiPartOffset, uiOrgMode, sizeof(UChar)*uiCurrPartNumb);
						
						// set context models
						load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
						  
						
						//! 注意该函数倒数第二个参数，此时为false  
						xRecurIntraCodingQT_cos(m_pcPredSearch, rpcBestCU_32x32, uiInitTrDepth, uiPartOffset, true, pcOrigYuv, pcPredYuv, pcResiYuv, uiPUDistY, uiPUDistC, false, dPUCost );
						  
						// check r-d cost
						if( dPUCost < dBestPUCost )
						{
							uiBestPUMode  = uiOrgMode;
							uiBestPUDistY = uiPUDistY;
							uiBestPUDistC = uiPUDistC;
							dBestPUCost   = dPUCost;
							
							xSetIntraResultQT_cos( m_pcPredSearch,rpcBestCU_32x32, uiInitTrDepth, uiPartOffset, true, pcRecoYuv );    //将最新数据拷贝至重构图像
							
							memcpy(m_pcPredSearch->m_puhQTTempTrIdx,  rpcBestCU_32x32->m_puhTrIdx + uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[0], rpcBestCU_32x32->m_puhCbf[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[1], rpcBestCU_32x32->m_puhCbf[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[2], rpcBestCU_32x32->m_puhCbf[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], rpcBestCU_32x32->m_puhTransformSkip[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcBestCU_32x32->m_puhTransformSkip[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcBestCU_32x32->m_puhTransformSkip[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						}
						
						//--- update overall distortion ---
						uiOverallDistY += uiBestPUDistY;
						uiOverallDistC += uiBestPUDistC;
		
						//--- update transform index and cbf ---
						//--- update transform index and cbf ---
						memcpy( rpcBestCU_32x32->m_puhTrIdx  + uiPartOffset,  m_pcPredSearch->m_puhQTTempTrIdx,  uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_32x32->m_puhCbf[0]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[0], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_32x32->m_puhCbf[1]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[1], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_32x32->m_puhCbf[2]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[2], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_32x32->m_puhTransformSkip[0]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_32x32->m_puhTransformSkip[1]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_32x32->m_puhTransformSkip[2]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], uiCurrPartNumb * sizeof( UChar ) );
						
						//=== update PU data ====
						memset(rpcBestCU_32x32->m_puhLumaIntraDir + uiPartOffset, uiBestPUMode, sizeof(UChar)*uiCurrPartNumb);
						copyToPic_cos ( rpcBestCU_32x32,uiDepth, uiPU, uiInitTrDepth );
					
					}			
				}
				
				//===== reset context models =====
				load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
	  
				//===== set distortion (rate and r-d costs are determined later) =====
				//ruiDistC                   = uiOverallDistC;
				rpcBestCU_32x32->m_uiTotalDistortion =  uiOverallDistY + uiOverallDistC;
				
				
				copyToPicLuma_cos( pcRecoYuv,rpcBestCU_32x32, rpcBestCU_32x32->m_uiCUAddr, rpcBestCU_32x32->m_uiAbsIdxInLCU);
				
				//亮度预测结束
				//	
				
				//色度预测
				{
					UInt    uiBestMode  = 0;
					UInt    uiBestDist  = 0;
					Double  dBestCost   = MAX_DOUBLE;
		  
					//----- init mode list -----
					UInt  uiMinMode = 0;
					UInt  uiModeList[ 5 ] = {0,26,10,1,36};
					UInt  uiLumaMode = rpcBestCU_32x32->m_puhLumaIntraDir[0];
					UInt  uiMaxMode = 5;
					UInt  uiCurrPartNumb = 256>>(uiDepth<<1);
					Int i;
					UInt uiMode;
					for(i = 0; i < 4; i++ )
					{
						if( uiLumaMode == uiModeList[i] )
						{
						  uiModeList[i] = 34; // VER+8 mode
						  break;
						}
					}
					
					//----- check chroma modes -----
					for(uiMode = uiMinMode; uiMode < uiMaxMode; uiMode++ )
					{
						UInt    uiDist = 0;
						UInt    uiBits;
						Double  dCost;
						//----- restore context models -----
						load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
			
						//----- chroma coding -----
						//rpcBestCU_32x32->setChromIntraDirSubParts  ( uiModeList[uiMode], 0, uiDepth );
						memset(rpcBestCU_32x32->m_puhChromaIntraDir, uiModeList[uiMode], sizeof(UChar)*uiCurrPartNumb);
						
						xRecurIntraChromaCodingQT_cos(m_pcPredSearch, rpcBestCU_32x32,  0, 0, pcOrigYuv, pcPredYuv, pcResiYuv, uiDist );
						
						load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
						
						uiBits = xGetIntraBitsQT_cos(m_pcPredSearch, rpcBestCU_32x32, 0, 0, false, true, false );
						dCost  = ((Double)uiDist + (Double)((Int)(m_pcRdCost->m_dLambda * uiBits +.5)));
						dCost = (Double)(UInt)floor(dCost);
			
						//----- compare -----
						if( dCost < dBestCost )
						{
							dBestCost   = dCost;
							uiBestDist  = uiDist;
							uiBestMode  = uiModeList[uiMode];
							//xSetIntraResultChromaQT( rpcBestCU_32x32, 0, 0, pcRecoYuv );
							xSetIntraResultChromaQT_cos(m_pcPredSearch,rpcBestCU_32x32, 0, 0, pcRecoYuv);
							memcpy( m_pcPredSearch->m_puhQTTempCbf[1], rpcBestCU_32x32->m_puhCbf[1], uiCurrPartNumb * sizeof( UChar ) );
							memcpy( m_pcPredSearch->m_puhQTTempCbf[2], rpcBestCU_32x32->m_puhCbf[2], uiCurrPartNumb * sizeof( UChar ) );
							memcpy( m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcBestCU_32x32->m_puhTransformSkip[1], uiCurrPartNumb * sizeof( UChar ) );
							memcpy( m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcBestCU_32x32->m_puhTransformSkip[2], uiCurrPartNumb * sizeof( UChar ) );
						}
					}
		  
					//----- set data -----
					memcpy( rpcBestCU_32x32->m_puhCbf[1], m_pcPredSearch->m_puhQTTempCbf[1], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcBestCU_32x32->m_puhCbf[2], m_pcPredSearch->m_puhQTTempCbf[2], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcBestCU_32x32->m_puhTransformSkip[1], m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcBestCU_32x32->m_puhTransformSkip[2], m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], uiCurrPartNumb * sizeof( UChar ) );
					
					memset(rpcBestCU_32x32->m_puhChromaIntraDir, uiBestMode, sizeof(UChar)*uiCurrPartNumb);
					rpcBestCU_32x32->m_uiTotalDistortion  += uiBestDist - uiOverallDistC;
					  
					
					//----- restore context models -----
					load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );		
						
				}
				//至此，32x32块两种模式下的CU均获得
				destoryCurYuv_cos(pcOrigYuv);  //原始Yuv可以删除了
				
				Out[0].ulpcBestCU  = (unsigned long)rpcBestCU_32x32;
				Out[0].ulpcPredYuvBest = (unsigned long)pcPredYuv;  //当前以2Nx2N模式开辟的
				Out[0].ulpcRecoYuvBest = (unsigned long)pcRecoYuv;
				Out[0].ulpcResiYuvBest = (unsigned long)pcResiYuv;
				
				Out[0].ulpcTempCU = In[0].ulpcTempCU;  //从输入获得的4个子CU组成的临时CU
				Out[0].ulpcPredYuv = In[0].ulpcPredYuv;
				Out[0].ulpcRecoYuv = In[0].ulpcRecoYuv;
				Out[0].ulpcResiYuv = In[0].ulpcResiYuv;
				
				iCount = 0 ;
			}	
		}
		window
		{
			In sliding(1,1); //输入4个16x16组成的临时最佳32x32块
			Out tumbling(1); //输出1个临时和2Nx2N模式的32x32块
		}
	};
}


//32x32块的2Nx2N帧内模式处理之后的相关编码
composite encodeRelated_32x32(output stream<unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out, 
							   input stream<unsigned long ulpcBestCU, unsigned long ulpcPredYuvBest, unsigned long ulpcRecoYuvBest, unsigned long ulpcResiYuvBest,
                                            unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>In)
{
	Out = encodeRelated_32x32(In)
	{
		int iCount = 0;
		int uiPartUnitIdx;  //记录处理的个数
		init
		{
			iCount = 0;
			uiPartUnitIdx = 0;
		}
		work
		{
			if(++iCount == 16)
			{
				//根据输入获得熵编码器指针
				//对应的YUV为pcPredYuvTemp,pcRecoYuvTemp
				//临时最佳CU和临时CU: rpcBestCU_32x32,rpcTempCU_32x32
				UInt uiDepth = 1; //-> m_puhDepth[0];
				double dRdCost = 0.0;
				int i = 0;
				
				TComDataCU* rpcBestCU = (TComDataCU*)(In[0].ulpcBestCU);  // 2Nx2N模式对应的CU
				TComYuv *pcPredYuvBest = (TComYuv*)(In[0].ulpcPredYuvBest);  //根据输入做相应转换
				TComYuv *pcRecoYuvBest = (TComYuv*)(In[0].ulpcRecoYuvBest);
				TComYuv *pcResiYuvBest = (TComYuv*)(In[0].ulpcResiYuvBest);
				
				
				TComDataCU* rpcTempCU = (TComDataCU*)(In[0].ulpcTempCU); //划分模式下的CU
				TComYuv *pcPredYuvTemp = (TComYuv*)(In[0].ulpcPredYuv);  //根据输入做相应转换
				TComYuv *pcRecoYuvTemp = (TComYuv*)(In[0].ulpcRecoYuv);
				TComYuv *pcResiYuvTemp = (TComYuv*)(In[0].ulpcResiYuv);
				
				//定义编码器
				TEncEntropy* m_pcEntropyCoder = &(m_pcEncTop->m_cEntropyCoder);

				TComRdCost* m_pcRdCost = &(m_pcEncTop->m_cRdCost);
				
				encodeIntra(m_pcCuEncoder,m_pcEntropyCoder,rpcBestCU,uiDepth);
			  
				store_cos(m_pcRDGoOnSbacCoder,m_pppcRDSbacCoder[uiDepth][2]);
			 
				rpcBestCU->m_uiTotalBits = getNumberOfWrittenBits_cos(m_pcEntropyCoder);
				rpcBestCU->m_uiTotalBins = getBinsCoded_cos(m_pcEntropyCoder);
				
				dRdCost = ((Double)rpcBestCU->m_uiTotalDistortion + (Double)((Int)(rpcBestCU->m_uiTotalBits * (m_pcRdCost->m_dLambda)+.5)));
				dRdCost = (Double)(UInt)floor(dRdCost);
				rpcBestCU->m_dTotalCost = dRdCost;
			  
				//xCheckDQP( rpcTempCU );
				//xCheckBestMode(rpcBestCU, rpcTempCU, uiDepth);
				if(rpcTempCU->m_dTotalCost < rpcBestCU->m_dTotalCost)
				{
					TComYuv* pcYuv;
					// Change Information data
					TComDataCU* pcCU = rpcBestCU;
					rpcBestCU = rpcTempCU;
					rpcTempCU = pcCU;

					// Change Prediction data
					pcYuv = pcPredYuvBest;
					pcPredYuvBest = pcPredYuvTemp;
					pcPredYuvTemp = pcYuv;
						
					// Change Reconstruction data
					pcYuv = pcRecoYuvBest;
					pcRecoYuvBest = pcRecoYuvTemp;
					pcRecoYuvTemp = pcYuv;

					pcYuv = NULL;
					pcCU  = NULL;
						
					// store temp best CI for next CU coding
					store_cos(m_pppcRDSbacCoder[uiDepth][2],m_pppcRDSbacCoder[uiDepth][1]);   
				}
					
				destoryCurCU_cos(rpcTempCU);
				destoryCurYuv_cos(pcPredYuvTemp);
				destoryCurYuv_cos(pcRecoYuvTemp);
				destoryCurYuv_cos(pcResiYuvTemp) ;  //删除临时16x16 CU开辟的空间;并置指针为NULL，
				
				
				//事实上到该for循环结束时CI_NEXT_BEST已获取,即m_pppcRDSbacCoder[2][1]状态已经完成，下一个32x32块可以进行  //通过互斥变量看能否实现
				
				//同时当4个16x16子块均处理时，可以将状态传至上一层，原代码为m_pppcRDSbacCoder[uhNextDepth][CI_NEXT_BEST]->store(m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]);
				if(++uiPartUnitIdx == 4)  
				{
					//m_pppcRDSbacCoder[uiDepth][1]->store(m_pppcRDSbacCoder[uiDepth-1][2]);
					uiPartUnitIdx = 0;
					store_cos(m_pppcRDSbacCoder[uiDepth][1], m_pppcRDSbacCoder[uiDepth-1][0]);
				}
				
				
				//最佳CU编码
				resetBits_cos(m_pcEntropyCoder);
				encodeSplitFlag_cos(m_pcEntropyCoder,rpcBestCU, 0, uiDepth, true );  // 当第3号8x8 编码后，4个8x8块拷贝至16x16临时CU中，然后对该临时CU编码，
				rpcBestCU->m_uiTotalBits += getNumberOfWrittenBits_cos(m_pcEntropyCoder); // split bits
				rpcBestCU->m_uiTotalBins += getBinsCoded_cos(m_pcEntropyCoder);
				//rpcBestCU->getTotalCost()  = m_pcRdCost->calcRdCost( rpcBestCU->getTotalBits(), rpcBestCU->getTotalDistortion() );
				dRdCost = ((Double)rpcBestCU->m_uiTotalDistortion + (Double)((Int)(rpcBestCU->m_uiTotalBits * (m_pcRdCost->m_dLambda)+.5)));
				dRdCost = (Double)(UInt)floor(dRdCost);
				rpcBestCU->m_dTotalCost = dRdCost;
					
				copyToPic_cos(rpcBestCU,uiDepth);                                                     // Copy Best data to Picture for next partition prediction.

				xCopyYuv2Pic_cos(m_pcCuEncoder,rpcBestCU,uiDepth);    //至此，一个32x32块的最佳状态已经完全确定
				
				Out[0].ulpcTempCU = (unsigned long)rpcBestCU;
				Out[0].ulpcPredYuv = (unsigned long)pcPredYuvBest;
				Out[0].ulpcRecoYuv = (unsigned long)pcRecoYuvBest;
				Out[0].ulpcResiYuv = (unsigned long)pcResiYuvBest;
				
				iCount = 0;	
			}
			
		}
		window
		{
			In sliding(1,1);
			Out tumbling(1);
		}
	};
}

//将某一32x32最终的CU状态及YUV拷贝至上一层64x64 CU的相应位置,也即64x64的分割模式
composite copyPartTo_64x64(output stream<unsigned long ulpcTempCU, unsigned long ulpcOrigYuv, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out, 
							input stream<unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>In)
{
	Out = xcopyPartTo_64x64(In)
	{
		int iCount = 0;
		int uiPartUnitIdx;
		int uiAbsZorderIdx;  //记录处理的32x32块的起始zorder;
		UChar uiDepth;
		UChar uhWidth;
        UChar uhHeight;
		
		TComDataCU* rpcTempCU_64x64; 
		TComYuv *pcOrigYuv;		
		TComYuv *pcPredYuvTemp, *pcRecoYuvTemp, *pcResiYuvTemp;
		
		init 
		{
			iCount = 0;
			uiPartUnitIdx = 0;
			uiAbsZorderIdx = 0;
			uiDepth  = 0;
			uhWidth  = 64;
			uhHeight = 64;
			rpcTempCU_64x64 = NULL;
			pcOrigYuv = NULL;  pcPredYuvTemp= NULL;   pcRecoYuvTemp = NULL;   pcResiYuvTemp = NULL;  //当前层即32x32块的临时Yuv
			                  
		}
		work
		{
			if(++iCount == 16)
			{
				//pcCU为LCU的指针
				TComPic* pcPic = pcCU->m_pcPic;
				TComPicYuv* pcPicYuvSrc = pcPic->m_apcPicYuv[0];
				
				int y;
				Pel* pDst, *pDstU, *pDstV;
				Pel* pSrc, *pSrcU, *pSrcV;
				UInt iDstStride, iSrcStride,iWidth,iCWidth;
				
				//TComYuv* m_ppcOrigYuv = NULL;
				//Int iQP = pcCU->m_phQP[0];
				
				UInt iCuAddr = pcCU->m_uiCUAddr;
				double dRdCost = 0.0;
				
				TComDataCU* pcSubBestPartCU = (TComDataCU*)(In[0].ulpcTempCU);//待完成	
				TComYuv *pcPredYuvBest_32x32 = (TComYuv*)(In[0].ulpcPredYuv);
				TComYuv *pcRecoYuvBest_32x32 = (TComYuv*)(In[0].ulpcRecoYuv); 
				TComYuv *pcResiYuvBest_32x32 = (TComYuv*)(In[0].ulpcResiYuv);  //根据输入完成
				
				//TEncEntropy m_cEntropyCoder;
				//TEncEntropy *m_pcEntropyCoder = &m_cEntropyCoder;
				
				//定义TEncSearch类型变量,
				//TEncSearch m_cSearch;  //调用构造函数完成对象初始化；由于是在栈上开辟的空间，所以退出该work后自动调用析构函数进行对象析构
				//TEncSearch* m_pcPredSearch = &m_cSearch; //获得指针
				
				//定义TComRdCost类型变量
				//TComRdCost m_cRdCost;
				//TComRdCost* m_pcRdCost = &m_cRdCost;
				TEncEntropy* m_pcEntropyCoder =  &(m_pcEncTop->m_cEntropyCoder);
			
				TComRdCost* m_pcRdCost = &(m_pcEncTop->m_cRdCost);
				if(uiPartUnitIdx== 0)  //说明是在处理一个新的64x64块
				{	
					
					createCurCU_cos(pcCU,rpcTempCU_64x64,uiDepth,uiAbsZorderIdx); 
					createCurYuv_cos(pcOrigYuv,64,64);  //创建原始YUV空间并初始化，注意释放空间
					createCurYuv_cos(pcPredYuvTemp,64,64);   //创建原始YUV空间并初始化，注意释放空间
					createCurYuv_cos(pcRecoYuvTemp,64,64);  //创建原始YUV空间并初始化，注意释放空间
					createCurYuv_cos(pcResiYuvTemp,64,64); //创建原始YUV空间并初始化，注意释放空间
					
					//pcOrigYuv 原始yuv数据拷贝
					//等价代码：m_ppcOrigYuv[1]->copyFromPicYuv( pcPic->getPicYuvOrg(), rpcBestCU->getAddr(), rpcBestCU->getZorderIdxInCU() );
					{
						int  y;
						short* pDst  = pcOrigYuv->m_apiBufY; 
						short* pDstU = pcOrigYuv->m_apiBufU;
						short* pDstV = pcOrigYuv->m_apiBufV;
						//Pel* pSrc     = pcPicYuvSrc->getLumaAddr ( iCuAddr, uiAbsZorderIdx );
						Pel* pSrc    = pcPicYuvSrc->m_piPicOrgY + pcPicYuvSrc->m_cuOffsetY[iCuAddr] + pcPicYuvSrc->m_buOffsetY[g_auiZscanToRaster[uiAbsZorderIdx]];
						Pel* pSrcU   = pcPicYuvSrc->m_piPicOrgU + pcPicYuvSrc->m_buOffsetC[iCuAddr] + pcPicYuvSrc->m_buOffsetC[g_auiZscanToRaster[uiAbsZorderIdx]];
						Pel* pSrcV   = pcPicYuvSrc->m_piPicOrgV + pcPicYuvSrc->m_buOffsetC[iCuAddr] + pcPicYuvSrc->m_buOffsetC[g_auiZscanToRaster[uiAbsZorderIdx]];
						
						//copyFromPicLuma
						UInt  iDstStride  =  pcOrigYuv->m_iWidth;
						UInt  iSrcStride  = pcPicYuvSrc->m_iPicWidth + ((pcPicYuvSrc->m_iLumaMarginX)<<1);
						UInt  iWidth = pcOrigYuv->m_iWidth;
						UInt  iCWidth = pcOrigYuv->m_iCWidth;
						for ( y = pcOrigYuv->m_iHeight; y != 0; y-- )
						{
							memcpy( pDst, pSrc, sizeof(short)*iWidth);
							pDst += iDstStride;
							pSrc += iSrcStride;
						}
						
						//copyFromPicChroma
						iDstStride = pcOrigYuv->m_iCWidth;
						iSrcStride =  (pcPicYuvSrc->m_iPicWidth)>>1 + ((pcPicYuvSrc->m_iChromaMarginX)<<1);
						for ( y = pcOrigYuv->m_iCHeight; y != 0; y-- )
						{
							memcpy( pDstU, pSrcU, sizeof(Pel)*(iCWidth) );
							memcpy( pDstV, pSrcV, sizeof(Pel)*(iCWidth) );
							pSrcU += iSrcStride;
							pSrcV += iSrcStride;
							pDstU += iDstStride;
							pDstV += iDstStride;
						} 				
					}
					
					//uiAbsZorderIdx += 256;  //每处理完一个64x64块，Zorder自增256;
				}
				
				//pcSubBestPartCU为第uiPartUnitIdx个16x16子块对应的最佳CU；需要根据输入转化为对应的指针
				//rpcTempCU_64x64->copyPartFrom( pcSubBestPartCU, uiPartUnitIdx, uiDepth+1 );         // Keep best part data to current temporary data.
				copyPartFrom_cos(rpcTempCU_64x64,pcSubBestPartCU,uiPartUnitIdx,uiDepth+1);
				
				//xCopyYuv2Tmp( pcSubBestPartCU->getTotalNumPart()*uiPartUnitIdx, uiDepth+1 ); 执行：pcRecoYuvBest[uiDepth+1]->copyToPartYuv( pcRecoYuvTemp[uiDepth], pcSubBestPartCU->getTotalNumPart()*uiPartUnitIdx );
				{
					
					//计算第uiPartUnitIdx个32x32子块在64x64块中对应位置
					UInt uiDstPartIdx = 64*uiPartUnitIdx; // pcSubBestPartCU->getTotalNumPart()*uiPartUnitIdx
					
					pSrc  = pcRecoYuvBest_32x32->m_apiBufY;
					pDst  =     pcRecoYuvTemp->m_apiBufY + ( g_auiRasterToPelX[g_auiZscanToRaster[uiDstPartIdx]] + g_auiRasterToPelY[g_auiZscanToRaster[uiDstPartIdx]]* pcRecoYuvTemp->m_iWidth );
					
					pSrcU = pcRecoYuvBest_32x32->m_apiBufU;
					pSrcV = pcRecoYuvBest_32x32->m_apiBufV;
					pDstU =     pcRecoYuvTemp->m_apiBufU + ( g_auiRasterToPelX[g_auiZscanToRaster[uiDstPartIdx]] + (g_auiRasterToPelY[g_auiZscanToRaster[uiDstPartIdx]]* pcRecoYuvTemp->m_iCWidth )>>1);
					pDstV =     pcRecoYuvTemp->m_apiBufV + ( g_auiRasterToPelX[g_auiZscanToRaster[uiDstPartIdx]] + (g_auiRasterToPelY[g_auiZscanToRaster[uiDstPartIdx]]* pcRecoYuvTemp->m_iCWidth )>>1);
	  
					iSrcStride = pcRecoYuvBest_32x32->m_iWidth;
					iDstStride =     pcRecoYuvTemp->m_iWidth;
					
					y = pcRecoYuvBest_32x32->m_iHeight;
					for( y; y!=0; y-- )
					{
						memcpy(pDst, pSrc, sizeof(Pel)*iSrcStride);
						pDst += iDstStride;
						pSrc += iSrcStride;
					}
					
					iSrcStride = pcRecoYuvBest_32x32->m_iCWidth;
					iDstStride =     pcRecoYuvTemp->m_iCWidth;
					
					y = pcRecoYuvBest_32x32->m_iCHeight;
					for ( y; y != 0; y-- )
					{
						memcpy( pDstU, pSrcU, sizeof(Pel)*(iSrcStride) );
						memcpy( pDstV, pSrcV, sizeof(Pel)*(iSrcStride) );
						pSrcU += iSrcStride;
						pSrcV += iSrcStride;
						pDstU += iDstStride;
						pDstV += iDstStride;
					}	
				}
				
				//32x32块已拷贝至相应地64x64块中，可删除
				destoryCurCU_cos(pcSubBestPartCU);
				destoryCurYuv_cos(pcPredYuvBest_32x32);
				destoryCurYuv_cos(pcRecoYuvBest_32x32);
				destoryCurYuv_cos(pcResiYuvBest_32x32);
				
				
				if(++uiPartUnitIdx == 4) //4个32x32块均接收到，对临时最佳64x64块编码
				{
					uiPartUnitIdx = 0; //重置0，用于标记下一个32x32块的开始
				
					//m_pcEntropyCoder->resetBits();  //这里的熵编码器的上一个状态是encodeRelated_8x8中第3号8x8进行encodeSplitFlag_cos编码结束之后的状态
					//m_pcEntropyCoder->encodeSplitFlag( rpcTempCU_64x64, 0, uiDepth, true );

					//rpcTempCU_64x64->getTotalBits() += m_pcEntropyCoder->getNumberOfWrittenBits(); // split bits
					//rpcTempCU_64x64->getTotalBins() += ((TEncBinCABAC *)((TEncSbac*)m_pcEntropyCoder->m_pcEntropyCoderIf)->getEncBinIf())->getBinsCoded();
					//rpcTempCU_64x64->getTotalCost()  = m_pcRdCost->calcRdCost( rpcTempCU_64x64->getTotalBits(), rpcTempCU_64x64->getTotalDistortion() );
					
					resetBits_cos(m_pcEntropyCoder);
					encodeSplitFlag_cos(m_pcEntropyCoder,rpcTempCU_64x64, 0, uiDepth, true );  // 当第3号8x8 编码后，4个8x8块拷贝至16x16临时CU中，然后对该临时CU编码，
					rpcTempCU_64x64->m_uiTotalBits += getNumberOfWrittenBits_cos(m_pcEntropyCoder); // split bits
					rpcTempCU_64x64->m_uiTotalBins += getBinsCoded_cos(m_pcEntropyCoder);
					dRdCost = ((Double)rpcTempCU_64x64->m_uiTotalDistortion + (Double)((Int)(rpcTempCU_64x64->m_uiTotalBits * (m_pcRdCost->m_dLambda)+.5)));
					dRdCost = (Double)(UInt)floor(dRdCost);
					rpcTempCU_64x64->m_dTotalCost = dRdCost;
					//m_pppcRDSbacCoder[uiDepth+1][CI_NEXT_BEST]->store(m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]);
					
					//xCheckBestMode( rpcBestCU_64x64, rpcTempCU_64x64, uiDepth);  //保存至最佳rpcBestCU_32x32
				}	

				Out[0].ulpcTempCU = (unsigned long)rpcTempCU_64x64;
				Out[0].ulpcOrigYuv= (unsigned long)pcOrigYuv;
				Out[0].ulpcPredYuv = (unsigned long)pcPredYuvTemp;
				Out[0].ulpcRecoYuv = (unsigned long)pcRecoYuvTemp;
				Out[0].ulpcResiYuv = (unsigned long)pcResiYuvTemp;
				
				iCount = 0;		
			}
	        
		}
		window
		{
			In sliding(1,1);   //输入一个最佳的32x32并处理
			Out tumbling(1);
		}	
	};
}

	
//64x64块的2Nx2N模式
composite xCheckRDCostIntra64x64_cos(output stream<unsigned long ulpcBestCU, unsigned long ulpcPredYuvBest, unsigned long ulpcRecoYuvBest, unsigned long ulpcResiYuvBest,
                                                   unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out,
									  input stream<unsigned long ulpcTempCU, unsigned long ulpcOrigYuv, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>In)
{
	Out = xCheckRDCostIntra64x64(In)
	{
		int iCount = 0;
		int uiPartUnitIdx;
		int uiAbsZorderIdx;  //记录处理的64x64块的起始zorder;
		UChar uiDepth;  //当前CU的深度
		
		init 
		{
			uiPartUnitIdx = 0;
			uiAbsZorderIdx = 0;
			uiDepth  = 0;
			
		}
		work
		{
			if(++iCount == 64)
			{
				TComDataCU* rpcBestCU_64x64 = NULL;
				TComYuv *pcPredYuv= NULL, *pcRecoYuv = NULL, *pcResiYuv = NULL;  // 2Nx2N模式对应的最佳预测，重构和残余Yuv
				//不加Best是为了和亮度和色度预测代码中各Yuv名称保持一致
			
				TComYuv *pcOrigYuv = (TComYuv *)(In[0].ulpcOrigYuv); //事实上 In[0]~In[3]中数据是一样的
				
				UInt    uiCurrPartNumb = 256>>(uiDepth<<1);
					
				 
				UInt    uiNumPU        = 1;  // 当前CU的分割模式（(SIZE_2Nx2N:1
				UInt    uiInitTrDepth  = 0 ; //!< 用于计算变换的深度，实际深度为该值+uiDepth  
				UInt    uiWidth = 64;  //!< 当前PU的宽度
				UInt    uiHeight = 64; //!< 当前PU的高度
				UInt    uiQNumParts    = 64;     
				//UInt    uiWidthBit     = pcCU->getIntraSizeIdx(0);
				UInt    uiOverallDistY = 0;
				UInt    uiOverallDistC = 0;
				UInt    CandNum = 0;
				Double  CandCostList[35];
				UInt uiPartOffset = 0;
				UInt uiPU = 0;
				
				//TEncSbac*** m_pppcRDSbacCoder;
				
				//定义TcomTrQuant类型变量
				TComTrQuant m_cTrQuant;
				TComTrQuant* m_pcTrQuant = &m_cTrQuant;
				
				//定义TEncSearch类型变量
				TEncSearch m_cSearch;  //调用构造函数完成对象初始化；由于是在栈上开辟的空间，所以退出该work后自动调用析构函数进行对象析构
				TEncSearch* m_pcPredSearch = &m_cSearch; //获得指针
				
				//定义编码器
				TEncEntropy m_cEntropyCoder;
				
				//定义TComRdCost类型变量
				TComRdCost m_cRdCost;
				TComRdCost* m_pcRdCost = &m_cRdCost;
				
				//m_cTrQuant.init(32,true,true,true,true,0);
				initTComRdCost(m_pcRdCost, &(m_pcEncTop->m_cRdCost));
				initTComTrQuant(m_pcTrQuant, &(m_pcEncTop->m_cTrQuant));
				
				//m_pcTrQuant 和 m_pcRdCost均为新创建变量的地址，而不是原m_pcEncTop下的指针
				//initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&(m_pcEncTop->m_cEntropyCoder),m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
				initTEncSearch(m_pcPredSearch,m_pcEncTop,m_pcTrQuant,64,4,1,0,&m_cEntropyCoder,m_pcRdCost,m_pcEncTop->m_pppcRDSbacCoder,&(m_pcEncTop->m_cRDGoOnSbacCoder) );
				
				createCurCU_cos(pcCU,rpcBestCU_64x64,uiDepth,uiAbsZorderIdx);
				createCurYuv_cos(pcPredYuv,64,64);//创建原始YUV空间并初始化，注意释放空间
				createCurYuv_cos(pcRecoYuv,64,64);//创建原始YUV空间并初始化，注意释放空间
				createCurYuv_cos(pcResiYuv,64,64);//创建原始YUV空间并初始化，注意释放空间
				
				uiAbsZorderIdx += uiCurrPartNumb;  //每处理完一个32x32块，Zorder自增64;
				if(uiAbsZorderIdx == 256) 
				{
					uiAbsZorderIdx = 0;
				}
				
				// rpcTempCU->setSkipFlagSubParts( false, 0, uiDepth );
				//rpcTempCU->setPartSizeSubParts( eSize, 0, uiDepth );
				//rpcTempCU->setPredModeSubParts( MODE_INTRA, 0, uiDepth );
				memset( rpcBestCU_64x64->m_skipFlag, 0, 256>>(2*uiDepth) );  //在uiDepth等于3时，有4个块
				memset( rpcBestCU_64x64->m_pePartSize,0, 256>>(2*uiDepth) );
				memset( rpcBestCU_64x64->m_pePredMode,1, 256>>(2*uiDepth) );
				
				setQPSubParts_cos( rpcBestCU_64x64, 0, uiDepth );
				
				///////////////////////////////////
				//m_pppcRDSbacCoder[uhNextDepth][CI_CURR_BEST]->load(m_pppcRDSbacCoder[uiDepth][CI_CURR_BEST]);
				//////////////////////////////////
				
				{
					//===== init pattern for luma prediction =====
					bool bAboveAvail = false;
					bool bLeftAvail  = false;
					Int numModesAvailable     = 35; //total number of Intra modes
					UInt uiRdModeList[35];
					Int numModesForFullRD = 3;  //!< MPM数目  
					Int i = 0,modeIdx = 0;
					//===== determine set of modes to be tested (using prediction signal only) =====
					//
					//Pel* piOrg         = getLumaAddr_cos( pcOrigYuv,uiPU, uiWidth );
					//Pel* piPred        = getLumaAddr_cos( pcPredYuv,uiPU, uiWidth );  //预测YUV空间需要开辟
					//UInt uiStride      = getStride_cos(pcPredYuv);
					Pel* piOrg  = pcOrigYuv->m_apiBufY + ( (uiPU * uiWidth)&(pcOrigYuv->m_iWidth - 1) + (( uiPU * uiWidth )&~( pcOrigYuv->m_iWidth - 1 )) * uiWidth );
					Pel* piPred = pcPredYuv->m_apiBufY + ( (uiPU * uiWidth)&(pcPredYuv->m_iWidth - 1) + (( uiPU * uiWidth )&~( pcOrigYuv->m_iWidth - 1 )) * uiWidth );
					UInt uiStride      = pcPredYuv->m_iWidth;
					
					// rpcBestCU_64x64->getPattern()->initPattern   ( rpcBestCU_64x64, uiInitTrDepth, uiPartOffset );
					// set luma parameters from CU data for accessing ADI data //!< 主要获取当前PU的邻域可用性，对参考样点进行设置及滤波  
					// rpcBestCU_64x64->getPattern()->initAdiPattern( rpcBestCU_64x64, uiPartOffset, uiInitTrDepth, m_piYuvExt, m_iYuvExtStride, m_iYuvExtHeight, bAboveAvail, bLeftAvail );
					initAdiPattern_cos(rpcBestCU_64x64,uiPartOffset, uiInitTrDepth, m_pcPredSearch->m_piYuvExt, m_pcPredSearch->m_iYuvExtStride, m_pcPredSearch->m_iYuvExtHeight, bAboveAvail, bLeftAvail);
				
					for(i = 0; i < numModesForFullRD; i++ ) 
					{
						CandCostList[ i ] = MAX_DOUBLE;
					}
					
					for(modeIdx = 0; modeIdx < 35; modeIdx++ )
					{
					  UInt uiMode = modeIdx;
					  UInt uiSad,iModeBits;
					  double cost;
					  predIntraLumaAng_cos(m_pcPredSearch,rpcBestCU_64x64,uiMode,piPred, uiStride, uiWidth, uiHeight, bAboveAvail, bLeftAvail);
					  
					  uiSad = calcHAD_cos(m_pcRdCost,piOrg, uiStride, piPred, uiStride, uiWidth, uiHeight);
					  
					  iModeBits = xModeBitsIntra_cos(m_pcPredSearch,rpcBestCU_64x64, uiMode, uiPU, uiPartOffset, uiDepth, uiInitTrDepth);
					  cost = (double)uiSad + (double)iModeBits * getSqrtLambda_cos(m_pcRdCost);
					  
					  CandNum = xUpdateCandList_cos(m_pcPredSearch,uiMode, cost, numModesForFullRD, uiRdModeList, CandCostList);
					}
					//快速算法
					{
						Int uiPreds[3] = {-1, -1, -1};
						Int iMode = -1;  //!< 如果三个MPMs的前两个相同，则iMode=1，否则iMode=2  
						Int numCand = getIntraDirLumaPredictor_cos( rpcBestCU_64x64,uiPartOffset, uiPreds, &iMode );  //!< 获取亮度帧内预测模式的三个MPMs  ,most probable mode
						Int i = 0,j = 0;
						if( iMode >= 0 )
						{
							numCand = iMode;
						}
					  
						for( j = 0; j < numCand; j++)
						{
							bool mostProbableModeIncluded = false;
							Int mostProbableMode = uiPreds[j];
							
							for( i = 0; i < numModesForFullRD; i++)
							{
							  mostProbableModeIncluded |= (mostProbableMode == uiRdModeList[i]);
							}
							if (!mostProbableModeIncluded)
							{
							  uiRdModeList[numModesForFullRD++] = mostProbableMode;
							}
						}
						
					}
					
					{
						UInt    uiBestPUMode  = 0;   //!< 存放最佳预测模式  
						UInt    uiBestPUDistY = 0;   //!< 存放最佳预测模式对应的亮度失真值 
						UInt    uiBestPUDistC = 0;   //!< 存放最佳预测模式对应的色度失真值 
						Double  dBestPUCost   = MAX_DOUBLE;  //!< 存放最佳预测模式对应的总代价 
						UInt uiMode = 0;
						
						UInt uiOrgMode = 0;  //存放最佳模式
						UInt uiCurrPartNumb =  256>>((uiDepth+uiInitTrDepth)<<1);
						// determine residual for partition
						UInt   uiPUDistY = 0;
						UInt   uiPUDistC = 0;
						Double dPUCost   = 0.0;
						
						for(uiMode = 0; uiMode < numModesForFullRD; uiMode++ )  //!< 遍历存储在uiRdModeList里的模式  
						{
						  // set luma prediction mode
						  UInt uiOrgMode = uiRdModeList[uiMode];
						  //rpcBestCU_64x64->setLumaIntraDirSubParts ( uiOrgMode, uiPartOffset, uiDepth + uiInitTrDepth );
						  
						  // determine residual for partition
						  UInt   uiPUDistY = 0;
						  UInt   uiPUDistC = 0;
						  Double dPUCost   = 0.0;
						  
						  memset(rpcBestCU_64x64->m_puhLumaIntraDir+uiPartOffset, uiOrgMode, sizeof(UChar)*uiCurrPartNumb);
						  
						  // set context models
						  load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
						  
						  
						  //整个亮度预测的函数中，两次调用xRecurIntraCodingQT,其中倒数第二个参数第一次为true，一次为false。该参数控制函数中的bCheckSplit的值，若为true，则TU只进行当前深度的计算，如果该值为false，则TU进行所有深度的计算。

						  xRecurIntraCodingQT_cos(m_pcPredSearch, rpcBestCU_64x64, uiInitTrDepth, uiPartOffset, true, pcOrigYuv, pcPredYuv, pcResiYuv, uiPUDistY, uiPUDistC, true, dPUCost );

						  
						  // check r-d cost
						  if( dPUCost < dBestPUCost )  //!< 更新最佳预测模式相关参数     //Cbf:coded block flag, code block pattern,用于标记亮度、色差的残差系数的额特性，是否有DC、AC
						  {
							uiBestPUMode  = uiOrgMode;
							uiBestPUDistY = uiPUDistY;
							uiBestPUDistC = uiPUDistC;
							dBestPUCost   = dPUCost;
							
							xSetIntraResultQT_cos( m_pcPredSearch,rpcBestCU_64x64, uiInitTrDepth, uiPartOffset, true, pcRecoYuv );    //将最新数据拷贝至重构图像
							
							memcpy(m_pcPredSearch->m_puhQTTempTrIdx,  rpcBestCU_64x64->m_puhTrIdx + uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[0], rpcBestCU_64x64->m_puhCbf[0]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[1], rpcBestCU_64x64->m_puhCbf[1]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[2], rpcBestCU_64x64->m_puhCbf[2]+ uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], rpcBestCU_64x64->m_puhTransformSkip[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcBestCU_64x64->m_puhTransformSkip[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcBestCU_64x64->m_puhTransformSkip[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						  }
						} // Mode loop
						
						uiOrgMode = uiBestPUMode;  //!<　设置模式为最佳预测模式  
						
						//rpcBestCU_64x64->setLumaIntraDirSubParts ( uiOrgMode, 0, uiDepth + uiInitTrDepth );
						memset(rpcBestCU_64x64->m_puhLumaIntraDir+uiPartOffset, uiOrgMode, sizeof(UChar)*uiCurrPartNumb);
						
						// set context models
						load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
						  
						
						//! 注意该函数倒数第二个参数，此时为false  
						xRecurIntraCodingQT_cos(m_pcPredSearch, rpcBestCU_64x64, uiInitTrDepth, uiPartOffset, true, pcOrigYuv, pcPredYuv, pcResiYuv, uiPUDistY, uiPUDistC, false, dPUCost );
						  
						// check r-d cost
						if( dPUCost < dBestPUCost )
						{
							uiBestPUMode  = uiOrgMode;
							uiBestPUDistY = uiPUDistY;
							uiBestPUDistC = uiPUDistC;
							dBestPUCost   = dPUCost;
							
							xSetIntraResultQT_cos( m_pcPredSearch,rpcBestCU_64x64, uiInitTrDepth, uiPartOffset, true, pcRecoYuv );    //将最新数据拷贝至重构图像
							
							memcpy(m_pcPredSearch->m_puhQTTempTrIdx,  rpcBestCU_64x64->m_puhTrIdx + uiPartOffset,  uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[0], rpcBestCU_64x64->m_puhCbf[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[1], rpcBestCU_64x64->m_puhCbf[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempCbf[2], rpcBestCU_64x64->m_puhCbf[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], rpcBestCU_64x64->m_puhTransformSkip[0]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcBestCU_64x64->m_puhTransformSkip[1]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
							memcpy(m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcBestCU_64x64->m_puhTransformSkip[2]+ uiPartOffset, uiCurrPartNumb * sizeof( UChar ) );
						}
						
						//--- update overall distortion ---
						uiOverallDistY += uiBestPUDistY;
						uiOverallDistC += uiBestPUDistC;
		
						//--- update transform index and cbf ---
						memcpy( rpcBestCU_64x64->m_puhTrIdx  + uiPartOffset, m_pcPredSearch->m_puhQTTempTrIdx,  uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_64x64->m_puhCbf[0]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[0], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_64x64->m_puhCbf[1]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[1], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_64x64->m_puhCbf[2]+ uiPartOffset,  m_pcPredSearch->m_puhQTTempCbf[2], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_64x64->m_puhTransformSkip[0]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[0], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_64x64->m_puhTransformSkip[1]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], uiCurrPartNumb * sizeof( UChar ) );
						memcpy( rpcBestCU_64x64->m_puhTransformSkip[2]+ uiPartOffset , m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], uiCurrPartNumb * sizeof( UChar ) );
						
						//=== update PU data ====
						memset(rpcBestCU_64x64->m_puhLumaIntraDir + uiPartOffset, uiBestPUMode, sizeof(UChar)*uiCurrPartNumb);
						copyToPic_cos ( rpcBestCU_64x64,uiDepth, uiPU, uiInitTrDepth );
					
					}			
				}
				
				//===== reset context models =====
				load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
	  
				//===== set distortion (rate and r-d costs are determined later) =====
				//ruiDistC                   = uiOverallDistC;
				rpcBestCU_64x64->m_uiTotalDistortion =  uiOverallDistY + uiOverallDistC;
				
				
				copyToPicLuma_cos( pcRecoYuv,rpcBestCU_64x64, rpcBestCU_64x64->m_uiCUAddr, rpcBestCU_64x64->m_uiAbsIdxInLCU);
				
				//亮度预测结束
				//	
				
				//色度预测
				{
					UInt    uiBestMode  = 0;
					UInt    uiBestDist  = 0;
					Double  dBestCost   = MAX_DOUBLE;
		  
					//----- init mode list -----
					UInt  uiMinMode = 0;
					UInt  uiModeList[ 5 ] = {0,26,10,1,36};
					UInt  uiLumaMode = rpcBestCU_64x64->m_puhLumaIntraDir[0];
					UInt  uiMaxMode = 5;
					UInt  uiCurrPartNumb = 256>>(uiDepth<<1);
					Int i;
					UInt uiMode;
					for(i = 0; i < 4; i++ )
					{
						if( uiLumaMode == uiModeList[i] )
						{
						  uiModeList[i] = 34; // VER+8 mode
						  break;
						}
					}
					
					//----- check chroma modes -----
					for(uiMode = uiMinMode; uiMode < uiMaxMode; uiMode++ )
					{
						UInt    uiDist = 0;
						UInt    uiBits;
						Double  dCost;
						//----- restore context models -----
						load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
			
						//----- chroma coding -----
						//rpcBestCU_64x64->setChromIntraDirSubParts  ( uiModeList[uiMode], 0, uiDepth );
						memset(rpcBestCU_64x64->m_puhChromaIntraDir, uiModeList[uiMode], sizeof(UChar)*uiCurrPartNumb);
						
						xRecurIntraChromaCodingQT_cos(m_pcPredSearch, rpcBestCU_64x64,  0, 0, pcOrigYuv, pcPredYuv, pcResiYuv, uiDist );
						
						load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );
						
						uiBits = xGetIntraBitsQT_cos(m_pcPredSearch, rpcBestCU_64x64, 0, 0, false, true, false );
						dCost  = ((Double)uiDist + (Double)((Int)(m_pcRdCost->m_dLambda * uiBits +.5)));
						dCost = (Double)(UInt)floor(dCost);
			
						//----- compare -----
						if( dCost < dBestCost )
						{
							dBestCost   = dCost;
							uiBestDist  = uiDist;
							uiBestMode  = uiModeList[uiMode];
							//xSetIntraResultChromaQT( rpcBestCU_64x64, 0, 0, pcRecoYuv );
							xSetIntraResultChromaQT_cos(m_pcPredSearch,rpcBestCU_64x64, 0, 0, pcRecoYuv);
							memcpy( m_pcPredSearch->m_puhQTTempCbf[1], rpcBestCU_64x64->m_puhCbf[1], uiCurrPartNumb * sizeof( UChar ) );
							memcpy( m_pcPredSearch->m_puhQTTempCbf[2], rpcBestCU_64x64->m_puhCbf[2], uiCurrPartNumb * sizeof( UChar ) );
							memcpy( m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], rpcBestCU_64x64->m_puhTransformSkip[1], uiCurrPartNumb * sizeof( UChar ) );
							memcpy( m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], rpcBestCU_64x64->m_puhTransformSkip[2], uiCurrPartNumb * sizeof( UChar ) );
						}
					}
		  
					//----- set data -----
					memcpy( rpcBestCU_64x64->m_puhCbf[1], m_pcPredSearch->m_puhQTTempCbf[1], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcBestCU_64x64->m_puhCbf[2], m_pcPredSearch->m_puhQTTempCbf[2], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcBestCU_64x64->m_puhTransformSkip[1], m_pcPredSearch->m_puhQTTempTransformSkipFlag[1], uiCurrPartNumb * sizeof( UChar ) );
					memcpy( rpcBestCU_64x64->m_puhTransformSkip[2], m_pcPredSearch->m_puhQTTempTransformSkipFlag[2], uiCurrPartNumb * sizeof( UChar ) );
					
					memset(rpcBestCU_64x64->m_puhChromaIntraDir, uiBestMode, sizeof(UChar)*uiCurrPartNumb);
					rpcBestCU_64x64->m_uiTotalDistortion  += uiBestDist - uiOverallDistC;
					  
					
					//----- restore context models -----
					load_cos(m_pcPredSearch->m_pcRDGoOnSbacCoder,m_pcPredSearch->m_pppcRDSbacCoder[uiDepth][0] );		
						
				}
				//至此，64x64块两种模式下的CU均获得	
				destoryCurYuv_cos(pcOrigYuv);  //原始Yuv可以删除了
				
				Out[0].ulpcBestCU  = (unsigned long)rpcBestCU_64x64;
				Out[0].ulpcPredYuvBest = (unsigned long)pcPredYuv;  //当前以2Nx2N模式开辟的
				Out[0].ulpcRecoYuvBest = (unsigned long)pcRecoYuv;
				Out[0].ulpcResiYuvBest = (unsigned long)pcResiYuv;
				
				Out[0].ulpcTempCU = In[0].ulpcTempCU;  //从输入获得的4个子CU组成的临时CU
				Out[0].ulpcPredYuv = In[0].ulpcPredYuv;
				Out[0].ulpcRecoYuv = In[0].ulpcRecoYuv;
				Out[0].ulpcResiYuv = In[0].ulpcResiYuv;
				
				iCount = 0;
			}
			
		}
		window
		{
			In sliding(1,1); //输入4个32x32块组成临时64x64块
			Out tumbling(1); //输出1个最佳64x64块
		}
	};
}

composite Main()
{
	stream<unsigned long ulpcOrigYuv>Out;
	stream<unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out1,Out4,Out7;
	stream<unsigned long ulpcTempCU, unsigned long ulpcOrigYuv, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out2,Out5,Out8;
	stream<unsigned long ulpcBestCU, unsigned long ulpcPredYuvBest, unsigned long ulpcRecoYuvBest, unsigned long ulpcResiYuvBest,
           unsigned long ulpcTempCU, unsigned long ulpcPredYuv, unsigned long ulpcRecoYuv, unsigned long ulpcResiYuv>Out3,Out6,Out9;
	
	
	Out = initSubCU_8x8()
	{
		UInt uiAbsZorderIdx;
		init
		{
			uiAbsZorderIdx = 0;
		}
		work
		{
			 UChar       uiDepth       = 3;
			 UChar uhWidth  = (64>>uiDepth);
             UChar uhHeight = (64>>uiDepth);
			
			 TComPic* pcPic = pcCU->m_pcPic;   //PC为当前LCU的指针
			 TComPicYuv* pcPicYuvSrc = pcPic->m_apcPicYuv[0];
			 TComYuv *pcOrigYuv = NULL;
			 
			 
			 Int iQP = pcCU->m_phQP[0];
			 UInt iCuAddr = pcCU->m_uiCUAddr;
			 
			 createCurYuv_cos(pcOrigYuv,8,8);  //创建原始YUV空间并初始化，注意释放空间
			  
			//原始yuv数据拷贝
			{
				int  y;
				short* pDst  = pcOrigYuv->m_apiBufY; 
				short* pDstU = pcOrigYuv->m_apiBufU;
				short* pDstV = pcOrigYuv->m_apiBufV;
				//Pel* pSrc     = pcPicYuvSrc->getLumaAddr ( iCuAddr, uiAbsZorderIdx );
				Pel* pSrc    = pcPicYuvSrc->m_piPicOrgY + pcPicYuvSrc->m_cuOffsetY[iCuAddr] + pcPicYuvSrc->m_buOffsetY[g_auiZscanToRaster[uiAbsZorderIdx]];
				Pel* pSrcU   = pcPicYuvSrc->m_piPicOrgU + pcPicYuvSrc->m_cuOffsetC[iCuAddr] + pcPicYuvSrc->m_buOffsetC[g_auiZscanToRaster[uiAbsZorderIdx]];
				Pel* pSrcV   = pcPicYuvSrc->m_piPicOrgV + pcPicYuvSrc->m_cuOffsetC[iCuAddr] + pcPicYuvSrc->m_buOffsetC[g_auiZscanToRaster[uiAbsZorderIdx]];
				
				//copyFromPicLuma
				UInt  iDstStride  =  pcOrigYuv->m_iWidth;
				UInt  iSrcStride  = pcPicYuvSrc->m_iPicWidth + ((pcPicYuvSrc->m_iLumaMarginX)<<1);
				UInt  iWidth = pcOrigYuv->m_iWidth;
				UInt  iCWidth = pcOrigYuv->m_iCWidth;
				
				for ( y = pcOrigYuv->m_iHeight; y != 0; y-- )
				{
					memcpy( pDst, pSrc, sizeof(short)*iWidth);
                    pDst += iDstStride;
                    pSrc += iSrcStride;
                }
			    
				//copyFromPicChroma
				iDstStride = pcOrigYuv->m_iCWidth;
				iSrcStride =  ((pcPicYuvSrc->m_iPicWidth)>>1) + ((pcPicYuvSrc->m_iChromaMarginX)<<1);
				for ( y = pcOrigYuv->m_iCHeight; y != 0; y-- )
				{
					memcpy( pDstU, pSrcU, sizeof(Pel)*(iCWidth) );
					memcpy( pDstV, pSrcV, sizeof(Pel)*(iCWidth) );
					pSrcU += iSrcStride;
					pSrcV += iSrcStride;
					pDstU += iDstStride;
					pDstV += iDstStride;
				} 
			}
			
			Out[0].ulpcOrigYuv = (unsigned long)pcOrigYuv;
			
			uiAbsZorderIdx += 4;  //每处理完一个8x8块，Zorder自增4;
			if(uiAbsZorderIdx == 256) 
			{
				uiAbsZorderIdx = 0;
			}
		} 
		window
		{
			Out tumbling(1);
		}
	};
	
	
	/***********8x8块预测*****************/
	Out1 = xCheckRDCostIntra8x8_cos(Out)();
	
	Out2 = copyPartTo_16x16(Out1)();
	///////////////16x16块临时最佳CU得到//////////////////
	Out3 = xCheckRDCostIntra16x16_cos(Out2)();
	Out4 = encodeRelated_16x16(Out3)();
	//16x16块最佳CU得到
	Out5 = copyPartTo_32x32(Out4)();
	
	
	//32x32块临时最佳CU得到
	Out6 = xCheckRDCostIntra32x32_cos(Out5)();
	
	Out7 = encodeRelated_32x32(Out6)();
	
	//32x32块最佳CU得到//
	Out8 = copyPartTo_64x64(Out7)();
	//64x64块临时CU得到//
	Out9 = xCheckRDCostIntra64x64_cos(Out8)();
	encodeRelated_64x64(Out9)
	{
		int iCount = 0;
		int uiPartUnitIdx;  //记录处理的个数
		init
		{
			uiPartUnitIdx = 0;
		}
		work
		{
			if(++iCount == 64)
			{
				//根据输入获得熵编码器指针
				//对应的YUV为pcPredYuvTemp,pcRecoYuvTemp
				//临时最佳CU和临时CU：rpcBestCU_64x64,rpcTempCU_64x64
				UInt uiDepth = 0; //-> m_puhDepth[0];
				double dRdCost;
				
				TComDataCU* rpcBestCU = (TComDataCU*)(Out9[0].ulpcBestCU);  // 2Nx2N模式对应的CU
				TComYuv *pcPredYuvBest = (TComYuv*)(Out9[0].ulpcPredYuvBest);  //根据输入做相应转换
				TComYuv *pcRecoYuvBest = (TComYuv*)(Out9[0].ulpcRecoYuvBest);
				TComYuv *pcResiYuvBest = (TComYuv*)(Out9[0].ulpcResiYuvBest);
				
				
				TComDataCU* rpcTempCU = (TComDataCU*)(Out9[0].ulpcTempCU); //划分模式下的CU
				TComYuv *pcPredYuvTemp = (TComYuv*)(Out9[0].ulpcPredYuv);  //根据输入做相应转换
				TComYuv *pcRecoYuvTemp = (TComYuv*)(Out9[0].ulpcRecoYuv);
				TComYuv *pcResiYuvTemp = (TComYuv*)(Out9[0].ulpcResiYuv);
				
				TEncEntropy* m_pcEntropyCoder =  &(m_pcEncTop->m_cEntropyCoder);
			
				TComRdCost* m_pcRdCost = &(m_pcEncTop->m_cRdCost);
				
				//定义TEncSearch类型变量,
				//TEncSearch m_cSearch;  //调用构造函数完成对象初始化；由于是在栈上开辟的空间，所以退出该work后自动调用析构函数进行对象析构
				//TEncSearch* m_pcPredSearch = &m_cSearch; //获得指针
				
				//定义TComRdCost类型变量
				//TComRdCost m_cRdCost;
				//TComRdCost* m_pcRdCost = &m_cRdCost;
				
				encodeIntra(m_pcCuEncoder,m_pcEntropyCoder,rpcBestCU,uiDepth);
			  
				store_cos(m_pcRDGoOnSbacCoder,m_pppcRDSbacCoder[uiDepth][2]);
			 
				rpcBestCU->m_uiTotalBits = getNumberOfWrittenBits_cos(m_pcEntropyCoder);
				rpcBestCU->m_uiTotalBins = getBinsCoded_cos(m_pcEntropyCoder);
				
				dRdCost = ((Double)rpcBestCU->m_uiTotalDistortion + (Double)((Int)(rpcBestCU->m_uiTotalBits * (m_pcRdCost->m_dLambda)+.5)));
				dRdCost = (Double)(UInt)floor(dRdCost);
				rpcBestCU->m_dTotalCost = dRdCost;
			  
				//xCheckDQP( rpcTempCU );
				//xCheckBestMode(rpcBestCU, rpcTempCU, uiDepth);
				if(rpcTempCU->m_dTotalCost < rpcBestCU->m_dTotalCost)
				{
					TComYuv* pcYuv;
					// Change Information data
					TComDataCU* pcCU = rpcBestCU;
					rpcBestCU = rpcTempCU;
					rpcTempCU = pcCU;

					// Change Prediction data
					pcYuv = pcPredYuvBest;
					pcPredYuvBest = pcPredYuvTemp;
					pcPredYuvTemp = pcYuv;
						
					// Change Reconstruction data
					pcYuv = pcRecoYuvBest;
					pcRecoYuvBest = pcRecoYuvTemp;
					pcRecoYuvTemp = pcYuv;

					pcYuv = NULL;
					pcCU  = NULL;
						
					// store temp best CI for next CU coding
					store_cos(m_pppcRDSbacCoder[uiDepth][2],m_pppcRDSbacCoder[uiDepth][1]);   
				}
					
				destoryCurCU_cos(rpcTempCU);
				destoryCurYuv_cos(pcPredYuvTemp);
				destoryCurYuv_cos(pcRecoYuvTemp);
				destoryCurYuv_cos(pcResiYuvTemp) ;  //删除临时16x16 CU开辟的空间;并置指针为NULL，
				
				
				//事实上到该for循环结束时CI_NEXT_BEST已获取,即m_pppcRDSbacCoder[2][1]状态已经完成，下一个32x32块可以进行  //通过互斥变量看能否实现
				
				//同时当4个16x16子块均处理时，可以将状态传至上一层，原代码为m_pppcRDSbacCoder[uhNextDepth][CI_NEXT_BEST]->store(m_pppcRDSbacCoder[uiDepth][CI_TEMP_BEST]);
				
				//if(++uiPartUnitIdx == 4)  
				//{
					//m_pppcRDSbacCoder[uiDepth][1]->store(m_pppcRDSbacCoder[uiDepth-1][2]);
					//store_cos(m_pppcRDSbacCoder[m_pppcRDSbacCoder[uiDepth][1],uiDepth-1][2]);
					//uiPartUnitIdx = 0;
				//}
				
				
				resetBits_cos(m_pcEntropyCoder);
				encodeSplitFlag_cos(m_pcEntropyCoder,rpcBestCU, 0, uiDepth, true );  // 当第3号8x8 编码后，4个8x8块拷贝至16x16临时CU中，然后对该临时CU编码，
				rpcBestCU->m_uiTotalBits += getNumberOfWrittenBits_cos(m_pcEntropyCoder); // split bits
				rpcBestCU->m_uiTotalBins += getBinsCoded_cos(m_pcEntropyCoder);
				//rpcBestCU->getTotalCost()  = m_pcRdCost->calcRdCost( rpcBestCU->getTotalBits(), rpcBestCU->getTotalDistortion() );
				dRdCost = ((Double)rpcBestCU->m_uiTotalDistortion + (Double)((Int)(rpcBestCU->m_uiTotalBits * (m_pcRdCost->m_dLambda)+.5)));
				dRdCost = (Double)(UInt)floor(dRdCost);
				rpcBestCU->m_dTotalCost = dRdCost;
					
				copyToPic_cos(rpcBestCU,uiDepth);                                                     // Copy Best data to Picture for next partition prediction.

				xCopyYuv2Pic_cos(m_pcCuEncoder,rpcBestCU,uiDepth);    //至此，一个64x64块的最佳状态已经完全确定
				
				destoryCurCU_cos(rpcBestCU);
				destoryCurYuv_cos(pcPredYuvBest);
				destoryCurYuv_cos(pcRecoYuvBest);
				destoryCurYuv_cos(pcResiYuvBest) ; 
				
				iCount = 0;
				
			} 
		}
		window
		{
			Out9 sliding(1,1);
		}
	};
	//64x64块最佳CU得到//
}