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

// �ṹ��洢������Ϣ

// ������������Ϣ�ṹ��
struct ServerInfo {

	int CpuCores_A;    // A�ڵ�cpu��Դ��
	int CpuCores_B;    // B�ڵ�cpu��Դ��

	int MemerySize_A;  // A�ڵ��ڴ���Դ��
	int MemerySize_B;  // B�ڵ��ڴ���Դ��

	int ServerCost;    // ����������
	int PowerCost;     // ���

	int ServerId;      // ������Id
};

// �����������Ϣ�ṹ��
struct VmInfo {

	int CpuCores;    // ���ĵ�cpu��Դ��

	int MemerySize;  // ���ĵ��ڴ���Դ��

	int VmTwoNodes;  // ��/˫�ڵ㲿��

	string VmId;     // �����Id
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
vector<int> serverRunVms;

// ��¼������������Ǹ���������
unordered_map<string, vector<int>> VmOnServer;   //[in] �����Id  [in] ��������ڵķ�����id,�����cpu,������ڴ棬��/˫�ڵ㲿��

// ��¼����������������Щ�����
//unordered_map<int, vector<string>> ServerOnVm;   //[in] ������Id  [in] �������ϣ�id,cpu,�ڴ棬��/˫�ڵ㲿��

vector<string> res;

#ifdef TEST
const string filePath = "training-1.txt";
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

// �����û��������
void GetRequest(string& op, string& reqVmType, string& reqId) {

	string _op, _reqVmType, _reqId;
	_op = op.substr(1, op.size() - 1);                                 // add
	_reqVmType = reqVmType.substr(0, reqVmType.size() - 1);            // ������ӵ����������
	_reqId = reqId.substr(0, reqId.size() - 1);                        // ������ӵ������id
	RequestInfos.push_back(vector<string>{_op, _reqVmType, _reqId});   // �û������������û�������Ϣ��
}

// �����û�ɾ������
void GetRequest(string& op, string& reqId) {
	string _op, _reqId;
	_reqId = reqId.substr(0, reqId.size() - 1);                        // ����ɾ���������id
	_op = op.substr(1, op.size() - 1);                                 // delete
	RequestInfos.push_back(vector<string>{_op, _reqId});               // �û�ɾ���������������û�������Ϣ��
}


// �ڷ������Ϸ����������Դ
bool AllocServerResources(ServerInfo& server, VmInfo& vm, int serverId, string vmId) {

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

			// ��¼������������ĸ��������ϣ�˫�ڵ㲿��
			VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1,2 };
#ifdef TEST
			cout << "(" << serverId << ")" << endl;
#endif
			res.push_back("(" + to_string(serverId) + ")\n");
			return true;
		}
		else {
			return false;                                               //��Դ���㣬�ڴ˷������Ϸ���������ʧ��
		}
	}
	else if (serverCoreA >= vmCores && serverMemoryA >= vmMemory) {     //���ڵ㲿����A�ڵ��Ƿ������������

		serverCoreA -= vmCores;
		serverMemoryA -= vmMemory;

		// ��¼������������ĸ��������ϣ�A�ڵ㲿��
		VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,1 };

#ifdef TEST
		cout << "(" << serverId << ", A)" << endl;
#endif
		res.push_back("(" + to_string(serverId) + ", A)\n");
		return true;
	}
	else if (serverCoreB >= vmCores && serverMemoryB >= vmMemory) {    //���ڵ㲿����B�ڵ��Ƿ������������

		serverCoreB -= vmCores;
		serverMemoryB -= vmMemory;

		// ��¼������������ĸ��������ϣ�B�ڵ㲿��
		VmOnServer[vmId] = vector<int>{ serverId,vmCores,vmMemory,2 };

#ifdef TEST
		cout << "(" << serverId << ", B)" << endl;
#endif
		res.push_back("(" + to_string(serverId) + ", B)\n");
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

// ���������������
int DealAddVM(vector<string>& addVmInfo) {

	string _reqVmType = addVmInfo[1], _reqId = addVmInfo[2];
	VmInfo vm = VmInfos[_reqVmType];

	int success = -1;
	for (int i = 0; i < serverNumber; i++) {

		auto& server = sysServerResource[i];
		if (AllocServerResources(server, vm, i, _reqId)) {   // ��������������Դ�����з�������Դ�Ϸ���
			serverRunVms[i]++;                               //  i:������Id, ������i�ϵ����е������+1
			success = 1;
			break;
		}
#ifdef TEST
		assert(server.CpuCores_A >= 0 && server.CpuCores_B >= 0 && server.MemerySize_A >= 0 && server.MemerySize_B >= 0);
#endif
	}
	return success;
}


// ����ɾ�����������
void DealDeleteVM(vector<string>& delVmInfo) {

	string _vmId = delVmInfo[1];

	vector<int> _vmInfo = VmOnServer[_vmId];

	int _serverId = _vmInfo[0];  //����������ķ�����id

	auto& server = sysServerResource[_serverId];

	ReleaseServerResources(server, _vmInfo, _serverId, _vmId);   //�ͷ������

	serverRunVms[_serverId]--;  //�������ϵ����������-1

}

// ��ʼ��������������
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


// ���ݷ�����
void Expansion() {
	string s = "(purchase, 0)\n";
#ifdef TEST
	cout << s << endl;
#endif
	res.push_back(s);
}

// Ǩ�������
void Migrate() {
	string s = "(migration, 0)\n";
#ifdef TEST
	cout << s << endl;
#endif
	res.push_back(s);
}

// ����
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

// ��������Ѽ���
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
	InitServer(); //��ʼ�������������Դ

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
			printf("The %d day begin matching!!!\n", day + 1);
		}
#endif
		Match(day);
		ServerPowerCost();
		//        break;
	}

	fclose(stdin);
	finish = clock();

	TOTALCOST = SERVERCOST + POWERCOST;  //�����ܷ���

#ifdef UPLOAD
	for (auto& s : res) std::cout << s;
#endif
#ifdef TEST
	printf("\nusr time: %f s \n", double(finish - start) / CLOCKS_PER_SEC);
	printf("server cost: %lld \npower cost: %lld \ntotal cost: %lld \n", SERVERCOST, POWERCOST, TOTALCOST);
#endif

	return 0;
}