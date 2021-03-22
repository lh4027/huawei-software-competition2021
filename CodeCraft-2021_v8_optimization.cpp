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
using namespace std;

#define TEST

//#define UPLOAD

// 结构体存储各种信息

// 单个服务器信息结构体
struct ServerInfo {

	int CpuCores_A;    // A节点cpu资源数
	int CpuCores_B;    // B节点cpu资源数

	int MemerySize_A;  // A节点内存资源数
	int MemerySize_B;  // B节点内存资源数

	int ServerCost;    // 服务器花费
	int PowerCost;     // 电费

	float Cpu_Memory_ratio;  // 内核比
	string ServerType;      // 服务器类型
};

// 单个虚拟机信息结构体
struct VmInfo {

	int CpuCores;    // 消耗的cpu资源数

	int MemerySize;  // 消耗的内存资源数

	int VmTwoNodes;  // 单/双节点部署

	string VmType;     // 虚拟机Id

	float Cpu_Memory_ratio;
};


// 所有服务器信息
unordered_map<string, ServerInfo> ServerInfos;

// 所有虚拟机信息
unordered_map<string, VmInfo> VmInfos;

// 一天所有请求信息
vector<vector<string>> RequestInfos;

// 购买的服务器数量
int serverNumber = 0;

// 购买的服务器信息（系统现存资源）
unordered_map<int, ServerInfo> sysServerResource;

// 当前开机的服务器上运行的虚拟机数量
map<int, int> serverRunVms;

// 记录虚拟机运行在那个服务器上
unordered_map<string, vector<int>> VmOnServer;   //[in] 虚拟机Id  [in] 虚拟机所在的服务器id,虚拟机cpu,虚拟机内存，单/双节点部署）

// 记录服务器上运行了哪些虚拟机
unordered_map<int, unordered_map<string, vector<int>>> ServerOnVm;   //[in] 服务器Id  [in] （虚拟机id, 虚拟机cpu,内存，单/双节点部署）

multimap<string, unordered_map<string, vector<int>>> DayServerOnVm; // 记录一天的编过服务器ID的服务器凑出的上的虚拟机信息，此顺序即为服务器ID顺序

vector<string> res;

// 迁移虚拟机的输出列表
vector<string> migrantList{ "(migration, 0)\n" };

#ifdef TEST
const string filePath = "training-1.txt";
#endif

// 成本
long long SERVERCOST = 0, POWERCOST = 0, TOTALCOST = 0;


// 解析服务器信息
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

	ServerInfos[_serverType].CpuCores_A = _cpuCores / 2;   // A节点cpu
	ServerInfos[_serverType].CpuCores_B = _cpuCores / 2;   // B节点cpu

	ServerInfos[_serverType].MemerySize_A = _memorySize / 2;  //A节点内存
	ServerInfos[_serverType].MemerySize_B = _memorySize / 2;  //B节点内存

	ServerInfos[_serverType].PowerCost = _powerCost;   //电费
	ServerInfos[_serverType].ServerCost = _serverCost; //服务器成本
}


// 解析虚拟机信息
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

// 解析用户添加请求
void GetRequest(string& op, string& reqVmType, string& reqId) {

	string _op, _reqVmType, _reqId;
	_op = op.substr(1, op.size() - 1);                                 // add
	_reqVmType = reqVmType.substr(0, reqVmType.size() - 1);            // 请求添加的虚拟机类型
	_reqId = reqId.substr(0, reqId.size() - 1);                        // 请求添加的虚拟机id
	RequestInfos.push_back(vector<string>{_op, _reqVmType, _reqId});   // 用户添加请求放入用户请求信息中
}

// 解析用户删除请求
void GetRequest(string& op, string& reqId) {
	string _op, _reqId;
	_reqId = reqId.substr(0, reqId.size() - 1);                        // 请求删除的虚拟机id
	_op = op.substr(1, op.size() - 1);                                 // delete
	RequestInfos.push_back(vector<string>{_op, _reqId});               // 用户删除虚拟机请求放入用户请求信息中
}


// 在服务器上分配虚拟机资源
bool AllocServerResources(ServerInfo& server, VmInfo& vm, int serverId, string vmId, vector<string>& res_tmp, int node) {

	int vmCores = vm.CpuCores,
		vmMemory = vm.MemerySize,
		vmTwoNodes = vm.VmTwoNodes;

	int& serverCoreA = server.CpuCores_A,
		& serverCoreB = server.CpuCores_B,
		& serverMemoryA = server.MemerySize_A,
		& serverMemoryB = server.MemerySize_B;

	if (vmTwoNodes) {   //双节点部署时
		int needCores = vmCores / 2, needMemory = vmMemory / 2;  //虚拟机资源数对半均分
		if (serverCoreA >= needCores && serverCoreB >= needCores && serverMemoryA >= needMemory && serverMemoryB >= needMemory) {   //服务器AB节点资源均满足
			// 资源分出
			serverCoreA -= needCores;
			serverCoreB -= needCores;
			serverMemoryA -= needMemory;
			serverMemoryB -= needMemory;

			// 记录虚拟机运行在哪个服务器上（双节点部署）
			VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1,2 };

			// 记录服务器上运行了哪些虚拟机(迁移使用)
			ServerOnVm[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 1, 2 }));

			res_tmp.push_back("(" + to_string(serverId) + ")\n");
			return true;
		}
		else {
			return false;                                               //资源不足，在此服务器上分配该虚拟机失败
		}
	}
	else if (node == 1 && serverCoreA >= vmCores && serverMemoryA >= vmMemory) {     //单节点部署检查A节点是否满足分配条件

		serverCoreA -= vmCores;
		serverMemoryA -= vmMemory;

		// 记录虚拟机运行在哪个服务器上（A节点部署）
		VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1 };

		// 记录服务器上运行了哪些虚拟机
		ServerOnVm[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 1}));

		res_tmp.push_back("(" + to_string(serverId) + ", A)\n");
		return true;
	}
	else if (node == 2 && serverCoreB >= vmCores && serverMemoryB >= vmMemory) {    //单节点部署检查B节点是否满足分配条件

		serverCoreB -= vmCores;
		serverMemoryB -= vmMemory;

		// 记录虚拟机运行在哪个服务器上（B节点部署）
		VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,2 };

		// 记录服务器上运行了哪些虚拟机
		ServerOnVm[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 2}));

		res_tmp.push_back("(" + to_string(serverId) + ", B)\n");
		return true;
	}
	return false;                                                      //单双节点资源均不足，在此服务器上分配该虚拟机失败                                         
}

// 在服务器上释放虚拟机资源
void ReleaseServerResources(ServerInfo& server, vector<int>& vmInfo, int serverId, string vmId) {
	// [in]:现有服务器资源 
	// [in]:运行的虚拟机信息[虚拟机所在的服务器id,虚拟机cpu,虚拟机内存，单/双节点部署]  
	// [in]:服务器Id 
	// [in]:虚拟机Id	
	if (vmInfo.size() == 5) {  // 双节点部署的虚拟机
		int cores = vmInfo[1] / 2, memory = vmInfo[2] / 2;

		server.CpuCores_A += cores;
		server.CpuCores_B += cores;
		server.MemerySize_A += memory;
		server.MemerySize_B += memory;
	}
	else { // 单节点部署
		int cores = vmInfo[1], memory = vmInfo[2];
		if (vmInfo[3] == 1) {  // A节点部署
			server.CpuCores_A += cores;
			server.MemerySize_A += memory;
		}
		else {                 // B节点部署
			server.CpuCores_B += cores;
			server.MemerySize_B += memory;
		}
	}
}


// 处理删除虚拟机操作
void DealDeleteVM(vector<string>& delVmInfo) {

	string _vmId = delVmInfo[1];

	vector<int> _vmInfo = VmOnServer[_vmId];

	int _serverId = _vmInfo[0];  //部署虚拟机的服务器id

	auto& server = sysServerResource[_serverId];

	ReleaseServerResources(server, _vmInfo, _serverId, _vmId);   //释放虚拟机

	serverRunVms[_serverId]--;  //服务器上的虚拟机数量-1
	VmOnServer.erase(_vmId);

	//更新每台服务器上存放的虚拟机
	for (auto it = ServerOnVm[_serverId].begin(); it != ServerOnVm[_serverId].end(); it++) {
		if (it->first == _vmId) {
			ServerOnVm[_serverId].erase(it);			//删除ServerOnVm上的虚拟机
			break;
		}
	}
	

	assert(serverRunVms[_serverId] >= 0);

}

unordered_map<string, float> PerformanceAndCostRatio;  //有序容器

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


		PerformanceAndCostRatio.insert(make_pair(serverType, (ServerCost + (requestdays - day) * PowerCost) / (Cpu + Memory)));  //根据key值可以自动排序
	}

}

vector<unordered_map<string, ServerInfo>> PatchServerinfos;
vector<vector< vector<string>>>PatchRequestinfos;

int PatchSize = 11;
// 将服务器根据内核比归类
void GetServerInfosPatch(unordered_map<string, ServerInfo>& serverinfos, unordered_map<string, VmInfo> vminfos, vector<vector<string>> requestinfos) {

	float CPU_to_Memory;
	multimap<float, string> serverinfostmp;
	unordered_map<string, ServerInfo> serverinfostmp2;
	multimap<float, vector<string>>     vminfostmp;
	for (auto& s : serverinfos) {
		CPU_to_Memory = (float)(s.second.CpuCores_A + s.second.CpuCores_B) / (float)(s.second.MemerySize_A + s.second.MemerySize_B);

		s.second.Cpu_Memory_ratio = CPU_to_Memory;

		serverinfostmp.insert(make_pair(CPU_to_Memory, s.first));
	}

	for (auto re : requestinfos) {
		if (re.size() > 2) {
			CPU_to_Memory = (float)(vminfos[re[1]].CpuCores) / (float)(vminfos[re[1]].MemerySize);
			vminfostmp.insert(make_pair(CPU_to_Memory, re));
		}
	}

	if (vminfostmp.size() > 500000) {   // 当虚拟机add请求数量很大时，分的更细

		PatchServerinfos.resize(PatchSize);

		for (auto s : serverinfostmp) {
			if (s.first <= 0.2) {   // 此参数可以修改（C语言老师看了想杀我系列）
				PatchServerinfos[0][s.second] = serverinfos[s.second];
			}
			else if (s.first <= 0.3) {
				PatchServerinfos[1][s.second] = serverinfos[s.second];
			}
			else if (s.first <= 0.4) {
				PatchServerinfos[2][s.second] = serverinfos[s.second];
			}
			else if (s.first <= 0.6) {
				PatchServerinfos[3][s.second] = serverinfos[s.second];
			}
			else if (s.first <= 0.8) {
				PatchServerinfos[4][s.second] = serverinfos[s.second];
			}
			else if (s.first <= 1.0) {
				PatchServerinfos[5][s.second] = serverinfos[s.second];
			}
			else if (s.first <= 2.0) {
				PatchServerinfos[6][s.second] = serverinfos[s.second];
			}
			else if (s.first <= 3.0) {
				PatchServerinfos[7][s.second] = serverinfos[s.second];
			}
			else if (s.first <= 5.0) {
				PatchServerinfos[8][s.second] = serverinfos[s.second];
			}
			else if (s.first <= 7.0) {
				PatchServerinfos[9][s.second] = serverinfos[s.second];
			}
			else {
				PatchServerinfos[10][s.second] = serverinfos[s.second];
			}
		}

		PatchRequestinfos.resize(PatchSize);
		for (auto vms : vminfostmp) {
			if (vms.first <= 0.1) {
				PatchRequestinfos[0].push_back(vms.second);
			}
			else if (vms.first <= 0.2) {
				PatchRequestinfos[1].push_back(vms.second);
			}
			else if (vms.first <= 0.3) {
				PatchRequestinfos[2].push_back(vms.second);
			}
			else if (vms.first <= 0.5) {
				PatchRequestinfos[3].push_back(vms.second);
			}
			else if (vms.first <= 0.7) {
				PatchRequestinfos[4].push_back(vms.second);
			}
			else if (vms.first <= 1.0) {
				PatchRequestinfos[5].push_back(vms.second);
			}
			else if (vms.first <= 2.0) {
				PatchRequestinfos[6].push_back(vms.second);
			}
			else if (vms.first <= 4.0) {
				PatchRequestinfos[7].push_back(vms.second);
			}
			else if (vms.first <= 6.0) {
				PatchRequestinfos[8].push_back(vms.second);
			}
			else if (vms.first <= 10.0) {
				PatchRequestinfos[9].push_back(vms.second);
			}
			else {
				PatchRequestinfos[10].push_back(vms.second);
			}
		}
	}
	else {  // 只分成2类，内核比>1和<1
		PatchServerinfos.resize(2);
		for (auto s : serverinfostmp) {
			if (s.first <= 1.0) {
				PatchServerinfos[0][s.second] = serverinfos[s.second];
			}
			else {
				PatchServerinfos[1][s.second] = serverinfos[s.second];
			}
		}
		PatchRequestinfos.resize(2);
		for (auto vms : vminfostmp) {
			if (vms.first <= 30) {
				PatchRequestinfos[0].push_back(vms.second);
			}
			else {
				PatchRequestinfos[1].push_back(vms.second);
			}
		}
	}
	serverinfostmp.clear();
	vminfostmp.clear();
}

int serverId = 0;  // 服务器ID(全局)
multimap<string, unordered_map<string, vector<int>>> ServerOnVm_type;  // 通过服务器类型来记录在该类型服务器上的vm，
vector<string>  ServerType;  // 按顺序存放每天选出的最优服务器类型名称，方便输出时按照此顺序找到对应服务器ID

// 扩容服务器 + 分配
void Expansion(int day, vector<unordered_map<string, ServerInfo>> serverinfos, vector<vector<vector<string>>> requestinfos_patch) {


	unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverBestmatch; // 记录购买的服务器和凑出的虚拟机匹配信息 <服务器型号，<虚拟机Id，vm内存，vmCpu,vmTwoNodes>>

	//unordered_map<string, int> ServerBuyNum;

	set<string> ServerTypeNum;

	multimap<string, ServerInfo> sysServerResourceTemp;
	//vector<vector<string>> requestinfos_patch;


	for (int patch = 0; patch < requestinfos_patch.size(); patch++) {

		while (requestinfos_patch[patch].size() > 0) {    // 如果请求信息还有一直循环

			unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverTemp; // 记录服务器和凑出的虚拟机匹配信息 <服务器型号，<虚拟机Id，vm内存，vmCpu,vmTwoNodes>>
			multimap<int, string> Matching_degree;  // 记录凑出的服务器的匹配程度差值，取最小
			unordered_map<string, ServerInfo> ServerResourceTemp;
			multimap<float, string> CostAndDiffer; // 将性价比考虑进去，记录差值与性价比权衡后的值

			for (auto s : serverinfos[patch]) {    // 和所有服务器进行匹配

				int taget_tatal = s.second.CpuCores_A + s.second.CpuCores_B + s.second.MemerySize_A + s.second.MemerySize_B;  //初始化服务器和凑到的虚拟机资源差值
				int taget_CpuCores_A = s.second.CpuCores_A, taget_CpuCores_B = s.second.CpuCores_B, taget_MemerySize_A = s.second.MemerySize_A, taget_MemerySize_B = s.second.MemerySize_B;
				bool flag = false;


				for (int i = 0; i < requestinfos_patch[patch].size(); i++) {
					if (requestinfos_patch[patch][i].size() > 2) {   //在所有add请求中凑数

						int vmCores = VmInfos[requestinfos_patch[patch][i][1]].CpuCores;
						int vmMemory = VmInfos[requestinfos_patch[patch][i][1]].MemerySize;
						int vmTwoNodes = VmInfos[requestinfos_patch[patch][i][1]].VmTwoNodes;
						vector<int> vminfoAdd;

						if (vmTwoNodes) {    // 双节点vm
							int needCores = vmCores / 2;
							int needMemory = vmMemory / 2;
							if (taget_CpuCores_A >= needCores && taget_CpuCores_B >= needCores && taget_MemerySize_A >= needMemory && taget_MemerySize_B >= needMemory) {
								taget_CpuCores_A -= needCores;
								taget_CpuCores_B -= needCores;
								taget_MemerySize_A -= needMemory;
								taget_MemerySize_B -= needMemory;

								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值

								vminfoAdd.push_back(vmCores);
								vminfoAdd.push_back(vmMemory);
								vminfoAdd.push_back(1);
								vminfoAdd.push_back(2);
								vmsOnserverTemp[s.first].insert(make_pair(requestinfos_patch[patch][i][2], vminfoAdd));
								vminfoAdd.clear();
								flag = true;

								if (i == requestinfos_patch[patch].size() - 1)
									Matching_degree.insert(make_pair(taget_tatal, s.first));   // 记录服务器与虚拟机匹配的差值
							}
							else {
								Matching_degree.insert(make_pair(taget_tatal, s.first));   // 记录服务器与虚拟机匹配的差值
								break;
							}
						}
						else {  // 单节点
							if (taget_CpuCores_A >= vmCores && taget_MemerySize_A >= vmMemory) {

								flag = true;

								taget_CpuCores_A -= vmCores;
								taget_MemerySize_A -= vmMemory;

								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值

								vminfoAdd.push_back(vmCores);
								vminfoAdd.push_back(vmMemory);
								vminfoAdd.push_back(1);

								vmsOnserverTemp[s.first].insert(make_pair(requestinfos_patch[patch][i][2], vminfoAdd));
								vminfoAdd.clear();


								if (i == requestinfos_patch[patch].size() - 1)
									Matching_degree.insert(make_pair(taget_tatal, s.first));   // 记录服务器与虚拟机匹配的差值
							}
							else if (taget_CpuCores_B >= vmCores && taget_MemerySize_B >= vmMemory) {
								flag = true;
								taget_CpuCores_B -= vmCores;
								taget_MemerySize_B -= vmMemory;

								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值

								vminfoAdd.push_back(vmCores);
								vminfoAdd.push_back(vmMemory);
								vminfoAdd.push_back(2);

								vmsOnserverTemp[s.first].insert(make_pair(requestinfos_patch[patch][i][2], vminfoAdd));
								vminfoAdd.clear();


								if (i == requestinfos_patch[patch].size() - 1)
									Matching_degree.insert(make_pair(taget_tatal, s.first));   // 记录服务器与虚拟机匹配的差值
							}
							else {
								Matching_degree.insert(make_pair(taget_tatal, s.first));   // 记录服务器与虚拟机匹配的差值
								break;
							}
						}

					}

				} // 一台服务器凑数结束

				// 记录这台服务器上的资源剩余分配情况
				ServerResourceTemp[s.first].CpuCores_A = taget_CpuCores_A;
				ServerResourceTemp[s.first].CpuCores_B = taget_CpuCores_B;
				ServerResourceTemp[s.first].MemerySize_A = taget_MemerySize_A;
				ServerResourceTemp[s.first].MemerySize_B = taget_MemerySize_B;
				ServerResourceTemp[s.first].PowerCost = ServerInfos[s.first].PowerCost;


			}  // 所有服务器一次循环结束
			//选出最匹配的所有服务器


			// 由于差值最小的服务器可能却不能满足分配要求，所以依次取下一个
			//multimap<int, string>::iterator first = Matching_degree.begin();
			for (multimap<int, string>::iterator it = Matching_degree.begin(); it != Matching_degree.end(); ) {
				if (vmsOnserverTemp[(*it).second].size() == 0) {
					it = Matching_degree.erase(it);
				}
				else {
					it++;
				}
			}
			for (auto m : Matching_degree) {
				float costanddiffer = m.first * 0.5 + PerformanceAndCostRatio[m.second] * 0.5;  // 此参数比例可以调节，经测试，0.5效果最好
				CostAndDiffer.insert(make_pair(costanddiffer, m.second));
			}

			auto first = CostAndDiffer.begin();
			//ServerType.push_back((*first).second);

			ServerTypeNum.insert((*first).second); // 记录一次买多少种服务器，用于输出

			ServerOnVm_type.insert(make_pair((*first).second, vmsOnserverTemp[(*first).second]));   //相同的服务器类型会排列在一起，组合成购买的输出，按顺序排serverID

			// 将购买的服务器及其剩余的资源数存入系统现在占有资源情况(此处用multimap按照服务器名称存入，是为了与ServerOnVm_type中的存的服务器顺序保持一致，后面再将其转换成id保存)
			sysServerResourceTemp.insert(make_pair((*first).second, ServerResourceTemp[(*first).second]));


			SERVERCOST += ServerInfos[(*first).second].ServerCost;  // 购买服务器费用累加计算

			// 从requestinfos去除分配好的虚拟机		
			requestinfos_patch[patch].erase(requestinfos_patch[patch].begin(), requestinfos_patch[patch].begin() + vmsOnserverTemp[(*first).second].size());
			Matching_degree.clear();


		}  // 一天所有请求分配结束
	}

	requestinfos_patch.clear(); // 清除一天的请求信息

	PatchRequestinfos.clear();
	PatchServerinfos.clear();

	// 输出购买类型数量信息
	string s = "(purchase, ";

	//s += to_string(ServerTypeNum.size()) + ") day: " + to_string(day) + "\n";
	s += to_string(ServerTypeNum.size()) + ")\n";
	res.push_back(s);

#ifdef TEST
	cout << s;
#endif 

	int ServertypeNum = 0;
	if (ServerTypeNum.size() != 0) {   // 购买数不为0时输出
		string serverType = (*ServerOnVm_type.begin()).first;
		multimap<string, ServerInfo>::iterator sSr = sysServerResourceTemp.begin();
		for (auto sov : ServerOnVm_type) {

			// 记录服务器上运行了那些虚拟机
			ServerOnVm[serverId] = sov.second;

			for (auto ss : sov.second) {
				if (ss.second.size() > 3) {   // 双节点
					VmOnServer[ss.first].push_back(serverId);
					VmOnServer[ss.first].push_back(ss.second[0]);
					VmOnServer[ss.first].push_back(ss.second[1]);
					VmOnServer[ss.first].push_back(ss.second[2]);
					VmOnServer[ss.first].push_back(ss.second[3]);

					serverRunVms[serverId]++;  // 记录该ID服务器上运行了多少台vm
				}
				else {      // 单节点
					VmOnServer[ss.first].push_back(serverId);
					VmOnServer[ss.first].push_back(ss.second[0]);
					VmOnServer[ss.first].push_back(ss.second[1]);
					VmOnServer[ss.first].push_back(ss.second[2]);

					serverRunVms[serverId]++;  // 记录该ID服务器上运行了多少台vm
				}
			}
			// 记录服务器现余资源（全局）
			sysServerResource[serverId] = (*sSr).second;
			sysServerResource[serverId].ServerType = sSr->first;
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
		// 输出购买服务器信息
		s = "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
		res.push_back(s);

#ifdef TEST
		cout << "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
#endif	

		//ServerBuyNum.clear();
	}

	// 接着来输出迁移的列表，这个迁移列表是上一天的，不是这一天的
	if (!migrantList.empty()) {  // 第一天没有机器可迁移
		for (int i = 0; i < migrantList.size(); ++i) {
			res.push_back(migrantList[i]);

#ifdef TEST
			cout << migrantList[i];
#endif
		}
		migrantList.clear();
	}
	else
		res.push_back("(migration, 0)\n");
}



// 此函数用来
int DealAddVm(vector<string> createVmInfo, vector<string>& res_tmp);
int DealAddVm(vector<string> createVmInfo, vector<string>& res_tmp) {
	string _reqVmType = createVmInfo[1], _reqId = createVmInfo[2];
	auto  vm = VmInfos[_reqVmType];
	// 那就试一下精准分配虚拟机，找出差值最小的服务器
	int minServerId = -1, minVal = INT32_MAX, node = -1;
	int vmCores = vm.CpuCores, vmMemo = vm.MemerySize, vmNode = vm.VmTwoNodes;
	if (vmNode == 0) {  // 单节点部署
		for (int i = 0; i < serverId; ++i) {
			// 找出和哪台服务器的哪个节点差值最小
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
	else {  // 双节点部署
		for (int i = 0; i < serverId; ++i) {
			// 找出和哪台服务器的差值最小
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
	// 部署进去，要改成分AB节点的了
	int success = -1;
	auto& server = sysServerResource[minServerId];
	if (AllocServerResources(server, vm, minServerId, _reqId, res_tmp, node)) {
		serverRunVms[minServerId]++;
		success = 1;
	}
	assert(server.CpuCores_A >= 0 && server.CpuCores_B >= 0 && server.MemerySize_A >= 0 && server.MemerySize_B >= 0);
	return success;

}


// 处理每一个add请求
int ServerIdAdd = 0; // 每天增加的服务器数量
int ReDealAddVM(multimap<string, unordered_map<string, vector<int>>> serverOnVm, vector<string> request, vector<string>& res_tmp, int day);
int ReDealAddVM(multimap<string, unordered_map<string, vector<int>>> serverOnVm, vector<string> request, vector<string>& res_tmp, int day) {
	string s = "(";
	if (VmOnServer.find(request[2]) != VmOnServer.end()) {
		vector<int>& serverInfo = VmOnServer[request[2]];
		if (serverInfo.size() == 5) {  // 双节点部署
			s += to_string(serverInfo[0]) + ")\n";
		}
		else {  // 单节点部署
			if (serverInfo[3] == 1)
				s += to_string(serverInfo[0]) + ", A)\n";		  // 分配到A节点			
			else
				s += to_string(serverInfo[0]) + ", B)\n";          // 分配到B节点	
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


// 迁移虚拟机
void Migrate(int day) {

	multimap<int, int> serverRunVmsTmp; // vm数量，服务器ID
	int vmSum = 0; // 虚拟机总数
	int moveNum = 0; // 迁移虚拟机数量
	vector<string> res_tmp; // 迁移过程结果暂存
	for (auto s : serverRunVms) {
		serverRunVmsTmp.insert(make_pair(s.second, s.first));
		vmSum += s.second;
	}
	map<int, int>::iterator head = serverRunVmsTmp.begin();
	auto  tail = serverRunVmsTmp.end();
	tail--; // 目的是将tail指针指向serverRunVmsTmp最后一个元素的位置


	while (moveNum < 5 * vmSum / 5000 && head != tail) {

		while (moveNum < 5 * vmSum / 5000 && head != tail && (*head).first != 0) {

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


			for (auto sov = ServerOnVm[(*head).second].begin(); sov != ServerOnVm[(*head).second].end() && moveNum < 5 * vmSum / 5000;) { // 遍历该服务器ID上的所存的vm

				int vmCores = (*sov).second[0],
					vmMemory = (*sov).second[1];

				if ((*sov).second.size() > 3) {  // 双节点

					int needCores = vmCores / 2, needMemory = vmMemory / 2;  //虚拟机资源数对半均分
					if (DestinationCoreA >= needCores && DestinationCoreB >= needCores && DestinationMemoryA >= needMemory && DestinationMemoryB >= needMemory) {   //服务器AB节点资源均满足

						// 目的服务器剩余资源更新
						DestinationCoreA -= needCores;
						DestinationCoreB -= needCores;
						DestinationMemoryA -= needMemory;
						DestinationMemoryB -= needMemory;

						//源服务器剩余资源更新
						SourceCoreA += needCores;
						SourceCoreB += needCores;
						SourceMemoryA += needMemory;
						SourceMemoryB += needMemory;


						// 更新虚拟机运行在哪个服务器上（双节点部署）
						VmOnServer[(*sov).first] = vector<int>{ (*tail).second,vmCores,vmMemory,1,2 };

						res_tmp.push_back("(" + (*sov).first + ", " + to_string((*tail).second) + ")\n");			//输出
						if (day == 319 && (*head).second == 2160) {
							fstream f;
							f.open("test.txt", ios::out | ios::app);
							f << "srcId: " << (*head).second << "rstId: " << (*tail).second << endl;
							f.close();
						}

						moveNum++;
						ServerOnVm[(*tail).second].insert(*sov);           // 将此虚拟机放入新位置
						sov = ServerOnVm[(*head).second].erase(sov);       // 将此虚拟机从原有位置删除

						serverRunVms[(*tail).second]++;                   //  目的服务器ID上的虚拟机数量加1
						serverRunVms[(*head).second]--;                   //  原服务器ID上的虚拟机数量减1
					

#ifdef TEST
						if (serverRunVms[(*head).second] == -1) {
							assert(serverRunVms[(*head).second] >= 0);
						}
#endif							
					}
					else {
						sov++;                                                                                 //资源不足，在此服务器上分配该虚拟机失败
					}
				}
				else if (DestinationCoreA >= vmCores && DestinationMemoryA >= vmMemory) {  // A节点

					if (day == 319 && (*head).second == 2160) {
						fstream f;
						f.open("test.txt", ios::out | ios::app);
						//f << "srcId: " << (*head).second << "rstId: " << (*tail).second << endl;
						f << "secId: " << (*head).second << "CA: " << SourceCoreA << "MA: " << SourceMemoryA  << "CB: " << SourceCoreB << "MB: " << SourceMemoryB << " " << (*sov).second[2] << endl;
						f.close();
					}

					// 目的服务器剩余资源更新
					DestinationCoreA -= vmCores;
					DestinationMemoryA -= vmMemory;

					//源服务器剩余资源更新
					if ((*sov).second[2] == 1) {
						SourceCoreA += vmCores;
						SourceMemoryA += vmMemory;
					}
					else if ((*sov).second[2] == 2) {
						SourceCoreB += vmCores;
						SourceMemoryB += vmMemory;
					}

					if (day == 319 && (*head).second == 2160) {
						fstream f;
						f.open("test.txt", ios::out | ios::app);
						//f << "srcId: " << (*head).second << "rstId: " << (*tail).second << endl;
						f << "secId: " << (*head).second << "CA: " << SourceCoreA << "MA: " << SourceMemoryA << "CB: " << SourceCoreB << "MB: " << SourceMemoryB << " " << (*sov).second[2] << endl;
						f.close();
					}

					// 更新虚拟机运行在哪个服务器上（双节点部署）
					VmOnServer[(*sov).first] = vector<int>{ (*tail).second,vmCores,vmMemory,1 };

					res_tmp.push_back("(" + (*sov).first + ", " + to_string((*tail).second) + ", A)\n");			//输出

					moveNum++;

					sov->second[2] = 1;

					ServerOnVm[(*tail).second].insert(*sov);           // 将此虚拟机放入新位置
					sov = ServerOnVm[(*head).second].erase(sov);       // 将此虚拟机从原有位置删除
					serverRunVms[(*tail).second]++;                   //  目的服务器ID上的虚拟机数量加1
					serverRunVms[(*head).second]--;                   //  原服务器ID上的虚拟机数量减1

#ifdef TEST
					if (serverRunVms[(*head).second] == -1) {
						assert(serverRunVms[(*head).second] >= 0);
					}
#endif	


				}
				else if (DestinationCoreB >= vmCores && DestinationMemoryB >= vmMemory) {  // B节点
					// 目的服务器剩余资源更新
					//printf("before id: %d, SourceCpuA: %d, SourceCpuB: %d, SourceMemoA: %d, SourceMemoB: %d\n", (*head).second, SourceCoreA, SourceCoreB, SourceMemoryA, SourceMemoryB);
					//printf("after id: %d DCA: %d, DCB: %d, SMA: %d, SMB: %d\n", (*tail).second, DestinationCoreA, DestinationCoreB, DestinationMemoryA, DestinationMemoryB);

					//printf("before vmCore: %d, vmMemo: %d\n", vmCores, vmMemory);

					if (day == 319 && (*head).second == 2160) {
						fstream f;
						f.open("test.txt", ios::out | ios::app);
						//f << "srcId: " << (*head).second << "rstId: " << (*tail).second << endl;
						f << "srcId: " << (*head).second << "CA: " << SourceCoreA << "MA: " << SourceMemoryA << "CB: " << SourceCoreB << "MB: " << SourceMemoryB << (*sov).second[2] << endl;
						f.close();
					}

					DestinationCoreB -= vmCores;
					DestinationMemoryB -= vmMemory;

					//源服务器剩余资源更新
					if ((*sov).second[2] == 1) {
						SourceCoreA += vmCores;
						SourceMemoryA += vmMemory;
					}
					else if ((*sov).second[2] == 2){
						SourceCoreB += vmCores;
						SourceMemoryB += vmMemory;
					}
					//printf("after id: %d SCA: %d, SCB: %d, SMA: %d, SMB: %d\n", (*head).second, SourceCoreA, SourceCoreB, SourceMemoryA, SourceMemoryB);
					//printf("after id: %d DCA: %d, DCB: %d, SMA: %d, SMB: %d\n", (*tail).second, DestinationCoreA, DestinationCoreB, DestinationMemoryA, DestinationMemoryB);

					//printf("after vmCore: %d, vmMemo: %d\n", vmCores, vmMemory);

					if (day == 319 && (*head).second == 2160) {
						fstream f;
						f.open("test.txt", ios::out | ios::app);
						//f << "srcId: " << (*head).second << "rstId: " << (*tail).second << endl;
						f << "srcId: " << (*head).second << "CA: " << SourceCoreA << "MA: " << SourceMemoryA << "CB: " << SourceCoreB << "MB: " << SourceMemoryB << endl;
						f.close();
					}

					// 更新虚拟机运行在哪个服务器上（双节点部署）
					VmOnServer[(*sov).first] = vector<int>{ (*tail).second,vmCores,vmMemory,2 };

					res_tmp.push_back("(" + (*sov).first + ", " + to_string((*tail).second) + ", B)\n");			//输出
					//if (day == 319 && (*head).second == 2160) {
					//	fstream f;
					//	f.open("test.txt", ios::out | ios::app);
					//	f << "srcId: " << (*head).second << "rstId: " << (*tail).second << endl;
					//	f.close();
					//}

					moveNum++;

					sov->second[2] = 2;

					ServerOnVm[(*tail).second].insert(*sov);           // 将此虚拟机放入新位置
					sov = ServerOnVm[(*head).second].erase(sov);       // 将此虚拟机从原有位置删除
					serverRunVms[(*tail).second]++;                   //  目的服务器ID上的虚拟机数量加1
					serverRunVms[(*head).second]--;                   //  原服务器ID上的虚拟机数量减1

#ifdef TEST
					if (serverRunVms[(*head).second] == -1) {
						assert(serverRunVms[(*head).second] >= 0);
					}
#endif	
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

		if (head == tail)
			break;
	}

	
	


	string s = "(migration, " + to_string(moveNum) + ")\n";
//#ifdef TEST 
//	cout << s;
//#endif
	//res.push_back(s);
	migrantList.push_back(s);
	for (auto s : res_tmp) {
//#ifdef TEST 
//		cout << s;
//#endif
		//res.push_back(s);
		migrantList.push_back(s);
	}

}


// 分配
void Match(int day, unordered_map<string, ServerInfo> serverinfos, vector<vector<string>> requestinfos) {

	vector<string> res_tmp;

	//auto Remingrequestinfos = requestinfos;


	if (day == 0)  // 第一天我们全买，后面也可以调整day的大小
	{
		GetServerInfosPatch(serverinfos, VmInfos, requestinfos);
		Expansion(day, PatchServerinfos, PatchRequestinfos);
		//Migrate();
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

		Migrate(day); // 其实是对前一天的进行迁移，输出顺序要改变

		if (day == 319) {
			fstream f;
			f.open("test.txt", ios::out | ios::app);
			f << "after" << day << " " << sysServerResource[2160].CpuCores_A << " " << sysServerResource[2160].CpuCores_B << " " << sysServerResource[2160].MemerySize_A << " " << sysServerResource[2160].MemerySize_B << endl;
			f.close();
		}
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

		// 合并连两个add请求处理的输出
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
	// 检查有没有负资源
	for (auto s : sysServerResource) {
		assert(s.second.CpuCores_A >= 0 && s.second.CpuCores_B >= 0 && s.second.MemerySize_A >= 0 && s.second.MemerySize_B >= 0);
	}
}

// 服务器电费计算
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

	int ServerTypes;    // 提供的服务器类型数量
	string serverType, cpuCores, memorySize, serverCost, powerCost;
	scanf("%d", &ServerTypes);

	for (int i = 0; i < ServerTypes; i++) {
		cin >> serverType >> cpuCores >> memorySize >> serverCost >> powerCost;  // 读入一个类型的服务器信息
		GetServerInfos(serverType, cpuCores, memorySize, serverCost, powerCost); // 解析服务器信息，并保存
	}

	int VmTypes = 0;    // 提供的虚拟机类型数量
	scanf("%d", &VmTypes);

	string vmType, vmCpuCores, vmMemory, vmTwoNodes;
	for (int i = 0; i < VmTypes; i++) {
		cin >> vmType >> vmCpuCores >> vmMemory >> vmTwoNodes;
		GetVmInfos(vmType, vmCpuCores, vmMemory, vmTwoNodes);
	}

	int RequestDays = 0, DayRequestNumbers = 0;
	scanf("%d", &RequestDays);
	string op, reqVmType, reqId;

	// 开始处理请求
	// InitServer(); //初始化购买服务器资源

	for (int day = 0; day < RequestDays; day++) {
		scanf("%d", &DayRequestNumbers);
		RequestInfos.clear();
		for (int i = 0; i < DayRequestNumbers; i++) {
			cin >> op;
			if (op[1] == 'a') {
				cin >> reqVmType >> reqId;
				GetRequest(op, reqVmType, reqId);    //解析一天中增加虚拟机请求
			}
			else {
				cin >> reqId;
				GetRequest(op, reqId);              //解析一天中删除虚拟机请求
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

	TOTALCOST = SERVERCOST + POWERCOST;  //计算总费用

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