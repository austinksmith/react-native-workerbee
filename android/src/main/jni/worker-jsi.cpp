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
    std::shared_ptr<CallInvoker> jsCallInvoker_;
    std::unordered_map<long, std::thread> workers_;
    long nextWorkerId_ = 1;

    explicit WorkerBeeJSI(facebook::jni::alias_ref<jhybridobject> jThis, Runtime* runtime, std::shared_ptr<CallInvoker> jsCallInvoker)
        : javaPart_(facebook::jni::make_global(jThis)), runtime_(runtime), jsCallInvoker_(jsCallInvoker) {}

    static facebook::jni::local_ref<jhybriddata> initHybrid(
        facebook::jni::alias_ref<jhybridobject> jThis,
        jlong jsContext,
        facebook::jni::alias_ref<CallInvokerHolder::javaobject> jsCallInvokerHolder) {
        
        auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
        return makeCxxInstance(jThis, reinterpret_cast<Runtime*>(jsContext), jsCallInvoker);
    }

    void installJSIBindings() {
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
            if (workers_[workerId].joinable()) {
                workers_[workerId].join(); // Ensure worker is finished before terminating
            }
            workers_.erase(workerId); // Remove from map
            cout << "Worker " << workerId << " terminated." << endl;
        }
    }
};

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        WorkerBeeJSI::registerNatives();
    });
}
