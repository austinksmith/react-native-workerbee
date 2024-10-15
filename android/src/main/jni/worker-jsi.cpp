#include <jni.h>
#include <jsi/jsi.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <fbjni/fbjni.h>
#include <thread>
#include <unordered_map>
#include <iostream>

using namespace facebook::jsi;
using namespace facebook::jni;
using namespace std;

class WorkerBeeJSI : public facebook::jni::HybridClass<WorkerBeeJSI> {
public:
    static constexpr auto kJavaDescriptor = "Lcom/austinksmith/reactnativeworkerbee/WorkerBeeModule;";

    static void registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", WorkerBeeJSI::initHybrid),
            makeNativeMethod("installJSIBindings", WorkerBeeJSI::installJSIBindings),
            makeNativeMethod("createWorker", WorkerBeeJSI::createWorker),
            makeNativeMethod("postMessage", WorkerBeeJSI::postMessage),
            makeNativeMethod("terminateWorker", WorkerBeeJSI::terminateWorker),
        });
    }

private:
    friend HybridBase;
    facebook::jni::global_ref<jobject> javaPart_;
    Runtime* runtime_;
    std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker_;
    std::unordered_map<long, std::thread> workers_;
    long nextWorkerId_ = 1;

    explicit WorkerBeeJSI(facebook::jni::alias_ref<jhybridobject> jThis, Runtime* runtime, std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker)
        : javaPart_(facebook::jni::make_global(jThis)), runtime_(runtime), jsCallInvoker_(jsCallInvoker) {}

    static facebook::jni::local_ref<jhybriddata> initHybrid(
        facebook::jni::alias_ref<jhybridobject> jThis,
        jlong jsContext,
        facebook::jni::alias_ref<facebook::react::CallInvokerHolder::javaobject> jsCallInvokerHolder) {
        
        auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
        return makeCxxInstance(jThis, reinterpret_cast<Runtime*>(jsContext), jsCallInvoker);
    }

    void installJSIBindings() {
        auto workerObject = Object(*runtime_);

        auto createWorkerFunc = Function::createFromHostFunction(*runtime_,
            PropNameID::forAscii(*runtime_, "createWorker"),
            1,
            [this](Runtime& runtime, const Value& thisValue, const Value* arguments, size_t count) -> Value {
                string scriptURL = arguments[0].getString(runtime).utf8(runtime);
                return Value((double)createWorkerInternal(scriptURL));
            });

        workerObject.setProperty(*runtime_, "createWorker", std::move(createWorkerFunc));
        runtime_->global().setProperty(*runtime_, "WorkerBeeModule", std::move(workerObject));
    }

    long createWorkerInternal(const string& scriptURL) {
        long workerId = nextWorkerId_++;
        workers_[workerId] = std::thread([this, workerId, scriptURL]() {
            cout << "Worker " << workerId << " started with script: " << scriptURL << endl;
            std::this_thread::sleep_for(std::chrono::seconds(5)); // Example delay
            cout << "Worker " << workerId << " finished." << endl;
        });
        return workerId;
    }

    void createWorker(jstring scriptURL) {
        // Implementation
    }

    void postMessage(jlong workerId, jstring message) {
        // Implementation
    }

    void terminateWorker(jlong workerId) {
        // Implementation
    }
};

// Function to register WorkerBeeJSI natives, called from JNI_OnLoad in WorkerBeeModule.cpp
extern "C" void registerWorkerbeeJSINatives() {
    WorkerBeeJSI::registerNatives();
}