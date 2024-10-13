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

class WorkerBeeJSI : public jni::HybridClass<WorkerBeeJSI> {
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
    jni::global_ref<jobject> javaPart_;
    Runtime* runtime_;
    std::shared_ptr<CallInvoker> jsCallInvoker_;
    std::unordered_map<long, std::thread> workers_;
    std::unordered_map<long, jni::global_ref<jobject>> workerRefs_; // To store worker references
    long nextWorkerId_ = 1;

    explicit WorkerBeeJSI(jni::alias_ref<jhybridobject> jThis)
        : javaPart_(jni::make_global(jThis)) {}

    static void installJSIBindings(jni::alias_ref<jhybridobject> jThis) {
        auto instance = getInstance(jThis);
        instance->installJSIBindingsInternal();
    }

    void installJSIBindingsInternal() {
        runtime_->global().setProperty(*runtime_, "WorkerBeeModule", Object(*runtime_));

        // Create the createWorker function
        auto createWorkerFunc = Function::createFromHostFunction(*runtime_,
            PropNameID::forAscii(*runtime_, "createWorker"),
            1,
            [this](Runtime& runtime, const Value& thisValue, const Value* arguments, size_t count) -> Value {
                string scriptURL = arguments[0].getString(runtime).utf8(runtime);
                return Value(createWorkerInternal(scriptURL));
            });

        runtime_->global().getProperty(*runtime_, "WorkerBeeModule").setProperty(*runtime_, "createWorker", createWorkerFunc);
    }

    long createWorkerInternal(const string& scriptURL) {
        long workerId = nextWorkerId_++;
        workers_[workerId] = std::thread([this, workerId, scriptURL]() {
            // Here you would implement your worker logic, e.g., executing the script
            cout << "Worker " << workerId << " started with script: " << scriptURL << endl;
            // Simulate doing work
            std::this_thread::sleep_for(std::chrono::seconds(5)); // Example delay
            cout << "Worker " << workerId << " finished." << endl;
        });
        return workerId;
    }

    void postMessage(long workerId, const string& message) {
        // This function should handle sending messages to a specific worker.
        if (workers_.find(workerId) != workers_.end()) {
            cout << "Message to worker " << workerId << ": " << message << endl;
            // TODO: Implement message passing logic here
        }
    }

    void terminateWorker(long workerId) {
        if (workers_.find(workerId) != workers_.end()) {
            // Here you would implement termination logic, e.g., stopping the thread.
            workers_[workerId].detach(); // Detach and allow it to exit cleanly
            workers_.erase(workerId); // Remove from map
            cout << "Worker " << workerId << " terminated." << endl;
        }
    }

public:
    static jni::local_ref<jhybriddata> initHybrid(
        jni::alias_ref<jhybridobject> jThis,
        jlong jsContext,
        jni::alias_ref<CallInvokerHolder::javaobject> jsCallInvokerHolder) {
        auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
        return makeCxxInstance(jThis, reinterpret_cast<Runtime*>(jsContext), jsCallInvoker);
    }
};

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
    return jni::initialize(vm, [] {
        WorkerBeeJSI::registerNatives();
    });
}
