#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <algorithm>
#include <cassert>

using namespace std;

//#define TEST

#define UPLOAD

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
vector<int> serverRunVms;

// 记录虚拟机运行在那个服务器上
unordered_map<string, vector<int>> VmOnServer;   //[in] 虚拟机Id  [in] 虚拟机所在的服务器id,虚拟机cpu,虚拟机内存，单/双节点部署）

// 记录服务器上运行了哪些虚拟机
//unordered_map<int, vector<string>> ServerOnVm;   //[in] 服务器Id  [in] 服务器上（id,cpu,内存，单/双节点部署）

vector<string> res;

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
bool AllocServerResources(ServerInfo& server, VmInfo& vm, int serverId, string vmId) {

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
			cout << "(" << serverId << ")" << endl;
#endif
			res.push_back("(" + to_string(serverId) + ")\n");
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
		cout << "(" << serverId << ", A)" << endl;
#endif
		res.push_back("(" + to_string(serverId) + ", A)\n");
		return true;
	}
	else if (serverCoreB >= vmCores && serverMemoryB >= vmMemory) {    //单节点部署检查B节点是否满足分配条件

		serverCoreB -= vmCores;
		serverMemoryB -= vmMemory;

		// 记录虚拟机运行在哪个服务器上（B节点部署）
		VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,2 };

#ifdef TEST
		cout << "(" << serverId << ", B)" << endl;
#endif
		res.push_back("(" + to_string(serverId) + ", B)\n");
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

// 处理创建虚拟机操作
int DealAddVM(vector<string>& addVmInfo) {

	string _reqVmType = addVmInfo[1], _reqId = addVmInfo[2];
	VmInfo vm = VmInfos[_reqVmType];

	int success = -1;
	for (int i = 0; i < serverNumber; i++) {

		auto& server = sysServerResource[i];
		if (AllocServerResources(server, vm, i, _reqId)) {   // 将请求的虚拟机资源在现有服务器资源上分配
			serverRunVms[i]++;                               //  i:服务器Id, 服务器i上的运行的虚拟机+1
			success = 1;
			break;
		}
#ifdef TEST
		assert(server.CpuCores_A >= 0 && server.CpuCores_B >= 0 && server.MemerySize_A >= 0 && server.MemerySize_B >= 0);
#endif
	}
	return success;
}


// 处理删除虚拟机操作
void DealDeleteVM(vector<string>& delVmInfo) {

	string _vmId = delVmInfo[1];

	vector<int> _vmInfo = VmOnServer[_vmId];

	int _serverId = _vmInfo[0];  //部署虚拟机的服务器id

	auto& server = sysServerResource[_serverId];

	ReleaseServerResources(server, _vmInfo, _serverId, _vmId);   //释放虚拟机

	serverRunVms[_serverId]--;  //服务器上的虚拟机数量-1

}

// 初始化服务器购买数
void InitServer() {

	string serverType = "hostUY41I";
	int n = 2500;
	serverRunVms.resize(n, 0);
	string initBuy = "(purchase, ";
	initBuy += to_string(2) + ")\n";

	res.push_back(initBuy);

	string pauseInfo = "(" + serverType + ", ";
	pauseInfo += std::to_string(n / 2) + ")\n";

	res.push_back(pauseInfo);

	for (int i = 0; i < n / 2; i++) {
		sysServerResource[serverNumber++] = ServerInfos[serverType];
		SERVERCOST += ServerInfos[serverType].ServerCost;

	}
	serverType = "host78BMY";
	pauseInfo = "(" + serverType + ", ";
	pauseInfo += std::to_string(serverNumber) + ")\n";

	res.push_back(pauseInfo);

	for (int i = 0; i < n / 2; i++) {
		sysServerResource[serverNumber++] = ServerInfos[serverType];
		SERVERCOST += ServerInfos[serverType].ServerCost;
	}

}


// 扩容服务器
void Expansion() {
	string s = "(purchase, 0)\n";
#ifdef TEST
	cout << s << endl;
#endif
	res.push_back(s);
}

// 迁移虚拟机
void Migrate() {
	string s = "(migration, 0)\n";
#ifdef TEST
	cout << s << endl;
#endif
	res.push_back(s);
}

// 分配
void Match(int day) {
	if (day != 0) Expansion();
	Migrate();
#ifdef TEST 
	printf("There are %d requests waiting to matching !!\n", RequestInfos.size());
#endif
	for (auto req : RequestInfos) {
		// 
		int opType = req.size() == 3 ? 1 : 0;
		if (opType) {
			int resourceEnough = DealAddVM(req);
			assert(resourceEnough != -1);
		}
		else {
			// 
			DealDeleteVM(req);
		}
	}
}

// 服务器电费计算
void ServerPowerCost() {
	for (int i = 0; i < serverNumber; i++) {
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
	InitServer(); //初始化购买服务器资源

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
			printf("The %d day begin matching!!!\n", day + 1);
		}
#endif
		Match(day);
		ServerPowerCost();
		//        break;
	}

	fclose(stdin);
	finish = clock();

	TOTALCOST = SERVERCOST + POWERCOST;  //计算总费用

#ifdef UPLOAD
	for (auto& s : res) std::cout << s;
#endif
#ifdef TEST
	printf("\nusr time: %f s \n", double(finish - start) / CLOCKS_PER_SEC);
	printf("server cost: %lld \npower cost: %lld \ntotal cost: %lld \n", SERVERCOST, POWERCOST, TOTALCOST);
#endif

	return 0;
}