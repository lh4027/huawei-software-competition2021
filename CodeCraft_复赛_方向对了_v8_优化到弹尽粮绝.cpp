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
//#include<thread>
//#include<mutex>
//#include<list>
//#include<math.h>
//using namespace std;
//
//#define MIGRATION_LIMT 35  // �˲�������ʱ�����ݼ�2�ķ����ӣ����ݼ�1��С
//#define VMCPUTOMEMEORY 1.0 // ����������ں˱Ȼ���Ϊ����ʱ�Ľ���
//#define DAY 0              // ѡ��ڼ��쿪ʼ�����ٽ������ݲ���
////#define MIGRATION_DAY 5    // ÿ��5��Ǩ��һ��
//#define DELETE_RATIO_TO_REQEST 0.002  // һ���������delete��ռ�ı���, �˲���ԽСǨ�ƴ���Խ�࣬ʱ��Խ��
//#define MIGRATE_SUCCESS_RATIO 0.08  // Ǩ����������ҵ����õķ������󣬻�Ҫ��Ҫ���������Ҹ��ŷ���������ֵ
//#define MIGRATE_FAIL_RATIO 0.02  // ������汾�б�Ǩ����������Ҳ��������õķ������Ĵ����������ٴκ�Ͳ����˵���ֵ������ȡ����С������Ҫ��������
//float   COST_PERFORMANCE = 0.8;       // �Լ۱ȿ��ǹ�ʽϵ����ֵԽ�����Խ�������Լ۱�
//
//#define ALLOC_DISCARD_RATIO 4  // ����ʱ������������Դʣ��Ƚ϶�ķ�������Խ���ʾ����Խ�٣������ϵ�ֵ����
//
////#define DIFF_RATIO_PARA 8
//
//int PatchSize = 10;           // �����������ر��ʱ���Է����������ں˱��зֵĿ���
//int QuestRange = 3000;        // �˲����������ٵ�����������Ч��������
//int buffer_num = 10;        // �˲����ĵ�Сò�Ʒ������ɱ�����ͣ�ʱ��ȴû�����Ӷ��٣���ֻ�����ݼ�2���ã�����������������ֵԽ��Խ�ܼӿ��ٶ�
//
//
//#define TEST
////#define UPLOAD
//
//#ifdef TEST
//const string filePath = "training-1.txt";
//#endif  
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
//	float Cpu_Memory_ratio;  // �ں˱�
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
//	string VmType;     // �����Id
//
//	float Cpu_Memory_ratio;  // ��������ں˱�
//};
//
//// ���ݼ������Ŀɹ���ķ�������Ϣ
//unordered_map<string, ServerInfo> ServerInfos;
//
//// ���ݼ�������������ͺ�
//unordered_map<string, VmInfo> VmInfos;
//
//// ���һ������������Ϣ
//list<vector<string>> RequestInfos;
//
//// ���ǰ������������Ϣ
//list<vector<string>> LastRequestInfos;
//
//// ���еķ�������Դ<serverId, ServerInfo>
//unordered_map<int, ServerInfo> existServerResource;
//
//// ��ǰ�����ķ����������е����������  <serverId, vmNum>
//map<int, int> ServerRunVmsNum;
//
//// ��¼������������Ǹ���������
//unordered_map<string, vector<int>> VmIdOnServer;   //[in] �����Id  [in] ��������ڵķ�����id,�����cpu,������ڴ棬��/˫�ڵ㲿��
//
//// ��¼����������������Щ�����
//unordered_map<int, unordered_map<string, vector<int>>> ServerIdOnVms;   //[in] ������Id  [in] �������id, �����cpu,�ڴ棬��/˫�ڵ㲿��
//
//vector<string> migrantList{ "(migration, 0)\n" };  // �����浱���Ǩ�����
//unordered_map<int, vector<string>> res; // �����������
//
//unordered_map<string, float> PerformanceAndCostRatio;  //����Լ۱�
//
//// ��������������ķ���������Ϣ
//vector<unordered_map<string, ServerInfo>> PatchServerinfos;
//vector<vector<list<vector<string>>>>PatchRequestinfos;
//
//int serverId = 0;  // ������ID(ȫ��)
//multimap<string, unordered_map<string, vector<int>>> ServerIdOnVms_type;  // ͨ����������������¼�ڸ����ͷ������ϵ�vm��
//vector<string>  ServerType;  // ��˳����ÿ��ѡ�������ŷ������������ƣ��������ʱ���մ�˳���ҵ���Ӧ������ID
//mutex m;//ʵ����m���󣬲�Ҫ���Ϊ�������
//
//// �ɱ�
//long long SERVER_COST = 0, POWER_COST = 0, TOTAL_COST = 0;
//
////map<int, map<int, list<vector<string>>>> divideResource;  // ���������CPU��MEMORY�ֿ���<cpu, <memory, vminfoList> >
//unordered_map<string, list<vector<string>>::iterator> addRequestIterMap;  // ӳ������������б�ĵ�������ʵ�ֳ���ʱ��ɾ������
//
//// ������������Ϣ
//void GetServerInfos(string& serverType, string& cpuCores, string& memorySize, string& serverCost, string& powerCost) {
//	string _serverType = "";
//	serverType.pop_back();
//	serverType.erase(0, 1);
//
//	int _cpuCores = 0, _memorySize = 0, _serverCost = 0, _powerCost = 0;
//	cpuCores.pop_back();
//	_cpuCores = stoi(cpuCores);
//
//	memorySize.pop_back();
//	_memorySize = stoi(memorySize);
//
//	serverCost.pop_back();
//	_serverCost = stoi(serverCost);
//
//	powerCost.pop_back();
//	_powerCost = stoi(powerCost);
//
//	ServerInfos[serverType].CpuCores_A = _cpuCores / 2;   // A�ڵ�cpu
//	ServerInfos[serverType].CpuCores_B = _cpuCores / 2;   // B�ڵ�cpu
//
//	ServerInfos[serverType].MemerySize_A = _memorySize / 2;  //A�ڵ��ڴ�
//	ServerInfos[serverType].MemerySize_B = _memorySize / 2;  //B�ڵ��ڴ�
//
//	ServerInfos[serverType].PowerCost = _powerCost;   //���
//	ServerInfos[serverType].ServerCost = _serverCost; //�������ɱ�
//}
//
//// �����������Ϣ
//void GetVmInfos(string& vmType, string& vmCpuCores, string& vmMemory, string& vmTwoNodes) {
//	string _vmType;
//
//	vmType.pop_back();
//	vmType.erase(0, 1);
//
//	int _vmCpuCores = 0, _vmMemory = 0, _vmTwoNodes = 0;
//	vmCpuCores.pop_back();
//	_vmCpuCores = stoi(vmCpuCores);
//
//	vmMemory.pop_back();
//	_vmMemory = stoi(vmMemory);
//
//	vmTwoNodes.pop_back();
//	_vmTwoNodes = stoi(vmTwoNodes);
//
//	VmInfos[vmType].CpuCores = _vmCpuCores;
//	VmInfos[vmType].MemerySize = _vmMemory;
//	VmInfos[vmType].VmTwoNodes = _vmTwoNodes;
//}
//
//// �����������
//void GetRequest(string& buff, string& reqestVmType, string& reqestId) {
//
//	string _buff, _reqVmType, _reqId;
//	buff.pop_back();
//	buff.erase(0, 1);
//
//	reqestVmType.pop_back();
//	reqestId.pop_back();
//	RequestInfos.push_back(vector<string>{buff, reqestVmType, reqestId});   // �û������������û�������Ϣ��
//}
//
//// ����ɾ������
//void GetRequest(string& buff, string& reqestId) {
//
//	reqestId.pop_back();
//
//	buff.pop_back();
//	buff.erase(0, 1);
//	RequestInfos.push_back(vector<string>{buff, reqestId});               // �û�ɾ���������������û�������Ϣ��
//}
//
//// �ڷ������Ϸ����������Դ
//bool AllocServerResources(ServerInfo& server, VmInfo& vm, int serverId, string vmId, vector<string>& res_tmp, int node) {
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
//			assert(serverCoreA >= 0 && serverCoreB >= 0 && serverMemoryA >= 0 && serverMemoryB >= 0);
//
//			// ��¼������������ĸ��������ϣ�˫�ڵ㲿��
//			VmIdOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1,2 };
//
//			// ��¼����������������Щ�����(Ǩ��ʹ��)
//			ServerIdOnVms[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 1, 2 }));
//
//			res_tmp.push_back("(" + to_string(serverId) + ")\n");
//			return true;
//		}
//		else {
//			return false;                                               //��Դ���㣬�ڴ˷������Ϸ���������ʧ��
//		}
//	}
//	else if (node == 1 && serverCoreA >= vmCores && serverMemoryA >= vmMemory) {     //���ڵ㲿����A�ڵ��Ƿ������������
//
//		serverCoreA -= vmCores;
//		serverMemoryA -= vmMemory;
//
//		assert(serverCoreA >= 0 && serverMemoryA >= 0);
//
//		// ��¼������������ĸ��������ϣ�A�ڵ㲿��
//		VmIdOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1 };
//
//		// ��¼����������������Щ�����
//		ServerIdOnVms[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 1}));
//
//		res_tmp.push_back("(" + to_string(serverId) + ", A)\n");
//		return true;
//	}
//	else if (node == 2 && serverCoreB >= vmCores && serverMemoryB >= vmMemory) {    //���ڵ㲿����B�ڵ��Ƿ������������
//
//		serverCoreB -= vmCores;
//		serverMemoryB -= vmMemory;
//
//		assert(serverCoreB >= 0 && serverMemoryB >= 0);
//
//		// ��¼������������ĸ��������ϣ�B�ڵ㲿��
//		VmIdOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,2 };
//
//		// ��¼����������������Щ�����
//		ServerIdOnVms[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 2}));
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
//// ---------------------------------------------------  ����ɾ�����������------------------------------------------------------------------//
//void DealDeleteVM(vector<string>& deleVmInfo) {
//
//	string _vmId = deleVmInfo[1];
//
//	vector<int> _vmInfo = VmIdOnServer[_vmId];
//
//	int _serverId = _vmInfo[0];  //����������ķ�����id
//
//	auto& server = existServerResource[_serverId];
//
//	ReleaseServerResources(server, _vmInfo, _serverId, _vmId);   //�ͷ������
//
//	ServerRunVmsNum[_serverId]--;  //�������ϵ����������-1
//
//	// ����VmIdOnServer
//	auto it1 = VmIdOnServer.find(_vmId);
//	VmIdOnServer.erase(it1);
//
//	//����ÿ̨�������ϴ�ŵ������
//	auto it2 = ServerIdOnVms[_serverId].find(_vmId);
//	ServerIdOnVms[_serverId].erase(it2);
//
//	assert(ServerRunVmsNum[_serverId] >= 0);
//
//}
//
//
//// --------------------------------------------------- ����������Լ۱� --------------------------------------------------------------------//
//void GetBestServer(int day, unordered_map<string, ServerInfo> serverinfos, int requestdays) {
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
//		PerformanceAndCostRatio.insert(make_pair(serverType, (ServerCost + (requestdays - day) * PowerCost) / (1.2 * Cpu + 0.8 * Memory)));
//		//PerformanceAndCostRatio.insert(make_pair(serverType, ServerCost + (requestdays - day) * PowerCost));
//		//PerformanceAndCostRatio.insert(make_pair(serverType, ServerCost * PowerCost));
//
//	}
//}
//
//
//// ---------------------------------------------------��������������������ں˱ȹ���-----------------------------------------------------------------------------------//
//void GetServerInfosPatch(unordered_map<string, ServerInfo> serverinfos, unordered_map<string, VmInfo> vminfos, list<vector<string>> requestinfos) {
//
//
//	vector<list<vector<string>>> requestinfos_buff;  // �зֵ�add����buffer
//	int buffer_length = requestinfos.size() / buffer_num;
//
//	if (requestinfos.size() < QuestRange) {   // �������С��QuestRange���Ͳ����з�
//		requestinfos_buff.resize(1);
//		requestinfos_buff[0] = requestinfos;
//	}
//	else {
//		requestinfos_buff.resize(buffer_num);
//		for (int i = 0; i < buffer_num; i++) {
//
//			if (i == buffer_num - 1) {
//
//				list<vector<string>>::iterator first = requestinfos.begin();
//				list<vector<string>>::iterator second = requestinfos.begin();
//				advance(second, requestinfos.size());
//				requestinfos_buff[i].assign(first, second);
//
//				requestinfos.erase(first, second);
//			}
//			else {
//				list<vector<string>>::iterator first = requestinfos.begin();
//				list<vector<string>>::iterator second = requestinfos.begin();
//				advance(second, buffer_length);
//
//				requestinfos_buff[i].assign(first, second);
//				requestinfos.erase(first, second);
//			}
//		}
//
//	}
//	PatchRequestinfos.resize(requestinfos_buff.size());
//	for (int i = 0; i < requestinfos_buff.size(); i++) {
//		float CPU_to_Memory;
//		int vmCpuAddMemory;
//		multimap<float, string> serverinfostmp;
//		unordered_map<string, ServerInfo> serverinfostmp2;
//		multimap<float, multimap<int, vector<string>>>     vminfostmp;  // �ں˱�->(vmCpu+�ڴ���Դ�� vm��Ϣ)
//		multimap<int, vector<string>> vmInfosSort;  // ֻ��һ��
//		for (auto s : serverinfos) {
//			CPU_to_Memory = (float)(s.second.CpuCores_A + s.second.CpuCores_B) / (float)(s.second.MemerySize_A + s.second.MemerySize_B);
//
//			serverinfostmp.insert(make_pair(CPU_to_Memory, s.first));
//		}
//
//		for (auto re : requestinfos_buff[i]) {
//			if (re.size() > 2) {
//				CPU_to_Memory = (float)(vminfos[re[1]].CpuCores) / (float)(vminfos[re[1]].MemerySize);  // �����vm������ں˱�
//				vmCpuAddMemory = vminfos[re[1]].CpuCores + vminfos[re[1]].MemerySize;                   // �����vm������Դ����cpu+�ڴ棩
//
//				vmInfosSort.insert(make_pair(vmCpuAddMemory, re));
//				vminfostmp.insert(make_pair(CPU_to_Memory, vmInfosSort));                                       // �����ں˱���������
//				vmInfosSort.clear();
//			}
//		}
//
//		if (serverinfostmp.size() >= 20000) {   // ���������������ܴ�ʱ���ֵĸ�ϸ
//
//			PatchServerinfos.resize(PatchSize);
//
//			for (auto s : serverinfostmp) {
//				if (s.first <= 0.2) {   // �˲��������޸ģ�C������ʦ������ɱ��ϵ�У�
//					PatchServerinfos[0][s.second] = serverinfos[s.second];
//				}
//				else if (s.first <= 0.5) {
//					PatchServerinfos[1][s.second] = serverinfos[s.second];
//				}
//				else if (s.first <= 1.0) {
//					PatchServerinfos[2][s.second] = serverinfos[s.second];
//				}
//				else if (s.first <= 5) {
//					PatchServerinfos[3][s.second] = serverinfos[s.second];
//				}
//				else {
//					PatchServerinfos[4][s.second] = serverinfos[s.second];
//				}
//			}
//
//			PatchRequestinfos[i].resize(PatchSize);
//			multimap<int, vector<string>, greater<int>> vmInfosSort_1;
//			multimap<int, vector<string>, greater<int>> vmInfosSort_2;
//			multimap<int, vector<string>, greater<int>> vmInfosSort_3;
//			multimap<int, vector<string>, greater<int>> vmInfosSort_4;
//			multimap<int, vector<string>, greater<int>> vmInfosSort_5;
//
//			for (auto vms = vminfostmp.begin(); vms != vminfostmp.end(); vms++) {
//				if ((*vms).first <= 0.3) {
//					auto it = (*vms).second.begin();
//					vmInfosSort_1.insert(make_pair((*it).first, (*it).second));
//				}
//				else if ((*vms).first <= 0.7) {
//					auto it = (*vms).second.begin();
//					vmInfosSort_2.insert(make_pair((*it).first, (*it).second));
//				}
//				else if ((*vms).first <= 1) {
//					auto it = (*vms).second.begin();
//					vmInfosSort_3.insert(make_pair((*it).first, (*it).second));
//				}
//				else if ((*vms).first <= 5) {
//					auto it = (*vms).second.begin();
//					vmInfosSort_4.insert(make_pair((*it).first, (*it).second));
//				}
//				else {
//					auto it = (*vms).second.begin();
//					vmInfosSort_5.insert(make_pair((*it).first, (*it).second));
//				}
//
//			}
//			for (auto vm : vmInfosSort_1) {
//				PatchRequestinfos[i][0].push_back(vm.second);
//			}
//			for (auto vm : vmInfosSort_2) {
//				PatchRequestinfos[i][1].push_back(vm.second);
//			}
//			for (auto vm : vmInfosSort_3) {
//				PatchRequestinfos[i][2].push_back(vm.second);
//			}
//			for (auto vm : vmInfosSort_4) {
//				PatchRequestinfos[i][3].push_back(vm.second);
//			}
//			for (auto vm : vmInfosSort_5) {
//				PatchRequestinfos[i][4].push_back(vm.second);
//			}
//
//			vmInfosSort_1.clear();
//			vmInfosSort_2.clear();
//			vmInfosSort_3.clear();
//			vmInfosSort_4.clear();
//			vmInfosSort_5.clear();
//		}
//		else if (serverinfostmp.size() < 200000) {  // ֻ�ֳ�2�࣬�ں˱�>1��<1
//			PatchServerinfos.resize(2);
//			for (auto s : serverinfostmp) {
//				if (s.first <= 1.0) {
//					PatchServerinfos[0][s.second] = serverinfos[s.second];
//				}
//				else {
//					PatchServerinfos[1][s.second] = serverinfos[s.second];
//				}
//			}
//
//			PatchRequestinfos[i].resize(2);
//			multimap<int, vector<string>, greater<int>> vmInfosSort_1;
//			multimap<int, vector<string>, greater<int>> vmInfosSort_2;
//			for (auto vms = vminfostmp.begin(); vms != vminfostmp.end(); vms++) {
//				if ((*vms).first <= 1.0) {                                               // �˲��������޸� 30���
//					auto it = (*vms).second.begin();
//					vmInfosSort_1.insert(make_pair((*it).first, (*it).second));
//				}
//				else {
//					auto it = (*vms).second.begin();
//					vmInfosSort_2.insert(make_pair((*it).first, (*it).second));
//				}
//			}
//
//			for (auto vm : vmInfosSort_1) {
//				PatchRequestinfos[i][0].push_back(vm.second);
//			}
//			for (auto vm : vmInfosSort_2) {
//				PatchRequestinfos[i][1].push_back(vm.second);
//			}
//			vmInfosSort_1.clear();
//			vmInfosSort_2.clear();
//
//		}
//		if (false) {  // �������ں˱Ƚ��з���(����Ч�����У�
//			PatchServerinfos.resize(1);
//			PatchServerinfos[0] = serverinfos;
//
//			PatchRequestinfos[i].resize(1);
//			multimap<int, vector<string>, greater<int>> vmInfosSort_1;
//			for (auto vms = vminfostmp.begin(); vms != vminfostmp.end(); vms++) {
//				auto it = (*vms).second.begin();
//				vmInfosSort_1.insert(make_pair((*it).first, (*it).second));
//			}
//
//			for (auto vm : vmInfosSort_1) {
//				PatchRequestinfos[i][0].push_back(vm.second);
//			}
//			vmInfosSort_1.clear();
//		}
//		serverinfostmp.clear();
//		vminfostmp.clear();
//
//	}
//}
//
//
//// ------------------------------------------------------------------ ���� + ���߳�--------------------------------------------------------------------------- //
//ofstream testExpansion("TestExpansion.txt");
//
//void assignVmToServer(int buff, vector<vector<list<vector<string>>>>& requestinfos_patch,
//	set<string>& ServerTypeNum, multimap<string, ServerInfo>& sysServerResourceTemp,
//	vector<unordered_map<string, ServerInfo>>& serverinfos) {
//
//	for (int buffNo = 0; buffNo < buff; buffNo++) {
//		for (int patch = 0; patch < requestinfos_patch[buffNo].size(); patch++) {
//
//			while (requestinfos_patch[buffNo][patch].size() > 0) {    // ���������Ϣ����һֱѭ��
//
//				unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverTemp; // ��¼�������ʹճ��������ƥ����Ϣ <�������ͺţ�<�����Id��vm�ڴ棬vmCpu,vmTwoNodes>>
//				multimap<int, string> Matching_degree;  // ��¼�ճ��ķ�������ƥ��̶Ȳ�ֵ��ȡ��С
//				unordered_map<string, ServerInfo> ServerResourceTemp;
//				multimap<float, string> CostAndDiffer; // ���Լ۱ȿ��ǽ�ȥ����¼��ֵ���Լ۱�Ȩ����ֵ
//
//				for (auto s : serverinfos[patch]) {    // �����з���������ƥ��
//
//					int taget_tatal = s.second.CpuCores_A + s.second.CpuCores_B + s.second.MemerySize_A + s.second.MemerySize_B;  //��ʼ���������ʹյ����������Դ��ֵ
//					int taget_CpuCores_A = s.second.CpuCores_A, taget_CpuCores_B = s.second.CpuCores_B, taget_MemerySize_A = s.second.MemerySize_A, taget_MemerySize_B = s.second.MemerySize_B;
//
//					int i = 0;
//					for (list<vector<string>>::iterator vm = requestinfos_patch[buffNo][patch].begin(); vm != requestinfos_patch[buffNo][patch].end(); ++vm, ++i) {
//
//						int vmCores = VmInfos[(*vm)[1]].CpuCores;
//						int vmMemory = VmInfos[(*vm)[1]].MemerySize;
//						int vmTwoNodes = VmInfos[(*vm)[1]].VmTwoNodes;
//						vector<int> vminfoAdd;
//
//						if (vmTwoNodes) {    // ˫�ڵ�vm
//
//							int needCores = vmCores / 2;
//							int needMemory = vmMemory / 2;
//							vminfoAdd.resize(4);
//
//							if (taget_CpuCores_A >= needCores && taget_CpuCores_B >= needCores && taget_MemerySize_A >= needMemory && taget_MemerySize_B >= needMemory) {
//								taget_CpuCores_A -= needCores;
//								taget_CpuCores_B -= needCores;
//								taget_MemerySize_A -= needMemory;
//								taget_MemerySize_B -= needMemory;
//
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ
//
//								vminfoAdd[0] = vmCores;
//								vminfoAdd[1] = vmMemory;
//								vminfoAdd[2] = 1;
//								vminfoAdd[3] = 2;
//								vmsOnserverTemp[s.first].insert(make_pair((*vm)[2], vminfoAdd));
//								vminfoAdd.clear();
//							}
//						}
//						else {  // ���ڵ�
//							vminfoAdd.resize(3);
//							if (taget_CpuCores_A >= vmCores && taget_MemerySize_A >= vmMemory) {
//
//								taget_CpuCores_A -= vmCores;
//								taget_MemerySize_A -= vmMemory;
//
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ
//
//								vminfoAdd[0] = vmCores;
//								vminfoAdd[1] = vmMemory;
//								vminfoAdd[2] = 1;
//
//								vmsOnserverTemp[s.first].insert(make_pair((*vm)[2], vminfoAdd));
//								vminfoAdd.clear();
//							}
//							else if (taget_CpuCores_B >= vmCores && taget_MemerySize_B >= vmMemory) {
//
//								taget_CpuCores_B -= vmCores;
//								taget_MemerySize_B -= vmMemory;
//
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ
//
//								vminfoAdd[0] = vmCores;
//								vminfoAdd[1] = vmMemory;
//								vminfoAdd[2] = 2;
//
//								vmsOnserverTemp[s.first].insert(make_pair((*vm)[2], vminfoAdd));
//								vminfoAdd.clear();
//							}
//						}
//					} // һ̨��������������
//					Matching_degree.insert(make_pair(taget_tatal, s.first));   // ��¼�������������ƥ��Ĳ�ֵ
//
//					// ��¼��̨�������ϵ���Դʣ��������
//					ServerResourceTemp[s.first].CpuCores_A = taget_CpuCores_A;
//					ServerResourceTemp[s.first].CpuCores_B = taget_CpuCores_B;
//					ServerResourceTemp[s.first].MemerySize_A = taget_MemerySize_A;
//					ServerResourceTemp[s.first].MemerySize_B = taget_MemerySize_B;
//					ServerResourceTemp[s.first].PowerCost = ServerInfos[s.first].PowerCost;
//
//				}  // ���з�����һ��ѭ������
//
//				//ѡ����ƥ������з�����
//
//				// ���ڲ�ֵ��С�ķ���������ȴ�����������Ҫ����������ȡ��һ��
//				for (multimap<int, string>::iterator it = Matching_degree.begin(); it != Matching_degree.end(); ) {
//					if (vmsOnserverTemp[(*it).second].size() == 0) {
//						it = Matching_degree.erase(it);
//					}
//					else {
//						it++;
//					}
//				}
//				for (auto m : Matching_degree) {
//					//float costanddiffer = (float)(ServerInfos[m.second].ServerCost) / vmsOnserverTemp[m.second].size();
//					float costanddiffer = m.first * (1.0 - COST_PERFORMANCE) + PerformanceAndCostRatio[m.second] * COST_PERFORMANCE;  // �˲����������Ե��ڣ������ԣ����ݼ�2�������Լ۱�Ч����ã����ݼ�1����0.5���
//					//float costanddiffer = m.first;
//					CostAndDiffer.insert(make_pair(costanddiffer, m.second));
//				}
//				auto first = CostAndDiffer.begin();
//
//				m.lock();
//				ServerTypeNum.insert((*first).second); // ��¼һ��������ַ��������������
//
//				ServerIdOnVms_type.insert(make_pair((*first).second, vmsOnserverTemp[(*first).second]));   //��ͬ�ķ��������ͻ�������һ����ϳɹ�����������˳����serverID
//
//				// ������ķ���������ʣ�����Դ������ϵͳ����ռ����Դ���(�˴���multimap���շ��������ƴ��룬��Ϊ����ServerIdOnVms_type�еĴ�ķ�����˳�򱣳�һ�£������ٽ���ת����id����)
//				sysServerResourceTemp.insert(make_pair((*first).second, ServerResourceTemp[(*first).second]));
//
//				SERVER_COST += ServerInfos[(*first).second].ServerCost;  // ��������������ۼӼ���
//				m.unlock();
//
//				// ��requestinfosȥ������õ������,������Ҫ�޸ģ����ɾ����������add����
//				auto s = vmsOnserverTemp[(*first).second]; // ��¼�պõ������
//
//				for (auto ss : s) {
//					int pos = 0;
//					vector<string>::iterator deleVminfotmp;
//					list<vector<string>>::iterator deleVminfo;
//					for (auto it = requestinfos_patch[buffNo][patch].begin(); it != requestinfos_patch[buffNo][patch].end(); ++it) {
//						deleVminfotmp = find(it->begin(), it->end(), ss.first);
//						if (deleVminfotmp != it->end()) {
//							deleVminfo = it;
//							break;
//						}
//					}
//					requestinfos_patch[buffNo][patch].erase(deleVminfo);
//				}
//				Matching_degree.clear();
//			}  // һ����������������
//		}
//	}
//}
//long long CPU = 0, Memory = 0;
//void Expansion(int day, vector<unordered_map<string, ServerInfo>> serverinfos, vector<vector<list<vector<string>>>> requestinfos_patch) {
//
//	unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverBestmatch; // ��¼����ķ������ʹճ��������ƥ����Ϣ <�������ͺţ�<�����Id��vm�ڴ棬vmCpu,vmTwoNodes>>
//
//	set<string> ServerTypeNum;
//
//	multimap<string, ServerInfo> sysServerResourceTemp;
//
//	int buffNum;
//	buffNum = requestinfos_patch.size() / 2;
//
//	vector<vector<vector<list<vector<string>>>>> requestinfos_patch_buff;
//
//
//	if (buffNum == 0) {
//		requestinfos_patch_buff.resize(1);
//		requestinfos_patch_buff[0].resize(buffNum);
//		requestinfos_patch_buff[0] = requestinfos_patch;
//		assignVmToServer(1, requestinfos_patch_buff[0], ServerTypeNum, sysServerResourceTemp, serverinfos);
//	}
//	else {
//		requestinfos_patch_buff.resize(2);
//		requestinfos_patch_buff[0].resize(buffNum);
//		requestinfos_patch_buff[1].resize(buffNum);
//
//		int n = 0;
//		for (auto re : requestinfos_patch) {
//
//			if (n < buffNum) {
//				requestinfos_patch_buff[0][n] = re;
//			}
//			else
//				requestinfos_patch_buff[1][n - buffNum] = re;
//			n++;
//		}
//
//		thread t1(assignVmToServer, buffNum, ref(requestinfos_patch_buff[0]), ref(ServerTypeNum), ref(sysServerResourceTemp), ref(serverinfos));
//		thread t2(assignVmToServer, buffNum, ref(requestinfos_patch_buff[1]), ref(ServerTypeNum), ref(sysServerResourceTemp), ref(serverinfos));
//		t1.join();
//		t2.join();
//	}
//
//	requestinfos_patch.clear();
//	requestinfos_patch_buff.clear(); // ���һ���������Ϣ
//	PatchRequestinfos.clear();
//	//PatchServerinfos.clear();
//
//	// �����������������Ϣ
//	string s = "(purchase, ";
//
//	s += to_string(ServerTypeNum.size()) + ")\n";
//	//res.push_back(s);
//	res[day].push_back(s);
//
//#ifdef TEST
//	cout << s;
//#endif 
//
//	int ServertypeNum = 0;
//	if (ServerTypeNum.size() != 0) {   // ��������Ϊ0ʱ���
//		string serverType = (*ServerIdOnVms_type.begin()).first;
//		multimap<string, ServerInfo>::iterator sSr = sysServerResourceTemp.begin();
//		for (auto sov : ServerIdOnVms_type) {
//
//			// ��¼����������������Щ�����
//			ServerIdOnVms[serverId] = sov.second;
//
//			for (auto ss : sov.second) {
//				if (ss.second.size() > 3) {   // ˫�ڵ�
//					VmIdOnServer[ss.first].push_back(serverId);
//					VmIdOnServer[ss.first].push_back(ss.second[0]);
//					VmIdOnServer[ss.first].push_back(ss.second[1]);
//					VmIdOnServer[ss.first].push_back(ss.second[2]);
//					VmIdOnServer[ss.first].push_back(ss.second[3]);
//
//					ServerRunVmsNum[serverId]++;  // ��¼��ID�������������˶���̨vm
//				}
//				else {      // ���ڵ�
//					VmIdOnServer[ss.first].push_back(serverId);
//					VmIdOnServer[ss.first].push_back(ss.second[0]);
//					VmIdOnServer[ss.first].push_back(ss.second[1]);
//					VmIdOnServer[ss.first].push_back(ss.second[2]);
//
//					ServerRunVmsNum[serverId]++;  // ��¼��ID�������������˶���̨vm
//				}
//			}
//			// ��¼������������Դ��ȫ�֣�
//			existServerResource[serverId] = (*sSr).second;
//			sSr++;
//			// for test
//			CPU += existServerResource[serverId].CpuCores_A + existServerResource[serverId].CpuCores_B;
//			Memory += existServerResource[serverId].MemerySize_A + existServerResource[serverId].MemerySize_B;
//			if (testExpansion.is_open()) {
//				testExpansion << "������ID: " + to_string(serverId) + "   ʣ����Դ: " + "   CPU_A:" + to_string(existServerResource[serverId].CpuCores_A)
//					+ "   CPU_B:" + to_string(existServerResource[serverId].CpuCores_B) + "   Mem_A:" + to_string(existServerResource[serverId].MemerySize_A)
//					+ "   Mem_B:" + to_string(existServerResource[serverId].MemerySize_B) + "\n";
//			}
//			if (sov.first != serverType) {
//				s = "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
//				res[day].push_back(s);
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
//		res[day].push_back(s);
//
//#ifdef TEST
//		cout << "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
//#endif	
//		//ServerBuyNum.clear();
//	}
//
//	//���������Ǩ���б��б�����һ��ģ�������һ��
//	if (!migrantList.empty()) {
//		for (int i = 0; i < migrantList.size(); ++i)
//		{
//			res[day].push_back(migrantList[i]);
//
//#ifdef TEST
//			cout << migrantList[i];
//#endif 
//		}
//		migrantList.clear();
//	}
//}
//
//
//// ----------------------------------------------------------------------������������-------------------------------------------------------------------------------------------- //
//vector<unordered_map<int, ServerInfo>> PatchExistServerResource;
//vector<multimap<int, int>> patchExistServerIdx;  // key: ��������Դ������value: ������id
//vector<vector<list<vector<string>>>>PatchRequestinfosLeft;
//
//void GetExsitServerResourcePatch(unordered_map<int, ServerInfo> existserverresource, unordered_map<string, VmInfo> vminfos, list<vector<string>> requestinfos) {
//
//	vector<list<vector<string>>> requestinfos_buff;  // �зֵ�add����buffer
//
//	int ServerResource;  // �ִ�ÿ̨��������Դ�ܺ�
//	multiset<int> VmResourceMin_1; // add������ں˱�С��1����Сvm��Դ�ܺ�
//	multiset<int> VmResourceMin_2; // add������ں˱ȴ���1����Сvm��Դ�ܺ�
//
//	//unordered_map<int, ServerInfo> existServerResourcePrune;  // ���ִ���Դ���м�֦������С��������������Դ������ϵͳ�ִ����������Դ��ʱ������̨���������б��г�ȥ�����μ�ѭ��
//
//	float CPU_to_Memory;
//
//	// ������������������������С�����з֣��������ں˱ȷ�Ϊ���࣬��������Դ��С��������
//	int buffer_length = requestinfos.size() / buffer_num;
//	if (requestinfos.size() < QuestRange) {   // �������С��QuestRange���Ͳ����з�
//		requestinfos_buff.resize(1);
//		requestinfos_buff[0] = requestinfos;
//	}
//	else {
//		requestinfos_buff.resize(buffer_num);
//		for (int i = 0; i < buffer_num; i++) {
//			if (i == buffer_num - 1) {
//
//				list<vector<string>>::iterator first = requestinfos.begin();
//				list<vector<string>>::iterator second = requestinfos.begin();
//				advance(second, requestinfos.size());
//				requestinfos_buff[i].assign(first, second);
//
//				requestinfos.erase(first, second);
//			}
//			else {
//				list<vector<string>>::iterator first = requestinfos.begin();
//				list<vector<string>>::iterator second = requestinfos.begin();
//				advance(second, buffer_length);
//
//				requestinfos_buff[i].assign(first, second);
//				requestinfos.erase(first, second);
//			}
//		}
//
//	}
//	PatchRequestinfosLeft.resize(requestinfos_buff.size());
//	addRequestIterMap.clear();
//	//divideResource.clear();
//	//divideResource.resize(requestinfos_buff.size());
//	for (int i = 0; i < requestinfos_buff.size(); i++) {
//
//		int vmCpuAddMemory;
//
//		multimap<float, multimap<int, vector<string>>>     vminfostmp;  // �ں˱�->(vmCpu+�ڴ���Դ�� vm��Ϣ)
//		multimap<int, vector<string>> vmInfosSort;  // ֻ��һ��
//
//		for (auto re : requestinfos_buff[i]) {
//			if (re.size() > 2) {
//				CPU_to_Memory = (float)(vminfos[re[1]].CpuCores) / (float)(vminfos[re[1]].MemerySize);  // �����vm������ں˱�
//				vmCpuAddMemory = vminfos[re[1]].CpuCores + vminfos[re[1]].MemerySize;                   // �����vm������Դ����cpu+�ڴ棩
//
//				vmInfosSort.insert(make_pair(vmCpuAddMemory, re));
//				vminfostmp.insert(make_pair(CPU_to_Memory, vmInfosSort));                                       // �����ں˱���������
//				vmInfosSort.clear();
//			}
//		}
//
//		PatchRequestinfosLeft[i].resize(2);
//		//divideResource[i].resize(2);
//		multimap<int, vector<string>, greater<int>> vmInfosSort_1;
//		multimap<int, vector<string>, greater<int>> vmInfosSort_2;
//		for (auto vms = vminfostmp.begin(); vms != vminfostmp.end(); vms++) {
//			if ((*vms).first <= 1.0) {
//				auto it = (*vms).second.begin();
//				vmInfosSort_1.insert(make_pair((*it).first, (*it).second));
//			}
//			else {
//				auto it = (*vms).second.begin();
//				vmInfosSort_2.insert(make_pair((*it).first, (*it).second));
//			}
//		}
//
//		for (auto vm : vmInfosSort_1) {
//			PatchRequestinfosLeft[i][0].push_back(vm.second);
//
//			addRequestIterMap[vm.second[2]] = prev(PatchRequestinfosLeft[i][0].end(), 1);
//			// ȡ���������
//			int vmcpu = VmInfos[vm.second[1]].VmTwoNodes ? VmInfos[vm.second[1]].CpuCores / 2 : VmInfos[vm.second[1]].CpuCores;
//			int vmmemo = VmInfos[vm.second[1]].VmTwoNodes ? VmInfos[vm.second[1]].MemerySize / 2 : VmInfos[vm.second[1]].MemerySize;
//			//divideResource[i][0][vmcpu][vmmemo].push_back(vm.second);
//		}
//		for (auto vm : vmInfosSort_2) {
//			PatchRequestinfosLeft[i][1].push_back(vm.second);
//
//			addRequestIterMap[vm.second[2]] = prev(PatchRequestinfosLeft[i][1].end(), 1);  // �����������Ȼ������Ϊ�е�ɵ��
//			// ȡ���������
//			int vmcpu = VmInfos[vm.second[1]].VmTwoNodes ? VmInfos[vm.second[1]].CpuCores / 2 : VmInfos[vm.second[1]].CpuCores;
//			int vmmemo = VmInfos[vm.second[1]].VmTwoNodes ? VmInfos[vm.second[1]].MemerySize / 2 : VmInfos[vm.second[1]].MemerySize;
//			//divideResource[i][0][vmcpu][vmmemo].push_back(vm.second);
//		}
//		// ȡ��������������С��Դ
//		multimap<int, vector<string>>::iterator tmp1 = vmInfosSort_1.end();
//		tmp1--;
//		VmResourceMin_1.insert((*tmp1).first);
//
//		multimap<int, vector<string>>::iterator tmp2 = vmInfosSort_2.end();
//		tmp2--;
//		VmResourceMin_2.insert((*tmp2).first);
//
//		vmInfosSort_1.clear();
//		vmInfosSort_2.clear();
//
//
//		vminfostmp.clear();
//
//	}
//
//	//��ʣ��������ϵ���Դ���࣬��ʱֻ�����ں˱ȷ�Ϊ����
//	multimap<float, int> exsitServerResourceTmp;
//	for (auto& s : existserverresource) {
//		CPU_to_Memory = (float)(s.second.CpuCores_A + s.second.CpuCores_B) / (float)(s.second.MemerySize_A + s.second.MemerySize_B);
//
//		exsitServerResourceTmp.insert(make_pair(CPU_to_Memory, s.first));
//	}
//	// ʣ���������Դֻ�ֳ�2�࣬�ں˱�>1��<1
//	PatchExistServerResource.clear();
//	PatchExistServerResource.resize(2);
//	patchExistServerIdx.clear();
//	patchExistServerIdx.resize(2);
//	for (auto s : exsitServerResourceTmp) {
//		ServerResource = existserverresource[s.second].CpuCores_A + existserverresource[s.second].CpuCores_B + existserverresource[s.second].MemerySize_A + existserverresource[s.second].MemerySize_B;
//		if (s.first <= 1.0 && ServerResource >= (*VmResourceMin_1.begin()) && (existserverresource[s.second].CpuCores_A != 0 && existserverresource[s.second].CpuCores_B != 0) && (existserverresource[s.second].MemerySize_A != 0 && existserverresource[s.second].MemerySize_B != 0)) {
//			int total = existserverresource[s.second].CpuCores_A + existserverresource[s.second].CpuCores_B + existserverresource[s.second].MemerySize_A + existserverresource[s.second].MemerySize_B;
//			PatchExistServerResource[0][s.second] = existserverresource[s.second];
//			patchExistServerIdx[0].insert(make_pair(total, s.second));
//		}
//		else if (s.first > 1.0 && ServerResource >= (*VmResourceMin_2.begin()) && (existserverresource[s.second].CpuCores_A != 0 && existserverresource[s.second].CpuCores_B != 0) && (existserverresource[s.second].MemerySize_A != 0 && existserverresource[s.second].MemerySize_B != 0)) {
//			int total = existserverresource[s.second].CpuCores_A + existserverresource[s.second].CpuCores_B + existserverresource[s.second].MemerySize_A + existserverresource[s.second].MemerySize_B;
//			PatchExistServerResource[1][s.second] = existserverresource[s.second];
//			patchExistServerIdx[1].insert(make_pair(total, s.second));
//		}
//	}
//	VmResourceMin_1.clear();
//	VmResourceMin_2.clear();
//	exsitServerResourceTmp.clear();
//}
//
//void allocOneServer(map<int, map<int, list<vector<string>>>>& divideResource, vector<string>& finalVm, int sCPUA, int sMemoA, int sCPUB, int sMemoB) {
//
//	list<vector<string>> vmList;
//	for (auto cpuIt = divideResource.begin(); cpuIt != divideResource.end() && (cpuIt->first <= sCPUA || cpuIt->first <= sCPUB); ++cpuIt) {
//		map<int, list<vector<string>>> memoMap = cpuIt->second;
//		for (auto memoIt = memoMap.begin(); memoIt != memoMap.end(); ++memoIt) {
//			if ((cpuIt->first <= sCPUA && memoIt->first <= sMemoA) || (cpuIt->first <= sCPUB && memoIt->first <= sMemoB)) {
//				list<vector<string>> vms = memoIt->second;
//				for (auto vmIt = vms.begin(); vmIt != vms.end(); ++vmIt) {
//					int vmCpu = VmInfos[(*vmIt)[1]].CpuCores, vmMemo = VmInfos[(*vmIt)[1]].MemerySize, vmTwoNodes = VmInfos[(*vmIt)[1]].VmTwoNodes;
//					if (vmTwoNodes == 0 || (vmTwoNodes && vmCpu / 2 <= sCPUA && vmCpu / 2 <= sCPUB && vmMemo / 2 <= sMemoA && vmMemo / 2 <= sMemoB)) {  // ���ڵ���������Ҫ�Ž�ȥ
//						vmList.push_front(*vmIt);  // �Ӵ�С�������
//					}
//				}
//			}
//			else
//				break;
//		}
//	}
//	for (auto vmIt = vmList.begin(); vmIt != vmList.end(); ++vmIt) {
//		int vmCpu = VmInfos[(*vmIt)[1]].CpuCores, vmMemo = VmInfos[(*vmIt)[1]].MemerySize, vmTwoNodes = VmInfos[(*vmIt)[1]].VmTwoNodes;
//		int remainCpuA, remainMemoA, remainCpuB, remainMemoB;
//		if (vmTwoNodes) {
//			remainCpuA = sCPUA - vmCpu / 2, remainMemoA = sMemoA - vmMemo / 2, remainCpuB = sCPUB - vmCpu / 2, remainMemoB = sMemoB - vmMemo / 2;
//		}
//		else {
//			remainCpuA = sCPUA - vmCpu, remainMemoA = sMemoA - vmMemo, remainCpuB = sCPUB - vmCpu, remainMemoB = sMemoB - vmMemo;
//		}
//		for (auto cpuIt = divideResource.begin(); cpuIt != divideResource.end() && (cpuIt->first <= remainCpuA || cpuIt->first <= remainCpuB); ++cpuIt) {
//			map<int, list<vector<string>>> memoMap = cpuIt->second;
//			for (auto memoIt = memoMap.begin(); memoIt != memoMap.end(); ++memoIt) {
//				if ((cpuIt->first <= remainCpuA && memoIt->first <= remainMemoA) || (cpuIt->first <= remainCpuB && memoIt->first <= remainMemoB)) {
//					list<vector<string>> vms = memoIt->second;
//					for (auto subVmIt = vms.begin(); subVmIt != vms.end(); ++subVmIt) {
//						int vmCpu = VmInfos[(*subVmIt)[1]].CpuCores, vmMemo = VmInfos[(*subVmIt)[1]].MemerySize, vmTwoNodes = VmInfos[(*subVmIt)[1]].VmTwoNodes;
//						if ((vmTwoNodes == 0 || (vmTwoNodes && vmCpu / 2 <= sCPUA && vmCpu / 2 <= sCPUB && vmMemo / 2 <= sMemoA && vmMemo / 2 <= sMemoB)) && (*vmIt)[2] != (*subVmIt)[2]) {  // ���ڵ���������Ҫ�Ž�ȥ
//							return;
//						}
//					}
//				}
//				else {
//					break;
//				}
//			}
//		}
//	}
//	int minVm = INT32_MAX;
//	for (auto& s : vmList) {
//		int totalVm = VmInfos[s[1]].CpuCores + VmInfos[s[1]].MemerySize, totalSer = sCPUA + sMemoA + sCPUB + sMemoB;
//		if (totalSer - totalVm < minVm) {
//			minVm = totalSer - totalVm;
//			finalVm = s;
//		}
//	}
//}
//
//void constructVmList(map<int, map<int, list<vector<string>>>>& divideResource, list<vector<string>>& vmList, int sCPUA, int sMemoA, int sCPUB, int sMemoB) {
//
//	for (auto cpuIt = divideResource.begin(); cpuIt != divideResource.end() && (cpuIt->first <= sCPUA || cpuIt->first <= sCPUB); ++cpuIt) {
//		map<int, list<vector<string>>> memoMap = cpuIt->second;
//		for (auto memoIt = memoMap.begin(); memoIt != memoMap.end() && ((cpuIt->first <= sCPUA && memoIt->first <= sMemoA) || (memoIt->first <= sMemoB)); ++memoIt) {
//			list<vector<string>> vms = memoIt->second;
//			for (auto vmIt = vms.begin(); vmIt != vms.end(); ++vmIt) {
//				int vmCpu = VmInfos[(*vmIt)[1]].CpuCores, vmMemo = VmInfos[(*vmIt)[1]].MemerySize, vmTwoNodes = VmInfos[(*vmIt)[1]].VmTwoNodes;
//				if (vmTwoNodes == 0 || (vmTwoNodes && vmCpu / 2 <= sCPUA && vmCpu / 2 <= sCPUB && vmMemo / 2 <= sMemoA && vmMemo / 2 <= sMemoB)) {  // ���ڵ���������Ҫ�Ž�ȥ
//					vmList.push_front(*vmIt);
//				}
//			}
//		}
//	}
//}
//
//multimap<int, unordered_map<string, pair< vector<int>, list<vector<string>>::iterator >>> ServerIdOnVms_alloc;  // ͨ��������Id����¼�����ڸ����ͷ������ϵ�vm��
//void DealAdd(list<vector<string>> requestinfos, unordered_map<int, ServerInfo>& existServerResource, unordered_map<string, VmInfo> vminfos, vector<string>& res_tmp) {
//
//	GetExsitServerResourcePatch(existServerResource, vminfos, requestinfos);
//
//	//clock_t s = clock();
//	for (int buffNo = 0; buffNo < PatchRequestinfosLeft.size(); buffNo++) {
//
//		for (int patch = 0; patch < PatchRequestinfosLeft[buffNo].size(); patch++) {
//
//			list<vector<string>> discardRequest;
//			unordered_map<int, int> unusedServer;  // ��һ�ֲ��ò������ķ�����id
//
//			map<int, map<int, list<vector<string>>>> divideResource;  // ���������CPU��MEMORY�ֿ���<cpu, <memory, vminfoList> >
//			unordered_map<string, list<vector<string>>::iterator> divideRequestIterMap;  // �������������ɾ��
//			for (auto& s : PatchRequestinfosLeft[buffNo][patch]) {
//				int vmcpu = VmInfos[s[1]].VmTwoNodes ? VmInfos[s[1]].CpuCores / 2 : VmInfos[s[1]].CpuCores;
//				int vmmemo = VmInfos[s[1]].VmTwoNodes ? VmInfos[s[1]].MemerySize / 2 : VmInfos[s[1]].MemerySize;
//				divideResource[vmcpu][vmmemo].push_back(s);
//				divideRequestIterMap.insert(make_pair(s[2], prev(divideResource[vmcpu][vmmemo].end(), 1)));
//			}
//
//			for (auto it = patchExistServerIdx[patch].begin(), nextIt = it; it != patchExistServerIdx[patch].end(); it = nextIt) {  // ����������Դ��С����������
//				++nextIt;
//				auto& s = PatchExistServerResource[patch][it->second];
//				if (s.CpuCores_A + s.CpuCores_B + s.MemerySize_A + s.MemerySize_B < 120) {
//					vector<string> oneVm;
//					allocOneServer(divideResource, oneVm, s.CpuCores_A, s.MemerySize_A, s.CpuCores_B, s.MemerySize_B);
//
//					if (!oneVm.empty()) {  // ��Ψһ������������Ψһ�ķ�����
//						int vvid = stoi(oneVm[2]);
//						unordered_map<string, pair<vector<int>, list<vector<string>>::iterator>> temp;
//						vector<int> vminfoAdd = { VmInfos[oneVm[1]].CpuCores, VmInfos[oneVm[1]].MemerySize };
//						if (VmInfos[oneVm[1]].VmTwoNodes) {
//							vminfoAdd.push_back(1);
//							vminfoAdd.push_back(2);
//						}
//						else {
//							// ���¼���һ�·����ĸ��ڵ�ȽϺ�
//							int node = -1;
//							int ca = s.CpuCores_A - VmInfos[oneVm[1]].CpuCores, cb = s.CpuCores_B - VmInfos[oneVm[1]].CpuCores;
//							int ma = s.MemerySize_A - VmInfos[oneVm[1]].MemerySize, mb = s.MemerySize_B - VmInfos[oneVm[1]].MemerySize;
//							if (ca >= 0 && ma >= 0 && cb >= 0 && mb >= 0)
//								if (ca + ma < cb + mb)
//									node = 1;
//								else
//									node = 2;
//							else if (ca >= 0 && ma >= 0)
//								node = 1;
//							else
//								node = 2;
//							vminfoAdd.push_back(node);
//						}
//						temp[oneVm[2]] = make_pair(vminfoAdd, addRequestIterMap[oneVm[2]]);
//
//						ServerIdOnVms_alloc.insert(make_pair(it->second, temp));
//						//PatchExistServerResource[patch][(*first).second] = ServerResourceTemp[(*first).second];
//
//
//						if (temp[oneVm[2]].first.size() == 4) {
//							existServerResource[it->second].CpuCores_A = s.CpuCores_A - VmInfos[oneVm[1]].CpuCores / 2;
//							existServerResource[it->second].CpuCores_B = s.CpuCores_B - VmInfos[oneVm[1]].CpuCores / 2;
//							existServerResource[it->second].MemerySize_A = s.MemerySize_A - VmInfos[oneVm[1]].MemerySize / 2;
//							existServerResource[it->second].MemerySize_B = s.MemerySize_B - VmInfos[oneVm[1]].MemerySize / 2;
//							divideResource[VmInfos[oneVm[1]].CpuCores / 2][VmInfos[oneVm[1]].MemerySize / 2].erase(divideRequestIterMap[oneVm[2]]);
//						}
//						else if (temp[oneVm[2]].first[2] == 1) {
//							existServerResource[it->second].CpuCores_A = s.CpuCores_A - VmInfos[oneVm[1]].CpuCores;
//							existServerResource[it->second].MemerySize_A = s.MemerySize_A - VmInfos[oneVm[1]].MemerySize;
//							divideResource[VmInfos[oneVm[1]].CpuCores][VmInfos[oneVm[1]].MemerySize].erase(divideRequestIterMap[oneVm[2]]);
//						}
//						else {
//							existServerResource[it->second].CpuCores_B = s.CpuCores_B - VmInfos[oneVm[1]].CpuCores;
//							existServerResource[it->second].MemerySize_B = s.MemerySize_B - VmInfos[oneVm[1]].MemerySize;
//							divideResource[VmInfos[oneVm[1]].CpuCores][VmInfos[oneVm[1]].MemerySize].erase(divideRequestIterMap[oneVm[2]]);
//						}
//
//						PatchExistServerResource[patch].erase(it->second);
//						PatchRequestinfosLeft[buffNo][patch].erase(addRequestIterMap[oneVm[2]]);
//
//						// ������
//						patchExistServerIdx[patch].erase(it);
//					}
//				}
//			}
//
//			while (PatchRequestinfosLeft[buffNo][patch].size() > 0) {    // ���������Ϣ����һֱѭ��
//
//				unordered_map<int, unordered_map<string, pair< vector<int>, list<vector<string>>::iterator >>> vmsOnserverTemp; // ��¼�������ʹճ��������ƥ����Ϣ <������id��<�����Id��vm�ڴ棬vmCpu,vmTwoNodes>>
//
//				multimap<int, int> Matching_degree;  // ��¼�ճ��ķ�������ƥ��̶Ȳ�ֵ��ȡ��С <��ֵ��������ID>
//				unordered_map<int, ServerInfo> ServerResourceTemp; // < ������Id, ��������Դ>
//
//				int i = 0, priorCA = -1, priorMA = -1, priorCB = -1, priorMB = -1, priorId = -1, priorTotal = -1;
//				for (auto it = patchExistServerIdx[patch].begin(); it != patchExistServerIdx[patch].end(); ++it, ++i) {  // ����������Դ��С����������
//				//for (auto &s : PatchExistServerResource[patch]) {    // �����з���������ƥ�䣬�Ż�ʱ����Կ��ǰ�ÿ�η������ȥ��
//					// ���������һЩ����������Ϊ��Щ��������ѡ�ϵĸ��ʺ�С
//					//if (i == patchExistServerIdx[patch].size() - floor(1.0 / ALLOC_DISCARD_RATIO * patchExistServerIdx[patch].size()))
//					//	break;
//
//					if (unusedServer.find(it->second) != unusedServer.end()) {  // ��̨�������ǲ������䵽���ʵ�������ģ�������һ��ѭ��
//						continue;
//					}
//
//					auto& s = PatchExistServerResource[patch][it->second];
//					int taget_tatal = s.CpuCores_A + s.CpuCores_B + s.MemerySize_A + s.MemerySize_B;  //��ʼ���������ʹյ����������Դ��ֵ
//					int taget_CpuCores_A = s.CpuCores_A, taget_CpuCores_B = s.CpuCores_B, taget_MemerySize_A = s.MemerySize_A, taget_MemerySize_B = s.MemerySize_B;
//
//					if (i == 0) {
//						priorCA = s.CpuCores_A, priorMA = s.MemerySize_A, priorCB = s.CpuCores_B, priorMB = s.MemerySize_B;
//						priorId = it->second, priorTotal = taget_tatal;
//					}
//					else if (priorCA == s.CpuCores_A && priorMA == s.MemerySize_A && priorCB == s.CpuCores_B && priorMB == s.MemerySize_B) {
//						vmsOnserverTemp[it->second] = vmsOnserverTemp[priorId];
//						Matching_degree.insert(make_pair(priorTotal, it->second));
//						ServerResourceTemp[it->second] = ServerResourceTemp[priorId];
//						continue;  // ��һ̨������ʣ����Դ����һ̨һģһ��������ʵ������ȥ�������е������ˣ�������һ̨����������Ϣ��ȥ����
//					}
//					else {
//						priorCA = s.CpuCores_A, priorMA = s.MemerySize_A, priorCB = s.CpuCores_B, priorMB = s.MemerySize_B;
//						priorId = it->second, priorTotal = taget_tatal;
//					}
//
//					list<vector<string>> vmList;
//					list<vector<string>>::iterator itStart, itEnd;
//					if (s.CpuCores_A + s.CpuCores_B + s.MemerySize_A + s.MemerySize_B < 0) {
//						// �½�һ��������б���һ̨������ֻ�ܷ�����Щ�����
//						constructVmList(divideResource, vmList, s.CpuCores_A, s.MemerySize_A, s.CpuCores_B, s.MemerySize_B);
//						itStart = vmList.begin();
//						itEnd = vmList.end();
//					}
//					else {
//						itStart = PatchRequestinfosLeft[buffNo][patch].begin();
//						itEnd = PatchRequestinfosLeft[buffNo][patch].end();
//					}
//
//					//for (list<vector<string>>::iterator vmIt = vmList.begin(); vmIt != vmList.end(); ++vmIt) {
//					//for (list<vector<string>>::iterator vm = PatchRequestinfosLeft[buffNo][patch].begin(); vm != PatchRequestinfosLeft[buffNo][patch].end(); ++vm) { // ������������
//					for (list<vector<string>>::iterator vmIt = itStart, nextIt = vmIt; vmIt != itEnd; vmIt = nextIt) {
//						++nextIt;
//						auto vm = addRequestIterMap[(*vmIt)[2]];
//
//						int vmCores = VmInfos[(*vm)[1]].CpuCores;
//						int vmMemory = VmInfos[(*vm)[1]].MemerySize;
//						int vmTwoNodes = VmInfos[(*vm)[1]].VmTwoNodes;
//
//						// �����̨�����������Ҫ������з�������������������Ǿ���ɾ�������棬����ٺϲ�����
//						if (vmCores + vmMemory > prev(patchExistServerIdx[patch].end(), 1)->first) { // �ܽ����Ŀ϶������������б�
//							discardRequest.push_back(*vm);
//							PatchRequestinfosLeft[buffNo][patch].erase(vm);
//							continue;
//						}
//
//						vector<int> vminfoAdd;
//
//						if (vmTwoNodes) {    // ˫�ڵ�vm
//
//							int needCores = vmCores / 2;
//							int needMemory = vmMemory / 2;
//							vminfoAdd.resize(4);
//
//							if (taget_CpuCores_A >= needCores && taget_CpuCores_B >= needCores && taget_MemerySize_A >= needMemory && taget_MemerySize_B >= needMemory) {
//								taget_CpuCores_A -= needCores;
//								taget_CpuCores_B -= needCores;
//								taget_MemerySize_A -= needMemory;
//								taget_MemerySize_B -= needMemory;
//
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ
//
//								priorTotal = taget_tatal;
//
//								vminfoAdd[0] = vmCores;
//								vminfoAdd[1] = vmMemory;
//								vminfoAdd[2] = 1;
//								vminfoAdd[3] = 2;
//								//vmsOnserverTemp[s.first].insert(make_pair((*vm)[2], vminfoAdd));
//								vmsOnserverTemp[it->second].insert(make_pair((*vm)[2], make_pair(vminfoAdd, vm)));
//
//								vminfoAdd.clear();
//							}
//						}
//						else {  // ���ڵ�
//							vminfoAdd.resize(3);
//							if (taget_CpuCores_A >= vmCores && taget_MemerySize_A >= vmMemory) {
//
//								taget_CpuCores_A -= vmCores;
//								taget_MemerySize_A -= vmMemory;
//
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ
//
//								priorTotal = taget_tatal;
//
//								vminfoAdd[0] = vmCores;
//								vminfoAdd[1] = vmMemory;
//								vminfoAdd[2] = 1;
//
//								//vmsOnserverTemp[s.first].insert(make_pair((*vm)[2], vminfoAdd));
//								vmsOnserverTemp[it->second].insert(make_pair((*vm)[2], make_pair(vminfoAdd, vm)));
//
//								vminfoAdd.clear();
//							}
//							else if (taget_CpuCores_B >= vmCores && taget_MemerySize_B >= vmMemory) {
//
//								taget_CpuCores_B -= vmCores;
//								taget_MemerySize_B -= vmMemory;
//
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ
//
//								priorTotal = taget_tatal;
//
//								vminfoAdd[0] = vmCores;
//								vminfoAdd[1] = vmMemory;
//								vminfoAdd[2] = 2;
//
//								//vmsOnserverTemp[s.first].insert(make_pair((*vm)[2], vminfoAdd));
//								vmsOnserverTemp[it->second].insert(make_pair((*vm)[2], make_pair(vminfoAdd, vm)));
//
//								vminfoAdd.clear();
//							}
//						}
//						priorTotal = taget_tatal;
//					} // һ̨��������������
//					Matching_degree.insert(make_pair(taget_tatal, it->second));   // ��¼�������������ƥ��Ĳ�ֵ
//
//					// ��¼��̨�������ϵ���Դʣ��������
//					ServerResourceTemp[it->second].CpuCores_A = taget_CpuCores_A;
//					ServerResourceTemp[it->second].CpuCores_B = taget_CpuCores_B;
//					ServerResourceTemp[it->second].MemerySize_A = taget_MemerySize_A;
//					ServerResourceTemp[it->second].MemerySize_B = taget_MemerySize_B;
//					//ServerResourceTemp[s.first].PowerCost = ServerInfos[s.first].PowerCost;
//
//				}  // ���з�����һ��ѭ������
//
//				//ѡ����ƥ������з�����
//
//				// ���ڲ�ֵ��С�ķ���������ȴ�����������Ҫ��������Ҫɾ��
//				for (multimap<int, int>::iterator it = Matching_degree.begin(); it != Matching_degree.end(); ) {
//					if (vmsOnserverTemp[(*it).second].size() == 0) {
//						// ��ô����������б�����̨��������ƥ�䲻�£����������Խ��Խ�پ͸���ûϷ�ˣ�����Щid��¼��������Ҳ���������
//						unusedServer.insert(make_pair(it->second, 1));
//
//						vmsOnserverTemp.erase((*it).second);
//						it = Matching_degree.erase(it);
//					}
//					else {
//						it++;
//					}
//				}
//				if (Matching_degree.size() == 0) {
//					break;
//				}
//				//else {
//				//	fstream f;
//				//	f.open("alloc_dataset1_version2.txt", ios::out | ios::app);
//				//	for (list<vector<string>>::iterator vm = PatchRequestinfosLeft[buffNo][patch].begin(); vm != PatchRequestinfosLeft[buffNo][patch].end(); ++vm) { // ������������
//				//		char test[100];
//				//		sprintf(test, "%s\t%d\t%d\t%d\n", (*vm)[2].c_str(), VmInfos[(*vm)[1]].CpuCores, VmInfos[(*vm)[1]].MemerySize, VmInfos[(*vm)[1]].VmTwoNodes);
//				//		f << test;
//				//	}
//				//	f << "----------------------------------------------------------------\n";
//				//	unordered_map<int, int> testMap;
//				//	int i = 0;
//				//	for (auto& id : patchExistServerIdx[patch]) {
//				//		if (vmsOnserverTemp.find(id.second) != vmsOnserverTemp.end()) {
//				//			testMap[id.second] = i;
//				//			auto& t = vmsOnserverTemp[id.second];
//				//			char test[100];
//				//			sprintf(test, "sid: %d\t%d\t%d\t%d\t%d\n", id.second, existServerResource[id.second].CpuCores_A, existServerResource[id.second].MemerySize_A, existServerResource[id.second].CpuCores_B, existServerResource[id.second].MemerySize_B);
//				//			f << test;
//				//			for (auto& h : t) {
//				//				sprintf(test, "%s\t%d\t%d\n", h.first.c_str(), h.second.first[0], h.second.first[1]);
//				//				int xixi = stoi(h.first.c_str());
//				//				f << test;
//				//			}
//				//			++i;
//				//		}
//				//	}
//				//	f << "----------------------------------------------------------------\n";
//
//				//	int sid = Matching_degree.begin()->second;
//				//	for (auto& t : vmsOnserverTemp[sid]) {
//				//		char test[100];
//				//		sprintf(test, "%d\t%s\t%d\t%d\t%d/%d\n", sid, t.first.c_str(), t.second.first[0], t.second.first[1], testMap[sid], patchExistServerIdx[patch].size());
//				//		f << test;
//				//	}
//				//	f << "----------------------------------------------------------------\n";
//				//	f.close();
//				//}
//
//				auto first = Matching_degree.begin();  // �˴�����ڲ�ֵ��ͬ����������Կ������ѡ����
//
//				ServerIdOnVms_alloc.insert(make_pair((*first).second, vmsOnserverTemp[(*first).second]));
//
//				//PatchExistServerResource[patch][(*first).second] = ServerResourceTemp[(*first).second];
//				PatchExistServerResource[patch].erase((*first).second);
//				// ������
//				for (auto it = patchExistServerIdx[patch].begin(); it != patchExistServerIdx[patch].end(); ++it) {
//					if (it->second == (*first).second) {
//						patchExistServerIdx[patch].erase(it);
//						break;
//					}
//				}
//
//				existServerResource[(*first).second].CpuCores_A = ServerResourceTemp[(*first).second].CpuCores_A;
//				existServerResource[(*first).second].CpuCores_B = ServerResourceTemp[(*first).second].CpuCores_B;
//				existServerResource[(*first).second].MemerySize_A = ServerResourceTemp[(*first).second].MemerySize_A;
//				existServerResource[(*first).second].MemerySize_B = ServerResourceTemp[(*first).second].MemerySize_B;
//
//				// ��PatchRequestinfosȥ������õ������
//				auto s = vmsOnserverTemp[(*first).second];
//
//				for (auto ss : s) {
//					//int pos = 0;
//					//vector<string>::iterator deleVminfotmp;
//					//list<vector<string>>::iterator deleVminfo;
//					//for (auto it = PatchRequestinfosLeft[buffNo][patch].begin(); it != PatchRequestinfosLeft[buffNo][patch].end(); ++it) {
//					//	deleVminfotmp = find(it->begin(), it->end(), ss.first);
//					//	if (deleVminfotmp != it->end()) {
//					//		deleVminfo = it;
//					//		break;
//					//	}
//					//}
//					PatchRequestinfosLeft[buffNo][patch].erase(ss.second.second);
//					if (ss.second.first.size() == 3) {
//						divideResource[ss.second.first[0]][ss.second.first[1]].erase(divideRequestIterMap[ss.first]);
//					}
//					else {
//						divideResource[ss.second.first[0] / 2][ss.second.first[1] / 2].erase(divideRequestIterMap[ss.first]);
//					}
//				}
//				Matching_degree.clear();
//				vmsOnserverTemp.clear();
//
//			}  // ����������
//			//// ��ɾ����������ӻ������ӵ�ͷ������
//			PatchRequestinfosLeft[buffNo][patch].insert(PatchRequestinfosLeft[buffNo][patch].begin(), discardRequest.begin(), discardRequest.end());
//		} // �ں˱�ѭ������
//	}  // �����з�ѭ������
//
//
//	//clock_t e = clock();
//	//fstream f;
//	//f.open("singlethread.txt", ios::out | ios::app);
//	//f << e - s << endl;
//	//f.close();
//
//	// ��¼����õ������
//	for (auto sov : ServerIdOnVms_alloc) {
//		for (auto ss : sov.second) {
//			if (ss.second.first.size() > 3) {   // ˫�ڵ�
//
//				VmIdOnServer[ss.first].push_back(sov.first);
//				VmIdOnServer[ss.first].push_back(ss.second.first[0]);
//				VmIdOnServer[ss.first].push_back(ss.second.first[1]);
//				VmIdOnServer[ss.first].push_back(ss.second.first[2]);
//				VmIdOnServer[ss.first].push_back(ss.second.first[3]);
//
//				ServerRunVmsNum[sov.first]++;  // ��¼��ID�������������˶���̨vm	
//			}
//			else {      // ���ڵ�
//				VmIdOnServer[ss.first].push_back(sov.first);
//				VmIdOnServer[ss.first].push_back(ss.second.first[0]);
//				VmIdOnServer[ss.first].push_back(ss.second.first[1]);
//				VmIdOnServer[ss.first].push_back(ss.second.first[2]);
//
//				ServerRunVmsNum[sov.first]++;  // ��¼��ID�������������˶���̨vm
//			}
//			// ��¼����������������Щ�����
//			ServerIdOnVms[sov.first].insert(make_pair(ss.first, ss.second.first));
//		}
//	}
//	ServerIdOnVms_alloc.clear();
//}
//
//// ----------------------------------------------------------------Ԥ����add����------------------------------------------- //
////int DealAddVm(vector<string> createVmInfo, vector<string>& res_tmp) {
////	string _reqVmType = createVmInfo[1], _reqId = createVmInfo[2];
////	auto  vm = VmInfos[_reqVmType];
////	// �Ǿ���һ�¾�׼������������ҳ���ֵ��С�ķ�����
////	int minServerId = -1, minVal = INT32_MAX, node = -1;
////	int vmCores = vm.CpuCores, vmMemo = vm.MemerySize, vmNode = vm.VmTwoNodes;
////	if (vmNode == 0) {  // ���ڵ㲿��
////		for (int i = 0; i < serverId; ++i) {
////			// �ҳ�����̨���������ĸ��ڵ��ֵ��С
////			int diff1 = existServerResource[i].CpuCores_A - vmCores, diff2 = existServerResource[i].MemerySize_A - vmMemo;
////			if (diff1 >= 0 && diff2 >= 0 && diff1 + diff2 < minVal) {
////				minServerId = i;
////				minVal = diff1 + diff2;
////				node = 1;
////			}
////			diff1 = existServerResource[i].CpuCores_B - vmCores, diff2 = existServerResource[i].MemerySize_B - vmMemo;
////			if (diff1 >= 0 && diff2 >= 0 && diff1 + diff2 < minVal) {
////				minServerId = i;
////				minVal = diff1 + diff2;
////				node = 2;
////			}
////		}
////	}
////	else {  // ˫�ڵ㲿��
////		for (int i = 0; i < serverId; ++i) {
////			// �ҳ�����̨�������Ĳ�ֵ��С
////			int diff1A = existServerResource[i].CpuCores_A - vmCores / 2;
////			int diff2A = existServerResource[i].MemerySize_A - vmMemo / 2;
////			int diff1B = existServerResource[i].CpuCores_B - vmCores / 2;
////			int diff2B = existServerResource[i].MemerySize_B - vmMemo / 2;
////			if (diff1A >= 0 && diff2A >= 0 && diff1B >= 0 && diff2B >= 0 && diff1A + diff2A + diff1B + diff2B < minVal) {
////				minServerId = i;
////				minVal = diff1A + diff2A + diff1B + diff2B;
////				node = 0;
////			}
////		}
////	}
////	if (minServerId == -1)
////		return -1;
////	// �����ȥ��Ҫ�ĳɷ�AB�ڵ����
////	int success = -1;
////	auto& server = existServerResource[minServerId];
////	if (AllocServerResources(server, vm, minServerId, _reqId, res_tmp, node)) {
////		ServerRunVmsNum[minServerId]++;
////		success = 1;
////	}
////	assert(server.CpuCores_A >= 0 && server.CpuCores_B >= 0 && server.MemerySize_A >= 0 && server.MemerySize_B >= 0);
////	return success;
////
////}
//
//// ----------------------------------����ÿһ��add����----------------------------------------------------------------------//
//int ServerIdAdd = 0; // ÿ�����ӵķ���������
//int ReDealAddVM(vector<string> request, vector<string>& res_tmp, int day);
//int ReDealAddVM(vector<string> request, vector<string>& res_tmp, int day) {
//	string s = "(";
//	if (VmIdOnServer.find(request[2]) != VmIdOnServer.end()) {
//		vector<int> serverInfo = VmIdOnServer[request[2]];
//		if (serverInfo.size() == 5) {  // ˫�ڵ㲿��
//			s += to_string(serverInfo[0]) + ")\n";
//		}
//		else {  // ���ڵ㲿��
//			if (serverInfo[3] == 1)
//				s += to_string(serverInfo[0]) + ", A)\n";		  // ���䵽A�ڵ�			
//			else
//				s += to_string(serverInfo[0]) + ", B)\n";          // ���䵽B�ڵ�	
//		}
//		res_tmp.push_back(s);
//		return 1;
//	}
//	return -1;
//}
//
//// ---------------------------------------------------------------Ǩ������� --------------------------------------------------------------------//
//int MigrationNum;
//
//void Migrate(int day, list<vector<string>>& requestinfos) {
//	// ͳ������������add��dele�ı�ֵ
//	float delete_ratio_request = 0.0;
//	int  DeleteNum = 0;
//	for (auto re : requestinfos) {
//		if (re[0] == "del") {
//			DeleteNum++;
//		}
//	}
//	delete_ratio_request = (float)DeleteNum / VmIdOnServer.size();
//	//day% MIGRATION_DAY == 0   delete_ratio_request > DELETE_RATIO_TO_REQEST
//	//if (day % MIGRATION_DAY == 0 && delete_ratio_request > DELETE_RATIO_TO_REQEST) {
//	if (delete_ratio_request > DELETE_RATIO_TO_REQEST) {
//		//fstream f;
//		//f.open("data_analyze4_dataset1.txt", ios::out | ios::app);
//		multimap<int, int, greater<int>> ServerRunVmsNumTmp; // vm������������ID
//		int vmSum = 0; // ���������
//		int moveNum = 0; // Ǩ�����������
//		vector<string> res_tmp; // Ǩ�ƹ��̽���ݴ�
//		for (auto& s : existServerResource) {
//			int totalRemain = s.second.CpuCores_A + s.second.CpuCores_B + s.second.MemerySize_A + s.second.MemerySize_B;
//			ServerRunVmsNumTmp.insert(make_pair(totalRemain, s.first));
//			vmSum += ServerRunVmsNum[s.first];
//		}
//		int i_head = 0; // ����headͬ����
//		for (auto head = ServerRunVmsNumTmp.begin(); head != ServerRunVmsNumTmp.end(); ++head, ++i_head) {
//
//			auto& SourceServer = existServerResource[(*head).second];
//			int& SourceCoreA = SourceServer.CpuCores_A,
//				& SourceCoreB = SourceServer.CpuCores_B,
//				& SourceMemoryA = SourceServer.MemerySize_A,
//				& SourceMemoryB = SourceServer.MemerySize_B;
//
//			int maxAttempTimes = ServerRunVmsNumTmp.size() - i_head - 1, i_tail = ServerRunVmsNumTmp.size() - 1;  // ÿ̨�����������Դ���
//			for (auto sov = ServerIdOnVms[head->second].begin(); sov != ServerIdOnVms[head->second].end() && moveNum < vmSum / MIGRATION_LIMT;) {
//
//				int vmCpu = sov->second[0], vmMemo = sov->second[1];
//				if (sov->second.size() > 3) {   // ˫�ڵ㲿��
//
//					int minTailServerId = head->second,
//						minTailServerVal = existServerResource[head->second].CpuCores_A + existServerResource[head->second].CpuCores_B
//						+ existServerResource[head->second].MemerySize_A + existServerResource[head->second].MemerySize_B;
//
//					int noFindTimes = 0, findTimes = 0, totalTimes = 0;
//					//for (auto tail = prev(ServerRunVmsNumTmp.end(), 1); tail != head; --tail) {  // �ҳ���ֵ��С�ķ�����ID
//					for (auto tail = next(head, 1); tail != ServerRunVmsNumTmp.end(); ++tail) {  // �ҳ���ֵ��С�ķ�����ID
//
//						int tailCpuA = existServerResource[tail->second].CpuCores_A, tailCpuB = existServerResource[tail->second].CpuCores_B,
//							tailMemoA = existServerResource[tail->second].MemerySize_A, tailMemoB = existServerResource[tail->second].MemerySize_B;
//
//						int diffCpuA = tailCpuA - vmCpu / 2, diffCpuB = tailCpuB - vmCpu / 2, diffMemoA = tailMemoA - vmMemo / 2, diffMemoB = tailMemoB - vmMemo / 2;
//						int remainVal = diffCpuA + diffCpuB + diffMemoA + diffMemoB;
//						if (diffCpuA >= 0 && diffCpuB >= 0 && diffMemoA >= 0 && diffMemoB >= 0 && remainVal <= minTailServerVal) {
//							minTailServerId = tail->second;
//							minTailServerVal = remainVal;
//						}
//						++totalTimes;
//						//if (minTailServerId != -1) {
//							//char test[100];
//							//sprintf(test, "%s\t%d -> %d\tcurrentMin: %d\n", sov->first.c_str(), head->second, tail->second, minTailServerId);
//							//f << test;
//						//}
//						if (minTailServerId == head->second) {  // -1��ʾ��û�ҵ����ʵķ���������ô���п�����Ҳ�Ҳ�����
//							++noFindTimes;
//							if ((float)noFindTimes / maxAttempTimes > MIGRATE_FAIL_RATIO) {  // -1����һ���Ĵ�����Ϳ��������ˣ����ֵ������Сһ��
//								break;
//							}
//						}
//						else {  // �Ѿ��ҵ��������ˣ�ֻ�ǿ��ܻ�û�ҵ�����ʵķ�����
//							if ((float)totalTimes / maxAttempTimes > MIGRATE_SUCCESS_RATIO) {
//								break;
//							}
//						}
//
//					}
//
//					if (minTailServerId == head->second) {  // ��˵��ֵ��С�ķ������ˣ����ŵ��µķ�������û�У���������һ̨
//						++sov;
//						continue;
//					}
//					else {  // �п��Էŵ��µķ�����
//
//						//char test[100];
//						//float attempTimes = (float)distance(head, ServerRunVmsNumTmp.end()) - 1;
//						//sprintf(test, "%s\t%d -> %d\t%d / %f\t %f\n", sov->first.c_str(), head->second, minTailServerId, findTimes, attempTimes, (float)findTimes / attempTimes * 100);
//						//f << test;
//
//						auto& Destinationserver = existServerResource[minTailServerId];
//						int& DestinationCoreA = Destinationserver.CpuCores_A,
//							& DestinationCoreB = Destinationserver.CpuCores_B,
//							& DestinationMemoryA = Destinationserver.MemerySize_A,
//							& DestinationMemoryB = Destinationserver.MemerySize_B;
//
//
//						// Ŀ�ķ�����ʣ����Դ����
//						DestinationCoreA -= vmCpu / 2;
//						DestinationCoreB -= vmCpu / 2;
//						DestinationMemoryA -= vmMemo / 2;
//						DestinationMemoryB -= vmMemo / 2;
//
//						SourceCoreA += vmCpu / 2;
//						SourceMemoryA += vmMemo / 2;
//						SourceCoreB += vmCpu / 2;
//						SourceMemoryB += vmMemo / 2;
//
//						// ����������������ĸ��������ϣ�˫�ڵ㲿��
//						VmIdOnServer[(*sov).first] = vector<int>{ minTailServerId ,vmCpu,vmMemo,1,2 };
//
//						res_tmp.push_back("(" + (*sov).first + ", " + to_string(minTailServerId) + ")\n");			//���
//
//						moveNum++;
//						ServerIdOnVms[minTailServerId].insert(*sov);           // ���������������λ��
//						sov = ServerIdOnVms[(*head).second].erase(sov);       // �����������ԭ��λ��ɾ��
//
//						ServerRunVmsNum[minTailServerId]++;                   //  Ŀ�ķ�����ID�ϵ������������1
//						ServerRunVmsNum[(*head).second]--;                   //  ԭ������ID�ϵ������������1
//
//						assert(ServerRunVmsNum[(*head).second] >= 0);
//					}
//
//				}
//				else {  // ���ڵ㲿��
//
//					pair<int, int> minTailServerId(head->second, 0);  // <id ,A/B�ڵ�>
//					int minTailServerVal = (sov->second[2] == 1 ? existServerResource[head->second].CpuCores_A + existServerResource[head->second].MemerySize_A :
//						existServerResource[head->second].CpuCores_B + existServerResource[head->second].MemerySize_B);
//					int noFindTimes = 0, totalTimes = 0;
//					//for (auto tail = prev(ServerRunVmsNumTmp.end(), 1); tail != head; --tail) {  // �ҳ���ֵ��С�ķ�����ID
//					for (auto tail = next(head, 1); tail != ServerRunVmsNumTmp.end(); ++tail) {  // �ҳ���ֵ��С�ķ�����ID
//
//						int tailCpuA = existServerResource[tail->second].CpuCores_A, tailCpuB = existServerResource[tail->second].CpuCores_B,
//							tailMemoA = existServerResource[tail->second].MemerySize_A, tailMemoB = existServerResource[tail->second].MemerySize_B;
//
//						int diffCpuA = tailCpuA - vmCpu, diffCpuB = tailCpuB - vmCpu,
//							diffMemoA = tailMemoA - vmMemo, diffMemoB = tailMemoB - vmMemo;
//
//
//						int remainValA = diffCpuA + diffMemoA;
//						int remainValB = diffCpuB + diffMemoB;
//						if (diffCpuA >= 0 && diffMemoA >= 0 && remainValA <= minTailServerVal) {
//							minTailServerId.first = tail->second;
//							minTailServerId.second = 1;
//							minTailServerVal = remainValA;
//						}
//						if (diffCpuB >= 0 && diffMemoB >= 0 && remainValB <= minTailServerVal) {
//							minTailServerId.first = tail->second;
//							minTailServerId.second = 2;
//							minTailServerVal = remainValB;
//						}
//						++totalTimes;
//						//if (minTailServerId.first != -1) {
//							/*char test[100];
//							sprintf(test, "%s\t%d -> %d\tcurrentMin: %d\n", sov->first.c_str(), head->second, tail->second, minTailServerId);
//							f << test;*/
//							/*++noFindTimes;*/
//						//}
//						if (minTailServerId.first == head->second) {  // -1��ʾ��û�ҵ����ʵķ���������ô���п�����Ҳ�Ҳ�����
//							++noFindTimes;
//							if ((float)noFindTimes / maxAttempTimes > MIGRATE_FAIL_RATIO) {   // -1����һ���Ĵ�����Ϳ��������ˣ����ֵ������Сһ��
//								break;
//							}
//						}
//						else {  // �Ѿ��ҵ��������ˣ�ֻ�ǿ��ܻ�û�ҵ�����ʵķ�����
//							if ((float)totalTimes / maxAttempTimes > MIGRATE_SUCCESS_RATIO) {
//								break;
//							}
//						}
//					}
//
//					if (minTailServerId.first == head->second) {  // ��˵��ֵ��С�ķ������ˣ����ŵ��µķ�������û�У���������һ̨
//						++sov;
//						continue;
//					}
//					else {   // �ŵ���
//
//						//char test[100];
//						//float attempTimes = (float)distance(head, ServerRunVmsNumTmp.end()) - 1;
//						//sprintf(test, "%s\t%d -> %d\t%d / %f\t %f\n", sov->first.c_str(), head->second, minTailServerId.first, findTimes, attempTimes, (float)findTimes / attempTimes * 100);
//						//f << test;
//
//						auto& Destinationserver = existServerResource[minTailServerId.first];
//						int& DestinationCoreA = Destinationserver.CpuCores_A,
//							& DestinationCoreB = Destinationserver.CpuCores_B,
//							& DestinationMemoryA = Destinationserver.MemerySize_A,
//							& DestinationMemoryB = Destinationserver.MemerySize_B;
//
//						// Ŀ�ķ�����ʣ����Դ����
//						if (minTailServerId.second == 1) {
//							DestinationCoreA -= vmCpu;
//							DestinationMemoryA -= vmMemo;
//						}
//						else if (minTailServerId.second == 2) {
//							DestinationCoreB -= vmCpu;
//							DestinationMemoryB -= vmMemo;
//						}
//
//						//Դ������ʣ����Դ����
//						if ((*sov).second[2] == 1) {
//							SourceCoreA += vmCpu;
//							SourceMemoryA += vmMemo;
//						}
//						else if ((*sov).second[2] == 2) {
//							SourceCoreB += vmCpu;
//							SourceMemoryB += vmMemo;
//						}
//
//
//						// ����������������ĸ���������
//						if (minTailServerId.second == 1) {
//							VmIdOnServer[(*sov).first] = vector<int>{ minTailServerId.first,vmCpu,vmMemo,1 };
//							res_tmp.push_back("(" + (*sov).first + ", " + to_string(minTailServerId.first) + ", A)\n");			//���
//							(*sov).second[2] = 1;
//						}
//						else {
//							VmIdOnServer[(*sov).first] = vector<int>{ minTailServerId.first,vmCpu,vmMemo,2 };
//							res_tmp.push_back("(" + (*sov).first + ", " + to_string(minTailServerId.first) + ", B)\n");			//���
//							(*sov).second[2] = 2;
//						}
//
//						moveNum++;
//
//						ServerIdOnVms[minTailServerId.first].insert(*sov);           // ���������������λ��
//						sov = ServerIdOnVms[(*head).second].erase(sov);       // �����������ԭ��λ��ɾ��
//						ServerRunVmsNum[minTailServerId.first]++;                   //  Ŀ�ķ�����ID�ϵ������������1
//						ServerRunVmsNum[(*head).second]--;                   //  ԭ������ID�ϵ������������1
//
//						assert(ServerRunVmsNum[(*head).second] >= 0);
//					}
//
//				}
//			}
//			if (moveNum != 0 && moveNum >= vmSum / MIGRATION_LIMT) {
//				break;
//			}
//		}
//		MigrationNum += moveNum;
//		string s = "(migration, " + to_string(moveNum) + ")\n";
//		migrantList.push_back(s);
//		for (auto s : res_tmp) {
//			migrantList.push_back(s);
//		}
//		//f.close();
//	}
//	else {
//		string s = "(migration, " + to_string(0) + ")\n";
//		migrantList.push_back(s);
//	}
//}
//
//// ---------------------------------------------------------------����ƥ�� --------------------------------------------------------------------- //
//void Schedule(int day, unordered_map<string, ServerInfo> serverinfos, list<vector<string>> requestinfos) {
//
//	vector<string> res_tmp;
//
//	if (day <= DAY)  // ��һ������ȫ�򣬺���Ҳ���Ե���day�Ĵ�С
//	{
//		GetServerInfosPatch(serverinfos, VmInfos, requestinfos);
//		Expansion(day, PatchServerinfos, PatchRequestinfos);
//#ifdef TEST 
//		printf("day %d There are %d requests!!\n", day, requestinfos.size());
//#endif
//		for (list<vector<string>>::iterator req = requestinfos.begin(); req != requestinfos.end(); req++) {
//			int opType = (*req).size() == 3 ? 1 : 0;
//			if (opType) {
//				int resourceEnough = ReDealAddVM(*req, res_tmp, day);
//			}
//			else {
//				DealDeleteVM(*req);
//			}
//
//		}
//		for (auto re : res_tmp) {
//			res[day].push_back(re);
//#ifdef TEST
//			cout << re;
//#endif
//		}
//
//		res_tmp.clear();
//
//		ServerIdAdd += ServerIdOnVms_type.size();
//		ServerIdOnVms_type.clear();
//	}
//	else {
//#ifdef TEST 
//		printf("day %d There are %d requests!!\n", day, requestinfos.size());
//#endif
//		Migrate(day, LastRequestInfos);
//		DealAdd(requestinfos, existServerResource, VmInfos, res_tmp);
//		Expansion(day, PatchServerinfos, PatchRequestinfosLeft);
//
//
//		for (list<vector<string>>::iterator req = requestinfos.begin(); req != requestinfos.end(); req++) {
//			int opType = (*req).size() == 3 ? 1 : 0;
//			if (opType) {  // add
//				//int vmid = stoi((*req)[2]);
//				int resourceEnough = ReDealAddVM(*req, res_tmp, day);
//				assert(resourceEnough != -1);
//			}
//			else {
//				DealDeleteVM(*req);
//			}
//		}
//
//		// �ϲ�������add����������
//		for (auto re : res_tmp) {
//			res[day].push_back(re);
//#ifdef TEST
//			cout << re;
//#endif
//		}
//
//		res_tmp.clear();
//		ServerIdAdd += ServerIdOnVms_type.size();
//		ServerIdOnVms_type.clear();
//		PatchRequestinfosLeft.clear();
//	}
//}
//
//// ��������Ѽ���
//void ServerPowerCost() {
//	for (int i = 0; i < serverId; i++) {
//		if (ServerRunVmsNum[i] != 0) {
//			POWER_COST += existServerResource[i].PowerCost;
//		}
//	}
//}
//
//
//int main() {
//	clock_t start_time, finish_time;
//	start_time = clock();
//
//#ifdef TEST
//	freopen(filePath.c_str(), "rb", stdin);
//#endif
//
//	int ServerTypesNum;    // �ṩ�ķ�������������
//	string serverType = "", cpuCores = "", memorySize = "", serverCost = "", powerCost = "";
//	scanf("%d", &ServerTypesNum);
//
//	for (int i = 0; i < ServerTypesNum; i++) {
//		cin >> serverType >> cpuCores >> memorySize >> serverCost >> powerCost;  // ����һ�����͵ķ�������Ϣ
//		GetServerInfos(serverType, cpuCores, memorySize, serverCost, powerCost); // ������������Ϣ��������
//	}
//
//	int VmTypesNum = 0;    // �ṩ���������������
//	scanf("%d", &VmTypesNum);
//
//	string vmType = "", vmCpuCores = "", vmMemory = "", vmTwoNodes = "";
//	for (int i = 0; i < VmTypesNum; i++) {
//		cin >> vmType >> vmCpuCores >> vmMemory >> vmTwoNodes;
//		GetVmInfos(vmType, vmCpuCores, vmMemory, vmTwoNodes);
//	}
//
//	int RequestDays = 0, DayRequestNumbers = 0;
//	int outputLimitDay = 0;
//	scanf("%d %d", &RequestDays, &outputLimitDay);
//	string buff, reqestVmType, reqestId;
//
//	for (int day = 0; day < RequestDays; day++) {
//		scanf("%d", &DayRequestNumbers);
//		if (day != 0) {
//
//			LastRequestInfos.clear();
//			for (auto& s : RequestInfos) {
//				LastRequestInfos.push_back(s);
//			}
//			RequestInfos.clear();
//		}
//		else
//		{
//			LastRequestInfos.clear();
//			RequestInfos.clear();
//		}
//		//RequestInfos.clear();
//		for (int i = 0; i < DayRequestNumbers; i++) {
//			cin >> buff;
//			if (buff[1] == 'a') {
//				cin >> reqestVmType >> reqestId;
//				GetRequest(buff, reqestVmType, reqestId);    //����һ�����������������
//			}
//			else {
//				cin >> reqestId;
//				GetRequest(buff, reqestId);              //����һ����ɾ�����������
//			}
//		}
//		if (DayRequestNumbers > 1000) {
//			COST_PERFORMANCE = 0.65;
//		}
//		else COST_PERFORMANCE = 0.8;
//		GetBestServer(day, ServerInfos, RequestDays);
//		Schedule(day, ServerInfos, RequestInfos);
//		ServerPowerCost();
//
//#ifdef UPLOAD
//		if (day + 1 >= outputLimitDay) {
//			int outputDay = day - outputLimitDay + 1;
//			for (auto& s : res[outputDay]) {
//				cout << s;
//			}
//			fflush(stdout);
//		}
//#endif
//
//	}
//
//#ifdef UPLOAD
//	// �����û�������Ϣ
//	int outputDay = RequestDays - outputLimitDay + 1;
//	for (int i = outputDay; i < RequestDays; i++) {
//		for (auto& s : res[i]) {
//			cout << s;
//		}
//		fflush(stdout);
//	}
//
//#endif
//
//	fclose(stdin);
//	finish_time = clock();
//
//	TOTAL_COST = SERVER_COST + POWER_COST;  //�����ܷ���
//
////#ifdef UPLOAD
////	for (auto& s : res) std::cout << s;
////#endif
//
//#ifdef TEST
//	ofstream out("output.txt");
//	for (int i = 0; i < RequestDays; ++i) {
//		for (auto& s : res[i]) {
//			if (out.is_open()) {
//				out << s;
//			}
//			//std::cout << s;
//		}
//	}
//	if (testExpansion.is_open()) {
//		testExpansion << "totalCPU: " + to_string(CPU) + "totalMem: " + to_string(Memory) + "\n";
//	}
//	testExpansion.close();
//	out.close();
//	printf("\nused time: %f s \n", double(finish_time - start_time) / CLOCKS_PER_SEC);
//	printf("server cost: %lld \npower cost: %lld \ntotal cost: %lld \n", SERVER_COST, POWER_COST, TOTAL_COST);
//	printf("Migrate Num: %d  \n", MigrationNum);
//#endif
//
//	return 0;
//}
