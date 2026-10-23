#include "SceTypes.hpp"
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

extern "C" {
int APS5_VABI sceHttp2Init(int, int, std::size_t, int);
int APS5_VABI sceHttp2CreateTemplate(int, const char*, int, int);
int APS5_VABI sceHttp2CreateRequestWithURL(int, const char*, const char*, std::uint64_t);
int APS5_VABI sceHttp2DeleteRequest(int);
int APS5_VABI sceHttp2DeleteTemplate(int);
int APS5_VABI sceHttp2Term(int);
int APS5_VABI sceHttp2SetAuthEnabled(int, int);
int APS5_VABI sceHttp2GetAuthEnabled(int, int*);
int APS5_VABI sceHttp2SetAutoRedirect(int, int);
int APS5_VABI sceHttp2GetAutoRedirect(int, int*);
[[noreturn]] void NotImplemented_nid_no_patch(const char*) { throw std::runtime_error("not implemented"); }
int APS5_VABI EqueueTriggerEvent_nid_postfix(KernelEqueue, uintptr_t, int16_t, void*) { return -1; }
}

static void Require(bool value) {
    if (!value) std::abort();
}

int main() {
    constexpr int InvalidId = static_cast<int>(0x817B1100u);
    constexpr int NullPointer = static_cast<int>(0x817B1225u);
    const int context = sceHttp2Init(1, 1, 0x10000, 4);
    const int tmpl = sceHttp2CreateTemplate(context, "agent", 2, 0);
    const int originalRequest = sceHttp2CreateRequestWithURL(tmpl, "GET", "https://example.com/original", 0);
    int value = -1;

    Require(sceHttp2GetAuthEnabled(tmpl, &value) == 0 && value == 1);
    Require(sceHttp2SetAuthEnabled(tmpl, 0) == 0);
    Require(sceHttp2GetAuthEnabled(tmpl, &value) == 0 && value == 0);
    Require(sceHttp2GetAuthEnabled(originalRequest, &value) == 0 && value == 1);
    Require(sceHttp2SetAuthEnabled(originalRequest, -7) == 0);
    Require(sceHttp2GetAuthEnabled(originalRequest, &value) == 0 && value == 1);

    Require(sceHttp2GetAutoRedirect(tmpl, &value) == 0 && value == 1);
    Require(sceHttp2SetAutoRedirect(tmpl, 0) == 0);
    Require(sceHttp2GetAutoRedirect(tmpl, &value) == 0 && value == 0);
    Require(sceHttp2GetAutoRedirect(originalRequest, &value) == 0 && value == 1);
    Require(sceHttp2SetAutoRedirect(originalRequest, 9) == 0);
    Require(sceHttp2GetAutoRedirect(originalRequest, &value) == 0 && value == 1);

    const int inheritedRequest = sceHttp2CreateRequestWithURL(tmpl, "GET", "https://example.com/inherited", 0);
    Require(sceHttp2GetAuthEnabled(inheritedRequest, &value) == 0 && value == 0);
    Require(sceHttp2GetAutoRedirect(inheritedRequest, &value) == 0 && value == 0);

    Require(sceHttp2SetAuthEnabled(0x7fffffff, 1) == InvalidId);
    Require(sceHttp2SetAutoRedirect(0x7fffffff, 1) == InvalidId);
    Require(sceHttp2GetAuthEnabled(0x7fffffff, &value) == InvalidId);
    Require(sceHttp2GetAutoRedirect(0x7fffffff, &value) == InvalidId);
    Require(sceHttp2GetAuthEnabled(tmpl, nullptr) == NullPointer);
    Require(sceHttp2GetAutoRedirect(tmpl, nullptr) == NullPointer);

    Require(sceHttp2DeleteRequest(inheritedRequest) == 0);
    Require(sceHttp2DeleteRequest(originalRequest) == 0);
    Require(sceHttp2DeleteTemplate(tmpl) == 0);
    Require(sceHttp2Term(context) == 0);
}
