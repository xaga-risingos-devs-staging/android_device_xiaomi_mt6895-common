#include <iostream>
#include <fstream>
#include <sys/system_properties.h>
#include <android/log.h>
#include <cstring>

#define LOG_TAG "xagaperformanced"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

const char* prop_name = "persist.sys.xaga_performance_mode";
const char* cpu_governor_paths[] = {
    "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor",
    "/sys/devices/system/cpu/cpu4/cpufreq/scaling_governor",
    "/sys/devices/system/cpu/cpu7/cpufreq/scaling_governor"
};
const char* ON_VALUE = "1";
const char* GOVERNOR_PERFORMANCE = "performance\n";
const char* GOVERNOR_SCHEDUTIL = "schedutil\n";

int main() {
    LOGI("Starting xagaperformanced");

    char value[PROP_VALUE_MAX] = {0};
    int ret = __system_property_get(prop_name, value);

    if (ret <= 0) {
        LOGE("Could not read property: %s", prop_name);
        std::cerr << "Could not read property: " << prop_name << std::endl;
        LOGI("Exiting xagaperformanced");
        return 1;
    }

    bool perf_mode = (std::string(value) == ON_VALUE);
    const char* governor = perf_mode ? GOVERNOR_PERFORMANCE : GOVERNOR_SCHEDUTIL;

    LOGI("Performance mode: %s", perf_mode ? "ON" : "OFF");
    std::cout << "Performance mode: " << (perf_mode ? "ON" : "OFF") << std::endl;

    for (const char* path : cpu_governor_paths) {
        LOGI("Setting %s to %s", path, perf_mode ? "performance" : "schedutil");
        std::ofstream file(path);
        if (!file) {
            LOGE("Failed to open: %s (%s)", path, strerror(errno));
            std::cerr << "Failed to open: " << path << std::endl;
            perror("Reason");
            continue;
        }
        file << governor;
        if (!file) {
            LOGE("Failed to write to: %s (%s)", path, strerror(errno));
            std::cerr << "Failed to write to: " << path << std::endl;
        } else {
            LOGI("Set %s to %s successfully", path, perf_mode ? "performance" : "schedutil");
            std::cout << "Set " << path << " to " << (perf_mode ? "performance" : "schedutil") << std::endl;
        }
    }

    LOGI("Exiting xagaperformanced");
    return 0;
}
