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
//#define MIGRATION_LIMT 35  // 此参数调大时，数据集2耗费增加，数据集1减小
//#define VMCPUTOMEMEORY 1.0 // 将虚拟机按内核比划分为两类时的界限
//#define DAY 0              // 选择第几天开始我们再进行扩容策略
////#define MIGRATION_DAY 5    // 每隔5天迁移一次
//#define DELETE_RATIO_TO_REQEST 0.002  // 一天的请求中delete所占的比重, 此参数越小迁移次数越多，时间越长
//#define MIGRATE_SUCCESS_RATIO 0.08  // 迁移中虚拟机找到可用的服务器后，还要不要继续往下找更优服务器的阈值
//#define MIGRATE_FAIL_RATIO 0.02  // 在这个版本中标迁移中虚拟机找不到可以用的服务器的次数超过多少次后就不找了的阈值，可以取到很小，不需要经常调整
//float   COST_PERFORMANCE = 0.8;       // 性价比考虑公式系数，值越大表明越倾向于性价比
//
//#define ALLOC_DISCARD_RATIO 4  // 分配时丢掉服务器资源剩余比较多的服务器，越大表示丢得越少（经验上的值？）
//
////#define DIFF_RATIO_PARA 8
//
//int PatchSize = 10;           // 服务器种类特别多时，对服务器按照内核比切分的块数
//int QuestRange = 3000;        // 此参数对请求少的天数作用无效甚至更差
//int buffer_num = 10;        // 此参数的调小貌似服务器成本会更低，时间却没有增加多少，且只对数据集2有用，对于请求多的天数该值越大越能加快速度
//
//
//#define TEST
////#define UPLOAD
//
//#ifdef TEST
//const string filePath = "training-1.txt";
//#endif  
//
//// 单个服务器信息结构体
//struct ServerInfo {
//
//	int CpuCores_A;    // A节点cpu资源数
//	int CpuCores_B;    // B节点cpu资源数
//
//	int MemerySize_A;  // A节点内存资源数
//	int MemerySize_B;  // B节点内存资源数
//
//	int ServerCost;    // 服务器花费
//	int PowerCost;     // 电费
//
//	float Cpu_Memory_ratio;  // 内核比
//};
//
//// 单个虚拟机信息结构体
//struct VmInfo {
//
//	int CpuCores;    // 消耗的cpu资源数
//
//	int MemerySize;  // 消耗的内存资源数
//
//	int VmTwoNodes;  // 单/双节点部署
//
//	string VmType;     // 虚拟机Id
//
//	float Cpu_Memory_ratio;  // 虚拟机的内核比
//};
//
//// 数据集给出的可购买的服务器信息
//unordered_map<string, ServerInfo> ServerInfos;
//
//// 数据集给出的虚拟机型号
//unordered_map<string, VmInfo> VmInfos;
//
//// 存放一天所有请求信息
//list<vector<string>> RequestInfos;
//
//// 存放前天所有请求信息
//list<vector<string>> LastRequestInfos;
//
//// 现有的服务器资源<serverId, ServerInfo>
//unordered_map<int, ServerInfo> existServerResource;
//
//// 当前开机的服务器上运行的虚拟机数量  <serverId, vmNum>
//map<int, int> ServerRunVmsNum;
//
//// 记录虚拟机运行在那个服务器上
//unordered_map<string, vector<int>> VmIdOnServer;   //[in] 虚拟机Id  [in] 虚拟机所在的服务器id,虚拟机cpu,虚拟机内存，单/双节点部署）
//
//// 记录服务器上运行了哪些虚拟机
//unordered_map<int, unordered_map<string, vector<int>>> ServerIdOnVms;   //[in] 服务器Id  [in] （虚拟机id, 虚拟机cpu,内存，单/双节点部署）
//
//vector<string> migrantList{ "(migration, 0)\n" };  // 用来存当天的迁移输出
//unordered_map<int, vector<string>> res; // 用来输出数据
//
//unordered_map<string, float> PerformanceAndCostRatio;  //存放性价比
//
//// 服务器和虚拟机的分批分配信息
//vector<unordered_map<string, ServerInfo>> PatchServerinfos;
//vector<vector<list<vector<string>>>>PatchRequestinfos;
//
//int serverId = 0;  // 服务器ID(全局)
//multimap<string, unordered_map<string, vector<int>>> ServerIdOnVms_type;  // 通过服务器类型来记录在该类型服务器上的vm，
//vector<string>  ServerType;  // 按顺序存放每天选出的最优服务器类型名称，方便输出时按照此顺序找到对应服务器ID
//mutex m;//实例化m对象，不要理解为定义变量
//
//// 成本
//long long SERVER_COST = 0, POWER_COST = 0, TOTAL_COST = 0;
//
////map<int, map<int, list<vector<string>>>> divideResource;  // 把虚拟机的CPU和MEMORY分开，<cpu, <memory, vminfoList> >
//unordered_map<string, list<vector<string>>::iterator> addRequestIterMap;  // 映射虚拟机请求列表的迭代器，实现常数时间删除请求
//
//// 解析服务器信息
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
//	ServerInfos[serverType].CpuCores_A = _cpuCores / 2;   // A节点cpu
//	ServerInfos[serverType].CpuCores_B = _cpuCores / 2;   // B节点cpu
//
//	ServerInfos[serverType].MemerySize_A = _memorySize / 2;  //A节点内存
//	ServerInfos[serverType].MemerySize_B = _memorySize / 2;  //B节点内存
//
//	ServerInfos[serverType].PowerCost = _powerCost;   //电费
//	ServerInfos[serverType].ServerCost = _serverCost; //服务器成本
//}
//
//// 解析虚拟机信息
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
//// 解析添加请求
//void GetRequest(string& buff, string& reqestVmType, string& reqestId) {
//
//	string _buff, _reqVmType, _reqId;
//	buff.pop_back();
//	buff.erase(0, 1);
//
//	reqestVmType.pop_back();
//	reqestId.pop_back();
//	RequestInfos.push_back(vector<string>{buff, reqestVmType, reqestId});   // 用户添加请求放入用户请求信息中
//}
//
//// 解析删除请求
//void GetRequest(string& buff, string& reqestId) {
//
//	reqestId.pop_back();
//
//	buff.pop_back();
//	buff.erase(0, 1);
//	RequestInfos.push_back(vector<string>{buff, reqestId});               // 用户删除虚拟机请求放入用户请求信息中
//}
//
//// 在服务器上分配虚拟机资源
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
//	if (vmTwoNodes) {   //双节点部署时
//		int needCores = vmCores / 2, needMemory = vmMemory / 2;  //虚拟机资源数对半均分
//		if (serverCoreA >= needCores && serverCoreB >= needCores && serverMemoryA >= needMemory && serverMemoryB >= needMemory) {   //服务器AB节点资源均满足
//			// 资源分出
//			serverCoreA -= needCores;
//			serverCoreB -= needCores;
//			serverMemoryA -= needMemory;
//			serverMemoryB -= needMemory;
//
//			assert(serverCoreA >= 0 && serverCoreB >= 0 && serverMemoryA >= 0 && serverMemoryB >= 0);
//
//			// 记录虚拟机运行在哪个服务器上（双节点部署）
//			VmIdOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1,2 };
//
//			// 记录服务器上运行了哪些虚拟机(迁移使用)
//			ServerIdOnVms[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 1, 2 }));
//
//			res_tmp.push_back("(" + to_string(serverId) + ")\n");
//			return true;
//		}
//		else {
//			return false;                                               //资源不足，在此服务器上分配该虚拟机失败
//		}
//	}
//	else if (node == 1 && serverCoreA >= vmCores && serverMemoryA >= vmMemory) {     //单节点部署检查A节点是否满足分配条件
//
//		serverCoreA -= vmCores;
//		serverMemoryA -= vmMemory;
//
//		assert(serverCoreA >= 0 && serverMemoryA >= 0);
//
//		// 记录虚拟机运行在哪个服务器上（A节点部署）
//		VmIdOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1 };
//
//		// 记录服务器上运行了哪些虚拟机
//		ServerIdOnVms[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 1}));
//
//		res_tmp.push_back("(" + to_string(serverId) + ", A)\n");
//		return true;
//	}
//	else if (node == 2 && serverCoreB >= vmCores && serverMemoryB >= vmMemory) {    //单节点部署检查B节点是否满足分配条件
//
//		serverCoreB -= vmCores;
//		serverMemoryB -= vmMemory;
//
//		assert(serverCoreB >= 0 && serverMemoryB >= 0);
//
//		// 记录虚拟机运行在哪个服务器上（B节点部署）
//		VmIdOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,2 };
//
//		// 记录服务器上运行了哪些虚拟机
//		ServerIdOnVms[serverId].insert(make_pair(vmId, vector<int>{vmCores, vmMemory, 2}));
//
//		res_tmp.push_back("(" + to_string(serverId) + ", B)\n");
//		return true;
//	}
//	return false;                                                      //单双节点资源均不足，在此服务器上分配该虚拟机失败                                         
//}
//
//// 在服务器上释放虚拟机资源
//void ReleaseServerResources(ServerInfo& server, vector<int>& vmInfo, int serverId, string vmId) {
//	// [in]:现有服务器资源 
//	// [in]:运行的虚拟机信息[虚拟机所在的服务器id,虚拟机cpu,虚拟机内存，单/双节点部署]  
//	// [in]:服务器Id 
//	// [in]:虚拟机Id	
//	if (vmInfo.size() == 5) {  // 双节点部署的虚拟机
//		int cores = vmInfo[1] / 2, memory = vmInfo[2] / 2;
//
//		server.CpuCores_A += cores;
//		server.CpuCores_B += cores;
//		server.MemerySize_A += memory;
//		server.MemerySize_B += memory;
//	}
//	else { // 单节点部署
//		int cores = vmInfo[1], memory = vmInfo[2];
//		if (vmInfo[3] == 1) {  // A节点部署
//			server.CpuCores_A += cores;
//			server.MemerySize_A += memory;
//		}
//		else {                 // B节点部署
//			server.CpuCores_B += cores;
//			server.MemerySize_B += memory;
//		}
//	}
//}
//
//// ---------------------------------------------------  处理删除虚拟机操作------------------------------------------------------------------//
//void DealDeleteVM(vector<string>& deleVmInfo) {
//
//	string _vmId = deleVmInfo[1];
//
//	vector<int> _vmInfo = VmIdOnServer[_vmId];
//
//	int _serverId = _vmInfo[0];  //部署虚拟机的服务器id
//
//	auto& server = existServerResource[_serverId];
//
//	ReleaseServerResources(server, _vmInfo, _serverId, _vmId);   //释放虚拟机
//
//	ServerRunVmsNum[_serverId]--;  //服务器上的虚拟机数量-1
//
//	// 更新VmIdOnServer
//	auto it1 = VmIdOnServer.find(_vmId);
//	VmIdOnServer.erase(it1);
//
//	//更新每台服务器上存放的虚拟机
//	auto it2 = ServerIdOnVms[_serverId].find(_vmId);
//	ServerIdOnVms[_serverId].erase(it2);
//
//	assert(ServerRunVmsNum[_serverId] >= 0);
//
//}
//
//
//// --------------------------------------------------- 计算服务器性价比 --------------------------------------------------------------------//
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
//// ---------------------------------------------------将服务器和虚拟机根据内核比归类-----------------------------------------------------------------------------------//
//void GetServerInfosPatch(unordered_map<string, ServerInfo> serverinfos, unordered_map<string, VmInfo> vminfos, list<vector<string>> requestinfos) {
//
//
//	vector<list<vector<string>>> requestinfos_buff;  // 切分的add请求buffer
//	int buffer_length = requestinfos.size() / buffer_num;
//
//	if (requestinfos.size() < QuestRange) {   // 如果数据小于QuestRange，就不用切分
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
//		multimap<float, multimap<int, vector<string>>>     vminfostmp;  // 内核比->(vmCpu+内存资源， vm信息)
//		multimap<int, vector<string>> vmInfosSort;  // 只存一个
//		for (auto s : serverinfos) {
//			CPU_to_Memory = (float)(s.second.CpuCores_A + s.second.CpuCores_B) / (float)(s.second.MemerySize_A + s.second.MemerySize_B);
//
//			serverinfostmp.insert(make_pair(CPU_to_Memory, s.first));
//		}
//
//		for (auto re : requestinfos_buff[i]) {
//			if (re.size() > 2) {
//				CPU_to_Memory = (float)(vminfos[re[1]].CpuCores) / (float)(vminfos[re[1]].MemerySize);  // 计算该vm请求的内核比
//				vmCpuAddMemory = vminfos[re[1]].CpuCores + vminfos[re[1]].MemerySize;                   // 计算该vm所需资源数（cpu+内存）
//
//				vmInfosSort.insert(make_pair(vmCpuAddMemory, re));
//				vminfostmp.insert(make_pair(CPU_to_Memory, vmInfosSort));                                       // 按照内核比升序排列
//				vmInfosSort.clear();
//			}
//		}
//
//		if (serverinfostmp.size() >= 20000) {   // 服务器种类数量很大时，分的更细
//
//			PatchServerinfos.resize(PatchSize);
//
//			for (auto s : serverinfostmp) {
//				if (s.first <= 0.2) {   // 此参数可以修改（C语言老师看了想杀我系列）
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
//		else if (serverinfostmp.size() < 200000) {  // 只分成2类，内核比>1和<1
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
//				if ((*vms).first <= 1.0) {                                               // 此参数可以修改 30最好
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
//		if (false) {  // 不按照内核比进行分类(测试效果不行）
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
//// ------------------------------------------------------------------ 扩容 + 多线程--------------------------------------------------------------------------- //
//ofstream testExpansion("TestExpansion.txt");
//
//void assignVmToServer(int buff, vector<vector<list<vector<string>>>>& requestinfos_patch,
//	set<string>& ServerTypeNum, multimap<string, ServerInfo>& sysServerResourceTemp,
//	vector<unordered_map<string, ServerInfo>>& serverinfos) {
//
//	for (int buffNo = 0; buffNo < buff; buffNo++) {
//		for (int patch = 0; patch < requestinfos_patch[buffNo].size(); patch++) {
//
//			while (requestinfos_patch[buffNo][patch].size() > 0) {    // 如果请求信息还有一直循环
//
//				unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverTemp; // 记录服务器和凑出的虚拟机匹配信息 <服务器型号，<虚拟机Id，vm内存，vmCpu,vmTwoNodes>>
//				multimap<int, string> Matching_degree;  // 记录凑出的服务器的匹配程度差值，取最小
//				unordered_map<string, ServerInfo> ServerResourceTemp;
//				multimap<float, string> CostAndDiffer; // 将性价比考虑进去，记录差值与性价比权衡后的值
//
//				for (auto s : serverinfos[patch]) {    // 和所有服务器进行匹配
//
//					int taget_tatal = s.second.CpuCores_A + s.second.CpuCores_B + s.second.MemerySize_A + s.second.MemerySize_B;  //初始化服务器和凑到的虚拟机资源差值
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
//						if (vmTwoNodes) {    // 双节点vm
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
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值
//
//								vminfoAdd[0] = vmCores;
//								vminfoAdd[1] = vmMemory;
//								vminfoAdd[2] = 1;
//								vminfoAdd[3] = 2;
//								vmsOnserverTemp[s.first].insert(make_pair((*vm)[2], vminfoAdd));
//								vminfoAdd.clear();
//							}
//						}
//						else {  // 单节点
//							vminfoAdd.resize(3);
//							if (taget_CpuCores_A >= vmCores && taget_MemerySize_A >= vmMemory) {
//
//								taget_CpuCores_A -= vmCores;
//								taget_MemerySize_A -= vmMemory;
//
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值
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
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值
//
//								vminfoAdd[0] = vmCores;
//								vminfoAdd[1] = vmMemory;
//								vminfoAdd[2] = 2;
//
//								vmsOnserverTemp[s.first].insert(make_pair((*vm)[2], vminfoAdd));
//								vminfoAdd.clear();
//							}
//						}
//					} // 一台服务器凑数结束
//					Matching_degree.insert(make_pair(taget_tatal, s.first));   // 记录服务器与虚拟机匹配的差值
//
//					// 记录这台服务器上的资源剩余分配情况
//					ServerResourceTemp[s.first].CpuCores_A = taget_CpuCores_A;
//					ServerResourceTemp[s.first].CpuCores_B = taget_CpuCores_B;
//					ServerResourceTemp[s.first].MemerySize_A = taget_MemerySize_A;
//					ServerResourceTemp[s.first].MemerySize_B = taget_MemerySize_B;
//					ServerResourceTemp[s.first].PowerCost = ServerInfos[s.first].PowerCost;
//
//				}  // 所有服务器一次循环结束
//
//				//选出最匹配的所有服务器
//
//				// 由于差值最小的服务器可能却不能满足分配要求，所以依次取下一个
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
//					float costanddiffer = m.first * (1.0 - COST_PERFORMANCE) + PerformanceAndCostRatio[m.second] * COST_PERFORMANCE;  // 此参数比例可以调节，经测试，数据集2不考虑性价比效果最好，数据集1考虑0.5最好
//					//float costanddiffer = m.first;
//					CostAndDiffer.insert(make_pair(costanddiffer, m.second));
//				}
//				auto first = CostAndDiffer.begin();
//
//				m.lock();
//				ServerTypeNum.insert((*first).second); // 记录一次买多少种服务器，用于输出
//
//				ServerIdOnVms_type.insert(make_pair((*first).second, vmsOnserverTemp[(*first).second]));   //相同的服务器类型会排列在一起，组合成购买的输出，按顺序排serverID
//
//				// 将购买的服务器及其剩余的资源数存入系统现在占有资源情况(此处用multimap按照服务器名称存入，是为了与ServerIdOnVms_type中的存的服务器顺序保持一致，后面再将其转换成id保存)
//				sysServerResourceTemp.insert(make_pair((*first).second, ServerResourceTemp[(*first).second]));
//
//				SERVER_COST += ServerInfos[(*first).second].ServerCost;  // 购买服务器费用累加计算
//				m.unlock();
//
//				// 从requestinfos去除分配好的虚拟机,这里需要修改，如何删除不连续的add请求
//				auto s = vmsOnserverTemp[(*first).second]; // 记录凑好的虚拟机
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
//			}  // 一天所有请求分配结束
//		}
//	}
//}
//long long CPU = 0, Memory = 0;
//void Expansion(int day, vector<unordered_map<string, ServerInfo>> serverinfos, vector<vector<list<vector<string>>>> requestinfos_patch) {
//
//	unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverBestmatch; // 记录购买的服务器和凑出的虚拟机匹配信息 <服务器型号，<虚拟机Id，vm内存，vmCpu,vmTwoNodes>>
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
//	requestinfos_patch_buff.clear(); // 清除一天的请求信息
//	PatchRequestinfos.clear();
//	//PatchServerinfos.clear();
//
//	// 输出购买类型数量信息
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
//	if (ServerTypeNum.size() != 0) {   // 购买数不为0时输出
//		string serverType = (*ServerIdOnVms_type.begin()).first;
//		multimap<string, ServerInfo>::iterator sSr = sysServerResourceTemp.begin();
//		for (auto sov : ServerIdOnVms_type) {
//
//			// 记录服务器上运行了那些虚拟机
//			ServerIdOnVms[serverId] = sov.second;
//
//			for (auto ss : sov.second) {
//				if (ss.second.size() > 3) {   // 双节点
//					VmIdOnServer[ss.first].push_back(serverId);
//					VmIdOnServer[ss.first].push_back(ss.second[0]);
//					VmIdOnServer[ss.first].push_back(ss.second[1]);
//					VmIdOnServer[ss.first].push_back(ss.second[2]);
//					VmIdOnServer[ss.first].push_back(ss.second[3]);
//
//					ServerRunVmsNum[serverId]++;  // 记录该ID服务器上运行了多少台vm
//				}
//				else {      // 单节点
//					VmIdOnServer[ss.first].push_back(serverId);
//					VmIdOnServer[ss.first].push_back(ss.second[0]);
//					VmIdOnServer[ss.first].push_back(ss.second[1]);
//					VmIdOnServer[ss.first].push_back(ss.second[2]);
//
//					ServerRunVmsNum[serverId]++;  // 记录该ID服务器上运行了多少台vm
//				}
//			}
//			// 记录服务器现余资源（全局）
//			existServerResource[serverId] = (*sSr).second;
//			sSr++;
//			// for test
//			CPU += existServerResource[serverId].CpuCores_A + existServerResource[serverId].CpuCores_B;
//			Memory += existServerResource[serverId].MemerySize_A + existServerResource[serverId].MemerySize_B;
//			if (testExpansion.is_open()) {
//				testExpansion << "服务器ID: " + to_string(serverId) + "   剩余资源: " + "   CPU_A:" + to_string(existServerResource[serverId].CpuCores_A)
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
//		// 输出购买服务器信息
//		s = "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
//		res[day].push_back(s);
//
//#ifdef TEST
//		cout << "(" + serverType + ", " + to_string(ServertypeNum) + ")\n";
//#endif	
//		//ServerBuyNum.clear();
//	}
//
//	//接下来输出迁移列表，列表是上一天的，不是这一天
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
//// ----------------------------------------------------------------------虚拟机分配策略-------------------------------------------------------------------------------------------- //
//vector<unordered_map<int, ServerInfo>> PatchExistServerResource;
//vector<multimap<int, int>> patchExistServerIdx;  // key: 服务器资源存量，value: 服务器id
//vector<vector<list<vector<string>>>>PatchRequestinfosLeft;
//
//void GetExsitServerResourcePatch(unordered_map<int, ServerInfo> existserverresource, unordered_map<string, VmInfo> vminfos, list<vector<string>> requestinfos) {
//
//	vector<list<vector<string>>> requestinfos_buff;  // 切分的add请求buffer
//
//	int ServerResource;  // 现存每台服务器资源总和
//	multiset<int> VmResourceMin_1; // add请求的内核比小于1的最小vm资源总和
//	multiset<int> VmResourceMin_2; // add请求的内核比大于1的最小vm资源总和
//
//	//unordered_map<int, ServerInfo> existServerResourcePrune;  // 对现存资源进行剪枝，当最小的虚拟机请求的资源量大于系统现存服务器上资源量时，把这台服务器从列表中除去，不参加循环
//
//	float CPU_to_Memory;
//
//	// 将虚拟机请求根据请求数量大小进行切分，并按照内核比分为两类，并按照资源大小降序排列
//	int buffer_length = requestinfos.size() / buffer_num;
//	if (requestinfos.size() < QuestRange) {   // 如果数据小于QuestRange，就不用切分
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
//		multimap<float, multimap<int, vector<string>>>     vminfostmp;  // 内核比->(vmCpu+内存资源， vm信息)
//		multimap<int, vector<string>> vmInfosSort;  // 只存一个
//
//		for (auto re : requestinfos_buff[i]) {
//			if (re.size() > 2) {
//				CPU_to_Memory = (float)(vminfos[re[1]].CpuCores) / (float)(vminfos[re[1]].MemerySize);  // 计算该vm请求的内核比
//				vmCpuAddMemory = vminfos[re[1]].CpuCores + vminfos[re[1]].MemerySize;                   // 计算该vm所需资源数（cpu+内存）
//
//				vmInfosSort.insert(make_pair(vmCpuAddMemory, re));
//				vminfostmp.insert(make_pair(CPU_to_Memory, vmInfosSort));                                       // 按照内核比升序排列
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
//			// 取虚拟机数据
//			int vmcpu = VmInfos[vm.second[1]].VmTwoNodes ? VmInfos[vm.second[1]].CpuCores / 2 : VmInfos[vm.second[1]].CpuCores;
//			int vmmemo = VmInfos[vm.second[1]].VmTwoNodes ? VmInfos[vm.second[1]].MemerySize / 2 : VmInfos[vm.second[1]].MemerySize;
//			//divideResource[i][0][vmcpu][vmmemo].push_back(vm.second);
//		}
//		for (auto vm : vmInfosSort_2) {
//			PatchRequestinfosLeft[i][1].push_back(vm.second);
//
//			addRequestIterMap[vm.second[2]] = prev(PatchRequestinfosLeft[i][1].end(), 1);  // 存迭代器（虽然这种行为有点傻）
//			// 取虚拟机数据
//			int vmcpu = VmInfos[vm.second[1]].VmTwoNodes ? VmInfos[vm.second[1]].CpuCores / 2 : VmInfos[vm.second[1]].CpuCores;
//			int vmmemo = VmInfos[vm.second[1]].VmTwoNodes ? VmInfos[vm.second[1]].MemerySize / 2 : VmInfos[vm.second[1]].MemerySize;
//			//divideResource[i][0][vmcpu][vmmemo].push_back(vm.second);
//		}
//		// 取出虚拟机请求的最小资源
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
//	//对剩余服务器上的资源分类，暂时只按照内核比分为两类
//	multimap<float, int> exsitServerResourceTmp;
//	for (auto& s : existserverresource) {
//		CPU_to_Memory = (float)(s.second.CpuCores_A + s.second.CpuCores_B) / (float)(s.second.MemerySize_A + s.second.MemerySize_B);
//
//		exsitServerResourceTmp.insert(make_pair(CPU_to_Memory, s.first));
//	}
//	// 剩余服务器资源只分成2类，内核比>1和<1
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
//					if (vmTwoNodes == 0 || (vmTwoNodes && vmCpu / 2 <= sCPUA && vmCpu / 2 <= sCPUB && vmMemo / 2 <= sMemoA && vmMemo / 2 <= sMemoB)) {  // 单节点的虚拟机都要放进去
//						vmList.push_front(*vmIt);  // 从大到小放虚拟机
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
//						if ((vmTwoNodes == 0 || (vmTwoNodes && vmCpu / 2 <= sCPUA && vmCpu / 2 <= sCPUB && vmMemo / 2 <= sMemoA && vmMemo / 2 <= sMemoB)) && (*vmIt)[2] != (*subVmIt)[2]) {  // 单节点的虚拟机都要放进去
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
//				if (vmTwoNodes == 0 || (vmTwoNodes && vmCpu / 2 <= sCPUA && vmCpu / 2 <= sCPUB && vmMemo / 2 <= sMemoA && vmMemo / 2 <= sMemoB)) {  // 单节点的虚拟机都要放进去
//					vmList.push_front(*vmIt);
//				}
//			}
//		}
//	}
//}
//
//multimap<int, unordered_map<string, pair< vector<int>, list<vector<string>>::iterator >>> ServerIdOnVms_alloc;  // 通过服务器Id来记录分配在该类型服务器上的vm，
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
//			unordered_map<int, int> unusedServer;  // 这一轮不用参与计算的服务器id
//
//			map<int, map<int, list<vector<string>>>> divideResource;  // 把虚拟机的CPU和MEMORY分开，<cpu, <memory, vminfoList> >
//			unordered_map<string, list<vector<string>>::iterator> divideRequestIterMap;  // 存迭代器，便于删除
//			for (auto& s : PatchRequestinfosLeft[buffNo][patch]) {
//				int vmcpu = VmInfos[s[1]].VmTwoNodes ? VmInfos[s[1]].CpuCores / 2 : VmInfos[s[1]].CpuCores;
//				int vmmemo = VmInfos[s[1]].VmTwoNodes ? VmInfos[s[1]].MemerySize / 2 : VmInfos[s[1]].MemerySize;
//				divideResource[vmcpu][vmmemo].push_back(s);
//				divideRequestIterMap.insert(make_pair(s[2], prev(divideResource[vmcpu][vmmemo].end(), 1)));
//			}
//
//			for (auto it = patchExistServerIdx[patch].begin(), nextIt = it; it != patchExistServerIdx[patch].end(); it = nextIt) {  // 按服务器资源从小到大来计算
//				++nextIt;
//				auto& s = PatchExistServerResource[patch][it->second];
//				if (s.CpuCores_A + s.CpuCores_B + s.MemerySize_A + s.MemerySize_B < 120) {
//					vector<string> oneVm;
//					allocOneServer(divideResource, oneVm, s.CpuCores_A, s.MemerySize_A, s.CpuCores_B, s.MemerySize_B);
//
//					if (!oneVm.empty()) {  // 把唯一的虚拟机分配给唯一的服务器
//						int vvid = stoi(oneVm[2]);
//						unordered_map<string, pair<vector<int>, list<vector<string>>::iterator>> temp;
//						vector<int> vminfoAdd = { VmInfos[oneVm[1]].CpuCores, VmInfos[oneVm[1]].MemerySize };
//						if (VmInfos[oneVm[1]].VmTwoNodes) {
//							vminfoAdd.push_back(1);
//							vminfoAdd.push_back(2);
//						}
//						else {
//							// 重新计算一下放在哪个节点比较好
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
//						// 测试用
//						patchExistServerIdx[patch].erase(it);
//					}
//				}
//			}
//
//			while (PatchRequestinfosLeft[buffNo][patch].size() > 0) {    // 如果请求信息还有一直循环
//
//				unordered_map<int, unordered_map<string, pair< vector<int>, list<vector<string>>::iterator >>> vmsOnserverTemp; // 记录服务器和凑出的虚拟机匹配信息 <服务器id，<虚拟机Id，vm内存，vmCpu,vmTwoNodes>>
//
//				multimap<int, int> Matching_degree;  // 记录凑出的服务器的匹配程度差值，取最小 <差值，服务器ID>
//				unordered_map<int, ServerInfo> ServerResourceTemp; // < 服务器Id, 服务器资源>
//
//				int i = 0, priorCA = -1, priorMA = -1, priorCB = -1, priorMB = -1, priorId = -1, priorTotal = -1;
//				for (auto it = patchExistServerIdx[patch].begin(); it != patchExistServerIdx[patch].end(); ++it, ++i) {  // 按服务器资源从小到大来计算
//				//for (auto &s : PatchExistServerResource[patch]) {    // 和所有服务器进行匹配，优化时间可以考虑把每次分配过的去除
//					// 丢掉后面的一些服务器，因为那些服务器被选上的概率很小
//					//if (i == patchExistServerIdx[patch].size() - floor(1.0 / ALLOC_DISCARD_RATIO * patchExistServerIdx[patch].size()))
//					//	break;
//
//					if (unusedServer.find(it->second) != unusedServer.end()) {  // 这台服务器是不可能配到合适的虚拟机的，跳过这一次循环
//						continue;
//					}
//
//					auto& s = PatchExistServerResource[patch][it->second];
//					int taget_tatal = s.CpuCores_A + s.CpuCores_B + s.MemerySize_A + s.MemerySize_B;  //初始化服务器和凑到的虚拟机资源差值
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
//						continue;  // 这一台服务器剩余资源和上一台一模一样，那其实不用再去遍历所有的请求了，复制上一台服务器的信息进去即可
//					}
//					else {
//						priorCA = s.CpuCores_A, priorMA = s.MemerySize_A, priorCB = s.CpuCores_B, priorMB = s.MemerySize_B;
//						priorId = it->second, priorTotal = taget_tatal;
//					}
//
//					list<vector<string>> vmList;
//					list<vector<string>>::iterator itStart, itEnd;
//					if (s.CpuCores_A + s.CpuCores_B + s.MemerySize_A + s.MemerySize_B < 0) {
//						// 新建一个虚拟机列表，这一台服务器只能放下这些虚拟机
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
//					//for (list<vector<string>>::iterator vm = PatchRequestinfosLeft[buffNo][patch].begin(); vm != PatchRequestinfosLeft[buffNo][patch].end(); ++vm) { // 遍历所有请求
//					for (list<vector<string>>::iterator vmIt = itStart, nextIt = vmIt; vmIt != itEnd; vmIt = nextIt) {
//						++nextIt;
//						auto vm = addRequestIterMap[(*vmIt)[2]];
//
//						int vmCores = VmInfos[(*vm)[1]].CpuCores;
//						int vmMemory = VmInfos[(*vm)[1]].MemerySize;
//						int vmTwoNodes = VmInfos[(*vm)[1]].VmTwoNodes;
//
//						// 如果这台虚拟机的容量要求比现有服务器的最大容量还大，那就先删掉并保存，最后再合并回来
//						if (vmCores + vmMemory > prev(patchExistServerIdx[patch].end(), 1)->first) { // 能进来的肯定不是削过的列表
//							discardRequest.push_back(*vm);
//							PatchRequestinfosLeft[buffNo][patch].erase(vm);
//							continue;
//						}
//
//						vector<int> vminfoAdd;
//
//						if (vmTwoNodes) {    // 双节点vm
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
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值
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
//						else {  // 单节点
//							vminfoAdd.resize(3);
//							if (taget_CpuCores_A >= vmCores && taget_MemerySize_A >= vmMemory) {
//
//								taget_CpuCores_A -= vmCores;
//								taget_MemerySize_A -= vmMemory;
//
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值
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
//								taget_tatal = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值
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
//					} // 一台服务器凑数结束
//					Matching_degree.insert(make_pair(taget_tatal, it->second));   // 记录服务器与虚拟机匹配的差值
//
//					// 记录这台服务器上的资源剩余分配情况
//					ServerResourceTemp[it->second].CpuCores_A = taget_CpuCores_A;
//					ServerResourceTemp[it->second].CpuCores_B = taget_CpuCores_B;
//					ServerResourceTemp[it->second].MemerySize_A = taget_MemerySize_A;
//					ServerResourceTemp[it->second].MemerySize_B = taget_MemerySize_B;
//					//ServerResourceTemp[s.first].PowerCost = ServerInfos[s.first].PowerCost;
//
//				}  // 所有服务器一次循环结束
//
//				//选出最匹配的所有服务器
//
//				// 由于差值最小的服务器可能却不能满足分配要求，所以需要删除
//				for (multimap<int, int>::iterator it = Matching_degree.begin(); it != Matching_degree.end(); ) {
//					if (vmsOnserverTemp[(*it).second].size() == 0) {
//						// 这么多虚拟机在列表里这台服务器都匹配不下，随着虚拟机越来越少就更加没戏了，把这些id记录下来，再也不进入计算
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
//				//	for (list<vector<string>>::iterator vm = PatchRequestinfosLeft[buffNo][patch].begin(); vm != PatchRequestinfosLeft[buffNo][patch].end(); ++vm) { // 遍历所有请求
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
//				auto first = Matching_degree.begin();  // 此处会存在差值相同的情况，可以考虑如何选最优
//
//				ServerIdOnVms_alloc.insert(make_pair((*first).second, vmsOnserverTemp[(*first).second]));
//
//				//PatchExistServerResource[patch][(*first).second] = ServerResourceTemp[(*first).second];
//				PatchExistServerResource[patch].erase((*first).second);
//				// 测试用
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
//				// 从PatchRequestinfos去除分配好的虚拟机
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
//			}  // 请求分配结束
//			//// 把删掉的虚拟机加回来，加到头部就行
//			PatchRequestinfosLeft[buffNo][patch].insert(PatchRequestinfosLeft[buffNo][patch].begin(), discardRequest.begin(), discardRequest.end());
//		} // 内核比循环结束
//	}  // 数据切分循环结束
//
//
//	//clock_t e = clock();
//	//fstream f;
//	//f.open("singlethread.txt", ios::out | ios::app);
//	//f << e - s << endl;
//	//f.close();
//
//	// 记录分配好的虚拟机
//	for (auto sov : ServerIdOnVms_alloc) {
//		for (auto ss : sov.second) {
//			if (ss.second.first.size() > 3) {   // 双节点
//
//				VmIdOnServer[ss.first].push_back(sov.first);
//				VmIdOnServer[ss.first].push_back(ss.second.first[0]);
//				VmIdOnServer[ss.first].push_back(ss.second.first[1]);
//				VmIdOnServer[ss.first].push_back(ss.second.first[2]);
//				VmIdOnServer[ss.first].push_back(ss.second.first[3]);
//
//				ServerRunVmsNum[sov.first]++;  // 记录该ID服务器上运行了多少台vm	
//			}
//			else {      // 单节点
//				VmIdOnServer[ss.first].push_back(sov.first);
//				VmIdOnServer[ss.first].push_back(ss.second.first[0]);
//				VmIdOnServer[ss.first].push_back(ss.second.first[1]);
//				VmIdOnServer[ss.first].push_back(ss.second.first[2]);
//
//				ServerRunVmsNum[sov.first]++;  // 记录该ID服务器上运行了多少台vm
//			}
//			// 记录服务器上运行了那些虚拟机
//			ServerIdOnVms[sov.first].insert(make_pair(ss.first, ss.second.first));
//		}
//	}
//	ServerIdOnVms_alloc.clear();
//}
//
//// ----------------------------------------------------------------预处理add请求------------------------------------------- //
////int DealAddVm(vector<string> createVmInfo, vector<string>& res_tmp) {
////	string _reqVmType = createVmInfo[1], _reqId = createVmInfo[2];
////	auto  vm = VmInfos[_reqVmType];
////	// 那就试一下精准分配虚拟机，找出差值最小的服务器
////	int minServerId = -1, minVal = INT32_MAX, node = -1;
////	int vmCores = vm.CpuCores, vmMemo = vm.MemerySize, vmNode = vm.VmTwoNodes;
////	if (vmNode == 0) {  // 单节点部署
////		for (int i = 0; i < serverId; ++i) {
////			// 找出和哪台服务器的哪个节点差值最小
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
////	else {  // 双节点部署
////		for (int i = 0; i < serverId; ++i) {
////			// 找出和哪台服务器的差值最小
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
////	// 部署进去，要改成分AB节点的了
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
//// ----------------------------------处理每一个add请求----------------------------------------------------------------------//
//int ServerIdAdd = 0; // 每天增加的服务器数量
//int ReDealAddVM(vector<string> request, vector<string>& res_tmp, int day);
//int ReDealAddVM(vector<string> request, vector<string>& res_tmp, int day) {
//	string s = "(";
//	if (VmIdOnServer.find(request[2]) != VmIdOnServer.end()) {
//		vector<int> serverInfo = VmIdOnServer[request[2]];
//		if (serverInfo.size() == 5) {  // 双节点部署
//			s += to_string(serverInfo[0]) + ")\n";
//		}
//		else {  // 单节点部署
//			if (serverInfo[3] == 1)
//				s += to_string(serverInfo[0]) + ", A)\n";		  // 分配到A节点			
//			else
//				s += to_string(serverInfo[0]) + ", B)\n";          // 分配到B节点	
//		}
//		res_tmp.push_back(s);
//		return 1;
//	}
//	return -1;
//}
//
//// ---------------------------------------------------------------迁移虚拟机 --------------------------------------------------------------------//
//int MigrationNum;
//
//void Migrate(int day, list<vector<string>>& requestinfos) {
//	// 统计请求数量中add和dele的比值
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
//		multimap<int, int, greater<int>> ServerRunVmsNumTmp; // vm数量，服务器ID
//		int vmSum = 0; // 虚拟机总数
//		int moveNum = 0; // 迁移虚拟机数量
//		vector<string> res_tmp; // 迁移过程结果暂存
//		for (auto& s : existServerResource) {
//			int totalRemain = s.second.CpuCores_A + s.second.CpuCores_B + s.second.MemerySize_A + s.second.MemerySize_B;
//			ServerRunVmsNumTmp.insert(make_pair(totalRemain, s.first));
//			vmSum += ServerRunVmsNum[s.first];
//		}
//		int i_head = 0; // 跟着head同步走
//		for (auto head = ServerRunVmsNumTmp.begin(); head != ServerRunVmsNumTmp.end(); ++head, ++i_head) {
//
//			auto& SourceServer = existServerResource[(*head).second];
//			int& SourceCoreA = SourceServer.CpuCores_A,
//				& SourceCoreB = SourceServer.CpuCores_B,
//				& SourceMemoryA = SourceServer.MemerySize_A,
//				& SourceMemoryB = SourceServer.MemerySize_B;
//
//			int maxAttempTimes = ServerRunVmsNumTmp.size() - i_head - 1, i_tail = ServerRunVmsNumTmp.size() - 1;  // 每台虚拟机的最大尝试次数
//			for (auto sov = ServerIdOnVms[head->second].begin(); sov != ServerIdOnVms[head->second].end() && moveNum < vmSum / MIGRATION_LIMT;) {
//
//				int vmCpu = sov->second[0], vmMemo = sov->second[1];
//				if (sov->second.size() > 3) {   // 双节点部署
//
//					int minTailServerId = head->second,
//						minTailServerVal = existServerResource[head->second].CpuCores_A + existServerResource[head->second].CpuCores_B
//						+ existServerResource[head->second].MemerySize_A + existServerResource[head->second].MemerySize_B;
//
//					int noFindTimes = 0, findTimes = 0, totalTimes = 0;
//					//for (auto tail = prev(ServerRunVmsNumTmp.end(), 1); tail != head; --tail) {  // 找出差值最小的服务器ID
//					for (auto tail = next(head, 1); tail != ServerRunVmsNumTmp.end(); ++tail) {  // 找出差值最小的服务器ID
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
//						if (minTailServerId == head->second) {  // -1表示还没找到合适的服务器，那么极有可能再也找不到了
//							++noFindTimes;
//							if ((float)noFindTimes / maxAttempTimes > MIGRATE_FAIL_RATIO) {  // -1到了一定的次数后就可以跳出了，这个值可以设小一点
//								break;
//							}
//						}
//						else {  // 已经找到服务器了，只是可能还没找到最合适的服务器
//							if ((float)totalTimes / maxAttempTimes > MIGRATE_SUCCESS_RATIO) {
//								break;
//							}
//						}
//
//					}
//
//					if (minTailServerId == head->second) {  // 别说差值最小的服务器了，连放得下的服务器都没有，就跳过这一台
//						++sov;
//						continue;
//					}
//					else {  // 有可以放得下的服务器
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
//						// 目的服务器剩余资源更新
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
//						// 更新虚拟机运行在哪个服务器上（双节点部署）
//						VmIdOnServer[(*sov).first] = vector<int>{ minTailServerId ,vmCpu,vmMemo,1,2 };
//
//						res_tmp.push_back("(" + (*sov).first + ", " + to_string(minTailServerId) + ")\n");			//输出
//
//						moveNum++;
//						ServerIdOnVms[minTailServerId].insert(*sov);           // 将此虚拟机放入新位置
//						sov = ServerIdOnVms[(*head).second].erase(sov);       // 将此虚拟机从原有位置删除
//
//						ServerRunVmsNum[minTailServerId]++;                   //  目的服务器ID上的虚拟机数量加1
//						ServerRunVmsNum[(*head).second]--;                   //  原服务器ID上的虚拟机数量减1
//
//						assert(ServerRunVmsNum[(*head).second] >= 0);
//					}
//
//				}
//				else {  // 单节点部署
//
//					pair<int, int> minTailServerId(head->second, 0);  // <id ,A/B节点>
//					int minTailServerVal = (sov->second[2] == 1 ? existServerResource[head->second].CpuCores_A + existServerResource[head->second].MemerySize_A :
//						existServerResource[head->second].CpuCores_B + existServerResource[head->second].MemerySize_B);
//					int noFindTimes = 0, totalTimes = 0;
//					//for (auto tail = prev(ServerRunVmsNumTmp.end(), 1); tail != head; --tail) {  // 找出差值最小的服务器ID
//					for (auto tail = next(head, 1); tail != ServerRunVmsNumTmp.end(); ++tail) {  // 找出差值最小的服务器ID
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
//						if (minTailServerId.first == head->second) {  // -1表示还没找到合适的服务器，那么极有可能再也找不到了
//							++noFindTimes;
//							if ((float)noFindTimes / maxAttempTimes > MIGRATE_FAIL_RATIO) {   // -1到了一定的次数后就可以跳出了，这个值可以设小一点
//								break;
//							}
//						}
//						else {  // 已经找到服务器了，只是可能还没找到最合适的服务器
//							if ((float)totalTimes / maxAttempTimes > MIGRATE_SUCCESS_RATIO) {
//								break;
//							}
//						}
//					}
//
//					if (minTailServerId.first == head->second) {  // 别说差值最小的服务器了，连放得下的服务器都没有，就跳过这一台
//						++sov;
//						continue;
//					}
//					else {   // 放得下
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
//						// 目的服务器剩余资源更新
//						if (minTailServerId.second == 1) {
//							DestinationCoreA -= vmCpu;
//							DestinationMemoryA -= vmMemo;
//						}
//						else if (minTailServerId.second == 2) {
//							DestinationCoreB -= vmCpu;
//							DestinationMemoryB -= vmMemo;
//						}
//
//						//源服务器剩余资源更新
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
//						// 更新虚拟机运行在哪个服务器上
//						if (minTailServerId.second == 1) {
//							VmIdOnServer[(*sov).first] = vector<int>{ minTailServerId.first,vmCpu,vmMemo,1 };
//							res_tmp.push_back("(" + (*sov).first + ", " + to_string(minTailServerId.first) + ", A)\n");			//输出
//							(*sov).second[2] = 1;
//						}
//						else {
//							VmIdOnServer[(*sov).first] = vector<int>{ minTailServerId.first,vmCpu,vmMemo,2 };
//							res_tmp.push_back("(" + (*sov).first + ", " + to_string(minTailServerId.first) + ", B)\n");			//输出
//							(*sov).second[2] = 2;
//						}
//
//						moveNum++;
//
//						ServerIdOnVms[minTailServerId.first].insert(*sov);           // 将此虚拟机放入新位置
//						sov = ServerIdOnVms[(*head).second].erase(sov);       // 将此虚拟机从原有位置删除
//						ServerRunVmsNum[minTailServerId.first]++;                   //  目的服务器ID上的虚拟机数量加1
//						ServerRunVmsNum[(*head).second]--;                   //  原服务器ID上的虚拟机数量减1
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
//// ---------------------------------------------------------------调度匹配 --------------------------------------------------------------------- //
//void Schedule(int day, unordered_map<string, ServerInfo> serverinfos, list<vector<string>> requestinfos) {
//
//	vector<string> res_tmp;
//
//	if (day <= DAY)  // 第一天我们全买，后面也可以调整day的大小
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
//		// 合并连两个add请求处理的输出
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
//// 服务器电费计算
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
//	int ServerTypesNum;    // 提供的服务器类型数量
//	string serverType = "", cpuCores = "", memorySize = "", serverCost = "", powerCost = "";
//	scanf("%d", &ServerTypesNum);
//
//	for (int i = 0; i < ServerTypesNum; i++) {
//		cin >> serverType >> cpuCores >> memorySize >> serverCost >> powerCost;  // 读入一个类型的服务器信息
//		GetServerInfos(serverType, cpuCores, memorySize, serverCost, powerCost); // 解析服务器信息，并保存
//	}
//
//	int VmTypesNum = 0;    // 提供的虚拟机类型数量
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
//				GetRequest(buff, reqestVmType, reqestId);    //解析一天中增加虚拟机请求
//			}
//			else {
//				cin >> reqestId;
//				GetRequest(buff, reqestId);              //解析一天中删除虚拟机请求
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
//	// 输出还没输出的信息
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
//	TOTAL_COST = SERVER_COST + POWER_COST;  //计算总费用
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
