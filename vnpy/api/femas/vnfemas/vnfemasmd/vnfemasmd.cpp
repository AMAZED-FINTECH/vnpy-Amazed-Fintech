// vnfemasmd.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "vnfemasmd.h"


///-------------------------------------------------------------------------------------
///C++�Ļص����������ݱ��浽������
///-------------------------------------------------------------------------------------

void MdApi::OnFrontConnected()
{
	Task task = Task();
	task.task_name = ONFRONTCONNECTED;
	this->task_queue.push(task);
};

void MdApi::OnFrontDisconnected(int nReason)
{
	Task task = Task();
	task.task_name = ONFRONTDISCONNECTED;
	task.task_id = nReason;
	this->task_queue.push(task);
};

void MdApi::OnHeartBeatWarning(int nTimeLapse)
{
	Task task = Task();
	task.task_name = ONHEARTBEATWARNING;
	task.task_id = nTimeLapse;
	this->task_queue.push(task);
};


void MdApi::OnPackageStart(int nTopicID, int nSequenceNo)
{
	//�ú�������
};

void MdApi::OnPackageEnd(int nTopicID, int nSequenceNo)
{
	//�ú�������
};
void MdApi::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	Task task = Task();
	task.task_name = ONRSPUSERLOGIN;
	if (pRspUserLogin)
	{
		CUstpFtdcRspUserLoginField *task_data = new CUstpFtdcRspUserLoginField();
		*task_data = *pRspUserLogin;
		task.task_data = task_data;
	}
	if (pRspInfo)
	{
		CUstpFtdcRspInfoField *task_error = new CUstpFtdcRspInfoField();
		*task_error = *pRspInfo;
		task.task_error = task_error;
	}

	task.task_id = nRequestID;
	task.task_last = bIsLast;
	this->task_queue.push(task);
};

void MdApi::OnRspUserLogout(CUstpFtdcRspUserLogoutField *pUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	Task task = Task();
	task.task_name = ONRSPUSERLOGOUT;
	if (pUserLogout)
	{
		CUstpFtdcRspUserLogoutField *task_data = new CUstpFtdcRspUserLogoutField();
		*task_data = *pUserLogout;
		task.task_data = task_data;
	}
	if (pRspInfo)
	{
		CUstpFtdcRspInfoField *task_error = new CUstpFtdcRspInfoField();
		*task_error = *pRspInfo;
		task.task_error = task_error;
	}
	task.task_id = nRequestID;
	task.task_last = bIsLast;
	this->task_queue.push(task);
};

void MdApi::OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	Task task = Task();
	task.task_name = ONRSPERROR;
	if (pRspInfo)
	{
		CUstpFtdcRspInfoField *task_error = new CUstpFtdcRspInfoField();
		*task_error = *pRspInfo;
		task.task_error = task_error;
	}
	task.task_id = nRequestID;
	task.task_last = bIsLast;
	this->task_queue.push(task);
};

void MdApi::OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	Task task = Task();
	task.task_name = ONRSPSUBMARKETDATA;
	if (pSpecificInstrument)
	{
		CUstpFtdcSpecificInstrumentField *task_data = new CUstpFtdcSpecificInstrumentField();
		*task_data = *pSpecificInstrument;
		task.task_data = task_data;
	}
	if (pRspInfo)
	{
		CUstpFtdcRspInfoField *task_error = new CUstpFtdcRspInfoField();
		*task_error = *pRspInfo;
		task.task_error = task_error;
	}
	task.task_id = nRequestID;
	task.task_last = bIsLast;
	this->task_queue.push(task);
};

void MdApi::OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	Task task = Task();
	task.task_name = ONRSPUNSUBMARKETDATA;
	if (pSpecificInstrument)
	{
		CUstpFtdcSpecificInstrumentField *task_data = new CUstpFtdcSpecificInstrumentField();
		*task_data = *pSpecificInstrument;
		task.task_data = task_data;
	}
	if (pRspInfo)
	{
		CUstpFtdcRspInfoField *task_error = new CUstpFtdcRspInfoField();
		*task_error = *pRspInfo;
		task.task_error = task_error;
	}
	task.task_id = nRequestID;
	task.task_last = bIsLast;
	this->task_queue.push(task);
};

void MdApi::OnRspSubscribeTopic(CUstpFtdcDisseminationField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	Task task = Task();
	task.task_name = ONRSPSUBSCRIBETOPIC;
	if (pSpecificInstrument)
	{
		CUstpFtdcDisseminationField *task_data = new CUstpFtdcDisseminationField();
		*task_data = *pSpecificInstrument;
		task.task_data = task_data;
	}
	if (pRspInfo)
	{
		CUstpFtdcRspInfoField *task_error = new CUstpFtdcRspInfoField();
		*task_error = *pRspInfo;
		task.task_error = task_error;
	}
	task.task_id = nRequestID;
	task.task_last = bIsLast;
	this->task_queue.push(task);
};

void MdApi::OnRspQryTopic(CUstpFtdcDisseminationField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	Task task = Task();
	task.task_name = ONRSPQRYTOPIC;
	if (pSpecificInstrument)
	{
		CUstpFtdcDisseminationField *task_data = new CUstpFtdcDisseminationField();
		*task_data = *pSpecificInstrument;
		task.task_data = task_data;
	}
	if (pRspInfo)
	{
		CUstpFtdcRspInfoField *task_error = new CUstpFtdcRspInfoField();
		*task_error = *pRspInfo;
		task.task_error = task_error;
	}
	task.task_id = nRequestID;
	task.task_last = bIsLast;
	this->task_queue.push(task);
};

void MdApi::OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pDepthMarketData)
{
	Task task = Task();
	task.task_name = ONRTNDEPTHMARKETDATA;
	if (pDepthMarketData)
	{
		CUstpFtdcDepthMarketDataField *task_data = new CUstpFtdcDepthMarketDataField();
		*task_data = *pDepthMarketData;
		task.task_data = task_data;
	}
	this->task_queue.push(task);
};



///-------------------------------------------------------------------------------------
///�����̴߳Ӷ�����ȡ�����ݣ�ת��Ϊpython����󣬽�������
///-------------------------------------------------------------------------------------

void MdApi::processTask()
{
    try
    {
        while (this->active)
        {
            Task task = this->task_queue.pop();
            
            switch (task.task_name)
            {
            case ONFRONTCONNECTED:
            {
                this->processFrontConnected(&task);
                break;
            }

            case ONFRONTDISCONNECTED:
            {
                this->processFrontDisconnected(&task);
                break;
            }

            case ONHEARTBEATWARNING:
            {
                this->processHeartBeatWarning(&task);
                break;
            }

            case ONRSPUSERLOGIN:
            {
                this->processRspUserLogin(&task);
                break;
            }

            case ONRSPUSERLOGOUT:
            {
                this->processRspUserLogout(&task);
                break;
            }

            case ONRSPERROR:
            {
                this->processRspError(&task);
                break;
            }

            case ONRSPSUBMARKETDATA:
            {
                this->processRspSubMarketData(&task);
                break;
            }

            case ONRSPUNSUBMARKETDATA:
            {
                this->processRspUnSubMarketData(&task);
                break;
            }

            case ONRTNDEPTHMARKETDATA:
            {
                this->processRtnDepthMarketData(&task);
                break;
            }

            };
        }
    }
    catch (const TerminatedError&)
    {
    }
};

void MdApi::processFrontConnected(Task *task)
{
	gil_scoped_acquire acquire;
	this->onFrontConnected();
};

void MdApi::processFrontDisconnected(Task *task)
{
	gil_scoped_acquire acquire;
	this->onFrontDisconnected(task->task_id);
};

void MdApi::processHeartBeatWarning(Task *task)
{
	gil_scoped_acquire acquire;
	this->onHeartBeatWarning(task->task_id);
};

void MdApi::processRspUserLogin(Task *task)
{
	gil_scoped_acquire acquire;
	dict data;
	if (task->task_data)
	{
		CUstpFtdcRspUserLoginField *task_data = (CUstpFtdcRspUserLoginField*)task->task_data;
		data["TradingDay"] = toUtf(task_data->TradingDay);
		data["LoginTime"] = toUtf(task_data->LoginTime);
		data["BrokerID"] = toUtf(task_data->BrokerID);
		data["UserID"] = toUtf(task_data->UserID);
		data["TradingSystemName"] = toUtf(task_data->TradingSystemName);
		data["MaxOrderLocalID"] = task_data->MaxOrderLocalID;
		data["PrivateFlowSize"] = task_data->PrivateFlowSize;
		data["DataCenterID"] = task_data->DataCenterID;
		data["UserFlowSize"] = task_data->UserFlowSize;
		delete task->task_data;
	}
	dict error;
	if (task->task_error)
	{
		CUstpFtdcRspInfoField *task_error = (CUstpFtdcRspInfoField*)task->task_error;
		error["ErrorID"] = task_error->ErrorID;
		error["ErrorMsg"] = toUtf(task_error->ErrorMsg);
		delete task->task_error;
	}
	this->onRspUserLogin(data, error, task->task_id, task->task_last);
};

void MdApi::processRspUserLogout(Task *task)
{
	gil_scoped_acquire acquire;
	dict data;
	if (task->task_data)
	{
		CUstpFtdcRspUserLogoutField *task_data = (CUstpFtdcRspUserLogoutField*)task->task_data;
		data["BrokerID"] = toUtf(task_data->BrokerID);
		data["UserID"] = toUtf(task_data->UserID);
		delete task->task_data;
	}
	dict error;
	if (task->task_error)
	{
		CUstpFtdcRspInfoField *task_error = (CUstpFtdcRspInfoField*)task->task_error;
		error["ErrorID"] = task_error->ErrorID;
		error["ErrorMsg"] = toUtf(task_error->ErrorMsg);
		delete task->task_error;
	}
	this->onRspUserLogout(data, error, task->task_id, task->task_last);
};

void MdApi::processRspError(Task *task)
{
	gil_scoped_acquire acquire;
	dict error;
	if (task->task_error)
	{
		CUstpFtdcRspInfoField *task_error = (CUstpFtdcRspInfoField*)task->task_error;
		error["ErrorID"] = task_error->ErrorID;
		error["ErrorMsg"] = toUtf(task_error->ErrorMsg);
		delete task->task_error;
	}
	this->onRspError(error, task->task_id, task->task_last);
};

void MdApi::processRspSubMarketData(Task *task)
{
	gil_scoped_acquire acquire;
	dict data;
	if (task->task_data)
	{
		CUstpFtdcSpecificInstrumentField *task_data = (CUstpFtdcSpecificInstrumentField*)task->task_data;
		data["InstrumentID"] = toUtf(task_data->InstrumentID);
		delete task->task_data;
	}
	dict error;
	if (task->task_error)
	{
		CUstpFtdcRspInfoField *task_error = (CUstpFtdcRspInfoField*)task->task_error;
		error["ErrorID"] = task_error->ErrorID;
		error["ErrorMsg"] = toUtf(task_error->ErrorMsg);
		delete task->task_error;
	}
	error["ErrorID"] = 0;
	this->onRspSubMarketData(data, error, task->task_id, task->task_last);
};

void MdApi::processRspUnSubMarketData(Task *task)
{
	gil_scoped_acquire acquire;
	dict data;
	if (task->task_data)
	{
		CUstpFtdcSpecificInstrumentField *task_data = (CUstpFtdcSpecificInstrumentField*)task->task_data;
		data["InstrumentID"] = toUtf(task_data->InstrumentID);
		delete task->task_data;
	}
	dict error;
	if (task->task_error)
	{
		CUstpFtdcRspInfoField *task_error = (CUstpFtdcRspInfoField*)task->task_error;
		error["ErrorID"] = task_error->ErrorID;
		error["ErrorMsg"] = toUtf(task_error->ErrorMsg);
		delete task->task_error;
	}
	this->onRspUnSubMarketData(data, error, task->task_id, task->task_last);
};

void MdApi::processRtnDepthMarketData(Task *task)
{
	gil_scoped_acquire acquire;
	dict data;
	if (task->task_data)
	{
		CUstpFtdcDepthMarketDataField *task_data = (CUstpFtdcDepthMarketDataField*)task->task_data;
		data["TradingDay"] = toUtf(task_data->TradingDay);
		data["ActionDay"] = toUtf(task_data->ActionDay);
		data["InstrumentID"] = toUtf(task_data->InstrumentID);
		data["LastPrice"] = task_data->LastPrice;
		data["PreSettlementPrice"] = task_data->PreSettlementPrice;
		data["PreClosePrice"] = task_data->PreClosePrice;
		data["PreOpenInterest"] = task_data->PreOpenInterest;
		data["OpenPrice"] = task_data->OpenPrice;
		data["HighestPrice"] = task_data->HighestPrice;
		data["LowestPrice"] = task_data->LowestPrice;
		data["Volume"] = task_data->Volume;
		data["Turnover"] = task_data->Turnover;
		data["OpenInterest"] = task_data->OpenInterest;
		data["ClosePrice"] = task_data->ClosePrice;
		data["SettlementPrice"] = task_data->SettlementPrice;
		data["UpperLimitPrice"] = task_data->UpperLimitPrice;
		data["LowerLimitPrice"] = task_data->LowerLimitPrice;
		data["PreDelta"] = task_data->PreDelta;
		data["CurrDelta"] = task_data->CurrDelta;
		data["UpdateTime"] = toUtf(task_data->UpdateTime);
		data["UpdateMillisec"] = task_data->UpdateMillisec;
		data["BidPrice1"] = task_data->BidPrice1;
		data["BidVolume1"] = task_data->BidVolume1;
		data["AskPrice1"] = task_data->AskPrice1;
		data["AskVolume1"] = task_data->AskVolume1;
		data["BidPrice2"] = task_data->BidPrice2;
		data["BidVolume2"] = task_data->BidVolume2;
		data["AskPrice2"] = task_data->AskPrice2;
		data["AskVolume2"] = task_data->AskVolume2;
		data["BidPrice3"] = task_data->BidPrice3;
		data["BidVolume3"] = task_data->BidVolume3;
		data["AskPrice3"] = task_data->AskPrice3;
		data["AskVolume3"] = task_data->AskVolume3;
		data["BidPrice4"] = task_data->BidPrice4;
		data["BidVolume4"] = task_data->BidVolume4;
		data["AskPrice4"] = task_data->AskPrice4;
		data["AskVolume4"] = task_data->AskVolume4;
		data["BidPrice5"] = task_data->BidPrice5;
		data["BidVolume5"] = task_data->BidVolume5;
		data["AskPrice5"] = task_data->AskPrice5;
		data["AskVolume5"] = task_data->AskVolume5;
		delete task->task_data;
	}
	this->onRtnDepthMarketData(data);
};


///-------------------------------------------------------------------------------------
///��������
///-------------------------------------------------------------------------------------

void MdApi::createFtdcMdApi(string pszFlowPath)
{
	this->api = CUstpFtdcMduserApi::CreateFtdcMduserApi(pszFlowPath.c_str());
	this->api->RegisterSpi(this);
};

void MdApi::release()
{
	this->api->Release();
};

void MdApi::init()
{
	this->active = true;
	this->task_thread = thread(&MdApi::processTask, this);

	this->api->Init();
};

int MdApi::join()
{
	int i = this->api->Join();
	return i;
};

int MdApi::exit()
{
	this->active = false;
    this->task_queue.terminate();
    this->task_thread.join();

	this->api->RegisterSpi(NULL);
	this->api->Release();
	this->api = NULL;
	return 1;
};

string MdApi::getTradingDay()
{
	string day = this->api->GetTradingDay();
	return day;
};

void MdApi::registerFront(string pszFrontAddress)
{
	this->api->RegisterFront((char*)pszFrontAddress.c_str());
};

int MdApi::reqUserLogin(const dict &req, int reqid)
{
	CUstpFtdcReqUserLoginField myreq = CUstpFtdcReqUserLoginField();
	memset(&myreq, 0, sizeof(myreq));
	getString(req, "TradingDay", myreq.TradingDay);
	getString(req, "BrokerID", myreq.BrokerID);
	getString(req, "UserID", myreq.UserID);
	getString(req, "Password", myreq.Password);
	getString(req, "UserProductInfo", myreq.UserProductInfo);
	getString(req, "InterfaceProductInfo", myreq.InterfaceProductInfo);
	getString(req, "ProtocolInfo", myreq.ProtocolInfo);
	getString(req, "MacAddress", myreq.MacAddress);
	int i = this->api->ReqUserLogin(&myreq, reqid);
	return i;
};

int MdApi::reqUserLogout(const dict &req, int reqid)
{
	CUstpFtdcReqUserLogoutField myreq = CUstpFtdcReqUserLogoutField();
	memset(&myreq, 0, sizeof(myreq));
	getString(req, "BrokerID", myreq.BrokerID);
	getString(req, "UserID", myreq.UserID);
	int i = this->api->ReqUserLogout(&myreq, reqid);
	return i;
}


void MdApi::setHeartbeatTimeout(int timeout)
{
	this->api->SetHeartbeatTimeout(timeout);
}



void MdApi::subscribeMarketDataTopic(int topicID, int resumeType)
{
	switch (resumeType)
	{
	case 0:
	{
		this->api->SubscribeMarketDataTopic(topicID, USTP_TERT_RESTART);
		break;
	}

	case 1:
	{
		this->api->SubscribeMarketDataTopic(topicID, USTP_TERT_RESUME);
		break;
	}

	case 2:
	{
		this->api->SubscribeMarketDataTopic(topicID, USTP_TERT_QUICK);
		break;
	}
	}
}

int MdApi::registerCertificateFile(string pszCertFileName, string pszKeyFileName, string pszCaFileName, string pszKeyFilePassword)
{
	int i = this->api->RegisterCertificateFile(pszCertFileName.c_str(), pszKeyFileName.c_str(), pszCaFileName.c_str(), pszKeyFilePassword.c_str());
	return i;
}

int MdApi::subMarketData(string instrumentID)
{
	char* buffer = (char*)instrumentID.c_str();
	char* myreq[1] = { buffer };
	int i = this->api->SubMarketData(myreq, 1);
	return i;
}

int MdApi::unSubMarketData(string instrumentID)
{
	char* buffer = (char*)instrumentID.c_str();
	char* myreq[1] = { buffer };
	int i = this->api->UnSubMarketData(myreq, 1);
	return i;
}

void MdApi::registerNameServer(string pszNsAddress)
{
	this->api->RegisterNameServer((char*)pszNsAddress.c_str());
};



///-------------------------------------------------------------------------------------
///Boost.Python��װ
///-------------------------------------------------------------------------------------

class PyMdApi: public MdApi
{
public:
	using MdApi::MdApi;

	void onFrontConnected() override
	{
		try
		{
			PYBIND11_OVERLOAD(void, MdApi, onFrontConnected);
		}
		catch (const error_already_set &e)
		{
			cout << e.what() << endl;
		}
	};

	void onFrontDisconnected(int reqid) override
	{
		try
		{
			PYBIND11_OVERLOAD(void, MdApi, onFrontDisconnected, reqid);
		}
		catch (const error_already_set &e)
		{
			cout << e.what() << endl;
		}
	};

	void onHeartBeatWarning(int reqid) override
	{
		try
		{
			PYBIND11_OVERLOAD(void, MdApi, onHeartBeatWarning, reqid);
		}
		catch (const error_already_set &e)
		{
			cout << e.what() << endl;
		}
	};

	void onRspUserLogin(const dict &data, const dict &error, int reqid, bool last) override
	{
		try
		{
			PYBIND11_OVERLOAD(void, MdApi, onRspUserLogin, data, error, reqid, last);
		}
		catch (const error_already_set &e)
		{
			cout << e.what() << endl;
		}
	};

	void onRspUserLogout(const dict &data, const dict &error, int reqid, bool last) override
	{
		try
		{
			PYBIND11_OVERLOAD(void, MdApi, onRspUserLogout, data, error, reqid, last);
		}
		catch (const error_already_set &e)
		{
			cout << e.what() << endl;
		}
	};

	void onRspError(const dict &error, int reqid, bool last) override
	{
		try
		{
			PYBIND11_OVERLOAD(void, MdApi, onRspError, error, reqid, last);
		}
		catch (const error_already_set &e)
		{
			cout << e.what() << endl;
		}
	};

	void onRspSubMarketData(const dict &data, const dict &error, int reqid, bool last) override
	{
		try
		{
			PYBIND11_OVERLOAD(void, MdApi, onRspSubMarketData, data, error, reqid, last);
		}
		catch (const error_already_set &e)
		{
			cout << e.what() << endl;
		}
	};

	void onRspUnSubMarketData(const dict &data, const dict &error, int reqid, bool last) override
	{
		try
		{
			PYBIND11_OVERLOAD(void, MdApi, onRspUnSubMarketData, data, error, reqid, last);
		}
		catch (const error_already_set &e)
		{
			cout << e.what() << endl;
		}
	};

	
	void onRtnDepthMarketData(const dict &data) override
	{
		try
		{
			PYBIND11_OVERLOAD(void, MdApi, onRtnDepthMarketData, data);
		}
		catch (const error_already_set &e)
		{
			cout << e.what() << endl;
		}
	};

	
};


PYBIND11_MODULE(vnfemasmd, m)
{
	class_<MdApi, PyMdApi> mdapi(m, "MdApi", module_local());
	mdapi
		.def(init<>())
		.def("createFtdcMdApi", &MdApi::createFtdcMdApi)
		.def("release", &MdApi::release)
		.def("init", &MdApi::init)
		.def("join", &MdApi::join)
		.def("exit", &MdApi::exit)
		.def("getTradingDay", &MdApi::getTradingDay)
		.def("registerFront", &MdApi::registerFront)
		.def("registerNameServer", &MdApi::registerNameServer)
		.def("registerCertificateFile", &MdApi::registerCertificateFile)
		.def("subscribeMarketDataTopic", &MdApi::subscribeMarketDataTopic)
		.def("subMarketData", &MdApi::subMarketData)
		.def("unSubMarketData", &MdApi::unSubMarketData)
		.def("setHeartbeatTimeout", &MdApi::setHeartbeatTimeout)
		.def("reqUserLogin", &MdApi::reqUserLogin)
		.def("reqUserLogout", &MdApi::reqUserLogout)
		.def("onFrontConnected", &MdApi::onFrontConnected)
		.def("onFrontDisconnected", &MdApi::onFrontDisconnected)
		.def("onHeartBeatWarning", &MdApi::onHeartBeatWarning)
		.def("onRspUserLogin", &MdApi::onRspUserLogin)
		.def("onRspUserLogout", &MdApi::onRspUserLogout)
		.def("onRspError", &MdApi::onRspError)

		.def("onRspSubMarketData", &MdApi::onRspSubMarketData)
		.def("onRspUnSubMarketData", &MdApi::onRspUnSubMarketData)
		.def("onRtnDepthMarketData", &MdApi::onRtnDepthMarketData)

		.def("onRspSubscribeTopic", &MdApi::onRspSubscribeTopic)
		.def("onRspQryTopic", &MdApi::onRspQryTopic)

		;
}