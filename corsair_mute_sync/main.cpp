#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#ifdef __APPLE__
#include <CUESDK/CUESDK.h>
#else
#include <CUESDK.h>
#endif

#include "help.h"

using std::string, std::cout, std::endl, std::vector, std::optional, std::nullopt;

/// @author iain
class InputParser{
public:
    InputParser (int &argc, char **argv){
        for (int i=1; i < argc; ++i)
            this->tokens.push_back(string(argv[i]));
    }
    const string& get(const string &option) const{
        auto itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()){
            return *itr;
        }
        static const string empty_string("");
        return empty_string;
    }
    bool exists(const string &option) const{
        return std::find(this->tokens.begin(), this->tokens.end(), option)
            != this->tokens.end();
    }
private:
    vector<string> tokens;
};

const char *toString(const CorsairError error) {
    switch (error) {
    case CE_Success:
        return "CE_Success";
    case CE_ServerNotFound:
        return "CE_ServerNotFound";
    case CE_NoControl:
        return "CE_NoControl";
    case CE_ProtocolHandshakeMissing:
        return "CE_ProtocolHandshakeMissing";
    case CE_IncompatibleProtocol:
        return "CE_IncompatibleProtocol";
    case CE_InvalidArguments:
        return "CE_InvalidArguments";
    default:
        return "unknown error";
    }
}

bool errorCheck(const string &msg) {
    const auto error = CorsairGetLastError();
    if (error != CE_Success) {
        std::cerr << msg << " (Error: " << toString(error) << ')' << endl;
        return true;
    }

    return false;
}

optional<bool> tryGetMicEnabled(const int deviceIndex) {
    bool result;
    if (CorsairGetBoolPropertyValue(deviceIndex, CDPI_Headset_MicEnabled, &result)) {
        return result;
    }
    return nullopt;
}

vector<int> getDevices() {
    const auto devicesCount = CorsairGetDeviceCount();
    vector<int> supportedDevices;
    for (int i = 0; i < devicesCount; ++i) {
        if (tryGetMicEnabled(i)) {
            supportedDevices.push_back(i);
        }
    }
    return supportedDevices;
}

void listDevices(vector<int> devices) {
    for (int i : devices) {
        const auto info = CorsairGetDeviceInfo(i);
        if (!info) {
            errorCheck("Get device info error");
            continue;
        }
        cout << "ID: " << info->deviceId << ", Model: " << info->model << endl;
    }
}

int selectDeviceById(vector<int> devices, string id)
{
    for (int i : devices) {
        const auto info = CorsairGetDeviceInfo(i);
        if (!info) {
            continue;
        }
        if (id.compare(info->deviceId) == 0) {
            return i;
        }
    }
    return -1;
}

int runCommand(string cmd) {
    return system(cmd.c_str());
}

int main(int argc, char *argv[]) {
    const InputParser input(argc, argv);

    CorsairPerformProtocolHandshake();
    if (errorCheck("Handshake error")) {
        getchar();
        return -1;
    }

    const auto devices = getDevices();

    if (input.exists("-h") || input.exists("--help")) {
        cout << helpMessage << endl;
        return 0;
    }

    if (input.exists("-l")) {
        listDevices(devices);
        return 0;
    }

    int device = -1;
    if (devices.size() == 0) {
        cout << "No supported device detected" << endl;
    } else if (input.exists("-d")) {
        const string id = input.get("-d");
        device = selectDeviceById(devices, id);
        if (device == -1) {
            cout << "No supported device with id " << id << " found" << endl;
        }
    } else if (devices.size() == 1) {
        device = devices[0];
    } else {
        cout << "Found multiple devices:" << endl;
        listDevices(devices);
    }

    if (device == -1) {
        return 1;
    }

    string   muteCmd = "echo muted";
    string unmuteCmd = "echo unmuted";

    if (input.exists("--01")) {
        muteCmd = unmuteCmd = input.get("--01");
    } else {
        if (input.exists("-0")) {
            muteCmd = input.get("-0");
        }
        if (input.exists("-1")) {
            unmuteCmd = input.get("-1");
        }
    }

    int state = -1;
    if (input.exists("-c")) {
        const auto result = tryGetMicEnabled(device);
        if (!result) return 0;
        state = *result;
    }

    int pollInterval = 100;

    if (input.exists("-p")) {
        pollInterval = std::stoi(input.get("-p"));
    }

    while (true) {
        const auto result = tryGetMicEnabled(device);
        if (!result) break;
        if (*result != state) {
            state = *result;
            if (state) {
                runCommand(unmuteCmd);
            } else {
                runCommand(muteCmd);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(pollInterval));
    }

    return 0;
}
