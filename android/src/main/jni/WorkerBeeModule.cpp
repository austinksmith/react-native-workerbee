#include <jni.h>
#include <jsi/jsi.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <thread>
#include <unordered_map>
#include <iostream>

using namespace facebook::jsi;
using namespace facebook::jni;
using namespace std;

class WorkerBeeModule : public jni::HybridClass<WorkerBeeModule> {
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
    jni::global_ref<jobject> javaPart_;
    Runtime* runtime_;
    std::shared_ptr<CallInvoker> jsCallInvoker_;
    std::unordered_map<long, std::thread> workers_;
    long nextWorkerId_ = 1;

    explicit WorkerBeeModule(jni::alias_ref<jhybridobject> jThis)
        : javaPart_(jni::make_global(jThis)) {}

    static jni::local_ref<jhybriddata> initHybrid(
        jni::alias_ref<jhybridobject> jThis,
        jlong jsContext,
        jni::alias_ref<CallInvokerHolder::javaobject> jsCallInvokerHolder) {
        auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
        return makeCxxInstance(jThis, reinterpret_cast<Runtime*>(jsContext), jsCallInvoker);
    }

    void installJSIBindings() {
        // Install JSI bindings here
        auto workerObject = Object(*runtime_);
        auto createWorkerFunc = Function::createFromHostFunction(*runtime_,
            PropNameID::forAscii(*runtime_, "createWorker"),
            1,
            [this](Runtime& runtime, const Value& thisValue, const Value* arguments, size_t count) -> Value {
                string scriptURL = arguments[0].getString(runtime).utf8(runtime);
                return Value(createWorkerInternal(scriptURL));
            });
        workerObject.setProperty(*runtime_, "createWorker", createWorkerFunc);
        runtime_->global().setProperty(*runtime_, "WorkerBeeModule", workerObject);
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

    void postMessage(long workerId, const string& message) {
        if (workers_.find(workerId) != workers_.end()) {
            cout << "Message to worker " << workerId << ": " << message << endl;
            // TODO: Implement message passing logic
        }
    }

    void terminateWorker(long workerId) {
        if (workers_.find(workerId) != workers_.end()) {
            workers_[workerId].detach(); // Detach to allow it to exit cleanly
            workers_.erase(workerId);
            cout << "Worker " << workerId << " terminated." << endl;
        }
    }
};

// JNI function for loading the module
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
    return jni::initialize(vm, [] {
        WorkerBeeModule::registerNatives();
    });
}