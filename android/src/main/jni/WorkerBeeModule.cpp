#include <jni.h>
#include <jsi/jsi.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <thread>
#include <unordered_map>
#include <iostream>
#include <fbjni/fbjni.h>

using namespace facebook::jsi;
using namespace facebook::jni;
using namespace std;

class WorkerBeeModule : public facebook::jni::HybridClass<WorkerBeeModule> {
public:
    static constexpr auto kJavaDescriptor = "Lcom/austinksmith/reactnativeworkerbee/WorkerBeeModule;";

    static void registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", WorkerBeeModule::initHybrid),
            makeNativeMethod("installJSIBindings", WorkerBeeModule::installJSIBindings),
            makeNativeMethod("createWorker", WorkerBeeModule::createWorker),
            makeNativeMethod("postMessage", WorkerBeeModule::postMessage),
            makeNativeMethod("terminateWorker", WorkerBeeModule::terminateWorker),
        });
    }

private:
    friend HybridBase;
    facebook::jni::global_ref<jobject> javaPart_;
    Runtime* runtime_;
    std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker_;
    std::unordered_map<long, std::thread> workers_;
    long nextWorkerId_ = 1;

    explicit WorkerBeeModule(facebook::jni::alias_ref<jhybridobject> jThis, Runtime* runtime, std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker)
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
            std::this_thread::sleep_for(std::chrono::seconds(5)); // Simulating work
            cout << "Worker " << workerId << " finished." << endl;
        });
        return workerId;
    }

    void createWorker(jstring scriptURL) {
        // Implementation
    }

    void postMessage(long workerId, const string& message) {
        if (workers_.find(workerId) != workers_.end()) {
            cout << "Message to worker " << workerId << ": " << message << endl;
            // TODO: Implement message passing logic
        }
    }

    void terminateWorker(long workerId) {
        if (workers_.find(workerId) != workers_.end()) {
            if (workers_[workerId].joinable()) {
                workers_[workerId].join(); // Ensure worker is finished before terminating
            }
            workers_.erase(workerId);
            cout << "Worker " << workerId << " terminated." << endl;
        }
    }
};

// Forward declaration of the function to register WorkerBeeJSI natives
void registerWorkerbeeJSINatives();

// JNI function for loading the module
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        WorkerBeeModule::registerNatives();
        registerWorkerbeeJSINatives(); // Register WorkerBeeJSI natives
    });
}