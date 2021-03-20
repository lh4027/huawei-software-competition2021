//#include <iostream>
//#include <string>
//#include <unordered_map>
//#include <map>
//#include <vector>
//#include <ctime>
//#include <algorithm>
//#include <cassert>
//#include<fstream>
//#include<set>
//using namespace std;
//
//#define TEST
//
////#define UPLOAD
//
//// �ṹ��洢������Ϣ
//
//// ������������Ϣ�ṹ��
//struct ServerInfo {
//
//	int CpuCores_A;    // A�ڵ�cpu��Դ��
//	int CpuCores_B;    // B�ڵ�cpu��Դ��
//
//	int MemerySize_A;  // A�ڵ��ڴ���Դ��
//	int MemerySize_B;  // B�ڵ��ڴ���Դ��
//
//	int ServerCost;    // ����������
//	int PowerCost;     // ���
//
//	int ServerId;      // ������Id
//};
//
//// �����������Ϣ�ṹ��
//struct VmInfo {
//
//	int CpuCores;    // ���ĵ�cpu��Դ��
//
//	int MemerySize;  // ���ĵ��ڴ���Դ��
//
//	int VmTwoNodes;  // ��/˫�ڵ㲿��
//
//	string VmId;     // �����Id
//};
//
//
//// ���з�������Ϣ
//unordered_map<string, ServerInfo> ServerInfos;
//
//// �����������Ϣ
//unordered_map<string, VmInfo> VmInfos;
//
//// һ������������Ϣ
//vector<vector<string>> RequestInfos;
//
//// ����ķ���������
//int serverNumber = 0;
//
//// ����ķ�������Ϣ��ϵͳ�ִ���Դ��
//unordered_map<int, ServerInfo> sysServerResource;
//
//// ��ǰ�����ķ����������е����������
//map<int, int> serverRunVms;
//
//// ��¼������������Ǹ���������
//unordered_map<string, vector<int>> VmOnServer;   //[in] �����Id  [in] ��������ڵķ�����id,�����cpu,������ڴ棬��/˫�ڵ㲿��
//
//// ��¼����������������Щ�����
//unordered_map<int, unordered_map<string, vector<int>>> ServerOnVm;   //[in] ������Id  [in] �������id, �����cpu,�ڴ棬��/˫�ڵ㲿��
//
//multimap<string, unordered_map<string, vector<int>>> DayServerOnVm; // ��¼һ��ı��������ID�ķ������ճ����ϵ��������Ϣ����˳��Ϊ������ID˳��
//
//vector<string> res;
//
//#ifdef TEST
//const string filePath = "training-1.txt";
//#endif
//
//// �ɱ�
//long long SERVERCOST = 0, POWERCOST = 0, TOTALCOST = 0;
//
//
//// ������������Ϣ
//void GetServerInfos(string& serverType, string& cpuCores, string& memorySize, string& serverCost, string& powerCost) {
//	string _serverType = "";
//
//	for (int i = 1; i < serverType.size() - 1; i++) {
//		_serverType += serverType[i];
//	}
//	int _cpuCores = 0, _memorySize = 0, _serverCost = 0, _powerCost = 0;
//
//	for (int i = 0; i < cpuCores.size() - 1; i++) {
//		_cpuCores = 10 * _cpuCores + cpuCores[i] - '0';
//	}
//	for (int i = 0; i < memorySize.size() - 1; i++) {
//		_memorySize = 10 * _memorySize + memorySize[i] - '0';
//	}
//	for (int i = 0; i < serverCost.size() - 1; i++) {
//		_serverCost = 10 * _serverCost + serverCost[i] - '0';
//	}
//	for (int i = 0; i < powerCost.size() - 1; i++) {
//		_powerCost = 10 * _powerCost + powerCost[i] - '0';
//	}
//
//	ServerInfos[_serverType].CpuCores_A = _cpuCores / 2;   // A�ڵ�cpu
//	ServerInfos[_serverType].CpuCores_B = _cpuCores / 2;   // B�ڵ�cpu
//
//	ServerInfos[_serverType].MemerySize_A = _memorySize / 2;  //A�ڵ��ڴ�
//	ServerInfos[_serverType].MemerySize_B = _memorySize / 2;  //B�ڵ��ڴ�
//
//	ServerInfos[_serverType].PowerCost = _powerCost;   //���
//	ServerInfos[_serverType].ServerCost = _serverCost; //�������ɱ�
//}
//
//
//// �����������Ϣ
//void GetVmInfos(string& vmType, string& vmCpuCores, string& vmMemory, string& vmTwoNodes) {
//	string _vmType;
//
//	for (int i = 1; i < vmType.size() - 1; i++) {
//		_vmType += vmType[i];
//	}
//
//	int _vmCpuCores = 0, _vmMemory = 0, _vmTwoNodes = 0;
//	for (int i = 0; i < vmCpuCores.size() - 1; i++) {
//		_vmCpuCores = _vmCpuCores * 10 + vmCpuCores[i] - '0';
//	}
//	for (int i = 0; i < vmMemory.size() - 1; i++) {
//		_vmMemory = _vmMemory * 10 + vmMemory[i] - '0';
//	}
//	if (vmTwoNodes[0] == '1') {
//		_vmTwoNodes = 1;
//	}
//	else {
//		_vmTwoNodes = 0;
//	}
//	VmInfos[_vmType].CpuCores = _vmCpuCores;
//	VmInfos[_vmType].MemerySize = _vmMemory;
//	VmInfos[_vmType].VmTwoNodes = _vmTwoNodes;
//}
//
//// �����û��������
//void GetRequest(string& op, string& reqVmType, string& reqId) {
//
//	string _op, _reqVmType, _reqId;
//	_op = op.substr(1, op.size() - 1);                                 // add
//	_reqVmType = reqVmType.substr(0, reqVmType.size() - 1);            // ������ӵ����������
//	_reqId = reqId.substr(0, reqId.size() - 1);                        // ������ӵ������id
//	RequestInfos.push_back(vector<string>{_op, _reqVmType, _reqId});   // �û������������û�������Ϣ��
//}
//
//// �����û�ɾ������
//void GetRequest(string& op, string& reqId) {
//	string _op, _reqId;
//	_reqId = reqId.substr(0, reqId.size() - 1);                        // ����ɾ���������id
//	_op = op.substr(1, op.size() - 1);                                 // delete
//	RequestInfos.push_back(vector<string>{_op, _reqId});               // �û�ɾ���������������û�������Ϣ��
//}
//
//
//// �ڷ������Ϸ����������Դ
//bool AllocServerResources(ServerInfo& server, VmInfo& vm, int serverId, string vmId, vector<string> &res_tmp) {
//
//	int vmCores = vm.CpuCores,
//		vmMemory = vm.MemerySize,
//		vmTwoNodes = vm.VmTwoNodes;
//
//	int& serverCoreA = server.CpuCores_A,
//		& serverCoreB = server.CpuCores_B,
//		& serverMemoryA = server.MemerySize_A,
//		& serverMemoryB = server.MemerySize_B;
//
//	if (vmTwoNodes) {   //˫�ڵ㲿��ʱ
//		int needCores = vmCores / 2, needMemory = vmMemory / 2;  //�������Դ���԰����
//		if (serverCoreA >= needCores && serverCoreB >= needCores && serverMemoryA >= needMemory && serverMemoryB >= needMemory) {   //������AB�ڵ���Դ������
//			// ��Դ�ֳ�
//			serverCoreA -= needCores;
//			serverCoreB -= needCores;
//			serverMemoryA -= needMemory;
//			serverMemoryB -= needMemory;
//
//			// ��¼������������ĸ��������ϣ�˫�ڵ㲿��
//			VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1,2 };
//
//			res_tmp.push_back("(" + to_string(serverId) + ")\n");
//			return true;
//		}
//		else {
//			return false;                                               //��Դ���㣬�ڴ˷������Ϸ���������ʧ��
//		}
//	}
//	else if (serverCoreA >= vmCores && serverMemoryA >= vmMemory) {     //���ڵ㲿����A�ڵ��Ƿ������������
//
//		serverCoreA -= vmCores;
//		serverMemoryA -= vmMemory;
//
//		// ��¼������������ĸ��������ϣ�A�ڵ㲿��
//		VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1 };
//
//		res_tmp.push_back("(" + to_string(serverId) + ", A)\n");
//		return true;
//	}
//	else if (serverCoreB >= vmCores && serverMemoryB >= vmMemory) {    //���ڵ㲿����B�ڵ��Ƿ������������
//
//		serverCoreB -= vmCores;
//		serverMemoryB -= vmMemory;
//
//		// ��¼������������ĸ��������ϣ�B�ڵ㲿��
//		VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,2 };
//
//		res_tmp.push_back("(" + to_string(serverId) + ", B)\n");
//		return true;
//	}
//	return false;                                                      //��˫�ڵ���Դ�����㣬�ڴ˷������Ϸ���������ʧ��                                             
//}
//
//// �ڷ��������ͷ��������Դ
//void ReleaseServerResources(ServerInfo& server, vector<int>& vmInfo, int serverId, string vmId) {
//	// [in]:���з�������Դ 
//	// [in]:���е��������Ϣ[��������ڵķ�����id,�����cpu,������ڴ棬��/˫�ڵ㲿��]  
//	// [in]:������Id 
//	// [in]:�����Id	
//	if (vmInfo.size() == 5) {  // ˫�ڵ㲿��������
//		int cores = vmInfo[1] / 2, memory = vmInfo[2] / 2;
//
//		server.CpuCores_A += cores;
//		server.CpuCores_B += cores;
//		server.MemerySize_A += memory;
//		server.MemerySize_B += memory;
//	}
//	else { // ���ڵ㲿��
//		int cores = vmInfo[1], memory = vmInfo[2];
//		if (vmInfo[3] == 1) {  // A�ڵ㲿��
//			server.CpuCores_A += cores;
//			server.MemerySize_A += memory;
//		}
//		else {                 // B�ڵ㲿��
//			server.CpuCores_B += cores;
//			server.MemerySize_B += memory;
//		}
//	}
//}
//
//
//// ����ɾ�����������
//void DealDeleteVM(vector<string>& delVmInfo) {
//
//	string _vmId = delVmInfo[1];
//
//	vector<int> _vmInfo = VmOnServer[_vmId];
//
//	int _serverId = _vmInfo[0];  //����������ķ�����id
//
//	auto& server = sysServerResource[_serverId];
//
//	ReleaseServerResources(server, _vmInfo, _serverId, _vmId);   //�ͷ������
//
//	serverRunVms[_serverId]--;  //�������ϵ����������-1
//
//	assert(serverRunVms[_serverId] >= 0);
//
//}
//
//
//multimap<float, string> PerformanceAndCostRatio;  //��������
//
//void GetBestServer(int day, unordered_map<string, ServerInfo> serverinfos) {
//
//	float Cpu;
//	float Memory;
//
//	float ServerCost;
//	float PowerCost;
//
//	string serverType;
//
//	for (auto s : serverinfos) {
//
//		serverType = s.first;
//		Cpu = s.second.CpuCores_A + s.second.CpuCores_B;
//		Memory = s.second.MemerySize_A + s.second.MemerySize_B;
//
//		ServerCost = s.second.ServerCost;
//		PowerCost = s.second.PowerCost;
//
//		PerformanceAndCostRatio.insert(make_pair((ServerCost + day * PowerCost) / (Cpu + Memory), serverType));  //����keyֵ�����Զ�����
//	}
//}
//
//
//int serverId = 0;  // ������ID(ȫ��)
//multimap<string, unordered_map<string, vector<int>>> ServerOnVm_type;  // ͨ����������������¼�ڸ����ͷ������ϵ�vm��
//vector<string>  ServerType;  // ��˳����ÿ��ѡ�������ŷ������������ƣ��������ʱ���մ�˳���ҵ���Ӧ������ID
//
//// ���ݷ����� + ����
//void Expansion(int day, unordered_map<string, ServerInfo> serverinfos, vector<vector<string>> requestinfos) {
//
//
//	//GetBestServer(day, serverinfos);  //�����Լ۱���ߵķ�����
//
//	unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverBestmatch; // ��¼����ķ������ʹճ��������ƥ����Ϣ <�������ͺţ�<�����Id��vm�ڴ棬vmCpu,vmTwoNodes>>
//
//	unordered_map<string, int> ServerBuyNum;
//
//	set<string> ServerTypeNum;
//
//	multimap<string, ServerInfo> sysServerResourceTemp;
//	vector<vector<string>> requestinfosAdd;
//
//	// ������add�����ó���
//	for (auto re : requestinfos) {
//		if (re.size() > 2) {
//			requestinfosAdd.push_back(re);
//		}
//	}
//
//	while (requestinfosAdd.size() > 0) {    // ���������Ϣ����һֱѭ��
//
//		unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverTemp; // ��¼�������ʹճ��������ƥ����Ϣ <�������ͺţ�<�����Id��vm�ڴ棬vmCpu,vmTwoNodes>>
//		multimap<int, string> Matching_degree;  // ��¼�ճ��ķ�������ƥ��̶Ȳ�ֵ��ȡ��С
//		unordered_map<string, ServerInfo> ServerResourceTemp;
//
//		for (auto s : serverinfos) {    // �����з���������ƥ��
//
//			int taget_tatal = s.second.CpuCores_A + s.second.CpuCores_B + s.second.MemerySize_A + s.second.MemerySize_B;  //��ʼ���������ʹյ����������Դ��ֵ
//			int taget_CpuCores_A = s.second.CpuCores_A, taget_CpuCores_B = s.second.CpuCores_B, taget_MemerySize_A = s.second.MemerySize_A, taget_MemerySize_B = s.second.MemerySize_B;
//			bool flag = false;
//
//
//			for (int i = 0; i < requestinfosAdd.size(); i++) {
//				if (requestinfosAdd[i].size() > 2) {   //������add�����д���
//
//					int vmCores = VmInfos[requestinfosAdd[i][1]].CpuCores;
//					int vmMemory = VmInfos[requestinfosAdd[i][1]].MemerySize;
//					int vmTwoNodes = VmInfos[requestinfosAdd[i][1]].VmTwoNodes;
//					vector<int> vminfoAdd;
//
//					if (vmTwoNodes) {    // ˫�ڵ�vm
//						int needCores = vmCores / 2;
//						int needMemory = vmMemory / 2;
//						if (taget_CpuCores_A >= needCores && taget_CpuCores_B >= needCores && taget_MemerySize_A >= needMemory && taget_MemerySize_B >= needMemory) {
//							taget_CpuCores_A -= needCores;
//							taget_CpuCores_B -= needCores;
//							taget_MemerySize_A -= needMemory;
//							taget_MemerySize_B -= needMemory;
//
//							taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ
//
//							vminfoAdd.push_back(vmCores);
//							vminfoAdd.push_back(vmMemory);
//							vminfoAdd.push_back(1);
//							vminfoAdd.push_back(2);
//							vmsOnserverTemp[s.first].insert(make_pair(requestinfosAdd[i][2], vminfoAdd));
//							vminfoAdd.clear();
//							flag = true;
//
//							if (i == requestinfosAdd.size() - 1)
//								Matching_degree.insert(make_pair(taget_tatal, s.first));   // ��¼�������������ƥ��Ĳ�ֵ
//						}
//						else {
//							Matching_degree.insert(make_pair(taget_tatal, s.first));   // ��¼�������������ƥ��Ĳ�ֵ
//							break;
//						}
//					}
//					else {  // ���ڵ�
//						if (taget_CpuCores_A >= vmCores && taget_MemerySize_A >= vmMemory) {
//
//							flag = true;
//
//							taget_CpuCores_A -= vmCores;
//							taget_MemerySize_A -= vmMemory;
//
//							taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ
//
//							vminfoAdd.push_back(vmCores);
//							vminfoAdd.push_back(vmMemory);
//							vminfoAdd.push_back(1);
//
//							vmsOnserverTemp[s.first].insert(make_pair(requestinfosAdd[i][2], vminfoAdd));
//							vminfoAdd.clear();
//
//
//							if (i == requestinfosAdd.size() - 1)
//								Matching_degree.insert(make_pair(taget_tatal, s.first));   // ��¼�������������ƥ��Ĳ�ֵ
//						}
//						else if (taget_CpuCores_B >= vmCores && taget_MemerySize_B >= vmMemory) {
//							flag = true;
//							taget_CpuCores_B -= vmCores;
//							taget_MemerySize_B -= vmMemory;
//
//							taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ
//
//							vminfoAdd.push_back(vmCores);
//							vminfoAdd.push_back(vmMemory);
//							vminfoAdd.push_back(2);
//
//							vmsOnserverTemp[s.first].insert(make_pair(requestinfosAdd[i][2], vminfoAdd));
//							vminfoAdd.clear();
//
//
//							if (i == requestinfosAdd.size() - 1)
//								Matching_degree.insert(make_pair(taget_tatal, s.first));   // ��¼�������������ƥ��Ĳ�ֵ
//						}
//						else {
//							Matching_degree.insert(make_pair(taget_tatal, s.first));   // ��¼�������������ƥ��Ĳ�ֵ
//							break;
//						}
//					}
//
//				}
//
//			} // һ̨��������������
//
//			// ��¼��̨�������ϵ���Դʣ��������
//			ServerResourceTemp[s.first].CpuCores_A = taget_CpuCores_A;
//			ServerResourceTemp[s.first].CpuCores_B = taget_CpuCores_B;
//			ServerResourceTemp[s.first].MemerySize_A = taget_MemerySize_A;
//			ServerResourceTemp[s.first].MemerySize_B = taget_MemerySize_B;
//			ServerResourceTemp[s.first].PowerCost = ServerInfos[s.first].PowerCost;
//
//		}  // ���з�����һ��ѭ������
//		//ѡ����ƥ������з�����
//
//		// ���ڲ�ֵ��С�ķ���������ȴ�����������Ҫ����������ȡ��һ��
//		multimap<int, string>::iterator first = Matching_degree.begin();
//		for (multimap<int, string>::iterator it = Matching_degree.begin(); it != Matching_degree.end(); ) {
//			if (vmsOnserverTemp[(*it).second].size() == 0) {
//				it++;
//			}
//			else
//			{
//				first = it; break;
//			}
//		}
//
//		//ServerType.push_back((*first).second);
//
//		ServerTypeNum.insert((*first).second); // ��¼һ��������ַ��������������
//
//		ServerOnVm_type.insert(make_pair((*first).second, vmsOnserverTemp[(*first).second]));   //��ͬ�ķ��������ͻ�������һ����ϳɹ�����������˳����serverID
//
//		// ������ķ���������ʣ�����Դ������ϵͳ����ռ����Դ���(�˴���multimap���շ��������ƴ��룬��Ϊ����ServerOnVm_type�еĴ�ķ�����˳�򱣳�һ�£������ٽ���ת����id����)
//		sysServerResourceTemp.insert(make_pair((*first).second, ServerResourceTemp[(*first).second]));
//
//		SERVERCOST += ServerInfos[(*first).second].ServerCost;  // ��������������ۼӼ���
//
//		// ��requestinfosȥ������õ������		
//		requestinfosAdd.erase(requestinfosAdd.begin(), requestinfosAdd.begin() + vmsOnserverTemp[(*first).second].size());
//		Matching_degree.clear();
//	}  // һ����������������
//
//	// �����������������Ϣ
//	string s = "(purchase, ";
//
//	s += to_string(ServerTypeNum.size()) + ")\n";
//	res.push_back(s);
//
//#ifdef TEST
//	cout << s;
//#endif 
//
//	int ServertypeNum = 0;
//	if (ServerTypeNum.size() != 0) {   // ��������Ϊ0ʱ���
//		string serverType = (*ServerOnVm_type.begin()).first;
//		multimap<string, ServerInfo>::iterator sSr = sysServerResourceTemp.begin();
//		for (auto sov : ServerOnVm_type) {
//
//			// ��¼����������������Щ�����
//			ServerOnVm[serverId] = sov.second;
//			for (auto ss : sov.second) {
//				if (ss.second.size() > 3) {   // ˫�ڵ�
//					VmOnServer[ss.first].push_back(serverId);
//					VmOnServer[ss.first].push_back(ss.second[0]);
//					VmOnServer[ss.first].push_back(ss.second[1]);
//					VmOnServer[ss.first].push_back(ss.second[2]);
//					VmOnServer[ss.first].push_back(ss.second[3]);
//
//					serverRunVms[serverId]++;  // ��¼��ID�������������˶���̨vm
//				}
//				else {      // ���ڵ�
//					VmOnServer[ss.first].push_back(serverId);
//					VmOnServer[ss.first].push_back(ss.second[0]);
//					VmOnServer[ss.first].push_back(ss.second[1]);
//					VmOnServer[ss.first].push_back(ss.second[2]);
//
//					serverRunVms[serverId]++;  // ��¼��ID�������������˶���̨vm
//				}
//			}
//
//			// ��¼������������Դ��ȫ�֣�
//			sysServerResource[serverId] = (*sSr).second;
//			sSr++;
//
//			// ��������������Ϣ
//			if (sov.first != serverType) {
//
//				s = "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
//
//				res.push_back(s);
//
//#ifdef TEST
//				cout << "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
//#endif
//				serverType = sov.first;
//				ServertypeNum = 0;
//			}
//			ServertypeNum++;
//			serverId++;
//		}
//		// ��������������Ϣ
//		s = "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
//		res.push_back(s);
//
//#ifdef TEST
//		cout << "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
//#endif	
//
//		ServerBuyNum.clear();
//	}
//
//}
//
//
//// �˺�������
//int DealAddVm(vector<string> createVmInfo, vector<string>& res_tmp);
//int DealAddVm(vector<string> createVmInfo, vector<string>& res_tmp) {
//
//	string _reqVmType = createVmInfo[1], _reqId = createVmInfo[2];
//	auto  vm = VmInfos[_reqVmType];
//	
//	int success = -1;
//	for (int i = 0; i < serverId; i++) {           //��������Ż��� ���粻����˳�������Դ����ϵͳ������Դ������ٷ��䣿
//	    auto& server = sysServerResource[i];
//	    if (AllocServerResources(server, vm, i, _reqId, res_tmp)) {
//	        serverRunVms[i]++;
//	        success = 1;
//	        break;
//	    }
//	    assert(server.CpuCores_A >= 0 && server.CpuCores_B >= 0 && server.MemerySize_A >= 0 && server.MemerySize_B>= 0);
//	}
//
//	return success;
//
//}
//
//// ����ÿһ��add����
//int ServerIdAdd = 0; // ÿ�����ӵķ���������
//int ReDealAddVM(multimap<string, unordered_map<string, vector<int>>> serverOnVm, vector<string> request, vector<string>& res_tmp, int day);
//int ReDealAddVM(multimap<string, unordered_map<string, vector<int>>> serverOnVm, vector<string> request, vector<string>& res_tmp, int day) {
//	int OutserverId = 0;  // ������ID
//	string s = "(";
//	for (auto sov : serverOnVm) {
//		for (auto vm : sov.second) {
//			if (request[2] == vm.first) {   // �����Idƥ����
//				if (vm.second.size() > 3) {  // ˫�ڵ㲿��
//					s += to_string(OutserverId + ServerIdAdd) + ")\n";
//				}
//				else {
//					if (vm.second[2] == 1)
//						s += to_string(OutserverId + ServerIdAdd) + ", A)\n";		  // ���䵽A�ڵ�			
//					else
//						s += to_string(OutserverId + ServerIdAdd) + ", B)\n";          // ���䵽B�ڵ�	
//				}
//				if (day > 0) {
//					for (auto &tmp : res_tmp) {
//						if (tmp == "null") {
//							tmp = s; break;
//						}
//					}
//				}
//				else
//				{
//					res_tmp.push_back(s);
//				}
//				return 1;
//			}		
//		}
//		OutserverId++;
//	}	
//	return -1;  
//}
//
//
//// Ǩ�������
//void Migrate() {
//	string s = "(migration, 0)\n";
//#ifdef TEST
//	cout << s;
//#endif
//	res.push_back(s);
//}
//
//
//
//// ����
//void Match(int day, unordered_map<string, ServerInfo> serverinfos, vector<vector<string>> requestinfos) {
//
//	vector<string> res_tmp;
//
//	auto Remingrequestinfos = requestinfos;
//
//	if (day == 0)  // ��һ������ȫ�򣬺���Ҳ���Ե���day�Ĵ�С
//	{
//		Expansion(day, serverinfos, requestinfos);
//		Migrate();
//#ifdef TEST 
//		//printf("There are %d requests waiting to matching !!\n", requestinfos.size());
//#endif
//		for (vector<vector<string>>::iterator req = requestinfos.begin(); req != requestinfos.end(); req++) {
//			int opType = (*req).size() == 3 ? 1 : 0;
//			if (opType) {
//				int resourceEnough = ReDealAddVM(ServerOnVm_type, *req, res_tmp, day);
//			}
//			else {
//				DealDeleteVM(*req);
//			}
//
//		}
//		for (auto re : res_tmp) {
//			res.push_back(re);
//#ifdef TEST
//			cout << re;
//#endif
//		}
//
//		res_tmp.clear();
//
//		ServerIdAdd += ServerOnVm_type.size();
//		ServerOnVm_type.clear();
//	}
//	else {
//#ifdef TEST 
//		//printf("There are %d requests waiting to matching !!\n", requestinfos.size());
//#endif
//
//		for (vector<vector<string>>::iterator req = requestinfos.begin(); req != requestinfos.end(); ) {
//			int opType = (*req).size() == 3 ? 1 : 0;
//			if (opType) {  // add
//				
//				int resourceEnough = DealAddVm(*req, res_tmp);
//				if (resourceEnough != -1) {	
//					req = requestinfos.erase(req);
//				}	
//				else
//				{
//					req++;
//					res_tmp.push_back("null");
//				}
//			}
//			else {
//				DealDeleteVM(*req);
//				req = requestinfos.erase(req);
//			}		
//		}
//		Expansion(day, serverinfos, requestinfos);
//		Migrate();	
//		for (vector<vector<string>>::iterator req = requestinfos.begin(); req != requestinfos.end(); req++) {
//			int opType = (*req).size() == 3 ? 1 : 0;
//			if (opType) {  // add
//
//				int resourceEnough = ReDealAddVM(ServerOnVm_type, *req, res_tmp, day);
//				assert(resourceEnough != -1);
//			}
//			else {
//				DealDeleteVM(*req);
//			}
//		}
//
//		// �ϲ�������add����������
//		for (auto re : res_tmp) {
//			res.push_back(re);
//#ifdef TEST
//			cout << re;
//#endif
//		}
//
//		res_tmp.clear();
//		ServerIdAdd += ServerOnVm_type.size();
//		ServerOnVm_type.clear();
//	}
//}
//
//// ��������Ѽ���
//void ServerPowerCost() {
//	for (int i = 0; i < serverId; i++) {
//		if (serverRunVms[i] != 0) {
//			POWERCOST += sysServerResource[i].PowerCost;
//		}
//	}
//}
//
//
//int main() {
//	clock_t start, finish;
//	start = clock();
//
//#ifdef TEST
//	freopen(filePath.c_str(), "rb", stdin);
//#endif
//
//	int ServerTypes;    // �ṩ�ķ�������������
//	string serverType, cpuCores, memorySize, serverCost, powerCost;
//	scanf("%d", &ServerTypes);
//
//	for (int i = 0; i < ServerTypes; i++) {
//		cin >> serverType >> cpuCores >> memorySize >> serverCost >> powerCost;  // ����һ�����͵ķ�������Ϣ
//		GetServerInfos(serverType, cpuCores, memorySize, serverCost, powerCost); // ������������Ϣ��������
//	}
//
//	int VmTypes = 0;    // �ṩ���������������
//	scanf("%d", &VmTypes);
//
//	string vmType, vmCpuCores, vmMemory, vmTwoNodes;
//	for (int i = 0; i < VmTypes; i++) {
//		cin >> vmType >> vmCpuCores >> vmMemory >> vmTwoNodes;
//		GetVmInfos(vmType, vmCpuCores, vmMemory, vmTwoNodes);
//	}
//
//	int RequestDays = 0, DayRequestNumbers = 0;
//	scanf("%d", &RequestDays);
//	string op, reqVmType, reqId;
//
//	// ��ʼ��������
//	// InitServer(); //��ʼ�������������Դ
//
//	for (int day = 0; day < RequestDays; day++) {
//		scanf("%d", &DayRequestNumbers);
//		RequestInfos.clear();
//		for (int i = 0; i < DayRequestNumbers; i++) {
//			cin >> op;
//			if (op[1] == 'a') {
//				cin >> reqVmType >> reqId;
//				GetRequest(op, reqVmType, reqId);    //����һ�����������������
//			}
//			else {
//				cin >> reqId;
//				GetRequest(op, reqId);              //����һ����ɾ�����������
//			}
//		}
//#ifdef TEST
//		if (day == 0 || (day + 1) % 100 == 0) {
//			//printf("The %d day begin matching!!!\n", day + 1);
//		}
//#endif
//		Match(day, ServerInfos, RequestInfos);
//
//		ServerPowerCost();
//
//	}
//
//	fclose(stdin);
//	finish = clock();
//
//	TOTALCOST = SERVERCOST + POWERCOST;  //�����ܷ���
//
//#ifdef UPLOAD
//	for (auto& s : res) std::cout << s;
//#endif
//
//
//
//#ifdef TEST
//	ofstream out("output.txt");
//	for (auto& s : res) {
//		if (out.is_open()) {
//			out << s;
//		}
//		//std::cout << s;
//	}
//	out.close();
//	printf("\nusr time: %f s \n", double(finish - start) / CLOCKS_PER_SEC);
//	printf("server cost: %lld \npower cost: %lld \ntotal cost: %lld \n", SERVERCOST, POWERCOST, TOTALCOST);
//#endif
//
//	return 0;
//}