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

// #define UPLOAD

// 结构体存储各种信息

// 单个服务器信息结构体
struct ServerInfo {

	int CpuCores_A;    // A节点cpu资源数
	int CpuCores_B;    // B节点cpu资源数

	int MemerySize_A;  // A节点内存资源数
	int MemerySize_B;  // B节点内存资源数

	int ServerCost;    // 服务器花费
	int PowerCost;     // 电费

	int ServerId;      // 服务器Id
};

// 单个虚拟机信息结构体
struct VmInfo {

	int CpuCores;    // 消耗的cpu资源数

	int MemerySize;  // 消耗的内存资源数

	int VmTwoNodes;  // 单/双节点部署

	string VmId;     // 虚拟机Id
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

#ifdef TEST
const string filePath = "../dataset/training-1.txt";
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
bool AllocServerResources(ServerInfo& server, VmInfo& vm, int serverId, string vmId, vector<string> &res_tmp, int res_tmp_idx) {

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


#ifdef TEST
			//cout << "(" << serverId << ")" << endl;
#endif
			//res.push_back("(" + to_string(serverId) + ")\n");

			// res_tmp.push_back("(" + to_string(serverId) + ")\n");
			res_tmp[res_tmp_idx] = "(" + to_string(serverId) + ")\n";
			return true;
		}
		else {
			return false;                                               //资源不足，在此服务器上分配该虚拟机失败
		}
	}
	else if (serverCoreA >= vmCores && serverMemoryA >= vmMemory) {     //单节点部署检查A节点是否满足分配条件

		serverCoreA -= vmCores;
		serverMemoryA -= vmMemory;

		// 记录虚拟机运行在哪个服务器上（A节点部署）
		VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1 };

#ifdef TEST
		//cout << "(" << serverId << ", A)" << endl;
#endif
		// res_tmp.push_back("(" + to_string(serverId) + ", A)\n");
		res_tmp[res_tmp_idx] = "(" + to_string(serverId) + ", A)\n";
		return true;
	}
	else if (serverCoreB >= vmCores && serverMemoryB >= vmMemory) {    //单节点部署检查B节点是否满足分配条件

		serverCoreB -= vmCores;
		serverMemoryB -= vmMemory;

		// 记录虚拟机运行在哪个服务器上（B节点部署）
		VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,2 };

#ifdef TEST
		//cout << "(" << serverId << ", B)" << endl;
#endif
		// res_tmp.push_back("(" + to_string(serverId) + ", B)\n");
		res_tmp[res_tmp_idx] = "(" + to_string(serverId) + ", B)\n";
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

	int test = serverRunVms[_serverId];
	assert(serverRunVms[_serverId] >= 0);

}


multimap<float, string> PerformanceAndCostRatio;  //有序容器

void GetBestServer(int day, unordered_map<string, ServerInfo> serverinfos) {


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


		PerformanceAndCostRatio.insert(make_pair((ServerCost + day * PowerCost) / (Cpu + Memory), serverType));  //根据key值可以自动排序
	}

}


int serverId = 0;  // 服务器ID(全局)
multimap<string, unordered_map<string, vector<int>>> ServerOnVm_type;  // 通过服务器类型来记录在该类型服务器上的vm，
vector<string>  ServerType;  // 按顺序存放每天选出的最优服务器类型名称，方便输出时按照此顺序找到对应服务器ID

// 扩容服务器 + 分配
void Expansion(int day, unordered_map<string, ServerInfo> serverinfos, vector<vector<string>> requestinfos) {


	//GetBestServer(day, serverinfos);  //计算性价比最高的服务器

	unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverBestmatch; // 记录购买的服务器和凑出的虚拟机匹配信息 <服务器型号，<虚拟机Id，vm内存，vmCpu,vmTwoNodes>>

	unordered_map<string, int> ServerBuyNum;

	set<string> ServerTypeNum;

	multimap<string, ServerInfo> sysServerResourceTemp;
	vector<vector<string>> requestinfosAdd;

	// 把所有add请求拿出来
	for (auto re : requestinfos) {
		if (re.size() > 2) {
			requestinfosAdd.push_back(re);
		}
	}

	while (requestinfosAdd.size() > 0) {    // 如果请求信息还有一直循环

		unordered_map<string, unordered_map<string, vector<int>>> vmsOnserverTemp; // 记录服务器和凑出的虚拟机匹配信息 <服务器型号，<虚拟机Id，vm内存，vmCpu,vmTwoNodes>>
		multimap<int, string> Matching_degree;  // 记录凑出的服务器的匹配程度差值，取最小
		unordered_map<string, ServerInfo> ServerResourceTemp;

		for (auto s : serverinfos) {    // 和所有服务器进行匹配

			int taget_tatal = s.second.CpuCores_A + s.second.CpuCores_B + s.second.MemerySize_A + s.second.MemerySize_B;  //初始化服务器和凑到的虚拟机资源差值
			int taget_CpuCores_A = s.second.CpuCores_A, taget_CpuCores_B = s.second.CpuCores_B, taget_MemerySize_A = s.second.MemerySize_A, taget_MemerySize_B = s.second.MemerySize_B;
			bool flag = false;


			for (int i = 0; i < requestinfosAdd.size(); i++) {
				if (requestinfosAdd[i].size() > 2) {   //在所有add请求中凑数

					int vmCores = VmInfos[requestinfosAdd[i][1]].CpuCores;
					int vmMemory = VmInfos[requestinfosAdd[i][1]].MemerySize;
					int vmTwoNodes = VmInfos[requestinfosAdd[i][1]].VmTwoNodes;
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
							vmsOnserverTemp[s.first].insert(make_pair(requestinfosAdd[i][2], vminfoAdd));
							vminfoAdd.clear();
							flag = true;

							if (i == requestinfosAdd.size() - 1)
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

							vmsOnserverTemp[s.first].insert(make_pair(requestinfosAdd[i][2], vminfoAdd));
							vminfoAdd.clear();


							if (i == requestinfosAdd.size() - 1)
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

							vmsOnserverTemp[s.first].insert(make_pair(requestinfosAdd[i][2], vminfoAdd));
							vminfoAdd.clear();


							if (i == requestinfosAdd.size() - 1)
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
		multimap<int, string>::iterator first = Matching_degree.begin();
		for (multimap<int, string>::iterator it = Matching_degree.begin(); it != Matching_degree.end(); ) {
			if (vmsOnserverTemp[(*it).second].size() == 0) {
				it++;
			}
			else
			{
				first = it; break;
			}
		}

		//ServerType.push_back((*first).second);

		ServerTypeNum.insert((*first).second); // 记录一次买多少种服务器，用于输出

		ServerOnVm_type.insert(make_pair((*first).second, vmsOnserverTemp[(*first).second]));   //相同的服务器类型会排列在一起，组合成购买的输出，按顺序排serverID

		// 将购买的服务器及其剩余的资源数存入系统现在占有资源情况(此处用multimap按照服务器名称存入，是为了与ServerOnVm_type中的存的服务器顺序保持一致，后面再将其转换成id保存)
		sysServerResourceTemp.insert(make_pair((*first).second, ServerResourceTemp[(*first).second]));


		SERVERCOST += ServerInfos[(*first).second].ServerCost;  // 购买服务器费用累加计算

		// 从requestinfos去除分配好的虚拟机		
		requestinfosAdd.erase(requestinfosAdd.begin(), requestinfosAdd.begin() + vmsOnserverTemp[(*first).second].size());
		Matching_degree.clear();


	}  // 一天所有请求分配结束


	// 输出购买类型数量信息
	string s = "(purchase, ";

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

		ServerBuyNum.clear();
	}
	

}


// // 此函数用来
// int DealAddVm(vector<string> createVmInfo, vector<string>& res_tmp);
// int DealAddVm(vector<string> createVmInfo, vector<string>& res_tmp) {
// 	string _reqVmType = createVmInfo[1], _reqId = createVmInfo[2];
// 	auto  vm = VmInfos[_reqVmType];
	
// 	// // 试一下排序法，效果还不如不排序
// 	// vector<pair<int, ServerInfo>> sysServerResourceVec;
// 	// for (auto it = sysServerResource.begin(); it != sysServerResource.end(); ++it) {
// 	// 	sysServerResourceVec.push_back(make_pair(it->first, it->second));
// 	// }
// 	// sort(sysServerResourceVec.begin(), sysServerResourceVec.end(), [](pair<int, ServerInfo>& a, pair<int, ServerInfo>& b) {
// 	// 	if (a.second.CpuCores_A + a.second.CpuCores_B + a.second.MemerySize_A + a.second.MemerySize_B != b.second.CpuCores_A + b.second.CpuCores_B + b.second.MemerySize_A + b.second.MemerySize_B)
// 	// 		return a.second.CpuCores_A + a.second.CpuCores_B + a.second.MemerySize_A + a.second.MemerySize_B < b.second.CpuCores_A + b.second.CpuCores_B + b.second.MemerySize_A + b.second.MemerySize_B;
// 	// 	else
// 	// 		return a.second.ServerCost < b.second.ServerCost;
// 	// });
// 	// int success = -1;
// 	// for (int i = 0; i < sysServerResourceVec.size(); i++) {           //这里可以优化， 比如不按照顺序分配资源，将系统现有资源排序后再分配？
// 	//     auto& server = sysServerResource[sysServerResourceVec[i].first];
// 	//     if (AllocServerResources(server, vm, sysServerResourceVec[i].first, _reqId, res_tmp)) {
// 	//         serverRunVms[sysServerResourceVec[i].first]++;
// 	//         success = 1;
// 	//         break;
// 	//     }
// 	//     assert(server.CpuCores_A >= 0 && server.CpuCores_B >= 0 && server.MemerySize_A >= 0 && server.MemerySize_B>= 0);
// 	// }

// 	// return success;
// 	// 那就试一下精准分配虚拟机，找出差值最小的服务器
// 	int minServerId = -1, minVal = INT32_MAX, node = -1;
// 	float minValF = 100000000;
// 	int vmCores = vm.CpuCores, vmMemo = vm.MemerySize, vmNode = vm.VmTwoNodes;
// 	if (vmNode == 0) {  // 单节点部署
// 		for (int i = 0; i < serverId; ++i) {
// 			// 找出和哪台服务器的哪个节点差值最小
// 			int diff1 = sysServerResource[i].CpuCores_A - vmCores, diff2 = sysServerResource[i].MemerySize_A - vmMemo;

// 			if (diff1 >= 0 && diff2 >= 0 && diff1 + diff2 < minVal) {
// 			// if (diff1 >= 0 && diff2 >= 0 && abs((float)diff1 / diff2 - 1) < minValF) {
// 				minServerId = i;
// 				minVal = diff1 + diff2;
// 				// minValF = abs((float)diff1 / diff2 - 1);
// 				node = 1;
// 			}
// 			diff1 = sysServerResource[i].CpuCores_B - vmCores, diff2 = sysServerResource[i].MemerySize_B - vmMemo;
// 			if (diff1 >= 0 && diff2 >= 0 && diff1 + diff2 < minVal) {
// 			// if (diff1 >= 0 && diff2 >= 0 && abs((float)diff1 / diff2 - 1) < minValF) {
// 				minServerId = i;
// 				minVal = diff1 + diff2;
// 				// minValF = abs((float)diff1 / diff2 - 1);
// 				node = 2;
// 			}
// 		}
// 	}
// 	else {  // 双节点部署
// 		for (int i = 0; i < serverId; ++i) {
// 			// 找出和哪台服务器的差值最小
// 			int diff1A = sysServerResource[i].CpuCores_A - vmCores / 2;
// 			int diff2A = sysServerResource[i].MemerySize_A - vmMemo / 2;
// 			int diff1B = sysServerResource[i].CpuCores_B - vmCores / 2;
// 			int diff2B = sysServerResource[i].MemerySize_B - vmMemo / 2;
// 			float ratio1 = abs(float(diff1A) / diff2A), ratio2 = abs(float(diff1B) / diff2B);
// 			float ratio = ratio1 / ratio2;
// 			if (diff1A >= 0 && diff2A >= 0 && diff1B >= 0 && diff2B >= 0 && diff1A + diff2A + diff1B + diff2B < minVal) {
// 			// if (diff1A >= 0 && diff2A >= 0 && diff1B >= 0 && diff2B >= 0 && ratio < minValF) {
// 				minServerId = i;
// 				diff1A + diff2A + diff1B + diff2B < minVal;
// 				// minValF = ratio;
// 				node = 0;
// 			}
// 		}
// 	}
// 	// 部署进去，要改成分AB节点的了
// 	int success = -1;
// 	auto& server = sysServerResource[minServerId];
// 	if (AllocServerResources(server, vm, minServerId, _reqId, res_tmp, node)) {
// 		serverRunVms[minServerId]++;
// 		success = 1;
// 	}
// 	assert(server.CpuCores_A >= 0 && server.CpuCores_B >= 0 && server.MemerySize_A >= 0 && server.MemerySize_B>= 0);
// 	return success;

// 	// int success = -1;
// 	// for (int i = 0; i < serverId; i++) {           //这里可以优化， 比如不按照顺序分配资源，将系统现有资源排序后再分配？
// 	//     auto& server = sysServerResource[i];
// 	//     if (AllocServerResources(server, vm, i, _reqId, res_tmp)) {
// 	//         serverRunVms[i]++;
// 	//         success = 1;
// 	//         break;
// 	//     }
// 	//     assert(server.CpuCores_A >= 0 && server.CpuCores_B >= 0 && server.MemerySize_A >= 0 && server.MemerySize_B>= 0);
// 	// }

// }

// 以同样的方式将新来的虚拟机分配给旧的服务器
int DealAddVm(vector<vector<string>> &requestinfos, vector<string>& res_tmp);
int DealAddVm(vector<vector<string>> &requestinfos, vector<string>& res_tmp) {

	vector<vector<string>> requestinfosAdd;

	int totalRemoved = 0;

	// 把所有add请求拿出来
	for (auto s: requestinfos) {
		if (s.size() > 2)
			requestinfosAdd.push_back(s);
	}

	res_tmp.resize(requestinfosAdd.size(), "null"); // 因为requestinfos顺序乱了，所以就不能用push_back了，先全部初始化为null

	// 做个从小到大的排序，尽可能凑出多台虚拟机，把单双结点的分开排，哪个在前哪个在后得多试试
	// 同时需要记录一下原先requestinfos的位置
	unordered_map<string, int> reqSrcIdx;
	for (int i = 0; i < requestinfosAdd.size(); ++i) {
		reqSrcIdx.insert(make_pair(requestinfosAdd[i][2], i));
	}
	sort(requestinfosAdd.begin(), requestinfosAdd.end(), [=](vector<string>&a, vector<string>&b){
		VmInfo &vm1 = VmInfos[a[1]], &vm2 = VmInfos[b[1]];
		if (vm1.VmTwoNodes != vm2.VmTwoNodes)
			return vm1.VmTwoNodes > vm2.VmTwoNodes;  // 把双节点部署的虚拟机排在前面
			// return vm1.VmTwoNodes < vm2.VmTwoNodes;  // 把单节点部署的虚拟机排在前面
		else if (vm1.CpuCores + vm1.MemerySize != vm2.CpuCores + vm2.MemerySize) 
			return vm1.CpuCores + vm1.MemerySize < vm2.CpuCores + vm2.MemerySize;
		else
			return a[2] < b[2];
	});

	while (requestinfosAdd.size() > 0) {
		unordered_map<int, unordered_map<string, pair<string, vector<int> > > > vmsOnserverTemp; // 记录服务器和凑出的虚拟机匹配信息 <服务器ID，pair<虚拟机类型, <虚拟机Id，vm内存，vmCpu,vmTwoNodes>>>
		multimap<int, int> Matching_degree;  // 记录凑出的服务器的匹配程度差值，取最小

		for (auto s : sysServerResource) {
			int target_total = s.second.CpuCores_A + s.second.CpuCores_B + s.second.MemerySize_A + s.second.MemerySize_B;
			int taget_CpuCores_A = s.second.CpuCores_A, taget_CpuCores_B = s.second.CpuCores_B, taget_MemerySize_A = s.second.MemerySize_A, taget_MemerySize_B = s.second.MemerySize_B;

			for (int i = 0; i <requestinfosAdd.size(); ++i) {
				if (requestinfosAdd[i].size() > 2) {
					int vmCores = VmInfos[requestinfosAdd[i][1]].CpuCores;
					int vmMemory = VmInfos[requestinfosAdd[i][1]].MemerySize;
					int vmTwoNodes = VmInfos[requestinfosAdd[i][1]].VmTwoNodes;
					vector<int> vminfoAdd;
				
					if (vmTwoNodes) {    // 双节点vm
						int needCores = vmCores / 2;
						int needMemory = vmMemory / 2;
						if (taget_CpuCores_A >= needCores && taget_CpuCores_B >= needCores && taget_MemerySize_A >= needMemory && taget_MemerySize_B >= needMemory) {
							taget_CpuCores_A -= needCores;
							taget_CpuCores_B -= needCores;
							taget_MemerySize_A -= needMemory;
							taget_MemerySize_B -= needMemory;

							target_total = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值

							vminfoAdd.push_back(vmCores);
							vminfoAdd.push_back(vmMemory);
							vminfoAdd.push_back(1);
							vminfoAdd.push_back(2);
							vmsOnserverTemp[s.first].insert(make_pair(requestinfosAdd[i][2], make_pair(requestinfosAdd[i][1], vminfoAdd)));
							vminfoAdd.clear();

							if (i == requestinfosAdd.size() - 1)
								Matching_degree.insert(make_pair(target_total, s.first));   // 记录服务器与虚拟机匹配的差值
						}
						else {
							Matching_degree.insert(make_pair(target_total, s.first));   // 记录服务器与虚拟机匹配的差值
							break;
						}
					}
					else {  // 单节点
						if (taget_CpuCores_A >= vmCores && taget_MemerySize_A >= vmMemory) {

							taget_CpuCores_A -= vmCores;
							taget_MemerySize_A -= vmMemory;

							target_total = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值

							vminfoAdd.push_back(vmCores);
							vminfoAdd.push_back(vmMemory);
							vminfoAdd.push_back(1);

							vmsOnserverTemp[s.first].insert(make_pair(requestinfosAdd[i][2], make_pair(requestinfosAdd[i][1], vminfoAdd)));
							vminfoAdd.clear();


							if (i == requestinfosAdd.size() - 1)
								Matching_degree.insert(make_pair(target_total, s.first));   // 记录服务器与虚拟机匹配的差值
						}
						else if (taget_CpuCores_B >= vmCores && taget_MemerySize_B >= vmMemory) {
							taget_CpuCores_B -= vmCores;
							taget_MemerySize_B -= vmMemory;

							target_total = taget_CpuCores_A + taget_CpuCores_B + taget_MemerySize_A + taget_MemerySize_B; // 计算差值

							vminfoAdd.push_back(vmCores);
							vminfoAdd.push_back(vmMemory);
							vminfoAdd.push_back(2);

							vmsOnserverTemp[s.first].insert(make_pair(requestinfosAdd[i][2], make_pair(requestinfosAdd[i][1], vminfoAdd)));
							vminfoAdd.clear();


							if (i == requestinfosAdd.size() - 1)
								Matching_degree.insert(make_pair(target_total, s.first));   // 记录服务器与虚拟机匹配的差值
						}
						else {
							Matching_degree.insert(make_pair(target_total, s.first));   // 记录服务器与虚拟机匹配的差值
							break;
						}
					}
				}
			} // 一台服务器凑数结束
		}  // 所有服务器一次循环结束
		if (Matching_degree.empty())
			return totalRemoved;
		//选出最匹配的所有服务器id

		// 由于差值最小的服务器可能却不能满足分配要求，所以依次取下一个，还要多处理一个如果一台可以用的服务器都没有的情况
		multimap<int, int>::iterator mini = Matching_degree.begin();
		
		for (multimap<int, int>::iterator it = Matching_degree.begin(); it != Matching_degree.end(); ) {
			if (vmsOnserverTemp[it->second].size() == 0) {
				it++;
				// 实在没有服务器可以匹配了就跳出去，因为是匹配不完的
				if (it == Matching_degree.end()) {
					// for (int z = 0; z < requestinfosAdd.size(); ++z)
						// res_tmp.push_back("null");
					return totalRemoved;
				}
			}
			else {
				mini = it; 
				break;
			}
		}

		unordered_map<string, pair<string, vector<int> >>& vmInfos = vmsOnserverTemp[mini->second];
		auto& server = sysServerResource[mini->second];
		for (int i = 0; i < vmsOnserverTemp[mini->second].size(); ++i) {
			string requestType = requestinfosAdd[i][1], requestId = requestinfosAdd[i][2];
			if (AllocServerResources(server, VmInfos[requestType], mini->second, requestId, res_tmp, reqSrcIdx[requestId])) {
				serverRunVms[mini->second]++;
			}
			assert(server.CpuCores_A >= 0 && server.CpuCores_B >= 0 && server.MemerySize_A >= 0 && server.MemerySize_B>= 0);
		}

		// 同步删除requestinfos，这段有非常非常大的优化空间
		for (int i = 0; i < vmsOnserverTemp[mini->second].size(); ++i) {
			for (int j = 0; j < requestinfos.size(); ++j) {
				if (requestinfosAdd[i][2] == requestinfos[j][2]) {
					requestinfos.erase(requestinfos.begin() + j);
					break;
				}
			}
		}
		// 从requestinfos去除分配好的虚拟机		
		requestinfosAdd.erase(requestinfosAdd.begin(), requestinfosAdd.begin() + vmsOnserverTemp[mini->second].size());
		totalRemoved += vmsOnserverTemp[mini->second].size();
		
		Matching_degree.clear();
	}  // 一天所有请求分配结束
	return totalRemoved;
}


// 处理每一个add请求
int ServerIdAdd = 0; // 每天增加的服务器数量
int ReDealAddVM(multimap<string, unordered_map<string, vector<int>>> serverOnVm, vector<string> request, vector<string>& res_tmp, int day);
int ReDealAddVM(multimap<string, unordered_map<string, vector<int>>> serverOnVm, vector<string> request, vector<string>& res_tmp, int day) {
	// int OutserverId = 0;  // 服务器ID
	// string s = "(";
	// for (auto sov : serverOnVm) {
	// 	for (auto vm : sov.second) {
	// 		if (request[2] == vm.first) {   // 虚拟机Id匹配上
	// 			if (vm.second.size() > 3) {  // 双节点部署
	// 				s += to_string(OutserverId + ServerIdAdd) + ")\n";
	// 			}
	// 			else {
	// 				if (vm.second[2] == 1)
	// 					s += to_string(OutserverId + ServerIdAdd) + ", A)\n";		  // 分配到A节点			
	// 				else
	// 					s += to_string(OutserverId + ServerIdAdd) + ", B)\n";          // 分配到B节点	
	// 			}
	// 			if (day > 0) {
	// 				for (auto &tmp : res_tmp) {
	// 					if (tmp == "null") {
	// 						tmp = s; break;
	// 					}
	// 				}
	// 			}
	// 			else
	// 			{
	// 				res_tmp.push_back(s);
	// 			}
	// 			return 1;
	// 		}		
	// 	}
	// 	OutserverId++;
	// }	
	// return -1;  

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
			for (auto &tmp : res_tmp) {
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
void Migrate() {
	string s = "(migration, 0)\n";
#ifdef TEST
	cout << s;
#endif
	res.push_back(s);
}



// 分配
void Match(int day, unordered_map<string, ServerInfo> serverinfos, vector<vector<string>> requestinfos) {

	vector<string> res_tmp;

	auto Remingrequestinfos = requestinfos;

	if (day == 0)  // 第一天我们全买，后面也可以调整day的大小
	{
		Expansion(day, serverinfos, requestinfos);
		Migrate();
#ifdef TEST 
		//printf("There are %d requests waiting to matching !!\n", requestinfos.size());
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
		//printf("There are %d requests waiting to matching !!\n", requestinfos.size());
#endif

		// for (vector<vector<string>>::iterator req = requestinfos.begin(); req != requestinfos.end(); ) {
		// 	int opType = (*req).size() == 3 ? 1 : 0;
		// 	if (opType) {  // add
				
		// 		int resourceEnough = DealAddVm(*req, res_tmp);
		// 		if (resourceEnough != -1) {	
		// 			req = requestinfos.erase(req);
		// 		}	
		// 		else
		// 		{
		// 			req++;
		// 			res_tmp.push_back("null");
		// 		}
		// 	}
		// 	else {
		// 		DealDeleteVM(*req);
		// 		req = requestinfos.erase(req);
		// 	}		
		// }

		// 修改过的代码
		int totalRemoved = DealAddVm(requestinfos, res_tmp);

		Expansion(day, serverinfos, requestinfos);
		Migrate();	
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