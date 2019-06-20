//ϵͳ
#ifdef WIN32
#include "stdafx.h"
#endif

#include "vnfemas.h"
#include "pybind11/pybind11.h"
#include "femas/USTPFtdcTraderApi.h"


using namespace pybind11;

//����
#define ONFRONTCONNECTED 0
#define ONQRYFRONTCONNECTED 1
#define ONFRONTDISCONNECTED 2
#define ONQRYFRONTDISCONNECTED 3
#define ONHEARTBEATWARNING 4
#define ONPACKAGESTART 5
#define ONPACKAGEEND 6
#define ONRSPERROR 7
#define ONRSPUSERLOGIN 8
#define ONRSPUSERLOGOUT 9
#define ONRSPUSERPASSWORDUPDATE 10
#define ONRSPORDERINSERT 11
#define ONRSPORDERACTION 12
#define ONRSPQUOTEINSERT 13
#define ONRSPQUOTEACTION 14
#define ONRSPFORQUOTE 15
#define ONRSPMARGINCOMBACTION 16
#define ONRSPUSERDEPOSIT 17
#define ONRSPTRANSFERMONEY 18
#define ONRTNFLOWMESSAGECANCEL 19
#define ONRTNTRADE 20
#define ONRTNORDER 21
#define ONERRRTNORDERINSERT 22
#define ONERRRTNORDERACTION 23
#define ONRTNINSTRUMENTSTATUS 24
#define ONRTNINVESTORACCOUNTDEPOSIT 25
#define ONRTNQUOTE 26
#define ONERRRTNQUOTEINSERT 27
#define ONERRRTNQUOTEACTION 28
#define ONRTNFORQUOTE 29
#define ONRTNMARGINCOMBINATIONLEG 30
#define ONRTNMARGINCOMBACTION 31
#define ONRTNUSERDEPOSIT 32
#define ONRSPQUERYUSERLOGIN 33
#define ONRSPQRYORDER 34
#define ONRSPQRYTRADE 35
#define ONRSPQRYUSERINVESTOR 36
#define ONRSPQRYTRADINGCODE 37
#define ONRSPQRYINVESTORACCOUNT 38
#define ONRSPQRYINSTRUMENT 39
#define ONRSPQRYEXCHANGE 40
#define ONRSPQRYINVESTORPOSITION 41
#define ONRSPQRYCOMPLIANCEPARAM 42
#define ONRSPQRYINVESTORFEE 43
#define ONRSPQRYINVESTORMARGIN 44
#define ONRSPQRYINVESTORCOMBPOSITION 45
#define ONRSPQRYINVESTORLEGPOSITION 46
#define ONRSPQRYINSTRUMENTGROUP 47
#define ONRSPQRYCLIENTMARGINCOMBTYPE 48
#define ONRSPEXECORDERINSERT 49
#define ONRSPEXECORDERACTION 50
#define ONRTNEXECORDER 51
#define ONERRRTNEXECORDERINSERT 52
#define ONERRRTNEXECORDERACTION 53
#define ONRTNTRANSFERMONEY 54
#define ONRSPQRYSYSTEMTIME 55



///-------------------------------------------------------------------------------------
///C++ SPI�Ļص���������ʵ��
///-------------------------------------------------------------------------------------

//API�ļ̳�ʵ��
class TdApi : public CUstpFtdcTraderSpi
{
private:
	CUstpFtdcTraderApi* api;			//API����
	thread task_thread;					//�����߳�ָ�루��python���������ݣ�
	TaskQueue task_queue;			    //�������
	bool active = false;				//����״̬

public:
	TdApi()
	{
	};

	~TdApi()
	{
		if (this->active)
		{
			this->exit();
		}
	};

	//-------------------------------------------------------------------------------------
	//API�ص�����
	//-------------------------------------------------------------------------------------

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();
	virtual void OnQryFrontConnected();
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected(int nReason) ;
	virtual void OnQryFrontDisconnected(int nReason);

	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	///@param nTimeLapse �����ϴν��ձ��ĵ�ʱ��
	virtual void OnHeartBeatWarning(int nTimeLapse) ;

	virtual void OnPackageStart(int nTopicID, int nSequenceNo);
	virtual void OnPackageEnd(int nTopicID, int nSequenceNo);

	///����Ӧ��
	virtual void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///���ǰ��ϵͳ�û���¼Ӧ��
	virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///�û��˳�Ӧ��
	virtual void OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///�û������޸�Ӧ��
	virtual void OnRspUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///����¼��Ӧ��
	virtual void OnRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///��������Ӧ��
	virtual void OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///����¼��Ӧ��
	virtual void OnRspQuoteInsert(CUstpFtdcInputQuoteField *pInputQuote, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///���۲���Ӧ��
	virtual void OnRspQuoteAction(CUstpFtdcQuoteActionField *pQuoteAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///ѯ������Ӧ��
	virtual void OnRspForQuote(CUstpFtdcReqForQuoteField *pReqForQuote, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///�ͻ��������Ӧ��
	virtual void OnRspMarginCombAction(CUstpFtdcInputMarginCombActionField *pInputMarginCombAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///�û���������Ӧ��
	virtual void OnRspUserDeposit(CUstpFtdcstpUserDepositField *pstpUserDeposit, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///�û�����ϯ�����Ӧ��
	virtual void OnRspTransferMoney(CUstpFtdcstpTransferMoneyField *pstpTransferMoney, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///����������֪ͨ
	virtual void OnRtnFlowMessageCancel(CUstpFtdcFlowMessageCancelField *pFlowMessageCancel) ;

	///�ɽ��ر�
	virtual void OnRtnTrade(CUstpFtdcTradeField *pTrade) ;

	///�����ر�
	virtual void OnRtnOrder(CUstpFtdcOrderField *pOrder) ;

	///����¼�����ر�
	virtual void OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo) ;

	///������������ر�
	virtual void OnErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo) ;

	///��Լ����״̬֪ͨ
	virtual void OnRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *pInstrumentStatus) ;

	///�˻������ر�
	virtual void OnRtnInvestorAccountDeposit(CUstpFtdcInvestorAccountDepositResField *pInvestorAccountDepositRes) ;

	///���ۻر�
	virtual void OnRtnQuote(CUstpFtdcRtnQuoteField *pRtnQuote) ;

	///����¼�����ر�
	virtual void OnErrRtnQuoteInsert(CUstpFtdcInputQuoteField *pInputQuote, CUstpFtdcRspInfoField *pRspInfo) ;

	///���۳�������ر�
	virtual void OnErrRtnQuoteAction(CUstpFtdcQuoteActionField *pQuoteAction, CUstpFtdcRspInfoField *pRspInfo) ;

	///ѯ�ۻر�
	virtual void OnRtnForQuote(CUstpFtdcReqForQuoteField *pReqForQuote) ;

	///������Ϲ���֪ͨ
	virtual void OnRtnMarginCombinationLeg(CUstpFtdcMarginCombinationLegField *pMarginCombinationLeg) ;

	///�ͻ��������ȷ��
	virtual void OnRtnMarginCombAction(CUstpFtdcInputMarginCombActionField *pInputMarginCombAction) ;

	///�û���������
	virtual void OnRtnUserDeposit(CUstpFtdcstpUserDepositField *pstpUserDeposit) ;

	///��ѯǰ��ϵͳ�û���¼Ӧ��
	virtual void OnRspQueryUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///������ѯӦ��
	virtual void OnRspQryOrder(CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///�ɽ�����ѯӦ��
	virtual void OnRspQryTrade(CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///����Ͷ�����˻���ѯӦ��
	virtual void OnRspQryUserInvestor(CUstpFtdcRspUserInvestorField *pRspUserInvestor, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///���ױ����ѯӦ��
	virtual void OnRspQryTradingCode(CUstpFtdcRspTradingCodeField *pRspTradingCode, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///Ͷ�����ʽ��˻���ѯӦ��
	virtual void OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///��Լ��ѯӦ��
	virtual void OnRspQryInstrument(CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///��������ѯӦ��
	virtual void OnRspQryExchange(CUstpFtdcRspExchangeField *pRspExchange, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///Ͷ���ֲֲ߳�ѯӦ��
	virtual void OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///�Ϲ������ѯӦ��
	virtual void OnRspQryComplianceParam(CUstpFtdcRspComplianceParamField *pRspComplianceParam, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///Ͷ�����������ʲ�ѯӦ��
	virtual void OnRspQryInvestorFee(CUstpFtdcInvestorFeeField *pInvestorFee, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///Ͷ���߱�֤���ʲ�ѯӦ��
	virtual void OnRspQryInvestorMargin(CUstpFtdcInvestorMarginField *pInvestorMargin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///���ױ�����ϳֲֲ�ѯӦ��
	virtual void OnRspQryInvestorCombPosition(CUstpFtdcRspInvestorCombPositionField *pRspInvestorCombPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///���ױ��뵥�ȳֲֲ�ѯӦ��
	virtual void OnRspQryInvestorLegPosition(CUstpFtdcRspInvestorLegPositionField *pRspInvestorLegPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///��Լ����Ϣ��ѯӦ��
	virtual void OnRspQryInstrumentGroup(CUstpFtdcRspInstrumentGroupField *pRspInstrumentGroup, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///��ϱ�֤�����Ͳ�ѯӦ��
	virtual void OnRspQryClientMarginCombType(CUstpFtdcRspClientMarginCombTypeField *pRspClientMarginCombType, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///��Ȩ¼��Ӧ��
	virtual void OnRspExecOrderInsert(CUstpFtdcInputExecOrderField *pInputExecOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///��Ȩ����Ӧ��
	virtual void OnRspExecOrderAction(CUstpFtdcInputExecOrderActionField *pInputExecOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///��Ȩ֪ͨ
	virtual void OnRtnExecOrder(CUstpFtdcExecOrderField *pExecOrder) ;

	///��Ȩ¼�����ر�
	virtual void OnErrRtnExecOrderInsert(CUstpFtdcInputExecOrderField *pInputExecOrder, CUstpFtdcRspInfoField *pRspInfo) ;

	///��Ȩ��������ر�
	virtual void OnErrRtnExecOrderAction(CUstpFtdcInputExecOrderActionField *pInputExecOrderAction, CUstpFtdcRspInfoField *pRspInfo) ;

	///����ϯ�ʽ�ͬ��֪ͨ
	virtual void OnRtnTransferMoney(CUstpFtdcSyncMoneyTransferField *pSyncMoneyTransfer) ;

	///ϵͳʱ���ѯӦ��
	virtual void OnRspQrySystemTime(CUstpFtdcRspQrySystemTimeField *pRspQrySystemTime, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;


	//-------------------------------------------------------------------------------------
	//task������
	//-------------------------------------------------------------------------------------

	void processTask();

	void processFrontConnected(Task *task);

	void processQryFrontConnected(Task *task);

	void processFrontDisconnected(Task *task);

	void processQryFrontDisconnected(Task *task);

	void processHeartBeatWarning(Task *task);

	void processPackageStart(Task *task);

	void processPackageEnd(Task *task);

	void processRspError(Task *task);

	void processRspUserLogin(Task *task);

	void processRspUserLogout(Task *task);

	void processRspUserPasswordUpdate(Task *task);

	void processRspOrderInsert(Task *task);

	void processRspOrderAction(Task *task);

	void processRspQuoteInsert(Task *task);

	void processRspQuoteAction(Task *task);

	void processRspForQuote(Task *task);

	void processRspMarginCombAction(Task *task);

	void processRspUserDeposit(Task *task);

	void processRspTransferMoney(Task *task);

	void processRtnFlowMessageCancel(Task *task);

	void processRtnTrade(Task *task);

	void processRtnOrder(Task *task);

	void processErrRtnOrderInsert(Task *task);

	void processErrRtnOrderAction(Task *task);

	void processRtnInstrumentStatus(Task *task);

	void processRtnInvestorAccountDeposit(Task *task);

	void processRtnQuote(Task *task);

	void processErrRtnQuoteInsert(Task *task);

	void processErrRtnQuoteAction(Task *task);

	void processRtnForQuote(Task *task);

	void processRtnMarginCombinationLeg(Task *task);

	void processRtnMarginCombAction(Task *task);

	void processRtnUserDeposit(Task *task);

	void processRspQueryUserLogin(Task *task);

	void processRspQryOrder(Task *task);

	void processRspQryTrade(Task *task);

	void processRspQryUserInvestor(Task *task);

	void processRspQryTradingCode(Task *task);

	void processRspQryInvestorAccount(Task *task);

	void processRspQryInstrument(Task *task);

	void processRspQryExchange(Task *task);

	void processRspQryInvestorPosition(Task *task);

	void processRspQryComplianceParam(Task *task);

	void processRspQryInvestorFee(Task *task);

	void processRspQryInvestorMargin(Task *task);

	void processRspQryInvestorCombPosition(Task *task);

	void processRspQryInvestorLegPosition(Task *task);

	void processRspQryInstrumentGroup(Task *task);

	void processRspQryClientMarginCombType(Task *task);

	void processRspExecOrderInsert(Task *task);

	void processRspExecOrderAction(Task *task);

	void processRtnExecOrder(Task *task);

	void processErrRtnExecOrderInsert(Task *task);

	void processErrRtnExecOrderAction(Task *task);

	void processRtnTransferMoney(Task *task);

	void processRspQrySystemTime(Task *task);



	//-------------------------------------------------------------------------------------
	//data���ص������������ֵ�
	//error���ص������Ĵ����ֵ�
	//id������id
	//last���Ƿ�Ϊ��󷵻�
	//i������
	//-------------------------------------------------------------------------------------
	virtual void onFrontConnected() {};

	virtual void onQryFrontConnected() {};

	virtual void onFrontDisconnected(int reqid) {};

	virtual void onQryFrontDisconnected(int reqid) {};

	virtual void onHeartBeatWarning(int reqid) {};

	virtual void onPackageStart(int reqid) {};

	virtual void onPackageEnd(int reqid) {};

	virtual void onRspError(const dict &data, int reqid, bool last) {};

	virtual void onRspUserLogin(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspUserLogout(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspUserPasswordUpdate(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspOrderInsert(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspOrderAction(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQuoteInsert(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQuoteAction(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspForQuote(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspMarginCombAction(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspUserDeposit(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspTransferMoney(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRtnFlowMessageCancel(const dict &data) {};

	virtual void onRtnTrade(const dict &data) {};

	virtual void onRtnOrder(const dict &data) {};

	virtual void onErrRtnOrderInsert(const dict &data, const dict &error) {};

	virtual void onErrRtnOrderAction(const dict &data, const dict &error) {};

	virtual void onRtnInstrumentStatus(const dict &data) {};

	virtual void onRtnInvestorAccountDeposit(const dict &data) {};

	virtual void onRtnQuote(const dict &data) {};

	virtual void onErrRtnQuoteInsert(const dict &data, const dict &error) {};

	virtual void onErrRtnQuoteAction(const dict &data, const dict &error) {};

	virtual void onRtnForQuote(const dict &data) {};

	virtual void onRtnMarginCombinationLeg(const dict &data) {};

	virtual void onRtnMarginCombAction(const dict &data) {};

	virtual void onRtnUserDeposit(const dict &data) {};

	virtual void onRspQueryUserLogin(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryOrder(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryTrade(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryUserInvestor(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryTradingCode(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryInvestorAccount(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryInstrument(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryExchange(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryInvestorPosition(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryComplianceParam(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryInvestorFee(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryInvestorMargin(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryInvestorCombPosition(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryInvestorLegPosition(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryInstrumentGroup(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspQryClientMarginCombType(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspExecOrderInsert(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRspExecOrderAction(const dict &data, const dict &error, int reqid, bool last) {};

	virtual void onRtnExecOrder(const dict &data) {};

	virtual void onErrRtnExecOrderInsert(const dict &data, const dict &error) {};

	virtual void onErrRtnExecOrderAction(const dict &data, const dict &error) {};

	virtual void onRtnTransferMoney(const dict &data) {};

	virtual void onRspQrySystemTime(const dict &data, const dict &error, int reqid, bool last) {};



	//-------------------------------------------------------------------------------------
	//req:���������������ֵ�
	//-------------------------------------------------------------------------------------

	void createFtdcTraderApi(string pszFlowPath = "");

	void release();

	void init();

	int join();

	int exit();

	string getTradingDay();

	void registerFront(string pszFrontAddress);

	void subscribePrivateTopic(int nType);

	void subscribePublicTopic(int nType);

	void subscribeUserTopic(int nType);

	int reqUserLogin(const dict &req, int reqid);

	int reqUserLogout(const dict &req, int reqid);

	int reqUserPasswordUpdate(const dict &req, int reqid);

	int reqOrderInsert(const dict &req, int reqid);

	int reqOrderAction(const dict &req, int reqid);

	int reqQuoteInsert(const dict &req, int reqid);

	int reqQuoteAction(const dict &req, int reqid);

	int reqForQuote(const dict &req, int reqid);

	int reqMarginCombAction(const dict &req, int reqid);

	int reqUserDeposit(const dict &req, int reqid);

	int reqTransferMoney(const dict &req, int reqid);

	int reqQryOrder(const dict &req, int reqid);

	int reqQryTrade(const dict &req, int reqid);

	int reqQryUserInvestor(const dict &req, int reqid);

	int reqQryTradingCode(const dict &req, int reqid);

	int reqQryInvestorAccount(const dict &req, int reqid);

	int reqQryInstrument(const dict &req, int reqid);

	int reqQryExchange(const dict &req, int reqid);

	int reqQryInvestorPosition(const dict &req, int reqid);

	int reqQryComplianceParam(const dict &req, int reqid);

	int reqQryInvestorFee(const dict &req, int reqid);

	int reqQryInvestorMargin(const dict &req, int reqid);

	int reqQryInvestorCombPosition(const dict &req, int reqid);

	int reqQryInvestorLegPosition(const dict &req, int reqid);

	int reqQryInstrumentGroup(const dict &req, int reqid);

	int reqQryClientMarginCombType(const dict &req, int reqid);

	int reqExecOrderInsert(const dict &req, int reqid);

	int reqExecOrderAction(const dict &req, int reqid);

	int reqQrySystemTime(const dict &req, int reqid);
};
