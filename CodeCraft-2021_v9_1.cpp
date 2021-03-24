#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <ctime>
#include <algorithm>
#include <cassert>
#include<fstream>
#include<set>
#include <omp.h>   // openMP

#include<thread>
#include<mutex>
using namespace std;

#define TEST

//#define UPLOAD

// �ṹ��洢������Ϣ

// ������������Ϣ�ṹ��
struct ServerInfo {

	int CpuCores_A;    // A�ڵ�cpu��Դ��
	int CpuCores_B;    // B�ڵ�cpu��Դ��

	int MemerySize_A;  // A�ڵ��ڴ���Դ��
	int MemerySize_B;  // B�ڵ��ڴ���Դ��

	int ServerCost;    // ����������
	int PowerCost;     // ���

	float Cpu_Memory_ratio;  // �ں˱�
	int ServerType;      // ������Id
};

// �����������Ϣ�ṹ��
struct VmInfo {

	int CpuCores;    // ���ĵ�cpu��Դ��

	int MemerySize;  // ���ĵ��ڴ���Դ��

	int VmTwoNodes;  // ��/˫�ڵ㲿��

	string VmType;     // �����Id

	float Cpu_Memory_ratio;
};


// ���з�������Ϣ
unordered_map<string, ServerInfo> ServerInfos;

// �����������Ϣ
unordered_map<string, VmInfo> VmInfos;

// һ������������Ϣ
vector<vector<string>> RequestInfos;

// ����ķ���������
int serverNumber = 0;

// ����ķ�������Ϣ��ϵͳ�ִ���Դ��
unordered_map<int, ServerInfo> sysServerResource;

// ��ǰ�����ķ����������е����������
map<int, int> serverRunVms;

// ��¼������������Ǹ���������
unordered_map<string, vector<int>> VmOnServer;   //[in] �����Id  [in] ��������ڵķ�����id,�����cpu,������ڴ棬��/˫�ڵ㲿��

// ��¼����������������Щ�����
unordered_map<int, unordered_map<string, vector<int>>> ServerOnVm;   //[in] ������Id  [in] �������id, �����cpu,�ڴ棬��/˫�ڵ㲿��

multimap<string, unordered_map<string, vector<int>>> DayServerOnVm; // ��¼һ��ı��������ID�ķ������ճ����ϵ��������Ϣ����˳��Ϊ������ID˳��

vector<string> migrantList{ "(migration, 0)\n" };
vector<string> res;

#ifdef TEST
const string filePath = "training-2.txt";
#endif

// �ɱ�
long long SERVERCOST = 0, POWERCOST = 0, TOTALCOST = 0;


// ������������Ϣ
void GetServerInfos(string& serverType, string& cpuCores, string& memorySize, string& serverCost, string& powerCost) {
	string _serverType = "";

	for (int i = 1; i < serverType.size() - 1; i++) {
		_serverType += serverType[i];
	}
	int _cpuCores = 0, _memorySize = 0, _serverCost = 0, _powerCost = 0;

	for (int i = 0; i < cpuCores.size() - 1; i++) {
		_cpuCores = 10 * _cpuCores + cpuCores[i] - '0';
	}
	for (int i = 0; i < memorySize.size() - 1; i++) {
		_memorySize = 10 * _memorySize + memorySize[i] - '0';
	}
	for (int i = 0; i < serverCost.size() - 1; i++) {
		_serverCost = 10 * _serverCost + serverCost[i] - '0';
	}
	for (int i = 0; i < powerCost.size() - 1; i++) {
		_powerCost = 10 * _powerCost + powerCost[i] - '0';
	}

	ServerInfos[_serverType].CpuCores_A = _cpuCores / 2;   // A�ڵ�cpu
	ServerInfos[_serverType].CpuCores_B = _cpuCores / 2;   // B�ڵ�cpu

	ServerInfos[_serverType].MemerySize_A = _memorySize / 2;  //A�ڵ��ڴ�
	ServerInfos[_serverType].MemerySize_B = _memorySize / 2;  //B�ڵ��ڴ�

	ServerInfos[_serverType].PowerCost = _powerCost;   //���
	ServerInfos[_serverType].ServerCost = _serverCost; //�������ɱ�
}


// �����������Ϣ
void GetVmInfos(string& vmType, string& vmCpuCores, string& vmMemory, string& vmTwoNodes) {
	string _vmType;

	for (int i = 1; i < vmType.size() - 1; i++) {
		_vmType += vmType[i];
	}

	int _vmCpuCores = 0, _vmMemory = 0, _vmTwoNodes = 0;
	for (int i = 0; i < vmCpuCores.size() - 1; i++) {
		_vmCpuCores = _vmCpuCores * 10 + vmCpuCores[i] - '0';
	}
	for (int i = 0; i < vmMemory.size() - 1; i++) {
		_vmMemory = _vmMemory * 10 + vmMemory[i] - '0';
	}
	if (vmTwoNodes[0] == '1') {
		_vmTwoNodes = 1;
	}
	else {
		_vmTwoNodes = 0;
	}
	VmInfos[_vmType].CpuCores = _vmCpuCores;
	VmInfos[_vmType].MemerySize = _vmMemory;
	VmInfos[_vmType].VmTwoNodes = _vmTwoNodes;
}

// �����û���������
void GetRequest(string& op, string& reqVmType, string& reqId) {

	string _op, _reqVmType, _reqId;
	_op = op.substr(1, op.size() - 1);                                 // add
	_reqVmType = reqVmType.substr(0, reqVmType.size() - 1);            // �������ӵ����������
	_reqId = reqId.substr(0, reqId.size() - 1);                        // �������ӵ������id
	RequestInfos.push_back(vector<string>{_op, _reqVmType, _reqId});   // �û�������������û�������Ϣ��
}

// �����û�ɾ������
void GetRequest(string& op, string& reqId) {
	string _op, _reqId;
	_reqId = reqId.substr(0, reqId.size() - 1);                        // ����ɾ���������id
	_op = op.substr(1, op.size() - 1);                                 // delete
	RequestInfos.push_back(vector<string>{_op, _reqId});               // �û�ɾ���������������û�������Ϣ��
}


// �ڷ������Ϸ����������Դ
bool AllocServerResources(ServerInfo& server, VmInfo& vm, int serverId, string vmId, vector<string>& res_tmp, int node) {

	int vmCores = vm.CpuCores,
		vmMemory = vm.MemerySize,
		vmTwoNodes = vm.VmTwoNodes;

	int& serverCoreA = server.CpuCores_A,
		& serverCoreB = server.CpuCores_B,
		& serverMemoryA = server.MemerySize_A,
		& serverMemoryB = server.MemerySize_B;

	if (vmTwoNodes) {   //˫�ڵ㲿��ʱ
		int needCores = vmCores / 2, needMemory = vmMemory / 2;  //�������Դ���԰����
		if (serverCoreA >= needCores && serverCoreB >= needCores && serverMemoryA >= needMemory && serverMemoryB >= needMemory) {   //������AB�ڵ���Դ������
			// ��Դ�ֳ�
			serverCoreA -= needCores;
			serverCoreB -= needCores;
			serverMemoryA -= needMemory;
			serverMemoryB -= needMemory;

			assert(serverCoreA >= 0 && serverCoreB >= 0 && serverMemoryA >= 0 && serverMemoryB >= 0);

			// ��¼������������ĸ��������ϣ�˫�ڵ㲿��
			VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1,2 };

			// ��¼����������������Щ�����(Ǩ��ʹ��)
			ServerOnVm[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 1, 2 }));

			res_tmp.push_back("(" + to_string(serverId) + ")\n");
			return true;
		}
		else {
			return false;                                               //��Դ���㣬�ڴ˷������Ϸ���������ʧ��
		}
	}
	else if (node == 1 && serverCoreA >= vmCores && serverMemoryA >= vmMemory) {     //���ڵ㲿����A�ڵ��Ƿ������������

		serverCoreA -= vmCores;
		serverMemoryA -= vmMemory;

		assert(serverCoreA >= 0 && serverMemoryA >= 0);

		// ��¼������������ĸ��������ϣ�A�ڵ㲿��
		VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1 };

		// ��¼����������������Щ�����
		ServerOnVm[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 1}));

		res_tmp.push_back("(" + to_string(serverId) + ", A)\n");
		return true;
	}
	else if (node == 2 && serverCoreB >= vmCores && serverMemoryB >= vmMemory) {    //���ڵ㲿����B�ڵ��Ƿ������������

		serverCoreB -= vmCores;
		serverMemoryB -= vmMemory;

		assert(serverCoreB >= 0 && serverMemoryB >= 0);

		// ��¼������������ĸ��������ϣ�B�ڵ㲿��
		VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,2 };

		// ��¼����������������Щ�����
		ServerOnVm[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 2}));

		res_tmp.push_back("(" + to_string(serverId) + ", B)\n");
		return true;
	}
	return false;                                                      //��˫�ڵ���Դ�����㣬�ڴ˷������Ϸ���������ʧ��                                         
}

// �ڷ��������ͷ��������Դ
void ReleaseServerResources(ServerInfo& server, vector<int>& vmInfo, int serverId, string vmId) {
	// [in]:���з�������Դ 
	// [in]:���е��������Ϣ[��������ڵķ�����id,�����cpu,������ڴ棬��/˫�ڵ㲿��]  
	// [in]:������Id 
	// [in]:�����Id	
	if (vmInfo.size() == 5) {  // ˫�ڵ㲿��������
		int cores = vmInfo[1] / 2, memory = vmInfo[2] / 2;

		server.CpuCores_A += cores;
		server.CpuCores_B += cores;
		server.MemerySize_A += memory;
		server.MemerySize_B += memory;
	}
	else { // ���ڵ㲿��
		int cores = vmInfo[1], memory = vmInfo[2];
		if (vmInfo[3] == 1) {  // A�ڵ㲿��
			server.CpuCores_A += cores;
			server.MemerySize_A += memory;
		}
		else {                 // B�ڵ㲿��
			server.CpuCores_B += cores;
			server.MemerySize_B += memory;
		}
	}
}


// ---------------------------------------------------  ����ɾ�����������------------------------------------------------------------------//
void DealDeleteVM(vector<string>& delVmInfo) {

	string _vmId = delVmInfo[1];

	vector<int> _vmInfo = VmOnServer[_vmId];

	int _serverId = _vmInfo[0];  //����������ķ�����id

	auto& server = sysServerResource[_serverId];

	ReleaseServerResources(server, _vmInfo, _serverId, _vmId);   //�ͷ������

	serverRunVms[_serverId]--;  //�������ϵ����������-1

	// ����VmOnServer
	auto it1 = VmOnServer.find(_vmId);
	VmOnServer.erase(it1);

	//����ÿ̨�������ϴ�ŵ������
	auto it2 = ServerOnVm[_serverId].find(_vmId);
	ServerOnVm[_serverId].erase(it2);

	assert(serverRunVms[_serverId] >= 0);

}

unordered_map<string, float> PerformanceAndCostRatio;  //��������

// --------------------------------------------------- ����������Լ۱� --------------------------------------------------------------------//
void GetBestServer(int day, unordered_map<string, ServerInfo> serverinfos, int requestdays) {


	float Cpu;
	float Memory;

	float ServerCost;
	float PowerCost;

	string serverType;

	for (auto s : serverinfos) {

		serverType = s.first;
		Cpu = s.second.CpuCores_A + s.second.CpuCores_B;
		Memory = s.second.MemerySize_A + s.second.MemerySize_B;

		ServerCost = s.second.ServerCost;
		PowerCost = s.second.PowerCost;


		PerformanceAndCostRatio.insert(make_pair(serverType, (ServerCost + (requestdays - day) * PowerCost) / (Cpu + Memory)));
	}

}

vector<unordered_map<string, ServerInfo>> PatchServerinfos;
vector<vector<vector<vector<string>>>>PatchRequestinfos;



int PatchSize = 5;
int QuestRange = 5000;
int buffer_num = 10;
// ---------------------------------------------------��������������������ں˱ȹ���-----------------------------------------------------------------------------------//
void GetServerInfosPatch(unordered_map<string, ServerInfo> serverinfos, unordered_map<string, VmInfo> vminfos, vector<vector<string>> requestinfos) {

	vector<vector<vector<string>>> requestinfos_buff;  // �зֵ�add����buffer

	int buffer_length = requestinfos.size() / buffer_num;

	if (requestinfos.size() < QuestRange) {   // �������С��QuestRange���Ͳ����з�
		requestinfos_buff.push_back(requestinfos);
	}
	else {	
		requestinfos_buff.resize(buffer_num);
		for (int i = 0; i < buffer_num; i++) {

			if (i == buffer_num - 1) {

				vector<vector<string>>::iterator first = requestinfos.begin();
				vector<vector<string>>::iterator second = requestinfos.begin() + requestinfos.size();

				requestinfos_buff[i].assign(first, second);
				requestinfos.erase(requestinfos.begin(), requestinfos.begin() + requestinfos.size());
			}
			else {
				vector<vector<string>>::iterator first = requestinfos.begin();
				vector<vector<string>>::iterator second = requestinfos.begin() + buffer_length;

				requestinfos_buff[i].assign(first, second);
				requestinfos.erase(requestinfos.begin(), requestinfos.begin() + buffer_length);
			}
		}

	}
	PatchRequestinfos.resize(buffer_num);
	for (int i = 0; i < requestinfos_buff.size(); i++) {
		float CPU_to_Memory;
		int vmCpuAddMemory;
		multimap<float, string> serverinfostmp;
		unordered_map<string, ServerInfo> serverinfostmp2;
		multimap<float, multimap<int, vector<string>>>     vminfostmp;  // �ں˱�->(vmCpu+�ڴ���Դ�� vm��Ϣ)
		multimap<int, vector<string>> vmInfosSort;  // ֻ��һ��
		for (auto s : serverinfos) {
			CPU_to_Memory = (float)(s.second.CpuCores_A + s.second.CpuCores_B) / (float)(s.second.MemerySize_A + s.second.MemerySize_B);

			//s.second.Cpu_Memory_ratio = CPU_to_Memory;

			serverinfostmp.insert(make_pair(CPU_to_Memory, s.first));
		}

		for (auto re : requestinfos_buff[i]) {
			if (re.size() > 2) {
				CPU_to_Memory = (float)(vminfos[re[1]].CpuCores) / (float)(vminfos[re[1]].MemerySize);  // �����vm������ں˱�
				vmCpuAddMemory = vminfos[re[1]].CpuCores + vminfos[re[1]].MemerySize;                   // �����vm������Դ����cpu+�ڴ棩

				vmInfosSort.insert(make_pair(vmCpuAddMemory, re));
				vminfostmp.insert(make_pair(CPU_to_Memory, vmInfosSort));                                       // �����ں˱���������
				vmInfosSort.clear();
			}
		}

		if (vminfostmp.size() > 5000000) {   // �������add���������ܴ�ʱ���ֵĸ�ϸ,��bug����

			PatchServerinfos.resize(PatchSize);

			for (auto s : serverinfostmp) {
				if (s.first <= 0.2) {   // �˲��������޸ģ�C������ʦ������ɱ��ϵ�У�
					PatchServerinfos[0][s.second] = serverinfos[s.second];
				}
				else if (s.first <= 0.5) {
					PatchServerinfos[1][s.second] = serverinfos[s.second];
				}
				else if (s.first <= 1.0) {
					PatchServerinfos[2][s.second] = serverinfos[s.second];
				}
				else if (s.first <= 5) {
					PatchServerinfos[3][s.second] = serverinfos[s.second];
				}
				else {
					PatchServerinfos[4][s.second] = serverinfos[s.second];
				}
			}

			PatchRequestinfos[i].resize(PatchSize);
			multimap<int, vector<string>, greater<int>> vmInfosSort_1;
			multimap<int, vector<string>, greater<int>> vmInfosSort_2;
			multimap<int, vector<string>, greater<int>> vmInfosSort_3;
			multimap<int, vector<string>, greater<int>> vmInfosSort_4;
			multimap<int, vector<string>, greater<int>> vmInfosSort_5;

			for (auto vms = vminfostmp.begin(); vms != vminfostmp.end(); vms++) {
				if ((*vms).first <= 0.3) {
					auto it = (*vms).second.begin();
					vmInfosSort_1.insert(make_pair((*it).first, (*it).second));
				}
				else if ((*vms).first <= 0.7) {
					auto it = (*vms).second.begin();
					vmInfosSort_2.insert(make_pair((*it).first, (*it).second));
				}
				else if ((*vms).first <= 1) {
					auto it = (*vms).second.begin();
					vmInfosSort_3.insert(make_pair((*it).first, (*it).second));
				}
				else if ((*vms).first <= 5) {
					auto it = (*vms).second.begin();
					vmInfosSort_4.insert(make_pair((*it).first, (*it).second));
				}
				else {
					auto it = (*vms).second.begin();
					vmInfosSort_5.insert(make_pair((*it).first, (*it).second));
				}

			}
			for (auto vm : vmInfosSort_1) {
				PatchRequestinfos[i][0].push_back(vm.second);
			}
			for (auto vm : vmInfosSort_2) {
				PatchRequestinfos[i][1].push_back(vm.second);
			}
			for (auto vm : vmInfosSort_3) {
				PatchRequestinfos[i][2].push_back(vm.second);
			}
			for (auto vm : vmInfosSort_4) {
				PatchRequestinfos[i][3].push_back(vm.second);
			}
			for (auto vm : vmInfosSort_5) {
				PatchRequestinfos[i][4].push_back(vm.second);
			}

			vmInfosSort_1.clear();
			vmInfosSort_2.clear();
			vmInfosSort_3.clear();
			vmInfosSort_4.clear();
			vmInfosSort_5.clear();
		}
		else if (vminfostmp.size() < 100000) {  // ֻ�ֳ�2�࣬�ں˱�>1��<1
			PatchServerinfos.resize(2);
			for (auto s : serverinfostmp) {
				if (s.first <= 1.0) {
					PatchServerinfos[0][s.second] = serverinfos[s.second];
				}
				else {
					PatchServerinfos[1][s.second] = serverinfos[s.second];
				}
			}

			PatchRequestinfos[i].resize(2);
			multimap<int, vector<string>, greater<int>> vmInfosSort_1;
			multimap<int, vector<string>, greater<int>> vmInfosSort_2;
			for (auto vms = vminfostmp.begin(); vms != vminfostmp.end(); vms++) {
				if ((*vms).first <= 1.0) {                                               // �˲��������޸� 30���
					auto it = (*vms).second.begin();
					vmInfosSort_1.insert(make_pair((*it).first, (*it).second));
				}
				else {
					auto it = (*vms).second.begin();
					vmInfosSort_2.insert(make_pair((*it).first, (*it).second));
				}
			}

			for (auto vm : vmInfosSort_1) {
				PatchRequestinfos[i][0].push_back(vm.second);
			}
			for (auto vm : vmInfosSort_2) {
				PatchRequestinfos[i][1].push_back(vm.second);
			}
			vmInfosSort_1.clear();
			vmInfosSort_2.clear();

		}
		if (false) {  // �������ں˱Ƚ��з���(����Ч�����У�
			PatchServerinfos.resize(1);
			PatchServerinfos[0] = serverinfos;

			PatchRequestinfos[i].resize(1);
			multimap<int, vector<string>, greater<int>> vmInfosSort_1;
			for (auto vms = vminfostmp.begin(); vms != vminfostmp.end(); vms++) {
				auto it = (*vms).second.begin();
				vmInfosSort_1.insert(make_pair((*it).first, (*it).second));
			}

			for (auto vm : vmInfosSort_1) {
				PatchRequestinfos[i][0].push_back(vm.second);
			}
			vmInfosSort_1.clear();
		}
		serverinfostmp.clear();
		vminfostmp.clear();

	}

	
}

int serverId = 0;  // ������ID(ȫ��)
multimap<string, unordered_map<string, vector<int>>> ServerOnVm_type;  // ͨ����������������¼�ڸ����ͷ������ϵ�vm��
vector<string>  ServerType;  // ��˳����ÿ��ѡ�������ŷ������������ƣ��������ʱ���մ�˳���ҵ���Ӧ������ID

// ------------------------------------------------------------------ ����--------------------------------------------------------------------------- //
void Expansion(int day, vector<unordered_map<string, ServerInfo>> serverinfos, vector<vector<vector<vector<string>>>> requestinfos_patch) {

	unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverBestmatch; // ��¼����ķ������ʹճ��������ƥ����Ϣ <�������ͺţ�<�����Id��vm�ڴ棬vmCpu,vmTwoNodes>>

	set<string> ServerTypeNum;

	multimap<string, ServerInfo> sysServerResourceTemp;

    //#pragma omp parallel for num_threads(4)
	for (int buff = 0; buff < buffer_num; buff++) {
		for (int patch = 0; patch < requestinfos_patch[buff].size(); patch++) {

			while (requestinfos_patch[buff][patch].size() > 0) {    // ���������Ϣ����һֱѭ��

				unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverTemp; // ��¼�������ʹճ��������ƥ����Ϣ <�������ͺţ�<�����Id��vm�ڴ棬vmCpu,vmTwoNodes>>
				multimap<int, string> Matching_degree;  // ��¼�ճ��ķ�������ƥ��̶Ȳ�ֵ��ȡ��С
				unordered_map<string, ServerInfo> ServerResourceTemp;
				multimap<float, string> CostAndDiffer; // ���Լ۱ȿ��ǽ�ȥ����¼��ֵ���Լ۱�Ȩ����ֵ

				for (auto s : serverinfos[patch]) {    // �����з���������ƥ��

					int taget_tatal = s.second.CpuCores_A + s.second.CpuCores_B + s.second.MemerySize_A + s.second.MemerySize_B;  //��ʼ���������ʹյ����������Դ��ֵ
					int taget_CpuCores_A = s.second.CpuCores_A, taget_CpuCores_B = s.second.CpuCores_B, taget_MemerySize_A = s.second.MemerySize_A, taget_MemerySize_B = s.second.MemerySize_B;
					bool flag = false;

					for (int i = 0; i < requestinfos_patch[buff][patch].size(); i++) {

						int vmCores = VmInfos[requestinfos_patch[buff][patch][i][1]].CpuCores;
						int vmMemory = VmInfos[requestinfos_patch[buff][patch][i][1]].MemerySize;
						int vmTwoNodes = VmInfos[requestinfos_patch[buff][patch][i][1]].VmTwoNodes;
						vector<int> vminfoAdd;

						if (vmTwoNodes) {    // ˫�ڵ�vm
							int needCores = vmCores / 2;
							int needMemory = vmMemory / 2;
							if (taget_CpuCores_A >= needCores && taget_CpuCores_B >= needCores && taget_MemerySize_A >= needMemory && taget_MemerySize_B >= needMemory) {
								taget_CpuCores_A -= needCores;
								taget_CpuCores_B -= needCores;
								taget_MemerySize_A -= needMemory;
								taget_MemerySize_B -= needMemory;

								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ

								vminfoAdd.push_back(vmCores);
								vminfoAdd.push_back(vmMemory);
								vminfoAdd.push_back(1);
								vminfoAdd.push_back(2);
								vmsOnserverTemp[s.first].insert(make_pair(requestinfos_patch[buff][patch][i][2], vminfoAdd));
								vminfoAdd.clear();


								if (i == requestinfos_patch[buff][patch].size() - 1)
									Matching_degree.insert(make_pair(taget_tatal, s.first));   // ��¼�������������ƥ��Ĳ�ֵ
							}
						}
						else {  // ���ڵ�
							if (taget_CpuCores_A >= vmCores && taget_MemerySize_A >= vmMemory) {

								flag = true;

								taget_CpuCores_A -= vmCores;
								taget_MemerySize_A -= vmMemory;

								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ

								vminfoAdd.push_back(vmCores);
								vminfoAdd.push_back(vmMemory);
								vminfoAdd.push_back(1);

								vmsOnserverTemp[s.first].insert(make_pair(requestinfos_patch[buff][patch][i][2], vminfoAdd));
								vminfoAdd.clear();
							}
							else if (taget_CpuCores_B >= vmCores && taget_MemerySize_B >= vmMemory) {

								taget_CpuCores_B -= vmCores;
								taget_MemerySize_B -= vmMemory;

								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // �����ֵ

								vminfoAdd.push_back(vmCores);
								vminfoAdd.push_back(vmMemory);
								vminfoAdd.push_back(2);

								vmsOnserverTemp[s.first].insert(make_pair(requestinfos_patch[buff][patch][i][2], vminfoAdd));
								vminfoAdd.clear();

							}

						}
					} // һ̨��������������
					Matching_degree.insert(make_pair(taget_tatal, s.first));   // ��¼�������������ƥ��Ĳ�ֵ

					// ��¼��̨�������ϵ���Դʣ��������
					ServerResourceTemp[s.first].CpuCores_A = taget_CpuCores_A;
					ServerResourceTemp[s.first].CpuCores_B = taget_CpuCores_B;
					ServerResourceTemp[s.first].MemerySize_A = taget_MemerySize_A;
					ServerResourceTemp[s.first].MemerySize_B = taget_MemerySize_B;
					ServerResourceTemp[s.first].PowerCost = ServerInfos[s.first].PowerCost;

				}  // ���з�����һ��ѭ������

				//ѡ����ƥ������з�����

				// ���ڲ�ֵ��С�ķ���������ȴ�����������Ҫ����������ȡ��һ��
				for (multimap<int, string>::iterator it = Matching_degree.begin(); it != Matching_degree.end(); ) {
					if (vmsOnserverTemp[(*it).second].size() == 0) {
						it = Matching_degree.erase(it);
					}
					else {
						it++;
					}
				}
				for (auto m : Matching_degree) {
					float costanddiffer = m.first * 0.5 + PerformanceAndCostRatio[m.second] * 0.5;  // �˲����������Ե��ڣ������ԣ����ݼ�2�������Լ۱�Ч����ã����ݼ�1����0.5���
					CostAndDiffer.insert(make_pair(costanddiffer, m.second));
				}

				auto first = CostAndDiffer.begin();


				ServerTypeNum.insert((*first).second); // ��¼һ��������ַ��������������

				ServerOnVm_type.insert(make_pair((*first).second, vmsOnserverTemp[(*first).second]));   //��ͬ�ķ��������ͻ�������һ����ϳɹ�����������˳����serverID

				// ������ķ���������ʣ�����Դ������ϵͳ����ռ����Դ���(�˴���multimap���շ��������ƴ��룬��Ϊ����ServerOnVm_type�еĴ�ķ�����˳�򱣳�һ�£������ٽ���ת����id����)
				sysServerResourceTemp.insert(make_pair((*first).second, ServerResourceTemp[(*first).second]));

				SERVERCOST += ServerInfos[(*first).second].ServerCost;  // ��������������ۼӼ���

				// ��requestinfosȥ������õ������,������Ҫ�޸ģ����ɾ����������add����
				auto s = vmsOnserverTemp[(*first).second]; // ��¼�պõ������

				for (auto ss : s) {
					int pos = 0;
					vector<string>::iterator deleVminfotmp;
					vector<vector<string>>::iterator deleVminfo;
					for (int pos = 0; pos < requestinfos_patch[buff][patch].size(); pos++) {
						deleVminfotmp = find(requestinfos_patch[buff][patch][pos].begin(), requestinfos_patch[buff][patch][pos].end(), ss.first);
						if (deleVminfotmp != requestinfos_patch[buff][patch][pos].end()) {
							deleVminfo = requestinfos_patch[buff][patch].begin() + pos;
							break;
						}

					}
					requestinfos_patch[buff][patch].erase(deleVminfo);
				}
				Matching_degree.clear();
			}  // һ����������������
		}

		requestinfos_patch[buff].clear(); // ���һ���������Ϣ
		PatchRequestinfos[buff].clear();
		PatchServerinfos.clear();
	}
	

	// �����������������Ϣ
	string s = "(purchase, ";

	s += to_string(ServerTypeNum.size()) + ")\n";
	res.push_back(s);

#ifdef TEST
	cout << s;
#endif 

	int ServertypeNum = 0;
	if (ServerTypeNum.size() != 0) {   // ��������Ϊ0ʱ���
		string serverType = (*ServerOnVm_type.begin()).first;
		multimap<string, ServerInfo>::iterator sSr = sysServerResourceTemp.begin();
		for (auto sov : ServerOnVm_type) {

			// ��¼����������������Щ�����
			ServerOnVm[serverId] = sov.second;

			for (auto ss : sov.second) {
				if (ss.second.size() > 3) {   // ˫�ڵ�
					VmOnServer[ss.first].push_back(serverId);
					VmOnServer[ss.first].push_back(ss.second[0]);
					VmOnServer[ss.first].push_back(ss.second[1]);
					VmOnServer[ss.first].push_back(ss.second[2]);
					VmOnServer[ss.first].push_back(ss.second[3]);

					serverRunVms[serverId]++;  // ��¼��ID�������������˶���̨vm
				}
				else {      // ���ڵ�
					VmOnServer[ss.first].push_back(serverId);
					VmOnServer[ss.first].push_back(ss.second[0]);
					VmOnServer[ss.first].push_back(ss.second[1]);
					VmOnServer[ss.first].push_back(ss.second[2]);

					serverRunVms[serverId]++;  // ��¼��ID�������������˶���̨vm
				}
			}
			// ��¼������������Դ��ȫ�֣�
			sysServerResource[serverId] = (*sSr).second;
			sSr++;
			if (sov.first != serverType) {
				s = "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
				res.push_back(s);

#ifdef TEST
				cout << "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
#endif
				serverType = sov.first;
				ServertypeNum = 0;
			}
			ServertypeNum++;
			serverId++;
		}
		// ��������������Ϣ
		s = "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
		res.push_back(s);

#ifdef TEST
		cout << "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
#endif	
		//ServerBuyNum.clear();
	}

	//���������Ǩ���б����б�����һ��ģ�������һ��
	if (!migrantList.empty()) {
		for (int i = 0; i < migrantList.size(); ++i)
		{
			res.push_back(migrantList[i]);

#ifdef TEST
			cout << migrantList[i];
#endif 
		}
		migrantList.clear();
	}
}

// ----------------------------------------------------------------Ԥ����add����------------------------------------------- //
int DealAddVm(vector<string> createVmInfo, vector<string>& res_tmp);
int DealAddVm(vector<string> createVmInfo, vector<string>& res_tmp) {
	string _reqVmType = createVmInfo[1], _reqId = createVmInfo[2];
	auto  vm = VmInfos[_reqVmType];
	// �Ǿ���һ�¾�׼������������ҳ���ֵ��С�ķ�����
	int minServerId = -1, minVal = INT32_MAX, node = -1;
	int vmCores = vm.CpuCores, vmMemo = vm.MemerySize, vmNode = vm.VmTwoNodes;
	if (vmNode == 0) {  // ���ڵ㲿��
		for (int i = 0; i < serverId; ++i) {
			// �ҳ�����̨���������ĸ��ڵ��ֵ��С
			int diff1 = sysServerResource[i].CpuCores_A - vmCores, diff2 = sysServerResource[i].MemerySize_A - vmMemo;
			if (diff1 >= 0 && diff2 >= 0 && diff1 + diff2 < minVal) {
				minServerId = i;
				minVal = diff1 + diff2;
				node = 1;
			}
			diff1 = sysServerResource[i].CpuCores_B - vmCores, diff2 = sysServerResource[i].MemerySize_B - vmMemo;
			if (diff1 >= 0 && diff2 >= 0 && diff1 + diff2 < minVal) {
				minServerId = i;
				minVal = diff1 + diff2;
				node = 2;
			}
		}
	}
	else {  // ˫�ڵ㲿��
		for (int i = 0; i < serverId; ++i) {
			// �ҳ�����̨�������Ĳ�ֵ��С
			int diff1A = sysServerResource[i].CpuCores_A - vmCores / 2;
			int diff2A = sysServerResource[i].MemerySize_A - vmMemo / 2;
			int diff1B = sysServerResource[i].CpuCores_B - vmCores / 2;
			int diff2B = sysServerResource[i].MemerySize_B - vmMemo / 2;
			if (diff1A >= 0 && diff2A >= 0 && diff1B >= 0 && diff2B >= 0 && diff1A + diff2A + diff1B + diff2B < minVal) {
				minServerId = i;
				minVal = diff1A + diff2A + diff1B + diff2B;
				node = 0;
			}
		}
	}
	// �����ȥ��Ҫ�ĳɷ�AB�ڵ����
	int success = -1;
	auto& server = sysServerResource[minServerId];
	if (AllocServerResources(server, vm, minServerId, _reqId, res_tmp, node)) {
		serverRunVms[minServerId]++;
		success = 1;
	}
	assert(server.CpuCores_A >= 0 && server.CpuCores_B >= 0 && server.MemerySize_A >= 0 && server.MemerySize_B >= 0);
	return success;

}


// ----------------------------------����ÿһ��add����----------------------------------------------------------------------//
int ServerIdAdd = 0; // ÿ�����ӵķ���������
int ReDealAddVM(multimap<string, unordered_map<string, vector<int>>> serverOnVm, vector<string> request, vector<string>& res_tmp, int day);
int ReDealAddVM(multimap<string, unordered_map<string, vector<int>>> serverOnVm, vector<string> request, vector<string>& res_tmp, int day) {
	string s = "(";
	if (VmOnServer.find(request[2]) != VmOnServer.end()) {
		vector<int> serverInfo = VmOnServer[request[2]];
		if (serverInfo.size() == 5) {  // ˫�ڵ㲿��
			s += to_string(serverInfo[0]) + ")\n";
		}
		else {  // ���ڵ㲿��
			if (serverInfo[3] == 1)
				s += to_string(serverInfo[0]) + ", A)\n";		  // ���䵽A�ڵ�			
			else
				s += to_string(serverInfo[0]) + ", B)\n";          // ���䵽B�ڵ�	
		}
		if (day > 0) {
			for (auto& tmp : res_tmp) {
				if (tmp == "null") {
					tmp = s; break;
				}
			}
		}
		else
		{
			res_tmp.push_back(s);
		}
		return 1;
	}
	return -1;
}

// ---------------------------------------------------------------Ǩ������� --------------------------------------------------------------------//
void Migrate() {

	multimap<int, int> serverRunVmsTmp; // vm������������ID
	int vmSum = 0; // ���������
	int moveNum = 0; // Ǩ�����������
	vector<string> res_tmp; // Ǩ�ƹ��̽���ݴ�
	for (auto s : serverRunVms) {
		serverRunVmsTmp.insert(make_pair(s.second, s.first)); // ���������������������
		vmSum += s.second;
	}
	map<int, int>::iterator head = serverRunVmsTmp.begin();
	auto  tail = serverRunVmsTmp.end();
	tail--; // Ŀ���ǽ�tailָ��ָ��serverRunVmsTmp���һ��Ԫ�ص�λ��

	while (head != tail) {
		if (moveNum > (vmSum / 1000) || (vmSum / 1000) == 0)
			break;
		while (head != tail && (*head).first != 0) {

			auto& Destinationserver = sysServerResource[(*(tail)).second];
			auto& SourceServer = sysServerResource[(*head).second];

			int& DestinationCoreA = Destinationserver.CpuCores_A,
				& DestinationCoreB = Destinationserver.CpuCores_B,
				& DestinationMemoryA = Destinationserver.MemerySize_A,
				& DestinationMemoryB = Destinationserver.MemerySize_B;

			int& SourceCoreA = SourceServer.CpuCores_A,
				& SourceCoreB = SourceServer.CpuCores_B,
				& SourceMemoryA = SourceServer.MemerySize_A,
				& SourceMemoryB = SourceServer.MemerySize_B;

			for (auto sov = ServerOnVm[(*head).second].begin(); sov != ServerOnVm[(*head).second].end();) { // �����÷�����ID�ϵ������vm

				int vmCores = (*sov).second[0],
					vmMemory = (*sov).second[1];

				if ((*sov).second.size() > 3) {  // ˫�ڵ�

					int needCores = vmCores / 2, needMemory = vmMemory / 2;  //�������Դ���԰����
					if (DestinationCoreA >= needCores && DestinationCoreB >= needCores && DestinationMemoryA >= needMemory && DestinationMemoryB >= needMemory) {   //������AB�ڵ���Դ������

						// Ŀ�ķ�����ʣ����Դ����
						DestinationCoreA -= needCores;
						DestinationCoreB -= needCores;
						DestinationMemoryA -= needMemory;
						DestinationMemoryB -= needMemory;

						//Դ������ʣ����Դ����
						SourceCoreA += needCores;
						SourceCoreB += needCores;
						SourceMemoryA += needMemory;
						SourceMemoryB += needMemory;

						// ����������������ĸ��������ϣ�˫�ڵ㲿��
						VmOnServer[(*sov).first] = vector<int>{ (*tail).second,vmCores,vmMemory,1,2 };

						res_tmp.push_back("(" + (*sov).first + ", " + to_string((*tail).second) + ")\n");			//���

						moveNum++;
						ServerOnVm[(*tail).second].insert(*sov);           // ���������������λ��
						sov = ServerOnVm[(*head).second].erase(sov);       // �����������ԭ��λ��ɾ��

						serverRunVms[(*tail).second]++;                   //  Ŀ�ķ�����ID�ϵ������������1
						serverRunVms[(*head).second]--;                   //  ԭ������ID�ϵ������������1

						assert(serverRunVms[(*head).second] >= 0);
					}
					else {
						sov++;                                                                                 //��Դ���㣬�ڴ˷������Ϸ���������ʧ��
					}
				}
				else if (DestinationCoreA >= vmCores && DestinationMemoryA >= vmMemory) {  // A�ڵ�

					// Ŀ�ķ�����ʣ����Դ����
					DestinationCoreA -= vmCores;
					DestinationMemoryA -= vmMemory;

					//Դ������ʣ����Դ����
					if ((*sov).second[2] == 1) {
						SourceCoreA += vmCores;
						SourceMemoryA += vmMemory;
					}
					else if ((*sov).second[2] == 2) {
						SourceCoreB += vmCores;
						SourceMemoryB += vmMemory;
					}


					// ����������������ĸ���������
					VmOnServer[(*sov).first] = vector<int>{ (*tail).second,vmCores,vmMemory,1 };

					res_tmp.push_back("(" + (*sov).first + ", " + to_string((*tail).second) + ", A)\n");			//���

					moveNum++;

					(*sov).second[2] = 1;
					ServerOnVm[(*tail).second].insert(*sov);           // ���������������λ��
					sov = ServerOnVm[(*head).second].erase(sov);       // �����������ԭ��λ��ɾ��
					serverRunVms[(*tail).second]++;                   //  Ŀ�ķ�����ID�ϵ������������1
					serverRunVms[(*head).second]--;                   //  ԭ������ID�ϵ������������1

					assert(serverRunVms[(*head).second] >= 0);

				}
				else if (DestinationCoreB >= vmCores && DestinationMemoryB >= vmMemory) {  // B�ڵ�
					// Ŀ�ķ�����ʣ����Դ����
					DestinationCoreB -= vmCores;
					DestinationMemoryB -= vmMemory;

					//Դ������ʣ����Դ����
					if ((*sov).second[2] == 1) {
						SourceCoreA += vmCores;
						SourceMemoryA += vmMemory;
					}
					else if ((*sov).second[2] == 2) {
						SourceCoreB += vmCores;
						SourceMemoryB += vmMemory;
					}

					// ����������������ĸ��������ϣ�˫�ڵ㲿��	
					VmOnServer[(*sov).first] = vector<int>{ (*tail).second,vmCores,vmMemory,2 };

					res_tmp.push_back("(" + (*sov).first + ", " + to_string((*tail).second) + ", B)\n");			//���

					moveNum++;
					(*sov).second[2] = 2;
					ServerOnVm[(*tail).second].insert(*sov);           // ���������������λ��
					sov = ServerOnVm[(*head).second].erase(sov);       // �����������ԭ��λ��ɾ��
					serverRunVms[(*tail).second]++;                   //  Ŀ�ķ�����ID�ϵ������������1
					serverRunVms[(*head).second]--;                   //  ԭ������ID�ϵ������������1

					assert(serverRunVms[(*head).second] >= 0);

				}
				else {
					sov++;
				}
			}
			tail--;
		}
		tail = serverRunVmsTmp.end();
		tail--;
		head++;
	}
	string s = "(migration, " + to_string(moveNum) + ")\n";
	migrantList.push_back(s);
	for (auto s : res_tmp) {
		migrantList.push_back(s);
	}
}

void _Migrate() {
	string s = "(migration, 0)\n";
#ifdef TEST
	cout << s;
#endif
	res.push_back(s);
}

// ---------------------------------------------------------------����ƥ�� --------------------------------------------------------------------- //
void Match(int day, unordered_map<string, ServerInfo> serverinfos, vector<vector<string>> requestinfos) {

	vector<string> res_tmp;

	if (day == 0)  // ��һ������ȫ�򣬺���Ҳ���Ե���day�Ĵ�С
	{
		GetServerInfosPatch(serverinfos, VmInfos, requestinfos);
		Expansion(day, PatchServerinfos, PatchRequestinfos);
		//Migrate();
		//_Migrate();
#ifdef TEST 
		printf("There are %d requests waiting to matching !!\n", requestinfos.size());
#endif
		for (vector<vector<string>>::iterator req = requestinfos.begin(); req != requestinfos.end(); req++) {
			int opType = (*req).size() == 3 ? 1 : 0;
			if (opType) {
				int resourceEnough = ReDealAddVM(ServerOnVm_type, *req, res_tmp, day);
			}
			else {
				DealDeleteVM(*req);
			}

		}
		for (auto re : res_tmp) {
			res.push_back(re);
#ifdef TEST
			cout << re;
#endif
		}

		res_tmp.clear();

		ServerIdAdd += ServerOnVm_type.size();
		ServerOnVm_type.clear();
	}
	else {
#ifdef TEST 
		printf("There are %d requests waiting to matching !!\n", requestinfos.size());
#endif
		//if (day % 5 == 0) {
			Migrate();
		//}
		
		for (vector<vector<string>>::iterator req = requestinfos.begin(); req != requestinfos.end(); ) {
			int opType = (*req).size() == 3 ? 1 : 0;
			if (opType) {  // add

				int resourceEnough = DealAddVm(*req, res_tmp);
				if (resourceEnough != -1) {
					req = requestinfos.erase(req);
				}
				else
				{
					req++;
					res_tmp.push_back("null");
				}
			}
			else {
				DealDeleteVM(*req);
				req = requestinfos.erase(req);
			}
		}
		GetServerInfosPatch(serverinfos, VmInfos, requestinfos);
		Expansion(day, PatchServerinfos, PatchRequestinfos);
		//if (day % 5 != 0) {
		//	_Migrate();
		//}
		//_Migrate();
		for (vector<vector<string>>::iterator req = requestinfos.begin(); req != requestinfos.end(); req++) {
			int opType = (*req).size() == 3 ? 1 : 0;
			if (opType) {  // add

				int resourceEnough = ReDealAddVM(ServerOnVm_type, *req, res_tmp, day);
				assert(resourceEnough != -1);
			}
			else {
				DealDeleteVM(*req);
			}
		}

		// �ϲ�������add�����������
		for (auto re : res_tmp) {
			res.push_back(re);
#ifdef TEST
			cout << re;
#endif
		}

		res_tmp.clear();
		ServerIdAdd += ServerOnVm_type.size();
		ServerOnVm_type.clear();

	}
}

// ��������Ѽ���
void ServerPowerCost() {
	for (int i = 0; i < serverId; i++) {
		if (serverRunVms[i] != 0) {
			POWERCOST += sysServerResource[i].PowerCost;
		}
	}
}


int main() {
	clock_t start, finish;
	start = clock();

#ifdef TEST
	freopen(filePath.c_str(), "rb", stdin);
#endif

	int ServerTypes;    // �ṩ�ķ�������������
	string serverType, cpuCores, memorySize, serverCost, powerCost;
	scanf("%d", &ServerTypes);

	for (int i = 0; i < ServerTypes; i++) {
		cin >> serverType >> cpuCores >> memorySize >> serverCost >> powerCost;  // ����һ�����͵ķ�������Ϣ
		GetServerInfos(serverType, cpuCores, memorySize, serverCost, powerCost); // ������������Ϣ��������
	}

	int VmTypes = 0;    // �ṩ���������������
	scanf("%d", &VmTypes);

	string vmType, vmCpuCores, vmMemory, vmTwoNodes;
	for (int i = 0; i < VmTypes; i++) {
		cin >> vmType >> vmCpuCores >> vmMemory >> vmTwoNodes;
		GetVmInfos(vmType, vmCpuCores, vmMemory, vmTwoNodes);
	}

	int RequestDays = 0, DayRequestNumbers = 0;
	scanf("%d", &RequestDays);
	string op, reqVmType, reqId;

	// ��ʼ��������
	// InitServer(); //��ʼ�������������Դ

	for (int day = 0; day < RequestDays; day++) {
		scanf("%d", &DayRequestNumbers);
		RequestInfos.clear();
		for (int i = 0; i < DayRequestNumbers; i++) {
			cin >> op;
			if (op[1] == 'a') {
				cin >> reqVmType >> reqId;
				GetRequest(op, reqVmType, reqId);    //����һ�����������������
			}
			else {
				cin >> reqId;
				GetRequest(op, reqId);              //����һ����ɾ�����������
			}
		}
#ifdef TEST
		if (day == 0 || (day + 1) % 100 == 0) {
			//printf("The %d day begin matching!!!\n", day + 1);
		}
#endif
		GetBestServer(day, ServerInfos, RequestDays);
		Match(day, ServerInfos, RequestInfos);
		ServerPowerCost();
	}

	fclose(stdin);
	finish = clock();

	TOTALCOST = SERVERCOST + POWERCOST;  //�����ܷ���

#ifdef UPLOAD
	for (auto& s : res) std::cout << s;
#endif



#ifdef TEST
	ofstream out("output.txt");
	for (auto& s : res) {
		if (out.is_open()) {
			out << s;
		}
		//std::cout << s;
	}
	out.close();
	printf("\nusr time: %f s \n", double(finish - start) / CLOCKS_PER_SEC);
	printf("server cost: %lld \npower cost: %lld \ntotal cost: %lld \n", SERVERCOST, POWERCOST, TOTALCOST);
#endif

	return 0;
}